// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
#define UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_


#include "base/macros.h"
#include "ui/gfx/geometry/size.h"
#include "ui/ozone/public/gl_ozone.h"
#include "ui/ozone/public/surface_factory_ozone.h"


namespace ui {

  class GLOzoneEglFb;
  class EglPlatform;

class SurfaceFactoryFb : public SurfaceFactoryOzone {
 public:
  SurfaceFactoryFb();
  explicit SurfaceFactoryFb(std::shared_ptr<EglPlatform> egl_platform);
  ~SurfaceFactoryFb() override;

  // SurfaceFactoryOzone:
  std::vector<gl::GLImplementation> GetAllowedGLImplementations() override;
  GLOzone* GetGLOzone(gl::GLImplementation implementation) override;
  std::unique_ptr<SurfaceOzoneCanvas> CreateCanvasForWidget(
      gfx::AcceleratedWidget widget,
      base::TaskRunner* task_runner) override;
  scoped_refptr<gfx::NativePixmap> CreateNativePixmap(
      gfx::AcceleratedWidget widget,
      VkDevice vk_device,
      gfx::Size size,
      gfx::BufferFormat format,
      gfx::BufferUsage usage) override;

 private:
  std::unique_ptr<GLOzoneEglFb> egl_implementation_;

  DISALLOW_COPY_AND_ASSIGN(SurfaceFactoryFb);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_SURFACE_FACTORY_FB_H_
