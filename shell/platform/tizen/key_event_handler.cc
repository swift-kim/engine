// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "key_event_handler.h"

#include "flutter/shell/platform/tizen/flutter_tizen_engine.h"

#include "flutter/shell/platform/tizen/tizen_log.h"

static constexpr char kPlatformBackButtonName[] = "XF86Back";

namespace flutter {

KeyEventHandler::KeyEventHandler(FlutterTizenEngine* engine) : engine_(engine) {
  key_event_handlers_.push_back(
      ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, OnKey, this));
  key_event_handlers_.push_back(
      ecore_event_handler_add(ECORE_EVENT_KEY_UP, OnKey, this));
}

KeyEventHandler::~KeyEventHandler() {
  for (auto handler : key_event_handlers_) {
    ecore_event_handler_del(handler);
  }
  key_event_handlers_.clear();
}

Eina_Bool KeyEventHandler::OnKey(void* data, int type, void* event) {
  auto* self = reinterpret_cast<KeyEventHandler*>(data);
  auto* key = reinterpret_cast<Ecore_Event_Key*>(event);
  auto* engine = self->engine_;
  auto is_down = type == ECORE_EVENT_KEY_DOWN;

  if (engine->text_input_channel) {
    if (is_down) {
      engine->text_input_channel->OnKeyDown(key);
    }
    if (engine->text_input_channel->IsSoftwareKeyboardShowing()) {
      return ECORE_CALLBACK_PASS_ON;
    }
  }

  // FlutterKeyEvent event = {};
  // event.struct_size = sizeof(event);
  // event.timestamp = key->timestamp * 1000;
  // event.type = is_down ? kFlutterKeyEventTypeDown : kFlutterKeyEventTypeUp;
  // event.physical = ...;
  // event.logical = ...;
  // event.character = ...;
  // event.synthesized = ...;

  // engine->SendKeyEvent(key_event, ..., ...);

  if (engine->key_event_channel) {
    engine->key_event_channel->SendKeyEvent(
        key, is_down, [engine, is_down](std::string keyname, bool handled) {
          if (!is_down && !handled && keyname == kPlatformBackButtonName) {
            // The device back button was pressed.
            if (engine->navigation_channel) {
              engine->navigation_channel->PopRoute();
            }
          }
        });
  }
  return ECORE_CALLBACK_DONE;
}

}  // namespace flutter
