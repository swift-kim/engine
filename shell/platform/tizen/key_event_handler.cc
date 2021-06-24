// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "key_event_handler.h"

#include <app.h>

#include "flutter/shell/platform/tizen/flutter_tizen_engine.h"
#include "flutter/shell/platform/tizen/keyboard_map_linux.h"
#include "flutter/shell/platform/tizen/tizen_log.h"

namespace flutter {

namespace {

constexpr char kBackKey[] = "XF86Back";
constexpr char kExitKey[] = "XF86Exit";

// The code prefix for unrecognized keys that are unique to Gtk, generated from
// platform-specific codes.
constexpr uint64_t kGtkKeyIdPlane = 0x00600000000;

// Converts the XKB scan code to the corresponding USB HID code.
uint64_t GetPhysicalKey(int scan_code) {
  auto iter = g_xkb_to_physical_key.find(scan_code);
  if (iter != g_xkb_to_physical_key.end()) {
    return iter->second;
  }
  return scan_code | kGtkKeyIdPlane;
}

const std::map<int, int> xkb_to_logical_key = {
    {0x00000026, 0x00000000061},  // keyA
    {0x00000038, 0x00000000062},  // keyB
    {0x00000036, 0x00000000063},  // keyC
    {0x00000028, 0x00000000064},  // keyD
    {0x0000001a, 0x00000000065},  // keyE
    {0x00000029, 0x00000000066},  // keyF
    {0x0000002a, 0x00000000067},  // keyG
    {0x0000002b, 0x00000000068},  // keyH
    {0x0000001f, 0x00000000069},  // keyI
    {0x0000002c, 0x0000000006a},  // keyJ
    {0x0000002d, 0x0000000006b},  // keyK
    {0x0000002e, 0x0000000006c},  // keyL
    {0x0000003a, 0x0000000006d},  // keyM
    {0x00000039, 0x0000000006e},  // keyN
    {0x00000020, 0x0000000006f},  // keyO
    {0x00000021, 0x00000000070},  // keyP
    {0x00000018, 0x00000000071},  // keyQ
    {0x0000001b, 0x00000000072},  // keyR
    {0x00000027, 0x00000000073},  // keyS
    {0x0000001c, 0x00000000074},  // keyT
    {0x0000001e, 0x00000000075},  // keyU
    {0x00000037, 0x00000000076},  // keyV
    {0x00000019, 0x00000000077},  // keyW
    {0x00000035, 0x00000000078},  // keyX
    {0x0000001d, 0x00000000079},  // keyY
    {0x00000034, 0x0000000007a},  // keyZ
    {0x0000000a, 0x00000000031},  // digit1
};

uint64_t GetLogicalKey(int scan_code) {
  auto iter = xkb_to_logical_key.find(scan_code);
  if (iter != xkb_to_logical_key.end()) {
    return iter->second;
  }
  return 0;
}

}  // namespace

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

  FT_LOGI("Keycode: %d, name: %s, mods: %d, is_down: %d", key->keycode,
          key->keyname, key->modifiers, is_down);

  if (engine->text_input_channel) {
    if (is_down) {
      engine->text_input_channel->OnKeyDown(key);
    }
    if (engine->text_input_channel->IsSoftwareKeyboardShowing()) {
      return ECORE_CALLBACK_PASS_ON;
    }
  }

  FlutterKeyEvent key_event = {};
  key_event.struct_size = sizeof(key_event);
  key_event.timestamp = key->timestamp * 1000;
  key_event.type = is_down ? kFlutterKeyEventTypeDown : kFlutterKeyEventTypeUp;
  key_event.physical = GetPhysicalKey(key->keycode);
  key_event.logical = GetLogicalKey(key->keycode);
  key_event.character = key->key;
  key_event.synthesized = false;

  FT_LOGI("timestamp: %d, key: %s, physical: %llu, logical: %llu",
          key->timestamp, key->key, key_event.physical, key_event.logical);

  self->response_id_ += 1;
  uint64_t response_id = self->response_id_;
  PendingResponse pending{
      .callback =
          [self, engine, keyname = std::string(key->keyname), is_down](
              bool handled, uint64_t response_id) {
            auto found = self->pending_responses_.find(response_id);
            if (found != self->pending_responses_.end()) {
              self->pending_responses_.erase(found);
            }
            if (!handled && !is_down) {
              if (keyname == kBackKey) {
                if (engine->navigation_channel) {
                  engine->navigation_channel->PopRoute();
                }
              } else if (keyname == kExitKey) {
                ui_app_exit();
              }
            }
          },
      .response_id = response_id,
  };
  auto pending_ptr = std::make_unique<PendingResponse>(std::move(pending));
  self->pending_responses_[response_id] = std::move(pending_ptr);

  engine->SendKeyEvent(
      key_event,
      [](bool handled, void* user_data) {
        PendingResponse* pending =
            reinterpret_cast<PendingResponse*>(user_data);
        auto callback = std::move(pending->callback);
        callback(handled, pending->response_id);
      },
      reinterpret_cast<void*>(self->pending_responses_[response_id].get()));

  return ECORE_CALLBACK_PASS_ON;
}

}  // namespace flutter
