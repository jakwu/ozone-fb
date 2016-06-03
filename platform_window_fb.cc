// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_window_fb.h"
#include "surface_factory_fb.h"

#include "ui/events/ozone/events_ozone.h"
#include "base/strings/string_number_conversions.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/events/devices/device_data_manager.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"


namespace ui {

// Touch events are reported in device coordinates. This scales the event to the
// window's coordinate space.
void ScaleTouchEvent(TouchEvent* event, const gfx::SizeF& size) {
  for (const auto& device :
       DeviceDataManager::GetInstance()->touchscreen_devices()) {
    if (device.id == event->source_device_id()) {
      gfx::SizeF touchscreen_size = gfx::SizeF(device.size);
      gfx::Transform transform;
      transform.Scale(size.width() / touchscreen_size.width(),
                      size.height() / touchscreen_size.height());
      event->UpdateForRootTransform(transform);
      return;
    }
  }
}


PlatformWindowFb::PlatformWindowFb(PlatformWindowDelegate* delegate,
                                   SurfaceFactoryFb* surface_factory,
                                   EventFactoryEvdev* event_factory,
                                   const gfx::Rect& bounds)
    : delegate_(delegate)
    , surface_factory_(surface_factory)
    , event_factory_(event_factory)
    , bounds_(bounds) {
  window_id_ = surface_factory_->AddWindow(this);
  delegate_->OnAcceleratedWidgetAvailable(window_id_, 1.f);
  ui::PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
}

PlatformWindowFb::~PlatformWindowFb() {
  ui::PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
  surface_factory_->RemoveWindow(window_id_, this);
}

gfx::Rect PlatformWindowFb::GetBounds() {
  return bounds_;
}

void PlatformWindowFb::SetBounds(const gfx::Rect& bounds) {
  bounds_ = bounds;
  delegate_->OnBoundsChanged(bounds);
}

void PlatformWindowFb::SetTitle(const base::string16& title) {
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
  event_factory_->WarpCursorTo(window_id_, gfx::PointF(location));
}

void PlatformWindowFb::ConfineCursorToBounds(const gfx::Rect& bounds) {
}

PlatformImeController* PlatformWindowFb::GetPlatformImeController() {
  return nullptr;
}

bool PlatformWindowFb::CanDispatchEvent(const ui::PlatformEvent& ne) {
  return true;
}

uint32_t PlatformWindowFb::DispatchEvent(const ui::PlatformEvent& native_event) {
  DCHECK(native_event);
  Event* event = static_cast<Event*>(native_event);
  if (event->IsTouchEvent()) {
    ScaleTouchEvent(static_cast<TouchEvent*>(event),
                    gfx::SizeF(bounds_.size()));
  }

  DispatchEventFromNativeUiEvent(
      native_event, base::Bind(&PlatformWindowDelegate::DispatchEvent,
                               base::Unretained(delegate_)));

  return ui::POST_DISPATCH_STOP_PROPAGATION;
}

}  // namespace ui
