from ctypes import *
import sbn_python_client as sbn


# these classes were generated with ChatGPT
class SAMPLE_Device_HK_tlm_t(Structure):
    _pack_ = 1
    _fields_ = [
        ("DeviceCounter", c_uint32),
        ("DeviceConfig", c_uint32),
        ("DeviceStatus", c_uint32)       
    ]

class SAMPLE_Hk_tlm_t(Structure):
    _pack_ = 1
    _fields_ = [
        ("TlmHeader", sbn.CFE_SB_Msg_t),
        ("CommandErrorCount", c_uint8),
        ("CommandCount", c_uint8),
        ("DeviceErrorCount", c_uint8),
        ("DeviceCount", c_uint8),
        ("DeviceEnabled", c_uint8),
        ("DeviceHK", SAMPLE_Device_HK_tlm_t),
    ]
