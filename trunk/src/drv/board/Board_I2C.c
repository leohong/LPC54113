// ==============================================================================
// FILE NAME: BOARD_I2C.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 03/01/2013, Leohong written
// --------------------
// ==============================================================================


#include "Board.h"
#include "Board_GPIO.h"
#include "Board_I2C.h"
#include "Board_Sw_I2C.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"
#include "utilMathAPI.h"

#define I2C_SLAVE_ADDR      0x66
#define I2C_RX_BUFFER_SIZE  2048 //A70LH_Larry_0002
#define I2C_TX_BUFFER_SIZE  2048

// I2C Master driving by Interrupt Mode
#define I2CM_INT_MODE

#define I2C_TIME_OUT (100)

// ==============================================================================
// Local variable Section
// ==============================================================================

//static BYTE m_cSLV_ADD =    0;
//static BYTE m_cRegFound =   FALSE;

volatile static RINGBUFF_T m_sI2C_Slave_Rx_RingBuffer; //A70LH_Larry_0002
volatile static RINGBUFF_T m_sI2C_Slave_Tx_RingBuffer; //A70LH_Larry_0002

volatile static BYTE m_acI2C_Slave_Rx_Buffer[I2C_RX_BUFFER_SIZE]; //A70LH_Larry_0002
volatile static BYTE m_acI2C_Slave_Tx_Buffer[I2C_TX_BUFFER_SIZE]; //A70LH_Larry_0002
// static WORD m_wTxCount = 0;

#ifdef I2CM_INT_MODE
volatile static I2CM_XFER_T  m_asi2cmXferRec[eBOARD_I2C_NUMBERS];
#endif // I2CM_INT_MODE


// ==============================================================================
// Initial Tables
// ==============================================================================
StaticSemaphore_t m_axI2cSemaphoreBuffer[eBOARD_I2C_NUMBERS];

static sBOARD_I2C_CFG m_asI2cCfg[eBOARD_I2C_NUMBERS] =
{
    {// Bus 0
        eI2C_MODE_HW,                               // I2C mode
        100,                                        // wKHz;
        FALSE,                                      // cSlaveEnable;
        0x66,                                       // cSlaveAddress;
        LPC_I2C0,                                   // LPC_I2C_T
        I2C0_IRQn,                                  // IRQn_Type
        eCOUNTER_TYPE_I2C_0_TIMEOUT,
        &m_axI2cSemaphoreBuffer[eBOARD_I2C_BUS0],   // StaticSemaphore_t
        NULL,                                       // SemaphoreHandle_t
    },
    {// Bus 1
        eI2C_MODE_HW,                               // I2C mode
        100,                                        // wKHz;
        FALSE,                                      // cSlaveEnable;
        0x66,                                       // cSlaveAddress;
        LPC_I2C1,                                   // LPC_I2C_T
        I2C1_IRQn,                                  // IRQn_Type
        eCOUNTER_TYPE_I2C_1_TIMEOUT,
        &m_axI2cSemaphoreBuffer[eBOARD_I2C_BUS1],   // StaticSemaphore_t
        NULL,                                       // SemaphoreHandle_t
    },
#ifdef ENABLE_SW_I2C
    {// Bus 2
        eI2C_MODE_SW,                               // I2C mode
        100,                                        // wKHz;
        FALSE,                                      // cSlaveEnable;
        0x66,                                       // cSlaveAddress;
        NULL,                                       // LPC_I2C_T
        I2C1_IRQn,                                  // IRQn_Type
        &m_axI2cSemaphoreBuffer[eBOARD_I2C_BUS2],   // StaticSemaphore_t
        NULL,                                       // SemaphoreHandle_t
    },
#endif // ENABLE_SW_I2C
};

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_GETSTATE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/20, Leo Create
// --------------------
// ==============================================================================
static BYTE Board_I2C_GetState(eBOARD_I2C eBus)
{
    return (BYTE)(m_asI2cCfg[eBus].psI2CRegCfg->STAT & I2C_STAT_CODE_BITMASK);
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SLAVESTATEHANDLER
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/26, Leo Create
// --------------------
// ==============================================================================
static void Board_I2C_SlaveStateHandler(eBOARD_I2C eBus)
{
    switch(Board_I2C_GetState(eBus))
    {
        case I2C_I2STAT_S_RX_PRE_SLA_DAT_ACK:           /* 0x80 SLA: Data received + ACK sent */
        case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_ACK:       /* 0x90 GC: Data received + ACK sent */
            RingBuffer_Insert((RINGBUFF_T *)&m_sI2C_Slave_Rx_RingBuffer, (BYTE *)&m_asI2cCfg[eBus].psI2CRegCfg->DAT);
            break;

        case I2C_I2STAT_S_RX_SLAW_ACK:                  /* 0x60 Own SLA+W received */
        case I2C_I2STAT_S_RX_ARB_LOST_M_SLA:            /* 0x68 Own SLA+W received after losing arbitration */
        case I2C_I2STAT_S_RX_GENCALL_ACK:               /* 0x70 GC+W received */
        case I2C_I2STAT_S_RX_ARB_LOST_M_GENCALL:        /* 0x78 GC+W received after losing arbitration */
            //m_sI2C_Slave_Rx_RingBuffer.head = 0; //A70_Larry_0155 mask //A70LH_Larry_0002
            //m_sI2C_Slave_Rx_RingBuffer.tail = 0; //A70_Larry_0155 mask //A70LH_Larry_0002
            break;

        case I2C_I2STAT_S_TX_SLAR_ACK:                  /* 0xA8 SLA+R received */
        case I2C_I2STAT_S_TX_ARB_LOST_M_SLA:            /* 0xB0 SLA+R received after losing arbitration */
            m_sI2C_Slave_Tx_RingBuffer.tail = 0;

        //m_wTxCount = 0;
        case I2C_I2STAT_S_TX_DAT_ACK:                   /* 0xB8 DATA sent and ACK received */
        {
            BYTE cData = 0;

            if(RingBuffer_Pop((RINGBUFF_T *)&m_sI2C_Slave_Tx_RingBuffer, &cData)) //A70LH_Larry_0002
            {
                m_asI2cCfg[eBus].psI2CRegCfg->DAT = cData;
            }

            //m_asI2cCfg[eBus].psI2CRegCfg->DAT = m_acI2C_Slave_TxBuffer[m_wTxCount++];
        }
        break;

        case I2C_I2STAT_S_TX_DAT_NACK:                  /* 0xC0 Data transmitted and NAK received */
        case I2C_I2STAT_S_TX_LAST_DAT_ACK:              /* 0xC8 Last data transmitted and ACK received */
            break;

        case I2C_I2STAT_S_RX_PRE_SLA_DAT_NACK:          /* 0x88 SLA: Data received + NAK sent */
        case I2C_I2STAT_S_RX_PRE_GENCALL_DAT_NACK:      /* 0x98 GC: Data received + NAK sent */
        case I2C_I2STAT_S_RX_STA_STO_SLVREC_SLVTRX:     /* 0xA0 STOP/Repeated START condition received */
            break;
    }

    /* Set clear control flags */
    m_asI2cCfg[eBus].psI2CRegCfg->CONSET = I2C_CON_AA;
    m_asI2cCfg[eBus].psI2CRegCfg->CONCLR = I2C_CON_SI;

    NVIC_ClearPendingIRQ(m_asI2cCfg[eBus].eI2CIrq);
}

// ==============================================================================
// FUNCTION NAME: I2C0_IRQHANDLER
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
// 2014/05/03, Leo Create
// --------------------
// ==============================================================================
void I2C0_IRQHandler(void)
{
#ifdef I2CM_INT_MODE
    if(TRUE == m_asI2cCfg[eBOARD_I2C_BUS0].cSlaveEnable)
    {
        Board_I2C_SlaveStateHandler(eBOARD_I2C_BUS0);
        //Chip_I2C_SlaveStateHandler(eBOARD_I2C_BUS0);
    }
    else
    {
        /* Call I2CM ISR function with the I2C device and transfer rec */
        Chip_I2CM_XferHandler(LPC_I2C0, (I2CM_XFER_T*)&m_asi2cmXferRec[eBOARD_I2C_BUS0]);
    }
#else
    Board_I2C_SlaveStateHandler(eBOARD_I2C_BUS0);
#endif // I2CM_INT_MODE
}

// ==============================================================================
// FUNCTION NAME: I2C1_IRQHANDLER
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
// 2014/01/26, Leo Create
// --------------------
// ==============================================================================
void I2C1_IRQHandler(void)
{
#ifdef I2CM_INT_MODE
    if(TRUE == m_asI2cCfg[eBOARD_I2C_BUS1].cSlaveEnable)
    {
        Board_I2C_SlaveStateHandler(eBOARD_I2C_BUS1);
        //Chip_I2C_SlaveStateHandler(eBOARD_I2C_BUS1);
    }
    else
    {
        /* Call I2CM ISR function with the I2C device and transfer rec */
        Chip_I2CM_XferHandler(LPC_I2C1, (I2CM_XFER_T*)&m_asi2cmXferRec[eBOARD_I2C_BUS1]);
    }
#else
    Board_I2C_SlaveStateHandler(eBOARD_I2C_BUS1);
#endif // I2CM_INT_MODE
}


#ifndef I2CM_INT_MODE
// ==============================================================================
// FUNCTION NAME: BOARD_I2C_STATECHANGE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_StateChange(eBOARD_I2C eBus)
{
    utilCounterSet(eCOUNTER_TYPE_I2C_TIMEOUT, 5);

    while(Chip_I2CM_StateChanged(m_asI2cCfg[eBus].psI2CRegCfg) == 0)
    {
        if(0 == utilCounterGet(eCOUNTER_TYPE_I2C_TIMEOUT))
        {
            return rcERROR;
        }
    }

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_START
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_Start(eBOARD_I2C eBus)
{
    eRESULT eResult = rcERROR;

    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            Chip_I2CM_ResetControl(m_asI2cCfg[eBus].psI2CRegCfg);
            Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);

            Chip_I2CM_SendStart(m_asI2cCfg[eBus].psI2CRegCfg);

            if(rcERROR == Board_I2C_StateChange(eBus))
            {
                return rcERROR;
            }

            switch(Chip_I2CM_GetCurState(m_asI2cCfg[eBus].psI2CRegCfg))
            {
                case I2C_I2STAT_M_TX_START:
                case I2C_I2STAT_M_TX_RESTART:
                    eResult = rcSUCCESS;
                    break;

                default:
                    eResult = rcERROR;
                    break;
            }

            break;

        case eBOARD_I2C_BUS2:
            eResult = Board_I2C_SW_Start(eBus);
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_WRITEADDRESS
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// BYTE cData:
// BOOL bNonAck:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_WriteAddress(eBOARD_I2C eBus, BYTE cData, BOOL bNonAck)
{
    eRESULT eResult = rcERROR;

    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);

            Chip_I2CM_WriteByte(m_asI2cCfg[eBus].psI2CRegCfg, cData);

            if(rcERROR == Board_I2C_StateChange(eBus))
            {
                return eResult;
            }

            switch(Chip_I2CM_GetCurState(m_asI2cCfg[eBus].psI2CRegCfg))
            {
                case I2C_I2STAT_M_TX_SLAW_ACK:
                case I2C_I2STAT_M_RX_SLAR_ACK:
                    eResult = rcSUCCESS;
                    break;

                case I2C_I2STAT_M_TX_SLAW_NACK:
                case I2C_I2STAT_M_RX_SLAR_NACK:
                    if(bNonAck)
                    {
                        eResult = rcSUCCESS;
                    }
                    else
                    {
                        eResult =  rcERROR;
                    }

                    break;

                default:
                    eResult =  rcERROR;
                    break;
            }

            break;

        case eBOARD_I2C_BUS2:
            eResult = Board_I2C_SW_SendData(eBus, cData, bNonAck);
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_WRITEBYTE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// BYTE cData:
// BOOL bNonAck:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_WriteByte(eBOARD_I2C eBus, BYTE cData, BOOL bNonAck)
{
    eRESULT eResult = rcERROR;
    //BYTE cI2C_Status = 0;

    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);

            Chip_I2CM_WriteByte(m_asI2cCfg[eBus].psI2CRegCfg, cData);

            if(rcERROR == Board_I2C_StateChange(eBus))
            {
                return eResult;
            }

            switch(Chip_I2CM_GetCurState(m_asI2cCfg[eBus].psI2CRegCfg))
            {
                case I2C_I2STAT_M_TX_DAT_ACK:
                    eResult =  rcSUCCESS;
                    break;

                case I2C_I2STAT_M_TX_DAT_NACK:
                    if(bNonAck)
                    {
                        eResult = rcSUCCESS;
                    }
                    else
                    {
                        eResult = rcERROR;
                    }

                    break;

                default:
                    eResult = rcERROR;
                    break;
            }

            break;

        case eBOARD_I2C_BUS2:
            eResult = Board_I2C_SW_SendData(eBus, cData, bNonAck);
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_READBYTE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// BOOL bNonAck:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static eRESULT Board_I2C_ReadByte(eBOARD_I2C eBus, BYTE *pcData, BOOL bNonAck)
{
    eRESULT eResult = rcERROR;

    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            if(bNonAck)
            {
                m_asI2cCfg[eBus].psI2CRegCfg->CONCLR = I2C_CON_AA;
            }
            else
            {
                m_asI2cCfg[eBus].psI2CRegCfg->CONSET = I2C_CON_AA;
            }

            Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);

            eResult = Board_I2C_StateChange(eBus);

            if(rcERROR == eResult)
            {
                return rcERROR;
            }

            *pcData = Chip_I2CM_ReadByte(m_asI2cCfg[eBus].psI2CRegCfg);
            break;

        case eBOARD_I2C_BUS2:
            eResult = Board_I2C_SW_ReadData(eBus, pcData, bNonAck);
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_STOP
// DESCRIPTION:
//
//
// Params:
// BYTE eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 05/01/2014, Leo Create
// --------------------
// ==============================================================================
static void Board_I2C_Stop(eBOARD_I2C eBus)
{
    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);
            Chip_I2CM_SendStop(m_asI2cCfg[eBus].psI2CRegCfg);
            break;

        case eBOARD_I2C_BUS2:
            Board_I2C_SW_Stop(eBus);
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch
}

#endif // I2CM_INT_MODE

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SLAVEADD_SET
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// I2C_SLAVE_ID eSlaveId:
// BYTE cAddress:
// BYTE cMask:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/14, Leo Create
// --------------------
// ==============================================================================
static void Board_I2C_SlaveAdd_Set(eBOARD_I2C eBus, I2C_SLAVE_ID eSlaveId, BYTE cAddress, BYTE cMask)
{
    switch(eSlaveId)
    {
        case I2C_SLAVE_0:
            m_asI2cCfg[eBus].psI2CRegCfg->ADR0 = cAddress;
            m_asI2cCfg[eBus].psI2CRegCfg->MASK[0] = cMask;
            break;

        case I2C_SLAVE_1:
            m_asI2cCfg[eBus].psI2CRegCfg->ADR1 = cAddress;
            m_asI2cCfg[eBus].psI2CRegCfg->MASK[1] = cMask;
            break;

        case I2C_SLAVE_2:
            m_asI2cCfg[eBus].psI2CRegCfg->ADR2 = cAddress;
            m_asI2cCfg[eBus].psI2CRegCfg->MASK[2] = cMask;
            break;

        case I2C_SLAVE_3:
            m_asI2cCfg[eBus].psI2CRegCfg->ADR3 = cAddress;
            m_asI2cCfg[eBus].psI2CRegCfg->MASK[3] = cMask;
            break;

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_ENABLE_SLAVEMODE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/14, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_Enable_SlaveMode(eBOARD_I2C eBus)
{
    /* Reset STA, STO, SI */
    m_asI2cCfg[eBus].psI2CRegCfg->CONCLR = I2C_CON_SI | I2C_CON_STO | I2C_CON_STA;

    /* Enter to Master Transmitter mode */
    m_asI2cCfg[eBus].psI2CRegCfg->CONSET = I2C_CON_I2EN | I2C_CON_AA;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_DISABLE_SLAVEMODE
// DESCRIPTION:
//
//
// Params:
// BYTE eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/02/06, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_Disable_SlaveMode(eBOARD_I2C eBus)
{
    /* Enter to Master Transmitter mode */
    m_asI2cCfg[eBus].psI2CRegCfg->CONCLR |= I2C_I2CONCLR_AAC;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_INIT
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/10/28, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_Init(eBOARD_I2C eBus)
{
    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
        {
            if(NULL == m_asI2cCfg[eBus].xSemaphore)
            {
                m_asI2cCfg[eBus].xSemaphore = xSemaphoreCreateMutexStatic(m_asI2cCfg[eBus].pxSemaphoreBuffer);
            }

            /* Initialize I2C */
            if(eBOARD_I2C_BUS0 == eBus)
            {
                if(m_asI2cCfg[eBus].wKHz > 400)
                {
                    Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
                }
                else
                {
                    Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
                }
            }

            Chip_I2CM_Init(m_asI2cCfg[eBus].psI2CRegCfg);
            Chip_I2CM_SetBusSpeed(m_asI2cCfg[eBus].psI2CRegCfg, I2CBD_K(m_asI2cCfg[eBus].wKHz));

            if(m_asI2cCfg[eBus].cSlaveEnable)
            {
                Board_I2C_SlaveAdd_Set(eBus, I2C_SLAVE_0, m_asI2cCfg[eBus].cSlaveAddress, 0);
                Board_I2C_Enable_SlaveMode(eBus);

                RingBuffer_Init((RINGBUFF_T *)&m_sI2C_Slave_Rx_RingBuffer, (uint8_t *)m_acI2C_Slave_Rx_Buffer, 1, I2C_RX_BUFFER_SIZE);
                RingBuffer_Init((RINGBUFF_T *)&m_sI2C_Slave_Tx_RingBuffer, (uint8_t *)m_acI2C_Slave_Tx_Buffer, 1, I2C_TX_BUFFER_SIZE);
                //m_wTxCount = 0;

                NVIC_ClearPendingIRQ(m_asI2cCfg[eBus].eI2CIrq);
                NVIC_SetPriority(m_asI2cCfg[eBus].eI2CIrq, eINT_PRIORITY_LEVEL_2);
                NVIC_EnableIRQ(m_asI2cCfg[eBus].eI2CIrq);
            }
            else
            {
#ifdef I2CM_INT_MODE
                NVIC_SetPriority(m_asI2cCfg[eBus].eI2CIrq, eINT_PRIORITY_LEVEL_2);
                Chip_I2C_SetMasterEventHandler((I2C_ID_T)eBus, Chip_I2C_EventHandler);
                NVIC_EnableIRQ(m_asI2cCfg[eBus].eI2CIrq);
#else
                NVIC_DisableIRQ(m_apsBoard_I2C_IRQ[psI2CCgf->eBus]);
#endif // I2CM_INT_MODE
            }
        }
        break;
        
#ifdef ENABLE_SW_I2C
        case eBOARD_I2C_BUS2:
        {
            m_asI2cCfg[eBus].xSemaphore = xSemaphoreCreateMutexStatic(m_asI2cCfg[eBus].pxSemaphoreBuffer);

            Board_I2C_SW_Speed(eBus, (DWORD)m_asI2cCfg[eBus].wKHz);
            Board_I2C_SW_Reset(eBus);
        }
        break;
#endif // ENABLE_SW_I2C
        default:
            break;
    }
        
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_DEINIT
// DESCRIPTION:
//
//
// Params:
// BYTE eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 25/11/2013, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_DeInit(eBOARD_I2C eBus)
{
    switch(eBus)
    {
        case eBOARD_I2C_BUS0:
        case eBOARD_I2C_BUS1:
            {
                /* Disable Interrupt for I2C channel */
                NVIC_DisableIRQ(m_asI2cCfg[eBus].eI2CIrq);
                Chip_I2CM_DeInit(m_asI2cCfg[eBus].psI2CRegCfg);
            }
            break;

#ifdef ENABLE_SW_I2C
        case eBOARD_I2C_BUS2:
            break;
#endif // ENABLE_SW_I2C

        default:
            ASSERT_ALWAYS();
            break;
    } // End of Switch
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_Bus_Reset
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/22, Leo
// --------------------
// ==============================================================================
void Board_I2C_Bus_Reset(eBOARD_I2C eBus)
{
    Chip_I2CM_ResetControl(m_asI2cCfg[eBus].psI2CRegCfg);
    Chip_I2CM_ClearSI(m_asI2cCfg[eBus].psI2CRegCfg);
    Chip_I2CM_SendStop(m_asI2cCfg[eBus].psI2CRegCfg);
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_Master_Write
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// BYTE cDeviceAddress:
// WORD wRegister:
// WORD wDataSz:
// BYTE *pcData:
// BYTE cFlag:
//
// Returns:
//
//
// modification history
// --------------------
// 2019/11/22, Leo
// --------------------
// ==============================================================================
eRESULT Board_I2C_Master_Write(eBOARD_I2C eBus,
                               BYTE cDeviceAddress,
                               WORD wRegister,
                               WORD wDataSz,
                               BYTE *pcData,
                               BYTE cFlag)
{
#ifdef I2CM_INT_MODE
    eRESULT eResult = rcERROR;

    if(NULL != m_asI2cCfg[eBus].xSemaphore)
    {
        //if(pdPASS == xSemaphoreTake(m_asI2cCfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_asI2cCfg[eBus].xSemaphore, portMAX_DELAY))
        {
            switch(eBus)
            {
                case eBOARD_I2C_BUS0:
                case eBOARD_I2C_BUS1:
                {
                    BYTE acDummy[1024];
                    WORD wCount = 0;

                    if(cFlag & I2C_REG_NEED)
                    {
                        if(cFlag & I2C_WORD_REG)
                        {
                            acDummy[0] = (0xFF00 & wRegister) >> 8; //High Byte
                            acDummy[1] = (0x00FF & wRegister);      //Low Byte

                            m_asi2cmXferRec[eBus].txSz = 2;
                        }
                        else
                        {
                            acDummy[0] = (0x00FF & wRegister);      //Low Byte
                            m_asi2cmXferRec[eBus].txSz = 1;
                        }
                    }

                    for(wCount = 0; wCount < wDataSz; wCount++)
                    {
                        acDummy[m_asi2cmXferRec[eBus].txSz + wCount] = pcData[wCount];
                    }

                    /* Setup I2C transfer record */
                    m_asi2cmXferRec[eBus].slaveAddr = cDeviceAddress >> 1;

                    if(TRUE == (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK))
                    {
                        m_asi2cmXferRec[eBus].options = I2CM_XFER_OPTION_IGNORE_NACK;
                    }
                    else
                    {
                        m_asi2cmXferRec[eBus].options = 0;
                    }

                    m_asi2cmXferRec[eBus].status = 0;
                    m_asi2cmXferRec[eBus].txSz += wDataSz;
                    m_asi2cmXferRec[eBus].rxSz = 0;
                    m_asi2cmXferRec[eBus].txBuff = acDummy;
                    m_asi2cmXferRec[eBus].rxBuff = NULL;
                    Chip_I2CM_Xfer(m_asI2cCfg[eBus].psI2CRegCfg, (I2CM_XFER_T*)&m_asi2cmXferRec[eBus]);

                    utilCounterSet(m_asI2cCfg[eBus].eTimer, I2C_TIME_OUT);

                    /* Test for still transferring data */
                    while (m_asi2cmXferRec[eBus].status == I2CM_STATUS_BUSY)
                    {
                        /* Sleep until next interrupt */
                        // __WFI();
                        if(0 == utilCounterGet(m_asI2cCfg[eBus].eTimer))
                        {
                            Board_I2C_Bus_Reset(eBus);
                            LOG_MSG(eDB_MSK_LIST_HOST, "I2C Bus%d 0x0%02X TIME OUT\r\n", eBus, cDeviceAddress);
                            break;
                        }
                    }

                    switch(m_asi2cmXferRec[eBus].status)
                    {
                        case I2CM_STATUS_OK:        //0x00
                            eResult = rcSUCCESS;
                            break;

                        case I2CM_STATUS_ERROR:     //0x01
                        case I2CM_STATUS_NAK:       //0x02
                        case I2CM_STATUS_BUS_ERROR: //0x03
                        case I2CM_STATUS_SLAVE_NAK: //0x04
                        case I2CM_STATUS_ARBLOST:   //0x05
                        case I2CM_STATUS_BUSY:      //0xFF
                        default:
                            break;
                    }
                }
                break;

#ifdef ENABLE_SW_I2C
                case eBOARD_I2C_BUS2:
                {
                    BYTE cAddress = cDeviceAddress;
                    BYTE acRegister[2] = {0};

                    ASSERT(eBus < eBOARD_I2C_NUMBERS);

                    if(rcERROR == Board_I2C_Start(eBus))
                    {
                        Board_I2C_Stop(eBus);
                        goto I2C_WR_END;
                    }

                    cAddress = cDeviceAddress & 0xFE;

                    if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                    {
                        Board_I2C_Stop(eBus);
                        goto I2C_WR_END;
                    }

                    if(cFlag & I2C_REG_NEED)
                    {
                        if(cFlag & I2C_WORD_REG)
                        {
                            acRegister[0] = (0xFF00 & wRegister) >> 8; //High Byte
                            acRegister[1] = (0x00FF & wRegister);     //Low Byte

                            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                            {
                                Board_I2C_Stop(eBus);
                                goto I2C_WR_END;
                            }

                            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[1], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                            {
                                Board_I2C_Stop(eBus);
                                goto I2C_WR_END;
                            }
                        }
                        else
                        {
                            acRegister[0] = (0x00FF & wRegister);  //Low Byte

                            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                            {
                                Board_I2C_Stop(eBus);
                                goto I2C_WR_END;
                            }
                        }
                    }

                    while(wDataSz)
                    {
                        if(rcERROR == Board_I2C_WriteByte(eBus, *(pcData++), (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                        {
                            Board_I2C_Stop(eBus);
                            goto I2C_WR_END;
                        }

                        wDataSz--;
                    }

                    Board_I2C_Stop(eBus);

                    eResult = rcSUCCESS;
                }
I2C_WR_END:
                break;
#endif // ENABLE_SW_I2C

                default:
                    ASSERT_ALWAYS();
                    break;
            } // End of Switch

            xSemaphoreGive(m_asI2cCfg[eBus].xSemaphore);
        }
    }

    return eResult;
#else
    BYTE cAddress = cDeviceAddress;
    BYTE acRegister[2] = {0};

    ASSERT(eBus < eBOARD_I2C_NUMBERS);

    __disable_irq();

    if(rcERROR == Board_I2C_Start(eBus))
    {
        Board_I2C_Stop(eBus);
         __enable_irq();
        return rcERROR;
    }

    cAddress = cDeviceAddress & 0xFE;

    if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
    {
        Board_I2C_Stop(eBus);
        __enable_irq();
        return rcERROR;
    }

    if(cFlag & I2C_REG_NEED)
    {
        if(cFlag & I2C_WORD_REG)
        {
            acRegister[0] = (0xFF00 & wRegister) >> 8; //High Byte
            acRegister[1] = (0x00FF & wRegister);     //Low Byte

            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
            {
                Board_I2C_Stop(eBus);
                __enable_irq();
                return rcERROR;
            }

            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[1], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
            {
                Board_I2C_Stop(eBus);
                __enable_irq();
                return rcERROR;
            }
        }
        else
        {
            acRegister[0] = (0x00FF & wRegister);  //Low Byte

            if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
            {
                Board_I2C_Stop(eBus);
                __enable_irq();
                return rcERROR;
            }
        }
    }

    while(wDataSz)
    {
        if(rcERROR == Board_I2C_WriteByte(eBus, *(pcData++), (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
        {
            Board_I2C_Stop(eBus);
            __enable_irq();
            return rcERROR;
        }

        wDataSz--;
    }

    Board_I2C_Stop(eBus);

    __enable_irq();

    return rcSUCCESS;
#endif // I2CM_INT_MODE
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_MASTER_READ
// DESCRIPTION:
//
//
// Params:
// BYTE eBus:
// BYTE cDeviceAddress:
// WORD wRegister:
// WORD wDataSz:
// BYTE *pcData:
// BYTE cFlag:
//
// Returns:
//
//
// modification history
// --------------------
// 03/01/2013, Leohong written
// --------------------
// ==============================================================================
eRESULT Board_I2C_Master_Read(eBOARD_I2C eBus,
                              BYTE cDeviceAddress,
                              WORD wRegister,
                              WORD wDataSz,
                              BYTE *pcData,
                              BYTE cFlag)
{
#ifdef I2CM_INT_MODE
    eRESULT eResult = rcERROR;
    BYTE acRegister[2] = {0};

    if(NULL != m_asI2cCfg[eBus].xSemaphore)
    {
        //if(pdPASS == xSemaphoreTake(m_asI2cCfg[eBus].xSemaphore, pdMS_TO_TICKS(dwTimeout)))
        if(pdPASS == xSemaphoreTake(m_asI2cCfg[eBus].xSemaphore, portMAX_DELAY))
        {
            switch(eBus)
            {
                case eBOARD_I2C_BUS0:
                case eBOARD_I2C_BUS1:
                {
                    if(cFlag & I2C_REG_NEED)
                    {
                        if(cFlag & I2C_WORD_REG)
                        {
                            acRegister[0] = (0xFF00 & wRegister) >> 8; //High Byte
                            acRegister[1] = (0x00FF & wRegister);     //Low Byte

                            m_asi2cmXferRec[eBus].txSz = 2;
                            m_asi2cmXferRec[eBus].txBuff = acRegister;
                        }
                        else
                        {
                            acRegister[0] = (0x00FF & wRegister);  //Low Byte

                            m_asi2cmXferRec[eBus].txSz = 1;
                            m_asi2cmXferRec[eBus].txBuff = acRegister;
                        }
                    }

                    /* Setup I2C transfer record */
                    m_asi2cmXferRec[eBus].slaveAddr = cDeviceAddress>>1;
                    m_asi2cmXferRec[eBus].options = 0;
                    m_asi2cmXferRec[eBus].status = 0;
                    m_asi2cmXferRec[eBus].rxSz = wDataSz;
                    m_asi2cmXferRec[eBus].rxBuff = pcData;
                    Chip_I2CM_Xfer(m_asI2cCfg[eBus].psI2CRegCfg, (I2CM_XFER_T*)&m_asi2cmXferRec[eBus]);

                    utilCounterSet(m_asI2cCfg[eBus].eTimer, I2C_TIME_OUT);

                    /* Test for still transferring data */
                    while (m_asi2cmXferRec[eBus].status == I2CM_STATUS_BUSY)
                    {
                        /* Sleep until next interrupt */
                        // __WFI();
                        if(0 == utilCounterGet(m_asI2cCfg[eBus].eTimer))
                        {
                            Board_I2C_Bus_Reset(eBus);
                            LOG_MSG(eDB_MSK_LIST_HOST, "I2C Bus%d 0x0%02X TIME OUT\r\n", eBus, cDeviceAddress);
                            break;
                        }
                    }

                    switch(m_asi2cmXferRec[eBus].status)
                    {
                        case I2CM_STATUS_OK:        //0x00
                            eResult = rcSUCCESS;
                            break;

                        case I2CM_STATUS_ERROR:     //0x01
                        case I2CM_STATUS_NAK:       //0x02
                        case I2CM_STATUS_BUS_ERROR: //0x03
                        case I2CM_STATUS_SLAVE_NAK: //0x04
                        case I2CM_STATUS_ARBLOST:   //0x05
                        case I2CM_STATUS_BUSY:      //0xFF
                        default:
                            break;
                    }
                }
                break;

#ifdef ENABLE_SW_I2C
                case eBOARD_I2C_BUS2:
                {
                    BYTE cAddress = cDeviceAddress;

                    if(rcERROR == Board_I2C_Start(eBus))
                    {
                        Board_I2C_Stop(eBus);
                        goto I2C_RD_END;
                    }

                    if(cFlag & I2C_ADD_TRAN)
                    {
                        cAddress = cDeviceAddress | 0x01;
                    }
                    else
                    {
                        cAddress = cDeviceAddress & 0xFE;
                    }

                    if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                    {
                        Board_I2C_Stop(eBus);
                        goto I2C_RD_END;
                    }

                    if(!(cFlag & I2C_ADD_TRAN)) //A70LH_Larry_0209
                    {
                        if(cFlag & I2C_REG_NEED)
                        {
                            if(cFlag & I2C_WORD_REG)
                            {
                                acRegister[0] = (0xFF00 & wRegister) >> 8; //High Byte
                                acRegister[1] = (0x00FF & wRegister);     //Low Byte

                                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                                {
                                    Board_I2C_Stop(eBus);
                                    goto I2C_RD_END;
                                }

                                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[1], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                                {
                                    Board_I2C_Stop(eBus);
                                    goto I2C_RD_END;
                                }
                            }
                            else
                            {
                                acRegister[0] = (0x00FF & wRegister);  //Low Byte

                                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                                {
                                    Board_I2C_Stop(eBus);
                                    goto I2C_RD_END;
                                }
                            }
                        }

                        if(rcERROR == Board_I2C_Start(eBus))
                        {
                            Board_I2C_Stop(eBus);
                            goto I2C_RD_END;
                        }

                        cAddress = cDeviceAddress | 0x01;

                        if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                        {
                            Board_I2C_Stop(eBus);
                            goto I2C_RD_END;
                        }
                    }

                    while(wDataSz)
                    {
                        BOOL bNonAckFlag = (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK);
                        BYTE cData = 0;

                        if(wDataSz == 1)
                        {
                            bNonAckFlag = TRUE;
                        }

                        if(rcSUCCESS == Board_I2C_ReadByte(eBus, &cData, bNonAckFlag))
                        {
                            *(pcData++) = cData;
                            wDataSz--;
                        }
                        else
                        {
                            Board_I2C_Stop(eBus);
                            goto I2C_RD_END;
                        }
                    }

                    Board_I2C_Stop(eBus);
                    eResult = rcSUCCESS;
                }
I2C_RD_END:
                break;
#endif // ENABLE_SW_I2C

                default:
                    break;
            }
            xSemaphoreGive(m_asI2cCfg[eBus].xSemaphore);
        }
    }

    return eResult;
#else

    BYTE cAddress = cDeviceAddress;
    BYTE acRegister[2] = {0};

    __disable_irq();

    if(rcERROR == Board_I2C_Start(eBus))
    {
        Board_I2C_Stop(eBus);
        __enable_irq();
        return rcERROR;
    }

    if(cFlag & I2C_ADD_TRAN)
    {
        cAddress = cDeviceAddress | 0x01;
    }
    else
    {
        cAddress = cDeviceAddress & 0xFE;
    }

    if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
    {
        Board_I2C_Stop(eBus);
        __enable_irq();
        return rcERROR;
    }

    if(!(cFlag & I2C_ADD_TRAN)) //A70LH_Larry_0209
    {

        if(cFlag & I2C_REG_NEED)
        {
            if(cFlag & I2C_WORD_REG)
            {
                acRegister[0] = (0xFF00 & wRegister) >> 8; //High Byte
                acRegister[1] = (0x00FF & wRegister);     //Low Byte

                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                {
                    Board_I2C_Stop(eBus);
                    __enable_irq();
                    return rcERROR;
                }

                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[1], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                {
                    Board_I2C_Stop(eBus);
                    __enable_irq();
                    return rcERROR;
                }
            }
            else
            {
                acRegister[0] = (0x00FF & wRegister);  //Low Byte

                if(rcERROR == Board_I2C_WriteByte(eBus, acRegister[0], (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
                {
                    Board_I2C_Stop(eBus);

                    __enable_irq();
                    return rcERROR;
                }
            }
        }

        if(rcERROR == Board_I2C_Start(eBus))
        {
            Board_I2C_Stop(eBus);

            __enable_irq();
            return rcERROR;
        }

        cAddress = cDeviceAddress | 0x01;

        if(rcERROR == Board_I2C_WriteAddress(eBus, cAddress, (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK)))
        {
            Board_I2C_Stop(eBus);

            __enable_irq();
            return rcERROR;
        }
    }

    while(wDataSz)
    {
        BOOL bNonAckFlag = (BOOL)GETBIT(cFlag, I2C_IGNORE_ACK);

        if(wDataSz == 1)
        {
            bNonAckFlag = TRUE;
        }

        *(pcData++) = Board_I2C_ReadByte(eBus, bNonAckFlag);
        wDataSz--;
    }

    Board_I2C_Stop(eBus);

    __enable_irq();
    return rcSUCCESS;
#endif // I2CM_INT_MODE
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SLAVE_WRITE
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// WORD wDataSz:
// BYTE *pcTxData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/28, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_Slave_Write(eBOARD_I2C eBus, WORD wDataSz, BYTE *pcTxData)
{
    eRESULT eResult = rcERROR;

    if(wDataSz == RingBuffer_InsertMult((RINGBUFF_T *)&m_sI2C_Slave_Tx_RingBuffer, (BYTE *)pcTxData, wDataSz))
    {
        eResult = rcSUCCESS;
    }

    // Reset output Counter
    //if(m_wTxCount)
    //{
    //   m_wTxCount = 0;
    //}
    //memcpy(m_acI2C_Slave_TxBuffer, pcTxData, wDataSz);
    return eResult;
}

// ==============================================================================
// FUNCTION NAME: BOARD_I2C_SLAVE_READ
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
// WORD wDataSz:
// BYTE *pcRxData:
//
// Returns:
//
//
// modification history
// --------------------
// 2014/01/28, Leo Create
// --------------------
// ==============================================================================
eRESULT Board_I2C_Slave_Read(eBOARD_I2C eBus, WORD wDataSz, BYTE *pcRxData)
{
    eRESULT eResult = rcERROR;

    if(wDataSz)
    {
        eResult = (eRESULT)RingBuffer_Pop((RINGBUFF_T *)&m_sI2C_Slave_Rx_RingBuffer, pcRxData);
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: Board_I2C_Slave_Buffer_Flush
// DESCRIPTION:
//
//
// Params:
// eBOARD_I2C eBus:
//
// Returns:
//
//
// Modification History
// --------------------
// 2014/07/01, Leo Create
// --------------------
// ==============================================================================
void Board_I2C_Slave_Buffer_Flush(eBOARD_I2C eBus)
{
    (void)eBus;
    m_sI2C_Slave_Tx_RingBuffer.head = 0;
    m_sI2C_Slave_Tx_RingBuffer.tail = 0;
}

