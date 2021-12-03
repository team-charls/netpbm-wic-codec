// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

export module trace;

import "pch.h";
import <cstdio>;

export template<typename... Args>
void trace(char const* const message, Args... args) noexcept
{
    char buffer[1024];
    static_cast<void>(snprintf(buffer, sizeof buffer, message, args...)); // NOLINT(cppcoreguidelines-pro-type-vararg)
    OutputDebugStringA(buffer);
}
