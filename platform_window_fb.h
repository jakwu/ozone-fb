// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_FB_PLATFORM_WINDOW_FB_H_
#define UI_OZONE_FB_PLATFORM_WINDOW_FB_H_

#include "base/macros.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/platform_window/platform_window.h"
#include "ui/events/platform/platform_event_dispatcher.h"

namespace ui {

class PlatformWindowDelegate;
class EventFactoryEvdev;
class PlatformWindowManager;

class PlatformWindowFb : public PlatformWindow, public PlatformEventDispatcher {
 public:
  PlatformWindowFb(PlatformWindowDelegate* delegate,
                   PlatformWindowManager* window_manager,
                   EventFactoryEvdev* event_factory,
                   const gfx::Rect& bounds);
  ~PlatformWindowFb() override;

  // PlatformWindow:
  gfx::Rect GetBounds() override;
  void SetBounds(const gfx::Rect& bounds) override;
  void SetTitle(const base::string16& title) override;
  void Show() override;
  void Hide() override;
  void Close() override;
  void SetCapture() override;
  void ReleaseCapture() override;
  void ToggleFullscreen() override;
  void Maximize() override;
  void Minimize() override;
  void Restore() override;
  void SetCursor(PlatformCursor cursor) override;
  void MoveCursorTo(const gfx::Point& location) override;
  void ConfineCursorToBounds(const gfx::Rect& bounds) override;
  PlatformImeController* GetPlatformImeController() override;

  // PlatformEventDispatcher:
  bool CanDispatchEvent(const PlatformEvent& event) override;
  uint32_t DispatchEvent(const PlatformEvent& event) override;

 private:
  PlatformWindowDelegate* delegate_;
  PlatformWindowManager* window_manager_;
  EventFactoryEvdev* event_factory_;
  gfx::Rect bounds_;
  gfx::AcceleratedWidget window_id_;

  DISALLOW_COPY_AND_ASSIGN(PlatformWindowFb);
};

}  // namespace ui

#endif  // UI_OZONE_FB_PLATFORM_WINDOW_FB_H_
