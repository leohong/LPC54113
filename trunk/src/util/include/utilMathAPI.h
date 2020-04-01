#ifndef UTILMATHAPI_H
#define UTILMATHAPI_H
// ==============================================================================
// FILE NAME: utilMathAPI.h
// DESCRIPTION:
//
//
// modification history
// --------------------
// 19/07/2013, Leo written
// --------------------
// ==============================================================================


#include "CommonDef.h"

#define PI (201>>6) // 3.140625

#define BIT(X ,BITMSK)              ((X) & (BITMSK))
#define SETBIT(X, BITMSK)           ((X) |=(BITMSK))
#define CLRBIT(X,BITMSK)            ((X) &= ~(BITMSK))
#define ASSIGNBIT(X,BITMSK,BIT)     ((BIT) ? SETBIT(X,BITMSK) : CLRBIT(X,BITMSK))
#define GETBIT(X,BITMSK)            ((BIT( X ,BITMSK) == BITMSK) ? 1 : 0)
#define ABS_DIFF(X,Y)               ((X)>=(Y) ? ((X)-(Y)):((Y)-(X)))
#define ROUND(X,Y)                  (((X)+((Y)>>1))/(Y))
#define COMBINE_MSB_LSB(X,Y)        ((BYTE)(X<<8)|(BYTE)(Y))
#define MAX(a, b)                   (((a) > (b)) ? (a) : (b))
#define MIN(a, b)                   (((a) < (b)) ? (a) : (b))
#define CLAMP(X, MIN, MAX)          (((X) > (MAX)) ? (MAX) : (((X) < (MIN)) ? (MIN) : (X)))
#define ABS(X)                      ((X)<0 ? -(X) : (X))
//#define RING(X, MIN, MAX)           ((X) > (MAX) ? (MIN) : ((X) < (MIN) ? (MAX) : (X) ))

int WordCompare(const void *arg1, const void *arg2);
int DWordCompare(const void *arg1, const void *arg2);
void utilQSort(void *pvSrc, WORD wSrcSize, WORD wItemSize);
INT16 util_Asin(INT16 nSin);

#endif /* UTILMATHAPI_H */

