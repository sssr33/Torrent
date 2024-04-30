#pragma once
#include "Dx11Include.h"

class Dx11HwndSwapChain {
public:
    Dx11HwndSwapChain(
        HWND hwnd,
        const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
        const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext,
        DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM);

    void Resize(
        const DirectX::XMUINT2& size,
        const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
        const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& d3dContext,
        const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext);

private:
    static constexpr auto MinSize = DirectX::XMUINT2(1, 1);
    static constexpr auto Dpi = 96.f;
    static constexpr auto BufferCount = 2u; // Use double-buffering to minimize latency.

    void CreateRenderTargets(
        const Microsoft::WRL::ComPtr<ID3D11Device>& d3dDevice,
        const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& d2dContext);

    static DirectX::XMUINT2 GetSize(HWND hwnd);
    static DirectX::XMUINT2 GetClampedSize(const DirectX::XMUINT2& size);

    Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3dRenderTargetView;
    Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2dTargetBitmap;
    D3D11_VIEWPORT screenViewport = {};
    DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
};
