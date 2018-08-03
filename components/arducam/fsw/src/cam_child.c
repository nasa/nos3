/* Copyright (C) 2009 - 2017 National Aeronautics and Space Administration. All Foreign Rights are Reserved to the U.S. Government.

This software is provided "as is" without any warranty of any, kind either express, implied, or statutory, including, but not
limited to, any warranty that the software will conform to, specifications any implied warranties of merchantability, fitness
for a particular purpose, and freedom from infringement, and any warranty that the documentation will conform to the program, or
any warranty that the software will be error free.

In no event shall NASA be liable for any damages, including, but not limited to direct, indirect, special or consequential damages,
arising out of, resulting from, or in any way connected with the software or its documentation.  Whether or not based upon warranty,
contract, tort or otherwise, and whether or not loss was sustained from, or arose out of the results of, or use of, the software,
documentation or services provided hereunder

ITC Team
NASA IV&V
ivv-itc@lists.nasa.gov
*/

/*******************************************************************************
** File: cam_child.c
**
** Purpose:
**   This file contains the source code for the CAM Child Task.
**
*******************************************************************************/

#include "cam_child.h"

/*                                                            
** CAM Child Task Startup Initialization                       
*/
int32 CAM_ChildInit(void)
{
    int32 result;
    
    /* Create child task (low priority command handler) */
    result = CFE_ES_CreateChildTask(&CAM_AppData.ChildTaskID,
                                    CAM_CHILD_TASK_NAME,
                                    (void *) CAM_ChildTask, 0,
                                    CAM_CHILD_TASK_STACK_SIZE,
                                    CAM_CHILD_TASK_PRIORITY, 0);
    
    if (result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CAM_CHILD_INIT_ERR_EID, CFE_EVS_ERROR,
           "CAM child task initialization error: create task failed: result = %ld", result);
    }
    
    return result;
} /* End of CAM_ChildInit() */


/* 
**  Name:  CAM_publish                                       
**                                                                            
**  Purpose:                                                                  
** 		   Break apart functionality, publish received data.
*/
int32 CAM_publish(void)
{
    OS_MutSemTake(CAM_AppData.data_mutex);
        CAM_AppData.Exp_Pkt.msg_count++;
        CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CAM_AppData.Exp_Pkt);
        CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.Exp_Pkt);
    OS_MutSemGive(CAM_AppData.data_mutex);
    return OS_SUCCESS;
} /* End of CAM_publish() */


/* 
**  Name:  CAM_eoe_publish                                       
**                                                                            
**  Purpose:                                                                  
** 		   Break apart functionality, publish experiment complete / falure.
*/
int32 CAM_eoe_publish(int32 value)
{
    OS_MutSemTake(CAM_AppData.data_mutex);

        switch (CAM_AppData.Exp)
        {
            case 1:
                if (value == OS_SUCCESS)
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_1_SUCCESS_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                else
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_1_FAILURE_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                break;
            case 2:
                if (value == OS_SUCCESS)
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_2_SUCCESS_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                else
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_2_FAILURE_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                break;
            case 3:
                if (value == OS_SUCCESS)
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_3_SUCCESS_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                else
                {
                    CFE_SB_SetCmdCode((CFE_SB_Msg_t *) &CAM_AppData.EoE, CAM_MGR_EOE_3_FAILURE_CC);
                    CFE_SB_SendMsg((CFE_SB_Msg_t *) &CAM_AppData.EoE);
                }
                break;
            default:
                CFE_EVS_SendEvent(CAM_CHILD_EXP_ERR_EID, CFE_EVS_ERROR, "CAM experiment ID error");
                CAM_AppData.State = CAM_STOP;
                break;
        }
    
    OS_MutSemGive(CAM_AppData.data_mutex);

    return OS_SUCCESS;
} /* End of CAM_eoe_publish() */


/* 
**  Name:  CAM_state                                       
**                                                                            
**  Purpose:                                                                  
** 		   	Checks the state of the experiment
**			Holds on pause and quits on stop
*/
int32 CAM_state(void)
{
    int32 result = OS_ERROR;
    uint32 state;

    OS_MutSemTake(CAM_AppData.data_mutex);
        state = CAM_AppData.State;
    OS_MutSemGive(CAM_AppData.data_mutex);
    
    switch (state)
    {
        case CAM_LOW_VOLTAGE:
            CFE_EVS_SendEvent(CAM_LOW_VOLTAGE_EID, CFE_EVS_INFORMATION, "CAM child task low voltage received");
            break;
        
        case CAM_TIME:
            CFE_EVS_SendEvent(CAM_TIME_EID, CFE_EVS_INFORMATION, "CAM child task timeout received");
            break;

        case CAM_STOP:
            // Send experiment error message to MGR
            CAM_eoe_publish(result);
            break;
        
        case CAM_PAUSE:
            while (state == CAM_PAUSE)
            {
                OS_MutSemTake(CAM_AppData.data_mutex);
                    state = CAM_AppData.State;
                OS_MutSemGive(CAM_AppData.data_mutex);
                OS_TaskDelay(1000);
            }
            if (state == CAM_STOP)
            {
                result = OS_ERROR;
            }
            result = OS_SUCCESS;
            break;

        default: // CAM_RUN
            result = OS_SUCCESS;
    }
    return result;
}


/* 
**  Name:  CAM_fifo                                          
**                                                                            
**  Purpose:                                                                  
** 		   Read the camera FIFO until commanded to stop, complete, or error occurs.
*/
int32 CAM_fifo(uint16* x, uint8* status)
{   
    int32 result = OS_SUCCESS;

    while( (*status > 0) && (*status <= 8) && (CAM_AppData.Exp_Pkt.msg_count < ((CAM_AppData.Exp_Pkt.length / CAM_DATA_SIZE) + 1) ) )
    // Status is used to track key points such as start and end of the image
    // Limiting this number ensures that cycling through the FIFO repeatedly is avoided
    {   
        // Read a packet
        OS_MutSemTake(CAM_AppData.data_mutex);
            result = CAM_read((char*) &CAM_AppData.Exp_Pkt.data, x, status);
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_ERR_EID, CFE_EVS_ERROR, "CAM read error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;	 
        (*x) = 0;

        // Publish the packet
        result = CAM_publish();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_PUBLISH_ERR_EID, CFE_EVS_ERROR, "CAM publish error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Delay between messages to allow for processing
        OS_TaskDelay(250);

        #ifdef STF1_DEBUG
            OS_MutSemTake(CAM_AppData.data_mutex);
                OS_printf("\n status   = %d \n", *status);	
                OS_printf("\n msg_count = %ld \n", CAM_AppData.Exp_Pkt.msg_count);
            OS_MutSemGive(CAM_AppData.data_mutex);	
        #endif
    }
    return result;
}


/* 
**  Name:  CAM_exp                                         
**                                                                            
**  Purpose:                                                                  
** 		   The experiment runs until the parent dies or is commanded to stop by the parent
** 		   but has the ability to be paused and resumed depending on the current state.
*/
int32 CAM_exp(void)
{
    int32  result = OS_ERROR;
    uint8  status = 1;
    uint16 x      = 0;

    while (status == 1)
    {   // Check state
        if (CAM_state() != OS_SUCCESS) break;

        // Initialize Serial Peripheral Interface
        result = CAM_init_spi();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_INIT_SPI_ERR_EID, CFE_EVS_ERROR, "CAM init spi error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Initialize Inter-Integrated Circuit
        result = CAM_init_i2c();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_INIT_I2C_ERR_EID, CFE_EVS_ERROR, "CAM init i2c error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Camera for Upload
        result = CAM_config();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CONFIG_ERR_EID, CFE_EVS_ERROR, "CAM configure camera for upload error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_jpeg_init();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_JPEG_INIT_ERR_EID, CFE_EVS_ERROR, "CAM jpeg init error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_yuv422();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_YUV422_ERR_EID, CFE_EVS_ERROR, "CAM yuv422 error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Registers
        result = CAM_jpeg();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_JPEG_ERR_EID, CFE_EVS_ERROR, "CAM jpeg error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Configure Camera for Size
        result = CAM_setup();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_SETUP_ERR_EID, CFE_EVS_ERROR, "CAM setup error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Upload Size
        result = CAM_setSize(CAM_AppData.Size);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_SET_SIZE_ERR_EID, CFE_EVS_ERROR, "CAM upload size error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Prepare for Capture
        result = CAM_capture_prep();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CAPTURE_PREP_ERR_EID, CFE_EVS_ERROR, "CAM capture prep error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Capture Image
        result = CAM_capture();
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_CAPTURE_ERR_EID, CFE_EVS_ERROR, "CAM capture error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Read FIFO Size
        result = CAM_read_fifo_length(&CAM_AppData.Exp_Pkt.length);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_FIFO_LEN_ERR_EID, CFE_EVS_ERROR, "CAM read fifo length error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Prepare for FIFO Read
        OS_MutSemTake(CAM_AppData.data_mutex);
            CAM_AppData.Exp_Pkt.msg_count = 0x0000;
            result = CAM_read_prep((char*) &CAM_AppData.Exp_Pkt.data, (uint16*) &x);
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (result != OS_SUCCESS)
        {	
            CFE_EVS_SendEvent(CAM_READ_PREP_ERR_EID, CFE_EVS_ERROR, "CAM read prep error");
            OS_MutSemTake(CAM_AppData.data_mutex);
                CAM_AppData.State = CAM_STOP;
            OS_MutSemGive(CAM_AppData.data_mutex);
        }
        if (CAM_state() != OS_SUCCESS) break;

        // Read FIFO
        result = CAM_fifo((uint16*) &x, (uint8*) &status);
        break;
    }

    return result;
}

/* 
**  Name:  CAM_ChildTask                                          
**                                                                            
**  Purpose:                                                                  
** 		   The child task remains active until provided the binary semaphore by the parent
**         when and experiment is kicked off.
*/
void CAM_ChildTask(void)
{
    int32  result;
    int32  state;

    result = CFE_ES_RegisterChildTask();
    if(result != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CAM_CHILD_REG_ERR_EID, CFE_EVS_ERROR, "CAM APP: reg child task error %ld", result);
        CFE_ES_ExitChildTask();
        return;
    }
    else
    {
        CFE_EVS_SendEvent(CAM_CHILD_INIT_EID, CFE_EVS_INFORMATION, "CAM child task initialization complete");
    }

    while (TRUE)
    {
        // Block on Semaphore
        OS_BinSemTake(CAM_AppData.sem_id);

        // Check State
        OS_MutSemTake(CAM_AppData.data_mutex);
            state = CAM_AppData.State;
        OS_MutSemGive(CAM_AppData.data_mutex);
        if (state == CAM_PAUSE)
        {
            while(CAM_state() != OS_SUCCESS);
        }

        // Initialize Child Process Flags
        OS_MutSemTake(CAM_AppData.data_mutex);
            CAM_AppData.State = CAM_RUN;
            switch (CAM_AppData.Exp)
            {
                case 1:
                    #ifdef OV2640
                        CAM_AppData.Size = size_160x120;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_320x240;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_320x240;
                    #endif
                    break;
                case 2:
                    #ifdef OV2640
                        CAM_AppData.Size = size_800x600;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    break;
                case 3:
                    #ifdef OV2640
                        CAM_AppData.Size = size_1600x1200;
                    #endif
                    #ifdef OV5640
                        CAM_AppData.Size = size_2592x1944;
                    #endif
                    #ifdef OV5642
                        CAM_AppData.Size = size_2592x1944;
                    #endif
                    break;
                default:
                    CFE_EVS_SendEvent(CAM_CHILD_EXP_ERR_EID, CFE_EVS_ERROR, "CAM experiment ID error");
                    CAM_AppData.State = CAM_STOP;
                    break;
            }
        OS_MutSemGive(CAM_AppData.data_mutex);

        // Run Experiment
        result = CAM_exp();
        // Check Result
        OS_MutSemTake(CAM_AppData.data_mutex);
            if ((result == OS_SUCCESS) && (CAM_AppData.State == CAM_RUN))
            {
                switch (CAM_AppData.Exp)
                {
                    case 1:
                        CFE_EVS_SendEvent(CAM_EXP1_EID, CFE_EVS_INFORMATION, "CAM EXP1 Complete");
                        break;
                    case 2:
                        CFE_EVS_SendEvent(CAM_EXP2_EID, CFE_EVS_INFORMATION, "CAM EXP2 Complete");
                        break;
                    case 3:
                        CFE_EVS_SendEvent(CAM_EXP3_EID, CFE_EVS_INFORMATION, "CAM EXP3 Complete");
                        break;
                    default:
                        break;
                }
                // Delay to allow for all CAM Tlm messages to be cleared from pipe
                OS_TaskDelay(10000);
                // Send experiment complete message to MGR
                CAM_eoe_publish((result));
            }
            // Cleanup
            CAM_AppData.State = CAM_STOP;
        OS_MutSemGive(CAM_AppData.data_mutex);
    }

    /* This call allows cFE to clean-up system resources */
    CFE_EVS_SendEvent(CAM_CHILD_INIT_EID, CFE_EVS_INFORMATION,
        "CAM child task exit complete");
    CFE_ES_ExitChildTask();
} /* End of CAM_ChildTask() */


/************************/
/*  End of File Comment */
/************************/
