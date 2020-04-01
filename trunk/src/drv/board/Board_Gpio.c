// ==============================================================================
// FILE NAME: BOARD_GPIO.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 09/01/2013, Leo Create
// --------------------
// ==============================================================================


#include "Board.h"
#include "Board_Gpio.h"
#include "Board_Gpio_Table.h"
#include "utilCounterAPI.h"
#include "utilLogAPI.h"
#include "utilMathAPI.h"

void (*fpGpioIntRiseCallback[eBOARD_INT_PIN_NUMBERS])(void);
void (*fpGpioIntFallCallback[eBOARD_INT_PIN_NUMBERS])(void);

#ifdef CORE_M4
static const IRQn_Type m_apsBoard_GpioInt_IRQ[eBOARD_INT_PIN_NUMBERS] =
{
    PIN_INT0_IRQn,
    PIN_INT1_IRQn,
    PIN_INT2_IRQn,
    PIN_INT3_IRQn,
    PIN_INT4_IRQn,
    PIN_INT5_IRQn,
    PIN_INT6_IRQn,
    PIN_INT7_IRQn
};
#endif // CORE_M4

#ifdef CORE_M0
static const IRQn_Type m_apsBoard_GpioInt_IRQ[eBOARD_INT_PIN_NUMBERS] =
{
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
    PIN_INT4_IRQn,
};
#endif // CORE_M0

// ==============================================================================
// FUNCTION NAME: GPIO_INTERRUPT_HANDLER
// DESCRIPTION:
//
//
// Params:
// eBOARD_INT_PIN eIntPin:
//
// Returns:
//
//
// modification history
// --------------------
// 04/01/2014, Leo Create
// --------------------
// ==============================================================================
static void GPIO_Interrupt_Handler(eBOARD_INT_PIN eIntPin)
{
    if(GETBIT(Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT), PININTCH(eIntPin)))
    {
        if(NULL != fpGpioIntRiseCallback[eIntPin])
        {
            fpGpioIntRiseCallback[eIntPin]();
        }

        Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT, PININTCH(eIntPin));
    }

    if(GETBIT(Chip_PININT_GetFallStates(LPC_GPIO_PIN_INT), PININTCH(eIntPin)))
    {
        if(NULL != fpGpioIntFallCallback[eIntPin])
        {
            fpGpioIntFallCallback[eIntPin]();
        }

        Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT, PININTCH(eIntPin));
    }

    NVIC_ClearPendingIRQ(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[eIntPin].cIntPortNum]);
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_INIT
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 15/01/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Init(void)
{
    BYTE cPinIntCount = 0;
    Board_Gpio_PinMux((sPIN_CFG *)m_asDefaultPinConfig, DEFAULT_PIN_NUMBERS);

    for(cPinIntCount = 0; cPinIntCount < eBOARD_INT_PIN_NUMBERS; cPinIntCount++)
    {
        fpGpioIntRiseCallback[cPinIntCount] = NULL;
        fpGpioIntFallCallback[cPinIntCount] = NULL;
    } // End of For

}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_PINMUX
// DESCRIPTION:
//
//
// Params:
// sPIN_CFG* psPinCfg:
// WORD wPinNumber:
//
// Returns:
//
//
// modification history
// --------------------
// 15/01/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_PinMux(sPIN_CFG *psPinCfg, WORD wPinNumber)
{
    WORD wPinCount = 0;

    for(wPinCount = 0; wPinCount < wPinNumber; wPinCount++)
    {
        Chip_SCU_PinMux(m_asPinMuxCfg[psPinCfg[wPinCount].wNetName].cPinGroup,
                        m_asPinMuxCfg[psPinCfg[wPinCount].wNetName].cPinNumber,
                        psPinCfg[wPinCount].wPinMode,
                        psPinCfg[wPinCount].cPinFunc);

        if(psPinCfg[wPinCount].eDirection != eIO_MODE_NONGPIO)
        {
            if(m_asPinMuxCfg[psPinCfg[wPinCount].wNetName].cGpioPort != 0xFF)
            {
                Chip_GPIO_SetPinDIR(LPC_GPIO_PORT,
                                    m_asPinMuxCfg[psPinCfg[wPinCount].wNetName].cGpioPort,
                                    m_asPinMuxCfg[psPinCfg[wPinCount].wNetName].cGpioPinNumber,
                                    psPinCfg[wPinCount].eDirection);

                if(eIO_MODE_OUTPUT == psPinCfg[wPinCount].eDirection)
                {
                    Board_Gpio_Set((eBOARD_PIN_TABLE)psPinCfg[wPinCount].wNetName, psPinCfg[wPinCount].cPinValue);
                }
            }
        }
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_SET
// DESCRIPTION:
//
//
// Params:
// eBOARD_PIN_TABLE ePin:
// BYTE cValue:
//
// Returns:
//
//
// modification history
// --------------------
// 16/01/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Set(eBOARD_PIN_TABLE ePin, BYTE cValue)
{
    ASSERT(pBOARD_PIN_NUMBERS > ePin);

    if(pBOARD_PIN_NUMBERS > ePin)
    {
        if(cValue)
        {
            Chip_GPIO_SetPinOutHigh(LPC_GPIO_PORT,
                                    m_asPinMuxCfg[ePin].cGpioPort,
                                    m_asPinMuxCfg[ePin].cGpioPinNumber);
        }
        else
        {
            Chip_GPIO_SetPinOutLow(LPC_GPIO_PORT,
                                   m_asPinMuxCfg[ePin].cGpioPort,
                                   m_asPinMuxCfg[ePin].cGpioPinNumber);
        }
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_TOGGLE_SET
// DESCRIPTION:
//
//
// Params:
// eBOARD_PIN_TABLE ePin:
//
// Returns:
//
//
// modification history
// --------------------
// 26/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Toggle_Set(eBOARD_PIN_TABLE ePin)
{
    Chip_GPIO_SetPinToggle(LPC_GPIO_PORT,
                           m_asPinMuxCfg[ePin].cGpioPort,
                           m_asPinMuxCfg[ePin].cGpioPinNumber);
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_GET
// DESCRIPTION:
//
//
// Params:
// eBOARD_PIN_TABLE ePin:
//
// Returns:
//
//
// modification history
// --------------------
// 16/01/2013, Leo Create
// --------------------
// ==============================================================================
BOOL Board_Gpio_Get(eBOARD_PIN_TABLE ePin)
{
    WORD wGpioPortValue = 0xFF;

    ASSERT(pBOARD_PIN_NUMBERS > ePin);

    if(pBOARD_PIN_NUMBERS > ePin)
    {
        wGpioPortValue =  Chip_GPIO_ReadPortBit(LPC_GPIO_PORT,
                                                m_asPinMuxCfg[ePin].cGpioPort,
                                                m_asPinMuxCfg[ePin].cGpioPinNumber);
    }

    return (BOOL)(!!wGpioPortValue);
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_DIR_SET
// DESCRIPTION:
//
//
// Params:
// eBOARD_PIN_TABLE ePin:
// eIO_MODE eMode:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/03/19, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Dir_Set(eBOARD_PIN_TABLE ePin, eIO_MODE eMode)
{
    ASSERT(pBOARD_PIN_NUMBERS > ePin);

    if(pBOARD_PIN_NUMBERS > ePin)
    {
        Chip_GPIO_SetPinDIR(LPC_GPIO_PORT,
                            m_asPinMuxCfg[ePin].cGpioPort,
                            m_asPinMuxCfg[ePin].cGpioPinNumber,
                            eMode);
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_GIMA_PINMUX
// DESCRIPTION:
//
//
// Params:
// sGIMA_MUX_CFG *psGimaMuxCfg:
// WORD wPinNumber:
//
// Returns:
//
//
// modification history
// --------------------
// 31/12/2013, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Gima_PinMux(sGIMA_PIN_CFG *psGimaCfg, WORD wPinNumber)
{
    WORD wPinCount = 0;

    for(wPinCount = 0; wPinCount < wPinNumber; wPinCount++)
    {
        switch(psGimaCfg[wPinCount].eFunc)
        {
            case GIMA_FUNC_CAP:
                ASSERT(psGimaCfg[wPinCount].sGimaPinCfg.cINFUNC < GIMA_CAP_IN_NUMBERS);
                ASSERT(psGimaCfg[wPinCount].sGimaPinCfg.cINFUNC < GIMA_CAP_CHANNEL_NUMBERS);

                Chip_Gima_CAP_IN_PinMux((GIMA_CAP_IN_T)psGimaCfg[wPinCount].sGimaPinCfg.cINFUNC,
                                        (GIMA_CAP_CHANNEL_T)psGimaCfg[wPinCount].sGimaPinCfg.cCHANNEL,
                                        psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                        psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                        psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                        psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                        psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                       );
                break;

            case GIMA_FUNC_SCT_CTIN:
                ASSERT(psGimaCfg[wPinCount].sGimaPinCfg.cINFUNC < GIMA_CTIN_NUMBERS);

                Chip_Gima_CTIN_IN_PinMux((GIMA_CTIN_IN_T)psGimaCfg[wPinCount].sGimaPinCfg.cINFUNC,
                                         psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                         psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                         psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                         psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                         psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                        );
                break;

            case GIMA_FUNC_ADCHS_TRIGGER:
                Chip_Gima_ADCHS_TRIGGER_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                                  psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                                  psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                                  psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                                  psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                                 );
                break;

            case GIMA_FUNC_EVENTROUTER_13:
                Chip_Gima_EVENTROUTER_13_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                                  );
                break;

            case GIMA_FUNC_EVENTROUTER_14:
                Chip_Gima_EVENTROUTER_14_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                                  );
                break;

            case GIMA_FUNC_EVENTROUTER_16:
                Chip_Gima_EVENTROUTER_16_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                                   psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                                  );
                break;

            case GIMA_FUNC_ADCSTART0:
                Chip_Gima_ADCSTART0_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                              psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                             );
                break;

            case GIMA_FUNC_ADCSTART1:
                Chip_Gima_ADCSTART1_IN_PinMux(psGimaCfg[wPinCount].sGimaPinCfg.bINV,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bEDGE,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bSYNCH,
                                              psGimaCfg[wPinCount].sGimaPinCfg.bPLUSE,
                                              psGimaCfg[wPinCount].sGimaPinCfg.dwSELECT
                                             );
                break;

            default:
                ASSERT_ALWAYS();
                break;
        } // end of switch
    } // end of for
}

// ==============================================================================
// FUNCTION NAME: Board_Gpio_Clock_Set
// DESCRIPTION:
//
//
// Params:
// sPIN_CFG *psPinCfg:
// WORD wPinNumber:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/05/03, Larry Create
// --------------------
// ==============================================================================
void Board_Gpio_Clock_Set(sPIN_CLK_CFG *psPinCfg, WORD wPinNumber)
{
    BYTE cPinCount = 0;

    for(cPinCount = 0; cPinCount < wPinNumber; cPinCount++)
    {
        Chip_SCU_ClockPinMux(m_asCLK_PinMuxCfg[psPinCfg[cPinCount].eNetName].cPinNumber,
                             psPinCfg[cPinCount].wPinMode,
                             psPinCfg[cPinCount].cPinFunc);

    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_INTPIN_SELECT
// DESCRIPTION:
//
//
// Params:
// sGPIO_INT_CFG sIntPinTable:
//
// Returns:
//
//
// modification history
// --------------------
// 02/01/2014, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_IntPin_Select(sGPIO_INT_CFG *psIntPinTable, WORD wPinNumber)
{
    WORD wPinCount = 0;

    for(wPinCount = 0; wPinCount < wPinNumber; wPinCount++)
    {
        ASSERT(psIntPinTable[wPinCount].cPin < pBOARD_PIN_NUMBERS);
        ASSERT(psIntPinTable[wPinCount].cIntPortNum < eBOARD_INT_PIN_NUMBERS);

        Chip_SCU_GPIOIntPinSel(psIntPinTable[wPinCount].cIntPortNum,
                               m_asPinMuxCfg[psIntPinTable[wPinCount].cPin].cGpioPort,
                               m_asPinMuxCfg[psIntPinTable[wPinCount].cPin].cGpioPinNumber);
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIOINT_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_PIN_TABLE eIntPin:
// eINT_MODE eMode:
// eINT_EDGE eEdge:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/16, Leo Create
// --------------------
// ==============================================================================
void Board_GpioInt_Init(eBOARD_PIN_TABLE eIntPin, eINT_MODE eMode, eINT_EDGE eEdge)
{
    BYTE cPinCount = 0;

    for(cPinCount = 0; cPinCount < GPIO_INT_PIN_NUMBER; cPinCount++)
    {
        if(m_asGpio_INT_Table[cPinCount].cPin == eIntPin)
        {
            break;
        }
    }

    ASSERT(cPinCount < GPIO_INT_PIN_NUMBER);

    if(cPinCount >= GPIO_INT_PIN_NUMBER)
    {
        return;
    }

    if(eINT_MODE_EDGE == eMode)
    {
        Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
    }
    else
    {
        Chip_PININT_SetPinModeLevel(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
    }

    switch(eEdge)
    {
        case eINT_EDGE_RISE:
            Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            Chip_PININT_DisableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            break;

        case eINT_EDGE_FALL:
            Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            break;

        case eINT_EDGE_BOTH_EDGE:
            Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch

    Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
    NVIC_ClearPendingIRQ(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[cPinCount].cIntPortNum]);

    NVIC_SetPriority(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[cPinCount].cIntPortNum], eINT_PRIORITY_LEVEL_5);
    NVIC_EnableIRQ(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[cPinCount].cIntPortNum]);
}

// ==============================================================================
// FUNCTION NAME: Board_GpioInt_DeInit
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/08/08, Leo Create
// --------------------
// ==============================================================================
void Board_GpioInt_DeInit(void)
{
    BYTE cPinCount = 0;

    for(cPinCount = 0; cPinCount < GPIO_INT_PIN_NUMBER; cPinCount++)
    {
        Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(m_asGpio_INT_Table[cPinCount].cIntPortNum));
        NVIC_ClearPendingIRQ(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[cPinCount].cIntPortNum]);
        NVIC_DisableIRQ(m_apsBoard_GpioInt_IRQ[m_asGpio_INT_Table[cPinCount].cIntPortNum]);
    }
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_REGISTER_RISECALLBACK
// DESCRIPTION:
//
//
// Params:
// eBOARD_INT_PIN eIntPin:
// void (*fpCallback)(void):
//
// Returns:
//
//
// modification history
// --------------------
// 02/01/2014, Leo Create
// --------------------
// ==============================================================================
void Board_GpioInt_Register_RiseCallback(eBOARD_PIN_TABLE eIntPin, void (*fpCallback)(void))
{
    BYTE cPinCount = 0;

    for(cPinCount = 0; cPinCount < GPIO_INT_PIN_NUMBER; cPinCount++)
    {
        if(m_asGpio_INT_Table[cPinCount].cPin == eIntPin)
        {
            break;
        }
    }

    ASSERT(cPinCount < GPIO_INT_PIN_NUMBER);

    if(cPinCount >= GPIO_INT_PIN_NUMBER)
    {
        return;
    }

    fpGpioIntRiseCallback[m_asGpio_INT_Table[cPinCount].cIntPortNum] = fpCallback;
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_REGISTER_FALLCALLBACK
// DESCRIPTION:
//
//
// Params:
// eBOARD_INT_PIN eIntPin:
// void (*fpCallback)(void):
//
// Returns:
//
//
// modification history
// --------------------
// 02/01/2014, Leo Create
// --------------------
// ==============================================================================
void Board_GpioInt_Register_FallCallback(eBOARD_PIN_TABLE eIntPin, void (*fpCallback)(void))
{
    BYTE cPinCount = 0;

    for(cPinCount = 0; cPinCount < GPIO_INT_PIN_NUMBER; cPinCount++)
    {
        if(m_asGpio_INT_Table[cPinCount].cPin == eIntPin)
        {
            break;
        }
    }

    ASSERT(cPinCount < GPIO_INT_PIN_NUMBER);

    if(cPinCount >= GPIO_INT_PIN_NUMBER)
    {
        return;
    }

    fpGpioIntFallCallback[m_asGpio_INT_Table[cPinCount].cIntPortNum] = fpCallback;
}

void GPIO0_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN0);
}

void GPIO1_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN1);
}

void GPIO2_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN2);
}

void GPIO3_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN3);
}

void GPIO4_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN4);
}

void GPIO5_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN5);
}

void GPIO6_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN6);
}

void GPIO7_IRQHandler(void)
{
    GPIO_Interrupt_Handler(eBOARD_INT_PIN7);
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_DUMP_IO_CFG
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/05/01, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Dump_IO_Cfg(void)
{
    WORD wPinCount = 0;

    for(wPinCount = 0; wPinCount < pBOARD_PIN_NUMBERS; wPinCount++)
    {
        if(0xFF != m_asPinMuxCfg[wPinCount].cGpioPort)
        {
            LOG_MSG(eDB_MSK_LIST_SYSCTRL, "(%02d,%02d),0x%04X\r\n", m_asPinMuxCfg[wPinCount].cGpioPort, m_asPinMuxCfg[wPinCount].cGpioPinNumber, LPC_SCU->SFSP[m_asPinMuxCfg[wPinCount].cGpioPort][m_asPinMuxCfg[wPinCount].cGpioPinNumber]);
        }
    } // End of For
}

// ==============================================================================
// FUNCTION NAME: BOARD_GPIO_DUMP_PIN_VALUE
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/05/01, Leo Create
// --------------------
// ==============================================================================
void Board_Gpio_Dump_Pin_Value(void)
{
    WORD wPinCount = 0;

    for(wPinCount = 0; wPinCount < 8; wPinCount++)
    {
        LOG_MSG(eDB_MSK_LIST_SYSCTRL, "(%02d),0x%04X\r\n", wPinCount, LPC_GPIO_PORT->PIN[wPinCount]);
    } // End of For
}

// ==============================================================================
// FUNCTION NAME: Board_Gpio_SSP0_CS_Set
// DESCRIPTION:
//
//
// Params:
// BYTE cVlaue:
//
// Returns:
//
//
// Modification History
// --------------------
// 2018/07/09, Chris Chang Create
// --------------------
// ==============================================================================
void Board_Gpio_SSP0_CS_Set(BYTE cVlaue)
{
    //Board_Gpio_Set(pGS2971_SPI_CS, cVlaue);
}


// ==============================================================================
// FUNCTION NAME: Board_Gpio_SSP1_CS_Set
// DESCRIPTION:
//
//
// Params:
// BYTE cVlaue:
//
// Returns:
//
//
// Modification History
// --------------------
// 2016/04/25, Larry Create
// --------------------
// ==============================================================================
void Board_Gpio_SSP1_CS_Set(BYTE cVlaue)
{
    //Board_Gpio_Set(pGS2971_SPI_CS, cVlaue);
}

#if 0
void Board_IT6613_HPD_Detect()
{
    BYTE cVlaue = LOW;
    cVlaue = Board_Gpio_Get(pIT6613_HPD);
    if(cVlaue == HIGH)
    {
     //LOG_MSG(eDB_MSK_LIST_IT6634, "Board_IT6613_HPD_Detect, pIT6613_HPD = %d\r\n", cVlaue);
    }
}
#endif // 0

void Board_IT6563_Detect()
{
    BYTE cVlaue = LOW;
    cVlaue = Board_Gpio_Get(pMCU_DP_DET);
    if(cVlaue == HIGH)
    {
      //LOG_MSG(eDB_MSK_LIST_IT6634, "Board_IT6563_Gpio_Detect, pMCU_DP_DET = %d\r\n", cVlaue);
    }
}

#if 0
void Board_IT6563_HPD_Change()
{
//     BYTE cVlaue = LOW;
     Board_Gpio_Dir_Set(pIT6613_HPD, eIO_MODE_OUTPUT);
     Board_Gpio_Set(pIT6613_HPD, LOW);
     utilDelayMs(10);
     Board_Gpio_Dir_Set(pIT6613_HPD, eIO_MODE_INPUT);
}
#endif // 0

// ==============================================================================
// FUNCTION NAME: Board_IT6563_Toggle
// DESCRIPTION:
//
//
// Params:
// void:
//
// Returns:
//
//
// Modification History
// --------------------
// 2017/04/25, Larry Create
// --------------------
// ==============================================================================
void Board_IT6563DP_Toggle(BOOL bEnable)
{
    //#warning "GPIO pGS2961A_RST ??"
    if(bEnable)
    {
      // Board_Gpio_Set(pDP_3_3V_EN, HIGH);
    }
    else
    {
       // Board_Gpio_Set(pDP_3_3V_EN, LOW);
    }
        
    
    //pIT6563_3V3_EN, HIGH
}


void Board_MainBoard_Video_Toggle(BOOL bEnable)
{
 
    if(bEnable)
    {
      // Board_Gpio_Set(pVIDEO_INT, LOW);
    }
    else
    {
       // Board_Gpio_Set(pVIDEO_INT, HIGH);
    }       
}
void Board_SCDC_Enable_Toggle(BOOL bEnable)
{
 
    if(bEnable)
    {
      Board_Gpio_Set(pSCDC_ENABLE, HIGH);
    }
    else
    {
       Board_Gpio_Set(pSCDC_ENABLE, LOW);
    }       
}

