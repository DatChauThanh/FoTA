/*******************************************************************************
* Title                 :   BIT_OP 
* Filename              :   BIT_OP.h
* Author                :   Chau Thanh Dat
* Origin Date           :   12/1/2024
* Version               :   1.0.0
* Notes                 :   None 
*****************************************************************************/

#ifndef BIT_OP
#define BIT_OP

#include "STD_TYPE.h"

#define ACCESS_REG_8BIT(REG)        (*(volatide uint8 *)REG)
#define ACCESS_REG_16BIT(REG)       (*(volatide uint16 *)REG)
#define ACCESS_REG_32BIT(REG)       (*(volatide uint32 *)REG)

#define SET_BIT_REG(REG,POS)        (REG |= (1 << POS))

#define CLR_BIT_REG(REG,POS)        (REG &= ~(1 << POS))

#define TOGGLE_BIT(REG,POS)         (REG ^= (1 << POS))

#define GET_BIT(REG,POS)            ((REG >> POS) & 1 )         

#define IS_BIT_SET(REG,POS)         (((REG >> POS) & 1 ) == 0)

#define IS_BIT_CLR(REG,POS)         (!(IS_BIT_SET(REG,BIT_NUM)))

#endif
