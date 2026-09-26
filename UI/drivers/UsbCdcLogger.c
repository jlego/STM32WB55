#include "stm32wbxx.h"
#include "stm32wbxx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

static int usb_initialized = 0;

void UsbCdcLogger_Init(void) {
  if (usb_initialized) return;
  
  // 初始化 USB 设备
  MX_USB_Device_Init();
  
  usb_initialized = 1;
}

void UsbCdcLogger_Send(const uint8_t* data, uint16_t size) {
  if (!usb_initialized) return;
  
  // 通过 USB CDC 发送数据
  CDC_Transmit_FS((uint8_t*)data, size);
}

int UsbCdcLogger_IsInitialized(void) {
  return usb_initialized;
}