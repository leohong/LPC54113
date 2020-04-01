// ==============================================================================
// FILE NAME: HALINPUTCTRL.C
// DESCRIPTION:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
#include "Board_Gpio.h"
#include "Board_Timer.h"
#include "halInputCtrlAPI.h"
#include "halKeyCodeList.h"
#include "utilLogAPI.h"
#include "utilCounterAPI.h"

#define PRINT_OUT_IR_DATA

#define DEBOUNDCETIMER  (50) //50ms
#define PRESSTHRESHOLD  (2)
#define HOLDTHRESHOLD   (10)
#define NO_KEY          (0xFFFF)
#define IRREPEATCOUNT   (5)

static WORD m_wKeyCheckCount;
static WORD m_wLastKey = eKEY_LIST_NUMBER;

static BOOL m_IrKeyFound = FALSE;
static BOOL m_IrKeyHold = FALSE;
static BYTE m_LastIrKeyCode = 0;
static eIR_STATE m_eIRState = eIR_STATE_HEAD;
static BYTE m_cIRDataCount = 0;
static BYTE m_acIRdata[4] = {0};
static WORD m_wCustomIdIndex = 0;
static WORD m_wIrRepeatCount = 0;
static BYTE m_acHoldCnt[eKEYINPUT_NUMBER] = {0};

volatile static DWORD m_dwLastwCaptureData = 0;

#define IR_DATA_QUEUE_LENGTH    (IR_BUFFER_SIZE)
#define IR_DATA_ITEM_SIZE       (sizeof(DWORD) / sizeof(BYTE))

static BYTE m_acIR_DATA_Buffer[IR_DATA_QUEUE_LENGTH * IR_DATA_ITEM_SIZE];
static QueueHandle_t m_xIR_DATA_QueueHDL;
static StaticQueue_t m_xIR_DATA_Queue;

typedef eRESULT(*fpINPUT_CALLBACK)(void *);

fpINPUT_CALLBACK m_fpInputCallback = NULL;

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_KEYPADGET
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
// 19/12/2013, Leo Create
// --------------------
// ==============================================================================
WORD halInputCtrl_KeyPadGet(void)
{
    static WORD_VAL uKeyPad = {0};
    uKeyPad.Val = 0xFFFF;

    uKeyPad.bits.b0 = !!(Board_Gpio_Get(pKEY_PWR));   // Power
    return uKeyPad.Val;
}

// ==============================================================================
// FUNCTION NAME: halInputCtrl_Detect_ISP_Key
// DESCRIPTION:
//
//
// Params:
// BYTE cEnable:
//
// Returns:
//
//
// Modification History
// --------------------
// 2015/03/05, Leo Create
// --------------------
// ==============================================================================
void halInputCtrl_Detect_ISP_Key(BYTE cEnable)
{

}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_KEYPAD_DECODE
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
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_KeyPad_Decode(void)
{
    sKEY_DATA sKeyStatus;

    sKeyStatus.eKeyType = eKEYINPUT_KEYPAD;
    sKeyStatus.eKeyEvent = eKEY_EVENT_NUMBER;
    sKeyStatus.wKeyCode = halInputCtrl_KeyPadGet();

    //If new key pressed Reset the deboundce Timer
    if(sKeyStatus.wKeyCode != m_wLastKey)
    {
        utilCounterSet(eCOUNTER_TYPE_KEY, DEBOUNDCETIMER);

        //check key release
        if(m_wKeyCheckCount >= HOLDTHRESHOLD)
        {
            m_wKeyCheckCount = 0;
            sKeyStatus.wKeyCode = m_wLastKey;
            sKeyStatus.eKeyEvent = eKEY_EVENT_RELEASE;

            //Process Key
            halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_KEYPAD]);
            return;
        }
    }

    //check key press with 100ms debounce
    if((sKeyStatus.wKeyCode == m_wLastKey) && (NO_KEY != sKeyStatus.wKeyCode))
    {
        if(0 == utilCounterGet(eCOUNTER_TYPE_KEY))
        {
            utilCounterSet(eCOUNTER_TYPE_KEY, DEBOUNDCETIMER);

            m_wKeyCheckCount++;

            if(m_wKeyCheckCount >= HOLDTHRESHOLD)
            {
                m_wKeyCheckCount = HOLDTHRESHOLD;
                sKeyStatus.wKeyCode = m_wLastKey;
                sKeyStatus.eKeyEvent = eKEY_EVENT_HOLD;

                //Hold Key
                halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_KEYPAD]);
            }
            else if(PRESSTHRESHOLD == m_wKeyCheckCount)
            {
                sKeyStatus.eKeyEvent = eKEY_EVENT_PRESSED;

                //Process Key
                halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_KEYPAD]);
            }
        }

        m_wLastKey = sKeyStatus.wKeyCode;
    }
    else
    {
        m_wKeyCheckCount = 0;
        m_wLastKey = sKeyStatus.wKeyCode;
        utilCounterSet(eCOUNTER_TYPE_KEY, 0);
    }
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_PROC
// DESCRIPTION:
//
//
// Params:
// sKEY_DATA *psKeyData:
// BYTE *pcHoldCnt:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_Proc(sKEY_DATA *psKeyData, BYTE *pcHoldCnt)
{
    sKEYLUT *psKeyLut = NULL;
    sKEY_DATA sOutPutKey = {0};
    BYTE cKeyIndex = 0;
    BYTE cKeyCode = eKEY_LIST_NUMBER;
    BYTE cKeyCondition = 0;
    WORD wKeyNumber = 0;

    switch(psKeyData->eKeyType)
    {
        case eKEYINPUT_KEYPAD:
            psKeyLut = (sKEYLUT*)m_acKeyPadLUT;
            wKeyNumber = KEYPADNUMBER;
            sOutPutKey.eKeyType = eKEYINPUT_KEYPAD;
            break;

        case eKEYINPUT_IR:
            psKeyLut = (sKEYLUT*)m_acIRKeyLUT;
            wKeyNumber = IRKEYPADNUMBER;
            sOutPutKey.eKeyType = eKEYINPUT_IR;
            break;

        default:
            ASSERT_ALWAYS();
            break;
    }

    //Check Event Founded
    for(cKeyIndex = 0; cKeyIndex < wKeyNumber; cKeyIndex++)
    {
        if(psKeyLut[cKeyIndex].wKeyCode == psKeyData->wKeyCode)
        {
            cKeyCondition = psKeyData->eKeyEvent & psKeyLut[cKeyIndex].cConditions;

            switch(cKeyCondition)
            {
                case eKEY_EVENT_PRESSED:
                    *pcHoldCnt = 0;
                    cKeyCode = psKeyLut[cKeyIndex].cPressKey;
                    sOutPutKey.eKeyEvent = eKEY_EVENT_PRESSED;
                    break;

                case eKEY_EVENT_HOLD:
                    if((0 == psKeyLut[cKeyIndex].cHoldCount)||(0 == (*pcHoldCnt % psKeyLut[cKeyIndex].cHoldCount)))
                    {
                        cKeyCode = psKeyLut[cKeyIndex].cHoldKey;
                        sOutPutKey.eKeyEvent = eKEY_EVENT_HOLD;
                    }

                    (*pcHoldCnt)++;
                    break;

                case eKEY_EVENT_RELEASE:
                    *pcHoldCnt = 0;
                    cKeyCode = psKeyLut[cKeyIndex].cReleaseKey;
                    sOutPutKey.eKeyEvent = eKEY_EVENT_RELEASE;
                    break;

                default:
                    //Wrong condition
                    //ASSERT_ALWAYS();
                    return;
            }

            //Found Key
            if(eKEY_LIST_NUMBER > cKeyCode)
            {
                if(NULL != m_fpInputCallback)
                {
                    sOutPutKey.wKeyCode = cKeyCode;
                    m_fpInputCallback(&sOutPutKey);
                    // LOG_MSG(eDB_MSK_LIST_INPUT, ".");
                }
            }

            return;
        }
    }
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_INIT
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
// 29/10/2013, Leo Create
// --------------------
// ==============================================================================
eRESULT halInputCtrl_Init(void)
{
    halInputCtrl_IR_Restart();

    // Global Input Multiplexer Array (GIMA)
    Board_Gpio_Gima_PinMux((sGIMA_PIN_CFG *)m_asGimaMuxCfgTable[0].psGimaCgf, m_asGimaMuxCfgTable[0].wPinNum);


    m_xIR_DATA_QueueHDL = xQueueCreateStatic(IR_DATA_QUEUE_LENGTH,
                                             IR_DATA_ITEM_SIZE,
                                             m_acIR_DATA_Buffer,
                                             &m_xIR_DATA_Queue);

    if(NULL == m_xIR_DATA_QueueHDL)
    {
        LOG_MSG(eDB_MSK_LIST_ASSERT, "m_xIR_DATA_QueueHDL Create Fail\r\n");
    }

    //Board_IR_Register_Callback(halInputCtrl_IR_PulseInsert);
    Board_Timer_Register_Callback(eBOARD_TIMER_TIMER1, halInputCtrl_IR_PulseInsert);

    return rcSUCCESS;
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_HEADGET
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
eRESULT halInputCtrl_IR_HeadGet(void)
{
    eRESULT eResult = rcERROR;
    sKEY_DATA sKeyStatus;
    DWORD dwPulseWidth = 0;
    static DWORD dwLastPulseWidth = 0;
    DWORD dwReadPulseWidth = 0;

    while((NULL != m_xIR_DATA_QueueHDL) && (pdTRUE == xQueueReceive(m_xIR_DATA_QueueHDL, (void *)&dwReadPulseWidth, 0)))
    {
        if(LEADER_MAX < dwReadPulseWidth)
            continue;

        dwPulseWidth = dwReadPulseWidth + dwLastPulseWidth;
        dwLastPulseWidth = dwReadPulseWidth;

        if((dwPulseWidth >= LEADER_MIN) && (dwPulseWidth <= LEADER_MAX))
        {
            //reset repeat flag
            m_wIrRepeatCount = 0;
            m_IrKeyFound = FALSE;
            return rcSUCCESS;
        }

        //look for repeat
        //look for leader
        if(m_IrKeyFound)
        {
            if((dwPulseWidth >= REPEAT_MIN) && (dwPulseWidth <= REPEAT_MAX))
            {
                utilCounterSet(eCOUNTER_TYPE_IRDECODE, 200);

                if(m_wIrRepeatCount >= IRREPEATCOUNT)
                {
                    m_wIrRepeatCount = IRREPEATCOUNT;
                    sKeyStatus.wKeyCode = m_LastIrKeyCode;
                    sKeyStatus.eKeyEvent = eKEY_EVENT_HOLD;
                    sKeyStatus.eKeyType = eKEYINPUT_IR;
                    m_IrKeyHold = TRUE;

                    //Hold Key
                    halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_IR]);
                    xQueueReset(m_xIR_DATA_QueueHDL);
                    return rcERROR;
                }
                else
                {
                    m_wIrRepeatCount++;
                }
            }
        }
    }

    return eResult;
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_DATAGET
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
eRESULT halInputCtrl_IR_DataGet(void)
{
    sKEY_DATA sKeyStatus;
    BYTE cBit = 0;
    WORD wCustomId = 0;
    DWORD dwPulseWidth = 0;
    DWORD dwReadPulseWidth = 0;

    while(NULL != m_xIR_DATA_QueueHDL)
    {
        xQueueReceive(m_xIR_DATA_QueueHDL, (void *)&dwReadPulseWidth, 10);
        dwPulseWidth = dwReadPulseWidth;

        xQueueReceive(m_xIR_DATA_QueueHDL, (void *)&dwReadPulseWidth, 10);
        dwPulseWidth += dwReadPulseWidth;

        if((dwPulseWidth >= BIT_0_MIN) && (dwPulseWidth <= BIT_0_MAX))
        {
            cBit = 0;
        }
        else if((dwPulseWidth >= BIT_1_MIN) && (dwPulseWidth <= BIT_1_MAX))
        {
            cBit = 1;
        }
        else
        {
            m_cIRDataCount = 0;
            return rcERROR;
        }

        m_acIRdata[(m_cIRDataCount / IR_BYTE_SIZE)] |= (cBit << (m_cIRDataCount % IR_BYTE_SIZE));
        m_cIRDataCount++;

        if(m_cIRDataCount >= IR_DATA_SIZE)
        {
            m_cIRDataCount = 0;
            wCustomId = (m_acIRdata[0] << 8) | m_acIRdata[1];

#ifdef PRINT_OUT_IR_DATA
            LOG_MSG(eDB_MSK_LIST_INPUT,
                    "0x%02X,0x%02X,0x%02X,0x%02X\r\n",
                    m_acIRdata[0], m_acIRdata[1],
                    m_acIRdata[2], m_acIRdata[3]);
#endif // 0

            if((m_awCustomCode[m_wCustomIdIndex] == wCustomId) || (m_awCustomCode[DEFAULT_ID_NUMBER] == wCustomId))
            {
                m_IrKeyFound = TRUE;
                sKeyStatus.wKeyCode = m_acIRdata[2];
                sKeyStatus.eKeyEvent = eKEY_EVENT_PRESSED;
                sKeyStatus.eKeyType = eKEYINPUT_IR;

                //Press Key
                halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_IR]);
                xQueueReset(m_xIR_DATA_QueueHDL);

                //Set data for repeat condition
                if(m_LastIrKeyCode != sKeyStatus.wKeyCode)
                {
                    m_LastIrKeyCode = sKeyStatus.wKeyCode;
                }
            }

            return rcSUCCESS;
        }
    }

    return rcBUSY;
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_DECODE
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_IR_Decode(void)
{
    sKEY_DATA sKeyStatus;

    if(utilCounterGet(eCOUNTER_TYPE_IRDECODE) == 0)
    {
        if(m_IrKeyHold)
        {
            sKeyStatus.wKeyCode = m_LastIrKeyCode;
            sKeyStatus.eKeyEvent = eKEY_EVENT_RELEASE;
            sKeyStatus.eKeyType = eKEYINPUT_IR;

            //Release Key
            halInputCtrl_Proc(&sKeyStatus, &m_acHoldCnt[eKEYINPUT_IR]);
            xQueueReset(m_xIR_DATA_QueueHDL);
        }

        halInputCtrl_IR_Restart();
    }

    switch(m_eIRState)
    {
        case eIR_STATE_HEAD:
            if(rcSUCCESS == halInputCtrl_IR_HeadGet())
            {
                m_eIRState = eIR_STATE_DATA;
                m_cIRDataCount = 0;
                utilCounterSet(eCOUNTER_TYPE_IRDECODE, 200);
                halInputCtrl_IR_Flush();
            }

            break;

        case eIR_STATE_DATA:
            if(rcBUSY != halInputCtrl_IR_DataGet())
            {
                m_eIRState = eIR_STATE_HEAD;
            }

            break;

        default:
            ASSERT_ALWAYS();
            m_eIRState = eIR_STATE_HEAD;
            break;
    }
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_FLUSH
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_IR_Flush(void)
{
    m_acIRdata[0] = m_acIRdata[1] = m_acIRdata[2] = m_acIRdata[3] = 0;
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_RESTART
// DESCRIPTION:
//
//
// Params:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_IR_Restart(void)
{
    halInputCtrl_IR_Flush();
    m_cIRDataCount = 0;
    m_wIrRepeatCount = 0;
    m_eIRState = eIR_STATE_HEAD;
    m_IrKeyFound = FALSE;
    m_IrKeyHold = FALSE;
}

// ==============================================================================
// FUNCTION NAME: halInputCtrl_IR_CustomId_Set
// DESCRIPTION:
//
//
// Params:
// BYTE cCustomIdIndex:
//
// Returns:
//
//
// modification history
// --------------------
// 23/04/2013, Leohong Create
// --------------------
// ==============================================================================
void halInputCtrl_IR_CustomId_Set(BYTE cCustomIdIndex)
{
    m_wCustomIdIndex = cCustomIdIndex;
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_IR_PULSEINSERT
// DESCRIPTION:
//
//
// Params:
// void *pvBuffer:
//
// Returns:
//
//
// modification history
// --------------------
// 24/12/2013, Leo Create
// --------------------
// ==============================================================================
void halInputCtrl_IR_PulseInsert(void *pvBuffer)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    DWORD dwPulseWidth = 0;
    DWORD *pdwPulseCount = (DWORD *)pvBuffer;

    dwPulseWidth = *pdwPulseCount - m_dwLastwCaptureData;

    if(NULL != m_xIR_DATA_QueueHDL)
    {
        xQueueSendFromISR(m_xIR_DATA_QueueHDL, (void *)&dwPulseWidth, &xHigherPriorityTaskWoken);
    }

    m_dwLastwCaptureData = *pdwPulseCount;

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// ==============================================================================
// FUNCTION NAME: HALINPUTCTRL_REGISTER_CALLBACK
// DESCRIPTION:
//
//
// Params:
// void (*fpCallback)(BYTE*):
//
// Returns:
//
//
// modification history
// --------------------
// 24/12/2013, Leo Create
// --------------------
// ==============================================================================
void halInputCtrl_Register_Callback(eRESULT(*fpCallback)(void *))
{
    m_fpInputCallback = fpCallback;
}

