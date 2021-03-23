// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_LOG_H_
#define EMBEDDER_TIZEN_LOG_H_

#include <dlog.h>

#include <cassert>
#include <cstdarg>
#include <cstdlib>

#undef __LOG
#define __LOG(prio, fmt, args...) \
  Logger::Print(prio, "%s: %s(%d) > " fmt, __FILE__, __func__, __LINE__, ##args)

#define FT_LOGD(fmt, args...) __LOG(DLOG_DEBUG, fmt, ##args)
#define FT_LOGI(fmt, args...) __LOG(DLOG_INFO, fmt, ##args)
#define FT_LOGW(fmt, args...) __LOG(DLOG_WARN, fmt, ##args)
#define FT_LOGE(fmt, args...) __LOG(DLOG_ERROR, fmt, ##args)

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

#define FT_UNIMPLEMENTED() FT_LOGD("UNIMPLEMENTED!")

class Logger {
 public:
  // Initializes the logging facility.
  // This must be called before any call to Print() is made.
  static bool Init();

  // Prints a message to dlog output, or stdout/stderr if dlog is unavailable.
  static void Print(int priority, const char* format, ...);

  // "ConsoleMessage" is the only valid log tag for TV devices.
  static inline const char* tag = "ConsoleMessage";

 private:
  explicit Logger();

  static void* Redirect(void* arg);

  static inline int (*dlog_vprint_)(log_id_t, int, const char*, const char*,
                                    va_list);
  static inline int stdout_pipe_[2];
  static inline int stderr_pipe_[2];
  static inline pthread_t stdout_thread_;
  static inline pthread_t stderr_thread_;
  static inline bool started_ = false;
};

#endif  // EMBEDDER_TIZEN_LOG_H_
