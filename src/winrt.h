#pragma once

// ReSharper disable once CppUnusedIncludeDirective
#include <Unknwn.h> // Required to enable classic COM support in WinRT.

#pragma warning(push)
#pragma warning(disable : 4946) // reinterpret_cast used between related classes
#pragma warning(disable : 5204) // class has virtual functions, but its trivial destructor is not virtual
#pragma warning(disable : 5246) // '': the initialization of a sub-object should be wrapped in braces
#pragma warning(disable : 5260) // the constant variable has external\internal linkage
#include <winrt/base.h>
#pragma warning(pop)

template<>
struct std::formatter<winrt::hresult> : std::formatter<int32_t>
{
    auto format(const winrt::hresult& result, std::format_context& ctx) const
    {
        return std::formatter<int32_t>::format(result.value, ctx);
    }
};
