// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "pch.h"

#include <winerror.h>
#include <winrt/base.h>

export module class_factory;

import errors;

export template<typename Class>
struct class_factory : winrt::implements<class_factory<Class>, IClassFactory>
{
    HRESULT __stdcall CreateInstance(
        IUnknown* outer,
        GUID const& interface_id,
        void** result) noexcept override
    {
        if (!result)
            return error_pointer;

        *result = nullptr;

        if (outer)
            return error_no_aggregation;

        try
        {
            return winrt::make<Class>()->QueryInterface(interface_id, result);
        }
        catch (...)
        {
            return winrt::to_hresult();
        }
    }

    HRESULT __stdcall LockServer(BOOL) noexcept override
    {
        return error_ok;
    }
};
