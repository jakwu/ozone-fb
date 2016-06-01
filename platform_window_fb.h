// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_WINDOW_FB_H_
#define UI_OZONE_PLATFORM_WINDOW_FB_H_

#include "base/files/file_path.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/platform_window/platform_window.h"

namespace ui {

class PlatformWindowDelegate;
class SurfaceFactoryFb;

class PlatformWindowFb : public PlatformWindow {
 public:
  PlatformWindowFb(PlatformWindowDelegate* delegate,
             SurfaceFactoryFb* surface_factory,
             const gfx::Rect& bounds);
  ~PlatformWindowFb() override;

  // PlatformWindow:
  gfx::Rect GetBounds() override;
  void SetBounds(const gfx::Rect& bounds) override;
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

 private:
  PlatformWindowDelegate* delegate_;
  SurfaceFactoryFb* surface_factory_;
  gfx::Rect bounds_;
  gfx::AcceleratedWidget widget_;

  DISALLOW_COPY_AND_ASSIGN(PlatformWindowFb);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_WINDOW_FB_H_
