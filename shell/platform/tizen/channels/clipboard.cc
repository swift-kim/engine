// Copyright 2021 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "clipboard.h"

#include "flutter/shell/platform/common/json_method_codec.h"

namespace flutter {

// Clipboard constants and variables
namespace clipboard {

// naive implementation using std::string as a container of internal clipboard
// data
std::string string_clipboard = "";

static constexpr char kTextKey[] = "text";
static constexpr char kTextPlainFormat[] = "text/plain";
static constexpr char kUnknownClipboardFormatError[] =
    "Unknown clipboard format error";
static constexpr char kUnknownClipboardError[] =
    "Unknown error during clipboard data retrieval";

void GetData(const MethodCall<rapidjson::Document>& call,
             std::unique_ptr<MethodResult<rapidjson::Document>> result) {
  const rapidjson::Value& format = call.arguments()[0];

  // https://api.flutter.dev/flutter/services/Clipboard/kTextPlain-constant.html
  // API supports only kTextPlain format
  if (strcmp(format.GetString(), kTextPlainFormat) != 0) {
    result->Error(kUnknownClipboardFormatError,
                  "Clipboard API only supports text.");
    return;
  }

  rapidjson::Document document;
  document.SetObject();
  rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
  document.AddMember(rapidjson::Value(kTextKey, allocator),
                     rapidjson::Value(string_clipboard, allocator), allocator);
  result->Success(document);
}

void SetData(const MethodCall<rapidjson::Document>& call,
             std::unique_ptr<MethodResult<rapidjson::Document>> result) {
  const rapidjson::Value& document = *call.arguments();
  rapidjson::Value::ConstMemberIterator itr = document.FindMember(kTextKey);
  if (itr == document.MemberEnd()) {
    result->Error(kUnknownClipboardError, "Invalid message format");
    return;
  }
  string_clipboard = itr->value.GetString();
  result->Success();
}

}  // namespace clipboard

}  // namespace flutter
