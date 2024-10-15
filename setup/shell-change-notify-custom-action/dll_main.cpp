// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

#include <Windows.h>
#include <msiquery.h>
#include <ShlObj.h>

unsigned int __stdcall SHChangeNotifyCustomAction(MSIHANDLE /*install*/)
{
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

    return ERROR_SUCCESS;
}
