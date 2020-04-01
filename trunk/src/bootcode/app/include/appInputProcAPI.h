#ifndef APPINPUTCTRLAPI_H
#define APPINPUTCTRLAPI_H
// ==============================================================================
// FILE NAME: APPINPUTCTRLAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 22/04/2013, Leohong written
// --------------------
// ==============================================================================


#include "CommonDef.h"
#include "halBoardCtrlAPI.h" //A70_Larry_0043

typedef enum
{
    eINPUT_EVENT_KEY,
    eINPUT_EVENT_POWER_ON,
    eINPUT_EVENT_POWER_OFF,

    eINPUT_EVENT_NUMBERS,
} eINPUT_EVENT;

typedef enum
{
    eINPUT_PARAM_REMOTE_ID,
    eINPUT_PARAM_UARTSET,
    eINPUT_PARAM_BAUDRATE,

    eINPUT_PARAM_NUMBERS,
} eINPUT_PARAM; //A70_Larry_0043

typedef enum
{
    eINPUT_BAUDRATE_2400,
    eINPUT_BAUDRATE_4800,
    eINPUT_BAUDRATE_9600,
    eINPUT_BAUDRATE_14400,
    eINPUT_BAUDRATE_19200,
    eINPUT_BAUDRATE_38400,
    eINPUT_BAUDRATE_57600,
    eINPUT_BAUDRATE_115200,
    eINPUT_BAUDRATE_1200,

    eINPUT_BAUDRATE_NUMBERS,
} eINPUT_BAUDRATE;

eRESULT appInputProc_Init(void);
void appInputProc_Process(void);
eRESULT appInputProc_BufferInsert(void *pvKey);
eRESULT appInputProc_CLIHandler(BYTE cKey);
void appInputProc_Param_Get(eINPUT_PARAM eParam, void *pvData);
void appInputProc_Param_Set(eINPUT_PARAM eParam, void *pvData);
void appInputProc_Remote_CustomId_Set(BYTE cRemoteId);

#endif // APPINPUTCTRLAPI_H

