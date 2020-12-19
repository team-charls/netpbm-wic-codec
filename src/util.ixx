// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "macros.h"
#include "pch.h"

export module util;

[[nodiscard]] HMODULE get_current_module() noexcept
{
    HMODULE module;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<PCTSTR>(get_current_module), &module);

    return module;
}

export
{
    constexpr std::byte operator"" _byte(const unsigned long long int n)
    {
        return static_cast<std::byte>(n);
    }


    [[nodiscard]] inline const char* pixel_format_to_string(const GUID& guid) noexcept
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

    [[nodiscard]] inline std::wstring guid_to_string(const GUID& guid)
    {
        std::wstring guid_text;

        guid_text.resize(39);
        VERIFY(StringFromGUID2(guid, guid_text.data(), static_cast<int>(guid_text.size())) != 0);

        // Remove the double null terminator.
        guid_text.resize(guid_text.size() - 1);

        return guid_text;
    }

    [[nodiscard]] inline std::wstring get_module_path()
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
}
