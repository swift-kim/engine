// Copyright 2021 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_log.h"

#include <dlfcn.h>
#include <unistd.h>

namespace flutter {

#ifndef __X64_SHELL__
bool Logger::Init() {
  // We cannot use dlog_print() for logging because any logs with log id
  // LOG_ID_APPS are ignored on TV devices. There is an internal function
  // __dlog_print() which allows a log id to be explicitly set, but the use of
  // such undocumented API is not allowed for apps published on the app store.
  // One way to effectively bypass the API verification is to obtain the
  // function's address from a library loaded in memory at runtime. Since the
  // function should be always available for use while the app is running,
  // dlclose() must not be called on the library until the process terminates.
  void* handle = dlopen("libdlog.so.0", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "Failed to load a lib: %s\n", dlerror());
    return false;
  }
  *(void**)(&dlog_vprint_) = dlsym(handle, "__dlog_vprint");
  if (!dlog_vprint_) {
    fprintf(stderr, "Symbol not found: %s\n", dlerror());
    dlclose(handle);
    return false;
  }
}

void* Logger::Redirect(void* arg) {
  int* pipe = static_cast<int*>(arg);
  ssize_t size;
  char buffer[1024];

  while ((size = read(pipe[0], buffer, sizeof(buffer) - 1)) > 0) {
    buffer[size] = 0;
    if (pipe == stdout_pipe_) {
      FML_LOG(ERROR) << std::string(buffer);
    } else {
      FML_LOG(INFO) << std::string(buffer);
    }
  }

  close(pipe[0]);
  close(pipe[1]);

  return nullptr;
}

bool Logger::Start() {
  if (started_) {
    FT_LOGI("The threads are already running.");
    return;
  }
  if (pipe(stdout_pipe_) < 0 || pipe(stderr_pipe_) < 0) {
    FT_LOGE("Failed to create pipes.");
    return;
  }
  if (dup2(stdout_pipe_[1], 1) < 0 || dup2(stderr_pipe_[1], 2) < 0) {
    FT_LOGE("Failed to duplicate file descriptors.");
    return;
  }
  if (pthread_create(&stdout_thread_, 0, Redirect, stdout_pipe_) != 0 ||
      pthread_create(&stderr_thread_, 0, Redirect, stderr_pipe_) != 0) {
    FT_LOGE("Failed to create threads.");
    return;
  }
  if (pthread_detach(stdout_thread_) != 0 ||
      pthread_detach(stderr_thread_) != 0) {
    FT_LOGW("Failed to detach threads.");
  }
  started_ = true;
}
#endif

void Logger::Print(int priority, const char* format, ...) {
  va_list args;
  va_start(args, format);

  if (dlog_vprint_) {
    dlog_vprint_(LOG_ID_MAIN, priority, tag, format, args);
  } else {
    fprintf(priority == DLOG_ERROR ? stderr : stdout, format, args);
  }

  va_end(args);
}

}  // namespace flutter
