// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_FB_GL_OZONE_EGL_FB_H_
#define UI_OZONE_PLATFORM_FB_GL_OZONE_EGL_FB_H_

#include "ui/ozone/common/gl_ozone_egl.h"
#include "base/macros.h"

#include <memory>

namespace ui {

class EglPlatform;

// GL implementation using EGL for Ozone fb platform.
class GLOzoneEglFb : public GLOzoneEGL {
 public:
  GLOzoneEglFb(std::shared_ptr<EglPlatform> egl_platform)
      : egl_platform_(egl_platform)
      , native_display_(0)
      , native_window_(0) {
  }
  ~GLOzoneEglFb() override {
  }

  // GLOzoneEGL implementation:
  scoped_refptr<gl::GLSurface> CreateViewGLSurface(
      gfx::AcceleratedWidget widget) override;
  scoped_refptr<gl::GLSurface> CreateOffscreenGLSurface(
      const gfx::Size& size) override;
  EGLNativeDisplayType GetNativeDisplay() override;
  bool LoadGLES2Bindings(gl::GLImplementation implementation) override;

  EGLNativeWindowType GetNativeWindow(gfx::AcceleratedWidget window_id);
  int GetDisplayIndex();

 private:
  std::shared_ptr<EglPlatform> egl_platform_;
  EGLNativeDisplayType native_display_;
  EGLNativeWindowType native_window_;

  DISALLOW_COPY_AND_ASSIGN(GLOzoneEglFb);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_FB_GL_OZONE_EGL_FB_H_
