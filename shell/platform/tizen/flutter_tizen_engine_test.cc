// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>
#include <chrono>
#include <thread>
#include "gtest/gtest.h"

#include "flutter/shell/platform/tizen/flutter_tizen_engine.h"

using namespace std::chrono_literals;

std::string TPK_ROOT_PATH = "/tpkroot";
std::string LIB_PATH = "/lib";
std::string RES_PATH = "/res";

TEST(FlutterTizenEngineTestSimple, Create_Headless) {
  flutter::FlutterTizenEngine* tizen_engine =
      new flutter::FlutterTizenEngine(false);
  EXPECT_TRUE(tizen_engine != nullptr);
  delete tizen_engine;
}

// TODO
TEST(FlutterTizenEngineTestSimple, DISABLED_Create_Headed) {
  flutter::FlutterTizenEngine* tizen_engine =
      new flutter::FlutterTizenEngine(true);
  EXPECT_TRUE(tizen_engine != nullptr);
  delete tizen_engine;
}

class FlutterTizenEngineTest : public testing::Test {
 public:
  FlutterTizenEngineTest() {
    std::string tpk_root;
    char path[256];
    EXPECT_TRUE(getcwd(path, sizeof(path)) != NULL);
    tpk_root = path + TPK_ROOT_PATH;

    assets_path = tpk_root + RES_PATH + "/flutter_assets";
    icu_data_path = tpk_root + RES_PATH + "/icudtl.dat";
    aot_lib_path = tpk_root + LIB_PATH + "/libapp.so";

    switches.push_back("--disable-observatory");
    // switches.push_back("--verbose-logging");
    // switches.push_back("--enable-dart-profiling");
    // switches.push_back("--enable-checked-mode");
  }

 protected:
  void SetUp() {
    engine_prop.assets_path = assets_path.c_str();
    engine_prop.icu_data_path = icu_data_path.c_str();
    engine_prop.aot_library_path = aot_lib_path.c_str();
    engine_prop.switches = switches.data();
    engine_prop.switches_count = switches.size();

    auto engine = std::make_unique<flutter::FlutterTizenEngine>(false);
    engine_ = engine.release();
  }

  void TearDown() {
    if (engine_) {
      delete engine_;
    }
    engine_ = nullptr;
  }

  std::string assets_path;
  std::string icu_data_path;
  std::string aot_lib_path;
  flutter::FlutterTizenEngine* engine_;
  FlutterDesktopEngineProperties engine_prop = {};
  std::vector<const char*> switches;
};

TEST_F(FlutterTizenEngineTest, Run) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->RunEngine(engine_prop));
  EXPECT_TRUE(true);
}

// TODO
TEST_F(FlutterTizenEngineTest, DISABLED_Run_Twice) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->RunEngine(engine_prop));
  EXPECT_FALSE(engine_->RunEngine(engine_prop));
  EXPECT_TRUE(true);
}

TEST_F(FlutterTizenEngineTest, Stop) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->RunEngine(engine_prop));
  EXPECT_TRUE(engine_->StopEngine());
}

TEST_F(FlutterTizenEngineTest, Stop_Twice) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->RunEngine(engine_prop));
  EXPECT_TRUE(engine_->StopEngine());
  EXPECT_FALSE(engine_->StopEngine());
}

TEST_F(FlutterTizenEngineTest, GetPluginRegistrar) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->GetPluginRegistrar() != nullptr);
}

// TODO
TEST_F(FlutterTizenEngineTest, DISABLED_GetTextureRegistrar) {
  EXPECT_TRUE(engine_ != nullptr);
  EXPECT_TRUE(engine_->GetTextureRegistrar() != nullptr);
}
