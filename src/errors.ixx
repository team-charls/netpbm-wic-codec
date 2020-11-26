// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT

module;

#include <winerror.h>
#include <winrt/base.h>


export module errors;

export {

constexpr winrt::hresult error_ok{S_OK};
constexpr winrt::hresult error_fail{E_FAIL};
constexpr winrt::hresult error_pointer{E_POINTER};
constexpr winrt::hresult error_no_aggregation{static_cast<winrt::hresult>(CLASS_E_NOAGGREGATION)};
constexpr winrt::hresult error_class_not_available{static_cast<winrt::hresult>(CLASS_E_CLASSNOTAVAILABLE)};
constexpr winrt::hresult error_invalid_argument{static_cast<winrt::hresult>(E_INVALIDARG)};

}

export namespace wincodec {

constexpr winrt::hresult error_palette_unavailable{static_cast<winrt::hresult>(WINCODEC_ERR_PALETTEUNAVAILABLE)};
constexpr winrt::hresult error_unsupported_operation{static_cast<winrt::hresult>(WINCODEC_ERR_UNSUPPORTEDOPERATION)};
constexpr winrt::hresult error_codec_no_thumbnail{static_cast<winrt::hresult>(WINCODEC_ERR_CODECNOTHUMBNAIL)};
constexpr winrt::hresult error_frame_missing{static_cast<winrt::hresult>(WINCODEC_ERR_FRAMEMISSING)};
constexpr winrt::hresult error_not_initialized{static_cast<winrt::hresult>(WINCODEC_ERR_NOTINITIALIZED)};
constexpr winrt::hresult error_wrong_state{static_cast<winrt::hresult>(WINCODEC_ERR_WRONGSTATE)};
constexpr winrt::hresult error_unsupported_pixel_format{static_cast<winrt::hresult>(WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT)};
constexpr winrt::hresult error_codec_too_many_scan_lines{static_cast<winrt::hresult>(WINCODEC_ERR_CODECTOOMANYSCANLINES)};
constexpr winrt::hresult error_component_not_found{static_cast<winrt::hresult>(WINCODEC_ERR_COMPONENTNOTFOUND)};
constexpr winrt::hresult error_bad_header{static_cast<winrt::hresult>(WINCODEC_ERR_BADHEADER)};
constexpr winrt::hresult error_bad_image{static_cast<winrt::hresult>(WINCODEC_ERR_BADIMAGE)};

} // namespace wincodec