// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

export module util;

import std;
import winrt;
import <win.hpp>;

import hresults;
import "macros.hpp";

using std::wstring;
using winrt::hresult;

namespace {

[[nodiscard]] HMODULE get_current_module() noexcept
{
    HMODULE current_module;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(get_current_module), &current_module);

    return current_module;
}

} // namespace

export [[nodiscard]] wstring guid_to_string(const GUID& guid)
{
    wstring guid_text;

    guid_text.resize(39);
    VERIFY(StringFromGUID2(guid, guid_text.data(), static_cast<int>(guid_text.size())) != 0);

    // Remove the double null terminator.
    guid_text.resize(guid_text.size() - 1);

    return guid_text;
}

export [[nodiscard]] wstring get_module_path()
{
    wstring path(100, L'?');
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

// Copied from fmtlib as standard C++20 lacks this helper method and requires fmt_ptr
export template<typename T>
[[nodiscard]] auto fmt_ptr(T p) noexcept -> const void*
{
    static_assert(std::is_pointer_v<T>);
    return std::bit_cast<const void*>(p);
}

export void inline check_hresult(const hresult result, const hresult result_to_throw)
{
    if (result < 0)
        throw_hresult(result_to_throw);
}

export [[nodiscard]] constexpr bool failed(const hresult result) noexcept
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

export void check_condition(const bool condition, const hresult result_to_throw)
{
    if (!condition)
        throw_hresult(result_to_throw);
}

export __declspec(noinline) HRESULT to_hresult() noexcept
{
    try
    {
        throw;
    }
    catch (winrt::hresult_error const& e)
    {
        return e.code();
    }
    catch (std::bad_alloc const&)
    {
        return error_out_of_memory;
    }

    // Explicitly don't catch other unexpected exceptions to trigger call to terminate with dump.
}
