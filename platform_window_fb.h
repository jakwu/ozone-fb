// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_FB_PLATFORM_WINDOW_FB_H_
#define UI_OZONE_FB_PLATFORM_WINDOW_FB_H_

#include "base/macros.h"
#include "ui/events/platform/platform_event_dispatcher.h"
#include "ui/platform_window/platform_window_delegate.h"
#include "ui/platform_window/stub/stub_window.h"


namespace ui {

class EventFactoryEvdev;

class PlatformWindowFb : public StubWindow, public PlatformEventDispatcher {
 public:
  PlatformWindowFb(PlatformWindowDelegate* delegate,
                   EventFactoryEvdev* event_factory,
                   const gfx::Rect& bounds);
  ~PlatformWindowFb() override;

  // PlatformEventDispatcher implementation:
  bool CanDispatchEvent(const PlatformEvent& event) override;
  uint32_t DispatchEvent(const PlatformEvent& event) override;
  void MoveCursorTo(const gfx::Point& location) override;

 private:
  EventFactoryEvdev* event_factory_;
  gfx::AcceleratedWidget window_id_;

  DISALLOW_COPY_AND_ASSIGN(PlatformWindowFb);
};

}  // namespace ui

#endif  // UI_OZONE_FB_PLATFORM_WINDOW_FB_H_
