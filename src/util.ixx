// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "macros.hpp"

export module util;

import std;
import <win.hpp>;
import winrt;

import errors;

namespace {

[[nodiscard]] HMODULE get_current_module() noexcept
{
    HMODULE module;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(get_current_module), &module);

    return module;
}

} // namespace

export constexpr std::byte operator"" _byte(const unsigned long long int n)
{
    return static_cast<std::byte>(n);
}


export [[nodiscard]] const char* pixel_format_to_string(const GUID& guid) noexcept
{
    if (guid == GUID_WICPixelFormat2bppGray)
        return "GUID_WICPixelFormat2bppGray";

    if (guid == GUID_WICPixelFormat4bppGray)
        return "GUID_WICPixelFormat4bppGray";

    if (guid == GUID_WICPixelFormat8bppGray)
        return "GUID_WICPixelFormat8bppGray";

    if (guid == GUID_WICPixelFormat16bppGray)
        return "GUID_WICPixelFormat16bppGray";

    if (guid == GUID_WICPixelFormat24bppRGB)
        return "GUID_WICPixelFormat24bppRGB";

    if (guid == GUID_WICPixelFormat48bppRGB)
        return "GUID_WICPixelFormat48bppRGB";

    return "Unknown";
}

export [[nodiscard]] std::wstring guid_to_string(const GUID& guid)
{
    std::wstring guid_text;

    guid_text.resize(39);
    VERIFY(StringFromGUID2(guid, guid_text.data(), static_cast<int>(guid_text.size())) != 0);

    // Remove the double null terminator.
    guid_text.resize(guid_text.size() - 1);

    return guid_text;
}

export [[nodiscard]] std::wstring get_module_path()
{
    std::wstring path(100, L'?');
    size_t path_size;
    size_t actual_size;

    do
    {
        path_size = path.size();
        actual_size = ::GetModuleFileName(get_current_module(), path.data(), static_cast<DWORD>(path_size));

        if (actual_size + 1 > path_size)
        {
            path.resize(path_size * 2, L'?');
        }
    } while (actual_size + 1 > path_size);

    path.resize(actual_size);
    return path;
}

export template<>
struct std::formatter<winrt::hresult> : std::formatter<int32_t>
{
    auto format(const winrt::hresult& result, std::format_context& ctx) const
    {
        return std::formatter<int32_t>::format(result.value, ctx);
    }
};

// Copied from fmtlib as standard C++20 lacks this helper method and requires fmt_ptr
export template<typename T>
[[nodiscard]]
auto fmt_ptr(T p) noexcept -> const void*
{
    static_assert(std::is_pointer_v<T>);
    return std::bit_cast<const void*>(p);
}

export void inline check_hresult(const winrt::hresult result, const winrt::hresult result_to_throw)
{
    if (result < 0)
        throw_hresult(result_to_throw);
}

export [[nodiscard]] constexpr bool failed(const winrt::hresult result) noexcept
{
    return result < 0;
}

export template<typename T>
T* check_in_pointer(_In_ T* pointer)
{
    if (!pointer)
        winrt::throw_hresult(error_invalid_argument);

    return pointer;
}

export template<typename T>
T* check_out_pointer(T* pointer)
{
    if (!pointer)
        winrt::throw_hresult(error_pointer);

    return pointer;
}

export void check_condition(const bool condition, const winrt::hresult result_to_throw)
{
    if (!condition)
        throw_hresult(result_to_throw);
}
