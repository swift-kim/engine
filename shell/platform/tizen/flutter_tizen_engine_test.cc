// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Included first as it collides with the X11 headers.
#include "gtest/gtest.h"

// #include "flutter/shell/platform/embedder/test_utils/proc_table_replacement.h"
#include "flutter/shell/platform/common/client_wrapper/include/flutter/plugin_registrar.h"
#include "flutter/shell/platform/common/incoming_message_dispatcher.h"
// #include "flutter/shell/platform/embedder/embedder.h"
#include "flutter/shell/platform/tizen/channels/key_event_channel.h"
#include "flutter/shell/platform/tizen/channels/lifecycle_channel.h"
#include "flutter/shell/platform/tizen/channels/localization_channel.h"
#include "flutter/shell/platform/tizen/channels/navigation_channel.h"
#include "flutter/shell/platform/tizen/channels/platform_channel.h"
#include "flutter/shell/platform/tizen/channels/platform_view_channel.h"
#include "flutter/shell/platform/tizen/channels/settings_channel.h"
#include "flutter/shell/platform/tizen/channels/text_input_channel.h"
#include "flutter/shell/platform/tizen/flutter_tizen_texture_registrar.h"
#include "flutter/shell/platform/tizen/key_event_handler.h"
#include "flutter/shell/platform/tizen/public/flutter_tizen.h"
#include "flutter/shell/platform/tizen/tizen_event_loop.h"
#include "flutter/shell/platform/tizen/tizen_renderer.h"
#ifdef TIZEN_RENDERER_EVAS_GL
#include "flutter/shell/platform/tizen/tizen_renderer_evas_gl.h"
#else
#include "flutter/shell/platform/tizen/tizen_renderer_ecore_wl2.h"
#include "flutter/shell/platform/tizen/tizen_vsync_waiter.h"
#endif
#include "flutter/shell/platform/tizen/touch_event_handler.h"


TEST(FlutterTizenEngineTest, Test1) {
  EXPECT_TRUE(true);
}

TEST(FlutterTizenEngineTest, Test2) {
  EXPECT_TRUE(true);
}