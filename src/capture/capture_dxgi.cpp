#include "capture_dxgi.h"
#include <iostream>

ScreenCapturer::ScreenCapturer() = default;

ScreenCapturer::~ScreenCapturer() = default;

bool ScreenCapturer::Initialize() {
    HRESULT hr;

    // Create D3D11 device
    D3D_FEATURE_LEVEL featureLevel;
    UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    hr = D3D11CreateDevice(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        flags, nullptr, 0, D3D11_SDK_VERSION,
        &device_, &featureLevel, &context_);

    if (FAILED(hr)) {
        std::cerr << "Failed to create D3D11 device.\n";
        return false;
    }

    // Get DXGI device
    ComPtr<IDXGIDevice> dxgiDevice;
    hr = device_.As(&dxgiDevice);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIAdapter> dxgiAdapter;
    hr = dxgiDevice->GetAdapter(&dxgiAdapter);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIOutput> dxgiOutput;
    hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
    if (FAILED(hr)) return false;

    ComPtr<IDXGIOutput1> dxgiOutput1;
    hr = dxgiOutput.As(&dxgiOutput1);
    if (FAILED(hr)) return false;

    hr = dxgiOutput1->DuplicateOutput(device_.Get(), &duplication_);
    if (FAILED(hr)) {
        std::cerr << "Failed to create output duplication.\n";
        return false;
    }

    duplication_->GetDesc(&outputDesc_);
    return true;
}

bool ScreenCapturer::CaptureFrame(ID3D11Texture2D** frame) {
    ComPtr<IDXGIResource> desktopResource;
    DXGI_OUTDUPL_FRAME_INFO frameInfo = {};

    HRESULT hr = duplication_->AcquireNextFrame(100, &frameInfo, &desktopResource);
    if (FAILED(hr)) {
        if (hr == DXGI_ERROR_WAIT_TIMEOUT) {
            return false; // No new frame
        }
        std::cerr << "Failed to acquire next frame.\n";
        return false;
    }

    ComPtr<ID3D11Texture2D> texture;
    hr = desktopResource.As(&texture);
    *frame = texture.Detach();  // Optional: if your function expects raw pointer
    
    if (FAILED(hr)) {
        duplication_->ReleaseFrame();
        std::cerr << "Failed to cast IDXGIResource to ID3D11Texture2D.\n";
        return false;
    }

    duplication_->ReleaseFrame();
    return true;
}
