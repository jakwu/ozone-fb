// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_


#include "base/macros.h"
#include "ui/ozone/public/surface_factory_ozone.h"
#include "base/threading/thread_checker.h"

#include <string>

namespace ui {

  class EglPlatform;

class SurfaceFactoryFb : public SurfaceFactoryOzone {
 public:
  explicit SurfaceFactoryFb(std::shared_ptr<EglPlatform> egl_platform);
  ~SurfaceFactoryFb() override;

  // SurfaceFactoryOzone:
  std::unique_ptr<SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget w) override;

  intptr_t GetNativeDisplay() override;
  std::unique_ptr<SurfaceOzoneEGL> CreateEGLSurfaceForWidget(
      gfx::AcceleratedWidget widget) override;
  bool LoadEGLGLES2Bindings(
      AddGLLibraryCallback add_gl_library,
      SetGLGetProcAddressProcCallback set_gl_get_proc_address) override;
  scoped_refptr<NativePixmap> CreateNativePixmap(
      gfx::AcceleratedWidget widget,
      gfx::Size size,
      gfx::BufferFormat format,
      gfx::BufferUsage usage) override;


  void SetBounds(const gfx::Rect& bounds) { bounds_ = bounds; }
  intptr_t GetNativeWindow(gfx::AcceleratedWidget window_id);

 private:
  std::shared_ptr<EglPlatform> egl_platform_;
  intptr_t native_display_;
  intptr_t native_window_;
  gfx::Rect bounds_;
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(SurfaceFactoryFb);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
