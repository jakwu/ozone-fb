// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fb_screen.h"

namespace ui {

namespace {

constexpr gfx::Size kDefaultWindowSize(800, 600);

}  // namespace

FbScreen::FbScreen() {
  DCHECK_LE(next_display_id_, std::numeric_limits<int64_t>::max());

  display::Display display(next_display_id_++);
  display.SetScaleAndBounds(1.0f,
                            gfx::Rect(gfx::Point(0, 0), kDefaultWindowSize));
  display_list_.AddDisplay(display, display::DisplayList::Type::PRIMARY);
}

FbScreen::~FbScreen() = default;

const std::vector<display::Display>& FbScreen::GetAllDisplays() const {
  return display_list_.displays();
}

display::Display FbScreen::GetPrimaryDisplay() const {
  auto iter = display_list_.GetPrimaryDisplayIterator();
  DCHECK(iter != display_list_.displays().end());
  return *iter;
}

display::Display FbScreen::GetDisplayForAcceleratedWidget(
    gfx::AcceleratedWidget widget) const {
  return GetPrimaryDisplay();
}

gfx::Point FbScreen::GetCursorScreenPoint() const {
  return gfx::Point();
}

gfx::AcceleratedWidget FbScreen::GetAcceleratedWidgetAtScreenPoint(
    const gfx::Point& point) const {
  return gfx::kNullAcceleratedWidget;
}

display::Display FbScreen::GetDisplayNearestPoint(
    const gfx::Point& point) const {
  return GetPrimaryDisplay();
}

display::Display FbScreen::GetDisplayMatching(
    const gfx::Rect& match_rect) const {
  return GetPrimaryDisplay();
}

void FbScreen::AddObserver(display::DisplayObserver* observer) {
  display_list_.AddObserver(observer);
}

void FbScreen::RemoveObserver(display::DisplayObserver* observer) {
  display_list_.RemoveObserver(observer);
}

}  // namespace ui
