// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

#pragma once

#include <sdkddkver.h> // Defines the highest available Windows platform.

#include <olectl.h>
#include <ShlObj.h>
#include <Unknwn.h>

#include "wic_bitmap_source.h"
#include <wincodec.h>

#pragma warning(push)
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#include <mfapi.h>
#pragma warning(pop)
