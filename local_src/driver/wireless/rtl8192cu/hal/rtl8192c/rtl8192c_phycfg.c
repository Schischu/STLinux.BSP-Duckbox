/******************************************************************************

     (c) Copyright 2008, RealTEK Technologies Inc. All Rights Reserved.

 Module:	hal8192sphy.c	

 Note:		Merge 92SE/SU PHY config as below
			1. BB register R/W API
 			2. RF register R/W API
 			3. Initial BB/RF/MAC config by reading BB/MAC/RF txt.
 			3. Power setting API
 			4. Channel switch API
 			5. Initial gain switch API.
 			6. Other BB/MAC/RF API.
 			
 Function:	PHY: Extern function, phy: local function
 		 
 Export:	PHY_FunctionName

 Abbrev:	NONE

 History:
	Data		Who		Remark	
	08/08/2008  MHC    	1. Port from 9x series phycfg.c
						2. Reorganize code arch and ad description.
						3. Collect similar function.
						4. Seperate extern/local API.
	08/12/2008	MHC		We must merge or move USB PHY relative function later.
	10/07/2008	MHC		Add IQ calibration for PHY.(Only 1T2R mode now!!!)
	11/06/2008	MHC		Add TX Power index PG file to config in 0xExx register
						area to map with EEPROM/EFUSE tx pwr index.
	
******************************************************************************/
#define _HAL_8192C_PHYCFG_C_

#include <drv_conf.h>
#include <osdep_service.h>
#include <drv_types.h>
#include <rtw_byteorder.h>

#include <hal_init.h>

#include "Hal8192CPhyReg.h"
#include "Hal8192CPhyCfg.h"
#include "HalRf.h"


/*---------------------------Define Local Constant---------------------------*/
/* Channel switch:The size of command tables for switch channel*/
#define MAX_PRECMD_CNT 16
#define MAX_RFDEPENDCMD_CNT 16
#define MAX_POSTCMD_CNT 16

#define MAX_DOZE_WAITING_TIMES_9x 64

/*---------------------------Define Local Constant---------------------------*/


/*------------------------Define global variable-----------------------------*/

/*------------------------Define local variable------------------------------*/


/*--------------------Define export function prototype-----------------------*/
// Please refer to header file
/*--------------------Define export function prototype-----------------------*/


/*---------------------Define local function prototype-----------------------*/
/* RF serial read/write by firmware 3wire. */
static	u32	phy_FwRFSerialRead(	IN	PADAPTER			Adapter,
									IN	RF90_RADIO_PATH_E	eRFPath,
									IN	u32				Offset	);
static	void	phy_FwRFSerialWrite(	IN	PADAPTER			Adapter,
										IN	RF90_RADIO_PATH_E	eRFPath,
										IN	u32				Offset,
										IN	u32				Data	);

/* RF serial read/write */
static	u32	phy_RFSerialRead(	IN	PADAPTER			Adapter,
									IN	RF90_RADIO_PATH_E	eRFPath,
									IN	u32				Offset	);
static	void	phy_RFSerialWrite(	IN	PADAPTER			Adapter,
									IN	RF90_RADIO_PATH_E	eRFPath,
									IN	u32				Offset,
									IN	u32				Data	);

static	u32	phy_CalculateBitShift(u32 BitMask	);

// Initialize relative setting
static	int	phy_BB8190_Config_HardCode(IN	PADAPTER	Adapter	);
static	int	phy_BB8192C_Config_ParaFile(IN	PADAPTER	Adapter	);

/* MAC config */
static	int	phy_ConfigMACWithParaFile(	IN	PADAPTER	Adapter,
												IN	u8* 	pFileName);
static	int	phy_ConfigMACWithHeaderFile(IN	PADAPTER		Adapter);
/* BB config */
static	int	phy_ConfigBBWithParaFile(	IN	PADAPTER	Adapter,
												IN	u8* 	pFileName);
static	int	phy_ConfigBBWithHeaderFile(	IN	PADAPTER	Adapter,
												IN	u8 		ConfigType);
static	int	phy_ConfigBBWithPgParaFile(	IN	PADAPTER	Adapter,
												IN	u8* 	pFileName);
static	int	phy_ConfigBBWithPgHeaderFile(	
												IN	PADAPTER	Adapter,
												IN	u8 		ConfigType);
static	int	phy_SetBBtoDiffRFWithParaFile(	
												IN	PADAPTER	Adapter,
												IN	u8* 		pFileName);
static	int	phy_SetBBtoDiffRFWithHeaderFile(	
												IN	PADAPTER	Adapter,
												IN	u8 		ConfigType);
/*Initialize Register definition*/
static	void	phy_InitBBRFRegisterDefinition(	IN	PADAPTER		Adapter	);

/* Channel switch related */
#if 0
static	BOOLEAN	phy_SetSwChnlCmdArray(	SwChnlCmd*		CmdTable,
										u4Byte			CmdTableIdx,
										u4Byte			CmdTableSz,
										SwChnlCmdID		CmdID,
										u4Byte			Para1,
										u4Byte			Para2,
										u4Byte			msDelay	);
static	BOOLEAN	phy_SwChnlStepByStep(	IN	PADAPTER	Adapter,
										IN	u1Byte		channel,
										IN	u1Byte		*stage,
										IN	u1Byte		*step,
										OUT u4Byte		*delay	);
#endif
// We should not call this function directly
static	void	phy_FinishSwChnlNow(	IN	PADAPTER	Adapter,
										IN	u8		channel		);


static	u8	phy_DbmToTxPwrIdx(	IN	PADAPTER		Adapter,
									IN	WIRELESS_MODE	WirelessMode,
									IN	int			PowerInDbm	);

						
#if 0
BOOLEAN
phy_SetRFPowerState8192SE(
	IN	PADAPTER			Adapter,
	IN	RT_RF_POWER_STATE	eRFPowerState
	);

BOOLEAN
phy_SetRFPowerState8192CU(
	IN	PADAPTER			Adapter,
	IN	RT_RF_POWER_STATE	eRFPowerState
	);
#endif

#if DEV_BUS_TYPE==PCI_INTERFACE
static	VOID
phy_CheckEphySwitchReady(
	IN	PADAPTER			Adapter
	);
#endif
						
int
PHY_ConfigRFExternalPA(
	IN	PADAPTER			Adapter,
	RF90_RADIO_PATH_E		eRFPath
);						

VOID
phy_ConfigBBExternalPA(
	IN	PADAPTER			Adapter
);

VOID
phy_SetRTL8192CERfSleep(
	IN	PADAPTER			Adapter
);

VOID
phy_SetRTL8192CERfOn(
	IN	PADAPTER			Adapter
);

				
/*----------------------------Function Body----------------------------------*/
//
// 1. BB register R/W API
//
/**
* Function:	PHY_QueryBBReg
*
* OverView:	Read "sepcific bits" from BB register
*
* Input:
*			PADAPTER		Adapter,
*			u4Byte			RegAddr,		//The target address to be readback
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be readback	
* Output:	None
* Return:		u4Byte			Data			//The readback register value
* Note:		This function is equal to "GetRegSetting" in PHY programming guide
*/
u32
PHY_QueryBBReg(
	IN	PADAPTER	Adapter,
	IN	u32		RegAddr,
	IN	u32		BitMask
	)
{	
  	u32	ReturnValue = 0, OriginalValue, BitShift;
	u16	BBWaitCounter = 0;

#if (DISABLE_BB_RF == 1)
	return 0;
#endif

	//RT_TRACE(COMP_RF, DBG_TRACE, ("--->PHY_QueryBBReg(): RegAddr(%#lx), BitMask(%#lx)\n", RegAddr, BitMask));

	OriginalValue = read32(Adapter, RegAddr);
	BitShift = phy_CalculateBitShift(BitMask);
	ReturnValue = (OriginalValue & BitMask) >> BitShift;

	//RTPRINT(FPHY, PHY_BBR, ("BBR MASK=0x%lx Addr[0x%lx]=0x%lx\n", BitMask, RegAddr, OriginalValue));
	//RT_TRACE(COMP_RF, DBG_TRACE, ("<---PHY_QueryBBReg(): RegAddr(%#lx), BitMask(%#lx), OriginalValue(%#lx)\n", RegAddr, BitMask, OriginalValue));

	return (ReturnValue);

}


/**
* Function:	PHY_SetBBReg
*
* OverView:	Write "Specific bits" to BB register (page 8~) 
*
* Input:
*			PADAPTER		Adapter,
*			u4Byte			RegAddr,		//The target address to be modified
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be modified	
*			u4Byte			Data			//The new register value in the target bit position
*										//of the target address			
*
* Output:	None
* Return:		None
* Note:		This function is equal to "PutRegSetting" in PHY programming guide
*/

VOID
PHY_SetBBReg(
	IN	PADAPTER	Adapter,
	IN	u32		RegAddr,
	IN	u32		BitMask,
	IN	u32		Data
	)
{
	HAL_DATA_TYPE	*pHalData		= GET_HAL_DATA(Adapter);
	u16			BBWaitCounter	= 0;
	u32			OriginalValue, BitShift;

#if (DISABLE_BB_RF == 1)
	return;
#endif

	//RT_TRACE(COMP_RF, DBG_TRACE, ("--->PHY_SetBBReg(): RegAddr(%#lx), BitMask(%#lx), Data(%#lx)\n", RegAddr, BitMask, Data));

	if(BitMask!= bMaskDWord){//if not "double word" write
		OriginalValue = read32(Adapter, RegAddr);
		BitShift = phy_CalculateBitShift(BitMask);
		Data = (((OriginalValue) & (~BitMask)) | (Data << BitShift));
	}

	write32(Adapter, RegAddr, Data);

	//RTPRINT(FPHY, PHY_BBW, ("BBW MASK=0x%lx Addr[0x%lx]=0x%lx\n", BitMask, RegAddr, Data));
	//RT_TRACE(COMP_RF, DBG_TRACE, ("<---PHY_SetBBReg(): RegAddr(%#lx), BitMask(%#lx), Data(%#lx)\n", RegAddr, BitMask, Data));
	
}


//
// 2. RF register R/W API
//
/**
* Function:	PHY_QueryRFReg
*
* OverView:	Query "Specific bits" to RF register (page 8~) 
*
* Input:
*			PADAPTER		Adapter,
*			RF90_RADIO_PATH_E	eRFPath,	//Radio path of A/B/C/D
*			u4Byte			RegAddr,		//The target address to be read
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be read	
*
* Output:	None
* Return:		u4Byte			Readback value
* Note:		This function is equal to "GetRFRegSetting" in PHY programming guide
*/
u32
PHY_QueryRFReg(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				RegAddr,
	IN	u32				BitMask
	)
{
	u32 Original_Value, Readback_Value, BitShift;	
	//HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	//u8	RFWaitCounter = 0;

#if (DISABLE_BB_RF == 1)
	return 0;
#endif
	
	//RT_TRACE(COMP_RF, DBG_TRACE, ("--->PHY_QueryRFReg(): RegAddr(%#lx), eRFPath(%#x), BitMask(%#lx)\n", RegAddr, eRFPath,BitMask));
	
#if (DEV_BUS_TYPE==USB_INTERFACE)
	//PlatformAcquireMutex(&pHalData->mxRFOperate);
#else
	//PlatformAcquireSpinLock(Adapter, RT_RF_OPERATE_SPINLOCK);
#endif

	
	Original_Value = phy_RFSerialRead(Adapter, eRFPath, RegAddr);
	
	BitShift =  phy_CalculateBitShift(BitMask);
	Readback_Value = (Original_Value & BitMask) >> BitShift;	

#if (DEV_BUS_TYPE==USB_INTERFACE)
	//PlatformReleaseMutex(&pHalData->mxRFOperate);
#else
	//PlatformReleaseSpinLock(Adapter, RT_RF_OPERATE_SPINLOCK);
#endif


	//RTPRINT(FPHY, PHY_RFR, ("RFR-%d MASK=0x%lx Addr[0x%lx]=0x%lx\n", eRFPath, BitMask, RegAddr, Original_Value));//BitMask(%#lx),BitMask,
	//RT_TRACE(COMP_RF, DBG_TRACE, ("<---PHY_QueryRFReg(): RegAddr(%#lx), eRFPath(%#x),  Original_Value(%#lx)\n", 
	//				RegAddr, eRFPath, Original_Value));
	
	return (Readback_Value);
}

/**
* Function:	PHY_SetRFReg
*
* OverView:	Write "Specific bits" to RF register (page 8~) 
*
* Input:
*			PADAPTER		Adapter,
*			RF90_RADIO_PATH_E	eRFPath,	//Radio path of A/B/C/D
*			u4Byte			RegAddr,		//The target address to be modified
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be modified	
*			u4Byte			Data			//The new register Data in the target bit position
*										//of the target address			
*
* Output:	None
* Return:		None
* Note:		This function is equal to "PutRFRegSetting" in PHY programming guide
*/
VOID
PHY_SetRFReg(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				RegAddr,
	IN	u32				BitMask,
	IN	u32				Data
	)
{

	//HAL_DATA_TYPE	*pHalData		= GET_HAL_DATA(Adapter);
	//u1Byte			RFWaitCounter	= 0;
	u32 			Original_Value, BitShift;

#if (DISABLE_BB_RF == 1)
	return;
#endif
	
	//RT_TRACE(COMP_RF, DBG_TRACE, ("--->PHY_SetRFReg(): RegAddr(%#lx), BitMask(%#lx), Data(%#lx), eRFPath(%#x)\n", 
	//	RegAddr, BitMask, Data, eRFPath));
	//RTPRINT(FINIT, INIT_RF, ("PHY_SetRFReg(): RegAddr(%#lx), BitMask(%#lx), Data(%#lx), eRFPath(%#x)\n", 
	//	RegAddr, BitMask, Data, eRFPath));


#if (DEV_BUS_TYPE==USB_INTERFACE)
	//PlatformAcquireMutex(&pHalData->mxRFOperate);
#else
	//PlatformAcquireSpinLock(Adapter, RT_RF_OPERATE_SPINLOCK);
#endif

	
	// RF data is 12 bits only
	if (BitMask != bRFRegOffsetMask) 
	{
		Original_Value = phy_RFSerialRead(Adapter, eRFPath, RegAddr);
		BitShift =  phy_CalculateBitShift(BitMask);
		Data = (((Original_Value) & (~BitMask)) | (Data<< BitShift));
	}
		
	phy_RFSerialWrite(Adapter, eRFPath, RegAddr, Data);
	


#if (DEV_BUS_TYPE==USB_INTERFACE)
	//PlatformReleaseMutex(&pHalData->mxRFOperate);
#else
	//PlatformReleaseSpinLock(Adapter, RT_RF_OPERATE_SPINLOCK);
#endif
	
	//PHY_QueryRFReg(Adapter,eRFPath,RegAddr,BitMask);
	//RT_TRACE(COMP_RF, DBG_TRACE, ("<---PHY_SetRFReg(): RegAddr(%#lx), BitMask(%#lx), Data(%#lx), eRFPath(%#x)\n", 
	//		RegAddr, BitMask, Data, eRFPath));

}


/*-----------------------------------------------------------------------------
 * Function:	phy_FwRFSerialRead()
 *
 * Overview:	We support firmware to execute RF-R/W.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	01/21/2008	MHC		Create Version 0.  
 *
 *---------------------------------------------------------------------------*/
static	u32
phy_FwRFSerialRead(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset	)
{
	u32		retValue = 0;		
	//RT_ASSERT(FALSE,("deprecate!\n"));
	return	(retValue);

}	/* phy_FwRFSerialRead */


/*-----------------------------------------------------------------------------
 * Function:	phy_FwRFSerialWrite()
 *
 * Overview:	We support firmware to execute RF-R/W.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	01/21/2008	MHC		Create Version 0.  
 *
 *---------------------------------------------------------------------------*/
static	VOID
phy_FwRFSerialWrite(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u32				Data	)
{
	//RT_ASSERT(FALSE,("deprecate!\n"));
}


/**
* Function:	phy_RFSerialRead
*
* OverView:	Read regster from RF chips 
*
* Input:
*			PADAPTER		Adapter,
*			RF90_RADIO_PATH_E	eRFPath,	//Radio path of A/B/C/D
*			u4Byte			Offset,		//The target address to be read			
*
* Output:	None
* Return:		u4Byte			reback value
* Note:		Threre are three types of serial operations: 
*			1. Software serial write
*			2. Hardware LSSI-Low Speed Serial Interface 
*			3. Hardware HSSI-High speed
*			serial write. Driver need to implement (1) and (2).
*			This function is equal to the combination of RF_ReadReg() and  RFLSSIRead()
*/
static	u32
phy_RFSerialRead(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset
	)
{
	u32						retValue = 0;
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	BB_REGISTER_DEFINITION_T	*pPhyReg = &pHalData->PHYRegDef[eRFPath];
	u32						NewOffset;
	u32 						tmplong,tmplong2;
	u8					RfPiEnable=0;
#if 0
	if(pHalData->RFChipID == RF_8225 && Offset > 0x24) //36 valid regs
		return	retValue;
	if(pHalData->RFChipID == RF_8256 && Offset > 0x2D) //45 valid regs
		return	retValue;
#endif
	//
	// Make sure RF register offset is correct 
	//
	Offset &= 0x3f;

	//
	// Switch page for 8256 RF IC
	//
	NewOffset = Offset;

	// 2009/06/17 MH We can not execute IO for power save or other accident mode.
	//if(RT_CANNOT_IO(Adapter))
	//{
	//	RTPRINT(FPHY, PHY_RFR, ("phy_RFSerialRead return all one\n"));
	//	return	0xFFFFFFFF;
	//}

	// For 92S LSSI Read RFLSSIRead
	// For RF A/B write 0x824/82c(does not work in the future) 
	// We must use 0x824 for RF A and B to execute read trigger
	tmplong = PHY_QueryBBReg(Adapter, rFPGA0_XA_HSSIParameter2, bMaskDWord);
	if(eRFPath == RF90_PATH_A)
		tmplong2 = tmplong;
	else
	tmplong2 = PHY_QueryBBReg(Adapter, pPhyReg->rfHSSIPara2, bMaskDWord);
	tmplong2 = (tmplong2 & (~bLSSIReadAddress)) | (NewOffset<<23) | bLSSIReadEdge;	//T65 RF
	
	PHY_SetBBReg(Adapter, rFPGA0_XA_HSSIParameter2, bMaskDWord, tmplong&(~bLSSIReadEdge));	
	udelay_os(1000);// PlatformStallExecution(1000);
	
	PHY_SetBBReg(Adapter, pPhyReg->rfHSSIPara2, bMaskDWord, tmplong2);	
	udelay_os(1000);//PlatformStallExecution(1000);
	
	PHY_SetBBReg(Adapter, rFPGA0_XA_HSSIParameter2, bMaskDWord, tmplong|bLSSIReadEdge);	
	udelay_os(1000);//PlatformStallExecution(1000);

	if(eRFPath == RF90_PATH_A)
		RfPiEnable = (u8)PHY_QueryBBReg(Adapter, rFPGA0_XA_HSSIParameter1, BIT8);
	else if(eRFPath == RF90_PATH_B)
		RfPiEnable = (u8)PHY_QueryBBReg(Adapter, rFPGA0_XB_HSSIParameter1, BIT8);
	
	if(RfPiEnable)
	{	// Read from BBreg8b8, 12 bits for 8190, 20bits for T65 RF
		retValue = PHY_QueryBBReg(Adapter, pPhyReg->rfLSSIReadBackPi, bLSSIReadBackData);
	
		//RTPRINT(FINIT, INIT_RF, ("Readback from RF-PI : 0x%x\n", retValue));
	}
	else
	{	//Read from BBreg8a0, 12 bits for 8190, 20 bits for T65 RF
		retValue = PHY_QueryBBReg(Adapter, pPhyReg->rfLSSIReadBack, bLSSIReadBackData);
		
		//RTPRINT(FINIT, INIT_RF,("Readback from RF-SI : 0x%x\n", retValue));
	}
	//RTPRINT(FPHY, PHY_RFR, ("RFR-%d Addr[0x%lx]=0x%lx\n", eRFPath, pPhyReg->rfLSSIReadBack, retValue));
	
	return retValue;	
		
}



/**
* Function:	phy_RFSerialWrite
*
* OverView:	Write data to RF register (page 8~) 
*
* Input:
*			PADAPTER		Adapter,
*			RF90_RADIO_PATH_E	eRFPath,	//Radio path of A/B/C/D
*			u4Byte			Offset,		//The target address to be read			
*			u4Byte			Data			//The new register Data in the target bit position
*										//of the target to be read			
*
* Output:	None
* Return:		None
* Note:		Threre are three types of serial operations: 
*			1. Software serial write
*			2. Hardware LSSI-Low Speed Serial Interface 
*			3. Hardware HSSI-High speed
*			serial write. Driver need to implement (1) and (2).
*			This function is equal to the combination of RF_ReadReg() and  RFLSSIRead()
 *
 * Note: 		  For RF8256 only
 *			 The total count of RTL8256(Zebra4) register is around 36 bit it only employs 
 *			 4-bit RF address. RTL8256 uses "register mode control bit" (Reg00[12], Reg00[10]) 
 *			 to access register address bigger than 0xf. See "Appendix-4 in PHY Configuration
 *			 programming guide" for more details. 
 *			 Thus, we define a sub-finction for RTL8526 register address conversion
 *		       ===========================================================
 *			 Register Mode		RegCTL[1]		RegCTL[0]		Note
 *								(Reg00[12])		(Reg00[10])
 *		       ===========================================================
 *			 Reg_Mode0				0				x			Reg 0 ~15(0x0 ~ 0xf)
 *		       ------------------------------------------------------------------
 *			 Reg_Mode1				1				0			Reg 16 ~30(0x1 ~ 0xf)
 *		       ------------------------------------------------------------------
 *			 Reg_Mode2				1				1			Reg 31 ~ 45(0x1 ~ 0xf)
 *		       ------------------------------------------------------------------
 *
 *	2008/09/02	MH	Add 92S RF definition
 *	
 *
 *
*/
static	VOID
phy_RFSerialWrite(
	IN	PADAPTER			Adapter,
	IN	RF90_RADIO_PATH_E	eRFPath,
	IN	u32				Offset,
	IN	u32				Data
	)
{
	u32						DataAndAddr = 0;
	HAL_DATA_TYPE				*pHalData = GET_HAL_DATA(Adapter);
	BB_REGISTER_DEFINITION_T	*pPhyReg = &pHalData->PHYRegDef[eRFPath];
	u32						NewOffset;
	
#if 0
	//<Roger_TODO> We should check valid regs for RF_6052 case.
	if(pHalData->RFChipID == RF_8225 && Offset > 0x24) //36 valid regs
		return;
	if(pHalData->RFChipID == RF_8256 && Offset > 0x2D) //45 valid regs
		return;
#endif

	// 2009/06/17 MH We can not execute IO for power save or other accident mode.
	//if(RT_CANNOT_IO(Adapter))
	//{
	//	RTPRINT(FPHY, PHY_RFW, ("phy_RFSerialWrite stop\n"));
	//	return;
	//}

	Offset &= 0x3f;

	//
	// Shadow Update
	//
	PHY_RFShadowWrite(Adapter, eRFPath, Offset, Data);	

	//
	// Switch page for 8256 RF IC
	//
	NewOffset = Offset;

	//
	// Put write addr in [5:0]  and write data in [31:16]
	//
	//DataAndAddr = (Data<<16) | (NewOffset&0x3f);
	DataAndAddr = ((NewOffset<<20) | (Data&0x000fffff)) & 0x0fffffff;	// T65 RF

	//
	// Write Operation
	//
	PHY_SetBBReg(Adapter, pPhyReg->rf3wireOffset, bMaskDWord, DataAndAddr);
	//RTPRINT(FPHY, PHY_RFW, ("RFW-%d Addr[0x%lx]=0x%lx\n", eRFPath, pPhyReg->rf3wireOffset, DataAndAddr));

}

/**
* Function:	phy_CalculateBitShift
*
* OverView:	Get shifted position of the BitMask
*
* Input:
*			u4Byte		BitMask,	
*
* Output:	none
* Return:		u4Byte		Return the shift bit bit position of the mask
*/
static	u32
phy_CalculateBitShift(
	u32 BitMask
	)
{
	u32 i;

	for(i=0; i<=31; i++)
	{
		if ( ((BitMask>>i) &  0x1 ) == 1)
			break;
	}

	return (i);
}


//
// 3. Initial MAC/BB/RF config by reading MAC/BB/RF txt.
//
/*-----------------------------------------------------------------------------
 * Function:    PHY_MACConfig8192C
 *
 * Overview:	Condig MAC by header file or parameter file.
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 *  When		Who		Remark
 *  08/12/2008	MHC		Create Version 0.
 *
 *---------------------------------------------------------------------------*/
extern	int
PHY_MACConfig8192C(
	IN	PADAPTER	Adapter
	)
{
	int		rtStatus = _SUCCESS;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

#if DEV_BUS_TYPE == PCI_INTERFACE
	u8			pszMACRegFile[] = RTL819X_PHY_MACREG;
#else
	u8			sz88CMACRegFile[] = RTL8188C_PHY_MACREG;
	u8			sz92CMACRegFile[] = RTL8192C_PHY_MACREG;
	
	u8			*pszMACRegFile;

	if(IS_92C_SERIAL(pHalData->VersionID)){
		pszMACRegFile = (u8*)&sz92CMACRegFile;
	}
	else{
		pszMACRegFile = (u8*)&sz88CMACRegFile;
	}
#endif

	//
	// Config MAC
	//
#ifdef CONFIG_EMBEDDED_FWIMG
	rtStatus = phy_ConfigMACWithHeaderFile(Adapter);
#else
	
	// Not make sure EEPROM, add later
	//RT_TRACE(COMP_INIT, DBG_LOUD, ("Read MACREG.txt\n"));
	rtStatus = phy_ConfigMACWithParaFile(Adapter, pszMACRegFile);
#endif

	return rtStatus;

}


int
PHY_BBConfig8192C(
	IN	PADAPTER	Adapter
	)
{
	int	rtStatus = _SUCCESS;
	u8		PathMap = 0, index = 0, rf_num = 0;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32	RegVal;

	phy_InitBBRFRegisterDefinition(Adapter);

	// Enable BB and RF
	RegVal = read16(Adapter, REG_SYS_FUNC_EN);
	write16(Adapter, REG_SYS_FUNC_EN, RegVal|BIT13|BIT0|BIT1);

	// 20090923 Joseph: Advised by Steven and Jenyu. Power sequence before init RF.
	write8(Adapter, REG_AFE_PLL_CTRL, 0x83);
	write8(Adapter, REG_AFE_PLL_CTRL+1, 0xdb);
	write8(Adapter, REG_RF_CTRL, RF_EN|RF_RSTB|RF_SDMRSTB);

#if DEV_BUS_TYPE == USB_INTERFACE
	write8(Adapter, REG_SYS_FUNC_EN, FEN_USBA | FEN_USBD | FEN_BB_GLB_RSTn | FEN_BBRSTB);
#else
	write8(Adapter, REG_SYS_FUNC_EN, FEN_PPLL|FEN_PCIEA|FEN_DIO_PCIE|FEN_USBA|FEN_BB_GLB_RSTn|FEN_BBRSTB);
#endif

	write8(Adapter, 0x15, 0xe9);
	write8(Adapter, REG_AFE_XTAL_CTRL+1, 0x80);

#if DEV_BUS_TYPE == PCI_INTERFACE
	// Force use left antenna by default for 88C.
	if(!IS_92C_SERIAL(pHalData->VersionID)|| IS_92C_1T2R(pHalData->VersionID))
	{
		RegVal = read32(Adapter, REG_LEDCFG0);
		write32(Adapter, REG_LEDCFG0, RegVal|BIT23);
	}
#endif

	//
	// Config BB and AGC
	//
	rtStatus = phy_BB8192C_Config_ParaFile(Adapter);
#if 0	
	switch(Adapter->MgntInfo.bRegHwParaFile)
	{
		case 0:
			phy_BB8190_Config_HardCode(Adapter);
			break;

		case 1:
			rtStatus = phy_BB8192C_Config_ParaFile(Adapter);
			break;

		case 2:
			// Partial Modify. 
			phy_BB8190_Config_HardCode(Adapter);
			phy_BB8192C_Config_ParaFile(Adapter);
			break;

		default:
			phy_BB8190_Config_HardCode(Adapter);
			break;
	}
#endif	
#if 0
	// Check BB/RF confiuration setting.
	// We only need to configure RF which is turned on.
	PathMap = (u1Byte)(PHY_QueryBBReg(Adapter, rFPGA0_TxInfo, 0xf) |
				PHY_QueryBBReg(Adapter, rOFDM0_TRxPathEnable, 0xf));
	pHalData->RF_PathMap = PathMap;
	for(index = 0; index<4; index++)
	{
		if((PathMap>>index)&0x1)
			rf_num++;
	}

	if((GET_RF_TYPE(Adapter) ==RF_1T1R && rf_num!=1) ||
		(GET_RF_TYPE(Adapter)==RF_1T2R && rf_num!=2) ||
		(GET_RF_TYPE(Adapter)==RF_2T2R && rf_num!=2) ||
		(GET_RF_TYPE(Adapter)==RF_2T2R_GREEN && rf_num!=2) ||
		(GET_RF_TYPE(Adapter)==RF_2T4R && rf_num!=4))
	{
		RT_TRACE(
			COMP_INIT, 
			DBG_LOUD, 
			("PHY_BBConfig8192C: RF_Type(%x) does not match RF_Num(%x)!!\n", pHalData->RF_Type, rf_num));
	}
#endif

	return rtStatus;
}


extern	int
PHY_RFConfig8192C(
	IN	PADAPTER	Adapter
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	int		rtStatus = _SUCCESS;

	//
	// RF config
	//
	rtStatus = PHY_RF6052_Config(Adapter);
#if 0	
	switch(pHalData->rf_chip)
	{
		case RF_6052:
			rtStatus = PHY_RF6052_Config(Adapter);
			break;
		case RF_8225:
			rtStatus = PHY_RF8225_Config(Adapter);
			break;
		case RF_8256:			
			rtStatus = PHY_RF8256_Config(Adapter);
			break;
		case RF_8258:
			break;
		case RF_PSEUDO_11N:
			rtStatus = PHY_RF8225_Config(Adapter);
			break;
		default: //for MacOs Warning: "RF_TYPE_MIN" not handled in switch
			break;
	}
#endif
	return rtStatus;
}


// Joseph test: new initialize order!!
// Test only!! This part need to be re-organized.
// Now it is just for 8256.
static	int
phy_BB8190_Config_HardCode(
	IN	PADAPTER	Adapter
	)
{
	//RT_ASSERT(FALSE, ("This function is not implement yet!! \n"));
	return _SUCCESS;
}

static	int
phy_BB8192C_Config_ParaFile(
	IN	PADAPTER	Adapter
	)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	int			rtStatus = _SUCCESS;

	u8				szBBRegPgFile[] = RTL819X_PHY_REG_PG;
	
	u8				sz88CBBRegFile[] = RTL8188C_PHY_REG;	
	u8				sz88CAGCTableFile[] = RTL8188C_AGC_TAB;

	u8				sz92CBBRegFile[] = RTL8192C_PHY_REG;	
	u8				sz92CAGCTableFile[] = RTL8192C_AGC_TAB;
	
	u8                             *pszBBRegFile, *pszAGCTableFile;
	
	//RT_TRACE(COMP_INIT, DBG_TRACE, ("==>phy_BB8192S_Config_ParaFile\n"));

	if(IS_92C_SERIAL(pHalData->VersionID)){
		pszBBRegFile=(u8*)&sz92CBBRegFile ;
		pszAGCTableFile =(u8*)&sz92CAGCTableFile;
	}
	else{
		pszBBRegFile=(u8*)&sz88CBBRegFile ;
		pszAGCTableFile =(u8*)&sz88CAGCTableFile;
	}

	//
	// 1. Read PHY_REG.TXT BB INIT!!
	// We will seperate as 88C / 92C according to chip version
	//
#ifdef CONFIG_EMBEDDED_FWIMG
	rtStatus = phy_ConfigBBWithHeaderFile(Adapter, BaseBand_Config_PHY_REG);	
#else	
	// No matter what kind of CHIP we always read PHY_REG.txt. We must copy different 
	// type of parameter files to phy_reg.txt at first.	
	rtStatus = phy_ConfigBBWithParaFile(Adapter,pszBBRegFile);
#endif

	if(rtStatus != _SUCCESS){
		//RT_TRACE(COMP_INIT, DBG_SERIOUS, ("phy_BB8192S_Config_ParaFile():Write BB Reg Fail!!"));
		goto phy_BB8190_Config_ParaFile_Fail;
	}

	//
	// 2. If EEPROM or EFUSE autoload OK, We must config by PHY_REG_PG.txt
	//
	if (pEEPROM->bautoload_fail_flag == _FALSE)
	{
		pHalData->pwrGroupCnt = 0;

#ifdef CONFIG_EMBEDDED_FWIMG
		rtStatus = phy_ConfigBBWithPgHeaderFile(Adapter, BaseBand_Config_PHY_REG);
#else
		rtStatus = phy_ConfigBBWithPgParaFile(Adapter, (u8*)&szBBRegPgFile);
#endif
	}
	
	if(rtStatus != _SUCCESS){
		//RT_TRACE(COMP_INIT, DBG_SERIOUS, ("phy_BB8192S_Config_ParaFile():BB_PG Reg Fail!!"));
		goto phy_BB8190_Config_ParaFile_Fail;
	}

	//
	// 3. BB AGC table Initialization
	//
#ifdef CONFIG_EMBEDDED_FWIMG
	rtStatus = phy_ConfigBBWithHeaderFile(Adapter, BaseBand_Config_AGC_TAB);
#else
	//RT_TRACE(COMP_INIT, DBG_LOUD, ("phy_BB8192S_Config_ParaFile AGC_TAB.txt\n"));
	rtStatus = phy_ConfigBBWithParaFile(Adapter, pszAGCTableFile);
#endif

	if(rtStatus != _SUCCESS){
		//RT_TRACE(COMP_FPGA, DBG_SERIOUS, ("phy_BB8192S_Config_ParaFile():AGC Table Fail\n"));
		goto phy_BB8190_Config_ParaFile_Fail;
	}

	// Check if the CCK HighPower is turned ON.
	// This is used to calculate PWDB.
	pHalData->bCckHighPower = (BOOLEAN)(PHY_QueryBBReg(Adapter, rFPGA0_XA_HSSIParameter2, 0x200));
	
phy_BB8190_Config_ParaFile_Fail:

	return rtStatus;
}



/*-----------------------------------------------------------------------------
 * Function:    phy_ConfigMACWithParaFile()
 *
 * Overview:    This function read BB parameters from general file format, and do register
 *			  Read/Write 
 *
 * Input:      	PADAPTER		Adapter
 *			ps1Byte 			pFileName			
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *			
 * Note: 		The format of MACPHY_REG.txt is different from PHY and RF. 
 *			[Register][Mask][Value]
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigMACWithParaFile(
	IN	PADAPTER		Adapter,
	IN	u8* 			pFileName
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	int		rtStatus = _SUCCESS;

	return rtStatus;
}

/*-----------------------------------------------------------------------------
 * Function:    phy_ConfigMACWithHeaderFile()
 *
 * Overview:    This function read BB parameters from Header file we gen, and do register
 *			  Read/Write 
 *
 * Input:      	PADAPTER		Adapter
 *			ps1Byte 			pFileName			
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *			
 * Note: 		The format of MACPHY_REG.txt is different from PHY and RF. 
 *			[Register][Mask][Value]
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigMACWithHeaderFile(
	IN	PADAPTER		Adapter
)
{
	u32					i = 0;
	u32					ArrayLength = 0;
	u32*					ptrArray;	
	//HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(Adapter);

	//2008.11.06 Modified by tynli.
	//RT_TRACE(COMP_INIT, DBG_LOUD, ("Read Rtl819XMACPHY_Array\n"));
	ArrayLength = MAC_2T_ArrayLength;
	ptrArray = (u32*)&Rtl819XMAC_Array[0];	

	for(i = 0 ;i < ArrayLength;i=i+2){ // Add by tynli for 2 column
		write8(Adapter, ptrArray[i], (u8)ptrArray[i+1]);
	}
	
	return _SUCCESS;
	
}


/*-----------------------------------------------------------------------------
 * Function:    phy_ConfigBBWithParaFile()
 *
 * Overview:    This function read BB parameters from general file format, and do register
 *			  Read/Write 
 *
 * Input:      	PADAPTER		Adapter
 *			ps1Byte 			pFileName			
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *	2008/11/06	MH	For 92S we do not support silent reset now. Disable 
 *					parameter file compare!!!!!!??
 *			
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigBBWithParaFile(
	IN	PADAPTER		Adapter,
	IN	u8* 			pFileName
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	int		rtStatus = _SUCCESS;

	return rtStatus;	
}

/*-----------------------------------------------------------------------------
 * Function:    phy_SetBBtoDiffRFWithParaFile()
 *
 * Overview:    This function read BB parameters from general file format, and do register
 *			  Read/Write 
 *
 * Input:      	PADAPTER		Adapter
 *			ps1Byte 			pFileName			
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *
 * 2008/11/10	tynli	
 * 2009/07/29	tynli (porting from 92SE branch) Add copy parameter file to buffer for silent reset
 *			
 *---------------------------------------------------------------------------*/
static	int
phy_SetBBtoDiffRFWithParaFile(
	IN	PADAPTER		Adapter,
	IN	u8* 			pFileName
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	int		rtStatus = _SUCCESS;


	return rtStatus;	
}

//****************************************
// The following is for High Power PA
//****************************************
VOID
phy_ConfigBBExternalPA(
	IN	PADAPTER			Adapter
)
{
#if (DEV_BUS_TYPE == USB_INTERFACE)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16 i=0;
	u32 temp=0;

	if(!pHalData->ExternalPA)
	{
		return;
	}

	DBG_8192C("external PA, BB Setting\n");
	
	PHY_SetBBReg(Adapter, 0xee8, BIT28, 1);
	temp = PHY_QueryBBReg(Adapter, 0x860, bMaskDWord);
	temp |= (BIT26|BIT21|BIT10|BIT5);
	PHY_SetBBReg(Adapter, 0x860, bMaskDWord, temp);
	PHY_SetBBReg(Adapter, 0x870, BIT10, 0);
	PHY_SetBBReg(Adapter, 0xc80, bMaskDWord, 0x20000080);
	PHY_SetBBReg(Adapter, 0xc88, bMaskDWord, 0x40000100);
#endif
}

/*-----------------------------------------------------------------------------
 * Function:    phy_ConfigBBWithHeaderFile()
 *
 * Overview:    This function read BB parameters from general file format, and do register
 *			  Read/Write 
 *
 * Input:      	PADAPTER		Adapter
 *			u1Byte 			ConfigType     0 => PHY_CONFIG
 *										 1 =>AGC_TAB
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *			
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigBBWithHeaderFile(
	IN	PADAPTER		Adapter,
	IN	u8 			ConfigType
)
{
	int i;
	u32*	Rtl819XPHY_REGArray_Table;
	u32*	Rtl819XAGCTAB_Array_Table;
	u16	PHY_REGArrayLen, AGCTAB_ArrayLen;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	//
	// 2009.11.24. Modified by tynli.
	//
	if(IS_92C_SERIAL(pHalData->VersionID))
	{
		AGCTAB_ArrayLen = AGCTAB_2TArrayLength;
		Rtl819XAGCTAB_Array_Table = (u32*)Rtl819XAGCTAB_2TArray;
		PHY_REGArrayLen = PHY_REG_2TArrayLength;
		Rtl819XPHY_REGArray_Table = (u32*)Rtl819XPHY_REG_2TArray;
	}
	else
	{
		AGCTAB_ArrayLen = AGCTAB_1TArrayLength;
		Rtl819XAGCTAB_Array_Table = (u32*)Rtl819XAGCTAB_1TArray;
		PHY_REGArrayLen = PHY_REG_1TArrayLength;
		Rtl819XPHY_REGArray_Table = (u32*)Rtl819XPHY_REG_1TArray;
	}

	if(ConfigType == BaseBand_Config_PHY_REG)
	{
		for(i=0;i<PHY_REGArrayLen;i=i+2)
		{
			if (Rtl819XPHY_REGArray_Table[i] == 0xfe)
				mdelay_os(50);
			else if (Rtl819XPHY_REGArray_Table[i] == 0xfd)
				mdelay_os(5);
			else if (Rtl819XPHY_REGArray_Table[i] == 0xfc)
				mdelay_os(1);
			else if (Rtl819XPHY_REGArray_Table[i] == 0xfb)
				udelay_os(50);
			else if (Rtl819XPHY_REGArray_Table[i] == 0xfa)
				udelay_os(5);
			else if (Rtl819XPHY_REGArray_Table[i] == 0xf9)
				udelay_os(1);
			PHY_SetBBReg(Adapter, Rtl819XPHY_REGArray_Table[i], bMaskDWord, Rtl819XPHY_REGArray_Table[i+1]);		

			// Add 1us delay between BB/RF register setting.
			udelay_os(1);

			//RT_TRACE(COMP_INIT, DBG_TRACE, ("The Rtl819XPHY_REGArray_Table[0] is %lx Rtl819XPHY_REGArray[1] is %lx \n",Rtl819XPHY_REGArray_Table[i], Rtl819XPHY_REGArray_Table[i+1]));
		}
		// for External PA
		phy_ConfigBBExternalPA(Adapter);
	}
	else if(ConfigType == BaseBand_Config_AGC_TAB)
	{
		for(i=0;i<AGCTAB_ArrayLen;i=i+2)
		{
			PHY_SetBBReg(Adapter, Rtl819XAGCTAB_Array_Table[i], bMaskDWord, Rtl819XAGCTAB_Array_Table[i+1]);		

			// Add 1us delay between BB/RF register setting.
			udelay_os(1);
			
			//RT_TRACE(COMP_INIT, DBG_TRACE, ("The Rtl819XAGCTAB_Array_Table[0] is %lx Rtl819XPHY_REGArray[1] is %lx \n",Rtl819XAGCTAB_Array_Table[i], Rtl819XAGCTAB_Array_Table[i+1]));
		}
	}
	
	return _SUCCESS;
	
}

/*-----------------------------------------------------------------------------
 * Function:    phy_SetBBtoDiffRFWithHeaderFile()
 *
 * Overview:    This function 
 *			
 *
 * Input:      	PADAPTER		Adapter
 *			u1Byte 			ConfigType     0 => PHY_CONFIG
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 * When			Who		Remark
 * 2008/11/10	tynli
 *			
 *---------------------------------------------------------------------------*/
static	int
phy_SetBBtoDiffRFWithHeaderFile(
	IN	PADAPTER		Adapter,
	IN	u8			ConfigType
)
{
#if 0 //Marked by tynli. 2009.11.24. CE does not need this function.

	int i;
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pu4Byte 			Rtl819XPHY_REGArraytoXTXR_Table;
	u2Byte			PHY_REGArraytoXTXRLen;
	
	if(GET_RF_TYPE(Adapter) == RF_1T1R)
	{
		Rtl819XPHY_REGArraytoXTXR_Table = Rtl819XPHY_REG_to1T1R_Array;
		PHY_REGArraytoXTXRLen = PHY_ChangeTo_1T1RArrayLength;
		RT_TRACE(COMP_INIT, DBG_LOUD, ("phy_SetBBtoDiffRFWithHeaderFile: Set to 1T1R..\n"));
	} 
	else if(GET_RF_TYPE(Adapter) == RF_1T2R)
	{
		Rtl819XPHY_REGArraytoXTXR_Table = Rtl819XPHY_REG_to1T2R_Array;
		PHY_REGArraytoXTXRLen = PHY_ChangeTo_1T2RArrayLength;
		RT_TRACE(COMP_INIT, DBG_LOUD, ("phy_SetBBtoDiffRFWithHeaderFile: Set to 1T2R..\n"));
	}
//	else if(pHalData->RF_Type == RF_2T2R || pHalData->RF_Type == RF_2T2R_GREEN)
//	{
//		Rtl819XPHY_REGArraytoXTXR_Table = Rtl819XPHY_REG_to2T2R_Array;
//		PHY_REGArraytoXTXRLen = PHY_ChangeTo_2T2RArrayLength;
//	}
	else
	{
		return RT_STATUS_FAILURE;
	}

	if(ConfigType == BaseBand_Config_PHY_REG)
	{
		for(i=0;i<PHY_REGArraytoXTXRLen;i=i+3)
		{
			if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xfe)
				delay_ms(50);
			else if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xfd)
				delay_ms(5);
			else if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xfc)
				delay_ms(1);
			else if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xfb)
				PlatformStallExecution(50);
			else if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xfa)
				PlatformStallExecution(5);
			else if (Rtl819XPHY_REGArraytoXTXR_Table[i] == 0xf9)
				PlatformStallExecution(1);
			PHY_SetBBReg(Adapter, Rtl819XPHY_REGArraytoXTXR_Table[i], Rtl819XPHY_REGArraytoXTXR_Table[i+1], Rtl819XPHY_REGArraytoXTXR_Table[i+2]);		
			RT_TRACE(COMP_INIT, DBG_TRACE, 
			("The Rtl819XPHY_REGArraytoXTXR_Table[0] is %lx Rtl819XPHY_REGArraytoXTXR_Table[1] is %lx Rtl819XPHY_REGArraytoXTXR_Table[2] is %lx \n",
			Rtl819XPHY_REGArraytoXTXR_Table[i],Rtl819XPHY_REGArraytoXTXR_Table[i+1], Rtl819XPHY_REGArraytoXTXR_Table[i+2]));
		}
	}
	else {
		RT_TRACE(COMP_INIT, DBG_LOUD, ("phy_SetBBtoDiffRFWithHeaderFile(): ConfigType != BaseBand_Config_PHY_REG\n"));
	}
#endif	// #if 0	
	return _SUCCESS;
}


VOID
storePwrIndexDiffRateOffset(
	IN	PADAPTER	Adapter,
	IN	u32		RegAddr,
	IN	u32		BitMask,
	IN	u32		Data
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	if(RegAddr == rTxAGC_A_Rate18_06)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][0] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][0] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][0]));
	}
	if(RegAddr == rTxAGC_A_Rate54_24)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][1] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][1] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][1]));
	}
	if(RegAddr == rTxAGC_A_CCK1_Mcs32)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][6] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][6] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][6]));
	}
	if(RegAddr == rTxAGC_B_CCK11_A_CCK2_11 && BitMask == 0xffffff00)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][7] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][7] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][7]));
	}	
	if(RegAddr == rTxAGC_A_Mcs03_Mcs00)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][2] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][2] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][2]));
	}
	if(RegAddr == rTxAGC_A_Mcs07_Mcs04)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][3] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][3] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][3]));
	}
	if(RegAddr == rTxAGC_A_Mcs11_Mcs08)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][4] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][4] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][4]));
	}
	if(RegAddr == rTxAGC_A_Mcs15_Mcs12)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][5] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][5] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][5]));
	}
	if(RegAddr == rTxAGC_B_Rate18_06)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][8] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][8] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][8]));
	}
	if(RegAddr == rTxAGC_B_Rate54_24)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][9] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][9] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][9]));
	}
	if(RegAddr == rTxAGC_B_CCK1_55_Mcs32)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][14] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][14] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][14]));
	}
	if(RegAddr == rTxAGC_B_CCK11_A_CCK2_11 && BitMask == 0x000000ff)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][15] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][15] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][15]));
	}	
	if(RegAddr == rTxAGC_B_Mcs03_Mcs00)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][10] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][10] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][10]));
	}
	if(RegAddr == rTxAGC_B_Mcs07_Mcs04)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][11] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][11] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][11]));
	}
	if(RegAddr == rTxAGC_B_Mcs11_Mcs08)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][12] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][12] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][12]));
	}
	if(RegAddr == rTxAGC_B_Mcs15_Mcs12)
	{
		pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][13] = Data;
		//RT_TRACE(COMP_INIT, DBG_TRACE, ("MCSTxPowerLevelOriginalOffset[%d][13] = 0x%lx\n", pHalData->pwrGroupCnt,
		//	pHalData->MCSTxPowerLevelOriginalOffset[pHalData->pwrGroupCnt][13]));
		pHalData->pwrGroupCnt++;
	}
}
/*-----------------------------------------------------------------------------
 * Function:	phy_ConfigBBWithPgParaFile
 *
 * Overview:	
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/06/2008 	MHC		Create Version 0.
 * 2009/07/29	tynli		(porting from 92SE branch)2009/03/11 Add copy parameter file to buffer for silent reset
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigBBWithPgParaFile(
	IN	PADAPTER		Adapter,
	IN	u8* 			pFileName)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	int		rtStatus = _SUCCESS;


	return rtStatus;
	
}	/* phy_ConfigBBWithPgParaFile */


/*-----------------------------------------------------------------------------
 * Function:	phy_ConfigBBWithPgHeaderFile
 *
 * Overview:	Config PHY_REG_PG array 
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Revised History:
 * When			Who		Remark
 * 11/06/2008 	MHC		Add later!!!!!!.. Please modify for new files!!!!
 * 11/10/2008	tynli		Modify to mew files.
 *---------------------------------------------------------------------------*/
static	int
phy_ConfigBBWithPgHeaderFile(
	IN	PADAPTER		Adapter,
	IN	u8 			ConfigType)
{
	int i;
	u32*	Rtl819XPHY_REGArray_Table_PG;
	u16	PHY_REGArrayPGLen;
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	// Default: pHalData->RF_Type = RF_2T2R.
	
	PHY_REGArrayPGLen = PHY_REG_Array_PGLength;
	Rtl819XPHY_REGArray_Table_PG = (u32*)Rtl819XPHY_REG_Array_PG;

	if(ConfigType == BaseBand_Config_PHY_REG)
	{
		for(i=0;i<PHY_REGArrayPGLen;i=i+3)
		{
			if (Rtl819XPHY_REGArray_Table_PG[i] == 0xfe)
				mdelay_os(50);
			else if (Rtl819XPHY_REGArray_Table_PG[i] == 0xfd)
				mdelay_os(5);
			else if (Rtl819XPHY_REGArray_Table_PG[i] == 0xfc)
				mdelay_os(1);
			else if (Rtl819XPHY_REGArray_Table_PG[i] == 0xfb)
				udelay_os(50);
			else if (Rtl819XPHY_REGArray_Table_PG[i] == 0xfa)
				udelay_os(5);
			else if (Rtl819XPHY_REGArray_Table_PG[i] == 0xf9)
				udelay_os(1);
			storePwrIndexDiffRateOffset(Adapter, Rtl819XPHY_REGArray_Table_PG[i], 
				Rtl819XPHY_REGArray_Table_PG[i+1], 
				Rtl819XPHY_REGArray_Table_PG[i+2]);
			//PHY_SetBBReg(Adapter, Rtl819XPHY_REGArray_Table_PG[i], Rtl819XPHY_REGArray_Table_PG[i+1], Rtl819XPHY_REGArray_Table_PG[i+2]);		
			//RT_TRACE(COMP_SEND, DBG_TRACE, ("The Rtl819XPHY_REGArray_Table_PG[0] is %lx Rtl819XPHY_REGArray_Table_PG[1] is %lx \n",Rtl819XPHY_REGArray_Table_PG[i], Rtl819XPHY_REGArray_Table_PG[i+1]));
		}
	}
	else
	{

		//RT_TRACE(COMP_SEND, DBG_LOUD, ("phy_ConfigBBWithPgHeaderFile(): ConfigType != BaseBand_Config_PHY_REG\n"));
	}
	
	return _SUCCESS;
	
}	/* phy_ConfigBBWithPgHeaderFile */


/*-----------------------------------------------------------------------------
 * Function:    PHY_ConfigRFWithParaFile()
 *
 * Overview:    This function read RF parameters from general file format, and do RF 3-wire
 *
 * Input:      	PADAPTER			Adapter
 *			ps1Byte 				pFileName			
 *			RF90_RADIO_PATH_E	eRFPath
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *			
 * Note:		Delay may be required for RF configuration
 *---------------------------------------------------------------------------*/
int
PHY_ConfigRFWithParaFile(
	IN	PADAPTER			Adapter,
	IN	u8* 				pFileName,
	RF90_RADIO_PATH_E		eRFPath
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	int	rtStatus = _SUCCESS;


	return rtStatus;
	
}

//****************************************
// The following is for High Power PA
//****************************************
#define HighPowerRadioAArrayLen 22
//This is for High power PA
u32 Rtl8192S_HighPower_RadioA_Array[HighPowerRadioAArrayLen] = {
0x013,0x00029ea4,
0x013,0x00025e74,
0x013,0x00020ea4,
0x013,0x0001ced0,
0x013,0x00019f40,
0x013,0x00014e70,
0x013,0x000106a0,
0x013,0x0000c670,
0x013,0x000082a0,
0x013,0x00004270,
0x013,0x00000240,
};

int
PHY_ConfigRFExternalPA(
	IN	PADAPTER			Adapter,
	RF90_RADIO_PATH_E		eRFPath
)
{
	int	rtStatus = _SUCCESS;
#if (DEV_BUS_TYPE == USB_INTERFACE)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u16 i=0;

	if(!pHalData->ExternalPA)
	{
		return rtStatus;
	}
	
	DBG_8192C("external PA, RF Setting\n");

	//add for SU High Power PA
	for(i = 0;i<HighPowerRadioAArrayLen; i=i+2)
	{
		//RT_TRACE(COMP_INIT, DBG_LOUD, ("External PA, write RF 0x%x=0x%x\n", Rtl8192S_HighPower_RadioA_Array[i], Rtl8192S_HighPower_RadioA_Array[i+1]));
		PHY_SetRFReg(Adapter, eRFPath, Rtl8192S_HighPower_RadioA_Array[i], bRFRegOffsetMask, Rtl8192S_HighPower_RadioA_Array[i+1]);
	}
#endif
	return rtStatus;
}
//****************************************
/*-----------------------------------------------------------------------------
 * Function:    PHY_ConfigRFWithHeaderFile()
 *
 * Overview:    This function read RF parameters from general file format, and do RF 3-wire
 *
 * Input:      	PADAPTER			Adapter
 *			ps1Byte 				pFileName			
 *			RF90_RADIO_PATH_E	eRFPath
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: configuration file exist
 *			
 * Note:		Delay may be required for RF configuration
 *---------------------------------------------------------------------------*/
int
PHY_ConfigRFWithHeaderFile(
	IN	PADAPTER			Adapter,
	RF90_RADIO_PATH_E		eRFPath
)
{

	int			i;
	int	rtStatus = _SUCCESS;
	u32*		Rtl819XRadioA_Array_Table;
	u32*		Rtl819XRadioB_Array_Table;
	u16		RadioA_ArrayLen,RadioB_ArrayLen;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	//
	// 2009.11.24. Modified by tynli.
	//
	if(IS_92C_SERIAL(pHalData->VersionID))
	{
		RadioA_ArrayLen = RadioA_2TArrayLength;
		Rtl819XRadioA_Array_Table=(u32*)Rtl819XRadioA_2TArray;

		RadioB_ArrayLen = RadioB_2TArrayLength;
		Rtl819XRadioB_Array_Table = (u32*)Rtl819XRadioB_2TArray;
			
	}
	else
	{
#if DEV_BUS_TYPE==USB_INTERFACE		
		if( BOARD_MINICARD == pHalData->BoardType )
		{
			RadioA_ArrayLen = RadioA_1T_mCardArrayLength;
			Rtl819XRadioA_Array_Table=(u32*)Rtl819XRadioA_1T_mCardArray;
			RadioB_ArrayLen = RadioB_1T_mCardArrayLength;	
			Rtl819XRadioB_Array_Table = (u32*)Rtl819XRadioB_1T_mCardArray;			
		}
		else
#endif				
		{
			
			RadioA_ArrayLen = RadioA_1TArrayLength;
			Rtl819XRadioA_Array_Table=(u32*)Rtl819XRadioA_1TArray;

			RadioB_ArrayLen = RadioB_1TArrayLength;
			Rtl819XRadioB_Array_Table = (u32*)Rtl819XRadioB_1TArray;
		}

	}

	switch(eRFPath){
		case RF90_PATH_A:
			for(i = 0;i<RadioA_ArrayLen; i=i+2)
			{
				if(Rtl819XRadioA_Array_Table[i] == 0xfe)
					mdelay_os(50);
				else if (Rtl819XRadioA_Array_Table[i] == 0xfd)
					mdelay_os(5);
				else if (Rtl819XRadioA_Array_Table[i] == 0xfc)
					mdelay_os(1);
				else if (Rtl819XRadioA_Array_Table[i] == 0xfb)
					udelay_os(50);
				else if (Rtl819XRadioA_Array_Table[i] == 0xfa)
					udelay_os(5);
				else if (Rtl819XRadioA_Array_Table[i] == 0xf9)
					udelay_os(1);
				else
				{
					PHY_SetRFReg(Adapter, eRFPath, Rtl819XRadioA_Array_Table[i], bRFRegOffsetMask, Rtl819XRadioA_Array_Table[i+1]);
					// Add 1us delay between BB/RF register setting.
					udelay_os(1);
				}
			}			
			//Add for High Power PA
			PHY_ConfigRFExternalPA(Adapter, eRFPath);
			break;
		case RF90_PATH_B:
			for(i = 0;i<RadioB_ArrayLen; i=i+2)
			{
				if(Rtl819XRadioB_Array_Table[i] == 0xfe)
				{ // Deay specific ms. Only RF configuration require delay.												
#if (DEV_BUS_TYPE == USB_INTERFACE)
					mdelay_os(1000);
#else
					mdelay_os(50);
#endif
				}
				else if (Rtl819XRadioB_Array_Table[i] == 0xfd)
					mdelay_os(5);
				else if (Rtl819XRadioB_Array_Table[i] == 0xfc)
					mdelay_os(1);
				else if (Rtl819XRadioB_Array_Table[i] == 0xfb)
					udelay_os(50);
				else if (Rtl819XRadioB_Array_Table[i] == 0xfa)
					udelay_os(5);
				else if (Rtl819XRadioB_Array_Table[i] == 0xf9)
					udelay_os(1);
				else
				{
					PHY_SetRFReg(Adapter, eRFPath, Rtl819XRadioB_Array_Table[i], bRFRegOffsetMask, Rtl819XRadioB_Array_Table[i+1]);
					// Add 1us delay between BB/RF register setting.
					udelay_os(1);
				}	
			}			
			break;
		case RF90_PATH_C:
			break;
		case RF90_PATH_D:
			break;
	}
	
	return _SUCCESS;

}


/*-----------------------------------------------------------------------------
 * Function:    PHY_CheckBBAndRFOK()
 *
 * Overview:    This function is write register and then readback to make sure whether
 *			  BB[PHY0, PHY1], RF[Patha, path b, path c, path d] is Ok
 *
 * Input:      	PADAPTER			Adapter
 *			HW90_BLOCK_E		CheckBlock
 *			RF90_RADIO_PATH_E	eRFPath		// it is used only when CheckBlock is HW90_BLOCK_RF
 *
 * Output:      NONE
 *
 * Return:      RT_STATUS_SUCCESS: PHY is OK
 *			
 * Note:		This function may be removed in the ASIC
 *---------------------------------------------------------------------------*/
int
PHY_CheckBBAndRFOK(
	IN	PADAPTER			Adapter,
	IN	HW90_BLOCK_E		CheckBlock,
	IN	RF90_RADIO_PATH_E	eRFPath
	)
{
	int			rtStatus = _SUCCESS;

	u32				i, CheckTimes = 4,ulRegRead;

	u32				WriteAddr[4];
	u32				WriteData[] = {0xfffff027, 0xaa55a02f, 0x00000027, 0x55aa502f};

	// Initialize register address offset to be checked
	WriteAddr[HW90_BLOCK_MAC] = 0x100;
	WriteAddr[HW90_BLOCK_PHY0] = 0x900;
	WriteAddr[HW90_BLOCK_PHY1] = 0x800;
	WriteAddr[HW90_BLOCK_RF] = 0x3;
	
	for(i=0 ; i < CheckTimes ; i++)
	{

		//
		// Write Data to register and readback
		//
		switch(CheckBlock)
		{
		case HW90_BLOCK_MAC:
			//RT_ASSERT(FALSE, ("PHY_CheckBBRFOK(): Never Write 0x100 here!"));
			//RT_TRACE(COMP_INIT, DBG_LOUD, ("PHY_CheckBBRFOK(): Never Write 0x100 here!\n"));
			break;
			
		case HW90_BLOCK_PHY0:
		case HW90_BLOCK_PHY1:
			write32(Adapter, WriteAddr[CheckBlock], WriteData[i]);
			ulRegRead = read32(Adapter, WriteAddr[CheckBlock]);
			break;

		case HW90_BLOCK_RF:
			// When initialization, we want the delay function(delay_ms(), delay_us() 
			// ==> actually we call PlatformStallExecution()) to do NdisStallExecution()
			// [busy wait] instead of NdisMSleep(). So we acquire RT_INITIAL_SPINLOCK 
			// to run at Dispatch level to achive it.	
			//cosa PlatformAcquireSpinLock(Adapter, RT_INITIAL_SPINLOCK);
			WriteData[i] &= 0xfff;
			PHY_SetRFReg(Adapter, eRFPath, WriteAddr[HW90_BLOCK_RF], bRFRegOffsetMask, WriteData[i]);
			// TODO: we should not delay for such a long time. Ask SD3
			mdelay_os(10);
			ulRegRead = PHY_QueryRFReg(Adapter, eRFPath, WriteAddr[HW90_BLOCK_RF], bMaskDWord);				
			mdelay_os(10);
			//cosa PlatformReleaseSpinLock(Adapter, RT_INITIAL_SPINLOCK);
			break;
			
		default:
			rtStatus = _FAIL;
			break;
		}


		//
		// Check whether readback data is correct
		//
		if(ulRegRead != WriteData[i])
		{
			//RT_TRACE(COMP_FPGA, DBG_LOUD, ("ulRegRead: %lx, WriteData: %lx \n", ulRegRead, WriteData[i]));
			rtStatus = _FAIL;			
			break;
		}
	}

	return rtStatus;
}

#if 0
VOID
PHY_SetRFPowerState8192SUsb(
	IN	PADAPTER		Adapter,
	IN	RF_POWER_STATE	RFPowerState
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			WaitShutDown = FALSE;
	//RF90_RADIO_PATH_E	eRFPath;
	u1Byte				eRFPath;
	BB_REGISTER_DEFINITION_T	*pPhyReg;
	
	if(pHalData->SetRFPowerStateInProgress == TRUE)
		return;
	
	pHalData->SetRFPowerStateInProgress = TRUE;
	
	// TODO: Emily, 2006.11.21, we should rewrite this function

	if(RFPowerState==RF_SHUT_DOWN)
	{
		RFPowerState=RF_OFF;
		WaitShutDown=TRUE;
	}
	
	
	pHalData->RFPowerState = RFPowerState;
	switch( pHalData->RFChipID )
	{
	case RF_8225:
	case RF_6052:
		switch( RFPowerState )
		{
		case RF_ON:
			break;
	
		case RF_SLEEP:
			break;
	
		case RF_OFF:
			break;
		}
		break;

	case RF_8256:
		switch( RFPowerState )
		{
		case RF_ON:
			break;
	
		case RF_SLEEP:
			break;
	
		case RF_OFF:
			for(eRFPath=(RF90_RADIO_PATH_E)RF90_PATH_A; eRFPath < RF90_PATH_MAX; eRFPath++)
			{
				if (!Adapter->HalFunc.PHYCheckIsLegalRfPathHandler(Adapter, eRFPath))		
					continue;	
				
				pPhyReg = &pHalData->PHYRegDef[eRFPath];
				PHY_SetBBReg(Adapter, pPhyReg->rfintfs, bRFSI_RFENV, bRFSI_RFENV);
				PHY_SetBBReg(Adapter, pPhyReg->rfintfo, bRFSI_RFENV, 0);
			}
			break;
		}
		break;
		
	case RF_8258:
		break;
	}// switch( pHalData->RFChipID )

	pHalData->SetRFPowerStateInProgress = FALSE;
}
#endif

extern	VOID
PHY_GetHWRegOriginalValue(
	IN	PADAPTER		Adapter
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	// read rx initial gain
	pHalData->DefaultInitialGain[0] = (u8)PHY_QueryBBReg(Adapter, rOFDM0_XAAGCCore1, bMaskByte0);
	pHalData->DefaultInitialGain[1] = (u8)PHY_QueryBBReg(Adapter, rOFDM0_XBAGCCore1, bMaskByte0);
	pHalData->DefaultInitialGain[2] = (u8)PHY_QueryBBReg(Adapter, rOFDM0_XCAGCCore1, bMaskByte0);
	pHalData->DefaultInitialGain[3] = (u8)PHY_QueryBBReg(Adapter, rOFDM0_XDAGCCore1, bMaskByte0);
	//RT_TRACE(COMP_INIT, DBG_LOUD,
	//("Default initial gain (c50=0x%x, c58=0x%x, c60=0x%x, c68=0x%x) \n", 
	//pHalData->DefaultInitialGain[0], pHalData->DefaultInitialGain[1], 
	//pHalData->DefaultInitialGain[2], pHalData->DefaultInitialGain[3]));

	// read framesync
	pHalData->framesync = (u8)PHY_QueryBBReg(Adapter, rOFDM0_RxDetector3, bMaskByte0);	 
	pHalData->framesyncC34 = PHY_QueryBBReg(Adapter, rOFDM0_RxDetector2, bMaskDWord);
	//RT_TRACE(COMP_INIT, DBG_LOUD, ("Default framesync (0x%x) = 0x%x \n", 
	//	rOFDM0_RxDetector3, pHalData->framesync));
}



/**
* Function:	phy_InitBBRFRegisterDefinition
*
* OverView:	Initialize Register definition offset for Radio Path A/B/C/D
*
* Input:
*			PADAPTER		Adapter,
*
* Output:	None
* Return:		None
* Note:		The initialization value is constant and it should never be changes
*/
static	VOID
phy_InitBBRFRegisterDefinition(
	IN	PADAPTER		Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);	

	// RF Interface Sowrtware Control
	pHalData->PHYRegDef[RF90_PATH_A].rfintfs = rFPGA0_XAB_RFInterfaceSW; // 16 LSBs if read 32-bit from 0x870
	pHalData->PHYRegDef[RF90_PATH_B].rfintfs = rFPGA0_XAB_RFInterfaceSW; // 16 MSBs if read 32-bit from 0x870 (16-bit for 0x872)
	pHalData->PHYRegDef[RF90_PATH_C].rfintfs = rFPGA0_XCD_RFInterfaceSW;// 16 LSBs if read 32-bit from 0x874
	pHalData->PHYRegDef[RF90_PATH_D].rfintfs = rFPGA0_XCD_RFInterfaceSW;// 16 MSBs if read 32-bit from 0x874 (16-bit for 0x876)

	// RF Interface Readback Value
	pHalData->PHYRegDef[RF90_PATH_A].rfintfi = rFPGA0_XAB_RFInterfaceRB; // 16 LSBs if read 32-bit from 0x8E0	
	pHalData->PHYRegDef[RF90_PATH_B].rfintfi = rFPGA0_XAB_RFInterfaceRB;// 16 MSBs if read 32-bit from 0x8E0 (16-bit for 0x8E2)
	pHalData->PHYRegDef[RF90_PATH_C].rfintfi = rFPGA0_XCD_RFInterfaceRB;// 16 LSBs if read 32-bit from 0x8E4
	pHalData->PHYRegDef[RF90_PATH_D].rfintfi = rFPGA0_XCD_RFInterfaceRB;// 16 MSBs if read 32-bit from 0x8E4 (16-bit for 0x8E6)

	// RF Interface Output (and Enable)
	pHalData->PHYRegDef[RF90_PATH_A].rfintfo = rFPGA0_XA_RFInterfaceOE; // 16 LSBs if read 32-bit from 0x860
	pHalData->PHYRegDef[RF90_PATH_B].rfintfo = rFPGA0_XB_RFInterfaceOE; // 16 LSBs if read 32-bit from 0x864

	// RF Interface (Output and)  Enable
	pHalData->PHYRegDef[RF90_PATH_A].rfintfe = rFPGA0_XA_RFInterfaceOE; // 16 MSBs if read 32-bit from 0x860 (16-bit for 0x862)
	pHalData->PHYRegDef[RF90_PATH_B].rfintfe = rFPGA0_XB_RFInterfaceOE; // 16 MSBs if read 32-bit from 0x864 (16-bit for 0x866)

	//Addr of LSSI. Wirte RF register by driver
	pHalData->PHYRegDef[RF90_PATH_A].rf3wireOffset = rFPGA0_XA_LSSIParameter; //LSSI Parameter
	pHalData->PHYRegDef[RF90_PATH_B].rf3wireOffset = rFPGA0_XB_LSSIParameter;

	// RF parameter
	pHalData->PHYRegDef[RF90_PATH_A].rfLSSI_Select = rFPGA0_XAB_RFParameter;  //BB Band Select
	pHalData->PHYRegDef[RF90_PATH_B].rfLSSI_Select = rFPGA0_XAB_RFParameter;
	pHalData->PHYRegDef[RF90_PATH_C].rfLSSI_Select = rFPGA0_XCD_RFParameter;
	pHalData->PHYRegDef[RF90_PATH_D].rfLSSI_Select = rFPGA0_XCD_RFParameter;

	// Tx AGC Gain Stage (same for all path. Should we remove this?)
	pHalData->PHYRegDef[RF90_PATH_A].rfTxGainStage = rFPGA0_TxGainStage; //Tx gain stage
	pHalData->PHYRegDef[RF90_PATH_B].rfTxGainStage = rFPGA0_TxGainStage; //Tx gain stage
	pHalData->PHYRegDef[RF90_PATH_C].rfTxGainStage = rFPGA0_TxGainStage; //Tx gain stage
	pHalData->PHYRegDef[RF90_PATH_D].rfTxGainStage = rFPGA0_TxGainStage; //Tx gain stage

	// Tranceiver A~D HSSI Parameter-1
	pHalData->PHYRegDef[RF90_PATH_A].rfHSSIPara1 = rFPGA0_XA_HSSIParameter1;  //wire control parameter1
	pHalData->PHYRegDef[RF90_PATH_B].rfHSSIPara1 = rFPGA0_XB_HSSIParameter1;  //wire control parameter1

	// Tranceiver A~D HSSI Parameter-2
	pHalData->PHYRegDef[RF90_PATH_A].rfHSSIPara2 = rFPGA0_XA_HSSIParameter2;  //wire control parameter2
	pHalData->PHYRegDef[RF90_PATH_B].rfHSSIPara2 = rFPGA0_XB_HSSIParameter2;  //wire control parameter2

	// RF switch Control
	pHalData->PHYRegDef[RF90_PATH_A].rfSwitchControl = rFPGA0_XAB_SwitchControl; //TR/Ant switch control
	pHalData->PHYRegDef[RF90_PATH_B].rfSwitchControl = rFPGA0_XAB_SwitchControl;
	pHalData->PHYRegDef[RF90_PATH_C].rfSwitchControl = rFPGA0_XCD_SwitchControl;
	pHalData->PHYRegDef[RF90_PATH_D].rfSwitchControl = rFPGA0_XCD_SwitchControl;

	// AGC control 1 
	pHalData->PHYRegDef[RF90_PATH_A].rfAGCControl1 = rOFDM0_XAAGCCore1;
	pHalData->PHYRegDef[RF90_PATH_B].rfAGCControl1 = rOFDM0_XBAGCCore1;
	pHalData->PHYRegDef[RF90_PATH_C].rfAGCControl1 = rOFDM0_XCAGCCore1;
	pHalData->PHYRegDef[RF90_PATH_D].rfAGCControl1 = rOFDM0_XDAGCCore1;

	// AGC control 2 
	pHalData->PHYRegDef[RF90_PATH_A].rfAGCControl2 = rOFDM0_XAAGCCore2;
	pHalData->PHYRegDef[RF90_PATH_B].rfAGCControl2 = rOFDM0_XBAGCCore2;
	pHalData->PHYRegDef[RF90_PATH_C].rfAGCControl2 = rOFDM0_XCAGCCore2;
	pHalData->PHYRegDef[RF90_PATH_D].rfAGCControl2 = rOFDM0_XDAGCCore2;

	// RX AFE control 1 
	pHalData->PHYRegDef[RF90_PATH_A].rfRxIQImbalance = rOFDM0_XARxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_B].rfRxIQImbalance = rOFDM0_XBRxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_C].rfRxIQImbalance = rOFDM0_XCRxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_D].rfRxIQImbalance = rOFDM0_XDRxIQImbalance;	

	// RX AFE control 1  
	pHalData->PHYRegDef[RF90_PATH_A].rfRxAFE = rOFDM0_XARxAFE;
	pHalData->PHYRegDef[RF90_PATH_B].rfRxAFE = rOFDM0_XBRxAFE;
	pHalData->PHYRegDef[RF90_PATH_C].rfRxAFE = rOFDM0_XCRxAFE;
	pHalData->PHYRegDef[RF90_PATH_D].rfRxAFE = rOFDM0_XDRxAFE;	

	// Tx AFE control 1 
	pHalData->PHYRegDef[RF90_PATH_A].rfTxIQImbalance = rOFDM0_XATxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_B].rfTxIQImbalance = rOFDM0_XBTxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_C].rfTxIQImbalance = rOFDM0_XCTxIQImbalance;
	pHalData->PHYRegDef[RF90_PATH_D].rfTxIQImbalance = rOFDM0_XDTxIQImbalance;	

	// Tx AFE control 2 
	pHalData->PHYRegDef[RF90_PATH_A].rfTxAFE = rOFDM0_XATxAFE;
	pHalData->PHYRegDef[RF90_PATH_B].rfTxAFE = rOFDM0_XBTxAFE;
	pHalData->PHYRegDef[RF90_PATH_C].rfTxAFE = rOFDM0_XCTxAFE;
	pHalData->PHYRegDef[RF90_PATH_D].rfTxAFE = rOFDM0_XDTxAFE;	

	// Tranceiver LSSI Readback SI mode 
	pHalData->PHYRegDef[RF90_PATH_A].rfLSSIReadBack = rFPGA0_XA_LSSIReadBack;
	pHalData->PHYRegDef[RF90_PATH_B].rfLSSIReadBack = rFPGA0_XB_LSSIReadBack;
	pHalData->PHYRegDef[RF90_PATH_C].rfLSSIReadBack = rFPGA0_XC_LSSIReadBack;
	pHalData->PHYRegDef[RF90_PATH_D].rfLSSIReadBack = rFPGA0_XD_LSSIReadBack;	

	// Tranceiver LSSI Readback PI mode 
	pHalData->PHYRegDef[RF90_PATH_A].rfLSSIReadBackPi = TransceiverA_HSPI_Readback;
	pHalData->PHYRegDef[RF90_PATH_B].rfLSSIReadBackPi = TransceiverB_HSPI_Readback;
	//pHalData->PHYRegDef[RF90_PATH_C].rfLSSIReadBackPi = rFPGA0_XC_LSSIReadBack;
	//pHalData->PHYRegDef[RF90_PATH_D].rfLSSIReadBackPi = rFPGA0_XD_LSSIReadBack;	

}

#if 0
//
//	Description: 
//		Change RF power state.
//
//	Assumption:	
//		This function must be executed in re-schdulable context, 
//		ie. PASSIVE_LEVEL.
//
//	050823, by rcnjko.
//
extern	BOOLEAN
PHY_SetRFPowerState(
	IN	PADAPTER			Adapter, 
	IN	RT_RF_POWER_STATE	eRFPowerState
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bResult = FALSE;

	RT_TRACE(COMP_RF, DBG_LOUD, ("---------> PHY_SetRFPowerState(): eRFPowerState(%d)\n", eRFPowerState));
	if(eRFPowerState == pHalData->eRFPowerState)
	{
		RT_TRACE(COMP_RF, DBG_LOUD, ("<--------- PHY_SetRFPowerState(): discard the request for eRFPowerState(%d) is the same.\n", eRFPowerState));
		return bResult;
	}
#if (DEV_BUS_TYPE == PCI_INTERFACE)
	bResult = phy_SetRFPowerState8192SE(Adapter, eRFPowerState);
#elif (DEV_BUS_TYPE == USB_INTERFACE)
	bResult = phy_SetRFPowerState8192CU(Adapter, eRFPowerState);
#endif
		
	RT_TRACE(COMP_RF, DBG_LOUD, ("<--------- PHY_SetRFPowerState(): bResult(%d)\n", bResult));

	return bResult;
}


#if (DEV_BUS_TYPE == PCI_INTERFACE)
/*-----------------------------------------------------------------------------
 * Function:	PHY_SetRtl8192seRfHalt()
 *
 * Overview:	For different power save scheme. Reboot/Halt(S3/S4)/SW radio/
 *				SW radio/IPS will call the scheme.
 *
 * Input:		IN	PADAPTER	pAdapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	03/27/2009	MHC		Merge from Macro SET_RTL8192SE_RF_HALT
 *						Because we need to send some parameter to the funtion.
 *						Macro is hard to maintain larger code.
 *
 *---------------------------------------------------------------------------*/
extern	void	
PHY_SetRtl8192seRfHalt(		IN	PADAPTER	pAdapter)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);	
	PMGNT_INFO		pMgntInfo = &(pAdapter->MgntInfo);
	u1Byte			u1bTmp;
	
	
}	// PHY_SetRtl8192seRfHalt


//
// Description:
//	Set the RF power state for 8192SE.
// Note:
//	This function may turn off the radio like halting the adapter, so when the eRFPowerState is eRFOn it may
//	mean that the NIC should be re-initialized.
// By Bruce, 2008-12-25.
//
BOOLEAN
phy_SetRFPowerState8192SE(
	IN	PADAPTER			Adapter,
	IN	RT_RF_POWER_STATE	eRFPowerState
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	BOOLEAN			bResult = TRUE;
	u1Byte			i, QueueID;
	PRT_POWER_SAVE_CONTROL	pPSC = GET_POWER_SAVE_CONTROL(pMgntInfo);
	
	pHalData->SetRFPowerStateInProgress = TRUE;
	
	switch(pHalData->RFChipID )
	{
		default:	// RL6052 for 8192S now!!!
		switch( eRFPowerState )
		{
			//
			// SW radio on/IPS site survey call will execute all flow
			// HW radio on
			//
			case eRfOn:
				{
				#if(MUTUAL_AUTHENTICATION == 1)
					if(pHalData->MutualAuthenticationFail)
						break;
				#endif
					if((pHalData->eRFPowerState == eRfOff) && RT_IN_PS_LEVEL(Adapter, RT_RF_OFF_LEVL_HALT_NIC))
					{ // The current RF state is OFF and the RF OFF level is halting the NIC, re-initialize the NIC.
						RT_STATUS rtstatus;
						u4Byte InitializeCount = 0;
						do
						{	
							InitializeCount++;
							rtstatus = NicIFEnableNIC( Adapter );
						}while( (rtstatus != RT_STATUS_SUCCESS) &&(InitializeCount <10) );
						RT_ASSERT(rtstatus == RT_STATUS_SUCCESS,("Nic Initialize Fail\n"));
						RT_CLEAR_PS_LEVEL(Adapter, RT_RF_OFF_LEVL_HALT_NIC);
					}
					else
					{ // This is the normal case, we just turn on the RF.
//						PlatformEFIOWrite2Byte(Adapter, CMDR, 0x37FC);
//						PlatformEFIOWrite1Byte(Adapter, TXPAUSE, 0x00);
//						PlatformEFIOWrite1Byte(Adapter, PHY_CCA, 0x3);
						phy_SetRTL8192CERfOn(Adapter);
					}
			
					// Turn on RF we are still linked, which might happen when 
					// we quickly turn off and on HW RF. 2006.05.12, by rcnjko.
					if( pMgntInfo->bMediaConnect == TRUE )
					{
						Adapter->ledpriv.LedControlHandler(Adapter, LED_CTL_LINK); 
					}
					else
					{
						// Turn off LED if RF is not ON.
						Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_NO_LINK); 
					}
				}
				break;
			// 
			// Card Disable/SW radio off/HW radio off/IPS enter call
			//
			case eRfOff:
				{					
				// Make sure BusyQueue is empty befor turn off RFE pwoer.
				for(QueueID = 0, i = 0; QueueID < MAX_TX_QUEUE; )
				{
					if(RTIsListEmpty(&Adapter->TcbBusyQueue[QueueID]))
					{
						//DbgPrint("QueueID = %d", QueueID);
						QueueID++;
						continue;
					}
					#if( DEV_BUS_TYPE==PCI_INTERFACE)
					else if(IsLowPowerState(Adapter))
					{
							RT_TRACE(COMP_POWER, DBG_LOUD, 
							("eRf Off/Sleep: %d times TcbBusyQueue[%d] !=0 but lower power state!\n", (i+1), QueueID));
						break;
					}
					#endif
					else
					{
							RT_TRACE(COMP_POWER, DBG_LOUD, 
							("eRf Off/Sleep: %d times TcbBusyQueue[%d] !=0 before doze!\n", (i+1), QueueID));
							PlatformStallExecution(10);
						i++;
					}
					
					if(i >= MAX_DOZE_WAITING_TIMES_9x)
					{
						RT_TRACE(COMP_POWER, DBG_WARNING, ("\n\n\n SetZebraRFPowerState8185B(): eRfOff: %d times TcbBusyQueue[%d] != 0 !!!\n\n\n", MAX_DOZE_WAITING_TIMES_9x, QueueID));
						break;
					}
				}			

					if(pPSC->RegRfPsLevel & RT_RF_OFF_LEVL_HALT_NIC)
					{ // Disable all components.
						// why not just call haltadapter92se ?? neo 2009 1,3
						//PHY_SetRtl8192seRfHalt(Adapter);
						NicIFDisableNIC(Adapter);
						RT_SET_PS_LEVEL(Adapter, RT_RF_OFF_LEVL_HALT_NIC);
						if(pHalData->pwrdown && pMgntInfo->RfOffReason>= RF_CHANGE_BY_HW)
							PlatformEFIOWrite1Byte(Adapter,0x03,0x31);
					} 
					else
					{ // Normal case.
						//SET_RTL8192SE_RF_SLEEP(Adapter);
		
						//
						//If Rf off reason is from IPS, Led should blink with no link, by Maddest 071015
						//
						if(pMgntInfo->RfOffReason==RF_CHANGE_BY_IPS )
						{
							Adapter->HalFunc.LedControlHandler(Adapter,LED_CTL_NO_LINK); 
						}
						else
						{
							// Turn off LED if RF is not ON.
							Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_POWER_OFF); 
						}
					}
				}
					break;
				
			case eRfSleep:
				{
					// HW setting had been configured with deeper mode.
					if(pHalData->eRFPowerState == eRfOff)
						break;
					
					// Make sure BusyQueue is empty befor turn off RFE pwoer.
					for(QueueID = 0, i = 0; QueueID < MAX_TX_QUEUE; )
					{
						if(RTIsListEmpty(&Adapter->TcbBusyQueue[QueueID]))
						{
							QueueID++;
							continue;
						}
#if( DEV_BUS_TYPE==PCI_INTERFACE)
						else if(IsLowPowerState(Adapter))
						{
							RT_TRACE(COMP_POWER, DBG_LOUD, ("eRf Off/Sleep: %d times TcbBusyQueue[%d] !=0 but lower power state!\n", (i+1), QueueID));
							break;
						}
#endif
					else
					{
						RT_TRACE(COMP_POWER, DBG_LOUD, ("eRf Off/Sleep: %d times TcbBusyQueue[%d] !=0 before doze!\n", (i+1), QueueID));
						PlatformStallExecution(10);
						i++;
					}
						
						if(i >= MAX_DOZE_WAITING_TIMES_9x)
						{
							RT_TRACE(COMP_POWER, DBG_WARNING, ("\n\n\n SetZebraRFPowerState8185B(): eRfOff: %d times TcbBusyQueue[%d] != 0 !!!\n\n\n", MAX_DOZE_WAITING_TIMES_9x, QueueID));
							break;
						}
					}		

					phy_SetRTL8192CERfSleep(Adapter);
				}
				break;
				
				default:
				bResult = FALSE;
				RT_ASSERT(FALSE, ("phy_SetRFPowerState8192S(): unknow state to set: 0x%X!!!\n", eRFPowerState));
					break;
		} 
				break;
	}

	if(bResult)
	{
		// Update current RF state variable.
		pHalData->eRFPowerState = eRFPowerState;
	}
	
	pHalData->SetRFPowerStateInProgress = FALSE;

	return bResult;
}


/*-----------------------------------------------------------------------------
 * Function:	PHY_SwitchEphyParameter()
 *
 * Overview:	To prevent ASPM error. We need to change some EPHY parameter to 
 *			replace HW autoload value..
 *
 * Input:		IN	PADAPTER			pAdapter
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	12/26/2008	MHC		Create. The flow is refered to DD PCIE.
 *
 *---------------------------------------------------------------------------*/
VOID
PHY_SwitchEphyParameter(
	IN	PADAPTER			Adapter
	)
{
	HAL_DATA_TYPE		*pHalData	= GET_HAL_DATA(Adapter);

	
}	// PHY_SwitchEphyParameter



#endif

#if (DEV_BUS_TYPE == USB_INTERFACE)
//
// Description:
//	Set the RF power state for 8192CU.
// Note:
//	This function may turn off the radio like halting the adapter, so when the eRFPowerState is eRFOn it may
//	mean that the NIC should be re-initialized.
// By Bruce, 2008-12-25.
//

BOOLEAN
phy_SetRFPowerState8192CU(
	IN	PADAPTER			Adapter,
	IN	RT_RF_POWER_STATE	eRFPowerState
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	BOOLEAN			bResult = TRUE;
	u1Byte			i, QueueID;
	u1Byte			u1bTmp;
	
	if(pHalData->SetRFPowerStateInProgress == TRUE)
		return FALSE;
	
	pHalData->SetRFPowerStateInProgress = TRUE;
	RT_TRACE(COMP_INIT, DBG_LOUD, ("======> phy_SetRFPowerState8192CU .\n"));
	switch(pHalData->RFChipID )
	{
		default:
		switch( eRFPowerState )
		{
			case eRfOn:
				RT_TRACE(COMP_INIT, DBG_LOUD, ("======> phy_SetRFPowerState8192CU-eRfOn .\n"));			
				NicIFEnableNIC(Adapter);
				break;
    	    
			// 
			// In current solution, RFSleep=RFOff in order to save power under 802.11 power save.
			// By Bruce, 2008-01-16.
			//
			case eRfSleep:
			case eRfOff:
				// HW setting had been configured.
				// Both of these RF configures are the same, configuring twice may cause HW abnormal.
				if(pHalData->eRFPowerState == eRfSleep || pHalData->eRFPowerState== eRfOff)
					break;
				
				// Make sure BusyQueue is empty befor turn off RFE pwoer.
				for(QueueID = 0, i = 0; QueueID < MAX_TX_QUEUE; )
				{
					if(RTIsListEmpty(&Adapter->TcbBusyQueue[QueueID]))
					{
						//DbgPrint("QueueID = %d", QueueID);
						QueueID++;
						continue;
					}					
					else
					{
						RT_TRACE(COMP_POWER, DBG_LOUD, ("eRf Off/Sleep: %d times TcbBusyQueue[%d] !=0 before doze!\n", (i+1), QueueID));
						PlatformSleepUs(10);
						i++;
					}
					
					if(i >= MAX_DOZE_WAITING_TIMES_9x)
					{
						RT_TRACE(COMP_POWER, DBG_WARNING, ("\n\n\n SetZebraRFPowerState8185B(): eRfOff: %d times TcbBusyQueue[%d] != 0 !!!\n\n\n", MAX_DOZE_WAITING_TIMES_9x, QueueID));
						break;
					}
				}				

				// 
				//RF Off/Sleep sequence. Designed/tested from SD4 Scott, SD1 Grent and Jonbon.
				// Added by 
				//
				//==================================================================

				RT_TRACE(COMP_INIT, DBG_LOUD, ("======> CardDisableWithoutHWSM -eRfOff.\n"));				
				CardDisableWithoutHWSM( Adapter);
	
				break;

			default:
				bResult = FALSE;
				RT_ASSERT(FALSE, ("phy_SetRFPowerState8192S(): unknow state to set: 0x%X!!!\n", eRFPowerState));
				break;
		} 
		break;
	}
			
	if(bResult)
	{
		// Update current RF state variable.
		pHalData->eRFPowerState = eRFPowerState;
		
		switch(pHalData->RFChipID )
		{
			default:		
			switch(pHalData->eRFPowerState)
			{
				case eRfOff:
					//
					//If Rf off reason is from IPS, Led should blink with no link, by Maddest 071015
					//
					if(pMgntInfo->RfOffReason==RF_CHANGE_BY_IPS )
					{
						Adapter->HalFunc.LedControlHandler(Adapter,LED_CTL_NO_LINK); 
					}
					else
					{
						// Turn off LED if RF is not ON.
						Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_POWER_OFF); 
					}
					break;
        		
				case eRfOn:
					// Turn on RF we are still linked, which might happen when 
					// we quickly turn off and on HW RF. 2006.05.12, by rcnjko.
					if( pMgntInfo->bMediaConnect == TRUE )
					{
						Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_LINK); 
					}
					else
					{
						// Turn off LED if RF is not ON.
						Adapter->HalFunc.LedControlHandler(Adapter, LED_CTL_NO_LINK); 
					}
					break;
        		
				default:
					// do nothing.
					break;
			}// Switch RF state

				break;
		}// Switch RFChipID
	}
	
	pHalData->SetRFPowerStateInProgress = FALSE;

	return bResult;
}
#endif
#endif

/*-----------------------------------------------------------------------------
 * Function:    GetTxPowerLevel8190()
 *
 * Overview:    This function is export to "common" moudule
 *
 * Input:       PADAPTER		Adapter
 *			psByte			Power Level
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 *---------------------------------------------------------------------------*/
extern	VOID
PHY_GetTxPowerLevel8192C(
	IN	PADAPTER		Adapter,
	OUT u32*    		powerlevel
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8			TxPwrLevel = 0;
	int			TxPwrDbm;
	
	//
	// Because the Tx power indexes are different, we report the maximum of them to 
	// meet the CCX TPC request. By Bruce, 2008-01-31.
	//

	// CCK
	TxPwrLevel = pHalData->CurrentCckTxPwrIdx;
	TxPwrDbm = phy_TxPwrIdxToDbm(Adapter, WIRELESS_MODE_B, TxPwrLevel);

	// Legacy OFDM
	TxPwrLevel = pHalData->CurrentOfdm24GTxPwrIdx + pHalData->LegacyHTTxPowerDiff;

	// Compare with Legacy OFDM Tx power.
	if(phy_TxPwrIdxToDbm(Adapter, WIRELESS_MODE_G, TxPwrLevel) > TxPwrDbm)
		TxPwrDbm = phy_TxPwrIdxToDbm(Adapter, WIRELESS_MODE_G, TxPwrLevel);

	// HT OFDM
	TxPwrLevel = pHalData->CurrentOfdm24GTxPwrIdx;
	
	// Compare with HT OFDM Tx power.
	if(phy_TxPwrIdxToDbm(Adapter, WIRELESS_MODE_N_24G, TxPwrLevel) > TxPwrDbm)
		TxPwrDbm = phy_TxPwrIdxToDbm(Adapter, WIRELESS_MODE_N_24G, TxPwrLevel);

	*powerlevel = TxPwrDbm;
}


void getTxPowerIndex(
	IN	PADAPTER		Adapter,
	IN	u8			channel,
	IN OUT u8*		cckPowerLevel,
	IN OUT u8*		ofdmPowerLevel
	)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	//HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u8				index = (channel -1);
	u8				i;
	// 1. CCK
	cckPowerLevel[RF90_PATH_A] = pEEPROM->TxPwrLevelCck[RF90_PATH_A][index];	//RF-A
	cckPowerLevel[RF90_PATH_B] = pEEPROM->TxPwrLevelCck[RF90_PATH_B][index];	//RF-B

	// 2. OFDM for 1S or 2S
	if (GET_RF_TYPE(Adapter) == RF_1T2R || GET_RF_TYPE(Adapter) == RF_1T1R)
	{
		// Read HT 40 OFDM TX power
		ofdmPowerLevel[RF90_PATH_A] = pEEPROM->TxPwrLevelHT40_1S[RF90_PATH_A][index];
		ofdmPowerLevel[RF90_PATH_B] = pEEPROM->TxPwrLevelHT40_1S[RF90_PATH_B][index];
	}
	else if (GET_RF_TYPE(Adapter) == RF_2T2R)
	{
		// Read HT 40 OFDM TX power
		ofdmPowerLevel[RF90_PATH_A] = pEEPROM->TxPwrLevelHT40_2S[RF90_PATH_A][index];
		ofdmPowerLevel[RF90_PATH_B] = pEEPROM->TxPwrLevelHT40_2S[RF90_PATH_B][index];
	}
	//RTPRINT(FPHY, PHY_TXPWR, ("Channel-%d, set tx power index !!\n", channel));
}

void ccxPowerIndexCheck(
	IN	PADAPTER		Adapter,
	IN	u8			channel,
	IN OUT u8*		cckPowerLevel,
	IN OUT u8*		ofdmPowerLevel
	)
{
#if 0
	PMGNT_INFO			pMgntInfo = &(Adapter->MgntInfo);
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PRT_CCX_INFO		pCcxInfo = GET_CCX_INFO(pMgntInfo);

	//
	// CCX 2 S31, AP control of client transmit power:
	// 1. We shall not exceed Cell Power Limit as possible as we can.
	// 2. Tolerance is +/- 5dB.
	// 3. 802.11h Power Contraint takes higher precedence over CCX Cell Power Limit.
	// 
	// TODO: 
	// 1. 802.11h power contraint 
	//
	// 071011, by rcnjko.
	//
	if(	pMgntInfo->OpMode == RT_OP_MODE_INFRASTRUCTURE && 
		pMgntInfo->mAssoc &&
		pCcxInfo->bUpdateCcxPwr &&
		pCcxInfo->bWithCcxCellPwr &&
		channel == pMgntInfo->dot11CurrentChannelNumber)
	{
		u1Byte	CckCellPwrIdx = phy_DbmToTxPwrIdx(Adapter, WIRELESS_MODE_B, pCcxInfo->CcxCellPwr);
		u1Byte	LegacyOfdmCellPwrIdx = phy_DbmToTxPwrIdx(Adapter, WIRELESS_MODE_G, pCcxInfo->CcxCellPwr);
		u1Byte	OfdmCellPwrIdx = phy_DbmToTxPwrIdx(Adapter, WIRELESS_MODE_N_24G, pCcxInfo->CcxCellPwr);

		RT_TRACE(COMP_TXAGC, DBG_LOUD, 
		("CCX Cell Limit: %d dbm => CCK Tx power index : %d, Legacy OFDM Tx power index : %d, OFDM Tx power index: %d\n", 
		pCcxInfo->CcxCellPwr, CckCellPwrIdx, LegacyOfdmCellPwrIdx, OfdmCellPwrIdx));
		RT_TRACE(COMP_TXAGC, DBG_LOUD, 
		("EEPROM channel(%d) => CCK Tx power index: %d, Legacy OFDM Tx power index : %d, OFDM Tx power index: %d\n",
		channel, cckPowerLevel[0], ofdmPowerLevel[0] + pHalData->LegacyHTTxPowerDiff, ofdmPowerLevel[0])); 

		// CCK
		if(cckPowerLevel[0] > CckCellPwrIdx)
			cckPowerLevel[0] = CckCellPwrIdx;
		// Legacy OFDM, HT OFDM
		if(ofdmPowerLevel[0] + pHalData->LegacyHTTxPowerDiff > LegacyOfdmCellPwrIdx)
		{
			if((OfdmCellPwrIdx - pHalData->LegacyHTTxPowerDiff) > 0)
			{
				ofdmPowerLevel[0] = OfdmCellPwrIdx - pHalData->LegacyHTTxPowerDiff;
			}
			else
			{
				ofdmPowerLevel[0] = 0;
			}
		}

		RT_TRACE(COMP_TXAGC, DBG_LOUD, 
		("Altered CCK Tx power index : %d, Legacy OFDM Tx power index: %d, OFDM Tx power index: %d\n", 
		cckPowerLevel[0], ofdmPowerLevel[0] + pHalData->LegacyHTTxPowerDiff, ofdmPowerLevel[0]));
	}

	pHalData->CurrentCckTxPwrIdx = cckPowerLevel[0];
	pHalData->CurrentOfdm24GTxPwrIdx = ofdmPowerLevel[0];

	RT_TRACE(COMP_TXAGC, DBG_LOUD, 
		("PHY_SetTxPowerLevel8192S(): CCK Tx power index : %d, Legacy OFDM Tx power index: %d, OFDM Tx power index: %d\n", 
		cckPowerLevel[0], ofdmPowerLevel[0] + pHalData->LegacyHTTxPowerDiff, ofdmPowerLevel[0]));
#endif	
}
/*-----------------------------------------------------------------------------
 * Function:    SetTxPowerLevel8190()
 *
 * Overview:    This function is export to "HalCommon" moudule
 *			We must consider RF path later!!!!!!!
 *
 * Input:       PADAPTER		Adapter
 *			u1Byte		channel
 *
 * Output:      NONE
 *
 * Return:      NONE
 *	2008/11/04	MHC		We remove EEPROM_93C56.
 *						We need to move CCX relative code to independet file.
 *	2009/01/21	MHC		Support new EEPROM format from SD3 requirement.
 *
 *---------------------------------------------------------------------------*/
extern	VOID
PHY_SetTxPowerLevel8192C(
	IN	PADAPTER		Adapter,
	IN	u8			channel
	)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u8	cckPowerLevel[2], ofdmPowerLevel[2];	// [0]:RF-A, [1]:RF-B

#if(MP_DRIVER == 1)
	return;
#endif

	//if(pHalData->bTXPowerDataReadFromEEPORM == _FALSE)
	//	return;

	getTxPowerIndex(Adapter, channel, &cckPowerLevel[0], &ofdmPowerLevel[0]);
	//RTPRINT(FPHY, PHY_TXPWR, ("Channel-%d, cckPowerLevel (A / B) = 0x%x / 0x%x,   ofdmPowerLevel (A / B) = 0x%x / 0x%x\n", 
	//	channel, cckPowerLevel[0], cckPowerLevel[1], ofdmPowerLevel[0], ofdmPowerLevel[1]));

	ccxPowerIndexCheck(Adapter, channel, &cckPowerLevel[0], &ofdmPowerLevel[0]);

	PHY_RF6052SetCckTxPower(Adapter, &cckPowerLevel[0]);
	PHY_RF6052SetOFDMTxPower(Adapter, &ofdmPowerLevel[0], channel);

#if 0
	switch(pHalData->rf_chip)
	{
		case RF_8225:
			PHY_SetRF8225CckTxPower(Adapter, cckPowerLevel[0]);
			PHY_SetRF8225OfdmTxPower(Adapter, ofdmPowerLevel[0]);
		break;

		case RF_8256:
			PHY_SetRF8256CCKTxPower(Adapter, cckPowerLevel[0]);
			PHY_SetRF8256OFDMTxPower(Adapter, ofdmPowerLevel[0]);
			break;

		case RF_6052:
			PHY_RF6052SetCckTxPower(Adapter, &cckPowerLevel[0]);
			PHY_RF6052SetOFDMTxPower(Adapter, &ofdmPowerLevel[0], channel);
			break;

		case RF_8258:
			break;
	}
#endif

}


//
//	Description:
//		Update transmit power level of all channel supported.
//
//	TODO: 
//		A mode.
//	By Bruce, 2008-02-04.
//
extern	BOOLEAN
PHY_UpdateTxPowerDbm8192C(
	IN	PADAPTER	Adapter,
	IN	int		powerInDbm
	)
{
	EEPROM_EFUSE_PRIV *pEEPROM = GET_EEPROM_EFUSE_PRIV(Adapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8				idx;
	u8			rf_path;

	// TODO: A mode Tx power.
	u8	CckTxPwrIdx = phy_DbmToTxPwrIdx(Adapter, WIRELESS_MODE_B, powerInDbm);
	u8	OfdmTxPwrIdx = phy_DbmToTxPwrIdx(Adapter, WIRELESS_MODE_N_24G, powerInDbm);

	if(OfdmTxPwrIdx - pHalData->LegacyHTTxPowerDiff > 0)
		OfdmTxPwrIdx -= pHalData->LegacyHTTxPowerDiff;
	else
		OfdmTxPwrIdx = 0;

	//RT_TRACE(COMP_TXAGC, DBG_LOUD, ("PHY_UpdateTxPowerDbm8192S(): %ld dBm , CckTxPwrIdx = %d, OfdmTxPwrIdx = %d\n", powerInDbm, CckTxPwrIdx, OfdmTxPwrIdx));

	for(idx = 0; idx < 14; idx++)
	{
		for (rf_path = 0; rf_path < 2; rf_path++)
		{
			pEEPROM->TxPwrLevelCck[rf_path][idx] = CckTxPwrIdx;
			pEEPROM->TxPwrLevelHT40_1S[rf_path][idx] = 
			pEEPROM->TxPwrLevelHT40_2S[rf_path][idx] = OfdmTxPwrIdx;
		}
	}

	//Adapter->HalFunc.SetTxPowerLevelHandler(Adapter, pHalData->CurrentChannel);//gtest:todo

	return _TRUE;	
}


/*
	Description:
		When beacon interval is changed, the values of the 
		hw registers should be modified.
	By tynli, 2008.10.24.

*/


extern void	
PHY_SetBeaconHwReg(	
	IN	PADAPTER		Adapter,
	IN	u16			BeaconInterval	
	)
{

}

//
//	Description:
//		Map dBm into Tx power index according to 
//		current HW model, for example, RF and PA, and
//		current wireless mode.
//	By Bruce, 2008-01-29.
//
static	u8
phy_DbmToTxPwrIdx(
	IN	PADAPTER		Adapter,
	IN	WIRELESS_MODE	WirelessMode,
	IN	int			PowerInDbm
	)
{
	u8				TxPwrIdx = 0;
	int				Offset = 0;
	

	//
	// Tested by MP, we found that CCK Index 0 equals to 8dbm, OFDM legacy equals to 
	// 3dbm, and OFDM HT equals to 0dbm repectively.
	// Note:
	//	The mapping may be different by different NICs. Do not use this formula for what needs accurate result.  
	// By Bruce, 2008-01-29.
	// 
	switch(WirelessMode)
	{
	case WIRELESS_MODE_B:
		Offset = -7;
		break;

	case WIRELESS_MODE_G:
	case WIRELESS_MODE_N_24G:
		Offset = -8;
		break;
	default:
		Offset = -8;
		break;
	}

	if((PowerInDbm - Offset) > 0)
	{
		TxPwrIdx = (u8)((PowerInDbm - Offset) * 2);
	}
	else
	{
		TxPwrIdx = 0;
	}

	// Tx Power Index is too large.
	if(TxPwrIdx > MAX_TXPWR_IDX_NMODE_92S)
		TxPwrIdx = MAX_TXPWR_IDX_NMODE_92S;

	return TxPwrIdx;
}

//
//	Description:
//		Map Tx power index into dBm according to 
//		current HW model, for example, RF and PA, and
//		current wireless mode.
//	By Bruce, 2008-01-29.
//
int
phy_TxPwrIdxToDbm(
	IN	PADAPTER		Adapter,
	IN	WIRELESS_MODE	WirelessMode,
	IN	u8			TxPwrIdx
	)
{
	int				Offset = 0;
	int				PwrOutDbm = 0;
	
	//
	// Tested by MP, we found that CCK Index 0 equals to -7dbm, OFDM legacy equals to -8dbm.
	// Note:
	//	The mapping may be different by different NICs. Do not use this formula for what needs accurate result.  
	// By Bruce, 2008-01-29.
	// 
	switch(WirelessMode)
	{
	case WIRELESS_MODE_B:
		Offset = -7;
		break;

	case WIRELESS_MODE_G:
	case WIRELESS_MODE_N_24G:
		Offset = -8;
	default:
		Offset = -8;	
		break;
	}

	PwrOutDbm = TxPwrIdx / 2 + Offset; // Discard the decimal part.

	return PwrOutDbm;
}


extern	VOID 
PHY_ScanOperationBackup8192C(
	IN	PADAPTER	Adapter,
	IN	u8		Operation
	)
{
#if 0
	IO_TYPE	IoType;
	
	if(!Adapter->bDriverStopped)
	{
		switch(Operation)
		{
			case SCAN_OPT_BACKUP:
				IoType = IO_CMD_PAUSE_DM_BY_SCAN;
				Adapter->HalFunc.SetHwRegHandler(Adapter,HW_VAR_IO_CMD,  (pu1Byte)&IoType);

				break;

			case SCAN_OPT_RESTORE:
				IoType = IO_CMD_RESUME_DM_BY_SCAN;
				Adapter->HalFunc.SetHwRegHandler(Adapter,HW_VAR_IO_CMD,  (pu1Byte)&IoType);
				break;

			default:
				RT_TRACE(COMP_SCAN, DBG_LOUD, ("Unknown Scan Backup Operation. \n"));
				break;
		}
	}
#endif	
}

/*-----------------------------------------------------------------------------
 * Function:    PHY_SetBWModeCallback8192C()
 *
 * Overview:    Timer callback function for SetSetBWMode
 *
 * Input:       	PRT_TIMER		pTimer
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		(1) We do not take j mode into consideration now
 *			(2) Will two workitem of "switch channel" and "switch channel bandwidth" run
 *			     concurrently?
 *---------------------------------------------------------------------------*/
VOID
_PHY_SetBWMode92C(
	IN	PADAPTER	Adapter
)
{
//	PADAPTER			Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u8				regBwOpMode;
	u8				regRRSR_RSC;

	//return;

	// Added it for 20/40 mhz switch time evaluation by guangan 070531
	//u4Byte				NowL, NowH;
	//u8Byte				BeginTime, EndTime; 

	/*RT_TRACE(COMP_SCAN, DBG_LOUD, ("==>PHY_SetBWModeCallback8192C()  Switch to %s bandwidth\n", \
					pHalData->CurrentChannelBW == HT_CHANNEL_WIDTH_20?"20MHz":"40MHz"))*/

	if(pHalData->rf_chip == RF_PSEUDO_11N)
	{
		//pHalData->SetBWModeInProgress= _FALSE;
		return;
	}

	// There is no 40MHz mode in RF_8225.
	if(pHalData->rf_chip==RF_8225)
		return;

	if(Adapter->bDriverStopped)
		return;

	// Added it for 20/40 mhz switch time evaluation by guangan 070531
	//NowL = PlatformEFIORead4Byte(Adapter, TSFR);
	//NowH = PlatformEFIORead4Byte(Adapter, TSFR+4);
	//BeginTime = ((u8Byte)NowH << 32) + NowL;
		
	//3//
	//3//<1>Set MAC register
	//3//
	//Adapter->HalFunc.SetBWModeHandler();
	
	regBwOpMode = read8(Adapter, REG_BWOPMODE);
	regRRSR_RSC = read8(Adapter, REG_RRSR+2);
	//regBwOpMode = Adapter->HalFunc.GetHwRegHandler(Adapter,HW_VAR_BWMODE,(pu1Byte)&regBwOpMode);
	
	switch(pHalData->CurrentChannelBW)
	{
		case HT_CHANNEL_WIDTH_20:
			regBwOpMode |= BW_OPMODE_20MHZ;
			   // 2007/02/07 Mark by Emily becasue we have not verify whether this register works
			write8(Adapter, REG_BWOPMODE, regBwOpMode);
			break;
			   
		case HT_CHANNEL_WIDTH_40:
			regBwOpMode &= ~BW_OPMODE_20MHZ;
				// 2007/02/07 Mark by Emily becasue we have not verify whether this register works
			write8(Adapter, REG_BWOPMODE, regBwOpMode);

			regRRSR_RSC = (regRRSR_RSC&0x90) |(pHalData->nCur40MhzPrimeSC<<5);
			write8(Adapter, REG_RRSR+2, regRRSR_RSC);
			break;

		default:
			/*RT_TRACE(COMP_DBG, DBG_LOUD, ("PHY_SetBWModeCallback8192C():
						unknown Bandwidth: %#X\n",pHalData->CurrentChannelBW));*/
			break;
	}
	
	//3//
	//3//<2>Set PHY related register
	//3//
	switch(pHalData->CurrentChannelBW)
	{
		/* 20 MHz channel*/
		case HT_CHANNEL_WIDTH_20:
			PHY_SetBBReg(Adapter, rFPGA0_RFMOD, bRFMOD, 0x0);
			PHY_SetBBReg(Adapter, rFPGA1_RFMOD, bRFMOD, 0x0);
			PHY_SetBBReg(Adapter, rFPGA0_AnalogParameter2, BIT10, 1);
			
			break;


		/* 40 MHz channel*/
		case HT_CHANNEL_WIDTH_40:
			PHY_SetBBReg(Adapter, rFPGA0_RFMOD, bRFMOD, 0x1);
			PHY_SetBBReg(Adapter, rFPGA1_RFMOD, bRFMOD, 0x1);
			
			// Set Control channel to upper or lower. These settings are required only for 40MHz
			PHY_SetBBReg(Adapter, rCCK0_System, bCCKSideBand, (pHalData->nCur40MhzPrimeSC>>1));
			PHY_SetBBReg(Adapter, rOFDM1_LSTF, 0xC00, pHalData->nCur40MhzPrimeSC);
			PHY_SetBBReg(Adapter, rFPGA0_AnalogParameter2, BIT10, 0);

			PHY_SetBBReg(Adapter, 0x818, (BIT26|BIT27), (pHalData->nCur40MhzPrimeSC==HAL_PRIME_CHNL_OFFSET_LOWER)?2:1);
			
			break;


			
		default:
			/*RT_TRACE(COMP_DBG, DBG_LOUD, ("PHY_SetBWModeCallback8192C(): unknown Bandwidth: %#X\n"\
						,pHalData->CurrentChannelBW));*/
			break;
			
	}
	//Skip over setting of J-mode in BB register here. Default value is "None J mode". Emily 20070315

	// Added it for 20/40 mhz switch time evaluation by guangan 070531
	//NowL = PlatformEFIORead4Byte(Adapter, TSFR);
	//NowH = PlatformEFIORead4Byte(Adapter, TSFR+4);
	//EndTime = ((u8Byte)NowH << 32) + NowL;
	//RT_TRACE(COMP_SCAN, DBG_LOUD, ("SetBWModeCallback8190Pci: time of SetBWMode = %I64d us!\n", (EndTime - BeginTime)));

	//3<3>Set RF related register
	switch(pHalData->rf_chip)
	{
		case RF_8225:		
			//PHY_SetRF8225Bandwidth(Adapter, pHalData->CurrentChannelBW);
			break;	
			
		case RF_8256:
			// Please implement this function in Hal8190PciPhy8256.c
			//PHY_SetRF8256Bandwidth(Adapter, pHalData->CurrentChannelBW);
			break;
			
		case RF_8258:
			// Please implement this function in Hal8190PciPhy8258.c
			// PHY_SetRF8258Bandwidth();
			break;

		case RF_PSEUDO_11N:
			// Do Nothing
			break;
			
		case RF_6052:
			PHY_RF6052SetBandwidth(Adapter, pHalData->CurrentChannelBW);
			break;	
			
		default:
			//RT_ASSERT(FALSE, ("Unknown RFChipID: %d\n", pHalData->RFChipID));
			break;
	}

	//pHalData->SetBWModeInProgress= FALSE;

	//RT_TRACE(COMP_SCAN, DBG_LOUD, ("<==PHY_SetBWModeCallback8192C() \n" ));
}

#if 0
extern	VOID
PHY_SetBWModeCallback8192C(
	IN	PRT_TIMER		pTimer
)
{
	PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;

	_PHY_SetBWMode92C(Adapter);
}
#endif

 /*-----------------------------------------------------------------------------
 * Function:   SetBWMode8190Pci()
 *
 * Overview:  This function is export to "HalCommon" moudule
 *
 * Input:       	PADAPTER			Adapter
 *			HT_CHANNEL_WIDTH	Bandwidth	//20M or 40M
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		We do not take j mode into consideration now
 *---------------------------------------------------------------------------*/
extern	VOID
PHY_SetBWMode8192C(
	IN	PADAPTER					Adapter,
	IN	HT_CHANNEL_WIDTH	Bandwidth,	// 20M or 40M
	IN	unsigned char	Offset		// Upper, Lower, or Don't care
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	HT_CHANNEL_WIDTH 	tmpBW= pHalData->CurrentChannelBW;
	// Modified it for 20/40 mhz switch by guangan 070531
	//PMGNT_INFO	pMgntInfo=&Adapter->MgntInfo;

	//return;
	
	//if(pHalData->SwChnlInProgress)
//	if(pMgntInfo->bScanInProgress)
//	{
//		RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SetBWMode8192C() %s Exit because bScanInProgress!\n", 
//					Bandwidth == HT_CHANNEL_WIDTH_20?"20MHz":"40MHz"));
//		return;
//	}

//	if(pHalData->SetBWModeInProgress)
//	{
//		// Modified it for 20/40 mhz switch by guangan 070531
//		RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SetBWMode8192C() %s cancel last timer because SetBWModeInProgress!\n", 
//					Bandwidth == HT_CHANNEL_WIDTH_20?"20MHz":"40MHz"));
//		PlatformCancelTimer(Adapter, &pHalData->SetBWModeTimer);
//		//return;
//	}

	//if(pHalData->SetBWModeInProgress)
	//	return;

	//pHalData->SetBWModeInProgress= TRUE;
	
	pHalData->CurrentChannelBW = Bandwidth;

#if 0
	if(Offset==HT_EXTCHNL_OFFSET_LOWER)
		pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_UPPER;
	else if(Offset==HT_EXTCHNL_OFFSET_UPPER)
		pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_LOWER;
	else
		pHalData->nCur40MhzPrimeSC = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
#else
	pHalData->nCur40MhzPrimeSC = Offset;
#endif

	if((!Adapter->bDriverStopped) && (!Adapter->bSurpriseRemoved))
	{
#ifdef USE_WORKITEM	
		//PlatformScheduleWorkItem(&(pHalData->SetBWModeWorkItem));
#else
	#if 0
		//PlatformSetTimer(Adapter, &(pHalData->SetBWModeTimer), 0);
	#else
		_PHY_SetBWMode92C(Adapter);
	#endif
#endif		
	}
	else
	{
		//RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SetBWMode8192C() SetBWModeInProgress FALSE driver sleep or unload\n"));	
		//pHalData->SetBWModeInProgress= FALSE;	
		pHalData->CurrentChannelBW = tmpBW;
	}
	
}

#if 0
extern	VOID
PHY_SwChnlCallback8192C(
	IN	PRT_TIMER		pTimer
	)
{
	PADAPTER		pAdapter = (PADAPTER)pTimer->Adapter;
	PADAPTER		Adapter = ADJUST_TO_ADAPTIVE_ADAPTER(pAdapter, TRUE);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte			delay;
	
	RT_TRACE(COMP_SCAN, DBG_LOUD, ("==>PHY_SwChnlCallback8192C(), switch to channel\
				%d\n", pHalData->CurrentChannel));
	
	if(Adapter->bDriverStopped)
		return;
	
	if(pHalData->RFChipID == RF_PSEUDO_11N)
	{
		pHalData->SwChnlInProgress=FALSE;
		return; 								//return immediately if it is peudo-phy	
	}
	

	do{
		if(!pHalData->SwChnlInProgress)
			break;

		if(!phy_SwChnlStepByStep(Adapter, pHalData->CurrentChannel, &pHalData->SwChnlStage, &pHalData->SwChnlStep, &delay))
		{
			if(delay>0)
			{
				PlatformSetTimer(Adapter, &pHalData->SwChnlTimer, delay);
			}
			else
			continue;
		}
		else
		{
			pHalData->SwChnlInProgress=FALSE;
		}
		break;
	}while(TRUE);

	RT_TRACE(COMP_SCAN, DBG_LOUD, ("<==PHY_SwChnlCallback8192C()\n"));
}
#endif

static void _PHY_SwChnl8192C(PADAPTER Adapter, u8 channel)
{
	u8 eRFPath;
	u32 param1, param2;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	//s1. pre common command - CmdID_SetTxPowerLevel
	PHY_SetTxPowerLevel8192C(Adapter, channel);

	//s2. RF dependent command - CmdID_RF_WriteReg, param1=RF_CHNLBW, param2=channel
	param1 = RF_CHNLBW;
	param2 = channel;
	for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	{
		pHalData->RfRegChnlVal[eRFPath] = ((pHalData->RfRegChnlVal[eRFPath] & 0xfffffc00) | param2);
		PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)eRFPath, param1, bRFRegOffsetMask, pHalData->RfRegChnlVal[eRFPath]);
	}
	
	
	//s3. post common command - CmdID_End, None

}

extern	VOID
PHY_SwChnl8192C(	// Call after initialization
	IN	PADAPTER	Adapter,
	IN	u8		channel
	)
{
	//PADAPTER Adapter =  ADJUST_TO_ADAPTIVE_ADAPTER(pAdapter, _TRUE);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u8	tmpchannel = pHalData->CurrentChannel;
	BOOLEAN  bResult = _TRUE;

	if(pHalData->rf_chip == RF_PSEUDO_11N)
	{
		//pHalData->SwChnlInProgress=FALSE;
		return; 								//return immediately if it is peudo-phy	
	}
	
	//if(pHalData->SwChnlInProgress)
	//	return;

	//if(pHalData->SetBWModeInProgress)
	//	return;

	//--------------------------------------------
	switch(pHalData->CurrentWirelessMode)
	{
		case WIRELESS_MODE_A:
		case WIRELESS_MODE_N_5G:
			//RT_ASSERT((channel>14), ("WIRELESS_MODE_A but channel<=14"));		
			break;
		
		case WIRELESS_MODE_B:
			//RT_ASSERT((channel<=14), ("WIRELESS_MODE_B but channel>14"));
			break;
		
		case WIRELESS_MODE_G:
		case WIRELESS_MODE_N_24G:
			//RT_ASSERT((channel<=14), ("WIRELESS_MODE_G but channel>14"));
			break;

		default:
			//RT_ASSERT(FALSE, ("Invalid WirelessMode(%#x)!!\n", pHalData->CurrentWirelessMode));
			break;
	}
	//--------------------------------------------
	
	//pHalData->SwChnlInProgress = TRUE;
	if(channel == 0)
		channel = 1;
	
	pHalData->CurrentChannel=channel;

	//pHalData->SwChnlStage=0;
	//pHalData->SwChnlStep=0;

	if((!Adapter->bDriverStopped) && (!Adapter->bSurpriseRemoved))
	{
#ifdef USE_WORKITEM	
		//bResult = PlatformScheduleWorkItem(&(pHalData->SwChnlWorkItem));
#else
		#if 0		
		//PlatformSetTimer(Adapter, &(pHalData->SwChnlTimer), 0);
		#else
		_PHY_SwChnl8192C(Adapter, channel);
		#endif
#endif
		if(bResult)
		{
			//RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SwChnl8192C SwChnlInProgress TRUE schdule workitem done\n"));
		}
		else
		{
			//RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SwChnl8192C SwChnlInProgress FALSE schdule workitem error\n"));		
			//if(IS_HARDWARE_TYPE_8192SU(Adapter))
			//{
			//	pHalData->SwChnlInProgress = FALSE; 	
				pHalData->CurrentChannel = tmpchannel;			
			//}
		}

	}
	else
	{
		//RT_TRACE(COMP_SCAN, DBG_LOUD, ("PHY_SwChnl8192C SwChnlInProgress FALSE driver sleep or unload\n"));	
		//if(IS_HARDWARE_TYPE_8192SU(Adapter))
		//{
		//	pHalData->SwChnlInProgress = FALSE;		
			pHalData->CurrentChannel = tmpchannel;
		//}
	}
}


//
// Description:
//	Switch channel synchronously. Called by SwChnlByDelayHandler.
//
// Implemented by Bruce, 2008-02-14.
// The following procedure is operted according to SwChanlCallback8190Pci().
// However, this procedure is performed synchronously  which should be running under
// passive level.
// 
extern	VOID
PHY_SwChnlPhy8192C(	// Only called during initialize
	IN	PADAPTER	Adapter,
	IN	u8		channel
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

	//RT_TRACE(COMP_SCAN | COMP_RM, DBG_LOUD, ("==>PHY_SwChnlPhy8192S(), switch from channel %d to channel %d.\n", pHalData->CurrentChannel, channel));

	// Cannot IO.
	//if(RT_CANNOT_IO(Adapter))
	//	return;

	// Channel Switching is in progress.
	//if(pHalData->SwChnlInProgress)
	//	return;
	
	//return immediately if it is peudo-phy
	if(pHalData->rf_chip == RF_PSEUDO_11N)
	{
		//pHalData->SwChnlInProgress=FALSE;
		return;
	}
	
	//pHalData->SwChnlInProgress = TRUE;
	if( channel == 0)
		channel = 1;
	
	pHalData->CurrentChannel=channel;
	
	//pHalData->SwChnlStage = 0;
	//pHalData->SwChnlStep = 0;
	
	phy_FinishSwChnlNow(Adapter,channel);
	
	//pHalData->SwChnlInProgress = FALSE;
}


static	BOOLEAN
phy_SwChnlStepByStep(
	IN	PADAPTER	Adapter,
	IN	u8		channel,
	IN	u8		*stage,
	IN	u8		*step,
	OUT u32		*delay
	)
{
#if 0
	HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(Adapter);
	PCHANNEL_ACCESS_SETTING	pChnlAccessSetting;
	SwChnlCmd				PreCommonCmd[MAX_PRECMD_CNT];
	u4Byte					PreCommonCmdCnt;
	SwChnlCmd				PostCommonCmd[MAX_POSTCMD_CNT];
	u4Byte					PostCommonCmdCnt;
	SwChnlCmd				RfDependCmd[MAX_RFDEPENDCMD_CNT];
	u4Byte					RfDependCmdCnt;
	SwChnlCmd				*CurrentCmd;	
	u1Byte					eRFPath;	
	u4Byte					RfTXPowerCtrl;
	BOOLEAN					bAdjRfTXPowerCtrl = _FALSE;
	
	
	RT_ASSERT((Adapter != NULL), ("Adapter should not be NULL\n"));
#if(MP_DRIVER != 1)
	RT_ASSERT(IsLegalChannel(Adapter, channel), ("illegal channel: %d\n", channel));
#endif
	RT_ASSERT((pHalData != NULL), ("pHalData should not be NULL\n"));
	
	pChnlAccessSetting = &Adapter->MgntInfo.Info8185.ChannelAccessSetting;
	RT_ASSERT((pChnlAccessSetting != NULL), ("pChnlAccessSetting should not be NULL\n"));
	
	//for(eRFPath = RF90_PATH_A; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	//for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
	//{
		// <1> Fill up pre common command.
	PreCommonCmdCnt = 0;
	phy_SetSwChnlCmdArray(PreCommonCmd, PreCommonCmdCnt++, MAX_PRECMD_CNT, 
				CmdID_SetTxPowerLevel, 0, 0, 0);
	phy_SetSwChnlCmdArray(PreCommonCmd, PreCommonCmdCnt++, MAX_PRECMD_CNT, 
				CmdID_End, 0, 0, 0);
	
		// <2> Fill up post common command.
	PostCommonCmdCnt = 0;

	phy_SetSwChnlCmdArray(PostCommonCmd, PostCommonCmdCnt++, MAX_POSTCMD_CNT, 
				CmdID_End, 0, 0, 0);
	
		// <3> Fill up RF dependent command.
	RfDependCmdCnt = 0;
	switch( pHalData->RFChipID )
	{
		case RF_8225:		
		RT_ASSERT((channel >= 1 && channel <= 14), ("illegal channel for Zebra: %d\n", channel));
		// 2008/09/04 MH Change channel. 
		if(channel==14) channel++;
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
			CmdID_RF_WriteReg, rZebra1_Channel, (0x10+channel-1), 10);
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
		CmdID_End, 0, 0, 0);
		break;	
		
	case RF_8256:
		// TEST!! This is not the table for 8256!!
		RT_ASSERT((channel >= 1 && channel <= 14), ("illegal channel for Zebra: %d\n", channel));
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
			CmdID_RF_WriteReg, rRfChannel, channel, 10);
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
		CmdID_End, 0, 0, 0);
		break;
		
	case RF_6052:
		RT_ASSERT((channel >= 1 && channel <= 14), ("illegal channel for Zebra: %d\n", channel));
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
			CmdID_RF_WriteReg, RF_CHNLBW, channel, 10);		
		phy_SetSwChnlCmdArray(RfDependCmd, RfDependCmdCnt++, MAX_RFDEPENDCMD_CNT, 
		CmdID_End, 0, 0, 0);		
		
		break;

	case RF_8258:
		break;

	// For FPGA two MAC verification
	case RF_PSEUDO_11N:
		return TRUE;
	default:
		RT_ASSERT(FALSE, ("Unknown RFChipID: %d\n", pHalData->RFChipID));
		return FALSE;
		break;
	}

	
	do{
		switch(*stage)
		{
		case 0:
			CurrentCmd=&PreCommonCmd[*step];
			break;
		case 1:
			CurrentCmd=&RfDependCmd[*step];
			break;
		case 2:
			CurrentCmd=&PostCommonCmd[*step];
			break;
		}
		
		if(CurrentCmd->CmdID==CmdID_End)
		{
			if((*stage)==2)
			{
				return TRUE;
			}
			else
			{
				(*stage)++;
				(*step)=0;
				continue;
			}
		}
		
		switch(CurrentCmd->CmdID)
		{
		case CmdID_SetTxPowerLevel:
			PHY_SetTxPowerLevel8192C(Adapter,channel);
			break;
		case CmdID_WritePortUlong:
			PlatformEFIOWrite4Byte(Adapter, CurrentCmd->Para1, CurrentCmd->Para2);
			break;
		case CmdID_WritePortUshort:
			PlatformEFIOWrite2Byte(Adapter, CurrentCmd->Para1, (u2Byte)CurrentCmd->Para2);
			break;
		case CmdID_WritePortUchar:
			PlatformEFIOWrite1Byte(Adapter, CurrentCmd->Para1, (u1Byte)CurrentCmd->Para2);
			break;
		case CmdID_RF_WriteReg:	// Only modify channel for the register now !!!!!
			for(eRFPath = 0; eRFPath <pHalData->NumTotalRFPath; eRFPath++)
			{
#if 1
				pHalData->RfRegChnlVal[eRFPath] = ((pHalData->RfRegChnlVal[eRFPath] & 0xfffffc00) | CurrentCmd->Para2);
				PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)eRFPath, CurrentCmd->Para1, bRFRegOffsetMask, pHalData->RfRegChnlVal[eRFPath]);
#else
				PHY_SetRFReg(Adapter, (RF90_RADIO_PATH_E)eRFPath, CurrentCmd->Para1, bRFRegOffsetMask, (CurrentCmd->Para2));
#endif
			}
			break;
		}
		
		break;
	}while(TRUE);
	//cosa }/*for(Number of RF paths)*/

	(*delay)=CurrentCmd->msDelay;
	(*step)++;
	return FALSE;
#endif	
	return _TRUE;
}


static	BOOLEAN
phy_SetSwChnlCmdArray(
	SwChnlCmd*		CmdTable,
	u32			CmdTableIdx,
	u32			CmdTableSz,
	SwChnlCmdID		CmdID,
	u32			Para1,
	u32			Para2,
	u32			msDelay
	)
{
	SwChnlCmd* pCmd;

	if(CmdTable == NULL)
	{
		//RT_ASSERT(FALSE, ("phy_SetSwChnlCmdArray(): CmdTable cannot be NULL.\n"));
		return _FALSE;
	}
	if(CmdTableIdx >= CmdTableSz)
	{
		//RT_ASSERT(FALSE, 
		//		("phy_SetSwChnlCmdArray(): Access invalid index, please check size of the table, CmdTableIdx:%ld, CmdTableSz:%ld\n",
		//		CmdTableIdx, CmdTableSz));
		return _FALSE;
	}

	pCmd = CmdTable + CmdTableIdx;
	pCmd->CmdID = CmdID;
	pCmd->Para1 = Para1;
	pCmd->Para2 = Para2;
	pCmd->msDelay = msDelay;

	return _TRUE;
}


static	void
phy_FinishSwChnlNow(	// We should not call this function directly
		IN	PADAPTER	Adapter,
		IN	u8		channel
		)
{
#if 0
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u32			delay;
  
	while(!phy_SwChnlStepByStep(Adapter,channel,&pHalData->SwChnlStage,&pHalData->SwChnlStep,&delay))
	{
		if(delay>0)
			mdelay_os(delay);
	}
#endif	
}


//
//	Description:
//		Configure H/W functionality to enable/disable Monitor mode.
//		Note, because we possibly need to configure BB and RF in this function, 
//		so caller should in PASSIVE_LEVEL. 080118, by rcnjko.
//
extern	VOID
PHY_SetMonitorMode8192C(
	IN	PADAPTER			pAdapter,
	IN	BOOLEAN				bEnableMonitorMode
	)
{
#if 0
	HAL_DATA_TYPE		*pHalData	= GET_HAL_DATA(pAdapter);
	BOOLEAN				bFilterOutNonAssociatedBSSID = FALSE;

	//2 Note: we may need to stop antenna diversity.
	if(bEnableMonitorMode)
	{
		bFilterOutNonAssociatedBSSID = FALSE;
		RT_TRACE(COMP_RM, DBG_LOUD, ("PHY_SetMonitorMode8192S(): enable monitor mode\n"));

		pHalData->bInMonitorMode = TRUE;
		pAdapter->HalFunc.AllowAllDestAddrHandler(pAdapter, TRUE, TRUE);
		pAdapter->HalFunc.SetHwRegHandler(pAdapter, HW_VAR_CHECK_BSSID, (pu1Byte)&bFilterOutNonAssociatedBSSID);
	}
	else
	{
		bFilterOutNonAssociatedBSSID = TRUE;
		RT_TRACE(COMP_RM, DBG_LOUD, ("PHY_SetMonitorMode8192S(): disable monitor mode\n"));

		pAdapter->HalFunc.AllowAllDestAddrHandler(pAdapter, FALSE, TRUE);
		pHalData->bInMonitorMode = FALSE;
		pAdapter->HalFunc.SetHwRegHandler(pAdapter, HW_VAR_CHECK_BSSID, (pu1Byte)&bFilterOutNonAssociatedBSSID);
	}
#endif	
}


/*-----------------------------------------------------------------------------
 * Function:	PHYCheckIsLegalRfPath8190Pci()
 *
 * Overview:	Check different RF type to execute legal judgement. If RF Path is illegal
 *			We will return false.
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	11/15/2007	MHC		Create Version 0.  
 *
 *---------------------------------------------------------------------------*/
extern	BOOLEAN	
PHY_CheckIsLegalRfPath8192C(	
	IN	PADAPTER	pAdapter,
	IN	u32	eRFPath)
{
//	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	BOOLEAN				rtValue = _TRUE;

	// NOt check RF Path now.!
#if 0	
	if (pHalData->RF_Type == RF_1T2R && eRFPath != RF90_PATH_A)
	{		
		rtValue = FALSE;
	}
	if (pHalData->RF_Type == RF_1T2R && eRFPath != RF90_PATH_A)
	{

	}
#endif
	return	rtValue;

}	/* PHY_CheckIsLegalRfPath8192C */

//-------------------------------------------------------------------------
//
//	IQK
//
//-------------------------------------------------------------------------
#define IQK_ADDA_REG_NUM	16
#define MAX_TOLERANCE		5
#define IQK_DELAY_TIME		1 	//ms

#define IQK_MAC_REG_NUM		4

u8			//bit0 = 1 => Tx OK, bit1 = 1 => Rx OK
_PHY_PathA_IQK(
	IN	PADAPTER	pAdapter,
	IN	BOOLEAN		configPathB
	)
{
	u32 regEAC, regE94, regE9C, regEA4;
	u8 result = 0x00;

	//RTPRINT(FINIT, INIT_IQK, ("Path A IQK!\n"));

	//path-A IQK setting
	//RTPRINT(FINIT, INIT_IQK, ("Path-A IQK setting!\n"));
	PHY_SetBBReg(pAdapter, 0xe30, bMaskDWord, 0x10008c1f);
	PHY_SetBBReg(pAdapter, 0xe34, bMaskDWord, 0x10008c1f);
	PHY_SetBBReg(pAdapter, 0xe38, bMaskDWord, 0x82140102);

	PHY_SetBBReg(pAdapter, 0xe3c, bMaskDWord, configPathB ? 0x28160202 : 0x28160502);

#if 1
	//path-B IQK setting
	if(configPathB)
	{
		PHY_SetBBReg(pAdapter, 0xe50, bMaskDWord, 0x10008c22);
		PHY_SetBBReg(pAdapter, 0xe54, bMaskDWord, 0x10008c22);
		PHY_SetBBReg(pAdapter, 0xe58, bMaskDWord, 0x82140102);
		PHY_SetBBReg(pAdapter, 0xe5c, bMaskDWord, 0x28160202);
	}
#endif
	//LO calibration setting
	//RTPRINT(FINIT, INIT_IQK, ("LO calibration setting!\n"));
	PHY_SetBBReg(pAdapter, 0xe4c, bMaskDWord, 0x001028d1);

	//One shot, path A LOK & IQK
	//RTPRINT(FINIT, INIT_IQK, ("One shot, path A LOK & IQK!\n"));
	PHY_SetBBReg(pAdapter, 0xe48, bMaskDWord, 0xf9000000);
	PHY_SetBBReg(pAdapter, 0xe48, bMaskDWord, 0xf8000000);
	
	// delay x ms
	//RTPRINT(FINIT, INIT_IQK, ("Delay %d ms for One shot, path A LOK & IQK.\n", IQK_DELAY_TIME));
	udelay_os(IQK_DELAY_TIME*1000);//PlatformStallExecution(IQK_DELAY_TIME*1000);

	// Check failed
	regEAC = PHY_QueryBBReg(pAdapter, 0xeac, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xeac = 0x%x\n", regEAC));
	regE94 = PHY_QueryBBReg(pAdapter, 0xe94, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xe94 = 0x%x\n", regE94));
	regE9C= PHY_QueryBBReg(pAdapter, 0xe9c, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xe9c = 0x%x\n", regE9C));
	regEA4= PHY_QueryBBReg(pAdapter, 0xea4, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xea4 = 0x%x\n", regEA4));

        if(!(regEAC & BIT28) &&		
		(((regE94 & 0x03FF0000)>>16) != 0x142) &&
		(((regE9C & 0x03FF0000)>>16) != 0x42) )
		result |= 0x01;
	else							//if Tx not OK, ignore Rx
		return result;

	if(!(regEAC & BIT27) &&		//if Tx is OK, check whether Rx is OK
		(((regEA4 & 0x03FF0000)>>16) != 0x132) &&
		(((regEAC & 0x03FF0000)>>16) != 0x36))
		result |= 0x02;
	else
		DBG_8192C("Path A Rx IQK fail!!\n");
	
	return result;


}

u8				//bit0 = 1 => Tx OK, bit1 = 1 => Rx OK
_PHY_PathB_IQK(
	IN	PADAPTER	pAdapter
	)
{
	u32 regEAC, regEB4, regEBC, regEC4, regECC;
	u8	result = 0x00;
	//RTPRINT(FINIT, INIT_IQK, ("Path B IQK!\n"));
#if 0
	//path-B IQK setting
	RTPRINT(FINIT, INIT_IQK, ("Path-B IQK setting!\n"));
	PHY_SetBBReg(pAdapter, 0xe50, bMaskDWord, 0x10008c22);
	PHY_SetBBReg(pAdapter, 0xe54, bMaskDWord, 0x10008c22);
	PHY_SetBBReg(pAdapter, 0xe58, bMaskDWord, 0x82140102);
	PHY_SetBBReg(pAdapter, 0xe5c, bMaskDWord, 0x28160202);

	//LO calibration setting
	RTPRINT(FINIT, INIT_IQK, ("LO calibration setting!\n"));
	PHY_SetBBReg(pAdapter, 0xe4c, bMaskDWord, 0x001028d1);
#endif
	//One shot, path B LOK & IQK
	//RTPRINT(FINIT, INIT_IQK, ("One shot, path A LOK & IQK!\n"));
	PHY_SetBBReg(pAdapter, 0xe60, bMaskDWord, 0x00000002);
	PHY_SetBBReg(pAdapter, 0xe60, bMaskDWord, 0x00000000);

	// delay x ms
	//RTPRINT(FINIT, INIT_IQK, ("Delay %d ms for One shot, path B LOK & IQK.\n", IQK_DELAY_TIME));
	udelay_os(IQK_DELAY_TIME*1000);//PlatformStallExecution(IQK_DELAY_TIME*1000);

	// Check failed
	regEAC = PHY_QueryBBReg(pAdapter, 0xeac, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xeac = 0x%x\n", regEAC));
	regEB4 = PHY_QueryBBReg(pAdapter, 0xeb4, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xeb4 = 0x%x\n", regEB4));
	regEBC= PHY_QueryBBReg(pAdapter, 0xebc, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xebc = 0x%x\n", regEBC));
	regEC4= PHY_QueryBBReg(pAdapter, 0xec4, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xec4 = 0x%x\n", regEC4));
	regECC= PHY_QueryBBReg(pAdapter, 0xecc, bMaskDWord);
	//RTPRINT(FINIT, INIT_IQK, ("0xecc = 0x%x\n", regECC));

	if(!(regEAC & BIT31) &&
		(((regEB4 & 0x03FF0000)>>16) != 0x142) &&
		(((regEBC & 0x03FF0000)>>16) != 0x42))
		result |= 0x01;
	else
		return result;

	if(!(regEAC & BIT30) &&
		(((regEC4 & 0x03FF0000)>>16) != 0x132) &&
		(((regECC & 0x03FF0000)>>16) != 0x36))
		result |= 0x02;
	else
		DBG_8192C("Path B Rx IQK fail!!\n");
	

	return result;

}

VOID
_PHY_PathAFillIQKMatrix(
	IN	PADAPTER	pAdapter,
	IN  BOOLEAN    	bIQKOK,
	IN	int		result[][8],
	IN	u8		final_candidate,
	IN  BOOLEAN		bTxOnly
	)
{
	u32	Oldval_0, X, TX0_A, reg;
	int	Y, TX0_C;
	
	DBG_8192C("Path A IQ Calibration %s !\n",(bIQKOK)?"Success":"Failed");

        if(final_candidate == 0xFF)
		return;
	else if(bIQKOK)
	{
		Oldval_0 = (PHY_QueryBBReg(pAdapter, 0xc80, bMaskDWord) >> 22) & 0x3FF;

			X = result[final_candidate][0];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;				
		TX0_A = (X * Oldval_0) >> 8;
		//RTPRINT(FINIT, INIT_IQK, ("X = 0x%lx, TX0_A = 0x%lx, Oldval_0 0x%lx\n", X, TX0_A, Oldval_0));
		PHY_SetBBReg(pAdapter, 0xc80, 0x3FF, TX0_A);
		PHY_SetBBReg(pAdapter, 0xc4c, BIT(24), ((X* Oldval_0>>7) & 0x1));

		Y = result[final_candidate][1];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;		
		TX0_C = (Y * Oldval_0) >> 8;
		//RTPRINT(FINIT, INIT_IQK, ("Y = 0x%lx, TX = 0x%lx\n", Y, TX0_C));
		PHY_SetBBReg(pAdapter, 0xc94, 0xF0000000, ((TX0_C&0x3C0)>>6));
		PHY_SetBBReg(pAdapter, 0xc80, 0x003F0000, (TX0_C&0x3F));
		PHY_SetBBReg(pAdapter, 0xc4c, BIT(26), ((Y* Oldval_0>>7) & 0x1));

	        if(bTxOnly)
		{
			DBG_8192C("_PHY_PathAFillIQKMatrix only Tx OK\n");
			return;
		}

		reg = result[final_candidate][2];
		PHY_SetBBReg(pAdapter, 0xc14, 0x3FF, reg);
	
		reg = result[final_candidate][3] & 0x3F;
		PHY_SetBBReg(pAdapter, 0xc14, 0xFC00, reg);

		reg = (result[final_candidate][3] >> 6) & 0xF;
		PHY_SetBBReg(pAdapter, 0xca0, 0xF0000000, reg);
	}
}

VOID
_PHY_PathBFillIQKMatrix(
	IN	PADAPTER	pAdapter,
	IN  BOOLEAN   	bIQKOK,
	IN	int		result[][8],
	IN	u8		final_candidate,
	IN	BOOLEAN		bTxOnly			//do Tx only
	)
{
	u32	Oldval_1, X, TX1_A, reg;
	int	Y, TX1_C;
	
	DBG_8192C("Path B IQ Calibration %s !\n",(bIQKOK)?"Success":"Failed");

        if(final_candidate == 0xFF)
		return;
	else if(bIQKOK)
	{
		Oldval_1 = (PHY_QueryBBReg(pAdapter, 0xc88, bMaskDWord) >> 22) & 0x3FF;

		X = result[final_candidate][4];
		if ((X & 0x00000200) != 0)
			X = X | 0xFFFFFC00;		
		TX1_A = (X * Oldval_1) >> 8;
		//RTPRINT(FINIT, INIT_IQK, ("X = 0x%lx, TX1_A = 0x%lx\n", X, TX1_A));
		PHY_SetBBReg(pAdapter, 0xc88, 0x3FF, TX1_A);
		PHY_SetBBReg(pAdapter, 0xc4c, BIT(28), ((X* Oldval_1>>7) & 0x1));

		Y = result[final_candidate][5];
		if ((Y & 0x00000200) != 0)
			Y = Y | 0xFFFFFC00;		
		TX1_C = (Y * Oldval_1) >> 8;
		//RTPRINT(FINIT, INIT_IQK, ("Y = 0x%lx, TX1_C = 0x%lx\n", Y, TX1_C));
		PHY_SetBBReg(pAdapter, 0xc9c, 0xF0000000, ((TX1_C&0x3C0)>>6));
		PHY_SetBBReg(pAdapter, 0xc88, 0x003F0000, (TX1_C&0x3F));
		PHY_SetBBReg(pAdapter, 0xc4c, BIT(30), ((Y* Oldval_1>>7) & 0x1));

		if(bTxOnly)
			return;

		reg = result[final_candidate][6];
		PHY_SetBBReg(pAdapter, 0xc1c, 0x3FF, reg);
	
		reg = result[final_candidate][7] & 0x3F;
		PHY_SetBBReg(pAdapter, 0xc1c, 0xFC00, reg);

		reg = (result[final_candidate][7] >> 6) & 0xF;
		PHY_SetBBReg(pAdapter, 0xc78, 0x0000F000, reg);
	}
}

VOID
_PHY_SaveADDARegisters(
	IN	PADAPTER	pAdapter,
	IN	u32*		ADDAReg,
	IN	u32*		ADDABackup
	)
{
	u32	i;
	
	//RTPRINT(FINIT, INIT_IQK, ("Save ADDA parameters.\n"));
	for( i = 0 ; i < IQK_ADDA_REG_NUM ; i++){
		ADDABackup[i] = PHY_QueryBBReg(pAdapter, ADDAReg[i], bMaskDWord);
	}
}
VOID
_PHY_SaveMACRegisters(
	IN	PADAPTER	pAdapter,
	IN	u32*		MACReg,
	IN	u32*		MACBackup
	)
{
	u32	i;
	
	//RTPRINT(FINIT, INIT_IQK, ("Save MAC parameters.\n"));
	for( i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++){
		MACBackup[i] =read8(pAdapter, MACReg[i]);		
	}
	MACBackup[i] = read32(pAdapter, MACReg[i]);		

}
VOID
_PHY_MACSettingCalibration(
	IN	PADAPTER	pAdapter,
	IN	u32*		MACReg,
	IN	u32*		MACBackup	
	)
{
	u32	i = 0;

	//RTPRINT(FINIT, INIT_IQK, ("MAC settings for Calibration.\n"));

	write8(pAdapter, MACReg[i], 0x3F);

	for(i = 1 ; i < (IQK_MAC_REG_NUM - 1); i++){
		write8(pAdapter, MACReg[i], (u8)(MACBackup[i]&(~BIT3)));
	}
	write8(pAdapter, MACReg[i], (u8)(MACBackup[i]&(~BIT5)));	

}

VOID
_PHY_ReloadMACRegisters(
	IN	PADAPTER	pAdapter,
	IN	u32*			MACReg,
	IN	u32*			MACBackup
	)
{
	u32	i;

	//RTPRINT(FINIT, INIT_IQK, ("Reload MAC parameters !\n"));
	for(i = 0 ; i < (IQK_MAC_REG_NUM - 1); i++){
		write8(pAdapter, MACReg[i], (u8)MACBackup[i]);
	}
	write32(pAdapter, MACReg[i], MACBackup[i]);	
}

VOID
_PHY_ReloadADDARegisters(
	IN	PADAPTER	pAdapter,
	IN	u32*		ADDAReg,
	IN	u32*		ADDABackup
	)
{
	u32	i;

	//RTPRINT(FINIT, INIT_IQK, ("Reload ADDA power saving parameters !\n"));
	for(i = 0 ; i < IQK_ADDA_REG_NUM ; i++){
		PHY_SetBBReg(pAdapter, ADDAReg[i], bMaskDWord, ADDABackup[i]);
	}
}

VOID
_PHY_PathADDAOn(
	IN	PADAPTER	pAdapter,
	IN	u32*		ADDAReg,
	IN	BOOLEAN		isPathAOn,
	IN	BOOLEAN		is2T
	)
{
	u32	pathOn;
	u32	i;

	//RTPRINT(FINIT, INIT_IQK, ("ADDA ON.\n"));

	pathOn = isPathAOn ? 0x04db25a4 : 0x0b1b25a4;
	if(_FALSE == is2T){
		pathOn = 0x0bdb25a0;
		PHY_SetBBReg(pAdapter, ADDAReg[0], bMaskDWord, 0x0b1b25a0);
	}
	else{
		PHY_SetBBReg(pAdapter, ADDAReg[0], bMaskDWord, pathOn);
	}
	
	for( i = 1 ; i < IQK_ADDA_REG_NUM ; i++){
		PHY_SetBBReg(pAdapter, ADDAReg[i], bMaskDWord, pathOn);
	}
	
}

VOID
_PHY_PathAStandBy(
	IN	PADAPTER	pAdapter
	)
{
	//RTPRINT(FINIT, INIT_IQK, ("Path-A standby mode!\n"));

	PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x0);
	PHY_SetBBReg(pAdapter, 0x840, bMaskDWord, 0x00010000);
	PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x80800000);
}

VOID
_PHY_PIModeSwitch(
	IN	PADAPTER	pAdapter,
	IN	BOOLEAN		PIMode
	)
{
	u32	mode;

	//RTPRINT(FINIT, INIT_IQK, ("BB Switch to %s mode!\n", (PIMode ? "PI" : "SI")));

	mode = PIMode ? 0x01000100 : 0x01000000;
	PHY_SetBBReg(pAdapter, 0x820, bMaskDWord, mode);
	PHY_SetBBReg(pAdapter, 0x828, bMaskDWord, mode);
}

/*
return _FALSE => do IQK again
*/
BOOLEAN							
_PHY_SimularityCompare(
	IN	PADAPTER	pAdapter,
	IN	int 		result[][8],
	IN	u8		 c1,
	IN	u8		 c2
	)
{
	u32		i, j, diff, SimularityBitMap, bound = 0;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);	
	u8		final_candidate[2] = {0xFF, 0xFF};	//for path A and path B
	BOOLEAN		bResult = _TRUE, is2T = IS_92C_SERIAL( pHalData->VersionID);
	
	if(is2T)
		bound = 8;
	else
		bound = 4;

	SimularityBitMap = 0;
	
	for( i = 0; i < bound; i++ )
	{
		diff = (result[c1][i] > result[c2][i]) ? (result[c1][i] - result[c2][i]) : (result[c2][i] - result[c1][i]);
		if (diff > MAX_TOLERANCE)
		{
			if((i == 2 || i == 6) && !SimularityBitMap)
			{
				if(result[c1][i]+result[c1][i+1] == 0)
					final_candidate[(i/4)] = c2;
				else if (result[c2][i]+result[c2][i+1] == 0)
					final_candidate[(i/4)] = c1;
				else
					SimularityBitMap = SimularityBitMap|(1<<i);					
			}
			else
				SimularityBitMap = SimularityBitMap|(1<<i);
		}
	}
	
	if ( SimularityBitMap == 0)
	{
		for( i = 0; i < (bound/4); i++ )
		{
			if(final_candidate[i] != 0xFF)
			{
				for( j = i*4; j < (i+1)*4-2; j++)
					result[3][j] = result[final_candidate[i]][j];
				bResult = _FALSE;
			}
		}
		return bResult;
	}
	else if (!(SimularityBitMap & 0x0F))			//path A OK
	{
		for(i = 0; i < 4; i++)
			result[3][i] = result[c1][i];
		return _FALSE;
	}
	else if (!(SimularityBitMap & 0xF0) && is2T)	//path B OK
	{
		for(i = 4; i < 8; i++)
			result[3][i] = result[c1][i];
		return _FALSE;
	}	
	else		
		return _FALSE;
	
}

VOID	
_PHY_IQCalibrate(
	IN	PADAPTER	pAdapter,
	IN	int 		result[][8],
	IN	u8		t,
	IN	BOOLEAN		is2T
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	u32			i;
	u8			PathAOK, PathBOK;
	u32			ADDA_REG[IQK_ADDA_REG_NUM] = {	0x85c, 0xe6c, 0xe70, 0xe74,
													0xe78, 0xe7c, 0xe80, 0xe84,
													0xe88, 0xe8c, 0xed0, 0xed4,
													0xed8, 0xedc, 0xee0, 0xeec };

	u32			IQK_MAC_REG[IQK_MAC_REG_NUM] = {0x522, 0x550,	0x551,0x040};
#if MP_DRIVER
	const u32	retryCount = 9;
#else
	const u32	retryCount = 2;
#endif

	// Note: IQ calibration must be performed after loading 
	// 		PHY_REG.txt , and radio_a, radio_b.txt	
	
	u32 bbvalue;
	BOOLEAN			isNormal = IS_NORMAL_CHIP(pHalData->VersionID);

	if(t==0)
	{
	 	bbvalue = PHY_QueryBBReg(pAdapter, 0x800, bMaskDWord);
		//RTPRINT(FINIT, INIT_IQK, ("PHY_IQCalibrate()==>0x%08lx\n",bbvalue));

		//RTPRINT(FINIT, INIT_IQK, ("IQ Calibration for %s\n", (is2T ? "2T2R" : "1T1R")));
	
	 	// Save ADDA parameters, turn Path A ADDA on
	 	_PHY_SaveADDARegisters(pAdapter, ADDA_REG, pHalData->ADDA_backup);
		_PHY_SaveMACRegisters(pAdapter, IQK_MAC_REG, pHalData->IQK_MAC_backup);
	}
 	_PHY_PathADDAOn(pAdapter, ADDA_REG, _TRUE, is2T);

	if(t==0)
	{
		pHalData->bRfPiEnable = (u8)PHY_QueryBBReg(pAdapter, rFPGA0_XA_HSSIParameter1, BIT(8));
	}
	if(!pHalData->bRfPiEnable){
		// Switch BB to PI mode to do IQ Calibration.
		_PHY_PIModeSwitch(pAdapter, _TRUE);
	}
	if(t==0)
	{
	// Store 0xC04, 0xC08, 0x874 vale
	 	pHalData->RegC04 = PHY_QueryBBReg(pAdapter, 0xc04, bMaskDWord);
	 	pHalData->RegC08 = PHY_QueryBBReg(pAdapter, 0xc08, bMaskDWord);
	 	pHalData->Reg874 = PHY_QueryBBReg(pAdapter, 0x874, bMaskDWord);
	}
	PHY_SetBBReg(pAdapter, 0xc04, bMaskDWord, 0x03a05600);
	PHY_SetBBReg(pAdapter, 0xc08, bMaskDWord, 0x000800e4);
	PHY_SetBBReg(pAdapter, 0x874, bMaskDWord, 0x22204000);

	if(is2T)
	{
		PHY_SetBBReg(pAdapter, 0x840, bMaskDWord, 0x00010000);
		PHY_SetBBReg(pAdapter, 0x844, bMaskDWord, 0x00010000);
	}
	
	//MAC settings
	_PHY_MACSettingCalibration(pAdapter, IQK_MAC_REG, pHalData->IQK_MAC_backup);
	//Page B init
	if(isNormal)
		PHY_SetBBReg(pAdapter, 0xb68, bMaskDWord, 0x00080000);		
	else
		PHY_SetBBReg(pAdapter, 0xb68, bMaskDWord, 0x0f600000);
	
	if(is2T)
	{
		if(isNormal)	
			PHY_SetBBReg(pAdapter, 0xb6c, bMaskDWord, 0x00080000);
		else
			PHY_SetBBReg(pAdapter, 0xb6c, bMaskDWord, 0x0f600000);
	}
	
	// IQ calibration setting
	//RTPRINT(FINIT, INIT_IQK, ("IQK setting!\n"));		
	PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x80800000);
	PHY_SetBBReg(pAdapter, 0xe40, bMaskDWord, 0x01007c00);
	PHY_SetBBReg(pAdapter, 0xe44, bMaskDWord, 0x01004800);

	for(i = 0 ; i < retryCount ; i++){
		PathAOK = _PHY_PathA_IQK(pAdapter, is2T);
		if(PathAOK == 0x03){
				DBG_8192C("Path A IQK Success!!\n");
				result[t][0] = (PHY_QueryBBReg(pAdapter, 0xe94, bMaskDWord)&0x3FF0000)>>16;
				result[t][1] = (PHY_QueryBBReg(pAdapter, 0xe9c, bMaskDWord)&0x3FF0000)>>16;
				result[t][2] = (PHY_QueryBBReg(pAdapter, 0xea4, bMaskDWord)&0x3FF0000)>>16;
				result[t][3] = (PHY_QueryBBReg(pAdapter, 0xeac, bMaskDWord)&0x3FF0000)>>16;
			break;
		}
		else if (i == (retryCount-1) && PathAOK == 0x01)	//Tx IQK OK
		{
			DBG_8192C("Path A IQK Only  Tx Success!!\n");
			
			result[t][0] = (PHY_QueryBBReg(pAdapter, 0xe94, bMaskDWord)&0x3FF0000)>>16;
			result[t][1] = (PHY_QueryBBReg(pAdapter, 0xe9c, bMaskDWord)&0x3FF0000)>>16;			
		}
	}

	if(0x00 == PathAOK){		
		DBG_8192C("Path A IQK failed!!\n");
	}

	if(is2T){
		_PHY_PathAStandBy(pAdapter);

		// Turn Path B ADDA on
		_PHY_PathADDAOn(pAdapter, ADDA_REG, _FALSE, is2T);

		for(i = 0 ; i < retryCount ; i++){
			PathBOK = _PHY_PathB_IQK(pAdapter);
			if(PathBOK == 0x03){
				DBG_8192C("Path B IQK Success!!\n");
				result[t][4] = (PHY_QueryBBReg(pAdapter, 0xeb4, bMaskDWord)&0x3FF0000)>>16;
				result[t][5] = (PHY_QueryBBReg(pAdapter, 0xebc, bMaskDWord)&0x3FF0000)>>16;
				result[t][6] = (PHY_QueryBBReg(pAdapter, 0xec4, bMaskDWord)&0x3FF0000)>>16;
				result[t][7] = (PHY_QueryBBReg(pAdapter, 0xecc, bMaskDWord)&0x3FF0000)>>16;
				break;
			}
			else if (i == (retryCount - 1) && PathBOK == 0x01)	//Tx IQK OK
			{
				DBG_8192C("Path B Only Tx IQK Success!!\n");
				result[t][4] = (PHY_QueryBBReg(pAdapter, 0xeb4, bMaskDWord)&0x3FF0000)>>16;
				result[t][5] = (PHY_QueryBBReg(pAdapter, 0xebc, bMaskDWord)&0x3FF0000)>>16;				
			}
		}

		if(0x00 == PathBOK){		
			DBG_8192C("Path B IQK failed!!\n");
		}
	}

#if 0
	_PHY_PathAFillIQKMatrix(pAdapter,bPathAOK);
	if(is2T){
		_PHY_PathBFillIQKMatrix(pAdapter,bPathBOK);
	}
#endif	
	//Back to BB mode, load original value
	//RTPRINT(FINIT, INIT_IQK, ("IQK:Back to BB mode, load original value!\n"));
	PHY_SetBBReg(pAdapter, 0xc04, bMaskDWord, pHalData->RegC04);
	PHY_SetBBReg(pAdapter, 0x874, bMaskDWord, pHalData->Reg874);
	PHY_SetBBReg(pAdapter, 0xc08, bMaskDWord, pHalData->RegC08);

	PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0);

	// Restore RX initial gain
	PHY_SetBBReg(pAdapter, 0x840, bMaskDWord, 0x00032ed3);
	if(is2T){
		PHY_SetBBReg(pAdapter, 0x844, bMaskDWord, 0x00032ed3);
	}
	if(t!=0)
	{
		if(!pHalData->bRfPiEnable){
		// Switch back BB to SI mode after finish IQ Calibration.
		_PHY_PIModeSwitch(pAdapter, _FALSE);
	        }

		// Reload ADDA power saving parameters
	 	_PHY_ReloadADDARegisters(pAdapter, ADDA_REG, pHalData->ADDA_backup);

		// Reload MAC parameters
		_PHY_ReloadMACRegisters(pAdapter, IQK_MAC_REG, pHalData->IQK_MAC_backup);
	}
	//RTPRINT(FINIT, INIT_IQK, ("_PHY_IQCalibrate() <==\n"));
	
}


VOID	
_PHY_LCCalibrate(
	IN	PADAPTER	pAdapter,
	IN	BOOLEAN		is2T
	)
{
	u8	tmpReg;
	u32 	RF_Amode, RF_Bmode, LC_Cal;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	BOOLEAN	isNormal = IS_NORMAL_CHIP(pHalData->VersionID);

	//Check continuous TX and Packet TX
	tmpReg = read32(pAdapter, 0xd03);

	if((tmpReg&0x70) != 0)			//Deal with contisuous TX case
		write8(pAdapter, 0xd03, tmpReg&0x8F);	//disable all continuous TX
	else 							// Deal with Packet TX case
		write8(pAdapter, 0x42, 0xFF);			// block all queues

	if((tmpReg&0x70) != 0)
	{
		//1. Read original RF mode
		//Path-A
		RF_Amode = PHY_QueryRFReg(pAdapter, RF90_PATH_A, 0x00, bMask12Bits);

		//Path-B
		if(is2T)
			RF_Bmode = PHY_QueryRFReg(pAdapter, RF90_PATH_B, 0x00, bMask12Bits);	

		//2. Set RF mode = standby mode
		//Path-A
		PHY_SetRFReg(pAdapter, RF90_PATH_A, 0x00, bMask12Bits, (RF_Amode&0x8FFFF)|0x10000);

		//Path-B
		if(is2T)
			PHY_SetRFReg(pAdapter, RF90_PATH_B, 0x00, bMask12Bits, (RF_Bmode&0x8FFFF)|0x10000);			
	}
	
	//3. Read RF reg18
	LC_Cal = PHY_QueryRFReg(pAdapter, RF90_PATH_A, 0x18, bMask12Bits);
	
	//4. Set LC calibration begin
	PHY_SetRFReg(pAdapter, RF90_PATH_A, 0x18, bMask12Bits, LC_Cal|0x08000);

	if(isNormal)
		mdelay_os(100);		
	else
		mdelay_os(3);

	//Restore original situation
	if((tmpReg&0x70) != 0)	//Deal with contisuous TX case 
	{  
		//Path-A
		write8(pAdapter, 0xd03, tmpReg);
		PHY_SetRFReg(pAdapter, RF90_PATH_A, 0x00, bMask12Bits, RF_Amode);
		
		//Path-B
		if(is2T)
			PHY_SetRFReg(pAdapter, RF90_PATH_B, 0x00, bMask12Bits, RF_Bmode);
	}
	else // Deal with Packet TX case
	{
		write8(pAdapter, 0x42, 0x00);	
	}
	
}


VOID	
_PHY_APCalibrate(
	IN	PADAPTER	pAdapter,
	IN	char 		delta,
	IN	BOOLEAN		is2T
	)
{

//Analog Pre-distortion calibration
#define		APK_BB_REG_NUM	5
#define		APK_AFE_REG_NUM	16
#define		APK_CURVE_REG_NUM 4
#define		PATH_NUM		2

#if 1//(PLATFORM == PLATFORM_WINDOWS)//???
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	u32 			regD[PATH_NUM];
	u32			tmpReg, index, offset, path, i, pathbound = PATH_NUM;
			
	u32			BB_backup[APK_BB_REG_NUM];
	u32			BB_REG[APK_BB_REG_NUM] = {	
						0x904, 0xc04, 0x800, 0xc08, 0x874 };
	u32			BB_AP_MODE[APK_BB_REG_NUM] = {	
						0x00000020, 0x00a05430, 0x02040000, 
						0x000800e4, 0x00204000 };
	u32			BB_normal_AP_MODE[APK_BB_REG_NUM] = {	
						0x00000020, 0x00a05430, 0x02040000, 
						0x000800e4, 0x22204000 };						

	u32			AFE_backup[APK_AFE_REG_NUM];
	u32			AFE_REG[APK_AFE_REG_NUM] = {	
						0x85c, 0xe6c, 0xe70, 0xe74, 0xe78, 
						0xe7c, 0xe80, 0xe84, 0xe88, 0xe8c, 
						0xed0, 0xed4, 0xed8, 0xedc, 0xee0,
						0xeec};

	u32			APK_RF_init_value[PATH_NUM][APK_BB_REG_NUM] = {
					{0x0852c, 0x1852c, 0x5852c, 0x1852c, 0x5852c},
					{0x2852e, 0x0852e, 0x3852e, 0x0852e, 0x0852e}
					};	

	u32			APK_normal_RF_init_value[PATH_NUM][APK_BB_REG_NUM] = {
					{0x0852c, 0x3852c, 0x0852c, 0x0852c, 0x4852c},
					{0x2852e, 0x0852e, 0x3852e, 0x0852e, 0x0852e}
					};
	
	u32			APK_RF_value_0[PATH_NUM][APK_BB_REG_NUM] = {
					{0x52019, 0x52014, 0x52013, 0x5200f, 0x5208d},
					{0x5201a, 0x52019, 0x52016, 0x52033, 0x52050}
					};

	u32			APK_normal_RF_value_0[PATH_NUM][APK_BB_REG_NUM] = {
					{0x52019, 0x52017, 0x52013, 0x52010, 0x5200d},
					{0x5201a, 0x52019, 0x52016, 0x52033, 0x52050}
					};
	
	u32			APK_RF_value_A[PATH_NUM][APK_BB_REG_NUM] = {
					{0x1adb0, 0x1adb0, 0x1ada0, 0x1ad90, 0x1ad80},		
					{0x00fb0, 0x00fb0, 0x00fa0, 0x00f90, 0x00f80}						
					};

	u32			AFE_on_off[PATH_NUM] = {
					0x04db25a4, 0x0b1b25a4};	//path A on path B off / path A off path B on

	u32			APK_offset[PATH_NUM] = {
					0xb68, 0xb6c};

	u32			APK_normal_offset[PATH_NUM] = {
					0xb28, 0xb98};
					
	u32			APK_value[PATH_NUM] = {
					0x92fc0000, 0x12fc0000};					

	u32			APK_normal_value[PATH_NUM] = {
					0x92680000, 0x12680000};					

	char			APK_delta_mapping[APK_BB_REG_NUM][13] = {
					{-4, -3, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},
					{-4, -3, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},											
					{-6, -4, -2, -2, -1, -1, 0, 1, 2, 3, 4, 5, 6},
					{-1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6},
					{-11, -9, -7, -5, -3, -1, 0, 0, 0, 0, 0, 0, 0}
					};
	
	u32			APK_normal_setting_value_1[13] = {
					0x01017018, 0xf7ed8f84, 0x40372d20, 0x5b554e48, 0x6f6a6560,
					0x807c7873, 0x8f8b8884, 0x9d999693, 0xa9a6a3a0, 0xb5b2afac,
					0x12680000, 0x00880000, 0x00880000
					};

	u32			APK_normal_setting_value_2[16] = {
					0x00810100, 0x00400056, 0x002b0032, 0x001f0024, 0x0019001c,
					0x00150017, 0x00120013, 0x00100011, 0x000e000f, 0x000c000d,
					0x000b000c, 0x000a000b, 0x0009000a, 0x00090009, 0x00080008,
					0x00080008
					};
	
	u32			APK_result[PATH_NUM][APK_BB_REG_NUM];	//val_1_1a, val_1_2a, val_2a, val_3a, val_4a
	u32			AP_curve[PATH_NUM][APK_CURVE_REG_NUM];

	int			BB_offset, delta_V, delta_offset;

	BOOLEAN			isNormal = IS_NORMAL_CHIP(pHalData->VersionID);

#if (MP_DRIVER == 1)
	PMPT_CONTEXT	pMptCtx = &(pAdapter->MptCtx);	

	pMptCtx->APK_bound[0] = 45;
	pMptCtx->APK_bound[1] = 52;		
#endif

	//RTPRINT(FINIT, INIT_IQK, ("==>PHY_APCalibrate() delta %d\n", delta));
	
	//RTPRINT(FINIT, INIT_IQK, ("AP Calibration for %s %s\n", (is2T ? "2T2R" : "1T1R"), (isNormal ? "Normal chip" : "Test chip")));

	if(!is2T)
		pathbound = 1;

	if(isNormal)
	{
#if (MP_DRIVER != 1)
		return;
#endif
	
		//settings adjust for normal chip
		for(index = 0; index < PATH_NUM; index ++)
		{
 			APK_offset[index] = APK_normal_offset[index];
			APK_value[index] = APK_normal_value[index];
			AFE_on_off[index] = 0x6fdb25a4;
		}

		for(index = 0; index < APK_BB_REG_NUM; index ++)
		{
			APK_RF_init_value[0][index] = APK_normal_RF_init_value[0][index];
			APK_RF_value_0[0][index] = APK_normal_RF_value_0[0][index];
			BB_AP_MODE[index] = BB_normal_AP_MODE[index];
		}

		//path A APK
		//load APK setting
		//path-A		
		offset = 0xb00;
		for(index = 0; index < 11; index ++)			
		{
			PHY_SetBBReg(pAdapter, offset, bMaskDWord, APK_normal_setting_value_1[index]);
			//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x%x value 0x%x\n", offset, PHY_QueryBBReg(pAdapter, offset, bMaskDWord))); 	
			
			offset += 0x04;
		}

		PHY_SetBBReg(pAdapter, 0xb98, bMaskDWord, 0x12680000);

		offset = 0xb68;
		for(; index < 13; index ++)			
		{
			PHY_SetBBReg(pAdapter, offset, bMaskDWord, APK_normal_setting_value_1[index]);
			//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x%x value 0x%x\n", offset, PHY_QueryBBReg(pAdapter, offset, bMaskDWord))); 	
			
			offset += 0x04;
		}	

		//page-B1
		PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x40000000);

		offset = 0xb00;
		for(index = 0; index < 16; index++)
		{
			PHY_SetBBReg(pAdapter, offset, bMaskDWord, APK_normal_setting_value_2[index]);		
			//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x%x value 0x%x\n", offset, PHY_QueryBBReg(pAdapter, offset, bMaskDWord))); 	
			
			offset += 0x04;
		}

		PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x00000000);
		
			
	}
	else
	{
		PHY_SetBBReg(pAdapter, 0xb68, bMaskDWord, 0x0fe00000);
		if(is2T)
			PHY_SetBBReg(pAdapter, 0xb68, bMaskDWord, 0x0fe00000);
	}
	
	//save BB default value													
	for(index = 0; index < APK_BB_REG_NUM ; index++)
		BB_backup[index] = PHY_QueryBBReg(pAdapter, BB_REG[index], bMaskDWord);

	//save AFE default value
	for(index = 0; index < APK_AFE_REG_NUM ; index++)
		AFE_backup[index] = PHY_QueryBBReg(pAdapter, AFE_REG[index], bMaskDWord);

	for(path = 0; path < pathbound; path++)
	{
		//save old AP curve													
		if(isNormal)
		{
			tmpReg = PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x3, bMaskDWord);
			AP_curve[path][0] = tmpReg & 0x1F;				//[4:0]

			tmpReg = PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x4, bMaskDWord);			
			AP_curve[path][1] = (tmpReg & 0xF8000) >> 15; 	//[19:15]						
			AP_curve[path][2] = (tmpReg & 0x7C00) >> 10;	//[14:10]
			AP_curve[path][3] = (tmpReg & 0x3E0) >> 5;		//[9:5]			
		}
		else
		{
			tmpReg = PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xe, bMaskDWord);
		
			AP_curve[path][0] = (tmpReg & 0xF8000) >> 15; 	//[19:15]			
			AP_curve[path][1] = (tmpReg & 0x7C00) >> 10;	//[14:10]
			AP_curve[path][2] = (tmpReg & 0x3E0) >> 5;		//[9:5]
			AP_curve[path][3] = tmpReg & 0x1F;				//[4:0]
		}
		
		//save RF default value
		regD[path] = PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xd, bMaskDWord);
		
		//Path A AFE all on, path B AFE All off or vise versa
		for(index = 0; index < APK_AFE_REG_NUM ; index++)
			PHY_SetBBReg(pAdapter, AFE_REG[index], bMaskDWord, AFE_on_off[path]);
		//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0xe70 %x\n", PHY_QueryBBReg(pAdapter, 0xe70, bMaskDWord)));		

		//BB to AP mode
		if(path == 0)
		{
			for(index = 0; index < APK_BB_REG_NUM ; index++)
				PHY_SetBBReg(pAdapter, BB_REG[index], bMaskDWord, BB_AP_MODE[index]);
		}

		//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x800 %x\n", PHY_QueryBBReg(pAdapter, 0x800, bMaskDWord)));				
		
		if(path == 0)	//Path B to standby mode
			PHY_SetRFReg(pAdapter, RF90_PATH_B, 0x0, bMaskDWord, 0x10000);			
		else			//Path A to standby mode
			PHY_SetRFReg(pAdapter, RF90_PATH_A, 0x0, bMaskDWord, 0x10000);			

		delta_offset = ((delta+14)/2);
		if(delta_offset < 0)
			delta_offset = 0;
		else if (delta_offset > 12)
			delta_offset = 12;
			
		//AP calibration
		for(index = 0; index < APK_BB_REG_NUM; index++)
		{
	
			if(index == 0 && isNormal)		//skip 
				continue;
					
			tmpReg = APK_RF_init_value[path][index];
#if 1			
			if(!pHalData->bAPKThermalMeterIgnore)
			{
				BB_offset = (tmpReg & 0xF0000) >> 16;

				if(!(tmpReg & BIT15)) //sign bit 0
				{
					BB_offset = -BB_offset;
				}

				delta_V = APK_delta_mapping[index][delta_offset];
				
				BB_offset += delta_V;

				//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() APK num %d delta_V %d delta_offset %d\n", index, delta_V, delta_offset));		
				
				if(BB_offset < 0)
				{
					tmpReg = tmpReg & (~BIT15);
					BB_offset = -BB_offset;
				}
				else
				{
					tmpReg = tmpReg | BIT15;
				}
				tmpReg = (tmpReg & 0xFFF0FFFF) | (BB_offset << 16);
			}
#endif
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x0, bMaskDWord, APK_RF_value_0[path][index]);
			//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x0 %x\n", PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x0, bMaskDWord)));		
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xd, bMaskDWord, tmpReg);
			//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0xd %x\n", PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xd, bMaskDWord)));					
			if(!isNormal)
			{
				PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xa, bMaskDWord, APK_RF_value_A[path][index]);
				//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0xa %x\n", PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xa, bMaskDWord)));					
			}
			
			// PA11+PAD01111, one shot	
			i = 0;
			do
			{
				PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x80000000);
//				if(index == 4)
				{
					PHY_SetBBReg(pAdapter, APK_offset[path], bMaskDWord, APK_value[0]);		
					//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x%x value 0x%x\n", APK_offset[path], PHY_QueryBBReg(pAdapter, APK_offset[path], bMaskDWord)));
					mdelay_os(3);				
					PHY_SetBBReg(pAdapter, APK_offset[path], bMaskDWord, APK_value[1]);
					//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0x%x value 0x%x\n", APK_offset[path], PHY_QueryBBReg(pAdapter, APK_offset[path], bMaskDWord)));
					if(isNormal)
					    mdelay_os(20);
					else
					    mdelay_os(3);
				}
				PHY_SetBBReg(pAdapter, 0xe28, bMaskDWord, 0x00000000);
				tmpReg = PHY_QueryRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xb, bMaskDWord);
				//RTPRINT(FINIT, INIT_IQK, ("PHY_APCalibrate() offset 0xb %x\n", tmpReg));		
				
				tmpReg = (tmpReg & 0x3E00) >> 9;
				i++;
			}
			while(tmpReg > 12 && i < 4);

			APK_result[path][index] = tmpReg;
		}
	}
	
	//reload BB default value	
	for(index = 0; index < APK_BB_REG_NUM ; index++)
		PHY_SetBBReg(pAdapter, BB_REG[index], bMaskDWord, BB_backup[index]);

	//reload AFE default value
	for(index = 0; index < APK_AFE_REG_NUM ; index++)
		PHY_SetBBReg(pAdapter, AFE_REG[index], bMaskDWord, AFE_backup[index]);

	//reload RF path default value
	for(path = 0; path < pathbound; path++)
	{
		PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xd, bMaskDWord, regD[path]);

#if 0
		for(index = 0; index < APK_BB_REG_NUM ; index++)
		{
			if(APK_result[path][index] > 12)
				APK_result[path][index] = AP_curve[path][index];
				
			RTPRINT(FINIT, INIT_IQK, ("apk result %d 0x%x \t", index, APK_result[path][index]));
		}
#endif
		if(APK_result[path][1] < 1 || APK_result[path][1] > 5)
			APK_result[path][1] = AP_curve[path][0];
		//RTPRINT(FINIT, INIT_IQK, ("apk result %d 0x%x \t", 1, APK_result[path][1]));			

		if(APK_result[path][2] < 2 || APK_result[path][2] > 6)
			APK_result[path][2] = AP_curve[path][1];
		//RTPRINT(FINIT, INIT_IQK, ("apk result %d 0x%x \t", 2, APK_result[path][2]));			

		if(APK_result[path][3] < 2 || APK_result[path][3] > 6)
			APK_result[path][3] = AP_curve[path][2];
		//RTPRINT(FINIT, INIT_IQK, ("apk result %d 0x%x \t", 3, APK_result[path][3]));			

		if(APK_result[path][4] < 5 || APK_result[path][4] > 9)
			APK_result[path][4] = AP_curve[path][3];
		//RTPRINT(FINIT, INIT_IQK, ("apk result %d 0x%x \t", 4, APK_result[path][4]));			
		
		
		
	}

	//RTPRINT(FINIT, INIT_IQK, ("\n"));
	

	for(path = 0; path < pathbound; path++)
	{
		if(isNormal)
		{
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x3, bMaskDWord, 
			((APK_result[path][1] << 15) | (APK_result[path][1] << 10) | (APK_result[path][1] << 5) | APK_result[path][1]));
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0x4, bMaskDWord, 
			((APK_result[path][2] << 15) | (APK_result[path][3] << 10) | (APK_result[path][4] << 5) | APK_result[path][4]));		
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xe, bMaskDWord, 
			((APK_result[path][4] << 15) | (APK_result[path][4] << 10) | (APK_result[path][4] << 5) | APK_result[path][4]));			
		}
		else
		{
			for(index = 0; index < 2; index++)
				pHalData->APKoutput[path][index] = ((APK_result[path][index] << 15) | (APK_result[path][2] << 10) | (APK_result[path][3] << 5) | APK_result[path][4]);

#if (MP_DRIVER == 1)
			if(pMptCtx->TxPwrLevel[path] > pMptCtx->APK_bound[path])	
			{
				PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xe, bMaskDWord, 
				pHalData->APKoutput[path][0]);
			}
			else
			{
				PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xe, bMaskDWord, 
				pHalData->APKoutput[path][1]);		
			}
#else
			PHY_SetRFReg(pAdapter, (RF90_RADIO_PATH_E)path, 0xe, bMaskDWord, 
			pHalData->APKoutput[path][0]);
#endif
		}
	}

	pHalData->bAPKdone = _TRUE;

	//RTPRINT(FINIT, INIT_IQK, ("<==PHY_APCalibrate()\n"));
#endif		
}
	
void PHY_SetRFPath(IN	PADAPTER	pAdapter,u8 antenna)
{
//	if(is2T)
//		return;
#if 1	
	HAL_DATA_TYPE	*pHalData	= GET_HAL_DATA(pAdapter);
	PHY_SetBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, 0x300, antenna);
#ifdef CONFIG_ANTENNA_DIVERSITY
	pHalData->CurAntenna = antenna ;
#endif
	
#else
	if(!pAdapter->hw_init_completed)
	{
		PHY_SetBBReg(pAdapter, 0x4C, BIT23, 0x01);
		PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT13, 0x01);
	}
	
	if(bMain)
		PHY_SetBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, 0x300, 0x2);	
	else
		PHY_SetBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, 0x300, 0x1);		

	//RT_TRACE(COMP_OID_SET, DBG_LOUD, ("_PHY_SetRFPathSwitch 0x4C %lx, 0x878 %lx, 0x860 %lx \n", PHY_QueryBBReg(pAdapter, 0x4C, BIT23), PHY_QueryBBReg(pAdapter, 0x878, BIT13), PHY_QueryBBReg(pAdapter, 0x860, 0x300)));
#endif
}

//return value TRUE => Main; FALSE => Aux

u8 PHY_QueryRFPath(IN PADAPTER	pAdapter)
{
//	if(is2T)
//		return _TRUE;
#if 1
	return PHY_QueryBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, 0x300);		
#else
	if(!pAdapter->hw_init_completed)
	{
		PHY_SetBBReg(pAdapter, 0x4C, BIT23, 0x01);
		PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT13, 0x01);
	}

	//RT_TRACE(COMP_OID_SET, DBG_LOUD, ("_PHY_QueryRFPathSwitch 0x4C %lx, 0x878 %lx, 0x860 %lx \n", PHY_QueryBBReg(pAdapter, 0x4C, BIT23), PHY_QueryBBReg(pAdapter, 0x878, BIT13), PHY_QueryBBReg(pAdapter, 0x860, 0x300)));

	if(PHY_QueryBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, 0x300) == 0x01)
		return _TRUE;
	else 
		return _FALSE;
#endif

}


VOID
_PHY_DumpRFReg(IN	PADAPTER	pAdapter)
{
	u32 rfRegValue,rfRegOffset;

	//RTPRINT(FINIT, INIT_RF, ("PHY_DumpRFReg()====>\n"));
	
	for(rfRegOffset = 0x00;rfRegOffset<=0x30;rfRegOffset++){		
		rfRegValue = PHY_QueryRFReg(pAdapter,RF90_PATH_A, rfRegOffset, bMaskDWord);
		//RTPRINT(FINIT, INIT_RF, (" 0x%02x = 0x%08x\n",rfRegOffset,rfRegValue));
	}
	//RTPRINT(FINIT, INIT_RF, ("<===== PHY_DumpRFReg()\n"));
}

#undef IQK_ADDA_REG_NUM
#undef IQK_DELAY_TIME


VOID
PHY_IQCalibrate(
	IN	PADAPTER	pAdapter
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	int			result[4][8];	//last is final result
	u8			i, final_candidate;
	BOOLEAN			bPathAOK, bPathBOK;
	int			RegE94, RegE9C, RegEA4, RegEAC, RegEB4, RegEBC, RegEC4, RegECC, RegTmp = 0;
	BOOLEAN			is12simular, is13simular, is23simular;	
	BOOLEAN 		bStartContTx = _FALSE;
#if (MP_DRIVER == 1)
	bStartContTx = pAdapter->MptCtx.bStartContTx;
#endif

	//ignore IQK when continuous Tx
	if(bStartContTx)
		return;

#if DISABLE_BB_RF
	return;
#endif

        DBG_8192C("IQK:Start!!!\n");

	for(i = 0; i < 8; i++)
	{
		result[0][i] = 0;
		result[1][i] = 0;
		result[2][i] = 0;
		result[3][i] = 0;
	}
	final_candidate = 0xff;
	bPathAOK = _FALSE;
	bPathBOK = _FALSE;
	is12simular = _FALSE;
	is23simular = _FALSE;
	is13simular = _FALSE;

	for (i=0; i<3; i++)
	{
	 	if(IS_92C_SERIAL( pHalData->VersionID)){
			 _PHY_IQCalibrate(pAdapter, result, i, _TRUE);
	 		//_PHY_DumpRFReg(pAdapter);
	 	}
	 	else{
	 		// For 88C 1T1R
	 		_PHY_IQCalibrate(pAdapter, result, i, _FALSE);
 		}
		
		if(i == 1)
		{
			is12simular = _PHY_SimularityCompare(pAdapter, result, 0, 1);
			if(is12simular)
			{
				final_candidate = 0;
				break;
			}
		}
		
		if(i == 2)
		{
			is13simular = _PHY_SimularityCompare(pAdapter, result, 0, 2);
			if(is13simular)
			{
				final_candidate = 0;			
				break;
			}
			
			is23simular = _PHY_SimularityCompare(pAdapter, result, 1, 2);
			if(is23simular)
				final_candidate = 1;
			else
			{
				for(i = 0; i < 8; i++)
					RegTmp += result[3][i];

				if(RegTmp != 0)
					final_candidate = 3;			
				else
					final_candidate = 0xFF;
			}
		}
	}

        for (i=0; i<4; i++)
	{
		RegE94 = result[i][0];
		RegE9C = result[i][1];
		RegEA4 = result[i][2];
		RegEAC = result[i][3];
		RegEB4 = result[i][4];
		RegEBC = result[i][5];
		RegEC4 = result[i][6];
		RegECC = result[i][7];
		//RTPRINT(FINIT, INIT_IQK, ("IQK: RegE94=%lx RegE9C=%lx RegEA4=%lx RegEAC=%lx RegEB4=%lx RegEBC=%lx RegEC4=%lx RegECC=%lx\n ", RegE94, RegE9C, RegEA4, RegEAC, RegEB4, RegEBC, RegEC4, RegECC));
	}

	if(final_candidate != 0xff)
	{
		pHalData->RegE94 = RegE94 = result[final_candidate][0];
		pHalData->RegE9C = RegE9C = result[final_candidate][1];
		RegEA4 = result[final_candidate][2];
		RegEAC = result[final_candidate][3];
		pHalData->RegEB4 = RegEB4 = result[final_candidate][4];
		pHalData->RegEBC = RegEBC = result[final_candidate][5];
		RegEC4 = result[final_candidate][6];
		RegECC = result[final_candidate][7];
		
		DBG_8192C("IQK: final_candidate is %x\n", final_candidate);
		
		DBG_8192C("IQK: RegE94=%x RegE9C=%x RegEA4=%x RegEAC=%x RegEB4=%x RegEBC=%x RegEC4=%x RegECC=%x\n ", RegE94, RegE9C, RegEA4, RegEAC, RegEB4, RegEBC, RegEC4, RegECC);
		
		bPathAOK = bPathBOK = _TRUE;
	}
	else
	{
		pHalData->RegE94 = pHalData->RegEB4 = 0x100;	//X default value
		pHalData->RegE9C = pHalData->RegEBC = 0x0;		//Y default value
	}
	
	if((RegE94 != 0)/*&&(RegEA4 != 0)*/)
		_PHY_PathAFillIQKMatrix(pAdapter, bPathAOK, result, final_candidate, (RegEA4 == 0));
	
	if(IS_92C_SERIAL( pHalData->VersionID)){
		if((RegEB4 != 0)/*&&(RegEC4 != 0)*/)
		_PHY_PathBFillIQKMatrix(pAdapter, bPathBOK, result, final_candidate, (RegEC4 == 0));
	}

}


VOID
PHY_LCCalibrate(
	IN	PADAPTER	pAdapter
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	BOOLEAN 		bStartContTx = _FALSE;
#if (MP_DRIVER == 1)	
	bStartContTx = pAdapter->MptCtx.bStartContTx;
#endif

#if DISABLE_BB_RF
	return;
#endif

	//ignore IQK when continuous Tx
	if(bStartContTx)
		return;

	if(IS_92C_SERIAL( pHalData->VersionID)){
		_PHY_LCCalibrate(pAdapter, _TRUE);
	}
	else{
		// For 88C 1T1R
		_PHY_LCCalibrate(pAdapter, _FALSE);
	}
}

VOID
PHY_APCalibrate(
	IN	PADAPTER	pAdapter,
	IN	char 		delta	
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

#if DISABLE_BB_RF
	return;
#endif

	if(pHalData->bAPKdone)
		return;

//	if(IS_NORMAL_CHIP(pHalData->VersionID))
//		return;

	if(IS_92C_SERIAL( pHalData->VersionID)){
		_PHY_APCalibrate(pAdapter, delta, _TRUE);
	}
	else{
		// For 88C 1T1R
		_PHY_APCalibrate(pAdapter, delta, _FALSE);
	}
}


//
// Move from phycfg.c to gen.c to be code independent later
// 
//-------------------------Move to other DIR later----------------------------*/
#if (DEV_BUS_TYPE == USB_INTERFACE)
VOID
DebugAllRegister_92SU_FPGA(
	IN	PADAPTER			Adapter
	)
{
	static u32	sMacRegCnt = 0x37c; 
	//static u4Byte	sCckRegCnt = 95; 
	//static u4Byte	sOfdmRegCnt = 64; 
	
	u32	i;
	u32	u4bMacReg; 
	
	// MAC registers
	//RT_TRACE(COMP_FPGA, DBG_LOUD, ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
	//RT_TRACE(COMP_FPGA, DBG_LOUD, ("MAC Registers:\n"));
	for(i = 0; i < sMacRegCnt; i+=4)
	{
		if(i%16 == 0)
	{
			if(i > 0)
		{
				//RT_TRACE(COMP_FPGA, DBG_LOUD, ("\n"));
		}
			//RT_TRACE(COMP_FPGA, DBG_LOUD, ("%03lX              ", i));
			}
		u4bMacReg = read32(Adapter, i);
		//RT_TRACE(COMP_FPGA, DBG_LOUD, ("%08lX    ", u4bMacReg));
		}
	//RT_TRACE(COMP_FPGA, DBG_LOUD, ("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n"));
}

#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
//
//	Description:
// 		To dump all Tx FIFO LLT related link-list table.
//		Added by Roger, 2009.03.10.
//
VOID
DumpAllTxFIFO_92SU(
	IN	PADAPTER			Adapter
	)
{
	u8	PageIdx;
	u32	u4bFIFOReg;	
	u8	PktOffset, QueueSel;
	
	if(Adapter->DumpAllTxFIFO)
	{
		//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("\nDump all Tx FIFO Reg Section1:\n"));
		for(PageIdx=0; PageIdx<0xff; PageIdx++)
		{
			write32(Adapter, 0x2b0, 0xb0280000+PageIdx*8);
			u4bFIFOReg = read32(Adapter, 0x2b4);
			//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("%#x:\t%08x\n", PageIdx, u4bFIFOReg));
		}

		//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("\nDump all Tx FIFO Reg Section2:\n"));
		for(PageIdx=0; PageIdx<0xff; PageIdx++)
		{
			write32(Adapter, 0x2b0, 0xb0270000+PageIdx*8);
			u4bFIFOReg = read32(Adapter, 0x2b4);
			//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("%#x:\t%08x\n", PageIdx, u4bFIFOReg));
		}

		//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("\nDump Specific Data section:\n"));
		for(PageIdx=0; PageIdx<0xff; PageIdx++)
		{
			write16(Adapter, 0x348, 0x2000+PageIdx*0x20);
			u4bFIFOReg = read32(Adapter, 0x340);
			PktOffset = (u8)((u4bFIFOReg & 0x00ff0000)>>16);			
			u4bFIFOReg = read32(Adapter, 0x344);
			QueueSel = (u8)((u4bFIFOReg & 0x00001f00)>>8);
			if((PktOffset == 0x20) && (QueueSel == 0x13))
			{
				//RT_TRACE(COMP_TXAGG, DBG_SERIOUS, ("PageIdx(%#x) contain PktOffset0x20 with QueueSel0x13!!\n", PageIdx));
			}
		}		
		Adapter->DumpAllTxFIFO = FALSE;
	}
}
#endif



//
//	Description:
// 		To dump all Tx FIFO LLT related link-list table.
//		Added by Roger, 2009.03.10.
//
VOID
DumpBBDbgPort_92CU(
	IN	PADAPTER			Adapter
	)
{

	//RT_TRACE(COMP_SEND, DBG_WARNING, ("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n"));
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("BaseBand Debug Ports:\n"));
	
	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0000);
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));
	
	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0803);
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));
	
	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0a06);
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));

	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0007);
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));

	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0100);
	PHY_SetBBReg(Adapter, 0x0a28, 0x00ff0000, 0x000f0000);	
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));

	PHY_SetBBReg(Adapter, 0x0908, 0xffff, 0x0100);
	PHY_SetBBReg(Adapter, 0x0a28, 0x00ff0000, 0x00150000);	
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xdf4, PHY_QueryBBReg(Adapter, 0x0df4, bMaskDWord)));

	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0x800, PHY_QueryBBReg(Adapter, 0x0800, bMaskDWord)));
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0x900, PHY_QueryBBReg(Adapter, 0x0900, bMaskDWord)));
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xa00, PHY_QueryBBReg(Adapter, 0x0a00, bMaskDWord)));
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xa54, PHY_QueryBBReg(Adapter, 0x0a54, bMaskDWord)));
	//RT_TRACE(COMP_SEND, DBG_WARNING, ("Offset[%x]: %x\n", 0xa58, PHY_QueryBBReg(Adapter, 0x0a58, bMaskDWord)));

}



VOID
RtUsbCheckForHangWorkItemCallback(
                IN void*   pContext
)
{
#if 0
	PADAPTER	  Adapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

#if (HAL_CODE_BASE!=RTL8192_C)
	RESET_TYPE	ResetType = RESET_TYPE_NORESET;
	static u1Byte	check_reset_cnt=0;
#endif

	RT_TRACE(COMP_INIT, DBG_LOUD, (" ==>RtUsbCheckForHangWorkItemCallback()\n"));
	
	// <Roger_Notes> To check whether we should put out device into D2.
	if(PlatformUsbChkSelectiveSuspend(Adapter))
		Adapter->HalFunc.HalEnterSSHandler(Adapter); 

	NicIFLinkStatusWatchdog(Adapter);

	//
	// <Roger_Notes> Schedule another system worker thread to perform Tx/Rx stuck checking.
	// to prevent driver running out of kernel-mode stack. 2009.07.14.
	//
#if (HAL_CODE_BASE==RTL8192_C)

#if (SILENT_RESET && (DEV_BUS_TYPE != USB_INTERFACE))
	if((!Adapter->bDriverStopped) && (!Adapter->bSurpriseRemoved))	
		PlatformScheduleWorkItem(&(pHalData->RtUsbCheckResetWorkItem));
#endif

#else
	//check if reset the driver
	// Give a chance check and indicate whether ap is disapeared(driver activate roam first), 2008.07.17 ,Lanhsin
	//if((check_reset_cnt++>=Reset_Cnt_Limit) && (!MgntRoamingInProgress(pMgntInfo)))
	if((check_reset_cnt++>=Reset_Cnt_Limit))
	{
    		ResetType = NicIFCheckResetOrNot(Adapter);
		check_reset_cnt = Reset_Cnt_Limit;
	}

	// Silent Reset for 8192
	if( (Adapter->ResetProgress==RESET_TYPE_NORESET) &&
	     (!Adapter->bResetInProgress) &&
	     (Adapter->bForcedSilentReset || (ResetType==RESET_TYPE_SILENT))) // This is control by OID set in Pomelo
	{
		Adapter->bResetInProgress = TRUE;
	
		// <Roger_Notes> Export necessary BaseBand debug port to dump specific information. 2009.05.25.
		DumpBBDbgPort_92CU(Adapter);	
	
#if (SILENT_RESET==1)       
		NicIFSilentReset(Adapter);
#else
		//Cancel All Pending In Irp
		PlatformUsbDisableInPipes(Adapter);

		 //Cancel All Pending Out Irp
		PlatformUsbDisableOutPipes(Adapter);

		//wait for complet all pending irp, the value should varies with nic and driver
		PlatformStallExecution(1000);

		// Re-Open Out Pipes.
		// This shall be done before FW download.
		PlatformUsbEnableOutPipes(Adapter);

		// Reset Tx/Rx data structure in driver.
		NicIFResetMemory(Adapter);
#endif
	}
        Adapter->bForcedSilentReset = FALSE;
	Adapter->bResetInProgress = FALSE;
#endif

	RT_TRACE(COMP_INIT, DBG_LOUD, (" <==RtUsbCheckForHangWorkItemCallback()\n"));
#endif
}


//
//	Description:
// 		To check whether we shall reset out NIC immediately. We schedule 
//		another system worker thread to prevent driver running out of kernel-mode stack.
//		Added by Roger, 2009.07.14.
//
VOID
RtUsbCheckResetWorkItemCallback(
                IN void*   pContext
)
{
#if 0
	PADAPTER	  Adapter = (PADAPTER)pContext;
	RESET_TYPE	ResetType = RESET_TYPE_NORESET;
      	PMGNT_INFO      pMgntInfo = &Adapter->MgntInfo;
	static u1Byte	check_reset_cnt=0;
	
	RT_TRACE(COMP_RESET, DBG_TRACE, (" ==>RtUsbCheckResetWorkItemCallback()\n"));
	
	if(Adapter->bDriverStopped || Adapter->bSurpriseRemoved)
		return;
	
#ifdef USB_TX_DRIVER_AGGREGATION_ENABLE
	//vivi temply mask, 20090811
	DumpAllTxFIFO_92SU(Adapter);	
#endif

	// Post-Processing Selective Suspend leave.
	if(PlatformUsbSelectiveSuspendLeavePostProc(Adapter))
	{
		ResetType = RESET_TYPE_SILENT;		
		RT_TRACE(COMP_RESET|COMP_POWER, DBG_TRACE, ("RtUsbCheckResetWorkItemCallback(): Force reset!!\n"));
	}	
	else if((check_reset_cnt++>=Reset_Cnt_Limit))
	{
    		ResetType = NicIFCheckResetOrNot(Adapter);
		check_reset_cnt = Reset_Cnt_Limit;
	}


	// Silent Reset for 8192
	if( (Adapter->ResetProgress==RESET_TYPE_NORESET) &&
	     (!Adapter->bResetInProgress) &&
	     (Adapter->bForcedSilentReset || (ResetType==RESET_TYPE_SILENT))) // This is control by OID set in Pomelo
	{
		Adapter->bResetInProgress = TRUE;
	
		RT_TRACE(COMP_RESET, DBG_WARNING, ("RtUsbCheckResetWorkItemCallback(): Reset in progress...\n"));
		
		// <Roger_Notes> Export necessary BaseBand debug port to dump specific information. 2009.05.25.
		//DumpBBDbgPort_92CU(Adapter);	
	
#if (SILENT_RESET==1)       
		NicIFSilentReset(Adapter);
#else
		//Cancel All Pending In Irp
		PlatformUsbDisableInPipes(Adapter);

		 //Cancel All Pending Out Irp
		PlatformUsbDisableOutPipes(Adapter);

		//wait for complet all pending irp, the value should varies with nic and driver
		PlatformStallExecution(1000);

		// Re-Open Out Pipes.
		// This shall be done before FW download.
		PlatformUsbEnableOutPipes(Adapter);

		// Reset Tx/Rx data structure in driver.
		NicIFResetMemory(Adapter);
#endif
	}

        Adapter->bForcedSilentReset = FALSE;
	Adapter->bResetInProgress = FALSE;

	RT_TRACE(COMP_RESET, DBG_TRACE, (" <==RtUsbCheckResetWorkItemCallback()\n"));
#endif
}

//
// Callback routine of the work item for set bandwidth mode.
//
VOID
SetBWModeCallback8192CUsbWorkItem(
    IN void*            pContext
    )
{
	PADAPTER		Adapter = (PADAPTER)pContext;

	_PHY_SetBWMode92C(Adapter);
}

#if 0
VOID
SwChnlCallback8192CUsb(
	IN	PRT_TIMER		pTimer
	)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte			delay;

	RT_TRACE(COMP_SCAN, DBG_LOUD, ("==>SwChnlCallback8190Pci(), switch to channel\
				%d\n", pHalData->CurrentChannel));
	
	if(Adapter->bDriverStopped)
		return;
	
	if(pHalData->RFChipID == RF_PSEUDO_11N)
	{
		pHalData->SwChnlInProgress=FALSE;
		return; 								//return immediately if it is peudo-phy	
	}
	
	do{
		if(!pHalData->SwChnlInProgress)
			break;

		if(!phy_SwChnlStepByStep(Adapter, pHalData->CurrentChannel, &pHalData->SwChnlStage, &pHalData->SwChnlStep, &delay))
		{
			if(delay>0)
			{
				//PlatformSetTimer(Adapter, &pHalData->SwChnlTimer, delay);
			}
			else
			continue;
		}
		else
		{
			//pHalData->SwChnlInProgress=FALSE;
		}
		break;
	}while(_TRUE);
}
#endif

//
// Callback routine of the work item for switch channel.
//
VOID
SwChnlCallback8192CUsbWorkItem(
    IN void*            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	//RT_TRACE(COMP_TRACE, DBG_LOUD, ("==> SwChnlCallback8192CUsbWorkItem()\n"));

	//if(pAdapter->bInSetPower && RT_USB_CANNOT_IO(pAdapter))
	//{
		//RT_TRACE(COMP_SCAN, DBG_LOUD, ("<== SwChnlCallback8192CUsbWorkItem() SwChnlInProgress FALSE driver sleep or unload\n"));
	
		//pHalData->SwChnlInProgress = FALSE;		
	//	return;
	//}

	phy_FinishSwChnlNow(pAdapter, pHalData->CurrentChannel);
	//pHalData->SwChnlInProgress = FALSE;
	
	//RT_TRACE(COMP_TRACE, DBG_LOUD, ("<== SwChnlCallback8192CUsbWorkItem()\n"));
}

#endif

#if 0
BOOLEAN
HalSetIO8192C(
	IN	PADAPTER			pAdapter,
	IN	IO_TYPE				IOType
)
{
	PADAPTER		Adapter = ADJUST_TO_ADAPTIVE_ADAPTER(pAdapter, TRUE);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bPostProcessing = FALSE;

	if(RT_USB_CANNOT_IO(pAdapter))
		return TRUE;	

	RT_TRACE(COMP_CMD, DBG_LOUD, ("-->HalSetIO8192C(): Set IO Cmd(%#x), SetIOInProgress(%d)\n", 
		IOType, pHalData->SetIOInProgress));

	do{
		switch(IOType)
		{
			case IO_CMD_RESUME_DM_BY_SCAN:
				RT_TRACE(COMP_CMD, DBG_LOUD, ("[IO CMD] Resume DM after scan.\n"));
				bPostProcessing = TRUE;
				break;
			case IO_CMD_PAUSE_DM_BY_SCAN:
				RT_TRACE(COMP_CMD, DBG_LOUD, ("[IO CMD] Pause DM before scan.\n"));
				bPostProcessing = TRUE;
				break;
			default:				
				break;
		}
	}while(FALSE);

	if(bPostProcessing && !pHalData->SetIOInProgress)
	{
		if(RT_USB_CANNOT_IO(Adapter))
		{
			RT_TRACE(COMP_CMD, DBG_WARNING, ("HalSetIO8192S(): USB can NOT IO!!\n"));
			return FALSE;
		}
		pHalData->SetIOInProgress = TRUE;
		pHalData->CurrentIOType = IOType; 
	}
	else
	{
		return FALSE;
	}

#ifdef USE_WORKITEM			
	PlatformScheduleWorkItem(&(pHalData->IOWorkItem));
#else
	PlatformSetTimer(Adapter, &(pHalData->SetIOTimer), 0);
#endif
	RT_TRACE(COMP_CMD, DBG_LOUD, ("<--HalSetIO8192C(): Set IO Type(%#x)\n", IOType));

	return TRUE;
}

VOID
phy_SetIO(
    PADAPTER		pAdapter
    )
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PMGNT_INFO	pMgntInfo = &pAdapter->MgntInfo;
	
	if(RT_USB_CANNOT_IO(pAdapter) || pAdapter->bResetInProgress)	
	{			
		RT_TRACE(COMP_CMD, DBG_WARNING, ("phy_SetIO(): USB can NOT IO return\n"));
		pHalData->SetIOInProgress = FALSE;
		return;
	}

	RT_TRACE(COMP_CMD, DBG_LOUD, ("--->phy_SetIO(): Cmd(%#x), SetIOInProgress(%d)\n", 
			pHalData->CurrentIOType, pHalData->SetIOInProgress));
			
	switch(pHalData->CurrentIOType)
	{
		case IO_CMD_RESUME_DM_BY_SCAN:
			DM_DigTable.CurIGValue = pMgntInfo->InitGain_Backup.XAAGCCore1;
			DM_Write_DIG(pAdapter);
			pAdapter->HalFunc.SetTxPowerLevelHandler(pAdapter, pHalData->CurrentChannel);
			break;		
		case IO_CMD_PAUSE_DM_BY_SCAN:
			pMgntInfo->InitGain_Backup.XAAGCCore1 = DM_DigTable.CurIGValue;
			DM_DigTable.CurIGValue = 0x17;
			DM_Write_DIG(pAdapter);
			break;
		default:
			break;
	}	
	pHalData->SetIOInProgress = FALSE;// Clear FW CMD operation flag.
	RT_TRACE(COMP_CMD, DBG_LOUD, ("<---phy_SetIO(): CurrentIOType(%#x)\n", pHalData->CurrentIOType));
	
}			

#ifdef USE_WORKITEM
VOID
SetIOWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	if(RT_USB_CANNOT_IO(pAdapter) || pAdapter->bResetInProgress)
	{
		RT_TRACE(COMP_CMD, DBG_WARNING, ("SetIOWorkItemCallback(): USB can NOT IO return\n"));
		pHalData->SetIOInProgress = FALSE;
		return;
	}
	phy_SetIO(pAdapter);
}
			
#else

//
// 	Callback routine of the timer callback for FW Cmd IO.
//
//	Description:
//		This routine will send specific CMD IO to FW and check whether it is done.
//
VOID
SetIOTimerCallback(
   IN		PRT_TIMER		pTimer
   )
{
	PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;

	if(Adapter->bDriverStopped)
	{
		RT_TRACE(COMP_RATE, DBG_TRACE, ("SetIOTimerCallback(): driver is going to unload\n"));
		return;
	}
	phy_SetIO(Adapter);	
}

//
// 2008/12/26 MH When we switch EPHY parameter. We must delay to wait ready bit.
// We wait at most 1000*100 us.
//
static	VOID
phy_CheckEphySwitchReady(
	IN	PADAPTER			Adapter
	)
{
	u4Byte	delay = 1000;
	u1Byte	regu1;

	regu1 = PlatformEFIORead1Byte(Adapter, 0x554);
	while ((regu1 & BIT5) && (delay > 0))
	{
		regu1 = PlatformEFIORead1Byte(Adapter, 0x554);
		delay--;
		delay_us(100);
	}	
	RT_TRACE(COMP_INIT, DBG_LOUD, ("regu1=%02x  delay = %ld\n", regu1, delay));	
	
}	// phy_CheckEphySwitchReady

#endif

BOOLEAN
SetAntennaConfig92C(
	IN	PADAPTER		pAdapter,
	IN	u1Byte			DefaultAnt		// 0: Main, 1: Aux.
)
{
	HAL_DATA_TYPE			*pHalData	= GET_HAL_DATA(pAdapter);
	PMGNT_INFO				pMgntInfo = &(pAdapter->MgntInfo);
	u4Byte					ulAntennaTx, ulAntennaRx;
	R_ANTENNA_SELECT_OFDM	*p_ofdm_tx;	/* OFDM Tx register */
	R_ANTENNA_SELECT_CCK	*p_cck_txrx;
	u1Byte					r_rx_antenna_ofdm=0, r_ant_select_cck_val=0;
	u1Byte					chgTx=0, chgRx=0;
	u4Byte					r_ant_sel_cck_val=0, r_ant_select_ofdm_val=0, r_ofdm_tx_en_val=0;
	u1Byte					rfChg = 0;
	static	u1Byte			TestStart=0;


	if(!TestStart)
	{
		TestStart = 1;
		rfChg = 1;
	}

	pMgntInfo->AntennaTest = 1;
	pHalData->AntennaTxPath = ANTENNA_A;
	if(DefaultAnt == 0)	//ANTENNA_A
	{
		if(pHalData->AntennaRxPath == ANTENNA_B)	//from B switch to A
			rfChg = 1;
		pHalData->AntennaRxPath = ANTENNA_A;
	}
	else
	{
		if(pHalData->AntennaRxPath == ANTENNA_A)	//from A switch to B
			rfChg = 1;
		pHalData->AntennaRxPath = ANTENNA_B;
	}

	ulAntennaTx = pHalData->AntennaTxPath;
	ulAntennaRx = pHalData->AntennaRxPath;
	
	p_ofdm_tx = (R_ANTENNA_SELECT_OFDM *)&r_ant_select_ofdm_val;
	p_cck_txrx = (R_ANTENNA_SELECT_CCK *)&r_ant_select_cck_val;

	
	p_ofdm_tx->r_ant_ht1			= 0x1;
	p_ofdm_tx->r_ant_ht2			= 0x2;	// Second TX RF path is A
	p_ofdm_tx->r_ant_non_ht 		= 0x3;	// 0x1+0x2=0x3

	// ��]�OTx 3-wire enable���HTx Ant path���}�~�|�}�ҡA
	// �ҥH�ݦb�]BB 0x824�P0x82C�ɡA�P�ɱNBB 0x804[3:0]�]��3(�P�ɥ��}Ant. A and B)�C
	// �n�ٹq�����p�U�AA Tx�� 0x90C=0x11111111�AB Tx�� 0x90C=0x22222222�AAB�P�ɶ}�N������ӳ]�w0x3321333
	
	switch(ulAntennaTx)
	{
	case ANTENNA_A:
		p_ofdm_tx->r_tx_antenna		= 0x1;
		r_ofdm_tx_en_val			= 0x1;
		p_ofdm_tx->r_ant_l 			= 0x1;
		p_ofdm_tx->r_ant_ht_s1 		= 0x1;
		p_ofdm_tx->r_ant_non_ht_s1 	= 0x1;
		p_cck_txrx->r_ccktx_enable	= 0x8;
		chgTx = 1;
		// From SD3 Willis suggestion !!! Set RF A=TX and B as standby
		if (IS_HARDWARE_TYPE_8192S(pAdapter))
		{
			PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
			PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 1);
			r_ofdm_tx_en_val			= 0x3;
			// Power save
			r_ant_select_ofdm_val = 0x11111111;

			// 2009/01/08 MH From Sd3 Willis. We need to close RFB by SW control
			if (pHalData->RF_Type == RF_2T2R)
			{
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 0);
			}
		}
		break;
	case ANTENNA_B:
		p_ofdm_tx->r_tx_antenna		= 0x2;
		r_ofdm_tx_en_val			= 0x2;
		p_ofdm_tx->r_ant_l 			= 0x2;	
		p_ofdm_tx->r_ant_ht_s1 		= 0x2;	
		p_ofdm_tx->r_ant_non_ht_s1 	= 0x2;
		p_cck_txrx->r_ccktx_enable	= 0x4;
		chgTx = 1;
		// From SD3 Willis suggestion !!! Set RF A as standby
		if (IS_HARDWARE_TYPE_8192S(pAdapter))
		{
			PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 1);
			PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);
			r_ofdm_tx_en_val			= 0x3;
			// Power save
			r_ant_select_ofdm_val = 0x22222222;

			// 2008/10/31 MH From SD3 Willi's suggestion. We must read RF 1T table.
			// 2009/01/08 MH From Sd3 Willis. We need to close RFA by SW control
			if (pHalData->RF_Type == RF_2T2R)
			{
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XA_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				//PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
		}
		break;

	case ANTENNA_AB:	// For 8192S
		p_ofdm_tx->r_tx_antenna		= 0x3;
		r_ofdm_tx_en_val			= 0x3;
		p_ofdm_tx->r_ant_l 			= 0x3;
		p_ofdm_tx->r_ant_ht_s1 		= 0x3;
		p_ofdm_tx->r_ant_non_ht_s1 	= 0x3;
		p_cck_txrx->r_ccktx_enable	= 0xC;
		chgTx = 1;
		// From SD3 Willis suggestion !!! Set RF B as standby
		if (IS_HARDWARE_TYPE_8192S(pAdapter))
		{
			PHY_SetBBReg(pAdapter, rFPGA0_XA_HSSIParameter2, 0xe, 2);
			PHY_SetBBReg(pAdapter, rFPGA0_XB_HSSIParameter2, 0xe, 2);
			// Disable Power save
			r_ant_select_ofdm_val = 0x3321333;
			
			// 2009/01/08 MH From Sd3 Willis. We need to enable RFA/B by SW control
			if (pHalData->RF_Type == RF_2T2R)
			{
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT10, 0);

				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFInterfaceSW, BIT26, 0);
				//PHY_SetBBReg(pAdapter, rFPGA0_XB_RFInterfaceOE, BIT10, 0);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT1, 1);
				PHY_SetBBReg(pAdapter, rFPGA0_XAB_RFParameter, BIT17, 1);
			}
		}
		break;
			default:
				break;
	}
	
	//
	// r_rx_antenna_ofdm, bit0=A, bit1=B, bit2=C, bit3=D
	// r_cckrx_enable : CCK default, 0=A, 1=B, 2=C, 3=D
	// r_cckrx_enable_2 : CCK option, 0=A, 1=B, 2=C, 3=D
	//
	switch(ulAntennaRx)
	{
	case ANTENNA_A:
		r_rx_antenna_ofdm 			= 0x1;	// A
		p_cck_txrx->r_cckrx_enable 	= 0x0;	// default: A
		p_cck_txrx->r_cckrx_enable_2	= 0x0;	// option: A
		chgRx = 1;
		break;
	case ANTENNA_B:
		r_rx_antenna_ofdm 			= 0x2;	// B
		p_cck_txrx->r_cckrx_enable 	= 0x1;	// default: B
		p_cck_txrx->r_cckrx_enable_2	= 0x1;	// option: B
		chgRx = 1;
		break;
	case ANTENNA_AB:	// For 8192S and 8192E/U...
		r_rx_antenna_ofdm 			= 0x3;	// AB
		p_cck_txrx->r_cckrx_enable 	= 0x0;	// default:A
		p_cck_txrx->r_cckrx_enable_2= 0x1;		// option:B
		chgRx = 1;
		break;
	default:
		break;
	}
	//
	// For 8192S later not make sure!!!! If we enable RF A only, we must set RF B reg 0x00
	// as 0x10159 from SD3 suggestion!!! Not make sure the reason!!
	//

	//==================================================
	
	if(chgTx && chgRx)
	{
		switch(pHalData->RFChipID)
		{
			case RF_8225:
			case RF_8256:
			case RF_6052:
				r_ant_sel_cck_val = r_ant_select_cck_val;
				if(rfChg)
				{
					//DbgPrint("Write RF-path register!!\n");
					PHY_SetBBReg(pAdapter, rFPGA1_TxInfo, 0xffffffff, r_ant_select_ofdm_val);		//OFDM Tx
					PHY_SetBBReg(pAdapter, rFPGA0_TxInfo, 0x0000000f, r_ofdm_tx_en_val);		//OFDM Tx
					PHY_SetBBReg(pAdapter, rOFDM0_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	//OFDM Rx
					PHY_SetBBReg(pAdapter, rOFDM1_TRxPathEnable, 0x0000000f, r_rx_antenna_ofdm);	//OFDM Rx
					PHY_SetBBReg(pAdapter, rCCK0_AFESetting, bMaskByte3, r_ant_sel_cck_val);		//CCK TxRx
				}
				break;
			default:
				RT_ASSERT(FALSE, ("Unsupported RFChipID for switching antenna.\n"));
				break;
		}
	}
	//DbgPrint("\n MPT_ProSwitchAntenna() is finished \n");
	return TRUE;
}

VOID
FillA2Entry8192C(
	IN	PADAPTER			Adapter,
	IN	u1Byte				index,
	IN	pu1Byte				val
)
{
	u4Byte				A2entry_index;
	
	PlatformEFIOWrite4Byte(Adapter, 0x2c4, ((pu4Byte)(val))[0]);
	PlatformEFIOWrite2Byte(Adapter, 0x2c8, ((pu2Byte)(val+4))[0]);

	A2entry_index = (u4Byte)index;
	//PlatformEFIOWrite4Byte(Adapter, WFM5, (FW_ADD_A2_ENTRY | (A2entry_index & 0xff)<<8)); 
	Adapter->HalFunc.SetFwCmdHandler(Adapter, FW_CMD_ADD_A2_ENTRY);
	RT_PRINT_ADDR(COMP_CMD, DBG_LOUD, ("Add A2 Entry:\n"), val);
}


//
// 2009/11/03 MH add for LPS mode power save sequence.
// 2009/11/03 According to document V10.
// 2009/11/24 According to document V11. by tynli.
//
VOID
phy_SetRTL8192CERfSleep(
	IN	PADAPTER			Adapter
)
{
	u4Byte	U4bTmp;
	u1Byte	delay = 5; //5: suggested by Scott.

	// a.	TXPAUSE 0x522[7:0] = 0xFF				//Pause MAC TX queue
	PlatformEFIOWrite1Byte(Adapter, REG_TXPAUSE, 0xFF);

	// b.	RF path 0 offset 0x00 = 0x00					// disable RF
	PHY_SetRFReg(Adapter, RF90_PATH_A, 0x00, 0xFF, 0x00);

	// c.	APSD_CTRL 0x600[7:0] = 0x40
	PlatformEFIOWrite1Byte(Adapter, REG_APSD_CTRL, 0x40);

	// d.	While( RF_PATH_A_REG_00 != 0) {
	//	APSD_CTRL 0x600[7:0] = 0x00
	//	RF path 0 offset 0x00 = 0x00
	//	APSD_CTRL 0x600[7:0] = 0x40
	//}
	// Added by tynli. 2009.11.24. Reference from Scott document v10.
	U4bTmp = PHY_QueryRFReg(Adapter, RF90_PATH_A, 0 ,bRFRegOffsetMask);
	while(U4bTmp != 0 && delay > 0 )
	{	
		PlatformEFIOWrite1Byte(Adapter, REG_APSD_CTRL, 0x0);
		PHY_SetRFReg(Adapter, RF90_PATH_A, 0x00, 0xFF, 0x00);
		PlatformEFIOWrite1Byte(Adapter, REG_APSD_CTRL, 0x40);
		U4bTmp = PHY_QueryRFReg(Adapter, RF90_PATH_A, 0 ,bRFRegOffsetMask);
		delay--;
	}

	if(delay == 0)
	{
		//Jump out the LPS turn off sequence to RF_ON_EXCEP
		PlatformEFIOWrite1Byte(Adapter, REG_APSD_CTRL, 0x00);
	
#if (DEV_BUS_TYPE == PCI_INTERFACE)
		PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE2);
		PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE3);
#else
		PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x16);
		PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x17);
#endif
		PlatformEFIOWrite1Byte(Adapter, REG_TXPAUSE, 0x00);
		RT_TRACE(COMP_POWER, DBG_LOUD, ("phy_SetRTL8192CERfSleep(): Fail !!! Switch RF timeout.\n"));
		return;
	}

	// e.	For PCIE: SYS_FUNC_EN 0x02[7:0] = 0xE2	//reset BB TRX function
#if (DEV_BUS_TYPE == PCI_INTERFACE)
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE2);
#else
	//e. For USB: SYS_FUNC_EN 0x02[7:0] = 0x16	//reset BB TRX function
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x16);
#endif

	// f.	SPS_CTRL 0x11[7:0] = 0x22
	PlatformEFIOWrite1Byte(Adapter, REG_SPS0_CTRL, 0x22);

	//g.	//SYS_CLKR 0x08[11] = 0					//gated MAC clock
	//PlatformEFIOWrite2Byte(Adapter, 0x08, PlatformEFIORead2Byte(Adapter, 0x08)&~BIT11);

	
}	// phy_SetRTL8192CERfSleep


//
// 2009/11/03 MH add for LPS mode power save sequence.
// 2009/11/03 According to document V10.
// 2009/11/24 According to document V11. by tynli.
//
VOID
phy_SetRTL8192CERfOn(
	IN	PADAPTER			Adapter
)
{
	// a.	//SYS_CLKR 0x08[11] = 1					//restore MAC clock
	//PlatformEFIOWrite2Byte(Adapter, 0x08, PlatformEFIORead2Byte(Adapter, 0x08)|BIT11);

	// b.	SPS_CTRL 0x11[7:0] = 0x2b
	PlatformEFIOWrite1Byte(Adapter, REG_SPS0_CTRL, 0x2b);
	//PHY_SetRFReg(Adapter, RF90_PATH_A, 0x00, 0xFF, 0x00);

	// c.	For PCIE: SYS_FUNC_EN 0x02[7:0] = 0xE3	//enable BB TRX function
	//	For USB: SYS_FUNC_EN 0x02[7:0] = 0x17
#if (DEV_BUS_TYPE == PCI_INTERFACE)
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE3);
#else
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x17);
#endif

	// RF_ON_EXCEP(d~g):
	// d.	APSD_CTRL 0x600[7:0] = 0x00
	PlatformEFIOWrite1Byte(Adapter, REG_APSD_CTRL, 0x00);

	// e.	For PCIE: SYS_FUNC_EN 0x02[7:0] = 0xE2	//reset BB TRX function again
	//f.	For PCIE: SYS_FUNC_EN 0x02[7:0] = 0xE3	//enable BB TRX function
#if (DEV_BUS_TYPE == PCI_INTERFACE)
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE2);
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0xE3);
#else
	// e.For USB: SYS_FUNC_EN 0x02[7:0] = 0x16
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x16);
	// f. For USB: SYS_FUNC_EN 0x02[7:0] = 0x17
	PlatformEFIOWrite1Byte(Adapter, REG_SYS_FUNC_EN, 0x17);
#endif

	// g.	TXPAUSE 0x522[7:0] = 0x00				//enable MAC TX queue
	PlatformEFIOWrite1Byte(Adapter, REG_TXPAUSE, 0x00);

	
}	// phy_SetRTL8192CERfSleep
#endif

