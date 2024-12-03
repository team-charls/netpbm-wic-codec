// SPDX-FileCopyrightText: © 2023 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

module;

// ReSharper disable once CppInconsistentNaming
#define _CRT_MEMCPY_S_INLINE inline

// ReSharper disable once CppUnusedIncludeDirective
#include <Unknwn.h> // Required to enable classic COM support in WinRT.

#include <winrt/base.h>

export module winrt;

export namespace winrt {

using winrt::check_hresult;
using winrt::check_win32;
using winrt::com_ptr;
using winrt::get_module_lock;
using winrt::hresult;
using winrt::implements;
using winrt::make;
using winrt::make_self;
using winrt::throw_hresult;
using winrt::hresult_error;

} // namespace winrt
