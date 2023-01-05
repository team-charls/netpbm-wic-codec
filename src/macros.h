// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once


#define SUPPRESS_WARNING_NEXT_LINE(x) \
    __pragma(warning(suppress \
                     : x)) // NOLINT(misc-macro-parentheses, bugprone-macro-parentheses, cppcoreguidelines-macro-usage)

// The macros below can be used to disable false positives. These warnings are too useful to disable them globally.

// A false warning is generated when a noexcept methods calls a function that is not marked noexcept.
// Reported to Microsoft:
// https://developercommunity.visualstudio.com/content/problem/804372/c26447-false-positive-when-using-stdscoped-lock-ev.html
#define SUPPRESS_FALSE_WARNING_C26447_NEXT_LINE SUPPRESS_WARNING_NEXT_LINE(26447)

// returning uninitialized memory (false positive)
// Reported to Microsoft (but was closed due to lower priority):
// https://developercommunity.visualstudio.com/t/static-analysis-emits-a-false-positive-c6101-error/804429
#define SUPPRESS_FALSE_WARNING_C6101_NEXT_LINE SUPPRESS_WARNING_NEXT_LINE(6101)


#ifdef NDEBUG

#define ASSERT(expression) static_cast<void>(0)
#define VERIFY(expression) static_cast<void>(expression)

#else

#define ASSERT(expression) \
    __pragma(warning(push)) __pragma(warning(disable : 26493)) assert(expression) __pragma(warning(pop))
#define VERIFY(expression) assert(expression)

#endif

// The TRACE macro can be used in debug build to watch the behaviour of the implementation
// when used by 3rd party applications.
#ifdef NDEBUG
    #define TRACE __noop
#else
    #ifdef __RESHARPER__
        // ReSharper 2022.3.1 fails to parse C++20 macro __VA_OPT__
        #define TRACE __noop
    #else
        // Use std::format directly to get compile time checking of the string arguments.
        #define TRACE(fmt, ...) OutputDebugStringA(std::format(fmt __VA_OPT__(,) __VA_ARGS__).c_str())
    #endif
#endif
