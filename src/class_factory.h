// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <winerror.h>
#include <winrt/base.h>


inline constexpr winrt::hresult error_ok{S_OK};
inline constexpr winrt::hresult error_fail{E_FAIL};
inline constexpr winrt::hresult error_pointer{E_POINTER};
inline constexpr winrt::hresult error_no_aggregation{static_cast<winrt::hresult>(CLASS_E_NOAGGREGATION)};
inline constexpr winrt::hresult error_class_not_available{static_cast<winrt::hresult>(CLASS_E_CLASSNOTAVAILABLE)};
inline constexpr winrt::hresult error_invalid_argument{static_cast<winrt::hresult>(E_INVALIDARG)};

template<typename Class>
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
