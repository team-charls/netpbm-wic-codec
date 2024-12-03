// SPDX-FileCopyrightText: © 2020 Team CharLS
// SPDX-License-Identifier: BSD-3-Clause

module;

#include "intellisense.hpp"

export module hresults;

import <win.hpp>;

export {

constexpr HRESULT success_ok{S_OK};
constexpr HRESULT success_false{S_FALSE};
constexpr HRESULT error_fail{E_FAIL};
constexpr HRESULT error_pointer{E_POINTER};
constexpr HRESULT error_already_initialized{ERROR_ALREADY_INITIALIZED};
constexpr HRESULT error_no_aggregation{CLASS_E_NOAGGREGATION};
constexpr HRESULT error_class_not_available{CLASS_E_CLASSNOTAVAILABLE};
constexpr HRESULT error_invalid_argument{E_INVALIDARG};
constexpr HRESULT error_access_denied{STG_E_ACCESSDENIED};
constexpr HRESULT error_out_of_memory{E_OUTOFMEMORY};
constexpr HRESULT error_not_valid_state{E_NOT_VALID_STATE};

namespace self_registration {

constexpr HRESULT error_class{SELFREG_E_CLASS};

}

namespace wincodec {

constexpr HRESULT error_palette_unavailable{WINCODEC_ERR_PALETTEUNAVAILABLE};
constexpr HRESULT error_unsupported_operation{WINCODEC_ERR_UNSUPPORTEDOPERATION};
constexpr HRESULT error_codec_no_thumbnail{WINCODEC_ERR_CODECNOTHUMBNAIL};
constexpr HRESULT error_frame_missing{WINCODEC_ERR_FRAMEMISSING};
constexpr HRESULT error_not_initialized{WINCODEC_ERR_NOTINITIALIZED};
constexpr HRESULT error_wrong_state{WINCODEC_ERR_WRONGSTATE};
constexpr HRESULT error_unsupported_pixel_format{WINCODEC_ERR_UNSUPPORTEDPIXELFORMAT};
constexpr HRESULT error_codec_too_many_scan_lines{WINCODEC_ERR_CODECTOOMANYSCANLINES};
constexpr HRESULT error_component_not_found{WINCODEC_ERR_COMPONENTNOTFOUND};
constexpr HRESULT error_bad_header{WINCODEC_ERR_BADHEADER};
constexpr HRESULT error_bad_image{WINCODEC_ERR_BADIMAGE};
constexpr HRESULT error_stream_not_available{WINCODEC_ERR_STREAMNOTAVAILABLE};
constexpr HRESULT error_stream_read{WINCODEC_ERR_STREAMREAD};

} // namespace wincodec

}
