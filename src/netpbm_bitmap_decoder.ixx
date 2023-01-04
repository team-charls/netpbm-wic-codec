// Copyright (c) Victor Derks.
// SPDX-License-Identifier: MIT
export module netpbm_bitmap_decoder;

import <winerror.h>;
import <guiddef.h>;

export void create_netpbm_bitmap_decoder_factory(GUID const& interface_id, void** result);
