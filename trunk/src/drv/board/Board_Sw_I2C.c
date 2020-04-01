// ===============================================================================
// FILE NAME: Board_Sw_I2C.c
// DESCRIPTION:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ===============================================================================


#include "Board_GPIO.h"
#include "Board_I2C.h"
#include "Board_Sw_I2C.h"
#include "utilCounterAPI.h"

typedef struct
{
    eBOARD_PIN_TABLE eSCL;
    eBOARD_PIN_TABLE eSDA;
} eSW_I2C_PIN;

static const eSW_I2C_PIN m_asSW_I2C_Pin[eBOARD_I2C_NUMBERS] =
{
    {pNXP_SCL2_A,  pNXP_SDA2_A},
    {pNXP_SCL1_A,  pNXP_SDA1_A},
    {pNXP_SCL2_A,  pNXP_SDA2_A},
};

// ==============================================================================
// Define Section
// ==============================================================================
#define I2C_DELAY(m) \
    do\
    {\
        DWORD dwCount = 0;\
        for(dwCount = 0; dwCount<m; dwCount++);\
    }while(0)

#define I2C_DELAY_TIME 250

static DWORD m_adwI2C_Delay_Time[eBOARD_I2C_NUMBERS] = {I2C_DELAY_TIME, I2C_DELAY_TIME, I2C_DELAY_TIME};

// ==============================================================================
// FUNCTION NAME: Board_I2C_SW_Speed
// DESCRIPTION:
//
//
// Params:
// WORD wKhz:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/01/06, Larry Create
// --------------------
// ==============================================================================
void Board_I2C_SW_Speed(BYTE cBus, DWORD dwKhz)
{
    m_adwI2C_Delay_Time[cBus] = (DWORD)(SystemCoreClock/dwKhz);
    m_adwI2C_Delay_Time[cBus] = m_adwI2C_Delay_Time[cBus]/10000; //Khz
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_SW_Reset
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_SW_Reset(BYTE cBus)
{
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, HIGH);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, HIGH);
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_SW_Wait_SCL
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
// ePIN_STATE ePie:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/06/16, Larry Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_SW_Wait_SCL(BYTE cBus, BYTE cPinState)
{
    utilCounterSet(eCOUNTER_TYPE_I2C_TIMEOUT, 200);

    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_INPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    while((BOOL)cPinState != Board_Gpio_Get(m_asSW_I2C_Pin[cBus].eSCL))
    {
        if(0 == utilCounterGet(eCOUNTER_TYPE_I2C_TIMEOUT))
        {
            return rcERROR;
        }
    }

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_SW_Wait_SDA
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
// ePIN_STATE ePieState:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/01/20, Larry Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_SW_Wait_SDA(BYTE cBus, BYTE cPinState)
{
    utilCounterSet(eCOUNTER_TYPE_I2C_TIMEOUT, 200);

    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_INPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    while((BOOL)cPinState != Board_Gpio_Get(m_asSW_I2C_Pin[cBus].eSDA))
    {
        if(0 == utilCounterGet(eCOUNTER_TYPE_I2C_TIMEOUT))
        {
            return rcERROR;
        }
    }

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SW_READACK
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_ReadAck(BYTE cBus)
{
    eRESULT eResult = rcERROR;

    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_INPUT);

    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    if(Board_I2C_SW_Wait_SCL(cBus, HIGH) == rcERROR)
    {
        eResult = rcERROR;
    }

    if(!Board_Gpio_Get(m_asSW_I2C_Pin[cBus].eSDA))  //Low ACK
    {
        eResult = rcSUCCESS;
    }

    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_OUTPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_SW_NonAck
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/06/16, Larry Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_NonAck(BYTE cBus)
{
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, LOW);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_OUTPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    Board_I2C_SW_Wait_SCL(cBus, HIGH);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, HIGH);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_OUTPUT);

    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);

    return rcSUCCESS;
}


// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SW_START
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_Start(BYTE cBus)
{
    eRESULT eResult = rcSUCCESS;

    Board_I2C_SW_Wait_SCL(cBus, HIGH);
    Board_I2C_SW_Wait_SDA(cBus, HIGH);

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, HIGH);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, HIGH);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_OUTPUT);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_OUTPUT);

    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, LOW);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SW_SENDDATA
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
// BYTE cData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_SendData(BYTE cBus, BYTE cData, BOOL bAck)
{
    eRESULT eResult = rcSUCCESS;
    BYTE cCount = 8;
    (void)bAck;

    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_OUTPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    do
    {
        if(cData & 0x80)
        {
            Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, HIGH);
        }
        else
        {
            Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, LOW);
        }

        I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
        Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, HIGH);
        I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
        Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);

        cData = cData << 1;
    }
    while(--cCount);

    eResult = Board_I2C_SW_ReadAck(cBus);

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SW_READDATA
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
// BYTE *pcData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_ReadData(BYTE cBus, BYTE *pcData, BOOL bNonAck)
{
    eRESULT eResult = rcSUCCESS;
    BYTE cCount = 8;
    BYTE cData = 0;

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, HIGH);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_INPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    do
    {
        cData = cData << 1;

        if(Board_I2C_SW_Wait_SCL(cBus, HIGH) == rcERROR)
        {
            eResult = rcERROR;
        }
        I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

        cData |= (Board_Gpio_Get(m_asSW_I2C_Pin[cBus].eSDA) & 0x01);

        Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_OUTPUT);
        Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);
        I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
    }
    while(--cCount);

    *pcData = cData;

    if(bNonAck)
    {
        eResult = Board_I2C_SW_ReadAck(cBus);
    }
    else
    {
        eResult = Board_I2C_SW_NonAck(cBus);
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SW_STOP
// DESCRIPTION:
//
//
// Params:
// BYTE cBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/18, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_SW_Stop(BYTE cBus)
{
    eRESULT eResult = rcSUCCESS;

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, LOW);
    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, LOW);

    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSDA, eIO_MODE_OUTPUT);
    Board_Gpio_Dir_Set(m_asSW_I2C_Pin[cBus].eSCL, eIO_MODE_OUTPUT);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSCL, HIGH);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);
    Board_I2C_SW_Wait_SCL(cBus, HIGH);

    Board_Gpio_Set(m_asSW_I2C_Pin[cBus].eSDA, HIGH);
    I2C_DELAY(m_adwI2C_Delay_Time[cBus]);

    return eResult;
}
