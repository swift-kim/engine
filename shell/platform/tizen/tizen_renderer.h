// Copyright 2020 Samsung Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EMBEDDER_TIZEN_RENDERER_H
#define EMBEDDER_TIZEN_RENDERER_H

#ifdef TIZEN_RENDERER_EVAS_GL
#undef EFL_BETA_API_SUPPORT
#include <Ecore.h>
#include <Elementary.h>
#else
#include <EGL/egl.h>
#endif

class TizenRenderer {
 public:
  struct TizenWindowGeometry {
    int32_t x{0}, y{0}, w{0}, h{0};
  };

  class Delegate {
   public:
    virtual void OnRotationChange(int angle) = 0;
  };

  TizenRenderer(TizenRenderer::Delegate& deleget);
  virtual ~TizenRenderer();

#ifdef TIZEN_RENDERER_EVAS_GL
  void ClearColor(float r, float g, float b, float a);
#endif
  bool OnMakeCurrent();
  bool OnClearCurrent();
  bool OnMakeResourceCurrent();
  bool OnPresent();
  uint32_t OnGetFBO();
  void* OnProcResolver(const char* name);
  void flush();
  virtual TizenWindowGeometry GetGeometry() = 0;
  bool IsValid();
  virtual void Show() = 0;
  virtual void SetRotate(int angle) = 0;
  virtual int GetEcoreWindowId() = 0;
  virtual void ResizeWithRotation(int32_t x, int32_t y, int32_t width,
                                  int32_t height, int32_t degree) = 0;

 protected:
  bool received_rotation{false};
  TizenRenderer::Delegate& delegate_;
  bool InitializeRenderer(int32_t x, int32_t y, int32_t w, int32_t h);
#ifdef TIZEN_RENDERER_EVAS_GL
  virtual void* SetupEvasWindow(int32_t x, int32_t y, int32_t w, int32_t h) = 0;
  virtual void DestoryEvasWindow() = 0;

 public:
  virtual void* GetImageHandle() = 0;

 protected:
  bool SetupEvasGL(int32_t x, int32_t y, int32_t w, int32_t h);
  void DestoryEvasGL();
#else
  virtual bool SetupDisplay() = 0;
  virtual bool SetupEcoreWlWindow(int32_t x, int32_t y, int32_t w,
                                  int32_t h) = 0;
  virtual bool SetupEglWindow(int32_t w, int32_t h) = 0;
  virtual EGLDisplay GetEGLDisplay() = 0;
  virtual EGLNativeWindowType GetEGLNativeWindowType() = 0;
  virtual void DestoryEglWindow() = 0;
  virtual void DestoryEcoreWlWindow() = 0;
  virtual void ShutdownDisplay() = 0;

  void DestoryEglSurface();
  bool SetupEglSurface();
#endif
  void DestoryRenderer();
  virtual void SendRotationChangeDone() = 0;

 private:
  bool is_valid_ = false;
#ifdef TIZEN_RENDERER_EVAS_GL
  Evas_GL_Config* gl_config_;
  Evas_GL* evas_gl_{nullptr};

  Evas_GL_Context* gl_context_;
  Evas_GL_Context* gl_resource_context_;

  Evas_GL_Surface* gl_surface_{nullptr};
  Evas_GL_Surface* gl_resource_surface_{nullptr};
#else
  bool ChooseEGLConfiguration();
  void PrintEGLError();

  EGLConfig egl_config_;
  EGLDisplay egl_display_ = EGL_NO_DISPLAY;
  EGLContext egl_context_ = EGL_NO_CONTEXT;
  EGLSurface egl_surface_ = EGL_NO_SURFACE;
  EGLContext egl_resource_context_ = EGL_NO_CONTEXT;
  EGLSurface egl_resource_surface_ = EGL_NO_SURFACE;
#endif
};

#endif  // EMBEDDER_TIZEN_RENDERER_H
