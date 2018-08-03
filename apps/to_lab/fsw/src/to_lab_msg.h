/************************************************************************
** File:
**
** Purpose: 
**  Define TO Lab Messages and info
**
** Notes:
**
** $Log: to_lab_msg.h  $
** Revision 1.1 2008/04/30 13:45:27GMT-05:00 rjmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/to_lab/fsw/src/project.pj
**
*************************************************************************/
#ifndef _to_lab_msg_h_
#define _to_lab_msg_h_

#define TO_NOP_CC                0       /*  no-op command     */
#define TO_RESET_STATUS_CC       1       /*  reset status      */
#define TO_ADD_PKT_CC            2       /*  add packet        */
#define TO_SEND_DATA_TYPES_CC    3       /*  send data types   */
#define TO_REMOVE_PKT_CC         4       /*  remove packet     */
#define TO_REMOVE_ALL_PKT_CC     5       /*  remove all packet */
#define TO_OUTPUT_ENABLE_CC      6       /*  output enable     */

/******************************************************************************/

typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint8              command_counter;
    uint8              command_error_counter;
    uint8              spareToAlign[2];
 
} to_hk_tlm_t;

#define TO_HK_TLM_LNGTH      sizeof(to_hk_tlm_t)

/******************************************************************************/

typedef struct
{
    uint8              TlmHeader[CFE_SB_TLM_HDR_SIZE];
    uint16             synch;
#if 0
    bit_field          bit1:1;
    bit_field          bit2:1;
    bit_field          bit34:2;
    bit_field          bit56:2;
    bit_field          bit78:2;

    bit_field          nibble1:4;
    bit_field          nibble2:4;
#endif
    uint8              bl1, bl2;       /* boolean */
    int8               b1, b2, b3, b4;
    int16              w1,w2;
    int32              dw1, dw2;
    float              f1, f2;
    double             df1, df2;
    char               str[10];
}to_data_types_fmt;

#define TO_DATA_TYPES_LNGTH   sizeof (to_data_types_fmt)

/******************************************************************************/

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_SB_MsgId_t     Stream;
    uint16             PktSize;
    CFE_SB_Qos_t       Flags;
    uint8              BufLimit;
}  TO_ADD_PKT_t;


/*****************************************************************************/

typedef struct {
      CFE_SB_MsgId_t   Stream;
      CFE_SB_Qos_t     Flags;
      uint16           BufLimit;
 } TO_subsciption_t;

/******************************************************************************/

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    CFE_SB_MsgId_t     Stream;
}  TO_REMOVE_PKT_t;

/******************************************************************************/

typedef struct
{
    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
    char               dest_IP[16];
}  TO_OUTPUT_ENABLE_PKT_t;

/******************************************************************************/


#endif /* _to_lab_msg_h_ */

/************************/
/*  End of File Comment */
/************************/
