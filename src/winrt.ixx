// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#define _CRT_MEMCPY_S_INLINE inline

// ReSharper disable once CppUnusedIncludeDirective
#include <Unknwn.h> // Required to enable classic COM support in WinRT.

#pragma warning(push)
#pragma warning(disable : 4265) // : '' : class has virtual functions, but its non-trivial destructor is not virtual
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#pragma warning(disable : 5260) // the constant variable has external\internal linkage
#include <winrt/base.h>
#pragma warning(pop)

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
using winrt::to_hresult;

} // namespace winrt
