// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "surface_factory_fb.h"
#include "platform_window_fb.h"
#include "platform_window_manager.h"

#include "base/memory/ptr_util.h"
#include "base/threading/worker_pool.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/ozone/public/native_pixmap.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {

namespace {

class FbSurface : public SurfaceOzoneCanvas {
 public:
  FbSurface(FrameBuffer* framebuffer) : framebuffer_(framebuffer) {}
  ~FbSurface() override {}

  // SurfaceOzoneCanvas overrides:
  void ResizeCanvas(const gfx::Size& viewport_size) override {
    surface_ = SkSurface::MakeRaster(SkImageInfo::MakeN32Premul(
        viewport_size.width(), viewport_size.height()));
  }
  sk_sp<SkSurface> GetSurface() override { return surface_; }
  void PresentCanvas(const gfx::Rect& damage) override {
    SkImageInfo info = framebuffer_->GetImageInfo();
    if (!surface_->getCanvas()->readPixels(info, framebuffer_->GetData(), framebuffer_->GetDataSize() / info.height(), 0, 0)) {
      LOG(ERROR) << "Failed to read pixel data";
    }
  }
  std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return nullptr;
  }

 private:
  FrameBuffer* framebuffer_;
  sk_sp<SkSurface> surface_;
};

class TestPixmap : public ui::NativePixmap {
 public:
  TestPixmap(gfx::BufferFormat format) : format_(format) {}

  void* GetEGLClientBuffer() const override { return nullptr; }
  int GetDmaBufFd() const override { return -1; }
  int GetDmaBufPitch() const override { return 0; }
  gfx::BufferFormat GetBufferFormat() const override { return format_; }
  gfx::Size GetBufferSize() const override { return gfx::Size(); }
  bool ScheduleOverlayPlane(gfx::AcceleratedWidget widget,
                            int plane_z_order,
                            gfx::OverlayTransform plane_transform,
                            const gfx::Rect& display_bounds,
                            const gfx::RectF& crop_rect) override {
    return true;
  }
  void SetProcessingCallback(
      const ProcessingCallback& processing_callback) override {}
  gfx::NativePixmapHandle ExportHandle() override {
    return gfx::NativePixmapHandle();
  }

 private:
  ~TestPixmap() override {}

  gfx::BufferFormat format_;

  DISALLOW_COPY_AND_ASSIGN(TestPixmap);
};

}  // namespace

SurfaceFactoryFb::SurfaceFactoryFb() : SurfaceFactoryFb(nullptr) {
}

SurfaceFactoryFb::SurfaceFactoryFb(PlatformWindowManager* window_manager)
  : window_manager_(window_manager) {
}

SurfaceFactoryFb::~SurfaceFactoryFb() {
}

void SurfaceFactoryFb::Initialize(const std::string& fb_dev) {
  if (!frameBuffer_) {
    frameBuffer_.reset(new FrameBuffer());
    frameBuffer_->Initialize(fb_dev);
  }
}

std::unique_ptr<SurfaceOzoneCanvas> SurfaceFactoryFb::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget) {
  PlatformWindowFb* window = window_manager_->GetWindow(widget);
  DCHECK(window);
  return base::WrapUnique<SurfaceOzoneCanvas>(new FbSurface(frameBuffer_.get()));
}

bool SurfaceFactoryFb::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback set_gl_get_proc_address) {
  return false;
}

scoped_refptr<NativePixmap> SurfaceFactoryFb::CreateNativePixmap(
    gfx::AcceleratedWidget widget,
    gfx::Size size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage) {
  return new TestPixmap(format);
}

}  // namespace ui
