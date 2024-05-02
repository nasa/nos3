#include <cstdio>



extern "C"
{
#include "sample_device.h"
}
uart_info_t SampleUart;
SAMPLE_Device_HK_tlm_t SampleHK;
SAMPLE_Device_Data_tlm_t SampleData;
int32_t status = OS_SUCCESS;




int main() {

  printf("testing we callled this file ZL!!\n");
    
  SampleUart.deviceString = SAMPLE_CFG_STRING;
  SampleUart.handle = SAMPLE_CFG_HANDLE;
  SampleUart.isOpen = PORT_CLOSED;
  SampleUart.baud = SAMPLE_CFG_BAUDRATE_HZ;

  sleep(20);

  status = uart_init_port(&SampleUart);
  for (int i=0;i<2;i++) status = SAMPLE_CommandDevice(&SampleUart, SAMPLE_DEVICE_NOOP_CMD, 0);

}



