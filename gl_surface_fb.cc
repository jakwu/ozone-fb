// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gl_surface_fb.h"

#include "ui/ozone/common/egl_util.h"


namespace ui {


GLSurfaceFb::GLSurfaceFb(gfx::AcceleratedWidget widget, EGLNativeWindowType native_window)
    : gl::NativeViewGLSurfaceEGL(native_window, nullptr),
      widget_(widget) {
}

GLSurfaceFb::~GLSurfaceFb() {
  Destroy();
}

bool GLSurfaceFb::SupportsSwapBuffersWithBounds() {
  return false;
}

//gfx::SwapResult GLSurfaceFb::SwapBuffersWithBounds(const std::vector<gfx::Rect>& rects,
//                                      PresentationCallback callback) {
//}

bool GLSurfaceFb::Resize(const gfx::Size& size,
            float scale_factor,
            ColorSpace color_space,
            bool has_alpha) {
  return true;
}

bool GLSurfaceFb::ScheduleOverlayPlane(int z_order,
                          gfx::OverlayTransform transform,
                          gl::GLImage* image,
                          const gfx::Rect& bounds_rect,
                          const gfx::RectF& crop_rect,
                          bool enable_blend,
                          std::unique_ptr<gfx::GpuFence> gpu_fence) {
  // Currently the Ozone-Fb platform doesn't use the gpu_fence, so we don't
  // propagate it further. If this changes we will need to store the gpu fence
  // to ensure it stays valid for as long as the operation needs it, and pass a
  // pointer to the fence in the call below.
  return image->ScheduleOverlayPlane(widget_, z_order, transform, bounds_rect,
                                     crop_rect, enable_blend,
                                     /* gpu_fence */ nullptr);
}

EGLConfig GLSurfaceFb::GetConfig() {
  if (!config_) {
    EGLint config_attribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
      EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
      EGL_NONE
    };
    config_ = ChooseEGLConfig(GetDisplay(), config_attribs);
  }
  return config_;
}

int GLSurfaceFb::GetBufferCount() const {
  return 1;
}


}  // namespace ui
