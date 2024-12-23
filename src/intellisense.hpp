// SPDX-FileCopyrightText: © 2024 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

// Note: 2 separate workarounds are used to make it easier to track the status of the respective issues.

// Include explicit headers as workaround that IntelliSense (VS 2022 17.12) fails to parse #import <win.hpp>
#if defined(__INTELLISENSE__)
#define _AMD64_
#include <combaseapi.h>
#include <libloaderapi.h>
#include <sal.h>
#include <wincodec.h>
#include <winreg.h>
#include <propkey.h>
#endif

// Include explicit headers as workaround that ReSharper (2024.3.2) fails to parse #import <win.hpp>
#if defined(__RESHARPER__)
// ReSharper disable once CppInconsistentNaming
#include <combaseapi.h>
#include <propkey.h>
#include <wincodec.h>

#include "macros.hpp"
#endif
