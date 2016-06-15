// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_OZONE_PLATFORM_FB_PLATFORM_WINDOW_MANAGER_H_
#define UI_OZONE_PLATFORM_FB_PLATFORM_WINDOW_MANAGER_H_

#include "base/id_map.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/thread_checker.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/ozone/public/surface_factory_ozone.h"

namespace ui {

class PlatformWindowFb;

class PlatformWindowManager {
 public:
  PlatformWindowManager();
  ~PlatformWindowManager();

  // Register a new window. Returns the window id.
  int32_t AddWindow(PlatformWindowFb* window);

  // Remove a window.
  void RemoveWindow(int32_t window_id, PlatformWindowFb* window);

  // Find a window object by id;
  PlatformWindowFb* GetWindow(int32_t window_id);

 private:

  IDMap<PlatformWindowFb> windows_;
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(PlatformWindowManager);
};

}  // namespace ui

#endif  // UI_OZONE_PLATFORM_FB_PLATFORM_WINDOW_MANAGER_H_
