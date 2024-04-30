#pragma once
#include "Dx11Include.h"

// maybe need to use https://learn.microsoft.com/en-us/windows/win32/api/d3d10/nn-d3d10-id3d10multithread
// https://codereview.chromium.org/1645333003/
class Dx11Device {
public:
    Dx11Device();

    const Microsoft::WRL::ComPtr<ID3D11Device>& GetD3DDevice() const;
    const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetD3DContext() const;
    D3D_FEATURE_LEVEL GetD3DFeatureLevel() const;

    const Microsoft::WRL::ComPtr<ID2D1Factory1>& GetD2DFactory() const;
    const Microsoft::WRL::ComPtr<ID2D1Device>& GetD2DDevice() const;
    const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& GetD2DContext() const;

    const Microsoft::WRL::ComPtr<IDWriteFactory>& GetDwriteFactory() const;

private:
    void CreateDeviceIndependentResources();
    void CreateDeviceResources(IDXGIAdapter* adapter = nullptr);
    void CreateD2DeviceResources();

#if defined(_DEBUG)
    static bool SdkLayersAvailable();
#endif

    Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> d3dContext;
    D3D_FEATURE_LEVEL d3dFeatureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1;

    Microsoft::WRL::ComPtr<ID2D1Factory1> d2dFactory;
    Microsoft::WRL::ComPtr<ID2D1Device> d2dDevice;
    Microsoft::WRL::ComPtr<ID2D1DeviceContext> d2dContext;

    Microsoft::WRL::ComPtr<IDWriteFactory> dwriteFactory;
};
