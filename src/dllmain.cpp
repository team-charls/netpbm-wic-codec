// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#include "pch.h"

import netpbm_bitmap_decoder;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
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

// {06891bbe-cc02-4bb2-9cf0-303fc4e668c3}
constexpr GUID CLSID_NetPbmDecoder{0x6891bbe, 0xcc02, 0x4bb2, {0x9c, 0xf0, 0x30, 0x3f, 0xc4, 0xe6, 0x68, 0xc3}};


_Check_return_
HRESULT __stdcall DllGetClassObject(_In_ GUID const& class_id, _In_ GUID const& interface_id, _Outptr_ void** result)
{
    if (class_id == CLSID_NetPbmDecoder)
        return create_netpbm_bitmap_decoder_factory(interface_id, result);

    return CLASS_E_CLASSNOTAVAILABLE;
}
