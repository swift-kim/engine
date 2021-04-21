// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_RENDERER_H
#define EMBEDDER_TIZEN_RENDERER_H

#include <cstdint>

class TizenRenderer {
 public:
  struct TizenWindowGeometry {
    int32_t x{0}, y{0}, w{0}, h{0};
  };

  class Delegate {
   public:
    virtual void OnRotationChange(int angle) = 0;
  };

  virtual ~TizenRenderer();

  bool IsValid() { return is_valid_; }

  virtual bool OnMakeCurrent() = 0;
  virtual bool OnClearCurrent() = 0;
  virtual bool OnMakeResourceCurrent() = 0;
  virtual bool OnPresent() = 0;
  virtual uint32_t OnGetFBO() = 0;
  virtual void* OnProcResolver(const char* name) = 0;

  virtual TizenWindowGeometry GetGeometry() = 0;
  virtual int GetEcoreWindowId() = 0;

  // TODO: For Evas GL only.
  virtual void* GetImageHandle() { return nullptr; };
  virtual void ClearColor(float r, float g, float b, float a){};

  virtual void SetRotate(int angle) = 0;
  virtual void ResizeWithRotation(int32_t x, int32_t y, int32_t width,
                                  int32_t height, int32_t degree) = 0;

 protected:
  explicit TizenRenderer(TizenRenderer::Delegate& delegate);

  virtual bool InitializeRenderer(int32_t x, int32_t y, int32_t w,
                                  int32_t h) = 0;
  virtual void Show() = 0;
  virtual void DestroyRenderer() = 0;

  virtual void SendRotationChangeDone() = 0;

  bool is_valid_ = false;

  bool received_rotation{false};
  TizenRenderer::Delegate& delegate_;
};

#endif  // EMBEDDER_TIZEN_RENDERER_H
