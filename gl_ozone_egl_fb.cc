// Copyright 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gl_ozone_egl_fb.h"
#include "gl_surface_fb.h"
#include "egl_platform.h"

#include "base/native_library.h"


namespace ui {

EGLNativeWindowType GLOzoneEglFb::GetNativeWindow(gfx::AcceleratedWidget window_id) {
  if (!native_window_) {
    native_window_ = egl_platform_->CreateWindow(GetNativeDisplay(),
      gfx::Rect(window_id >> 16, window_id & 0xFFFF));
  }
  DCHECK(native_window_);
  return native_window_;
}

int GLOzoneEglFb::GetDisplayIndex() {
  return egl_platform_->GetDisplayIndex();
}

scoped_refptr<gl::GLSurface> GLOzoneEglFb::CreateViewGLSurface(
    gfx::AcceleratedWidget widget) {
return gl::InitializeGLSurface(new GLSurfaceFb(widget, GetNativeWindow(widget)));
}

scoped_refptr<gl::GLSurface> GLOzoneEglFb::CreateOffscreenGLSurface(
    const gfx::Size& size) {
  return gl::InitializeGLSurface(new gl::PbufferGLSurfaceEGL(size));
}

EGLNativeDisplayType GLOzoneEglFb::GetNativeDisplay() {
  DCHECK(egl_platform_.get());
  if (!native_display_) {
    native_display_ = egl_platform_->CreateDisplay();
  }
  DCHECK(native_display_);
  return native_display_;
}

bool GLOzoneEglFb::LoadGLES2Bindings(gl::GLImplementation implementation) {
  switch (implementation) {
    case gl::kGLImplementationEGLGLES2:
      {
        void* lib_egl = egl_platform_->GetEglLibrary();
        void* lib_gles2 = egl_platform_->GetGles2Library();
        gl::GLGetProcAddressProc gl_proc =
            reinterpret_cast<gl::GLGetProcAddressProc>(
                base::GetFunctionPointerFromNativeLibrary(lib_egl,
                                                          "eglGetProcAddress"));
        if (!lib_egl || !lib_gles2 || !gl_proc) {
          return false;
        }

        gl::SetGLGetProcAddressProc(gl_proc);
        gl::AddGLNativeLibrary(lib_egl);
        gl::AddGLNativeLibrary(lib_gles2);
        return true;
      }
    default:
      break;
  }
  return false;
}

}  // namespace ui
