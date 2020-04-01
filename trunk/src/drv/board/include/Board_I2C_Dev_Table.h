#ifndef BOARD_I2C_DEV_TABLE_H
#define BOARD_I2C_DEV_TABLE_H
// ===============================================================================
// FILE NAME: Board_I2C_Dev_Table.h
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/07/14, Leo Create
// --------------------
// ===============================================================================

#include "CommonDef.h"
#include "Board_I2C.h"

#define EDID_HDMI_HOST_BUS      (eBOARD_I2C_BUS0)
#define EDID_HDMI_HOST_ADDRESS  (0xA2)

#define EDID_DP_HOST_BUS        (eBOARD_I2C_BUS0)
#define EDID_DP_HOST_ADDRESS    0xA8)


/*
#define EEPROM_FOR_MCU_BUS      eBOARD_I2C_BUS1
#define EEPROM_FOR_MCU_ADDRESS  0xAC

#define EEPROM_FOR_HOST_BUS     eBOARD_I2C_BUS1
#define EEPROM_FOR_HOST_ADDRESS 0xAE
*/
#define EEPROM_I2C_RETRY        10
#define EEPROM_I2C_FLAG         I2C_DEFAULT|I2C_REG_NEED|I2C_WORD_REG



#define I2C_ITE66341_ADDR       0x94
#define I2C_BUS_IT66341         eBOARD_I2C_BUS1
#define I2C_FLAG_IT66341        (I2C_DEFAULT|I2C_REG_NEED)
#define I2C_RETRY_IT66341       10

#define I2C_ITE6613_ADDR        0x98
#define I2C_BUS_IT6613_0        eBOARD_I2C_BUS1
#define I2C_FLAG_ITE6613        (I2C_DEFAULT|I2C_REG_NEED)
#define I2C_RETRY_ITE6613       10
#endif /* BOARD_I2C_DEV_TABLE_H */

