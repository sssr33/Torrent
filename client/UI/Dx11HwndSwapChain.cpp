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
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Microsoft Store apps must use _FLIP_ SwapEffects.
    swapChainDesc.Flags = 0;
    swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
    swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;

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
    // CreateSwapChainForComposition works with DXGI_ALPHA_MODE_PREMULTIPLIED and needs DCompositionCreateDevice
    hr = dxgiFactory->CreateSwapChainForComposition(d3dDevice.Get(), &swapChainDesc, nullptr, &dxgiSwapChain);
    Helpers::ThrowIfFailed(hr);

    hr = dxgiSwapChain.As(&this->swapChain);
    Helpers::ThrowIfFailed(hr);

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
    // ensures that the application will only render after each VSync, minimizing power consumption.
    hr = dxgiDevice->SetMaximumFrameLatency(1);
    Helpers::ThrowIfFailed(hr);

    this->CreateRenderTargets(d3dDevice, d2dContext);

    // Bind our swap chain to the window.
    hr = DCompositionCreateDevice(dxgiDevice.Get(), IID_PPV_ARGS(&dcomp));
    Helpers::ThrowIfFailed(hr);

    hr = dcomp->CreateTargetForHwnd(hwnd, FALSE, &target);
    Helpers::ThrowIfFailed(hr);

    hr = dcomp->CreateVisual(&visual);
    Helpers::ThrowIfFailed(hr);

    hr = target->SetRoot(visual.Get());
    Helpers::ThrowIfFailed(hr);

    hr = visual->SetContent(this->swapChain.Get());
    Helpers::ThrowIfFailed(hr);

    hr = dcomp->Commit();
    Helpers::ThrowIfFailed(hr);
}

const Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& Dx11HwndSwapChain::GetD3DRenderTargetView() const {
    return this->d3dRenderTargetView;
}

const Microsoft::WRL::ComPtr<ID2D1Bitmap1>& Dx11HwndSwapChain::GetD2DRenderTargetBitmap() const {
    return this->d2dTargetBitmap;
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

void Dx11HwndSwapChain::Present(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext) {
    HRESULT hr = S_OK;
    // The first argument instructs DXGI to block until VSync, putting the application
    // to sleep until the next VSync. This ensures we don't waste any cycles rendering
    // frames that will never be displayed to the screen.
    DXGI_PRESENT_PARAMETERS parameters = { 0 };

    hr = this->swapChain->Present1(1, 0, &parameters);
    Helpers::ThrowIfFailed(hr);

    /*hr = this->swapChain->Present(0, DXGI_PRESENT_RESTART);
    Helpers::ThrowIfFailed(hr);

    hr = this->swapChain->Present(1, DXGI_PRESENT_DO_NOT_SEQUENCE);
    Helpers::ThrowIfFailed(hr);*/

    Microsoft::WRL::ComPtr<ID3D11DeviceContext1> d3dContext1;

    hr = d3dContext.As(&d3dContext1);
    Helpers::ThrowIfFailed(hr);

    d3dContext1->DiscardView(this->d3dRenderTargetView.Get());
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
