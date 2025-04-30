#ifndef CAPTURE_DXGI_H
#define CAPTURE_DXGI_H

#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;

class ScreenCapturer {
public:
    ScreenCapturer();
    ~ScreenCapturer();

    bool Initialize();
    bool CaptureFrame(ID3D11Texture2D** frame);

private:
    ComPtr<ID3D11Device> device_;
    ComPtr<ID3D11DeviceContext> context_;
    ComPtr<IDXGIOutputDuplication> duplication_;
    DXGI_OUTDUPL_DESC outputDesc_;
};

#endif // CAPTURE_DXGI_H
