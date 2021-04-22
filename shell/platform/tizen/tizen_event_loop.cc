// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "tizen_event_loop.h"

#include <atomic>
#include <utility>

#ifdef TIZEN_RENDERER_EVAS_GL
#include <Evas_GL.h>
#endif

#include "flutter/shell/platform/tizen/tizen_log.h"
#include "flutter/shell/platform/tizen/tizen_renderer.h"

TizenEventLoop::TizenEventLoop(std::thread::id main_thread_id,
                               TaskExpiredCallback on_task_expired)
    : main_thread_id_(main_thread_id),
      on_task_expired_(std::move(on_task_expired)) {
  ecore_pipe_ = ecore_pipe_add(ExcuteTaskEvents, this);
}

TizenEventLoop::~TizenEventLoop() {
  if (ecore_pipe_) {
    ecore_pipe_del(ecore_pipe_);
  }
}

bool TizenEventLoop::RunsTasksOnCurrentThread() const {
  return std::this_thread::get_id() == main_thread_id_;
}

void TizenEventLoop::ExcuteTaskEvents(std::chrono::nanoseconds max_wait) {
  const auto now = TaskTimePoint::clock::now();
  {
    std::lock_guard<std::mutex> lock1(task_queue_mutex_);
    std::lock_guard<std::mutex> lock2(expired_tasks_mutex_);
    while (!task_queue_.empty()) {
      const auto& top = task_queue_.top();

      if (top.fire_time > now) {
        break;
      }

      expired_tasks_.push_back(task_queue_.top().task);
      task_queue_.pop();
    }
  }
  OnTaskExpired();
}

TizenEventLoop::TaskTimePoint TizenEventLoop::TimePointFromFlutterTime(
    uint64_t flutter_target_time_nanos) {
  const auto now = TaskTimePoint::clock::now();
  const int64_t flutter_duration =
      flutter_target_time_nanos - FlutterEngineGetCurrentTime();
  return now + std::chrono::nanoseconds(flutter_duration);
}

void TizenEventLoop::PostTask(FlutterTask flutter_task,
                              uint64_t flutter_target_time_nanos) {
  Task task;
  task.order = ++task_order_;
  task.fire_time = TimePointFromFlutterTime(flutter_target_time_nanos);
  task.task = flutter_task;
  if (ecore_pipe_) {
    ecore_pipe_write(ecore_pipe_, &task, sizeof(task));
  }
}

Eina_Bool TizenEventLoop::TaskTimerCallback(void* data) {
  TizenEventLoop* tizenEventLoop = reinterpret_cast<TizenEventLoop*>(data);
  tizenEventLoop->ExcuteTaskEvents();
  return EINA_FALSE;
}

void TizenEventLoop::ExcuteTaskEvents(void* data, void* buffer,
                                      unsigned int nbyte) {
  TizenEventLoop* tizenEventLoop = reinterpret_cast<TizenEventLoop*>(data);
  Task* p_task = reinterpret_cast<Task*>(buffer);

  const double flutter_duration =
      ((double)(p_task->fire_time.time_since_epoch().count()) -
       FlutterEngineGetCurrentTime()) /
      1000000000.0;
  if (flutter_duration > 0) {
    {
      std::lock_guard<std::mutex> lock(tizenEventLoop->task_queue_mutex_);
      tizenEventLoop->task_queue_.push(*p_task);
    }
    ecore_timer_add(flutter_duration, TaskTimerCallback, tizenEventLoop);
  } else {
    {
      std::lock_guard<std::mutex> lock(tizenEventLoop->expired_tasks_mutex_);
      tizenEventLoop->expired_tasks_.push_back(p_task->task);
    }
    tizenEventLoop->OnTaskExpired();
  }
}

TizenPlatformEventLoop::TizenPlatformEventLoop(
    std::thread::id main_thread_id, TaskExpiredCallback on_task_expired)
    : TizenEventLoop(main_thread_id, on_task_expired) {}

TizenPlatformEventLoop::~TizenPlatformEventLoop() {}

void TizenPlatformEventLoop::OnTaskExpired() {
  for (const auto& task : expired_tasks_) {
    on_task_expired_(&task);
  }
  expired_tasks_.clear();
}

#ifdef TIZEN_RENDERER_EVAS_GL
TizenRenderEventLoop::TizenRenderEventLoop(std::thread::id main_thread_id,
                                           TaskExpiredCallback on_task_expired,
                                           TizenRenderer* tizen_renderer)
    : TizenEventLoop(main_thread_id, on_task_expired),
      tizen_renderer_(tizen_renderer) {
  evas_object_image_pixels_get_callback_set(
      (Evas_Object*)tizen_renderer_->GetImageHandle(),
      [](void* data, Evas_Object* o) {  // Render call back
        TizenRenderEventLoop* self = (TizenRenderEventLoop*)data;
        {
          std::lock_guard<std::mutex> lock(self->expired_tasks_mutex_);
          for (const auto& task : self->expired_tasks_) {
            self->on_task_expired_(&task);
          }
          self->expired_tasks_.clear();
        }
        self->has_pending_renderer_callback_ = false;
      },
      this);
}

TizenRenderEventLoop::~TizenRenderEventLoop() {}

void TizenRenderEventLoop::OnTaskExpired() {
  size_t expired_tasks_count = 0;
  std::lock_guard<std::mutex> lock(expired_tasks_mutex_);
  expired_tasks_count = expired_tasks_.size();
  if (has_pending_renderer_callback_ == false && expired_tasks_count) {
    evas_object_image_pixels_dirty_set(
        (Evas_Object*)tizen_renderer_->GetImageHandle(), EINA_TRUE);
    has_pending_renderer_callback_ = true;
  } else {
    // Do nothing
    FT_LOGD("Ignore set dirty");
  }
}
#endif
