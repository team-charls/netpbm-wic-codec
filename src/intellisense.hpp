// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

// Include explicit headers as workaround that IntelliSense in VS 2022 17.12 fails to parse #import <win.h>
#if defined(__INTELLISENSE__) || defined(__RESHARPER__)
#define _AMD64_
#include <combaseapi.h>
#include <libloaderapi.h>
#include <sal.h>
#include <wincodec.h>
#include <winreg.h>
#endif
