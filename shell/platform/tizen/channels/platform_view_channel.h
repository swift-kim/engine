// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_PLATFORM_VIEW_CHANNEL_H_
#define EMBEDDER_PLATFORM_VIEW_CHANNEL_H_

#include <Ecore_Input.h>

#include <map>
#include <memory>
#include <string>

#include "flutter/shell/platform/common/client_wrapper/include/flutter/binary_messenger.h"
#include "flutter/shell/platform/common/client_wrapper/include/flutter/method_channel.h"
#include "rapidjson/document.h"

class PlatformView;
class PlatformViewFactory;

namespace flutter {

class FlutterTizenEngine;

class PlatformViewChannel {
 public:
  explicit PlatformViewChannel(BinaryMessenger* messenger,
                               FlutterTizenEngine* engine);
  virtual ~PlatformViewChannel();

  void Dispose();

  std::map<std::string, std::unique_ptr<PlatformViewFactory>>& ViewFactories() {
    return view_factories_;
  }
  std::map<int, PlatformView*>& ViewInstances() { return view_instances_; }

  void SendKeyEvent(Ecore_Event_Key* key, bool is_down);
  int CurrentFocusedViewId();

  void DispatchCompositionUpdateEvent(const std::string& key);
  void DispatchCompositionEndEvent(const std::string& key);

 private:
  void HandleMethodCall(const MethodCall<EncodableValue>& call,
                        std::unique_ptr<MethodResult<EncodableValue>> result);

  FlutterTizenEngine* engine_;
  std::unique_ptr<MethodChannel<EncodableValue>> channel_;
  std::map<std::string, std::unique_ptr<PlatformViewFactory>> view_factories_;
  std::map<int, PlatformView*> view_instances_;
};

}  // namespace flutter

#endif  // EMBEDDER_PLATFORM_VIEW_CHANNEL_H_
