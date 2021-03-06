// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_renderer.h"

TizenRenderer::TizenRenderer(TizenRenderer::Delegate& delegate)
    : delegate_(delegate) {}

TizenRenderer::~TizenRenderer() = default;
