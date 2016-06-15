# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'internal_ozone_platform_deps': [
      'ozone_platform_fb',
    ],
    'internal_ozone_platforms': [
      'fb'
    ],
  },
  'targets': [
    {
      'target_name': 'ozone_platform_fb',
      'type': 'static_library',
      'defines': [
        'OZONE_IMPLEMENTATION',
      ],
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/ui/events/events.gyp:events',
        '<(DEPTH)/ui/events/ozone/events_ozone.gyp:events_ozone_evdev',
        '<(DEPTH)/ui/gfx/gfx.gyp:gfx',
      ],
      'sources': [
        'ozone_platform_fb.cc',
        'ozone_platform_fb.h',
        'surface_factory_fb.cc',
        'surface_factory_fb.h',
        'platform_window_fb.cc',
        'platform_window_fb.h',
        'frame_buffer.cc',
        'frame_buffer.h',
      ],
    },
  ],
}
