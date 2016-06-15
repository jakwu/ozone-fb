// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform_window_manager.h"

namespace ui {

PlatformWindowManager::PlatformWindowManager() {
}

PlatformWindowManager::~PlatformWindowManager() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

int32_t PlatformWindowManager::AddWindow(PlatformWindowFb* window) {
  return windows_.Add(window);
}

void PlatformWindowManager::RemoveWindow(int32_t window_id, PlatformWindowFb* window) {
  DCHECK_EQ(window, windows_.Lookup(window_id));
  windows_.Remove(window_id);
}

PlatformWindowFb* PlatformWindowManager::GetWindow(int32_t window_id) {
  return windows_.Lookup(window_id);
}

}  // namespace ui
