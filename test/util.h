// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include "winrt.h"

#include "cpp_unit_test.h"

#define SUPPRESS_WARNING_NEXT_LINE(x) \
    __pragma(warning(suppress \
                     : x)) // NOLINT(misc-macro-parentheses, bugprone-macro-parentheses, cppcoreguidelines-macro-usage)

// The static analyzer detects that a invalid argument is passed, for unit testing
// this warning must be suppressed.
#define SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE SUPPRESS_WARNING_NEXT_LINE(6387)


constexpr bool succeeded(winrt::hresult const result) noexcept
{
    return result >= 0;
}

template<>
inline std::wstring Microsoft::VisualStudio::CppUnitTestFramework::ToString<winrt::hresult>(const winrt::hresult& q)
{
    RETURN_WIDE_STRING(static_cast<int>(q));
}
