{
  'variables':  {
    'ozone_platform_fb%': 1,
  },
  'conditions': [
    ['<(ozone_platform_fb) == 1', {
      'variables':  {
        'external_ozone_platform_deps': [
          '<(DEPTH)/ozone-fb/fb.gypi:ozone_platform_fb',
        ],
        'external_ozone_platforms': [
          'fb'
        ],
        'ozone_platform%': 'fb',
      },
    }],
  ],
}
