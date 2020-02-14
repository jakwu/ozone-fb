// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "surface_factory_fb.h"
#include "platform_window_fb.h"
#include "egl_platform.h"
#include "gl_ozone_egl_fb.h"
#include "frame_buffer.h"

#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/files/file_util.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/gfx/buffer_format_util.h"
#include "ui/gfx/native_pixmap.h"
#include "ui/ozone/public/surface_ozone_canvas.h"


namespace ui {

namespace {

const char kRendereWaitFor[] = "renderer-wait-for";

//=============================================================================
// SurfaceOzoneCanvasFb: Software rendering surface
//=============================================================================

class SurfaceOzoneCanvasFb : public SurfaceOzoneCanvas {
 public:
  SurfaceOzoneCanvasFb(const std::string& fb_device)
      : renderer_enabled_(false) {
    frame_buffer_.reset(new FrameBuffer());
    frame_buffer_->Initialize(fb_device);
    DCHECK(frame_buffer_->GetDataSize());

    base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
    if (cmd->HasSwitch(kRendereWaitFor)) {
      renderer_wait_for_ = cmd->GetSwitchValueASCII(kRendereWaitFor);
    }
    renderer_enabled_ = renderer_wait_for_.empty();
    if (!renderer_enabled_) {
      LOG(INFO) << "Software renderer is disabled and is waiting for '"
        << renderer_wait_for_ << "'";
    }
  }
  ~SurfaceOzoneCanvasFb() override {}

  // SurfaceOzoneCanvas overrides:
  void ResizeCanvas(const gfx::Size& viewport_size) override {
    surface_ = SkSurface::MakeRaster(SkImageInfo::MakeN32Premul(
        viewport_size.width(), viewport_size.height()));
  }
  sk_sp<SkSurface> GetSurface() override { return surface_; }
  void PresentCanvas(const gfx::Rect& damage) override {
    if (!renderer_enabled_) {
      base::FilePath path(renderer_wait_for_);
      renderer_enabled_ = base::PathExists(path);
    }
    if (renderer_enabled_) {
      SkImageInfo info = frame_buffer_->GetImageInfo();
      if (!surface_->getCanvas()->readPixels(
          info, frame_buffer_->GetData(),
          frame_buffer_->GetDataSize() / info.height(),
          0, 0)) {
        LOG(ERROR) << "Failed to read pixel data";
      }
    }
  }
  std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return nullptr;
  }

 private:
  std::unique_ptr<FrameBuffer> frame_buffer_;
  sk_sp<SkSurface> surface_;
  bool renderer_enabled_;
  std::string renderer_wait_for_;
};


//=============================================================================
// TestPixmap
//=============================================================================

class TestPixmap : public gfx::NativePixmap {
 public:
  explicit TestPixmap(gfx::BufferFormat format) : format_(format) {}

  bool AreDmaBufFdsValid() const override { return false; }
  int GetDmaBufFd(size_t plane) const override { return -1; }
  uint32_t GetDmaBufPitch(size_t plane) const override { return 0; }
  size_t GetDmaBufOffset(size_t plane) const override { return 0; }
  size_t GetDmaBufPlaneSize(size_t plane) const override { return 0; }
  uint64_t GetBufferFormatModifier() const override { return 0; }
  gfx::BufferFormat GetBufferFormat() const override { return format_; }
  size_t GetNumberOfPlanes() const override {
    return gfx::NumberOfPlanesForLinearBufferFormat(format_);
  }
  gfx::Size GetBufferSize() const override { return gfx::Size(); }
  uint32_t GetUniqueId() const override { return 0; }
  bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                            int plane_z_order,
                            gfx::OverlayTransform plane_transform,
                            const gfx::Rect& display_bounds,
                            const gfx::RectF& crop_rect,
                            bool enable_blend,
                            std::unique_ptr<gfx::GpuFence> gpu_fence) override {
    return true;
  }
  gfx::NativePixmapHandle ExportHandle() override {
    return gfx::NativePixmapHandle();
  }

 private:
  ~TestPixmap() override {}

  gfx::BufferFormat format_;

  DISALLOW_COPY_AND_ASSIGN(TestPixmap);
};

}  // namespace


//=============================================================================
// SurfaceFactoryFb
//=============================================================================

SurfaceFactoryFb::SurfaceFactoryFb() : SurfaceFactoryFb(nullptr) {}

SurfaceFactoryFb::SurfaceFactoryFb(std::shared_ptr<EglPlatform> egl_platform) {
  if (egl_platform) {
    egl_implementation_ =
        std::make_unique<GLOzoneEglFb>(egl_platform);
  }
}

SurfaceFactoryFb::~SurfaceFactoryFb() {
}

std::vector<gl::GLImplementation> SurfaceFactoryFb::GetAllowedGLImplementations() {
  std::vector<gl::GLImplementation> impls;
  if (egl_implementation_)
    impls.push_back(gl::kGLImplementationEGLGLES2);
  return impls;
}

GLOzone* SurfaceFactoryFb::GetGLOzone(gl::GLImplementation implementation) {
  switch (implementation) {
    case gl::kGLImplementationEGLGLES2:
      return egl_implementation_.get();
    default:
      break;
  }
  return nullptr;
}

std::unique_ptr<SurfaceOzoneCanvas> SurfaceFactoryFb::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget,
    base::TaskRunner* task_runner) {
  std::stringstream ss;
  ss << "/dev/fb" << egl_implementation_->GetDisplayIndex();
  return base::WrapUnique<SurfaceOzoneCanvas>(new SurfaceOzoneCanvasFb(ss.str()));
}

scoped_refptr<gfx::NativePixmap> SurfaceFactoryFb::CreateNativePixmap(
    gfx::AcceleratedWidget widget,
    VkDevice vk_device,
    gfx::Size size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage) {
  return base::MakeRefCounted<TestPixmap>(format);
}

}  // namespace ui
