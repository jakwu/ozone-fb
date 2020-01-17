
#ifndef UI_OZONE_PLATFORM_FRAME_BUFFER_H_
#define UI_OZONE_PLATFORM_FRAME_BUFFER_H_

#include "third_party/skia/include/core/SkImageInfo.h"

#include <linux/fb.h>
#include <string>

namespace ui {

class FrameBuffer
{
public:
  FrameBuffer();
  ~FrameBuffer();

  bool Initialize(const std::string& fb_dev);
  const SkImageInfo& GetImageInfo() const { return image_info_; }
  char* GetData();
  size_t GetDataSize() const { return screensize_; }

private:
  bool Open(const std::string& fb_dev);
  void Close();

  bool ApplyImageInfo(const SkImageInfo& image_info);

  int fd_;
  size_t screensize_;
  char *data_;
  struct fb_var_screeninfo vinfo_;
  struct fb_fix_screeninfo finfo_;
  SkImageInfo image_info_;
};

}  // namespace ui


#endif /* UI_OZONE_PLATFORM_FRAME_BUFFER_H_ */
