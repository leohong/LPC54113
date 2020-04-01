#ifndef BOARD_GPIO_DEFINE_H
#define BOARD_GPIO_DEFINE_H
// ==============================================================================
// FILE NAME: Board_GPIO_Define.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 24/10/2016, Leo Create
// --------------------
// ==============================================================================

#include "CommonDef.h"

#define PDN_ENABLE        (1 << 3)  // Pull-down enable
#define PDN_DISABLE       (0 << 3)  // Pull-down disable
#define PUP_ENABLE        (0 << 4)  // Pull-up enable
#define PUP_DISABLE       (1 << 4)  // Pull-up disable
#define SLEWRATE_SLOW     (0 << 5)  // Slew rate for low noise with medium speed
#define SLEWRATE_FAST     (1 << 5)  // Slew rate for medium noise with fast speed
#define INBUF_ENABLE      (1 << 6)  // Input buffer
#define INBUF_DISABLE     (0 << 6)  // Input buffer
#define FILTER_ENABLE     (0 << 7)  // Glitch filter (for signals below 30MHz)
#define FILTER_DISABLE    (1 << 7)  // No glitch filter (for signals above 30MHz)
#define DRIVE_8MA         (1 << 8)  // Drive strength of 8mA
#define DRIVE_14MA        (1 << 9)  // Drive strength of 14mA
#define DRIVE_20MA        (3 << 8)  // Drive strength of 20mA

/* Configuration examples for various I/O pins */
#define EMC_IO          (PUP_DISABLE | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define LCD_PINCONFIG   (PUP_DISABLE | PDN_DISABLE | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define CLK_IN          (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define CLK_OUT         (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define GPIO_PUP        (PUP_ENABLE  | PDN_DISABLE | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define GPIO_PDN        (PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define GPIO_NOPULL     (PUP_DISABLE | PDN_DISABLE | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define UART_RX_TX      (PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_ENABLE  | FILTER_ENABLE )
#define SSP_IO          (PUP_ENABLE  | PDN_ENABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
//#define SSP_IO          (PUP_DISABLE  | PUP_DISABLE  | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define GPIO_DIS        (PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_DISABLE | FILTER_DISABLE)
#define GPIO_OUT        (PUP_DISABLE | PDN_ENABLE  | SLEWRATE_SLOW | INBUF_DISABLE | FILTER_DISABLE)
#define RMII_IN         (PUP_DISABLE | PDN_DISABLE | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)
#define RMII_OUT        (PUP_DISABLE | PDN_DISABLE | SLEWRATE_FAST | INBUF_DISABLE | FILTER_DISABLE)
#define SD_IO           (PUP_ENABLE  | PDN_DISABLE | SLEWRATE_FAST | INBUF_ENABLE  | FILTER_DISABLE)

#define USB_VBUS        (SCU_MODE_INACT    |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define I2C_IO          (                                                SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define SDMMC_DAT       (SCU_MODE_INACT    | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define SDMMC_CD        (SCU_MODE_INACT    |                             SCU_MODE_INBUFF_EN                   )
#define SDMMC_CLK       (SCU_MODE_INACT    | SCU_MODE_HIGHSPEEDSLEW_EN                                        )
#define GPIO_IN         (SCU_MODE_INACT    |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define GPIO_IN_UP      (SCU_MODE_PULLUP   |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define GPIO_IN_DN      (SCU_MODE_PULLDOWN |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define GPIO_IN_RP      (SCU_MODE_REPEATER |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define UART_TX         (SCU_MODE_PULLDOWN                                                                    )
#define UART_RX         (SCU_MODE_INACT    |                             SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define GPIO_OUT_UP     (SCU_MODE_PULLUP   |                                                  SCU_MODE_ZIF_DIS)
#define GPIO_OUT_DN     (SCU_MODE_PULLDOWN |                                                  SCU_MODE_ZIF_DIS)
#define GPIO_OUT_RP     (SCU_MODE_REPEATER |                                                  SCU_MODE_ZIF_DIS)
#define GPIO_INACT      (SCU_MODE_INACT    |                                                  SCU_MODE_ZIF_DIS)
#define GPIO_CLK        (                    SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)
#define HIGH_SPEED      (SCU_MODE_INACT    | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS)

typedef struct
{
    BYTE    cPinGroup;
    BYTE    cPinNumber;
    BYTE    cGpioPort;
    BYTE    cGpioPinNumber;
} sGPIO_PIN_MUX_CFG;

static const sGPIO_PIN_MUX_CFG m_asPinMuxCfg[] =
{
/*
                  P     P   G     G
                  I     I   P     P
                  N     N   I     I
                            O     O
                  G     N         N
                  R     U   G     U
                  O     M   R     M
                  U     B   O     B
                  P     E   U     E
                        R   P     R
*/
    /* P0_0  */{0x00,  0,  0x00,   0},
    /* P0_1  */{0x00,  1,  0x00,   1},
    /* P1_0  */{0x01,  0,  0x00,   4},
    /* P1_1  */{0x01,  1,  0x00,   8},
    /* P1_2  */{0x01,  2,  0x00,   9},
    /* P1_3  */{0x01,  3,  0x00,   10},
    /* P1_4  */{0x01,  4,  0x00,   11},
    /* P1_5  */{0x01,  5,  0x01,   8},
    /* P1_6  */{0x01,  6,  0x01,   9},
    /* P1_7  */{0x01,  7,  0x01,   0},
    /* P1_8  */{0x01,  8,  0x01,   1},
    /* P1_9  */{0x01,  9,  0x01,   2},
    /* P1_10 */{0x01,  10, 0x01,   3},
    /* P1_11 */{0x01,  11, 0x01,   4},
    /* P1_12 */{0x01,  12, 0x01,   5},
    /* P1_13 */{0x01,  13, 0x01,   6},
    /* P1_14 */{0x01,  14, 0x01,   7},
    /* P1_15 */{0x01,  15, 0x00,   2},
    /* P1_16 */{0x01,  16, 0x00,   3},
    /* P1_17 */{0x01,  17, 0x00,   12},
    /* P1_18 */{0x01,  18, 0x00,   13},
    /* P1_19 */{0x01,  19, 0xFF,   0xFF},
    /* P1_20 */{0x01,  20, 0x00,   15},
    /* P2_0  */{0x02,  0,  0x05,   0},
    /* P2_1  */{0x02,  1,  0x05,   1},
    /* P2_2  */{0x02,  2,  0x05,   2},
    /* P2_3  */{0x02,  3,  0x05,   3},
    /* P2_4  */{0x02,  4,  0x05,   4},
    /* P2_5  */{0x02,  5,  0x05,   5},
    /* P2_6  */{0x02,  6,  0x05,   6},
    /* P2_7  */{0x02,  7,  0x00,   7},
    /* P2_8  */{0x02,  8,  0x05,   7},
    /* P2_9  */{0x02,  9,  0x01,   10},
    /* P2_10 */{0x02,  10, 0x00,   14},
    /* P2_11 */{0x02,  11, 0x01,   11},
    /* P2_12 */{0x02,  12, 0x01,   12},
    /* P2_13 */{0x02,  13, 0x01,   13},
    /* P3_0  */{0x03,  0,  0xFF,   0xFF},
    /* P3_1  */{0x03,  1,  0x05,   8},
    /* P3_2  */{0x03,  2,  0x05,   9},
    /* P3_3  */{0x03,  3,  0xFF,   0xFF},
    /* P3_4  */{0x03,  4,  0x01,   14},
    /* P3_5  */{0x03,  5,  0x01,   15},
    /* P3_6  */{0x03,  6,  0x00,   6},
    /* P3_7  */{0x03,  7,  0x05,   10},
    /* P3_8  */{0x03,  8,  0x05,   11},
    /* P4_0  */{0x04,  0,  0x02,   0},
    /* P4_1  */{0x04,  1,  0x02,   1},
    /* P4_2  */{0x04,  2,  0x02,   2},
    /* P4_3  */{0x04,  3,  0x02,   3},
    /* P4_4  */{0x04,  4,  0x02,   4},
    /* P4_5  */{0x04,  5,  0x02,   5},
    /* P4_6  */{0x04,  6,  0x02,   6},
    /* P4_7  */{0x04,  7,  0xFF,   0xFF},
    /* P4_8  */{0x04,  8,  0x05,   12},
    /* P4_9  */{0x04,  9,  0x05,   13},
    /* P4_10 */{0x04,  10, 0x05,   14},
    /* P5_0  */{0x05,  0,  0x02,   9},
    /* P5_1  */{0x05,  1,  0x02,   10},
    /* P5_2  */{0x05,  2,  0x02,   11},
    /* P5_3  */{0x05,  3,  0x02,   12},
    /* P5_4  */{0x05,  4,  0x02,   13},
    /* P5_5  */{0x05,  5,  0x02,   14},
    /* P5_6  */{0x05,  6,  0x02,   15},
    /* P5_7  */{0x05,  7,  0x02,   7},
    /* P6_0  */{0x06,  0,  0xFF,   0xFF},
    /* P6_1  */{0x06,  1,  0x03,   0},
    /* P6_2  */{0x06,  2,  0x03,   1},
    /* P6_3  */{0x06,  3,  0x03,   2},
    /* P6_4  */{0x06,  4,  0x03,   3},
    /* P6_5  */{0x06,  5,  0x03,   4},
    /* P6_6  */{0x06,  6,  0x00,   5},
    /* P6_7  */{0x06,  7,  0x05,   15},
    /* P6_8  */{0x06,  8,  0x05,   16},
    /* P6_9  */{0x06,  9,  0x03,   5},
    /* P6_10 */{0x06,  10, 0x03,   6},
    /* P6_11 */{0x06,  11, 0x03,   7},
    /* P6_12 */{0x06,  12, 0x02,   8},
    /* P7_0  */{0x07,  0,  0x03,   8},
    /* P7_1  */{0x07,  1,  0x03,   9},
    /* P7_2  */{0x07,  2,  0x03,   10},
    /* P7_3  */{0x07,  3,  0x03,   11},
    /* P7_4  */{0x07,  4,  0x03,   12},
    /* P7_5  */{0x07,  5,  0x03,   13},
    /* P7_6  */{0x07,  6,  0x03,   14},
    /* P7_7  */{0x07,  7,  0x03,   15},
    /* P8_0  */{0x08,  0,  0x04,   0},
    /* P8_1  */{0x08,  1,  0x04,   1},
    /* P8_2  */{0x08,  2,  0x04,   2},
    /* P8_3  */{0x08,  3,  0x04,   3},
    /* P8_4  */{0x08,  4,  0x04,   4},
    /* P8_5  */{0x08,  5,  0x04,   5},
    /* P8_6  */{0x08,  6,  0x04,   6},
    /* P8_7  */{0x08,  7,  0x04,   7},
    /* P8_8  */{0x08,  8,  0xFF,   0xFF},
    /* P9_0  */{0x09,  0,  0x04,   12},
    /* P9_1  */{0x09,  1,  0x04,   13},
    /* P9_2  */{0x09,  2,  0x04,   14},
    /* P9_3  */{0x09,  3,  0x04,   15},
    /* P9_4  */{0x09,  4,  0x05,   17},
    /* P9_5  */{0x09,  5,  0x05,   18},
    /* P9_6  */{0x09,  6,  0x04,   11},
    /* PA_0  */{0x0A,  0,  0xFF,   0xFF},
    /* PA_1  */{0x0A,  1,  0x04,   8},
    /* PA_2  */{0x0A,  2,  0x04,   9},
    /* PA_3  */{0x0A,  3,  0x04,   10},
    /* PA_4  */{0x0A,  4,  0x05,   19},
    /* PB_0  */{0x0B,  0,  0x05,   20},
    /* PB_1  */{0x0B,  1,  0x05,   21},
    /* PB_2  */{0x0B,  2,  0x05,   22},
    /* PB_3  */{0x0B,  3,  0x05,   23},
    /* PB_4  */{0x0B,  4,  0x05,   24},
    /* PB_5  */{0x0B,  5,  0x05,   25},
    /* PB_6  */{0x0B,  6,  0x05,   26},
    /* PC_0  */{0x0C,  0,  0xFF,   0xFF},
    /* PC_1  */{0x0C,  1,  0x06,   0},
    /* PC_2  */{0x0C,  2,  0x06,   1},
    /* PC_3  */{0x0C,  3,  0x06,   2},
    /* PC_4  */{0x0C,  4,  0x06,   3},
    /* PC_5  */{0x0C,  5,  0x06,   4},
    /* PC_6  */{0x0C,  6,  0x06,   5},
    /* PC_7  */{0x0C,  7,  0x06,   6},
    /* PC_8  */{0x0C,  8,  0x06,   7},
    /* PC_9  */{0x0C,  9,  0x06,   8},
    /* PC_10 */{0x0C,  10, 0x06,   9},
    /* PC_11 */{0x0C,  11, 0x06,   10},
    /* PC_12 */{0x0C,  12, 0x06,   11},
    /* PC_13 */{0x0C,  13, 0x06,   12},
    /* PC_14 */{0x0C,  14, 0x06,   13},
    /* PD_0  */{0x0D,  0,  0x06,   14},
    /* PD_1  */{0x0D,  1,  0x06,   15},
    /* PD_2  */{0x0D,  2,  0x06,   16},
    /* PD_3  */{0x0D,  3,  0x06,   17},
    /* PD_4  */{0x0D,  4,  0x06,   18},
    /* PD_5  */{0x0D,  5,  0x06,   19},
    /* PD_6  */{0x0D,  6,  0x06,   20},
    /* PD_7  */{0x0D,  7,  0x06,   21},
    /* PD_8  */{0x0D,  8,  0x06,   22},
    /* PD_9  */{0x0D,  9,  0x06,   23},
    /* PD_10 */{0x0D,  10, 0x06,   24},
    /* PD_11 */{0x0D,  11, 0x06,   25},
    /* PD_12 */{0x0D,  12, 0x06,   26},
    /* PD_13 */{0x0D,  13, 0x06,   27},
    /* PD_14 */{0x0D,  14, 0x06,   28},
    /* PD_15 */{0x0D,  15, 0x06,   29},
    /* PD_16 */{0x0D,  16, 0x06,   30},
    /* PE_0  */{0x0E,  0,  0x07,   0},
    /* PE_1  */{0x0E,  1,  0x07,   1},
    /* PE_2  */{0x0E,  2,  0x07,   2},
    /* PE_3  */{0x0E,  3,  0x07,   3},
    /* PE_4  */{0x0E,  4,  0x07,   4},
    /* PE_5  */{0x0E,  5,  0x07,   5},
    /* PE_6  */{0x0E,  6,  0x07,   6},
    /* PE_7  */{0x0E,  7,  0x07,   7},
    /* PE_8  */{0x0E,  8,  0x07,   8},
    /* PE_9  */{0x0E,  9,  0x07,   9},
    /* PE_10 */{0x0E,  10, 0x07,   10},
    /* PE_11 */{0x0E,  11, 0x07,   11},
    /* PE_12 */{0x0E,  12, 0x07,   12},
    /* PE_13 */{0x0E,  13, 0x07,   13},
    /* PE_14 */{0x0E,  14, 0x07,   14},
    /* PE_15 */{0x0E,  15, 0x07,   15},
    /* PF_0  */{0x0F,  0,  0xFF,   0xFF},
    /* PF_1  */{0x0F,  1,  0x07,   16},
    /* PF_2  */{0x0F,  2,  0x07,   17},
    /* PF_3  */{0x0F,  3,  0x07,   18},
    /* PF_4  */{0x0F,  4,  0xFF,   0xFF},
    /* PF_5  */{0x0F,  5,  0x07,   19},
    /* PF_6  */{0x0F,  6,  0x07,   20},
    /* PF_7  */{0x0F,  7,  0x07,   21},
    /* PF_8  */{0x0F,  8,  0x07,   22},
    /* PF_9  */{0x0F,  9,  0x07,   23},
    /* PF_10 */{0x0F,  10, 0x07,   24},
    /* PF_11 */{0x0F,  11, 0x07,   25},
};

typedef enum
{
    eBOARD_INT_PIN0 = 0,
    eBOARD_INT_PIN1,
    eBOARD_INT_PIN2,
    eBOARD_INT_PIN3,
    eBOARD_INT_PIN4,
    eBOARD_INT_PIN5,
    eBOARD_INT_PIN6,
    eBOARD_INT_PIN7,

    eBOARD_INT_PIN_NUMBERS,
} eBOARD_INT_PIN;

typedef enum
{
    eINT_MODE_EDGE,
    eINT_MODE_LEVEL,

    eINT_MODE_NUMBERS,
} eINT_MODE;

typedef enum
{
    eINT_EDGE_RISE,
    eINT_EDGE_FALL,
    eINT_EDGE_BOTH_EDGE,

    eINT_EDGE_NUMBERS,
} eINT_EDGE;

typedef enum
{
    eIO_MODE_INPUT = 0,
    eIO_MODE_OUTPUT,
    eIO_MODE_NONGPIO,

    eIO_MODE_NUMBERS,
} eIO_MODE;

typedef struct
{
    WORD                wNetName;
    WORD                wPinMode;
    BYTE                cPinFunc;
    eIO_MODE            eDirection;
    BYTE                cPinValue;
} sPIN_CFG;

typedef struct
{
    const   sPIN_CFG *psPinCfg;
    WORD    wPinNum;
} sGPIO_CFG;

typedef struct
{
    BYTE                cINFUNC;
    BYTE                cCHANNEL;
    FunctionalState     bINV;
    FunctionalState     bEDGE;
    FunctionalState     bSYNCH;
    FunctionalState     bPLUSE;
    DWORD               dwSELECT;
} sGIMA_CFG;

typedef struct
{
    GIMA_FUNC_T eFunc;
    sGIMA_CFG   sGimaPinCfg;
} sGIMA_PIN_CFG;

typedef struct
{
    const sGIMA_PIN_CFG *psGimaCgf;
    WORD                wPinNum;
} sGIMA_MUX_CFG;


typedef struct
{
    BYTE cPin;
    BYTE cIntPortNum;
} sGPIO_INT_CFG;

typedef struct
{
    WORD    wPinName;
    BYTE    cStatus;
    WORD    wDelayTime;
} sPWRSEQ;

typedef enum
{
    pCLK0,
    pCLK1,
    pCLK2,
    pCLK3,

    pBOARD_CLKPIN_NUMBERS,
} eBOARD_CLK_PIN_TABLE;

typedef struct
{
    eBOARD_CLK_PIN_TABLE    eNetName;
    WORD                    wPinMode;
    BYTE                    cPinFunc;
} sPIN_CLK_CFG;

static const sGPIO_PIN_MUX_CFG m_asCLK_PinMuxCfg[] =
{
    /*
                      P     P   G     G
                      I     I   P     P
                      N     N   I     I
                                O     O
                      G     N         N
                      R     U   G     U
                      O     M   R     M
                      U     B   O     B
                      P     E   U     E
                            R   P     R
    */
    /* CLK0  */{0x00,  0,  0xFF,   0xFF},
    /* CLK1  */{0x00,  1,  0xFF,   0xFF},
    /* CLK2  */{0x00,  2,  0xFF,   0xFF},
    /* CLK3  */{0x00,  3,  0xFF,   0xFF},
};

#endif /* BOARD_GPIO_DEFINE_H */
