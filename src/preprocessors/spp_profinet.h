/* $Id$ */
/* Snort Preprocessor Plugin Header File Template */

/* This file gets included in plugbase.h when it is integrated into the rest 
 * of the program.  
 */
#ifndef __SPP_PROFINET_H__
#define __SPP_PROFINET_H__

/* Define Ethertypes that are not defined in the decode.h but
    are used within the ProfiNet environment */
#define ETHERNET_TYPE_VLAN            0x8100
#define ETHERNET_TYPE_SNMP            0x814c
#define ETHERNET_TYPE_PN              0x8892
#define ETHERNET_TYPE_MRP             0x88e3
#define ETHERNET_TYPE_LLDP            0x88cc

#define FRAME_ID_DCP_HELLO      0xfefc
#define FRAME_ID_DCP_GETORSET   0xfefd
#define FRAME_ID_DCP_IDENT_REQ  0xfefe
#define FRAME_ID_DCP_IDENT_RES  0xfeff

/* 
 * list of function prototypes to export for this preprocessor 
 */
void SetupProfiNet();

#endif  /* __SPP_PROFINET__H__ */
