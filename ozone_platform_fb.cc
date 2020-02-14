// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone_platform_fb.h"
#include "platform_window_fb.h"
#include "surface_factory_fb.h"
#include "egl_platform.h"
#include "fb_screen.h"

#include "base/macros.h"
#include "base/memory/ptr_util.h"
#include "base/environment.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/ozone/layout/keyboard_layout_engine_manager.h"
#include "ui/events/ozone/layout/stub/stub_keyboard_layout_engine.h"
#include "ui/ozone/public/ozone_platform.h"
#include "ui/ozone/common/stub_overlay_manager.h"
#include "ui/ozone/public/ozone_switches.h"
#include "ui/base/ime/input_method_minimal.h"
#include "ui/platform_window/platform_window_init_properties.h"
#include "ui/display/types/native_display_delegate.h"


namespace ui {

namespace {

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
    return surface_factory_.get();
  }
  OverlayManagerOzone* GetOverlayManager() override {
    return overlay_manager_.get();
  }
  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_.get();
  }
  InputController* GetInputController() override {
    return event_factory_->input_controller();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }
  std::unique_ptr<SystemInputInjector> CreateSystemInputInjector() override {
    return event_factory_->CreateSystemInputInjector();
  }
  std::unique_ptr<PlatformWindowBase> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      PlatformWindowInitProperties properties) override {
    return std::make_unique<PlatformWindowFb>(delegate,
      event_factory_.get(), properties.bounds);
  }
  std::unique_ptr<display::NativeDisplayDelegate> CreateNativeDisplayDelegate()
      override {
    return nullptr;
  }
  std::unique_ptr<PlatformScreen> CreateScreen() override {
    return std::make_unique<FbScreen>();
  }
  std::unique_ptr<InputMethod> CreateInputMethod(
      internal::InputMethodDelegate* delegate) override {
    return std::make_unique<InputMethodMinimal>(delegate);
  }

  void InitializeUI(const InitParams& params) override {
    device_manager_ = CreateDeviceManager();
    cursor_factory_ = std::make_unique<CursorFactoryOzone>();
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
    overlay_manager_ = std::make_unique<StubOverlayManager>();

    KeyboardLayoutEngineManager::SetKeyboardLayoutEngine(
        std::make_unique<StubKeyboardLayoutEngine>());
    ui::KeyboardLayoutEngineManager::GetKeyboardLayoutEngine()
        ->SetCurrentLayoutByName("us");

    event_factory_ = std::make_unique<EventFactoryEvdev>(
        nullptr, device_manager_.get(),
        KeyboardLayoutEngineManager::GetKeyboardLayoutEngine());

    surface_factory_ = std::make_unique<SurfaceFactoryFb>(egl_platform_);
  }

  void InitializeGPU(const InitParams& params) override {
    surface_factory_ = std::make_unique<SurfaceFactoryFb>(egl_platform_);
  }

 private:
  std::shared_ptr<EglPlatform> egl_platform_;
  std::unique_ptr<DeviceManager> device_manager_;
  std::unique_ptr<EventFactoryEvdev> event_factory_;
  std::unique_ptr<SurfaceFactoryFb> surface_factory_;
  std::unique_ptr<CursorFactoryOzone> cursor_factory_;
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
