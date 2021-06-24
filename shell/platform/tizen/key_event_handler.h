// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_KEY_EVENT_HANDLER_H_
#define EMBEDDER_KEY_EVENT_HANDLER_H_

#include <Ecore_Input.h>

#include <map>
#include <vector>

namespace flutter {

class FlutterTizenEngine;

class KeyEventHandler {
 public:
  explicit KeyEventHandler(FlutterTizenEngine* engine);
  virtual ~KeyEventHandler();

 private:
  struct PendingResponse {
    std::function<void(bool, uint64_t)> callback;
    uint64_t response_id;
  };

  // Information for key events that have been sent to the framework but yet
  // to receive the response. Indexed by response IDs.
  std::map<uint64_t, std::unique_ptr<PendingResponse>> pending_responses_;

  // A self-incrementing integer, used as the ID for the next entry for
  // |pending_responses_|.
  uint64_t response_id_ = 0;

  FlutterTizenEngine* engine_;
  std::vector<Ecore_Event_Handler*> key_event_handlers_;

  static Eina_Bool OnKey(void* data, int type, void* event);
};

}  // namespace flutter

#endif  // EMBEDDER_KEY_EVENT_HANDLER_H_
