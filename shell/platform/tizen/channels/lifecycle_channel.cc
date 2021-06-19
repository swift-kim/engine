// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lifecycle_channel.h"

#include "flutter/shell/platform/common/client_wrapper/include/flutter/standard_message_codec.h"
#include "flutter/shell/platform/tizen/flutter_tizen_engine.h"
#include "flutter/shell/platform/tizen/tizen_log.h"

static constexpr char kChannelName[] = "flutter/lifecycle";
static constexpr char kInactive[] = "AppLifecycleState.inactive";
static constexpr char kResumed[] = "AppLifecycleState.resumed";
static constexpr char kPaused[] = "AppLifecycleState.paused";
static constexpr char kDetached[] = "AppLifecycleState.detached";

LifecycleChannel::LifecycleChannel(flutter::BinaryMessenger* messenger)
    : channel_(std::make_unique<
               flutter::BasicMessageChannel<flutter::EncodableValue>>(
          messenger,
          kChannelName,
          &flutter::StandardMessageCodec::GetInstance())) {}

LifecycleChannel::~LifecycleChannel() {}

void LifecycleChannel::AppIsInactive() {
  FT_LOGI("Sending %s message.", kInactive);
  channel_->Send(flutter::EncodableValue(kInactive));
}

void LifecycleChannel::AppIsResumed() {
  FT_LOGI("Sending %s message.", kResumed);
  channel_->Send(flutter::EncodableValue(kResumed));
}

void LifecycleChannel::AppIsPaused() {
  FT_LOGI("Sending %s message.", kPaused);
  channel_->Send(flutter::EncodableValue(kPaused));
}

void LifecycleChannel::AppIsDetached() {
  FT_LOGI("Sending %s message.", kDetached);
  channel_->Send(flutter::EncodableValue(kDetached));
}
