#ifndef GENERICTYPEDEFS_H
#define GENERICTYPEDEFS_H
// ==============================================================================
// FILE NAME: GENERICTYPEDEFS.H
// DESCRIPTION: 
// 
// 
// modification history
// --------------------
// 03/01/2013, Leohong Create
// --------------------
// ==============================================================================

#include "LPC_types.h"

typedef uint8_t     BYTE;                           /* 8-bit unsigned  */
typedef uint16_t    WORD;                           /* 16-bit unsigned */
typedef uint32_t    DWORD;                          /* 32-bit unsigned */
typedef uint64_t    QWORD;                          /* 64-bit unsigned */
typedef Bool        BOOL;
typedef int8_t      INT8;
typedef int16_t     INT16, *PINT16;
typedef int32_t     INT32;
typedef int64_t     INT64;
typedef int32_t     LONG, *PLONG;      

#define HIGH    1
#define LOW     0

#if 1  //A70_Eva_0001
typedef BYTE u8_t;
typedef WORD u16_t;
typedef DWORD u32_t;
typedef BOOL tBoolean;
typedef int16_t s16_t;

#define BIT0			0x01
#define BIT1			0x02
#define BIT2			0x04
#define BIT3			0x08
#define BIT4			0x10
#define BIT5			0x20
#define BIT6			0x40
#define BIT7			0x80
#endif

typedef union
{
    WORD Val;
    BYTE v[2];
    struct
    {
        BYTE LB;
        BYTE HB;
    } byte;
    
    struct
    {
        unsigned int b0:1;
        unsigned int b1:1;
        unsigned int b2:1;
        unsigned int b3:1;
        unsigned int b4:1;
        unsigned int b5:1;
        unsigned int b6:1;
        unsigned int b7:1;
        unsigned int b8:1;
        unsigned int b9:1;
        unsigned int b10:1;
        unsigned int b11:1;
        unsigned int b12:1;
        unsigned int b13:1;
        unsigned int b14:1;
        unsigned int b15:1;
    } bits;
} WORD_VAL, WORD_BITS;



#endif // GENERICTYPEDEFS_H
