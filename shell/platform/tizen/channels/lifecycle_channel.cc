// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "lifecycle_channel.h"

#include <variant>

#include "flutter/shell/platform/common/client_wrapper/include/flutter/standard_message_codec.h"
#include "flutter/shell/platform/tizen/tizen_log.h"

namespace flutter {

namespace {

constexpr char kChannelName[] = "flutter/lifecycle";

constexpr char kInactive[] = "AppLifecycleState.inactive";
constexpr char kResumed[] = "AppLifecycleState.resumed";
constexpr char kPaused[] = "AppLifecycleState.paused";
constexpr char kDetached[] = "AppLifecycleState.detached";

// Codec extension for UTF-8 strings.
class StringSerializer : public StandardCodecSerializer {
 public:
  StringSerializer() = default;
  virtual ~StringSerializer() = default;

  // Returns the shared serializer instance.
  static const StringSerializer& GetInstance() {
    static StringSerializer sInstance;
    return sInstance;
  }

  virtual void WriteValue(const EncodableValue& value,
                          ByteStreamWriter* stream) const override {
    if (auto string_value = std::get_if<std::string>(&value)) {
      size_t size = string_value->size();
      if (size > 0) {
        stream->WriteBytes(
            reinterpret_cast<const uint8_t*>(string_value->data()), size);
      }
    }
  }
};

}  // namespace

LifecycleChannel::LifecycleChannel(BinaryMessenger* messenger)
    : channel_(std::make_unique<BasicMessageChannel<EncodableValue>>(
          messenger,
          kChannelName,
          &StandardMessageCodec::GetInstance(
              &StringSerializer::GetInstance()))) {}

LifecycleChannel::~LifecycleChannel() {}

void LifecycleChannel::AppIsInactive() {
  FT_LOGI("Sending %s message.", kInactive);
  channel_->Send(EncodableValue(kInactive));
}

void LifecycleChannel::AppIsResumed() {
  FT_LOGI("Sending %s message.", kResumed);
  channel_->Send(EncodableValue(kResumed));
}

void LifecycleChannel::AppIsPaused() {
  FT_LOGI("Sending %s message.", kPaused);
  channel_->Send(EncodableValue(kPaused));
}

void LifecycleChannel::AppIsDetached() {
  FT_LOGI("Sending %s message.", kDetached);
  channel_->Send(EncodableValue(kDetached));
}

}  // namespace flutter
