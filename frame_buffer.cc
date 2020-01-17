#include "frame_buffer.h"

#include "base/logging.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

namespace {
  static void dump_screeninfo(const char* msg, struct fb_var_screeninfo& vinfo)
  {
    LOG(INFO) << msg << " fb_var_screeninfo:\n"
	  << "  xres: " << vinfo.xres << "\n"
      << "  yres: " << vinfo.yres << "\n"
	  << "  xres_virtual: " << vinfo.xres_virtual << "\n"
      << "  yres_virtual: " << vinfo.yres_virtual << "\n"
      << "  xoffset: " << vinfo.xoffset << "\n"
      << "  yoffset: " << vinfo.yoffset << "\n"
      << "  bits_per_pixel: " << vinfo.bits_per_pixel << "\n"
      << "  red: " << vinfo.red.offset <<"/" << vinfo.red.length << " MSB right: " << vinfo.red.msb_right << "\n"
      << "  green: " << vinfo.green.offset << "/" << vinfo.green.length << " MSB right: " << vinfo.green.msb_right << "\n"
      << "  blue: " << vinfo.blue.offset << "/" << vinfo.blue.length << " MSB right: " << vinfo.blue.msb_right << "\n"
      << "  transp: " << vinfo.transp.offset << "/" << vinfo.transp.length << " MSB right: " << vinfo.transp.msb_right << "\n"
	;
  }
}

namespace ui {

FrameBuffer::FrameBuffer()
  : fd_(-1), screensize_(0), data_(0)
{
}

FrameBuffer::~FrameBuffer()
{
    Close();
}

bool FrameBuffer::Initialize(const std::string& fb_dev)
{
  if (-1 == fd_) {
    if (!Open(fb_dev))
    {
      return true;
    }
  }
  return true;
}

void FrameBuffer::Close()
{
  if (data_ && screensize_)
    munmap(data_, screensize_);
  if (fd_ != -1)
    close(fd_);
}

bool FrameBuffer::Open(const std::string& fb_dev)
{
  std::string dev = "/dev/fb0";

  if (!fb_dev.empty())
  {
    dev = fb_dev;
  }

  // Open the file for reading and writing
  fd_ = open(dev.c_str(), O_RDWR);
  if (fd_ == -1)
  {
    LOG(FATAL) << "Failed to open frame buffer: " << errno;
    Close();
    return false;
  }

  // Get variable screen information
  if (ioctl(fd_, FBIOGET_VSCREENINFO, &vinfo_) == -1)
  {
    LOG(FATAL) << "Failed to read variable information: " << errno;
    Close();
    return false;
  }

  dump_screeninfo("Current", vinfo_);

  // Figure out the size of the screen in bytes
  screensize_ = vinfo_.xres * vinfo_.yres * vinfo_.bits_per_pixel / 8;
  LOG(INFO) << "Frame buffer size: " << screensize_;

  // Map the device to memory
  data_ = (char*)mmap(0, screensize_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
  if ((int)data_ == -1)
  {
    LOG(FATAL) << "Failed to map framebuffer device to memory: " << errno;
    Close();
    return 0;
  }

  // Get fixed screen information
  if (ioctl(fd_, FBIOGET_FSCREENINFO, &finfo_) == -1)
  {
    LOG(FATAL) << "Failed to read fixed information: " << errno;
    Close();
    return false;
  }

  image_info_ = SkImageInfo::MakeN32Premul(vinfo_.xres, vinfo_.yres);

  //if (!ApplyImageInfo(image_info_))
  //{
  //  LOG(FATAL) << "Failed to apply variable information: " << errno;
  //  Close();
  //  return false;
  //}

  return true;
}

bool FrameBuffer::ApplyImageInfo(const SkImageInfo& image_info)
{
  vinfo_.xres_virtual = image_info.width();
  vinfo_.yres_virtual = image_info.height();
  vinfo_.bits_per_pixel = SkColorTypeBytesPerPixel(image_info.colorType()) * 8;

  switch (image_info.colorType())
  {
  case kAlpha_8_SkColorType:
    {
      vinfo_.red.offset = 0;
      vinfo_.red.length = 0;

      vinfo_.green.offset = 0;
      vinfo_.green.length = 0;

      vinfo_.blue.offset = 0;
      vinfo_.blue.length = 0;

      vinfo_.transp.offset = 0;
      vinfo_.transp.length = 8;
    }
    break;
  default:
  case kRGB_565_SkColorType:
    {
      vinfo_.red.offset = 0;
      vinfo_.red.length = 5;

      vinfo_.green.offset = 5;
      vinfo_.green.length = 6;

      vinfo_.blue.offset = 11;
      vinfo_.blue.length = 5;

      vinfo_.transp.offset = 0;
      vinfo_.transp.length = 0;
    }
    break;
  case kARGB_4444_SkColorType:
    {
      vinfo_.transp.offset = 0;
      vinfo_.transp.length = 4;

      vinfo_.red.offset = 4;
      vinfo_.red.length = 4;

      vinfo_.green.offset = 8;
      vinfo_.green.length = 4;

      vinfo_.blue.offset = 12;
      vinfo_.blue.length = 4;
    }
    break;
  case kRGBA_8888_SkColorType:
    {
      vinfo_.red.offset = 0;
      vinfo_.red.length = 8;

      vinfo_.green.offset = 8;
      vinfo_.green.length = 8;

      vinfo_.blue.offset = 16;
      vinfo_.blue.length = 8;

      vinfo_.transp.offset = 24;
      vinfo_.transp.length = 8;
    }
    break;
  case kBGRA_8888_SkColorType:
    {
      vinfo_.blue.offset = 0;
      vinfo_.blue.length = 8;

      vinfo_.green.offset = 8;
      vinfo_.green.length = 8;

      vinfo_.red.offset = 16;
      vinfo_.red.length = 8;

      vinfo_.transp.offset = 24;
      vinfo_.transp.length = 8;
    }
    break;
  case kIndex_8_SkColorType:
    {
      vinfo_.blue.offset = 0;
      vinfo_.blue.length = 0;

      vinfo_.green.offset = 0;
      vinfo_.green.length = 0;

      vinfo_.red.offset = 0;
      vinfo_.red.length = 0;

      vinfo_.transp.offset = 0;
      vinfo_.transp.length = 0;
    }
    break;
  }

  dump_screeninfo("Applying", vinfo_);

  return (ioctl(fd_, FBIOPUT_VSCREENINFO, &vinfo_) != 0);
}

char* FrameBuffer::GetData()
{
    int dummy = 0;
    if (fd_ == -1)
        return 0;
    ioctl(fd_, FBIO_WAITFORVSYNC, &dummy);
    return data_;
}
}
