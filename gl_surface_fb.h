// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_FB_GL_SURFACE_FB_H_
#define UI_OZONE_PLATFORM_FB_GL_SURFACE_FB_H_

#include "base/macros.h"
#include "ui/gl/gl_surface_egl.h"

namespace ui {

class GLSurfaceFb : public gl::NativeViewGLSurfaceEGL {
 public:
  GLSurfaceFb(gfx::AcceleratedWidget widget, EGLNativeWindowType native_window);

  // gl::GLSurface:
  bool SupportsSwapBuffersWithBounds() override;
  //gfx::SwapResult SwapBuffersWithBounds(const std::vector<gfx::Rect>& rects,
  //                                      PresentationCallback callback) override;
  bool Resize(const gfx::Size& size,
              float scale_factor,
              ColorSpace color_space,
              bool has_alpha) override;
  bool ScheduleOverlayPlane(int z_order,
                            gfx::OverlayTransform transform,
                            gl::GLImage* image,
                            const gfx::Rect& bounds_rect,
                            const gfx::RectF& crop_rect,
                            bool enable_blend,
                            std::unique_ptr<gfx::GpuFence> gpu_fence) override;
  EGLConfig GetConfig() override;
  int GetBufferCount() const override;

 protected:
  ~GLSurfaceFb() override;

  gfx::AcceleratedWidget widget_;

 private:
  DISALLOW_COPY_AND_ASSIGN(GLSurfaceFb);
};


}  // namespace ui

#endif  // UI_OZONE_PLATFORM_FB_GL_SURFACE_FB_H_
