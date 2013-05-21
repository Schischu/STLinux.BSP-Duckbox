/*$Source$*/
/*****************************�ļ�ͷ��ע��*************************************/
//
//			Copyright (C), 2012-2017, AV Frontier Tech. Co., Ltd.
//
//
// �� �� ����	$RCSfile$
//
// �� �� �ߣ�	D26LF
//
// ����ʱ�䣺	2012.09.07
//
// �����£�	$Date$
//
//				$Author$
//
//				$Revision$
//
//				$State$
//
// �ļ�������	define
//
/******************************************************************************/

#ifndef __YWDEFS_H
#define __YWDEFS_H

/********************************  �ļ�����************************************/

#include <linux/delay.h>
#include <linux/slab.h>

#ifdef __cplusplus
extern "C" {
#endif


/********************************  ��������************************************/

typedef char			S8;
typedef unsigned char	U8;

typedef short			S16;
typedef unsigned short	U16;

typedef int			    S32;
typedef unsigned int	U32;

typedef char			INT8;
typedef unsigned char	UINT8;

typedef short			INT16;
typedef unsigned short	UINT16;

typedef int			    INT32;
typedef unsigned int	UINT32;

//typedef unsigned int	IOARCH_Handle_t;
typedef unsigned int	YW_ErrorType_T;



typedef unsigned long long UINT64;
typedef unsigned long long U64;

typedef long long INT64;
typedef long long S64;

#ifndef DEFINED_BOOL
#define DEFINED_BOOL
typedef int BOOL;
/* BOOL type constant values */
#ifndef TRUE
    #define TRUE (1 == 1)
#endif
#ifndef FALSE
    #define FALSE (!TRUE)
#endif
#endif

#ifndef NULL
	#define NULL 0
#endif

typedef const char * ST_Revision_t;
#define YWTUNERi_MAX_TUNER_NUM 		4

#define YW_NO_ERROR 				0
#define YWHAL_ERROR_NO_MEMORY 				-1
#define YWHAL_ERROR_INVALID_HANDLE			-2
#define YWHAL_ERROR_BAD_PARAMETER 			-3
#define YWHAL_ERROR_FEATURE_NOT_SUPPORTED 	-4
#define YWHAL_ERROR_UNKNOWN_DEVICE			-5

#define SUCCESS         0       /* Success return */
#define RET_SUCCESS		((INT32)0)
#define ERR_FAILUE		-9

/********************************  ���ݽṹ************************************/

/********************************  �� �� ��************************************/

/********************************  ��������************************************/

/********************************  ��������************************************/

/********************************  ��������************************************/

#define YWOS_TaskSleep(ms)		msleep(ms)
#define usleep(us)				udelay(us)

#define YWOS_Free(p)			kfree(p)
#define YWOS_Malloc(n)			kmalloc(n, GFP_KERNEL)

#define YWLIB_Memset(a, b, c)	memset(a, b, c)
#define YWLIB_Memcpy(a, b, c)	memcpy(a, b, c)
#define YWLIB_Strcpy(a, b)		strcpy(a, b)

#define YWOSTRACE(x)

/********************************  ��������************************************/



#ifdef __cplusplus
}
#endif


#endif  /* __YWDEFS_H */
/* EOF------------------------------------------------------------------------*/

/* BOL-------------------------------------------------------------------*/
//$Log$
/* EOL-------------------------------------------------------------------*/

