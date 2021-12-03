// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "macros.h"
#include "winrt.h"

export module registry;

import errors;
import "pch.h";
import <span>;

using winrt::check_win32;

SUPPRESS_WARNING_NEXT_LINE(26493)                  // Don't use C-style casts
const HKEY hkey_local_machine{HKEY_LOCAL_MACHINE}; // NOLINT

export namespace registry {

inline void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name,
                      _Null_terminated_ const wchar_t* value)
{
    const auto length = wcslen(value) + 1;
    check_win32(RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_SZ, value,
                               static_cast<DWORD>(length * sizeof(wchar_t)))); // NOLINT(bugprone-misplaced-widening-cast)
}

inline void set_value(const std::wstring& sub_key, _Null_terminated_ const wchar_t* value_name,
                      _Null_terminated_ const wchar_t* value)
{
    set_value(sub_key.c_str(), value_name, value);
}

inline void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name, uint32_t value)
{
    check_win32(RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_DWORD, &value, sizeof value));
}

inline void set_value(const std::wstring& sub_key, _Null_terminated_ const wchar_t* value_name, const uint32_t value)
{
    set_value(sub_key.c_str(), value_name, value);
}

inline void set_value(_Null_terminated_ const wchar_t* sub_key, _Null_terminated_ const wchar_t* value_name,
                      const void* value, const size_t value_size_in_bytes)
{
    check_win32(
        RegSetKeyValue(hkey_local_machine, sub_key, value_name, REG_BINARY, value, static_cast<DWORD>(value_size_in_bytes)));
}

inline void set_value(const std::wstring& sub_key, _Null_terminated_ const wchar_t* value_name,
                      std::span<const std::byte> value)
{
    set_value(sub_key.c_str(), value_name, value.data(), value.size());
}

inline void set_value(const std::wstring& sub_key, _Null_terminated_ const wchar_t* value_name, const void* value,
                      const size_t value_size_in_bytes)
{
    set_value(sub_key.c_str(), value_name, value, value_size_in_bytes);
}

inline HRESULT delete_tree(_Null_terminated_ const wchar_t* sub_key) noexcept
{
    const LSTATUS result = RegDeleteTreeW(hkey_local_machine, sub_key);
    if (result != ERROR_SUCCESS)
    {
        return HRESULT_FROM_WIN32(result);
    }

    return error_ok;
}

} // namespace registry
