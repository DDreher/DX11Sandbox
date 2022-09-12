#pragma comment(lib, "dxguid.lib")

#include "Renderer/DX11Util.h"

#include <d3d11sdklayers.h>

void DX11ResultFailed(HRESULT result, const char* dx_call, const char* file, uint32 line)
{
    const char* error_code = DXGetErrorString(result);
    char error_desc[512];
    DXGetErrorDescription(result, error_desc, sizeof(error_desc));
    LOG_ERROR("DX11 Call Failed!\n [Location] {}:{}\n [Code] {} \n [Status] {}\n [Msg] {}", file, line, dx_call, error_code, error_desc);
    abort();
}

void ReportLiveObjects(ComPtr<ID3D11Device> device)
{
#if _RENDER_DEBUG
    if (device != nullptr)
    {
        ComPtr<ID3D11Debug> device_debug;
        DX11_VERIFY(device.As(&device_debug));
        device_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        device_debug.Reset();
    }
#endif
}

void SetDebugName(ID3D11DeviceChild* device_child, const std::string& name)
{
#if _RENDER_DEBUG
    if (device_child != nullptr && name.empty() == false)
    {
        device_child->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)name.size(), name.c_str());
    }
#endif
}

