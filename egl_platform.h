
#ifndef UI_OZONE_PLATFORM_FB_EGL_PLATFORM_H_
#define UI_OZONE_PLATFORM_FB_EGL_PLATFORM_H_

#define EGL_API_FB
#define LINUX

#include "ui/gfx/geometry/rect.h"
#include "ui/gl/gl_bindings.h"

#include <string>


namespace ui {

class EglPlatform {
 public:

  virtual ~EglPlatform() {}

  virtual bool InitializeHardware() = 0;
  virtual void ShutdownHardware() = 0;

  virtual void* GetEglLibrary() = 0;
  virtual void* GetGles2Library() = 0;

  virtual EGLNativeDisplayType CreateDisplay(const gfx::Rect& rect = gfx::Rect()) = 0;
  virtual void DestroyDisplay(EGLNativeDisplayType display) = 0;

  virtual EGLNativeWindowType CreateWindow(EGLNativeDisplayType display,
                                        const gfx::Rect& rect) = 0;
  virtual void DestroyWindow(EGLNativeWindowType window) = 0;

  virtual std::string GetEglSoName() const = 0;
  virtual std::string GetGled2SoName() const = 0;

  virtual int32_t GetDisplayIndex() const = 0;
};


class EglPlatformBase: public EglPlatform {
 public:

  virtual ~EglPlatformBase() {}

  bool InitializeHardware() override { return true; }
  void ShutdownHardware() override {}

  void* GetEglLibrary() override;
  void* GetGles2Library() override;
};

namespace Vivante {

class EglPlatform: public EglPlatformBase {
 public:
     
  EglPlatform(int display_index, 
              const std::string& egl_so_name,
              const std::string& gles2_so_name)
      : display_index_(display_index)
      , egl_so_name_(egl_so_name)
      , gles2_so_name_(gles2_so_name) {}

  virtual ~EglPlatform() {}

  EGLNativeDisplayType CreateDisplay(const gfx::Rect& rect = gfx::Rect()) override;
  void DestroyDisplay(EGLNativeDisplayType display) override;

  EGLNativeWindowType CreateWindow(EGLNativeDisplayType display,
                                   const gfx::Rect& rect) override;
  void DestroyWindow(EGLNativeWindowType window) override;

  std::string GetEglSoName() const override { return egl_so_name_; }
  std::string GetGled2SoName() const override { return gles2_so_name_; }

  int GetDisplayIndex() const override { return display_index_; }

 private:
  int display_index_;
  std::string egl_so_name_;
  std::string gles2_so_name_;
};

}

}


#endif // UI_OZONE_PLATFORM_FB_EGL_PLATFORM_H_
