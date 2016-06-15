// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "surface_factory_fb.h"
#include "platform_window_fb.h"
#include "platform_window_manager.h"

#include "base/threading/worker_pool.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/ozone/public/surface_ozone_canvas.h"

namespace ui {

namespace {

class FbSurface : public SurfaceOzoneCanvas {
 public:
  FbSurface(FrameBuffer* framebuffer) : framebuffer_(framebuffer) {}
  ~FbSurface() override {}

  // SurfaceOzoneCanvas overrides:
  void ResizeCanvas(const gfx::Size& viewport_size) override {
    surface_ = skia::AdoptRef(SkSurface::NewRaster(SkImageInfo::MakeN32Premul(
        viewport_size.width(), viewport_size.height())));
  }
  skia::RefPtr<SkCanvas> GetCanvas() override {
    return skia::SharePtr(surface_->getCanvas());
  }
  void PresentCanvas(const gfx::Rect& damage) override {
    SkImageInfo info = framebuffer_->GetImageInfo();
    if (!surface_->getCanvas()->readPixels(info, framebuffer_->GetData(), framebuffer_->GetDataSize() / info.height(), 0, 0)) {
      LOG(ERROR) << "Failed to read pixel data";
    }
  }
  scoped_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return scoped_ptr<gfx::VSyncProvider>();
  }

 private:
  FrameBuffer* framebuffer_;
  skia::RefPtr<SkSurface> surface_;
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

scoped_ptr<SurfaceOzoneCanvas> SurfaceFactoryFb::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget) {
  PlatformWindowFb* window = window_manager_->GetWindow(widget);
  DCHECK(window);
  return make_scoped_ptr<SurfaceOzoneCanvas>(new FbSurface(frameBuffer_.get()));
}

bool SurfaceFactoryFb::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback set_gl_get_proc_address) {
  return false;
}

}  // namespace ui
