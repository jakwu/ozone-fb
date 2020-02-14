
#include "platform_window_fb.h"

#include "base/bind.h"
#include "ui/events/event.h"
#include "ui/events/ozone/events_ozone.h"
#include "ui/events/platform/platform_event_source.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/native_widget_types.h"
#include "ui/events/ozone/evdev/event_factory_evdev.h"
#include "ui/events/devices/device_data_manager.h"

namespace ui {
#if 0
// Touch events are reported in device coordinates. This scales the event to the
// window's coordinate space.
void ScaleTouchEvent(TouchEvent* event, const gfx::SizeF& size) {
  for (const auto& device :
       DeviceDataManager::GetInstance()->GetTouchscreenDevices()) {
    if (device.id == event->source_device_id()) {
      gfx::SizeF touchscreen_size = gfx::SizeF(device.size);
      gfx::Transform transform;
      transform.Scale(size.width() / touchscreen_size.width(),
                      size.height() / touchscreen_size.height());
      event->UpdateForRootTransform(transform);
      return;
    }
  }
}
#endif

PlatformWindowFb::PlatformWindowFb(PlatformWindowDelegate* delegate,
                                   EventFactoryEvdev* event_factory,
                                   const gfx::Rect& bounds)
    : StubWindow(delegate, false, bounds)
    , event_factory_(event_factory) {
  window_id_ = (bounds.width() << 16) + bounds.height();
  delegate->OnAcceleratedWidgetAvailable(window_id_);

  if (PlatformEventSource::GetInstance())
    PlatformEventSource::GetInstance()->AddPlatformEventDispatcher(this);
}

PlatformWindowFb::~PlatformWindowFb() {
  if (PlatformEventSource::GetInstance())
    PlatformEventSource::GetInstance()->RemovePlatformEventDispatcher(this);
}

bool PlatformWindowFb::CanDispatchEvent(const PlatformEvent& ne) {
  return true;
}

uint32_t PlatformWindowFb::DispatchEvent(const PlatformEvent& native_event) {
#if 0
  Event* event = static_cast<Event*>(native_event);
  if (event->IsTouchEvent()) {
    ScaleTouchEvent(static_cast<TouchEvent*>(event),
                    gfx::SizeF(bounds_.size()));
  }
#endif
  DispatchEventFromNativeUiEvent(
      native_event, base::BindOnce(&PlatformWindowDelegate::DispatchEvent,
                                   base::Unretained(delegate())));

  return ui::POST_DISPATCH_STOP_PROPAGATION;
}

void PlatformWindowFb::MoveCursorTo(const gfx::Point& location) {
  event_factory_->WarpCursorTo(window_id_, gfx::PointF(location));
}

}  // namespace ui
