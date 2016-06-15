// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_window_fb.h"
#include "platform_window_manager.h"

#include "base/bind.h"
#include "ui/events/ozone/events_ozone.h"
#include "base/strings/string_number_conversions.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"


namespace ui {

PlatformWindowFb::PlatformWindowFb(PlatformWindowDelegate* delegate,
                                   PlatformWindowManager* window_manager,
                                   EventFactoryEvdev* event_factory,
                                   const gfx::Rect& bounds)
    : delegate_(delegate)
    , window_manager_(window_manager)
    , event_factory_(event_factory)
    , bounds_(bounds) {
  window_id_ = window_manager_->AddWindow(this);
  delegate_->OnAcceleratedWidgetAvailable(window_id_);
  ui::PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
}

PlatformWindowFb::~PlatformWindowFb() {
  ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
  window_manager_->RemoveWindow(window_id_, this);
}

gfx::Rect PlatformWindowFb::GetBounds() {
  return bounds_;
}

void PlatformWindowFb::SetBounds(const gfx::Rect& bounds) {
  bounds_ = bounds;
  delegate_->OnBoundsChanged(bounds);
}

void PlatformWindowFb::Show() {
}

void PlatformWindowFb::Hide() {
}

void PlatformWindowFb::Close() {
}

void PlatformWindowFb::SetCapture() {
}

void PlatformWindowFb::ReleaseCapture() {
}

void PlatformWindowFb::ToggleFullscreen() {
}

void PlatformWindowFb::Maximize() {
}

void PlatformWindowFb::Minimize() {
}

void PlatformWindowFb::Restore() {
}

void PlatformWindowFb::SetCursor(PlatformCursor cursor) {
}

void PlatformWindowFb::MoveCursorTo(const gfx::Point& location) {
  event_factory_->WarpCursorTo(window_id_, location);
}

bool PlatformWindowFb::CanDispatchEvent(const ui::PlatformEvent& ne) {
  return true;
}

uint32_t PlatformWindowFb::DispatchEvent(const ui::PlatformEvent& native_event) {
  DispatchEventFromNativeUiEvent(
      native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                               base::Unretained(delegate_)));

  return ui::POST_DISPATCH_STOP_PROPAGATION;
}

}  // namespace ui
