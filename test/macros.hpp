// Copyright (c) Team CharLS.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#define SUPPRESS_WARNING_NEXT_LINE(x) \
    __pragma(warning(suppress \
                     : x)) // NOLINT(misc-macro-parentheses, bugprone-macro-parentheses, cppcoreguidelines-macro-usage)

// The static analyzer detects that an invalid argument is passed, for unit testing
// this warning must be suppressed.
#define SUPPRESS_WARNING_6387_INVALID_ARGUMENT_NEXT_LINE SUPPRESS_WARNING_NEXT_LINE(6387)
