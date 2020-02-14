
#include "egl_platform.h"
#include "base/native_library.h"
#include "base/files/file_path.h"


namespace ui
{
//=============================================================================
// EglPlatformBase
//=============================================================================
void* EglPlatformBase::GetEglLibrary() {
  static base::NativeLibrary s_egl_library = nullptr;

  if (!s_egl_library)
  {
    base::NativeLibraryLoadError error;
    s_egl_library = base::LoadNativeLibrary(base::FilePath(GetEglSoName()), &error);
    DCHECK(s_egl_library);
  }

  return s_egl_library;
}

void* EglPlatformBase::GetGles2Library() {
  static base::NativeLibrary s_gles_library = nullptr;

  if (!s_gles_library)
  {
    base::NativeLibraryLoadError error;
    s_gles_library = base::LoadNativeLibrary(base::FilePath(GetGled2SoName()), &error);
    DCHECK(s_gles_library);
  }

  return s_gles_library;
}


namespace Vivante {


typedef EGLNativeDisplayType(*fbGetDisplayByIndexFn)(
    int DisplayIndex
    );

typedef void(*fbGetDisplayGeometryFn)(
    EGLNativeDisplayType Display,
    int * Width,
    int * Height
    );

typedef void(*fbDestroyDisplayFn)(
    EGLNativeDisplayType Display
    );

typedef EGLNativeWindowType(*fbCreateWindowFn)(
    EGLNativeDisplayType Display,
    int X,
    int Y,
    int Width,
    int Height
    );

typedef void(*fbDestroyWindowFn)(
    EGLNativeWindowType Window
    );


//=============================================================================
// EglPlatform
//=============================================================================

EGLNativeDisplayType EglPlatform::CreateDisplay(const gfx::Rect& /*rect*/) {
  static fbGetDisplayByIndexFn get_display = nullptr;
  if (!get_display) {
      get_display = reinterpret_cast<fbGetDisplayByIndexFn>(
        base::GetFunctionPointerFromNativeLibrary(
          GetEglLibrary(), "fbGetDisplayByIndex"));
  }
  DCHECK(get_display);
  return get_display(display_index_);
}

void EglPlatform::DestroyDisplay(EGLNativeDisplayType display) {
  static fbDestroyDisplayFn destroy_display = nullptr;
  if (!destroy_display) {
      destroy_display = reinterpret_cast<fbDestroyDisplayFn>(
        base::GetFunctionPointerFromNativeLibrary(
          GetEglLibrary(), "fbDestroyDisplay"));
  }
  DCHECK(destroy_display);
  destroy_display(display);
}

EGLNativeWindowType EglPlatform::CreateWindow(
    EGLNativeDisplayType display,
    const gfx::Rect& rect) {
  static fbCreateWindowFn ctreate_window = nullptr;

  if (!ctreate_window) {
      ctreate_window = reinterpret_cast<fbCreateWindowFn>(
        base::GetFunctionPointerFromNativeLibrary(
          GetEglLibrary(), "fbCreateWindow"));
  }
  DCHECK(ctreate_window);
  return ctreate_window(display, rect.x(), rect.y(),
                        rect.width(), rect.height());
}

void EglPlatform::DestroyWindow(EGLNativeWindowType window) {
  static fbDestroyWindowFn destroy_window = nullptr;
  if (!destroy_window) {
      destroy_window = reinterpret_cast<fbDestroyWindowFn>(
        base::GetFunctionPointerFromNativeLibrary(
          GetEglLibrary(), "fbDestroyWindow"));
  }
  DCHECK(destroy_window);
  destroy_window(window);
}

} // namespace Vivante

} // namespace gfx