// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_window_fb.h"
#include "surface_factory_fb.h"

#include <string>

#include "base/files/file_path.h"
#include "base/strings/string_number_conversions.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/platform_window/platform_window_delegate.h"

namespace ui {

PlatformWindowFb::PlatformWindowFb(PlatformWindowDelegate* delegate,
                       SurfaceFactoryFb* surface_factory,
                       const gfx::Rect& bounds)
    : delegate_(delegate), surface_factory_(surface_factory), bounds_(bounds) {
  widget_ = surface_factory_->AddWindow(this);
  delegate_->OnAcceleratedWidgetAvailable(widget_);
}

PlatformWindowFb::~PlatformWindowFb() {
  surface_factory_->RemoveWindow(widget_, this);
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
}

void PlatformWindowFb::ConfineCursorToBounds(const gfx::Rect& bounds) {
}

}  // namespace ui
