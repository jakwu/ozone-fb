// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_

#include "frame_buffer.h"

#include "base/macros.h"
#include "ui/ozone/public/surface_factory_ozone.h"

#include <string>

namespace ui {

  class PlatformWindowManager;

class SurfaceFactoryFb : public SurfaceFactoryOzone {
 public:
  SurfaceFactoryFb();
  explicit SurfaceFactoryFb(PlatformWindowManager* window_manager);
  ~SurfaceFactoryFb() override;

  // Initialize (mainly check that we have a place to write output to).
  void Initialize(const std::string& fb_dev);

  // SurfaceFactoryOzone:
  std::unique_ptr<SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget w) override;
  bool LoadEGLGLES2Bindings(
      AddGLLibraryCallback add_gl_library,
      SetGLGetProcAddressProcCallback set_gl_get_proc_address) override;
  scoped_refptr<NativePixmap> CreateNativePixmap(
      gfx::AcceleratedWidget widget,
      gfx::Size size,
      gfx::BufferFormat format,
      gfx::BufferUsage usage) override;

 private:
  PlatformWindowManager* window_manager_;
  std::unique_ptr<FrameBuffer> frameBuffer_;

  DISALLOW_COPY_AND_ASSIGN(SurfaceFactoryFb);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
