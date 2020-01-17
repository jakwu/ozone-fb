// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone_platform_fb.h"
#include "platform_window_fb.h"
#include "surface_factory_fb.h"
#include "egl_platform.h"

#include "base/command_line.h"
#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/environment.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/common/native_display_delegate_ozone.h"
#include "ui/ozone/public/ozone_switches.h"


namespace ui {

namespace {

const char kPlatformFbDev[] = "OZONE_FRAMEBUFFER_DEVICE";

// OzonePlatform for testing
//
// This platform dumps images to a file for testing purposes.
class OzonePlatformFb : public OzonePlatform {
 public:
  OzonePlatformFb(std::shared_ptr<EglPlatform> egl_platform)
      : egl_platform_(egl_platform) {
  }
  ~OzonePlatformFb() override {
  }

  // OzonePlatform:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_ozone_.get();
  }
  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }
  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_ozone_.get();
  }
  InputController* GetInputController() override {
    return event_factory_ozone_->input_controller();
  }
  GpuPlatformSupport* GetGpuPlatformSupport() override {
    return gpu_platform_support_.get();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }
  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return nullptr;  // no input injection support.
  }
  std::unique_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
    return base::WrapUnique<PlatformWindow>(
        new PlatformWindowFb(delegate,
                             event_factory_ozone_.get(),
                             bounds));
  }
  std::unique_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate() override {
    return base::WrapUnique(new NativeDisplayDelegateOzone());
  }

  void InitializeUI() override {
    if (!surface_factory_ozone_) {
      surface_factory_ozone_.reset(new SurfaceFactoryFb(egl_platform_));
    }
    // This unbreaks tests that create their own.
    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        base::WrapUnique(new StubKeyboardLayoutEngine()));
    device_manager_ = CreateDeviceManager();
    event_factory_ozone_.reset(new EventFactoryEvdev(
        NULL, device_manager_.get(),
        KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()));
    overlay_manager_.reset(new StubOverlayManager());
    cursor_factory_ozone_.reset(new CursorFactoryOzone);
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
  }

  void InitializeGPU() override {
    if (!surface_factory_ozone_)
      surface_factory_ozone_.reset(new SurfaceFactoryFb(egl_platform_));
    gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
  }

 private:
  std::shared_ptr<EglPlatform> egl_platform_;
  std::unique_ptr<DeviceManager> device_manager_;
  std::unique_ptr<EventFactoryEvdev> event_factory_ozone_;
  std::unique_ptr<SurfaceFactoryFb> surface_factory_ozone_;
  std::unique_ptr<CursorFactoryOzone> cursor_factory_ozone_;
  std::unique_ptr<GpuPlatformSupport> gpu_platform_support_;
  std::unique_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;
  std::unique_ptr<OverlayManagerOzone> overlay_manager_;

  DISALLOW_COPY_AND_ASSIGN(OzonePlatformFb);
};

}  // namespace

const char kDefaultEglSoname[] = "libEGL.so.1";
const char kDefaultGlesSoname[] = "libGLESv2.so.2";

OzonePlatform* CreateOzonePlatformFb() {
  std::shared_ptr<EglPlatform> egl_platform(
    new Vivante::EglPlatform(0,
                             kDefaultEglSoname,
                             kDefaultGlesSoname));
  return new OzonePlatformFb(egl_platform);
}

}  // namespace ui
