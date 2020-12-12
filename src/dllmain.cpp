// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

#include "trace.h"

import netpbm_bitmap_decoder;
import errors;
import guids;

BOOL APIENTRY DllMain( HMODULE /*hModule*/,
                       DWORD  ul_reason_for_call,
                       LPVOID /*reserved*/
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

_Check_return_
HRESULT __stdcall DllGetClassObject(_In_ GUID const& class_id, _In_ GUID const& interface_id, _Outptr_ void** result)
{
    if (class_id == CLSID_NetPbmDecoder)
        return create_netpbm_bitmap_decoder_factory(interface_id, result);

    return CLASS_E_CLASSNOTAVAILABLE;
}

// Purpose: Used to determine whether the COM sub-system can unload the DLL from memory.
__control_entrypoint(DllExport)
    HRESULT __stdcall DllCanUnloadNow()
{
    const auto result = winrt::get_module_lock() ? S_FALSE : S_OK;
    TRACE("netpbm-wic-codec::DllCanUnloadNow hr = %d (0 = S_OK -> unload OK)\n", result);
    return result;
}

HRESULT __stdcall DllRegisterServer()
{
    return SELFREG_E_CLASS;
}

HRESULT __stdcall DllUnregisterServer()
{
    return error_ok;
}
