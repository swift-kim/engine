// Copyright 2021 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_CLIPBOARD_H_
#define EMBEDDER_CLIPBOARD_H_

#include <memory>

#include "flutter/shell/platform/common/client_wrapper/include/flutter/method_channel.h"
#include "rapidjson/document.h"

namespace flutter {

namespace clipboard {

void GetData(const MethodCall<rapidjson::Document>& call,
             std::unique_ptr<MethodResult<rapidjson::Document>> result);

void SetData(const MethodCall<rapidjson::Document>& call,
             std::unique_ptr<MethodResult<rapidjson::Document>> result);

}  // namespace clipboard

}  // namespace flutter

#endif  // EMBEDDER_CLIPBOARD_H_
