/*$Source$*/
/*****************************�ļ�ͷ��ע��*************************************/
//
//			Copyright (C), 2011-2016, AV Frontier Tech. Co., Ltd.
//
//
// �� �� ����	$RCSfile$
//
// �� �� �ߣ�	Administrator
//
// ����ʱ�䣺	2011.05.09
//
// �����£�	$Date$
//
//				$Author$
//
//				$Revision$
//
//				$State$
//
// �ļ�������	d0367�ڲ�ͷ�ļ�
//
/******************************************************************************/

#ifndef __D0367_INNER_H
#define __D0367_INNER_H

/********************************  �ļ�����************************************/


#ifdef __cplusplus
extern "C" {
#endif


/********************************  ��������************************************/

/********************************  ���ݽṹ************************************/

/********************************  �� �� ��************************************/

/********************************  ��������************************************/

/********************************  ��������************************************/

/********************************  ��������************************************/

U16 ChipGetRegAddress(U32 FieldId);
int ChipGetFieldMask(U32 FieldId);
int ChipGetFieldSign(U32 FieldId);
int ChipGetFieldPosition(U8 Mask);
int ChipGetFieldBits(int mask, int Position);
S32 ChipGetRegisterIndex(TUNER_IOREG_DeviceMap_t *DeviceMap,
							IOARCH_Handle_t IOHandle, U16 RegId);

void D0367_write(TUNER_IOREG_DeviceMap_t *DeviceMap,
									IOARCH_Handle_t IOHandle,
									unsigned char *pcData, int nbdata);
void D0367_read(TUNER_IOREG_DeviceMap_t *DeviceMap,
									IOARCH_Handle_t IOHandle,
									unsigned char *pcData, int NbRegs);

/********************************  ��������************************************/




#ifdef __cplusplus
}
#endif


#endif  /* __D0367_INNER_H */
/* EOF------------------------------------------------------------------------*/

/* BOL-------------------------------------------------------------------*/
//$Log$
/* EOL-------------------------------------------------------------------*/

