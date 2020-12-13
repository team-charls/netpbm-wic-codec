// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <SDKDDKVer.h> // Defines the highest available Windows platform.

#include <Unknwn.h>
#include <Shlwapi.h>

#pragma warning(push)
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#include <winrt/base.h>
#pragma warning(pop)
