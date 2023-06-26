/*
 * $Header: dcitypes.h $
 */

/*
  Copyright (c) 2010, BaseSoft Inc.  All rights reserved.
*/

#ifndef DCITYPES_H
#define DCITYPES_H


#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

typedef unsigned char	DciText;

#ifndef OCI_ORACLE
typedef unsigned char	ub1;                   
typedef   signed char	sb1;                  
 
typedef unsigned short	ub2;                
typedef   signed short	sb2;               

typedef unsigned int	ub4;                   
typedef   signed int	sb4;                   

#ifdef WIN32
typedef unsigned __int64 ub8;
typedef   signed __int64 sb8;
#else

#ifdef __LP64__
typedef unsigned long ub8;
typedef   signed long sb8;
#else
typedef unsigned long long ub8;
typedef   signed long long sb8;
#endif /* __LP64__ */

#endif /* WIN32 */

typedef char	eb1;
typedef short	eb2;               
typedef int		eb4;               

typedef sb1	b1;                   
typedef sb2	b2;
typedef sb4	b4;

typedef          int	eword;
typedef unsigned int	uword;
typedef   signed int	sword;

typedef unsigned long	ubig;             
typedef   signed long	sbig;             

typedef DciText         text;
typedef unsigned short  utext;
#endif /* OCI_ORACLE */

#ifndef CONST
#define CONST const
#endif

#define dvoid void

#ifndef boolean
#define boolean int
#endif

struct DCITime
{
	ub1 DCITimeHH;                          /* hours; range is 0 <= hours <=23 */
	ub1 DCITimeMI;                     /* minutes; range is 0 <= minutes <= 59 */
	ub1 DCITimeSS;                     /* seconds; range is 0 <= seconds <= 59 */
};
typedef struct DCITime DCITime;

struct DCIDate
{
	sb2 DCIDateYYYY;         /* gregorian year; range is -4712 <= year <= 9999 */
	ub1 DCIDateMM;                          /* month; range is 1 <= month < 12 */
	ub1 DCIDateDD;                             /* day; range is 1 <= day <= 31 */
	DCITime DCIDateTime;                                               /* time */
};
typedef struct DCIDate DCIDate;

#define DCI_NUMBER_UNSIGNED 0                        /* Unsigned type -- ubX */
#define DCI_NUMBER_SIGNED   2                          /* Signed type -- sbX */

#define DCI_NUMBER_SIZE 22
struct DCINumber
{
	ub1 DCINumberPart[DCI_NUMBER_SIZE];
};
typedef struct DCINumber DCINumber;
/*
#define DCIDateGetTime(date, hour, min, sec) \
{ \
	*hour = (date)->DCIDateTime.DCITimeHH; \
	*min = (date)->DCIDateTime.DCITimeMI; \
	*sec = (date)->DCIDateTime.DCITimeSS; \
}

#define DCIDateGetDate(date, year, month, day) \
{ \
	*year = (date)->DCIDateYYYY; \
	*month = (date)->DCIDateMM; \
	*day = (date)->DCIDateDD; \
}

#define DCIDateSetTime(date, hour, min, sec) \
{ \
	(date)->DCIDateTime.DCITimeHH = hour; \
	(date)->DCIDateTime.DCITimeMI = min; \
	(date)->DCIDateTime.DCITimeSS = sec; \
}

#define DCIDateSetDate(date, year, month, day) \
{ \
	(date)->DCIDateYYYY = year; \
	(date)->DCIDateMM = month; \
	(date)->DCIDateDD = day; \
}
*/
#endif /* DCITYPES_H */
