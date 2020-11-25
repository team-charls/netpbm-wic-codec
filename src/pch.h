// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.
#pragma once

#include <sdkddkver.h> // Defines the highest available Windows platform.

#include <OleCtl.h>
#include <ShlObj.h>
#include <Unknwn.h>
#include <wincodec.h>

#pragma warning(push)
#pragma warning(disable : 4619) // #pragma warning : there is no warning number '5204' (older MSVC compiler versions don't know 5204)
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#include <mfapi.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 4619) // #pragma warning : there is no warning number '5204'  (older MSVC compiler versions don't know 5204)
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#include <winrt/base.h>
#pragma warning(pop)
