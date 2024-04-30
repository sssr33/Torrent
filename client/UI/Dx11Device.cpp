#include "Dx11Device.h"

#include <Helpers/HResultException.h>

Dx11Device::Dx11Device() {
	this->CreateDeviceIndependentResources();
	this->CreateDeviceResources();
}

const Microsoft::WRL::ComPtr<ID3D11Device>& Dx11Device::GetD3DDevice() const {
	return this->d3dDevice;
}

const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Dx11Device::GetD3DContext() const {
	return this->d3dContext;
}

D3D_FEATURE_LEVEL Dx11Device::GetD3DFeatureLevel() const {
	return this->d3dFeatureLevel;
}

const Microsoft::WRL::ComPtr<ID2D1Factory1>& Dx11Device::GetD2DFactory() const {
	return this->d2dFactory;
}

const Microsoft::WRL::ComPtr<ID2D1Device>& Dx11Device::GetD2DDevice() const {
	return this->d2dDevice;
}

const Microsoft::WRL::ComPtr<ID2D1DeviceContext>& Dx11Device::GetD2DContext() const {
	return this->d2dContext;
}

const Microsoft::WRL::ComPtr<IDWriteFactory>& Dx11Device::GetDwriteFactory() const {
	return this->dwriteFactory;
}

void Dx11Device::CreateDeviceIndependentResources() {
	HRESULT hr = S_OK;
	D2D1_FACTORY_OPTIONS options = {};

#if defined(_DEBUG)
	// If the project is in a debug build, enable Direct2D debugging via SDK Layers.
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_MULTI_THREADED,
		__uuidof(this->d2dFactory.Get()),
		&options,
		&this->d2dFactory
	);
	Helpers::ThrowIfFailed(hr);

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(this->dwriteFactory.Get()),
		&this->dwriteFactory
	);
	Helpers::ThrowIfFailed(hr);
}

void Dx11Device::CreateDeviceResources(IDXGIAdapter* adapter /*= nullptr*/) {
	// This flag adds support for surfaces with a different color channel ordering
	// than the API default. It is required for compatibility with Direct2D.
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (Dx11Device::SdkLayersAvailable()) {
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
#endif

	// This array defines the set of DirectX hardware feature levels this app will support.
	// Note the ordering should be preserved.
	// Don't forget to declare your application's minimum required feature level in its
	// description.  All applications are assumed to support 9.1 unless otherwise stated.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	HRESULT hr = S_OK;

	// Create the Direct3D 11 API device object and a corresponding context.
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_9_1;

	hr = D3D11CreateDevice(
		adapter,					// Specify nullptr to use the default adapter.
		adapter ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,	// Create a device using the hardware graphics driver.
		0,							// Should be 0 unless the driver is D3D_DRIVER_TYPE_SOFTWARE.
		creationFlags,				// Set debug and Direct2D compatibility flags.
		featureLevels,				// List of feature levels this app can support.
		ARRAYSIZE(featureLevels),	// Size of the list above.
		D3D11_SDK_VERSION,			// Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		&device,					// Returns the Direct3D device created.
		&featureLevel,			// Returns feature level of device created.
		&context					// Returns the device immediate context.
	);

	if (FAILED(hr)) {
		// If the initialization fails, fall back to the WARP device.
		// For more information on WARP, see: 
		// https://go.microsoft.com/fwlink/?LinkId=286690
		hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP, // Create a WARP device instead of a hardware device.
			0,
			creationFlags,
			featureLevels,
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION,
			&device,
			&featureLevel,
			&context
		);
		Helpers::ThrowIfFailed(hr);
	}

	hr = device.As(&this->d3dDevice);
	Helpers::ThrowIfFailed(hr);

	hr = context.As(&this->d3dContext);
	Helpers::ThrowIfFailed(hr);

	this->d3dFeatureLevel = featureLevel;

	this->CreateD2DeviceResources();
}

void Dx11Device::CreateD2DeviceResources() {
	HRESULT hr = S_OK;
	Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;

	hr = this->d3dDevice.As(&dxgiDevice);
	Helpers::ThrowIfFailed(hr);

	hr = this->d2dFactory->CreateDevice(dxgiDevice.Get(), &this->d2dDevice);
	Helpers::ThrowIfFailed(hr);

	hr = this->d2dDevice->CreateDeviceContext(
		D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
		&this->d2dContext
	);
	Helpers::ThrowIfFailed(hr);
}

#if defined(_DEBUG)
bool Dx11Device::SdkLayersAvailable() {
	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_NULL,       // There is no need to create a real hardware device.
		0,
		D3D11_CREATE_DEVICE_DEBUG,  // Check for the SDK layers.
		nullptr,                    // Any feature level will do.
		0,
		D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Microsoft Store apps.
		nullptr,                    // No need to keep the D3D device reference.
		nullptr,                    // No need to know the feature level.
		nullptr                     // No need to keep the D3D device context reference.
	);

	return SUCCEEDED(hr);
}
#endif
