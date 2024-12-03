// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

// Workaround for MSVC defect https://developercommunity.visualstudio.com/t/IntelliSense-error-E3344-due-to-CppUnit/10185254
#ifdef __INTELLISENSE__
inline void __stdcall Internal_SetExpectedExceptionMessage(const unsigned short* /*message*/)
{
}
#endif

#include <CppUnitTest.h>
