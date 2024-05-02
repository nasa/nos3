// ======================================================================
// \title  SampleSim.cpp
// \author jstar
// \brief  cpp file for SampleSim component implementation class
// ======================================================================

#include "Components/SampleSim/SampleSim.hpp"
#include "FpConfig.hpp"

extern "C"{
#include "sample_device.h"
#include "libuart.h"
}

uart_info_t SampleUart;
SAMPLE_Device_HK_tlm_t SampleHK;
SAMPLE_Device_Data_tlm_t SampleData;
int32_t status = OS_SUCCESS;

uint32_t  DeviceCounter;
uint32_t  DeviceConfig;
uint32_t  DeviceStatus;


namespace Components {

  // ----------------------------------------------------------------------
  // Component construction and destruction
  // ----------------------------------------------------------------------

  SampleSim ::
    SampleSim(const char *const compName) : SampleSimComponentBase(compName),
    m_greetingCount(0)
{

}
  
  SampleSim ::
    ~SampleSim()
  {

  }

  // ----------------------------------------------------------------------
  // Handler implementations for commands
  // ----------------------------------------------------------------------

  void SampleSim :: SAY_HELLO_cmdHandler(FwOpcodeType opCode, U32 cmdSeq, const Fw::CmdStringArg& greeting) {
    // Copy the command string input into an event string for the Hello event
    Fw::LogStringArg eventGreeting(greeting.toChar());
    // Emit the Hello event with the copied string
    this->log_ACTIVITY_HI_Hello(eventGreeting);
    
    this->tlmWrite_GreetingCount(++this->m_greetingCount);
    
    

    SampleUart.deviceString = SAMPLE_CFG_STRING;
    SampleUart.handle = SAMPLE_CFG_HANDLE;
    SampleUart.isOpen = PORT_CLOSED;
    SampleUart.baud = SAMPLE_CFG_BAUDRATE_HZ;

    printf("sleeping 3 seconds\n");
    sleep(3);
    

    status = uart_init_port(&SampleUart);
    // for (int i=0;i<2;i++) status = SAMPLE_CommandDevice(&SampleUart, SAMPLE_DEVICE_NOOP_CMD, 0);
    status = SAMPLE_CommandDevice(&SampleUart, SAMPLE_DEVICE_NOOP_CMD, 0);
    printf("Done sending noop command from fprime\n");

    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void SampleSim :: NOOP_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
  
    SampleUart.deviceString = SAMPLE_CFG_STRING;
    SampleUart.handle = SAMPLE_CFG_HANDLE;
    SampleUart.isOpen = PORT_CLOSED;
    SampleUart.baud = SAMPLE_CFG_BAUDRATE_HZ;
    

    status = uart_init_port(&SampleUart);
    status = SAMPLE_CommandDevice(&SampleUart, SAMPLE_DEVICE_NOOP_CMD, 0);
    this->log_ACTIVITY_HI_TELEM("NOOP SENT");
    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

  void SampleSim :: REQUEST_HOUSEKEEPING_cmdHandler(FwOpcodeType opCode, U32 cmdSeq) {
    
    SampleUart.deviceString = SAMPLE_CFG_STRING;
    SampleUart.handle = SAMPLE_CFG_HANDLE;
    SampleUart.isOpen = PORT_CLOSED;
    SampleUart.baud = SAMPLE_CFG_BAUDRATE_HZ;

    // printf("sleeping 3 seconds\n");
    // this->log_ACTIVITY_HI_Hello("SLEEPING 3 SECONDS");
    // sleep(3);
    

    status = uart_init_port(&SampleUart);
    
    status = SAMPLE_RequestHK(&SampleUart, &SampleHK);
    if (status == OS_SUCCESS)
    {
        this->log_ACTIVITY_HI_TELEM("RequestHK command success\n");
    }
    else
    {
        this->log_ACTIVITY_HI_TELEM("RequestHK command failed!\n");
    }

    DeviceCounter = SampleHK.DeviceCounter;
    DeviceConfig =  SampleHK.DeviceConfig;
    DeviceStatus = SampleHK.DeviceStatus;

    this->tlmWrite_DeviceCounter(DeviceCounter);
    this->tlmWrite_DeviceConfig(DeviceConfig);
    this->tlmWrite_DeviceStatus(DeviceStatus);
    
    // Tell the fprime command system that we have completed the processing of the supplied command with OK status
    this->cmdResponse_out(opCode, cmdSeq, Fw::CmdResponse::OK);
  }

}
