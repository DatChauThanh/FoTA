/*****************************************************/
 /* Author      : Mosad                              */
 /* Version     : v01                                */
 /* date        : 29/4/2020                          */
/*****************************************************/
#ifndef RTE_DATA_TYPES_H
#define RTE_DATA_TYPES_H
#include "main.h"

/* SystemStateType  values */
#define    SYS_IDLE                    0x0u
#define    SYS_NEW_UPDATE_REQ          0x1u
#define    SYS_REC_UPDATE              0x2u
#define    SYS_DECRYPT                 0x3u
#define    SYS_SEND_UPDATE             0x4u
#define    SYS_DONE_UPDATE             0x5u



/* Header flag  values */
#define    HEADER_NOT_SET              0x00u
#define    HEADER_SET                  0x01u
/* Buffer Flag values */ 
#define    BUFFER_NOT_SET              0x00u
#define    BUFFER_SET                  0x01u
#define    DATA_BUFFER_SIZE            0x10u
/* User response */
#define   ACCEPT_UPDATE                0X01u
#define   REFUSE_UPDATE                0X02u

#endif
