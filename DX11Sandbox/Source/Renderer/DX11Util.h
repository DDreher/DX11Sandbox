#pragma once

#include "DXErr.h"

void DX11ResultFailed(HRESULT result, const char* dx_call, const char* file, uint32 line);

#define DX11_VERIFY(x) { HRESULT hresult = x; if(FAILED(hresult)) { DX11ResultFailed(hresult, #x, __FILE__, __LINE__);} }
