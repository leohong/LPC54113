#ifndef UTILLOGAPI_H
#define UTILLOGAPI_H
// ==============================================================================
// FILE NAME: UTILLOGAPI.H
// DESCRIPTION:
//
//
// modification history
// --------------------
// 02/07/2013, Leo written
// --------------------
// ==============================================================================


#include "Commondef.h"

typedef enum
{
    /* 00 */ eDB_MSK_LIST_ASSERT = 0,
    /* 01 */ eDB_MSK_LIST_IAP,
    /* 02 */ eDB_MSK_LIST_VIDEO_SWITCHER,
    /* 03 */ eDB_MSK_LIST_INPUT,
    /* 04 */ eDB_MSK_LIST_CLI,
    /* 05 */ eDB_MSK_LIST_HOST,
    /* 06 */ eDB_MSK_LIST_EVENT_QUEUE,
    /* 07 */ eDB_MSK_LIST_SYSCTRL,
    /* 08 */ eDB_MSK_LIST_USB,

    eDB_MSK_LIST_NUMBERS,
} eDB_MSK_LIST;

#define LOG_ALL         (eDB_MSK_LIST_NUMBERS)
#define ASSERT_MASK     (0x0001 << eDB_MSK_LIST_ASSERT)

#ifdef __DEBUG__
#define LOG_MSG(mask, fmt, arg...) utilLog_Printf(mask, fmt, ##arg)

/* __ASM("BKPT 0");\ */
#define ASSERT(X) \
    do\
    {\
        if(!(X))\
        {\
            utilLog_Printf(eDB_MSK_LIST_ASSERT, "\r\nASSERT: %s: %d\r\n", __FILE__, __LINE__);\
        }\
    }\
    while(0)

#define ASSERT_ALWAYS() \
    do\
    {\
        {\
            utilLog_Printf(eDB_MSK_LIST_ASSERT, "\r\nASSERT_ALWAYS: %s: %d\r\n", __FILE__, __LINE__);\
        }\
    }\
    while(0)

#else

#define LOG_MSG(mask, fmt, arg...)

#ifdef ENABLE_ASSERT
#define ASSERT(X) \
    do\
    {\
        if(!(X))\
        {\
            utilLog_Printf(eDB_MSK_LIST_ASSERT, "\r\nASSERT: %s: %d\r\n", __FILE__, __LINE__);\
        }\
    }\
    while(0)

#define ASSERT_ALWAYS() \
    do\
    {\
        {\
            utilLog_Printf(eDB_MSK_LIST_ASSERT, "\r\nASSERT_ALWAYS: %s: %d\r\n", __FILE__, __LINE__);\
        }\
    }\
    while(0)

#else
#define ASSERT(X)
#define ASSERT_ALWAYS()
#endif /* ENABLE_ASSERT */

#endif // __DEBUG__

typedef enum
{
    eLOG_PARAM_FLAG,

    eLOG_PARAM_NUMBERS,
} eLOG_PARAM;

void utilLog_Init(void);
void utilLog_Mask_Set(eDB_MSK_LIST eMask, BYTE cEnable);
BYTE utilLog_Mask_Get(eDB_MSK_LIST eMask);
void utilLog_Printf(eDB_MSK_LIST eMask, char *pcfmtstring, ...);
void utilLog_Mask_Print_Out(void);

#endif // UTILLOGAPI_H

