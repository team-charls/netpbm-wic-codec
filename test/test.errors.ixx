// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include "winrt.h"

export module test.errors;

export {

inline constexpr winrt::hresult error_ok{S_OK};
inline constexpr winrt::hresult error_fail{E_FAIL};
inline constexpr winrt::hresult error_pointer{E_POINTER};
inline constexpr winrt::hresult error_no_aggregation{CLASS_E_NOAGGREGATION};
inline constexpr winrt::hresult error_class_not_available{CLASS_E_CLASSNOTAVAILABLE};
inline constexpr winrt::hresult error_invalid_argument{E_INVALIDARG};

namespace wincodec {

inline constexpr winrt::hresult error_palette_unavailable{WINCODEC_ERR_PALETTEUNAVAILABLE};
inline constexpr winrt::hresult error_unsupported_operation{WINCODEC_ERR_UNSUPPORTEDOPERATION};
inline constexpr winrt::hresult error_codec_no_thumbnail{WINCODEC_ERR_CODECNOTHUMBNAIL};
inline constexpr winrt::hresult error_frame_missing{WINCODEC_ERR_FRAMEMISSING};
inline constexpr winrt::hresult error_not_initialized{WINCODEC_ERR_NOTINITIALIZED};
inline constexpr winrt::hresult error_wrong_state{WINCODEC_ERR_WRONGSTATE};
inline constexpr winrt::hresult error_unsupported_pixel_format{WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT};
inline constexpr winrt::hresult error_codec_too_many_scan_lines{WINCODEC_ERR_CODECTOOMANYSCANLINES};
inline constexpr winrt::hresult error_component_not_found{WINCODEC_ERR_COMPONENTNOTFOUND};
inline constexpr winrt::hresult error_bad_header{WINCODEC_ERR_BADHEADER};
inline constexpr winrt::hresult error_bad_image{WINCODEC_ERR_BADIMAGE};

} // namespace wincodec

constexpr bool failed(const winrt::hresult result) noexcept
{
    return result < 0;
}

}
