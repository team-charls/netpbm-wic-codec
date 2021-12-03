// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "winrt.h"

export module class_factory;

import errors;
import "pch.h";

export template<typename Class>
struct class_factory : winrt::implements<class_factory<Class>, IClassFactory>
{
    HRESULT __stdcall CreateInstance(IUnknown* outer, GUID const& interface_id, void** result) noexcept override
    try
    {
        *check_out_pointer(result) = nullptr;
        check_condition(!outer, error_no_aggregation);

        return winrt::make<Class>()->QueryInterface(interface_id, result);
    }
    catch (...)
    {
        return winrt::to_hresult();
    }

    HRESULT __stdcall LockServer(BOOL) noexcept override
    {
        return error_ok;
    }
};
