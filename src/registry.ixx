// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

export module registry;

import std;
import winrt;
import <win.hpp>;

import hresults;
import "macros.hpp";

using std::wstring;
using winrt::check_win32;

SUPPRESS_WARNING_NEXT_LINE(26493)                  // Don't use C-style casts (used by macro HKEY_LOCAL_MACHINE)
const HKEY hkey_local_machine{HKEY_LOCAL_MACHINE}; // NOLINT

namespace registry {

export void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name,
                      _Null_terminated_ const wchar_t* value)
{
    const auto length = wcslen(value) + 1;
    check_win32(RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_SZ, value,
                               static_cast<DWORD>(length * sizeof(wchar_t)))); // NOLINT(bugprone-misplaced-widening-cast)
}

export inline void set_value(const wstring& sub_key, _Null_terminated_ const wchar_t* value_name,
                             _Null_terminated_ const wchar_t* value)
{
    set_value(sub_key.c_str(), value_name, value);
}

export void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name,
                      const std::uint32_t value)
{
    check_win32(RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_DWORD, &value, sizeof value));
}

export void set_value(const wstring& sub_key, _Null_terminated_ const wchar_t* value_name, const std::uint32_t value)
{
    set_value(sub_key.c_str(), value_name, value);
}

export void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name,
                      const void* value, const size_t value_size_in_bytes)
{
    check_win32(
        RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_BINARY, value, static_cast<DWORD>(value_size_in_bytes)));
}

export void set_value(const wstring& sub_key, _Null_terminated_ const wchar_t* value_name,
                      const std::span<const std::byte> value)
{
    set_value(sub_key.c_str(), value_name, value.data(), value.size());
}

export void set_value(const wstring& sub_key, _Null_terminated_ const wchar_t* value_name, const void* value,
                      const size_t value_size_in_bytes)
{
    set_value(sub_key.c_str(), value_name, value, value_size_in_bytes);
}

export HRESULT delete_tree(_Null_terminated_ const wchar_t* sub_key) noexcept
{
    if (const LSTATUS result{RegDeleteTreeW(hkey_local_machine, sub_key)}; result != ERROR_SUCCESS)
    {
        return HRESULT_FROM_WIN32(result);
    }

    return success_ok;
}

} // namespace registry
