#pragma once
#include <d3d11.h>

#include "Renderer/DX11Types.h"
#include "Renderer/DXErr.h"

void DX11ResultFailed(HRESULT result, const char* dx_call, const char* file, uint32 line);

#define DX11_VERIFY(x) { HRESULT hresult = x; if(FAILED(hresult)) { DX11ResultFailed(hresult, #x, __FILE__, __LINE__);} }

// Safe release of COM pointers
template<typename T>
inline void SafeRelease(T& com_ptr)
{
    CHECK(com_ptr != nullptr);
    com_ptr->Release();
    com_ptr = nullptr;
}

void ReportLiveObjects(ComPtr<ID3D11Device> device);

void SetDebugName(ID3D11DeviceChild* device_child, const std::string& name);
