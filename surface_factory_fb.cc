// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "surface_factory_fb.h"
#include "platform_window_fb.h"
#include "egl_platform.h"
#include "frame_buffer.h"

#include "base/command_line.h"
#include "base/memory/ptr_util.h"
#include "base/files/file_util.h"
#include "base/files/file_path_watcher.h"
#include "base/threading/worker_pool.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "ui/gfx/skia_util.h"
#include "ui/gfx/vsync_provider.h"
#include "ui/ozone/public/native_pixmap.h"
#include "ui/ozone/public/surface_ozone_canvas.h"
#include "ui/ozone/public/surface_ozone_egl.h"
#include "ui/ozone/common/egl_util.h"


namespace ui {

namespace {

const char kRendereWaitFor[] = "renderer-wait-for";

//=============================================================================
// SurfaceOzoneCanvasFb: Software rendering surface
//=============================================================================

class SurfaceOzoneCanvasFb : public SurfaceOzoneCanvas {
 public:
  SurfaceOzoneCanvasFb(const std::string& fb_device)
      : rendering_allowed_(false) {
    frame_buffer_.reset(new FrameBuffer());
    frame_buffer_->Initialize(fb_device);
    DCHECK(frame_buffer_->GetDataSize());

    base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
    if (cmd->HasSwitch(kRendereWaitFor)) {
      renderer_wait_for_ = cmd->GetSwitchValueASCII(kRendereWaitFor);
    }
    rendering_allowed_ = renderer_wait_for_.empty();
    if (!rendering_allowed_) {
      LOG(INFO) << "Software renderer is disabled and is waiting for '"
        << renderer_wait_for_ << "'";
      file_watcher_.reset(new base::FilePathWatcher());
      file_watcher_->Watch(base::FilePath(renderer_wait_for_), false,
        base::Bind(&SurfaceOzoneCanvasFb::OnWaitForFileChanged,
            base::Unretained(this)));
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
    base::AutoLock lock(lock_);
    if (rendering_allowed_) {
      PaintCanvas();
    }
  }
  std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return nullptr;
  }

 private:
  void OnWaitForFileChanged(const base::FilePath& path, bool error) {
    base::AutoLock lock(lock_);
    rendering_allowed_ = base::PathExists(path);
    if (rendering_allowed_) {
      LOG(INFO) << "Rendering enabled";
      PaintCanvas();
    } else {
      LOG(INFO) << "Rendering disabled";
    }
  }
  void PaintCanvas() {
    SkImageInfo info = frame_buffer_->GetImageInfo();
    if (!surface_->getCanvas()->readPixels(
        info, frame_buffer_->GetData(),
        frame_buffer_->GetDataSize() / info.height(),
        0, 0)) {
    LOG(ERROR) << "Failed to read pixel data";
    }
  }
  std::unique_ptr<FrameBuffer> frame_buffer_;
  sk_sp<SkSurface> surface_;
  bool rendering_allowed_;
  std::string renderer_wait_for_;
  std::unique_ptr<base::FilePathWatcher> file_watcher_;
  base::Lock lock_;
};



//=============================================================================
// SurfaceOzoneEglFb: EGL surfave
//=============================================================================

class SurfaceOzoneEglFb : public SurfaceOzoneEGL {
 public:
  SurfaceOzoneEglFb(gfx::AcceleratedWidget window_id,
                    SurfaceFactoryFb& parent)
      : window_id_(window_id)
      , parent_(parent) {
  }
  ~SurfaceOzoneEglFb() override {
  }

  intptr_t GetNativeWindow() override
  {
    return parent_.GetNativeWindow(window_id_);
  }

  bool OnSwapBuffers() override { return true; }

  void OnSwapBuffersAsync(const SwapCompletionCallback& callback) override {
    callback.Run(gfx::SwapResult::SWAP_ACK);
  }

  bool ResizeNativeWindow(const gfx::Size& viewport_size) override {
    return true;
  }

  std::unique_ptr<gfx::VSyncProvider> CreateVSyncProvider() override {
    return nullptr;
  }

  void* /* EGLConfig */ GetEGLSurfaceConfig(
      const EglConfigCallbacks& egl) override {
    EGLint broken_props[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_NONE
    };
    return ChooseEGLConfig(egl, broken_props);
  }

 private:
  gfx::AcceleratedWidget window_id_;
  SurfaceFactoryFb& parent_;
};


//=============================================================================
// TestPixmap
//=============================================================================

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


//=============================================================================
// SurfaceFactoryFb
//=============================================================================

SurfaceFactoryFb::SurfaceFactoryFb(std::shared_ptr<EglPlatform> egl_platform)
  : egl_platform_(egl_platform)
  , native_display_(0)
  , native_window_(0) {
  DCHECK(egl_platform.get());
}

SurfaceFactoryFb::~SurfaceFactoryFb() {
}

intptr_t SurfaceFactoryFb::GetNativeWindow(gfx::AcceleratedWidget window_id) {
  if (!native_window_) {
    native_window_ = egl_platform_->CreateWindow(GetNativeDisplay(),
      gfx::Rect(window_id >> 16, window_id & 0xFFFF));
  }
  DCHECK(native_window_);
  return native_window_;
}


std::unique_ptr<SurfaceOzoneCanvas> SurfaceFactoryFb::CreateCanvasForWidget(
    gfx::AcceleratedWidget widget) {
  std::stringstream ss;
  ss << "/dev/fb" << egl_platform_->GetDisplayIndex();
  return base::WrapUnique<SurfaceOzoneCanvas>(new SurfaceOzoneCanvasFb(ss.str()));
}

/// ELG

intptr_t SurfaceFactoryFb::GetNativeDisplay() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(egl_platform_.get());
  if (!native_display_) {
    native_display_ = egl_platform_->CreateDisplay(bounds_);
  }
  DCHECK(native_display_);
  return native_display_;
}

std::unique_ptr<SurfaceOzoneEGL>
SurfaceFactoryFb::CreateEGLSurfaceForWidget(
    gfx::AcceleratedWidget widget) {
  DCHECK(thread_checker_.CalledOnValidThread());
  return base::WrapUnique<SurfaceOzoneEGL>(
      new SurfaceOzoneEglFb(widget, *this));
}

bool SurfaceFactoryFb::LoadEGLGLES2Bindings(
    AddGLLibraryCallback add_gl_library,
    SetGLGetProcAddressProcCallback set_gl_get_proc_address) {

  base::NativeLibrary gles_library = egl_platform_->GetGles2Library();
  base::NativeLibrary egl_library = egl_platform_->GetEglLibrary();

  DCHECK(gles_library);
  DCHECK(egl_library);

  SurfaceFactoryOzone::GLGetProcAddressProc get_proc_address =
      reinterpret_cast<SurfaceFactoryOzone::GLGetProcAddressProc>(
          base::GetFunctionPointerFromNativeLibrary(egl_library,
                                                    "eglGetProcAddress"));
  if (!get_proc_address) {
    LOG(ERROR) << "eglGetProcAddress not found.";
    base::UnloadNativeLibrary(egl_library);
    base::UnloadNativeLibrary(gles_library);
    return false;
  }

  set_gl_get_proc_address.Run(get_proc_address);
  add_gl_library.Run(egl_library);
  add_gl_library.Run(gles_library);

  return true;
}

scoped_refptr<NativePixmap> SurfaceFactoryFb::CreateNativePixmap(
    gfx::AcceleratedWidget widget,
    gfx::Size size,
    gfx::BufferFormat format,
    gfx::BufferUsage usage) {
  return new TestPixmap(format);
}

}  // namespace ui
