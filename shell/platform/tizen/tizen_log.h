// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_LOG_H_
#define EMBEDDER_TIZEN_LOG_H_

#ifndef __X64_SHELL__
#include <dlog.h>
#else
#define log_priority int
#define DLOG_DEBUG 0
#define DLOG_WARN 1
#define DLOG_INFO 2
#define DLOG_ERROR 3
#endif

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "flutter/fml/logging.h"

namespace flutter {

// Starts logging threads which constantly redirect stdout/stderr to dlog.
// The threads can be started only once per process.
void StartLogging();

#define __FT_LOG(severity, fmt, args...)      \
  do {                                        \
    char buffer[1024];                        \
    sprintf(buffer, fmt, ##args);             \
    FML_LOG(severity) << std::string(buffer); \
  } while (0)

#define FT_LOGD(fmt, args...) __FT_LOG(DEBUG, fmt, ##args)
#define FT_LOGI(fmt, args...) __FT_LOG(INFO, fmt, ##args)
#define FT_LOGW(fmt, args...) __FT_LOG(WARNING, fmt, ##args)
#define FT_LOGE(fmt, args...) __FT_LOG(ERROR, fmt, ##args)

#if defined(NDEBUG)
#define FT_ASSERT(assertion) ((void)0)
#define FT_ASSERT_NOT_REACHED() ((void)0)
#define FT_ASSERT_STATIC(assertion, reason)
#else
#define FT_ASSERT(assertion) assert(assertion);
#define FT_ASSERT_NOT_REACHED() \
  do {                          \
    assert(false);              \
  } while (0)
#define FT_ASSERT_STATIC(assertion, reason) static_assert(assertion, reason)
#endif

#define FT_RELEASE_ASSERT(assertion) \
  do {                               \
    if (!(assertion)) {              \
      FT_LOGE("RELEASE_ASSERT");     \
      abort();                       \
    }                                \
  } while (0)

#define FT_RELEASE_ASSERT_NOT_REACHED()    \
  do {                                     \
    FT_LOGE("RELEASE_ASSERT_NOT_REACHED"); \
    abort();                               \
  } while (0)

/* COMPILE_ASSERT */
#ifndef FT_COMPILE_ASSERT
#define FT_COMPILE_ASSERT(exp, name) static_assert((exp), #name)
#endif

#define FT_UNIMPLEMENTED() FT_LOGW("UNIMPLEMENTED!")

}  // namespace flutter

#endif  // EMBEDDER_TIZEN_LOG_H_
