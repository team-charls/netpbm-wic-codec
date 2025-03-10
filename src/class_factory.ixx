// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

export module class_factory;

import <win.hpp>;
import winrt;

import hresults;
import util;

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
        return to_hresult();
    }

    HRESULT __stdcall LockServer(BOOL) noexcept override
    {
        return success_ok;
    }
};
