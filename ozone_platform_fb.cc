// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ozone_platform_fb.h"
#include "platform_window_fb.h"
#include "surface_factory_fb.h"
#include "platform_window_manager.h"

#include "base/command_line.h"
#include "base/environment.h"
#include "ui/ozone/public/cursor_factory_ozone.h"
#include "ui/ozone/public/gpu_platform_support.h"
#include "ui/ozone/public/gpu_platform_support_host.h"
#include "ui/events/ozone/device/device_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/ozone/public/ozone_platform.h"
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
  OzonePlatformFb(const std::string& fb_dev) : fb_dev_(fb_dev) {}
  ~OzonePlatformFb() override {}

  // OzonePlatform:
  ui::SurfaceFactoryOzone* GetSurfaceFactoryOzone() override {
    return surface_factory_ozone_.get();
  }
  CursorFactoryOzone* GetCursorFactoryOzone() override {
    return cursor_factory_ozone_.get();
  }
  GpuPlatformSupport* GetGpuPlatformSupport() override {
    return gpu_platform_support_.get();
  }
  GpuPlatformSupportHost* GetGpuPlatformSupportHost() override {
    return gpu_platform_support_host_.get();
  }
  scoped_ptr<PlatformWindow> CreatePlatformWindow(
      PlatformWindowDelegate* delegate,
      const gfx::Rect& bounds) override {
    return make_scoped_ptr<PlatformWindow>(
        new PlatformWindowFb(delegate,
                             window_manager_.get(),
                             event_factory_ozone_.get(),
                             bounds));
  }
  scoped_ptr<NativeDisplayDelegate> CreateNativeDisplayDelegate() override {
    return scoped_ptr<NativeDisplayDelegate>(new NativeDisplayDelegateOzone());
  }

  void InitializeUI() override {
    window_manager_.reset(new PlatformWindowManager());
    surface_factory_ozone_.reset(new SurfaceFactoryFb(window_manager_.get()));
    surface_factory_ozone_->Initialize(fb_dev_);
    device_manager_ = CreateDeviceManager();
    event_factory_ozone_.reset(
        new EventFactoryEvdev(NULL, device_manager_.get()));

    //cursor_factory_ozone_.reset(new BitmapCursorFactoryOzone);
    cursor_factory_ozone_.reset(new CursorFactoryOzone);
    gpu_platform_support_host_.reset(CreateStubGpuPlatformSupportHost());
  }

  void InitializeGPU() override {
    if (!surface_factory_ozone_)
      surface_factory_ozone_.reset(new SurfaceFactoryFb());
    gpu_platform_support_.reset(CreateStubGpuPlatformSupport());
  }

 private:
  std::string fb_dev_;
  scoped_ptr<PlatformWindowManager> window_manager_;
  scoped_ptr<DeviceManager> device_manager_;
  scoped_ptr<EventFactoryEvdev> event_factory_ozone_;
  scoped_ptr<SurfaceFactoryFb> surface_factory_ozone_;
  scoped_ptr<CursorFactoryOzone> cursor_factory_ozone_;
  scoped_ptr<GpuPlatformSupport> gpu_platform_support_;
  scoped_ptr<GpuPlatformSupportHost> gpu_platform_support_host_;

  DISALLOW_COPY_AND_ASSIGN(OzonePlatformFb);
};

}  // namespace

OzonePlatform* CreateOzonePlatformFb() {
  std::string fb_dev;
  base::CommandLine* cmd = base::CommandLine::ForCurrentProcess();
  
  if (cmd->HasSwitch(switches::kOzoneDumpFile)) {
    fb_dev = cmd->GetSwitchValueASCII(switches::kOzoneDumpFile);
  }
  if (fb_dev.empty()) {
    scoped_ptr<base::Environment> env(base::Environment::Create());
    env->GetVar(kPlatformFbDev, &fb_dev);
  }
  if (fb_dev.empty()) {
    fb_dev = "/dev/fb0";
  }

  return new OzonePlatformFb(fb_dev);
}

}  // namespace ui
