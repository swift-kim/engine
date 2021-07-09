// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_LOG_H_
#define EMBEDDER_TIZEN_LOG_H_

#include <pthread.h>

#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

#ifndef __X64_SHELL__
#include <dlog.h>
#endif

#include "flutter/fml/logging.h"

namespace flutter {

class Logger {
 public:
#ifndef __X64_SHELL__
  // Initializes the logging facility.
  // This must be called before any call to Print() is made.
  static bool Init();

  // Starts logging threads which constantly redirect stdout/stderr to dlog,
  // if applicable.
  // The threads should be started only once per process.
  static bool Start();
#endif

  // Prints a log message to either dlog or console.
  static void Print(const char* message);

 private:
  explicit Logger();

#ifndef __X64_SHELL__
  static void* Redirect(void* arg);

  static inline int (
      *dlog_vprint_)(log_id_t, int, const char*, const char*, va_list);
  static inline int stdout_pipe_[2];
  static inline int stderr_pipe_[2];
  static inline pthread_t stdout_thread_;
  static inline pthread_t stderr_thread_;
  static inline bool started_ = false;
#endif
};

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
