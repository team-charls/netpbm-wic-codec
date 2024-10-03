// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

// Workaround for MSVC defect developercommunity.visualstudio.com/t/IntelliSense-error-E3344-due-to-CppUnit/10185254
#ifdef __INTELLISENSE__
inline void __stdcall Internal_SetExpectedExceptionMessage(const unsigned short* /*message*/)
{
}
#endif

#include <CppUnitTest.h>
