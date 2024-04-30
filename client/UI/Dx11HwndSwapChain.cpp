#include "Dx11HwndSwapChain.h"

#include <Helpers/HResultException.h>
#include <algorithm>

Dx11HwndSwapChain::Dx11HwndSwapChain(
    HWND hwnd,
    const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext,
    DXGI_FORMAT format /*= DXGI_FORMAT_B8G8R8A8_UNORM*/)
    : format(format)
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    auto windowSize = Dx11HwndSwapChain::GetSize(hwnd);

    swapChainDesc.Width = windowSize.x;
    swapChainDesc.Height = windowSize.y;
    swapChainDesc.Format = this->format;
    swapChainDesc.Stereo = FALSE;
    // Don't use multi-sampling
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = Dx11HwndSwapChain::BufferCount;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;// DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Microsoft Store apps must use _FLIP_ SwapEffects.
    swapChainDesc.Flags = 0;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

    HRESULT hr = S_OK;
    Microsoft::WRL::ComPtr<IDXGIDevice3> dxgiDevice;
    hr = d3dDevice->QueryInterface(dxgiDevice.GetAddressOf());
    Helpers::ThrowIfFailed(hr);

    Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    Helpers::ThrowIfFailed(hr);

    Microsoft::WRL::ComPtr<IDXGIFactory4> dxgiFactory;
    hr = dxgiAdapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
    Helpers::ThrowIfFailed(hr);

    Microsoft::WRL::ComPtr<IDXGISwapChain1> dxgiSwapChain;
    hr = dxgiFactory->CreateSwapChainForHwnd(d3dDevice.Get(), hwnd, &swapChainDesc, nullptr, nullptr, &dxgiSwapChain);
    Helpers::ThrowIfFailed(hr);

    hr = dxgiSwapChain.As(&this->swapChain);
    Helpers::ThrowIfFailed(hr);

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
    // ensures that the application will only render after each VSync, minimizing power consumption.
    hr = dxgiDevice->SetMaximumFrameLatency(1);
    Helpers::ThrowIfFailed(hr);

    this->CreateRenderTargets(d3dDevice, d2dContext);
}

void Dx11HwndSwapChain::Resize(
    const DirectX::XMUINT2& size,
    const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
    const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext,
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext)
{
    HRESULT hr = S_OK;
    auto clampedSize = Dx11HwndSwapChain::GetClampedSize(size);

    this->d3dRenderTargetView = nullptr;
    this->d2dTargetBitmap = nullptr;

    // Clear the previous window size specific context.
    ID3D11RenderTargetView* nullViews[] = { nullptr };
    d3dContext->OMSetRenderTargets(ARRAYSIZE(nullViews), nullViews, nullptr);
    d2dContext->SetTarget(nullptr);

    // prevents memory leak in ResizeBuffers
    d3dContext->Flush();

    hr = this->swapChain->ResizeBuffers(
        Dx11HwndSwapChain::BufferCount,
        clampedSize.x,
        clampedSize.y,
        this->format,
        0
    );
    Helpers::ThrowIfFailed(hr);

    this->CreateRenderTargets(d3dDevice, d2dContext);
}

void Dx11HwndSwapChain::CreateRenderTargets(
    const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext)
{
    HRESULT hr = S_OK;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    hr = this->swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    Helpers::ThrowIfFailed(hr);

    hr = d3dDevice->CreateRenderTargetView(
        backBuffer.Get(),
        nullptr,
        &this->d3dRenderTargetView
    );
    Helpers::ThrowIfFailed(hr);

    D3D11_TEXTURE2D_DESC backBufferDesc;

    backBuffer->GetDesc(&backBufferDesc);

    this->screenViewport = CD3D11_VIEWPORT(
        0.0f,
        0.0f,
        static_cast<float>(backBufferDesc.Width),
        static_cast<float>(backBufferDesc.Height)
    );

    D2D1_BITMAP_PROPERTIES1 bitmapProperties =
        D2D1::BitmapProperties1(
            D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(backBufferDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED),
            Dx11HwndSwapChain::Dpi,
            Dx11HwndSwapChain::Dpi
        );

    Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
    hr = backBuffer.As(&dxgiBackBuffer);
    Helpers::ThrowIfFailed(hr);

    hr = d2dContext->CreateBitmapFromDxgiSurface(
        dxgiBackBuffer.Get(),
        &bitmapProperties,
        &this->d2dTargetBitmap
    );
    Helpers::ThrowIfFailed(hr);
}

DirectX::XMUINT2 Dx11HwndSwapChain::GetSize(HWND hwnd) {
    RECT rect = {};

    if (!GetClientRect(hwnd, &rect)) {
        return Dx11HwndSwapChain::MinSize;
    }

    return Dx11HwndSwapChain::GetClampedSize(
        DirectX::XMUINT2(
            static_cast<uint32_t>(std::abs(rect.right - rect.left)),
            static_cast<uint32_t>(std::abs(rect.bottom - rect.top))
        ));
}

DirectX::XMUINT2 Dx11HwndSwapChain::GetClampedSize(const DirectX::XMUINT2& size) {
    return DirectX::XMUINT2(
        (std::max)(Dx11HwndSwapChain::MinSize.x, size.x),
        (std::max)(Dx11HwndSwapChain::MinSize.y, size.y)
    );
}
