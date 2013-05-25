/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2007, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 *************************************************************************/


#include "rt_config.h"
#ifdef LINUX
#include "linux/time.h"
#endif // LINUX //


#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
#define ATE_BBP_REG_NUM	168
UCHAR restore_BBP[ATE_BBP_REG_NUM]={0};
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //

// 802.11 MAC Header, Type:Data, Length:24bytes
UCHAR TemplateFrame[24] = {0x08,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0x00,0xAA,0xBB,0x12,0x34,0x56,0x00,0x11,0x22,0xAA,0xBB,0xCC,0x00,0x00};	
 
extern RTMP_RF_REGS RF2850RegTable[];
extern UCHAR NUM_OF_2850_CHNL;

extern FREQUENCY_ITEM FreqItems3020[];
extern UCHAR NUM_OF_3020_CHNL;


#ifdef RTMP_MAC_USB
extern UCHAR EpToQueue[];
extern VOID RTUSBRejectPendingPackets(IN PRTMP_ADAPTER pAd);
#endif // RTMP_MAC_USB //


static CHAR CCKRateTable[] = {0, 1, 2, 3, 8, 9, 10, 11, -1}; /* CCK Mode. */
static CHAR OFDMRateTable[] = {0, 1, 2, 3, 4, 5, 6, 7, -1}; /* OFDM Mode. */
static CHAR HTMIXRateTable[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, -1}; /* HT Mix Mode. */




static INT TxDmaBusy(
	IN PRTMP_ADAPTER pAd);

static INT RxDmaBusy(
	IN PRTMP_ADAPTER pAd);

static VOID RtmpDmaEnable(
	IN PRTMP_ADAPTER pAd,
	IN INT Enable);

static VOID BbpSoftReset(
	IN PRTMP_ADAPTER pAd);

static VOID RtmpRfIoWrite(
	IN PRTMP_ADAPTER pAd);

static INT ATESetUpFrame(
	IN PRTMP_ADAPTER pAd,
	IN UINT32 TxIdx);

static INT ATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index);

static NDIS_STATUS ATESTART(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS ATESTOP(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS TXCARR(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS TXCONT(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS TXFRAME(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS RXFRAME(
	IN PRTMP_ADAPTER pAd);

#ifdef RALINK_QA
static NDIS_STATUS TXSTOP(
	IN PRTMP_ADAPTER pAd);

static NDIS_STATUS RXSTOP(
	IN PRTMP_ADAPTER pAd);
#endif // RALINK_QA //

static NDIS_STATUS ATECmdHandler(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

static int CheckMCSValid(
	IN PRTMP_ADAPTER pAd, 
	IN UCHAR Mode,
	IN UCHAR Mcs);


#ifdef RTMP_MAC_USB
static VOID ATEWriteTxInfo(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXINFO_STRUC 	pTxInfo,
	IN	  USHORT		USBDMApktLen,
	IN	  BOOLEAN		bWiv,
	IN	  UCHAR			QueueSel,
	IN	  UCHAR			NextValid,
	IN	  UCHAR			TxBurst);

static VOID ATEWriteTxWI(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXWI_STRUC 	pTxWI,
	IN	BOOLEAN			FRAG,	
	IN	BOOLEAN			InsTimestamp,
	IN	BOOLEAN 		AMPDU,
	IN	BOOLEAN 		Ack,
	IN	BOOLEAN 		NSeq,		// HW new a sequence.
	IN	UCHAR			BASize,
	IN	UCHAR			WCID,
	IN	ULONG			Length,
	IN	UCHAR 			PID,
	IN	UCHAR			MIMOps,
	IN	UCHAR			Txopmode,	
	IN	BOOLEAN			CfAck,	
	IN	HTTRANSMIT_SETTING	Transmit);

#endif // RTMP_MAC_USB //

static VOID SetJapanFilter(
	IN	PRTMP_ADAPTER	pAd);


#define	LEN_OF_ARG 16


#ifdef RALINK_QA

static VOID memcpy_exl(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, ULONG len);
static VOID memcpy_exs(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, ULONG len);
static VOID RTMP_IO_READ_BULK(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, UINT32 len);

static INT ResponseToGUI(
	IN  struct ate_racfghdr *pRaCfg,
	IN	struct iwreq	*pwrq,
	IN  INT Length,
	IN  INT Status)
{
	(pRaCfg)->length = OS_HTONS((Length));
	(pRaCfg)->status = OS_HTONS((Status));
	(pwrq)->u.data.length = sizeof((pRaCfg)->magic_no) + sizeof((pRaCfg)->command_type)
							+ sizeof((pRaCfg)->command_id) + sizeof((pRaCfg)->length)
							+ sizeof((pRaCfg)->sequence) + OS_NTOHS((pRaCfg)->length);
	DBGPRINT(RT_DEBUG_TRACE, ("wrq->u.data.length = %d\n", (pwrq)->u.data.length));
	if (copy_to_user((pwrq)->u.data.pointer, (UCHAR *)(pRaCfg), (pwrq)->u.data.length))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("copy_to_user() fail in %s\n", __FUNCTION__));
		return (-EFAULT);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("%s is done !\n", __FUNCTION__));
	}

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_START(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_START\n"));
	
	/* Prepare feedback as soon as we can to avoid QA timeout. */
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);
	
#ifdef	CONFIG_RT2880_ATE_CMD_NEW
	Set_ATE_Proc(pAdapter, "ATESTART");
#else
	Set_ATE_Proc(pAdapter, "APSTOP");
#endif // CONFIG_RT2880_ATE_CMD_NEW //

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_STOP(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	INT32 ret;

	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_STOP\n"));

	/*
		Distinguish this command came from QA(via ate agent)
		or ate agent according to the existence of pid in payload.

		No need to prepare feedback if this cmd came directly from ate agent,
		not from QA.
	*/
	pRaCfg->length = OS_NTOHS(pRaCfg->length);

	if (pRaCfg->length == sizeof(pAdapter->ate.AtePid))
	{
		/*
			This command came from QA.
			Get the pid of ATE agent.
		*/
		memcpy((UCHAR *)&pAdapter->ate.AtePid,
						(&pRaCfg->data[0]) - 2/* == sizeof(pRaCfg->status) */,
						sizeof(pAdapter->ate.AtePid));					

		/* Prepare feedback as soon as we can to avoid QA timeout. */
		ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

		/*
			Kill ATE agent when leaving ATE mode.

			We must kill ATE agent first before setting ATESTOP,
			or Microsoft will report sth. wrong. 
		*/
		ret = KILL_THREAD_PID(pAdapter->ate.AtePid, SIGTERM, 1);

		if (ret)
			DBGPRINT(RT_DEBUG_ERROR, ("%s: unable to kill ate thread\n", 
				RTMP_OS_NETDEV_GET_DEVNAME(pAdapter->net_dev)));
	}
		

	/* AP/STA might have in ATE_STOP mode due to cmd from QA. */
	if (ATE_ON(pAdapter))
	{
		/* Someone has killed ate agent while QA GUI is still open. */

#ifdef	CONFIG_RT2880_ATE_CMD_NEW
		Set_ATE_Proc(pAdapter, "ATESTOP");
#else
		Set_ATE_Proc(pAdapter, "APSTART");
#endif
		DBGPRINT(RT_DEBUG_TRACE, ("RACFG_CMD_AP_START is done !\n"));
	}
	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_RF_WRITE_ALL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32 R1, R2, R3, R4;
	USHORT channel;
	
	memcpy(&R1, pRaCfg->data-2, 4);
	memcpy(&R2, pRaCfg->data+2, 4);
	memcpy(&R3, pRaCfg->data+6, 4);
	memcpy(&R4, pRaCfg->data+10, 4);
	memcpy(&channel, pRaCfg->data+14, 2);		
	
	pAdapter->LatchRfRegs.R1 = OS_NTOHL(R1);
	pAdapter->LatchRfRegs.R2 = OS_NTOHL(R2);
	pAdapter->LatchRfRegs.R3 = OS_NTOHL(R3);
	pAdapter->LatchRfRegs.R4 = OS_NTOHL(R4);
	pAdapter->LatchRfRegs.Channel = OS_NTOHS(channel);

	RTMP_RF_IO_WRITE32(pAdapter, pAdapter->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAdapter, pAdapter->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAdapter, pAdapter->LatchRfRegs.R3);
	RTMP_RF_IO_WRITE32(pAdapter, pAdapter->LatchRfRegs.R4);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return  NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_E2PROM_READ16(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT16	offset=0, value=0;
	USHORT  tmp=0;				

	offset = OS_NTOHS(pRaCfg->status);

	/* "tmp" is especially for some compilers... */
	RT28xx_EEPROM_READ16(pAdapter, offset, tmp);
	value = tmp;
	value = OS_HTONS(value);
	
	DBGPRINT(RT_DEBUG_TRACE,("EEPROM Read offset = 0x%04x, value = 0x%04x\n", offset, value));
	memcpy(pRaCfg->data, &value, 2);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+2, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_E2PROM_WRITE16(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT	offset, value;
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&value, pRaCfg->data, 2);
	value = OS_NTOHS(value);
	RT28xx_EEPROM_WRITE16(pAdapter, offset, value);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_E2PROM_READ_ALL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT buffer[EEPROM_SIZE/2];

	rt_ee_read_all(pAdapter,(USHORT *)buffer);
	memcpy_exs(pAdapter, pRaCfg->data, (UCHAR *)buffer, EEPROM_SIZE);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+EEPROM_SIZE, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_E2PROM_WRITE_ALL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT buffer[EEPROM_SIZE/2];

	NdisZeroMemory((UCHAR *)buffer, EEPROM_SIZE);
	memcpy_exs(pAdapter, (UCHAR *)buffer, (UCHAR *)&pRaCfg->status, EEPROM_SIZE);
	rt_ee_write_all(pAdapter,(USHORT *)buffer);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_IO_READ(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32	offset;
	UINT32	value;
	
	memcpy(&offset, &pRaCfg->status, 4);
	offset = OS_NTOHL(offset);

	/*
		We do not need the base address.
		So just extract the offset out.
	*/
	{
		offset &= 0x0000FFFF;
		RTMP_IO_READ32(pAdapter, offset, &value);
	}
	value = OS_HTONL(value);
	memcpy(pRaCfg->data, &value, 4);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+4, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_IO_WRITE(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32	offset, value;
					
	memcpy(&offset, pRaCfg->data-2, 4);
	memcpy(&value, pRaCfg->data+2, 4);

	offset = OS_NTOHL(offset);

	/*
		We do not need the base address.
		So just extract the offset out.
	*/
	offset &= 0x0000FFFF;
	value = OS_NTOHL(value);
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_IO_WRITE: offset = %x, value = %x\n", offset, value));
	RTMP_IO_WRITE32(pAdapter, offset, value);
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_IO_READ_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32	offset;
	USHORT	len;
	
	memcpy(&offset, &pRaCfg->status, 4);
	offset = OS_NTOHL(offset);

	/*
		We do not need the base address.
		So just extract the offset out.
	*/
	offset &= 0x0000FFFF;
	memcpy(&len, pRaCfg->data+2, 2);
	len = OS_NTOHS(len);

	if (len > 371)
	{
		DBGPRINT(RT_DEBUG_TRACE,("length requested is too large, make it smaller\n"));
		pRaCfg->length = OS_HTONS(2);
		pRaCfg->status = OS_HTONS(1);

		return -EFAULT;
	}

	RTMP_IO_READ_BULK(pAdapter, pRaCfg->data, (UCHAR *)offset, len*4);// unit in four bytes

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+(len*4), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_BBP_READ8(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT	offset;
	UCHAR	value;
	
	value = 0;
	offset = OS_NTOHS(pRaCfg->status);

	if (ATE_ON(pAdapter))
	{
		ATE_BBP_IO_READ8_BY_REG_ID(pAdapter, offset, &value);
	}
	else
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAdapter, offset, &value);	
	}

	pRaCfg->data[0] = value;
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+1, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_BBP_WRITE8(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT	offset;
	UCHAR	value;
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&value, pRaCfg->data, 1);

	if (ATE_ON(pAdapter))
	{
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAdapter, offset, value);
	}
	else
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAdapter, offset, value);
	}

	if ((offset == BBP_R1) || (offset == BBP_R3))
	{
		SyncTxRxConfig(pAdapter, offset, value);
	}
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_BBP_READ_ALL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT bbp_reg_index;
	
	for (bbp_reg_index = 0; bbp_reg_index < MAX_BBP_ID+1; bbp_reg_index++)
	{
		pRaCfg->data[bbp_reg_index] = 0;
		
		if (ATE_ON(pAdapter))
		{
			ATE_BBP_IO_READ8_BY_REG_ID(pAdapter, bbp_reg_index, &pRaCfg->data[bbp_reg_index]);
		}
		else
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAdapter, bbp_reg_index, &pRaCfg->data[bbp_reg_index]);
		}
	}
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+MAX_BBP_ID+1, NDIS_STATUS_SUCCESS);
	
	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_GET_NOISE_LEVEL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UCHAR	channel;
	INT32   buffer[3][10];/* 3 : RxPath ; 10 : no. of per rssi samples */

	channel = (OS_NTOHS(pRaCfg->status) & 0x00FF);
	CalNoiseLevel(pAdapter, channel, buffer);
	memcpy_exl(pAdapter, (UCHAR *)pRaCfg->data, (UCHAR *)&(buffer[0][0]), (sizeof(INT32)*3*10));

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+(sizeof(INT32)*3*10), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}



static  INT DO_RACFG_CMD_GET_COUNTER(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	memcpy_exl(pAdapter, &pRaCfg->data[0], (UCHAR *)&pAdapter->ate.U2M, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[4], (UCHAR *)&pAdapter->ate.OtherData, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[8], (UCHAR *)&pAdapter->ate.Beacon, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[12], (UCHAR *)&pAdapter->ate.OtherCount, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[16], (UCHAR *)&pAdapter->ate.TxAc0, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[20], (UCHAR *)&pAdapter->ate.TxAc1, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[24], (UCHAR *)&pAdapter->ate.TxAc2, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[28], (UCHAR *)&pAdapter->ate.TxAc3, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[32], (UCHAR *)&pAdapter->ate.TxHCCA, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[36], (UCHAR *)&pAdapter->ate.TxMgmt, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[40], (UCHAR *)&pAdapter->ate.RSSI0, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[44], (UCHAR *)&pAdapter->ate.RSSI1, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[48], (UCHAR *)&pAdapter->ate.RSSI2, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[52], (UCHAR *)&pAdapter->ate.SNR0, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[56], (UCHAR *)&pAdapter->ate.SNR1, 4);
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+60, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_CLEAR_COUNTER(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	pAdapter->ate.U2M = 0;
	pAdapter->ate.OtherData = 0;
	pAdapter->ate.Beacon = 0;
	pAdapter->ate.OtherCount = 0;
	pAdapter->ate.TxAc0 = 0;
	pAdapter->ate.TxAc1 = 0;
	pAdapter->ate.TxAc2 = 0;
	pAdapter->ate.TxAc3 = 0;
	pAdapter->ate.TxHCCA = 0;
	pAdapter->ate.TxMgmt = 0;
	pAdapter->ate.TxDoneCount = 0;
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_TX_START(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT *p;
	USHORT	err = 1;
	UCHAR   Bbp22Value = 0, Bbp24Value = 0;

	if ((pAdapter->ate.TxStatus != 0) && (pAdapter->ate.Mode & ATE_TXFRAME))
	{
		DBGPRINT(RT_DEBUG_TRACE,("Ate Tx is already running, to run next Tx, you must stop it first\n"));
		err = 2;
		goto tx_start_error;
	}
	else if ((pAdapter->ate.TxStatus != 0) && !(pAdapter->ate.Mode & ATE_TXFRAME))
	{
		int i = 0;

		while ((i++ < 10) && (pAdapter->ate.TxStatus != 0))
		{
			RTMPusecDelay(5000);
		}

		/* force it to stop */
		pAdapter->ate.TxStatus = 0;
		pAdapter->ate.TxDoneCount = 0;
		pAdapter->ate.bQATxStart = FALSE;
	}

#ifdef NEW_TXCONT
	/* Reset ATE mode and set Tx/Rx Idle */
	/* New proposed TXCONT solution. */
	if (pAdapter->ate.Mode |= ATE_TXFRAME)
	{
		TXSTOP(pAdapter);
	}
#endif // NEW_TXCONT //

	/*
		If pRaCfg->length == 0, this "RACFG_CMD_TX_START"
		is for Carrier test or Carrier Suppression.
	*/
	if (OS_NTOHS(pRaCfg->length) != 0)
	{
		/* get frame info */
#ifdef RTMP_MAC_USB
		NdisMoveMemory(&pAdapter->ate.TxInfo, pRaCfg->data - 2, 4);
#ifdef RT_BIG_ENDIAN
		RTMPDescriptorEndianChange((PUCHAR) &pAdapter->ate.TxInfo, TYPE_TXINFO);
#endif // RT_BIG_ENDIAN //
#endif // RTMP_MAC_USB //

		NdisMoveMemory(&pAdapter->ate.TxWI, pRaCfg->data + 2, 16);						
#ifdef RT_BIG_ENDIAN
		RTMPWIEndianChange((PUCHAR)&pAdapter->ate.TxWI, TYPE_TXWI);
#endif // RT_BIG_ENDIAN //

		NdisMoveMemory(&pAdapter->ate.TxCount, pRaCfg->data + 18, 4);
		pAdapter->ate.TxCount = OS_NTOHL(pAdapter->ate.TxCount);

		p = (USHORT *)(&pRaCfg->data[22]);

		/* always use QID_AC_BE */
		pAdapter->ate.QID = 0;

		p = (USHORT *)(&pRaCfg->data[24]);
		pAdapter->ate.HLen = OS_NTOHS(*p);

		if (pAdapter->ate.HLen > 32)
		{
			DBGPRINT(RT_DEBUG_ERROR,("pAdapter->ate.HLen > 32\n"));
			err = 3;
			goto tx_start_error;
		}

		NdisMoveMemory(&pAdapter->ate.Header, pRaCfg->data + 26, pAdapter->ate.HLen);

		pAdapter->ate.PLen = OS_NTOHS(pRaCfg->length) - (pAdapter->ate.HLen + 28);

		if (pAdapter->ate.PLen > 32)
		{
			DBGPRINT(RT_DEBUG_ERROR,("pAdapter->ate.PLen > 32\n"));
			err = 4;
			goto tx_start_error;
		}

		NdisMoveMemory(&pAdapter->ate.Pattern, pRaCfg->data + 26 + pAdapter->ate.HLen, pAdapter->ate.PLen);
		pAdapter->ate.DLen = pAdapter->ate.TxWI.MPDUtotalByteCount - pAdapter->ate.HLen;


	}

	ATE_BBP_IO_READ8_BY_REG_ID(pAdapter, BBP_R22, &Bbp22Value);

	switch (Bbp22Value)
	{
		case BBP22_TXFRAME:
			{
				if (pAdapter->ate.TxCount == 0)
				{
				}
				DBGPRINT(RT_DEBUG_TRACE,("START TXFRAME\n"));
				pAdapter->ate.bQATxStart = TRUE;
				Set_ATE_Proc(pAdapter, "TXFRAME");
			}
			break;

		case BBP22_TXCONT_OR_CARRSUPP:
			{
				DBGPRINT(RT_DEBUG_TRACE,("BBP22_TXCONT_OR_CARRSUPP\n"));
				ATE_BBP_IO_READ8_BY_REG_ID(pAdapter, BBP_R24, &Bbp24Value);

				switch (Bbp24Value)
				{
					case BBP24_TXCONT:
						{
							DBGPRINT(RT_DEBUG_TRACE,("START TXCONT\n"));
							pAdapter->ate.bQATxStart = TRUE;
#ifdef NEW_TXCONT
							/* New proposed solution. */
							/* Let QA handle all hardware manipulation. */ 
							/* Driver only needs to send a few training frames. */
							DBGPRINT(RT_DEBUG_OFF,("ATE ERROR! It should not reach here!\n"));
							Set_ATE_Proc(pAdapter, "TXFRAME");
#else
							Set_ATE_Proc(pAdapter, "TXCONT");
#endif // NEW_TXCONT //
						}
						break;

					case BBP24_CARRSUPP:
						{
							DBGPRINT(RT_DEBUG_TRACE,("START TXCARRSUPP\n"));
							pAdapter->ate.bQATxStart = TRUE;
							pAdapter->ate.Mode |= ATE_TXCARRSUPP;
						}
						break;

					default:
						{
							DBGPRINT(RT_DEBUG_ERROR,("Unknown TX subtype !"));
						}
						break;
				}
			}
			break;	

		case BBP22_TXCARR:
			{
				DBGPRINT(RT_DEBUG_TRACE,("START TXCARR\n"));
				pAdapter->ate.bQATxStart = TRUE;
				Set_ATE_Proc(pAdapter, "TXCARR");
			}
			break;							

		default:
			{
				DBGPRINT(RT_DEBUG_ERROR,("Unknown Start TX subtype !"));
			}
			break;
	}

	if (pAdapter->ate.bQATxStart == TRUE)
	{
		ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);
		return NDIS_STATUS_SUCCESS;
	}

tx_start_error:
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), err);

	return err;
}


static  INT DO_RACFG_CMD_GET_TX_STATUS(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32 count=0;
	
	count = OS_HTONL(pAdapter->ate.TxDoneCount);
	NdisMoveMemory(pRaCfg->data, &count, 4);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+4, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_TX_STOP(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_TX_STOP\n"));

	Set_ATE_Proc(pAdapter, "TXSTOP");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_RX_START(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_RX_START\n"));

	pAdapter->ate.bQARxStart = TRUE;
	Set_ATE_Proc(pAdapter, "RXFRAME");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}	


static  INT DO_RACFG_CMD_RX_STOP(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_RX_STOP\n"));

	Set_ATE_Proc(pAdapter, "ATESTART");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_START_TX_CARRIER(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_START_TX_CARRIER\n"));

	Set_ATE_Proc(pAdapter, "TXCARR");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_START_TX_CONT(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_START_TX_CONT\n"));

	Set_ATE_Proc(pAdapter, "TXCONT");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_START_TX_FRAME(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_START_TX_FRAME\n"));

	Set_ATE_Proc(pAdapter, "TXFRAME");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}	


static  INT DO_RACFG_CMD_ATE_SET_BW(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_BW\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);

	Set_ATE_TX_BW_Proc(pAdapter, str);
	
	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_TX_POWER0(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);

	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_TX_POWER0\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_POWER0_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_TX_POWER1(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_TX_POWER1\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_POWER1_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}




static  INT DO_RACFG_CMD_ATE_SET_FREQ_OFFSET(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);

	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_FREQ_OFFSET\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_FREQOFFSET_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_GET_STATISTICS(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_GET_STATISTICS\n"));

	memcpy_exl(pAdapter, &pRaCfg->data[0], (UCHAR *)&pAdapter->ate.TxDoneCount, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[4], (UCHAR *)&pAdapter->WlanCounters.RetryCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[8], (UCHAR *)&pAdapter->WlanCounters.FailedCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[12], (UCHAR *)&pAdapter->WlanCounters.RTSSuccessCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[16], (UCHAR *)&pAdapter->WlanCounters.RTSFailureCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[20], (UCHAR *)&pAdapter->WlanCounters.ReceivedFragmentCount.QuadPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[24], (UCHAR *)&pAdapter->WlanCounters.FCSErrorCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[28], (UCHAR *)&pAdapter->Counters8023.RxNoBuffer, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[32], (UCHAR *)&pAdapter->WlanCounters.FrameDuplicateCount.u.LowPart, 4);
	memcpy_exl(pAdapter, &pRaCfg->data[36], (UCHAR *)&pAdapter->RalinkCounters.OneSecFalseCCACnt, 4);
	
	if (pAdapter->ate.RxAntennaSel == 0)
	{
		INT32 RSSI0 = 0;
		INT32 RSSI1 = 0;
		INT32 RSSI2 = 0;

		RSSI0 = (INT32)(pAdapter->ate.LastRssi0 - pAdapter->BbpRssiToDbmDelta);
		RSSI1 = (INT32)(pAdapter->ate.LastRssi1 - pAdapter->BbpRssiToDbmDelta);
		RSSI2 = (INT32)(pAdapter->ate.LastRssi2 - pAdapter->BbpRssiToDbmDelta);
		memcpy_exl(pAdapter, &pRaCfg->data[40], (UCHAR *)&RSSI0, 4);
		memcpy_exl(pAdapter, &pRaCfg->data[44], (UCHAR *)&RSSI1, 4);
		memcpy_exl(pAdapter, &pRaCfg->data[48], (UCHAR *)&RSSI2, 4);
		ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+52, NDIS_STATUS_SUCCESS);
	}
	else
	{
		INT32 RSSI0 = 0;
	
		RSSI0 = (INT32)(pAdapter->ate.LastRssi0 - pAdapter->BbpRssiToDbmDelta);
		memcpy_exl(pAdapter, &pRaCfg->data[40], (UCHAR *)&RSSI0, 4);
		ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+44, NDIS_STATUS_SUCCESS);
	}

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_RESET_COUNTER(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 1;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);

	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_RESET_COUNTER\n"));				

	sprintf((char *)str, "%d", value);
	Set_ResetStatCounter_Proc(pAdapter, str);

	pAdapter->ate.TxDoneCount = 0;

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SEL_TX_ANTENNA(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)	
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SEL_TX_ANTENNA\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_Antenna_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SEL_RX_ANTENNA(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SEL_RX_ANTENNA\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_RX_Antenna_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_PREAMBLE(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_PREAMBLE\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_MODE_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_CHANNEL(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_CHANNEL\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_CHANNEL_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_ADDR1(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_ADDR1\n"));

	/*
		Addr is an array of UCHAR,
		so no need to perform endian swap.
	*/
	memcpy(pAdapter->ate.Addr1, (PUCHAR)(pRaCfg->data - 2), MAC_ADDR_LEN);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_ADDR2(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_ADDR2\n"));

	/*
		Addr is an array of UCHAR,
		so no need to perform endian swap.
	*/
	memcpy(pAdapter->ate.Addr2, (PUCHAR)(pRaCfg->data - 2), MAC_ADDR_LEN);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_ADDR3(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_ADDR3\n"));

	/*
		Addr is an array of UCHAR,
		so no need to perform endian swap.
	*/
	memcpy(pAdapter->ate.Addr3, (PUCHAR)(pRaCfg->data - 2), MAC_ADDR_LEN);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_RATE(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_RATE\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_MCS_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_TX_FRAME_LEN(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	SHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_TX_FRAME_LEN\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);
	sprintf((char *)str, "%d", value);
	Set_ATE_TX_LENGTH_Proc(pAdapter, str);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_SET_TX_FRAME_COUNT(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT    value = 0;
	STRING    str[LEN_OF_ARG];

	NdisZeroMemory(str, LEN_OF_ARG);
	
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_ATE_SET_TX_FRAME_COUNT\n"));				

	memcpy((PUCHAR)&value, (PUCHAR)&(pRaCfg->status), 2);
	value = OS_NTOHS(value);

	{
		sprintf((char *)str, "%d", value);
		Set_ATE_TX_COUNT_Proc(pAdapter, str);
	}

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_START_RX_FRAME(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	DBGPRINT(RT_DEBUG_TRACE,("RACFG_CMD_RX_START\n"));

	Set_ATE_Proc(pAdapter, "RXFRAME");

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_E2PROM_READ_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT offset;
	USHORT len;
	USHORT buffer[EEPROM_SIZE/2];
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&len, pRaCfg->data, 2);
	len = OS_NTOHS(len);
	
	rt_ee_read_all(pAdapter, (USHORT *)buffer);

	if (offset + len <= EEPROM_SIZE)
		memcpy_exs(pAdapter, pRaCfg->data, (UCHAR *)buffer+offset, len);
	else
		DBGPRINT(RT_DEBUG_ERROR, ("exceed EEPROM size\n"));

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+len, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_E2PROM_WRITE_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT offset;
	USHORT len;
	USHORT buffer[EEPROM_SIZE/2];
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&len, pRaCfg->data, 2);
	len = OS_NTOHS(len);

	rt_ee_read_all(pAdapter,(USHORT *)buffer);
	memcpy_exs(pAdapter, (UCHAR *)buffer + offset, (UCHAR *)pRaCfg->data + 2, len);
	rt_ee_write_all(pAdapter,(USHORT *)buffer);

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_IO_WRITE_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	UINT32 offset, i, value;
	USHORT len;
	
	memcpy(&offset, &pRaCfg->status, 4);
	offset = OS_NTOHL(offset);
	memcpy(&len, pRaCfg->data+2, 2);
	len = OS_NTOHS(len);
	
	for (i = 0; i < len; i += 4)
	{
		memcpy_exl(pAdapter, (UCHAR *)&value, pRaCfg->data+4+i, 4);
		DBGPRINT(RT_DEBUG_TRACE,("Write %x %x\n", offset + i, value));
		RTMP_IO_WRITE32(pAdapter, ((offset+i) & (0xffff)), value);
	}

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_BBP_READ_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT offset;
	USHORT len;
	USHORT j;
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&len, pRaCfg->data, 2);
	len = OS_NTOHS(len);
	
	for (j = offset; j < (offset+len); j++)
	{
		pRaCfg->data[j - offset] = 0;
		
		if (pAdapter->ate.Mode == ATE_STOP)
		{
			RTMP_BBP_IO_READ8_BY_REG_ID(pAdapter, j, &pRaCfg->data[j - offset]);
		}
		else
		{
			ATE_BBP_IO_READ8_BY_REG_ID(pAdapter, j, &pRaCfg->data[j - offset]);
		}
	}

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status)+len, NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}


static  INT DO_RACFG_CMD_ATE_BBP_WRITE_BULK(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg)
{
	USHORT offset;
	USHORT len;
	USHORT j;
	UCHAR *value;
	
	offset = OS_NTOHS(pRaCfg->status);
	memcpy(&len, pRaCfg->data, 2);
	len = OS_NTOHS(len);
					
	for (j = offset; j < (offset+len); j++)
	{
		value = pRaCfg->data + 2 + (j - offset);
		if (pAdapter->ate.Mode == ATE_STOP)
		{
			RTMP_BBP_IO_WRITE8_BY_REG_ID(pAdapter, j,  *value);
		}
		else
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAdapter, j,  *value);
		}
	}

	ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);

	return NDIS_STATUS_SUCCESS;
}





typedef INT (*RACFG_CMD_HANDLER)(
	IN	PRTMP_ADAPTER	pAdapter,
	IN	struct iwreq	*wrq,
	IN  struct ate_racfghdr *pRaCfg);


RACFG_CMD_HANDLER RACFG_CMD_SET1[] =
{
	/* cmd id start from 0x0 */
	DO_RACFG_CMD_RF_WRITE_ALL,	
	DO_RACFG_CMD_E2PROM_READ16,
	DO_RACFG_CMD_E2PROM_WRITE16,
	DO_RACFG_CMD_E2PROM_READ_ALL,
	DO_RACFG_CMD_E2PROM_WRITE_ALL,
	DO_RACFG_CMD_IO_READ,
	DO_RACFG_CMD_IO_WRITE,
	DO_RACFG_CMD_IO_READ_BULK,
	DO_RACFG_CMD_BBP_READ8,
	DO_RACFG_CMD_BBP_WRITE8,
	DO_RACFG_CMD_BBP_READ_ALL,
	DO_RACFG_CMD_GET_COUNTER,
	DO_RACFG_CMD_CLEAR_COUNTER,
	NULL /* RACFG_CMD_RSV1 */,
	NULL /* RACFG_CMD_RSV2 */,
	NULL /* RACFG_CMD_RSV3 */,
	DO_RACFG_CMD_TX_START,
	DO_RACFG_CMD_GET_TX_STATUS,
	DO_RACFG_CMD_TX_STOP,
	DO_RACFG_CMD_RX_START,
	DO_RACFG_CMD_RX_STOP,
	DO_RACFG_CMD_GET_NOISE_LEVEL,
	NULL

	/* cmd id end with 0x1e */
};


RACFG_CMD_HANDLER RACFG_CMD_SET2[] =
{
	/* cmd id start from 0x80 */
	DO_RACFG_CMD_ATE_START,
	DO_RACFG_CMD_ATE_STOP
	/* cmd id end with 0x81 */
};


RACFG_CMD_HANDLER RACFG_CMD_SET3[] =
{
	/* cmd id start from 0x100 */
	DO_RACFG_CMD_ATE_START_TX_CARRIER,
	DO_RACFG_CMD_ATE_START_TX_CONT,
	DO_RACFG_CMD_ATE_START_TX_FRAME,
	DO_RACFG_CMD_ATE_SET_BW,
	DO_RACFG_CMD_ATE_SET_TX_POWER0,
	DO_RACFG_CMD_ATE_SET_TX_POWER1,
	DO_RACFG_CMD_ATE_SET_FREQ_OFFSET,
	DO_RACFG_CMD_ATE_GET_STATISTICS,
	DO_RACFG_CMD_ATE_RESET_COUNTER,
	DO_RACFG_CMD_ATE_SEL_TX_ANTENNA,
	DO_RACFG_CMD_ATE_SEL_RX_ANTENNA,
	DO_RACFG_CMD_ATE_SET_PREAMBLE,
	DO_RACFG_CMD_ATE_SET_CHANNEL,
	DO_RACFG_CMD_ATE_SET_ADDR1,
	DO_RACFG_CMD_ATE_SET_ADDR2,
	DO_RACFG_CMD_ATE_SET_ADDR3,
	DO_RACFG_CMD_ATE_SET_RATE,
	DO_RACFG_CMD_ATE_SET_TX_FRAME_LEN,
	DO_RACFG_CMD_ATE_SET_TX_FRAME_COUNT,
	DO_RACFG_CMD_ATE_START_RX_FRAME,
	DO_RACFG_CMD_ATE_E2PROM_READ_BULK,
	DO_RACFG_CMD_ATE_E2PROM_WRITE_BULK,
	DO_RACFG_CMD_ATE_IO_WRITE_BULK,
	DO_RACFG_CMD_ATE_BBP_READ_BULK,
	DO_RACFG_CMD_ATE_BBP_WRITE_BULK,
	NULL,
	NULL,
	NULL
	/* cmd id end with 0x11b */
};


RACFG_CMD_HANDLER RACFG_CMD_SET4[] =
{
	/* cmd id start from 0x200 */
	/* cmd id end with 0x200 */
};


/*
#define MAX_RACFG_TABLE_NUMBER 4
UINT16 RACFG_CMD_INDEX_RANGE_LIMIT[] = { 0x0000, 0x0080, 0x0100, 0x0200, 0xffff };
RACFG_CMD_HANDLER *RACFG_CMD_TABLE[] = { RACFG_CMD_TABLE1, RACFG_CMD_TABLE2, RACFG_CMD_TABLE3, RACFG_CMD_TABLE4 };
*/


typedef struct _RACFG_CMD_TABLE_{
	RACFG_CMD_HANDLER *cmdSet;
	int	cmdSetSize;
	int	cmdOffset;
}RACFG_CMD_TABLE;


RACFG_CMD_TABLE RACFG_CMD_TABLES[]={
	{
		RACFG_CMD_SET1,
		sizeof(RACFG_CMD_SET1) / sizeof(RACFG_CMD_HANDLER),
		0x0,
	},
	{
		RACFG_CMD_SET2,
		sizeof(RACFG_CMD_SET2) / sizeof(RACFG_CMD_HANDLER),
		0x80,
	},
	{
		RACFG_CMD_SET3,
		sizeof(RACFG_CMD_SET3) / sizeof(RACFG_CMD_HANDLER),
		0x100,
	},
	{
		RACFG_CMD_SET4,
		sizeof(RACFG_CMD_SET4) / sizeof(RACFG_CMD_HANDLER),
		0x200,
	},
};
#endif // RALINK_QA //




#ifdef RTMP_MAC_USB
static INT TxDmaBusy(
	IN PRTMP_ADAPTER pAd)
{
	INT result;
	USB_DMA_CFG_STRUC UsbCfg;

	RTMP_IO_READ32(pAd, USB_DMA_CFG, &UsbCfg.word);	// disable DMA

	if (UsbCfg.field.TxBusy)
		result = 1;
	else
		result = 0;

	return result;
}


static INT RxDmaBusy(
	IN PRTMP_ADAPTER pAd)
{
	INT result;
	USB_DMA_CFG_STRUC UsbCfg;

	RTMP_IO_READ32(pAd, USB_DMA_CFG, &UsbCfg.word);	// disable DMA
	if (UsbCfg.field.RxBusy)
		result = 1;
	else
		result = 0;

	return result;
}


static VOID RtmpDmaEnable(
	IN PRTMP_ADAPTER pAd,
	IN INT Enable)
{
	BOOLEAN value;
	ULONG WaitCnt;
	USB_DMA_CFG_STRUC UsbCfg;
	
	value = Enable > 0 ? 1 : 0;

	// check DMA is in busy mode.
	WaitCnt = 0;

	while (TxDmaBusy(pAd) || RxDmaBusy(pAd))
	{
		RTMPusecDelay(10);
		if (WaitCnt++ > 100)
			break;
	}

	// Why not to clear USB DMA TX path first ???
	RTMP_IO_READ32(pAd, USB_DMA_CFG, &UsbCfg.word);	// disable DMA
	UsbCfg.field.TxBulkEn = value;
	UsbCfg.field.RxBulkEn = value;
	RTMP_IO_WRITE32(pAd, USB_DMA_CFG, UsbCfg.word);	// abort all TX rings
	RTMPusecDelay(5000);

	return;
}
#endif // RTMP_MAC_USB //


static VOID BbpSoftReset(
	IN PRTMP_ADAPTER pAd)
{
	UCHAR BbpData = 0;

	// Soft reset, set BBP R21 bit0=1->0
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R21, &BbpData);
	BbpData |= 0x00000001; //set bit0=1
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);

	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R21, &BbpData);
	BbpData &= ~(0x00000001); //set bit0=0
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R21, BbpData);

	return;
}


static VOID RtmpRfIoWrite(
	IN PRTMP_ADAPTER pAd)
{
	// Set RF value 1's set R3[bit2] = [0]
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 & (~0x04)));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	RTMPusecDelay(200);

	// Set RF value 2's set R3[bit2] = [1]
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 | 0x04));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	RTMPusecDelay(200);

	// Set RF value 3's set R3[bit2] = [0]
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R1);
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R2);
	RTMP_RF_IO_WRITE32(pAd, (pAd->LatchRfRegs.R3 & (~0x04)));
	RTMP_RF_IO_WRITE32(pAd, pAd->LatchRfRegs.R4);

	return;
}


static int CheckMCSValid(
	IN PRTMP_ADAPTER	pAd, 
	IN UCHAR Mode,
	IN UCHAR Mcs)
{
	int i;
	PCHAR pRateTab = NULL;

	switch (Mode)
	{
		case MODE_CCK:
			pRateTab = CCKRateTable;
			break;
		case MODE_OFDM:
			pRateTab = OFDMRateTable;
			break;
			
		case 2: /*MODE_HTMIX*/
		case 3: /*MODE_HTGREENFIELD*/
				pRateTab = HTMIXRateTable;
			break;
			
		default: 
			DBGPRINT(RT_DEBUG_ERROR, ("unrecognizable Tx Mode %d\n", Mode));
			return -1;
			break;
	}

	i = 0;
	while (pRateTab[i] != -1)
	{
		if (pRateTab[i] == Mcs)
			return 0;
		i++;
	}

	return -1;
}


static INT ATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index)
{
	ULONG R;
	CHAR TxPower = 0;
	UCHAR Bbp94 = 0;
	BOOLEAN bPowerReduce = FALSE;

#ifdef RALINK_QA
	if ((pAd->ate.bQATxStart == TRUE) || (pAd->ate.bQARxStart == TRUE))
	{
		/* 
			When QA is used for Tx, pAd->ate.TxPower0/1 and real tx power
			are not synchronized.
		*/
		return 0;
	}
	else
#endif // RALINK_QA //

	if (index == 0)
	{
		TxPower = pAd->ate.TxPower0;
	}
	else if (index == 1)
	{
		TxPower = pAd->ate.TxPower1;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Only TxPower0 and TxPower1 are adjustable !\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("TxPower%d is out of range !\n", index));
	}

	{
		if (pAd->ate.Channel <= 14)
		{
			if (TxPower > 31)
			{
				// R3, R4 can't large than 31 (0x24), 31 ~ 36 used by BBP 94
				R = 31;
				if (TxPower <= 36)
					Bbp94 = BBPR94_DEFAULT + (UCHAR)(TxPower - 31);		
			}
			else if (TxPower < 0)
			{
				// R3, R4 can't less than 0, -1 ~ -6 used by BBP 94
				R = 0;
				if (TxPower >= -6)
					Bbp94 = BBPR94_DEFAULT + TxPower;
			}
			else
			{  
				// 0 ~ 31
				R = (ULONG) TxPower;
				Bbp94 = BBPR94_DEFAULT;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("%s (TxPower=%d, R=%ld, BBP_R94=%d)\n", __FUNCTION__, TxPower, R, Bbp94));
		}
		else /* 5.5 GHz */
		{
			if (TxPower > 15)
			{
				// R3, R4 can't large than 15 (0x0F)
				R = 15;
			}
			else if (TxPower < 0)
			{
				// R3, R4 can't less than 0
				// -1 ~ -7
				ASSERT((TxPower >= -7));
				R = (ULONG)(TxPower + 7);
				bPowerReduce = TRUE;
			}
			else
			{  
				// 0 ~ 15
				R = (ULONG) TxPower;
			}

			DBGPRINT(RT_DEBUG_TRACE, ("%s (TxPower=%d, R=%lu)\n", __FUNCTION__, TxPower, R));
		}

		if (pAd->ate.Channel <= 14)
		{
			if (index == 0)
			{
				// shift TX power control to correct RF(R3) register bit position
				R = R << 9;		
				R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);
				pAd->LatchRfRegs.R3 = R;
			}
			else
			{
				// shift TX power control to correct RF(R4) register bit position
				R = R << 6;		
				R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);
				pAd->LatchRfRegs.R4 = R;
			}
		}
		else /* 5.5GHz */
		{
			if (bPowerReduce == FALSE)
			{
				if (index == 0)
				{
					// shift TX power control to correct RF(R3) register bit position
					R = (R << 10) | (1 << 9);		
					R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);
					pAd->LatchRfRegs.R3 = R;
				}
				else
				{
					// shift TX power control to correct RF(R4) register bit position
					R = (R << 7) | (1 << 6);		
					R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);
					pAd->LatchRfRegs.R4 = R;
				}
			}
			else
			{
				if (index == 0)
				{
					// shift TX power control to correct RF(R3) register bit position
					R = (R << 10);		
					R |= (pAd->LatchRfRegs.R3 & 0xffffc1ff);

					/* Clear bit 9 of R3 to reduce 7dB. */ 
					pAd->LatchRfRegs.R3 = (R & (~(1 << 9)));
				}
				else
				{
					// shift TX power control to correct RF(R4) register bit position
					R = (R << 7);		
					R |= (pAd->LatchRfRegs.R4 & 0xfffff83f);

					/* Clear bit 6 of R4 to reduce 7dB. */ 
					pAd->LatchRfRegs.R4 = (R & (~(1 << 6)));
				}
			}
		}
		RtmpRfIoWrite(pAd);
	}

	return 0;
}

#ifdef NEW_TXCONT
static UINT32 Default_TX_PIN_CFG;
#define RA_TX_PIN_CFG 0x1328
#define TxCont_TX_PIN_CFG 0x000C00F0
#endif // NEW_TXCONT //

static NDIS_STATUS ATESTART(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0, atemode=0, temp=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;
#ifdef RTMP_MAC_USB
#ifdef CONFIG_STA_SUPPORT
	BOOLEAN       Cancelled;
#endif // CONFIG_STA_SUPPORT //
	UCHAR LoopCount=0;
#endif // RTMP_MAC_USB //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

#ifdef RTMP_MAC_USB
	RTMP_OS_NETDEV_STOP_QUEUE(pAd->net_dev);
#endif // RTMP_MAC_USB //

	atemode = pAd->ate.Mode;
	pAd->ate.Mode = ATE_START;

        if (atemode == ATE_STOP)
        {
                /* DUT just enters ATE mode from normal mode. */
                /* Only at this moment, we need to switch back to the channel of normal mode */
                        AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);
                        /* empty function */
                        AsicLockChannel(pAd, pAd->CommonCfg.Channel);
        }
#ifdef RTMP_MAC_USB
        while ((pAd->PendingRx > 0) && LoopCount < 2)    
        {
                /* delay 0.5 seconds */
                OS_WAIT(500);
                LoopCount++;
        }
#endif // RTMP_MAC_USB //

	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
	
	/* Disable auto responder */
	RTMP_IO_READ32(pAd, AUTO_RSP_CFG, &temp);
	temp = temp & 0xFFFFFFFE;
	RTMP_IO_WRITE32(pAd, AUTO_RSP_CFG, temp);

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= 0xFFFFFFEF; 
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	if (atemode == ATE_TXCARR)
	{
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
		/* RT35xx ATE will reuse this code segment. */
		/* Hardware Reset BBP */
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
		temp = temp | 0x00000002;
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
		temp = temp & ~(0x00000002);
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);

		/* Restore All BBP Value */
		for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);

		/*
			The RfIcType will be reset to zero after the hardware reset bbp command.
			Therefore, we must restore the original RfIcType.
		*/
		pAd->RfIcType=RestoreRfICType;
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //

		/* No Carrier Test set BBP R22 bit7=0, bit6=0, bit[5~0]=0x0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= 0xFFFFFF00; // clear bit7, bit6, bit[5~0]	
	    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
	}
	else if (atemode == ATE_TXCARRSUPP)
	{
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
		/* RT35xx ATE will reuse this code segment. */
		/* Hardware Reset BBP */
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
		temp = temp | 0x00000002;
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
		temp = temp & ~(0x00000002);
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);

		/* Restore All BBP Value */
		for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);


		/*
			The RfIcType will be reset to zero after the hardware reset bbp command.
			Therefore, we must restore the original RfIcType.
		*/
		pAd->RfIcType=RestoreRfICType;			
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //

		/* No Cont. TX set BBP R22 bit7=0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= ~(1 << 7); // set bit7=0
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

		/* No Carrier Suppression set BBP R24 bit0=0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R24, &BbpData);
		BbpData &= 0xFFFFFFFE; // clear bit0	
	    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R24, BbpData);
	}		

	/*
		We should free some resource which was allocated
		when ATE_TXFRAME , ATE_STOP, and ATE_TXCONT.
	*/
	else if ((atemode & ATE_TXFRAME) || (atemode == ATE_STOP))
	{
		if ((atemode == ATE_TXCONT))
		{
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
			/* RT35xx ATE will reuse this code segment. */
			/* Hardware Reset BBP */
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
			temp = temp | 0x00000002;
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);
			RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &temp);
			temp = temp & ~(0x00000002);
			RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, temp);

			/* Restore All BBP Value */
			for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);


		pAd->RfIcType=RestoreRfICType;			
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //

			/* Not Cont. TX anymore, so set BBP R22 bit7=0 */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
			BbpData &= ~(1 << 7); // set bit7=0
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
#ifdef NEW_TXCONT
			BbpSoftReset(pAd);
			RTMP_IO_WRITE32(pAd, RA_TX_PIN_CFG, Default_TX_PIN_CFG);
#endif // NEW_TXCONT //
		}

		/* Abort Tx, Rx DMA. */
		RtmpDmaEnable(pAd, 0);

		/* Start Tx, RX DMA */
		RtmpDmaEnable(pAd, 1);
	}

#ifdef RTMP_MAC_USB
	RTUSBRejectPendingPackets(pAd);
	RTUSBCleanUpDataBulkOutQueue(pAd);

#ifdef CONFIG_STA_SUPPORT
	/*
		It will be called in MlmeSuspend().
		Cancel pending timers.
	*/
	RTMPCancelTimer(&pAd->MlmeAux.AssocTimer,      &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.ReassocTimer,    &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.DisassocTimer,   &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.AuthTimer,       &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.BeaconTimer,     &Cancelled);
	RTMPCancelTimer(&pAd->MlmeAux.ScanTimer,       &Cancelled);
#endif // CONFIG_STA_SUPPORT //

//	RTUSBCleanUpMLMEWaitQueue(pAd);	
	RTUSBCleanUpMLMEBulkOutQueue(pAd);


	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	/* Disable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/*
		Make sure there are no pending bulk in/out IRPs before we go on.
		pAd->BulkFlags != 0 : wait bulk out finish
	*/	
	while ((pAd->PendingRx > 0))	
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);

		/* delay 0.5 seconds */
		RTMPusecDelay(500000);
		pAd->PendingRx = 0;
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))
			/* pAd->BulkFlags != 0 : wait bulk out finish */	
	{
		do 
		{
			/* 
				pAd->BulkOutPending[y] will be set to FALSE
				in RTUSBCancelPendingBulkOutIRP(pAd)
			*/
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);			

	}

	ASSERT(pAd->PendingRx == 0);
#endif // RTMP_MAC_USB //

	/* reset Rx statistics. */
	pAd->ate.LastSNR0 = 0;
	pAd->ate.LastSNR1 = 0;
	pAd->ate.LastRssi0 = 0;
	pAd->ate.LastRssi1 = 0;
	pAd->ate.LastRssi2 = 0;
	pAd->ate.AvgRssi0 = 0;
	pAd->ate.AvgRssi1 = 0;
	pAd->ate.AvgRssi2 = 0;
	pAd->ate.AvgRssi0X8 = 0;
	pAd->ate.AvgRssi1X8 = 0;
	pAd->ate.AvgRssi2X8 = 0;
	pAd->ate.NumOfAvgRssiSample = 0;

#ifdef RALINK_QA
	/* Tx frame */
	pAd->ate.bQATxStart = FALSE;
	pAd->ate.bQARxStart = FALSE;
	pAd->ate.seq = 0; 

	/* counters */
	pAd->ate.U2M = 0;
	pAd->ate.OtherData = 0;
	pAd->ate.Beacon = 0;
	pAd->ate.OtherCount = 0;
	pAd->ate.TxAc0 = 0;
	pAd->ate.TxAc1 = 0;
	pAd->ate.TxAc2 = 0;
	pAd->ate.TxAc3 = 0;
	pAd->ate.TxHCCA = 0;
	pAd->ate.TxMgmt = 0;
	pAd->ate.RSSI0 = 0;
	pAd->ate.RSSI1 = 0;
	pAd->ate.RSSI2 = 0;
	pAd->ate.SNR0 = 0;
	pAd->ate.SNR1 = 0;

	/* control */
	pAd->ate.TxDoneCount = 0;
	// TxStatus : 0 --> task is idle, 1 --> task is running
	pAd->ate.TxStatus = 0;
#endif // RALINK_QA //

	// Soft reset BBP.
	BbpSoftReset(pAd);


#ifdef CONFIG_STA_SUPPORT 
	/* LinkDown() has "AsicDisableSync();" and "RTMP_BBP_IO_R/W8_BY_REG_ID();" inside. */
//	LinkDown(pAd, FALSE);
//	AsicEnableBssSync(pAd);
	AsicDisableSync(pAd);
	/* 
		If we skip "LinkDown()", we should disable protection
		to prevent from sending out RTS or CTS-to-self.
	*/
	ATEDisableAsicProtect(pAd);
	RTMPStationStop(pAd);
#endif // CONFIG_STA_SUPPORT //


#ifdef RTMP_MAC_USB
	/* Default value in BBP R22 is 0x0. */
	BbpData = 0;
   	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

	/* Clear bit4 to stop continuous Tx production test. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= 0xFFFFFFEF;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);		

	/* Clear ATE Bulk in/out counter and continue setup */
	InterlockedExchange(&pAd->BulkOutRemained, 0);

	/* NdisAcquireSpinLock()/NdisReleaseSpinLock() need only one argument in RT28xx */
	NdisAcquireSpinLock(&pAd->GenericLock);	
	pAd->ContinBulkOut = FALSE;		
	pAd->ContinBulkIn = FALSE;
	NdisReleaseSpinLock(&pAd->GenericLock);

	RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
#endif // RTMP_MAC_USB //


	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS ATESTOP(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0, ring_index=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	UINT32			bbp_index=0;
	UCHAR			RestoreRfICType=pAd->RfIcType;
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) // 
	UCHAR			BbpData = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	/* RT35xx ATE will reuse this code segment. */
	/* hardware reset BBP */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData = MacData | 0x00000002;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	RTMPusecDelay(10000);

	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData = MacData & ~(0x00000002);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Supposed that we have had a record in restore_BBP[] */
	/* restore all BBP value */
	for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd,bbp_index,restore_BBP[bbp_index]);

	/*
		The RfIcType will be reset to zero after the hardware reset bbp command.
		Therefore, we must restore the original RfIcType.
	*/
	ASSERT(RestoreRfICType != 0);
	pAd->RfIcType=RestoreRfICType;
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) // 

	/* Default value in BBP R22 is 0x0. */
	BbpData = 0;
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

	/* Clear bit4 to stop continuous Tx production test. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= 0xFFFFFFEF;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData); 
	
	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
	
	/* Abort Tx, RX DMA */
	RtmpDmaEnable(pAd, 0);

	/* Disable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);


#ifdef RTMP_MAC_USB
	/* 
		Make sure there are no pending bulk in/out IRPs before we go on.
		pAd->BulkFlags != 0 : wait bulk out finish
	*/
	while (pAd->PendingRx > 0)
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RTMPusecDelay(500000);
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))
			/* pAd->BulkFlags != 0 : wait bulk out finish */
	{
		do 
		{
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);			

		RTMPusecDelay(500000);
	}

	ASSERT(pAd->PendingRx == 0);
/*=========================================================================*/
/*      Reset Rx RING                                                      */ 
/*=========================================================================*/
//		InterlockedExchange(&pAd->PendingRx, 0);
	pAd->PendingRx = 0;
	/* Next Rx Read index */
	pAd->NextRxBulkInReadIndex = 0;	
	/* Rx Bulk pointer */
	pAd->NextRxBulkInIndex = RX_RING_SIZE - 1;	
	pAd->NextRxBulkInPosition = 0;
	for (ring_index = 0; ring_index < (RX_RING_SIZE); ring_index++)
	{
		PRX_CONTEXT  pRxContext = &(pAd->RxContext[ring_index]);
		NdisZeroMemory(pRxContext->TransferBuffer, MAX_RXBULK_SIZE);

		pRxContext->pAd	= pAd;
		pRxContext->pIrp = NULL;
		pRxContext->BulkInOffset = 0;
		pRxContext->bRxHandling = FALSE;
		pRxContext->InUse		= FALSE;
		pRxContext->IRPPending	= FALSE;
		pRxContext->Readable	= FALSE;
//		pRxContext->ReorderInUse = FALSE;
//		pRxContext->ReadPosOffset = 0;
	}

/*=========================================================================*/
/*      Reset Tx RING                                                      */ 
/*=========================================================================*/
	do 
	{
		RTUSBCancelPendingBulkOutIRP(pAd);
	} while (FALSE);

	/* Enable auto responder. */
	RTMP_IO_READ32(pAd, AUTO_RSP_CFG, &MacData);
	MacData = MacData | (0x01);
	RTMP_IO_WRITE32(pAd, AUTO_RSP_CFG, MacData);

	AsicEnableBssSync(pAd);

	/* Soft reset BBP.*/
	/* In 2870 chipset, ATE_BBP_IO_READ8_BY_REG_ID() == RTMP_BBP_IO_READ8_BY_REG_ID() */
	/* Both rt2870ap and rt2870sta use BbpSoftReset(pAd) to do BBP soft reset */
	BbpSoftReset(pAd);
	{
#ifdef CONFIG_STA_SUPPORT
		/* Set all state machines back IDLE */
		pAd->Mlme.CntlMachine.CurrState    = CNTL_IDLE;
		pAd->Mlme.AssocMachine.CurrState   = ASSOC_IDLE;
		pAd->Mlme.AuthMachine.CurrState    = AUTH_REQ_IDLE;
		pAd->Mlme.AuthRspMachine.CurrState = AUTH_RSP_IDLE;
		pAd->Mlme.SyncMachine.CurrState    = SYNC_IDLE;
		pAd->Mlme.ActMachine.CurrState    = ACT_IDLE;
#endif // CONFIG_STA_SUPPORT //

		/*
			===> refer to MlmeRestartStateMachine().
			When we entered ATE_START mode, PeriodicTimer was not cancelled.
			So we don't have to set it here.
		*/
//		RTMPSetTimer(pAd, &pAd->Mlme.PeriodicTimer, MLME_TASK_EXEC_INTV);
		
		ASSERT(pAd->CommonCfg.Channel != 0);			

		AsicSwitchChannel(pAd, pAd->CommonCfg.Channel, FALSE);

		/* empty function */
		AsicLockChannel(pAd, pAd->CommonCfg.Channel);


#ifdef CONFIG_STA_SUPPORT 
	    RTMPStationStart(pAd);
#endif // CONFIG_STA_SUPPORT //
	}	

	/* Clear ATE Bulk in/out counter and continue setup. */
	InterlockedExchange(&pAd->BulkOutRemained, 0);				
	NdisAcquireSpinLock(&pAd->GenericLock);
	pAd->ContinBulkOut = FALSE;		
	pAd->ContinBulkIn = FALSE;
	NdisReleaseSpinLock(&pAd->GenericLock);		

	/* Wait 50ms to prevent next URB to bulkout during HW reset. */
	/* todo : remove this if not necessary */
	RTMPusecDelay(50000);

	pAd->ate.Mode = ATE_STOP;
#endif // RTMP_MAC_USB //

	/* restore RX_FILTR_CFG */

#ifdef CONFIG_STA_SUPPORT 
	/* restore RX_FILTR_CFG due to that QA maybe set it to 0x3 */
	RTMP_IO_WRITE32(pAd, RX_FILTR_CFG, STANORMAL);
#endif // CONFIG_STA_SUPPORT //

	/* Enable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Enable Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

	/* Enable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);


#ifdef RTMP_MAC_USB
	/* Wait 10ms for all of the bulk-in URBs to complete. */
	/* todo : remove this if not necessary */
	RTMPusecDelay(10000);

	/* Everything is ready to start normal Tx/Rx. */
	RTUSBBulkReceive(pAd);
#endif // RTMP_MAC_USB //
	RTMP_OS_NETDEV_START_QUEUE(pAd->net_dev);

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXCARR(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	UINT32			bbp_index=0;	 
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //
	UCHAR			BbpData = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	pAd->ate.Mode = ATE_TXCARR;

#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	/* RT35xx ATE will reuse this code segment. */
	for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		restore_BBP[bbp_index]=0;

	/* Record All BBP Value */
	for (bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		ATE_BBP_IO_READ8_BY_REG_ID(pAd,bbp_index,&restore_BBP[bbp_index]);
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //

#ifdef RTMP_MAC_USB
	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
#endif // RTMP_MAC_USB //

	/* QA has done the following steps if it is used. */
	if (pAd->ate.bQATxStart == FALSE) 
	{
		/* RT3883 does not need BbpSoftReset() */
		/* Soft reset BBP. */
		BbpSoftReset(pAd);

		/* Carrier Test set BBP R22 bit7=1, bit6=1, bit[5~0]=0x01 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= 0xFFFFFF00; // clear bit7, bit6, bit[5~0]
		BbpData |= 0x000000C1; // set bit7=1, bit6=1, bit[5~0]=0x01
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

		/* set MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 1 */
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
		MacData = MacData | 0x00000010;
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXCONT(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	UINT32			bbp_index=0;
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //
	UCHAR			BbpData = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	if (pAd->ate.bQATxStart == TRUE)
	{
		/*
			set MAC_SYS_CTRL(0x1004) bit4(Continuous Tx Production Test)
			and bit2(MAC TX enable) back to zero.
		*/ 
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
		MacData &= 0xFFFFFFEB;
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

		/* set BBP R22 bit7=0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= 0xFFFFFF7F; // set bit7=0
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
	}
#ifdef NEW_TXCONT 
	else
	{
		/* store the original value of RA_TX_PIN_CFG */
		RTMP_IO_READ32(pAd, RA_TX_PIN_CFG, &Default_TX_PIN_CFG);
	}
#endif // NEW_TXCONT //

#if defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883)
	/* RT35xx ATE will reuse this code segment. */
	for(bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		restore_BBP[bbp_index]=0;

	/* Record All BBP Value */
	for(bbp_index=0;bbp_index<ATE_BBP_REG_NUM;bbp_index++)
		ATE_BBP_IO_READ8_BY_REG_ID(pAd,bbp_index,&restore_BBP[bbp_index]);
#endif // defined(RT30xx) || defined(RT305x) || defined(RT3350) || defined(RT3352) || defined(RT3883) || defined(RT2883) //


	/* Step 1: send 50 packets first. */
	pAd->ate.Mode = ATE_TXCONT;
	pAd->ate.TxCount = 50;

	/* RT3883 does not need BbpSoftReset() */
	/* Soft reset BBP. */
	BbpSoftReset(pAd);

	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);


	/* Do it after Tx/Rx DMA is aborted. */
	pAd->ate.TxDoneCount = 0;
	
	/* Only needed if we have to send some normal frames. */
	SetJapanFilter(pAd);


#ifdef RTMP_MAC_USB
	/* Setup frame format. */
	ATESetUpFrame(pAd, 0);
#endif // RTMP_MAC_USB //

	/* Enable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Start Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

#ifdef RTMP_MAC_USB
	InterlockedExchange(&pAd->BulkOutRemained, pAd->ate.TxCount);
#endif // RTMP_MAC_USB //

#ifdef RALINK_QA
	if (pAd->ate.bQATxStart == TRUE)
	{
		pAd->ate.TxStatus = 1;
	}
#endif // RALINK_QA //


#ifdef RTMP_MAC_USB
	NdisAcquireSpinLock(&pAd->GenericLock);
	pAd->ContinBulkOut = FALSE;		
	NdisReleaseSpinLock(&pAd->GenericLock);

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	/* Kick bulk out */ 
	RTUSBKickBulkOut(pAd);

	/* To make sure all the 50 frames have been bulk out before executing step 2 */
	while (atomic_read(&pAd->BulkOutRemained) > 0)
	{
		RTMPusecDelay(5000);
	}
#endif // RTMP_MAC_USB //

#ifdef NEW_TXCONT
	/* give RA_TX_PIN_CFG(0x1328) a proper value. */
	MacData = TxCont_TX_PIN_CFG;
	RTMP_IO_WRITE32(pAd, RA_TX_PIN_CFG, MacData);

	/* Cont. TX set BBP R22 bit7=1 */
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
	BbpData |= 0x00000080; // set bit7=1
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
#else
	/* Step 2: send more 50 packets then start continue mode. */
	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	/* Cont. TX set BBP R22 bit7=1 */
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
	BbpData |= 0x00000080; // set bit7=1
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

	pAd->ate.TxCount = 50;

	pAd->ate.TxDoneCount = 0;
	SetJapanFilter(pAd);


#ifdef RTMP_MAC_USB
	/* Setup frame format. */
	ATESetUpFrame(pAd, 0);
#endif // RTMP_MAC_USB //

	/* Enable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Start Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);

#ifdef RTMP_MAC_USB
	InterlockedExchange(&pAd->BulkOutRemained, pAd->ate.TxCount);
#endif // RTMP_MAC_USB //
#ifdef RALINK_QA
	if (pAd->ate.bQATxStart == TRUE)
	{
		pAd->ate.TxStatus = 1;
	}
#endif // RALINK_QA //


#ifdef RTMP_MAC_USB
	NdisAcquireSpinLock(&pAd->GenericLock);
	pAd->ContinBulkOut = FALSE;		
	NdisReleaseSpinLock(&pAd->GenericLock);

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);
	/* Kick bulk out */
	RTUSBKickBulkOut(pAd);

	/* Let pAd->BulkOutRemained be consumed to zero. */
#endif // RTMP_MAC_USB //
	RTMPusecDelay(500);

	/* enable continuous tx production test */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= 0x00000010;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);							
#endif // NEW_TXCONT //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS TXFRAME(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0;
#ifdef RTMP_MAC_USB
	ULONG			IrqFlags = 0;
#endif // RTMP_MAC_USB //
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s(Count=%d)\n", __FUNCTION__, pAd->ate.TxCount));
	pAd->ate.Mode |= ATE_TXFRAME;


#ifdef RTMP_MAC_USB
	/* Soft reset BBP. */
	BbpSoftReset(pAd);

	/* Default value in BBP R22 is 0x0. */
	BbpData = 0;
   	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

	/* Clear bit4 to stop continuous Tx production test. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= 0xFFFFFFEF;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
#endif // RTMP_MAC_USB //

#ifdef RALINK_QA
	/* add this for LoopBack mode */
	if (pAd->ate.bQARxStart == FALSE)  
	{
		/* Disable Rx */
		RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
		MacData &= ~(1 << 3);
		RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
	}

	if (pAd->ate.bQATxStart == TRUE)  
	{
		pAd->ate.TxStatus = 1;
	}
#else
	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);
#endif // RALINK_QA //


#ifdef RTMP_MAC_USB
	/* Enable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

    SetJapanFilter(pAd);
	
	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	pAd->ate.TxDoneCount = 0;

    /* Setup frame format. */
	ATESetUpFrame(pAd, 0);	

	/* Start Tx, RX DMA. */
	RtmpDmaEnable(pAd, 1);

	/* Check count is continuous or not yet. */
	if (pAd->ate.TxCount == 0)
	{
		InterlockedExchange(&pAd->BulkOutRemained, 0);
	}
	else
	{
		InterlockedExchange(&pAd->BulkOutRemained, pAd->ate.TxCount);
	}

	DBGPRINT(RT_DEBUG_TRACE, ("bulk out count = %d\n", atomic_read(&pAd->BulkOutRemained)));
	ASSERT((atomic_read(&pAd->BulkOutRemained) >= 0));

	if (atomic_read(&pAd->BulkOutRemained) == 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Send packet continuously\n"));

		/* NdisAcquireSpinLock() == spin_lock_bh() */
		/* NdisAcquireSpinLock only need one argument. */
		NdisAcquireSpinLock(&pAd->GenericLock);
		pAd->ContinBulkOut = TRUE;		
		NdisReleaseSpinLock(&pAd->GenericLock);

		/* BULK_OUT_LOCK() == spin_lock_irqsave() */
		BULK_OUT_LOCK(&pAd->BulkOutLock[0], IrqFlags);
		pAd->BulkOutPending[0] = FALSE;
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[0], IrqFlags);
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Send packets depend on counter\n"));

		NdisAcquireSpinLock(&pAd->GenericLock);
		pAd->ContinBulkOut = FALSE;		
		NdisReleaseSpinLock(&pAd->GenericLock);

		BULK_OUT_LOCK(&pAd->BulkOutLock[0], IrqFlags);
		pAd->BulkOutPending[0] = FALSE;
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[0], IrqFlags);
	}				

	RTUSB_SET_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	/* Kick bulk out */ 
	RTUSBKickBulkOut(pAd);
#endif // RTMP_MAC_USB //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS RXFRAME(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;
#ifdef RTMP_MAC_USB
	UINT32			ring_index=0;
#endif // RTMP_MAC_USB //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	/* Disable Rx of MAC block */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	/* Default value in BBP R22 is 0x0. */
	BbpData = 0;
	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);


	/* Clear bit4 to stop continuous Tx production test. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= 0xFFFFFFEF;
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	pAd->ate.Mode |= ATE_RXFRAME;

#ifdef RTMP_MAC_USB
	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);
#endif // RTMP_MAC_USB //

	/* Disable Tx of MAC block. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

#ifdef RTMP_MAC_USB
    /* Reset Rx RING */
	for (ring_index = 0; ring_index < (RX_RING_SIZE); ring_index++)
	{
		PRX_CONTEXT  pRxContext = &(pAd->RxContext[ring_index]);

		pRxContext->InUse = FALSE;
		pRxContext->IRPPending = FALSE;
		pRxContext->Readable = FALSE;

		/* Get the URB from kernel(i.e., host control driver) back to driver. */
		RTUSB_UNLINK_URB(pRxContext->pUrb);

		/* Sleep 200 microsecs to give cancellation time to work. */
		RTMPusecDelay(200);
		pAd->BulkInReq = 0;

//		InterlockedExchange(&pAd->PendingRx, 0);
		pAd->PendingRx = 0;
		/* Next Rx Read index */
		pAd->NextRxBulkInReadIndex = 0;
		/* Rx Bulk pointer */
		pAd->NextRxBulkInIndex		= RX_RING_SIZE - 1;	
		pAd->NextRxBulkInPosition = 0;			
	}

	/* read to clear counters */
	RTUSBReadMACRegister(pAd, RX_STA_CNT0, &MacData); // RX PHY & RX CRC count
	RTUSBReadMACRegister(pAd, RX_STA_CNT1, &MacData); // RX PLCP error count & CCA false alarm count
	RTUSBReadMACRegister(pAd, RX_STA_CNT2, &MacData); // RX FIFO overflow frame count & RX duplicated filtered frame count

	pAd->ContinBulkIn = TRUE;

	/* Enable Tx, RX DMA. */
	RtmpDmaEnable(pAd, 1);
#endif // RTMP_MAC_USB //

	/* Enable Rx of MAC block. */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData |= (1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

#ifdef RTMP_MAC_USB
	/* Kick bulk in. */
	RTUSBBulkReceive(pAd);
#endif // RTMP_MAC_USB //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


#ifdef RALINK_QA
static NDIS_STATUS TXSTOP(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0, atemode=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;
	UCHAR			BbpData = 0;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));
	
	atemode = pAd->ate.Mode;
	pAd->ate.Mode &= ATE_TXSTOP;
	pAd->ate.bQATxStart = FALSE;

	if (atemode == ATE_TXCARR)
	{
		/* No Carrier Test set BBP R22 bit7=0, bit6=0, bit[5~0]=0x0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= 0xFFFFFF00; // clear bit7, bit6, bit[5~0]	
	    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
	}
	else if (atemode == ATE_TXCARRSUPP)
	{
		/* No Cont. TX set BBP R22 bit7=0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
		BbpData &= ~(1 << 7); // set bit7=0
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);

		/* No Carrier Suppression set BBP R24 bit0=0 */
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R24, &BbpData);
		BbpData &= 0xFFFFFFFE; // clear bit0	
	    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R24, BbpData);
	}		

	/*
		We should free some resource which was allocated
		when ATE_TXFRAME, ATE_STOP, and ATE_TXCONT.
	*/
	else if ((atemode & ATE_TXFRAME) || (atemode == ATE_STOP))
	{
		if (atemode == ATE_TXCONT)
		{
#ifndef NEW_TXCONT
			// No Cont. TX set BBP R22 bit7=0
			/* QA will do this in new TXCONT proposal */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R22, &BbpData);
			BbpData &= ~(1 << 7); //set bit7=0
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R22, BbpData);
#endif // !NEW_TXCONT //
		}


#ifdef RTMP_MAC_USB
	RTUSBRejectPendingPackets(pAd);
	RTUSBCleanUpDataBulkOutQueue(pAd);

//	RTUSBCleanUpMLMEWaitQueue(pAd);

	/* empty function so far */
	RTUSBCleanUpMLMEBulkOutQueue(pAd);

	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	/* pAd->PendingRx is not of type atomic_t anymore */
	/* pAd->BulkFlags != 0 : wait bulk out finish */
	/* todo : BulkInLock */
	while (pAd->PendingRx > 0)
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RTMPusecDelay(500000);
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))	
			/* pAd->BulkFlags != 0 : wait bulk out finish */
	{
		do 
		{
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);			

		RTMPusecDelay(500000);
	}

	ASSERT(pAd->PendingRx == 0);

	/* Enable Tx, Rx DMA. */
	RtmpDmaEnable(pAd, 1);
#endif // RTMP_MAC_USB //
	}

	/* task Tx status : 0 --> task is idle, 1 --> task is running */
	pAd->ate.TxStatus = 0;

#ifndef NEW_TXCONT
	/* Soft reset BBP. */
	BbpSoftReset(pAd);
#endif // !NEW_TXCONT //

	/* Disable Tx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 2);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

#ifdef RTMP_MAC_USB
	/* Clear ATE Bulk in/out counter and continue setup */
	InterlockedExchange(&pAd->BulkOutRemained, 0);
	pAd->ContinBulkOut = FALSE;
#endif // RTMP_MAC_USB //

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}


static NDIS_STATUS RXSTOP(
	IN PRTMP_ADAPTER pAd)
{
	UINT32			MacData=0;
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : ===> %s\n", __FUNCTION__));

	/* Disable Rx */
	RTMP_IO_READ32(pAd, MAC_SYS_CTRL, &MacData);
	MacData &= ~(1 << 3);
	RTMP_IO_WRITE32(pAd, MAC_SYS_CTRL, MacData);

	pAd->ate.Mode &= ATE_RXSTOP;
	pAd->ate.bQARxStart = FALSE;
//	pAd->ate.TxDoneCount = pAd->ate.TxCount;

#ifdef RTMP_MAC_USB
	RTUSBRejectPendingPackets(pAd);
	RTUSBCleanUpDataBulkOutQueue(pAd);

//	RTUSBCleanUpMLMEWaitQueue(pAd);	
	RTUSBCleanUpMLMEBulkOutQueue(pAd);

	/* Abort Tx, RX DMA. */
	RtmpDmaEnable(pAd, 0);

	while (pAd->PendingRx > 0)
	{
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RTMPusecDelay(500000);
	}

	while (((pAd->BulkOutPending[0] == TRUE) ||
			(pAd->BulkOutPending[1] == TRUE) || 
			(pAd->BulkOutPending[2] == TRUE) ||
			(pAd->BulkOutPending[3] == TRUE)) && (pAd->BulkFlags != 0))	
			/* pAd->BulkFlags != 0 : wait bulk out finish */
	{
		do
		{
			RTUSBCancelPendingBulkOutIRP(pAd);
		} while (FALSE);
		
		RTMPusecDelay(500000);
	}

	ASSERT(pAd->PendingRx == 0);
	pAd->ContinBulkIn = FALSE;
#endif // RTMP_MAC_USB //

	/* RT3883 does not need BbpSoftReset() */
	/* Soft reset BBP. */
	BbpSoftReset(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("ATE : <=== %s\n", __FUNCTION__));
	return Status;
}
#endif // RALINK_QA //


/*
==========================================================================
    Description:
        Set ATE operation mode to
        0. ATESTART  = Start ATE Mode
        1. ATESTOP   = Stop ATE Mode
        2. TXCARR    = Transmit Carrier
        3. TXCONT    = Continuous Transmit
        4. TXFRAME   = Transmit Frames
        5. RXFRAME   = Receive Frames
#ifdef RALINK_QA
        6. TXSTOP    = Stop Any Type of Transmition
        7. RXSTOP    = Stop Receiving Frames        
#endif // RALINK_QA //

    Return:
        NDIS_STATUS_SUCCESS if all parameters are OK.
==========================================================================
*/
static NDIS_STATUS	ATECmdHandler(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	NDIS_STATUS		Status = NDIS_STATUS_SUCCESS;

	DBGPRINT(RT_DEBUG_TRACE, ("===> %s\n", __FUNCTION__));

#ifdef	CONFIG_RT2880_ATE_CMD_NEW
	if (!strcmp(arg, "ATESTART")) 		
	{
                /* Enter/Reset ATE mode and set Tx/Rx Idle */
		Status = ATESTART(pAd);
	}
	else if (!strcmp(arg, "ATESTOP")) 
	{
                /* Leave ATE mode */
		Status = ATESTOP(pAd);
	}
#else
	if (!strcmp(arg, "APSTOP")) 		
	{
		Status = ATESTART(pAd);
	}
	else if (!strcmp(arg, "APSTART")) 
	{
		Status = ATESTOP(pAd);
	}
#endif
	else if (!strcmp(arg, "TXCARR"))	
	{
		ATEAsicSwitchChannel(pAd);

		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pAd->ate.Channel);

		RTMPusecDelay(5000);

		Status = TXCARR(pAd);
	}
	else if (!strcmp(arg, "TXCONT"))	
	{
		ATEAsicSwitchChannel(pAd);

		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pAd->ate.Channel);

		RTMPusecDelay(5000);

		Status = TXCONT(pAd);
	}
	else if (!strcmp(arg, "TXFRAME")) 
	{
		ATEAsicSwitchChannel(pAd);

		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pAd->ate.Channel);

		RTMPusecDelay(5000);
		Status = TXFRAME(pAd);
	}
	else if (!strcmp(arg, "RXFRAME")) 
	{
		ATEAsicSwitchChannel(pAd);

		/* AsicLockChannel() is empty function so far in fact */
		AsicLockChannel(pAd, pAd->ate.Channel);

		RTMPusecDelay(5000);
		Status = RXFRAME(pAd);
	}
#ifdef RALINK_QA
	/* Enter ATE mode and set Tx/Rx Idle */
	else if (!strcmp(arg, "TXSTOP"))
	{
		Status = TXSTOP(pAd);
	}
	else if (!strcmp(arg, "RXSTOP"))
	{
		Status = RXSTOP(pAd);
	}
#endif // RALINK_QA //
	else
	{	
		DBGPRINT(RT_DEBUG_TRACE, ("ATE : Invalid arg !\n"));
		Status = NDIS_STATUS_INVALID_DATA;
	}
	RTMPusecDelay(5000);

	DBGPRINT(RT_DEBUG_TRACE, ("<=== %s\n", __FUNCTION__));
	return Status;
}


INT	Set_ATE_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	// Handle ATEACTIVE and ATEPASSIVE commands as a special case
	if (!strcmp(arg, "ATEACTIVE"))
	{
		pAd->ate.PassiveMode = FALSE;
		return TRUE;
	}

	if (!strcmp(arg, "ATEPASSIVE"))
	{
		pAd->ate.PassiveMode = TRUE;
		return TRUE;
	}

	// Disallow all other ATE commands in passive mode
	if (pAd->ate.PassiveMode)
		return TRUE;

	if (ATECmdHandler(pAd, arg) == NDIS_STATUS_SUCCESS)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_Proc Success\n"));

		return TRUE;
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_Proc Failed\n"));
		return FALSE;
	}
}


/* 
==========================================================================
    Description:
        Set ATE ADDR1=DA for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR3=DA for TxFrame(STA : To DS = 1 ; From DS = 0)        
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_DA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	PSTRING				value;
	INT					i;

	// Mac address acceptable format 01:02:03:04:05:06 length 17	
	if (strlen(arg) != 17)  
		return FALSE;

    for (i = 0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pAd->ate.Addr3[i++], 1);
#endif // CONFIG_STA_SUPPORT //
	}

	/* sanity check */
	if (i != 6)
	{
		return FALSE;  
	}

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_DA_Proc (DA = %2X:%2X:%2X:%2X:%2X:%2X)\n", pAd->ate.Addr3[0],
		pAd->ate.Addr3[1], pAd->ate.Addr3[2], pAd->ate.Addr3[3], pAd->ate.Addr3[4], pAd->ate.Addr3[5]));
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_DA_Proc Success\n"));
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE ADDR3=SA for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR2=SA for TxFrame(STA : To DS = 1 ; From DS = 0)
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_SA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	PSTRING				value;
	INT					i;

	// Mac address acceptable format 01:02:03:04:05:06 length 17	
	if (strlen(arg) != 17)  
		return FALSE;

    for (i=0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pAd->ate.Addr2[i++], 1);
#endif // CONFIG_STA_SUPPORT //
	}

	/* sanity check */
	if (i != 6)
	{
		return FALSE;
	}

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_SA_Proc (SA = %2X:%2X:%2X:%2X:%2X:%2X)\n", pAd->ate.Addr2[0],
		pAd->ate.Addr2[1], pAd->ate.Addr2[2], pAd->ate.Addr2[3], pAd->ate.Addr2[4], pAd->ate.Addr2[5]));
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_SA_Proc Success\n"));

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE ADDR2=BSSID for TxFrame(AP  : To DS = 0 ; From DS = 1)
        or
        Set ATE ADDR1=BSSID for TxFrame(STA : To DS = 1 ; From DS = 0)

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_BSSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	PSTRING				value;
	INT					i;

	// Mac address acceptable format 01:02:03:04:05:06 length 17	
	if (strlen(arg) != 17)  
		return FALSE;

    for (i=0, value = rstrtok(arg, ":"); value; value = rstrtok(NULL, ":")) 
	{
		/* sanity check */
		if ((strlen(value) != 2) || (!isxdigit(*value)) || (!isxdigit(*(value+1))))
		{
			return FALSE;  
		}

#ifdef CONFIG_STA_SUPPORT
		AtoH(value, &pAd->ate.Addr1[i++], 1);
#endif // CONFIG_STA_SUPPORT //
	}

	/* sanity check */
	if(i != 6)
	{
		return FALSE;
	}

#ifdef CONFIG_STA_SUPPORT
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_BSSID_Proc (BSSID = %2X:%2X:%2X:%2X:%2X:%2X)\n",	pAd->ate.Addr1[0],
		pAd->ate.Addr1[1], pAd->ate.Addr1[2], pAd->ate.Addr1[3], pAd->ate.Addr1[4], pAd->ate.Addr1[5]));
#endif // CONFIG_STA_SUPPORT //

	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_BSSID_Proc Success\n"));

	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Channel

    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_CHANNEL_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR channel;

	channel = simple_strtol(arg, 0, 10);

	// to allow A band channel : ((channel < 1) || (channel > 14))
	if ((channel < 1) || (channel > 216))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_CHANNEL_Proc::Out of range, it should be in range of 1~14.\n"));
		return FALSE;
	}

		
	pAd->ate.Channel = channel;


	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_CHANNEL_Proc (ATE Channel = %d)\n", pAd->ate.Channel));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_CHANNEL_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Power0
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER0_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	CHAR TxPower;
	
	TxPower = simple_strtol(arg, 0, 10);

	if (pAd->ate.Channel <= 14)
	{
		if ((TxPower > 31) || (TxPower < 0))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_POWER0_Proc::Out of range (Value=%d)\n", TxPower));
			return FALSE;
		}
	}
	else/* 5.5 GHz */
	{
		if ((TxPower > 15) || (TxPower < -7))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_POWER0_Proc::Out of range (Value=%d)\n", TxPower));
			return FALSE;
		}
	}

	pAd->ate.TxPower0 = TxPower;
	ATETxPwrHandler(pAd, 0);


	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_POWER0_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx Power1
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_POWER1_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	CHAR TxPower;
	
	TxPower = simple_strtol(arg, 0, 10);

	if (pAd->ate.Channel <= 14)
	{
		if ((TxPower > 31) || (TxPower < 0))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_POWER1_Proc::Out of range (Value=%d)\n", TxPower));
			return FALSE;
		}
	}
	else
	{
		if ((TxPower > 15) || (TxPower < -7))
		{
			DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_POWER1_Proc::Out of range (Value=%d)\n", TxPower));
			return FALSE;
		}
	}

	pAd->ate.TxPower1 = TxPower;
	ATETxPwrHandler(pAd, 1);


	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_POWER1_Proc Success\n"));

	
	return TRUE;
}




/* 
==========================================================================
    Description:
        Set ATE Tx Antenna
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_Antenna_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	CHAR value;
	
	value = simple_strtol(arg, 0, 10);

	if ((value > 2) || (value < 0))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_Antenna_Proc::Out of range (Value=%d)\n", value));
		return FALSE;
	}

	pAd->ate.TxAntennaSel = value;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_Antenna_Proc (Antenna = %d)\n", pAd->ate.TxAntennaSel));
	DBGPRINT(RT_DEBUG_TRACE,("Ralink: Set_ATE_TX_Antenna_Proc Success\n"));

	/* calibration power unbalance issues */
	ATEAsicSwitchChannel(pAd);

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Rx Antenna
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_RX_Antenna_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	CHAR value;
	
	value = simple_strtol(arg, 0, 10);

	if ((value > 3) || (value < 0))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_RX_Antenna_Proc::Out of range (Value=%d)\n", value));
		return FALSE;
	}

	pAd->ate.RxAntennaSel = value;

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_RX_Antenna_Proc (Antenna = %d)\n", pAd->ate.RxAntennaSel));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_RX_Antenna_Proc Success\n"));

	/* calibration power unbalance issues */
	ATEAsicSwitchChannel(pAd);

	
	return TRUE;
}




/* 
==========================================================================
    Description:
        Set ATE RF frequence offset
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_FREQOFFSET_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR RFFreqOffset = 0;
	ULONG R4 = 0;
	
	RFFreqOffset = simple_strtol(arg, 0, 10);

	if (RFFreqOffset >= 64)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_FREQOFFSET_Proc::Out of range(0 ~ 63).\n"));
		return FALSE;
	}

	pAd->ate.RFFreqOffset = RFFreqOffset;

	{
		// RT28xx
		// shift TX power control to correct RF register bit position
		R4 = pAd->ate.RFFreqOffset << 15;		
		R4 |= (pAd->LatchRfRegs.R4 & ((~0x001f8000)));
		pAd->LatchRfRegs.R4 = R4;
		
		RtmpRfIoWrite(pAd);
	}
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_FREQOFFSET_Proc (RFFreqOffset = %d)\n", pAd->ate.RFFreqOffset));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_FREQOFFSET_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE RF BW
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_BW_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	INT powerIndex;
	UCHAR value = 0;
	UCHAR BBPCurrentBW;
	
	BBPCurrentBW = simple_strtol(arg, 0, 10);

	if ((BBPCurrentBW == 0)
		)
	{
		pAd->ate.TxWI.BW = BW_20;
	}
	else
	{
		pAd->ate.TxWI.BW = BW_40;
 	}

	/* RT35xx ATE will reuse this code segment. */
	// Fix the error spectrum of CCK-40MHZ.
	// Turn on BBP 20MHz mode by request here.
	if ((pAd->ate.TxWI.PHYMODE == MODE_CCK) && (pAd->ate.TxWI.BW == BW_40))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_BW_Proc!! Warning!! CCK only supports 20MHZ!!\nBandwidth switch to 20\n"));
		pAd->ate.TxWI.BW = BW_20;
	}

	if (pAd->ate.TxWI.BW == BW_20)
	{
		if (pAd->ate.Channel <= 14)
		{
 			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
 			{
				if (pAd->Tx20MPwrCfgGBand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgGBand[powerIndex]);	
				}
				RTMPusecDelay(5000);				
			}
		}
		else
		{
 			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
 			{
				if (pAd->Tx20MPwrCfgABand[powerIndex] == 0xffffffff)
					continue;

 				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx20MPwrCfgABand[powerIndex]);	
 				}
 				RTMPusecDelay(5000);				
 			}
		}
 
		// Set BBP R4 bit[4:3]=0:0
 		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
 		value &= (~0x18);
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);


		// Set BBP R66=0x3C
		value = 0x3C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, value);

		// Set BBP R68=0x0B
		// to improve Rx sensitivity.
		value = 0x0B;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, value);
		// Set BBP R69=0x16
		value = 0x16;
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, value);
		// Set BBP R70=0x08
		value = 0x08;
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, value);
		// Set BBP R73=0x11
		value = 0x11;
 		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, value);

		if (pAd->ate.Channel == 14)
		{
			INT TxMode = pAd->ate.TxWI.PHYMODE;

			if (TxMode == MODE_CCK)
			{
				// when Channel==14 && Mode==CCK && BandWidth==20M, BBP R4 bit5=1
 				ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
				value |= 0x20; //set bit5=1
 				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);				
			}
		}

		// set BW = 20 MHz
		{
			pAd->LatchRfRegs.R4 &= ~0x00200000;
			RtmpRfIoWrite(pAd);
		}
	}
	// If bandwidth = 40M, set RF Reg4 bit 21 = 0.
	else if (pAd->ate.TxWI.BW == BW_40)
	{
		if (pAd->ate.Channel <= 14)
		{
			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
			{
				if (pAd->Tx40MPwrCfgGBand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgGBand[powerIndex]);	
				}
					RTMPusecDelay(5000);				
			}
		}
		else
		{
			for (powerIndex=0; powerIndex<MAX_TXPOWER_ARRAY_SIZE; powerIndex++)
			{
				if (pAd->Tx40MPwrCfgABand[powerIndex] == 0xffffffff)
					continue;

				{
					RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + powerIndex*4, pAd->Tx40MPwrCfgABand[powerIndex]);	
				}
					RTMPusecDelay(5000);				
			}		

			if ((pAd->ate.TxWI.PHYMODE >= 2) && (pAd->ate.TxWI.MCS == 7))
			{
    			value = 0x28;
    			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R67, value);
			}
		}

		// Set BBP R4 bit[4:3]=1:0
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &value);
		value &= (~0x18);
		value |= 0x10;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, value);


		// Set BBP R66=0x3C
		value = 0x3C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, value);

		// Set BBP R68=0x0C
		// to improve Rx sensitivity
		value = 0x0C;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R68, value);
		// Set BBP R69=0x1A
		value = 0x1A;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, value);
		// Set BBP R70=0x0A
		value = 0x0A;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, value);
		// Set BBP R73=0x16
		value = 0x16;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R73, value);

		// If bandwidth = 40M, set RF Reg4 bit 21 = 1.
		// set BW = 40 MHz
		{
		pAd->LatchRfRegs.R4 |= 0x00200000;
		RtmpRfIoWrite(pAd);
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_BW_Proc (BBPCurrentBW = %d)\n", pAd->ate.TxWI.BW));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_BW_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame length
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_LENGTH_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	pAd->ate.TxLength = simple_strtol(arg, 0, 10);

	if ((pAd->ate.TxLength < 24) || (pAd->ate.TxLength > (MAX_FRAME_SIZE - 34/* == 2312 */)))
	{
		pAd->ate.TxLength = (MAX_FRAME_SIZE - 34/* == 2312 */);
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_LENGTH_Proc::Out of range, it should be in range of 24~%d.\n", (MAX_FRAME_SIZE - 34/* == 2312 */)));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_LENGTH_Proc (TxLength = %d)\n", pAd->ate.TxLength));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_LENGTH_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame count
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_COUNT_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	pAd->ate.TxCount = simple_strtol(arg, 0, 10);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_COUNT_Proc (TxCount = %d)\n", pAd->ate.TxCount));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_COUNT_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame MCS
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_MCS_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	UCHAR MCS;
	INT result;

	MCS = simple_strtol(arg, 0, 10);
	result = CheckMCSValid(pAd, pAd->ate.TxWI.PHYMODE, MCS);

	if (result != -1)
	{
		pAd->ate.TxWI.MCS = (UCHAR)MCS;
	}
	else
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_MCS_Proc::Out of range, refer to rate table.\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_MCS_Proc (MCS = %d)\n", pAd->ate.TxWI.MCS));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_MCS_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame Mode
        0: MODE_CCK
        1: MODE_OFDM
        2: MODE_HTMIX
        3: MODE_HTGREENFIELD
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_MODE_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	UCHAR BbpData = 0;

	pAd->ate.TxWI.PHYMODE = simple_strtol(arg, 0, 10);

	if (pAd->ate.TxWI.PHYMODE > 3)
	{
		pAd->ate.TxWI.PHYMODE = 0;
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_MODE_Proc::Out of range.\nIt should be in range of 0~3\n"));
		DBGPRINT(RT_DEBUG_ERROR, ("0: CCK, 1: OFDM, 2: HT_MIX, 3: HT_GREEN_FIELD.\n"));
		return FALSE;
	}

	// Turn on BBP 20MHz mode by request here.
	if (pAd->ate.TxWI.PHYMODE == MODE_CCK)
	{
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BbpData);
		BbpData &= (~0x18);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
		pAd->ate.TxWI.BW = BW_20;
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_MODE_Proc::CCK Only support 20MHZ. Switch to 20MHZ.\n"));
	}


	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_MODE_Proc (TxMode = %d)\n", pAd->ate.TxWI.PHYMODE));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_MODE_Proc Success\n"));

	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame GI
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_TX_GI_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{
	pAd->ate.TxWI.ShortGI = simple_strtol(arg, 0, 10);

	if (pAd->ate.TxWI.ShortGI > 1)
	{
		pAd->ate.TxWI.ShortGI = 0;
		DBGPRINT(RT_DEBUG_ERROR, ("Set_ATE_TX_GI_Proc::Out of range\n"));
		return FALSE;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_TX_GI_Proc (GI = %d)\n", pAd->ate.TxWI.ShortGI));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_TX_GI_Proc Success\n"));

	
	return TRUE;
}


INT	Set_ATE_RX_FER_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	pAd->ate.bRxFER = simple_strtol(arg, 0, 10);

	if (pAd->ate.bRxFER == 1)
	{
		pAd->ate.RxCntPerSec = 0;
		pAd->ate.RxTotalCnt = 0;
	}

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_RX_FER_Proc (bRxFER = %d)\n", pAd->ate.bRxFER));
	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_RX_FER_Proc Success\n"));

	
	return TRUE;
}


INT Set_ATE_Read_RF_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	{
		DBGPRINT(RT_DEBUG_OFF, ("R1 = %x\n", pAd->LatchRfRegs.R1));
		DBGPRINT(RT_DEBUG_OFF, ("R2 = %x\n", pAd->LatchRfRegs.R2));
		DBGPRINT(RT_DEBUG_OFF, ("R3 = %x\n", pAd->LatchRfRegs.R3));
		DBGPRINT(RT_DEBUG_OFF, ("R4 = %x\n", pAd->LatchRfRegs.R4));
	}
	return TRUE;
}


INT Set_ATE_Write_RF1_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);	

		pAd->LatchRfRegs.R1 = value;
		RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF2_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

		pAd->LatchRfRegs.R2 = value;
		RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF3_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

		pAd->LatchRfRegs.R3 = value;
		RtmpRfIoWrite(pAd);

	return TRUE;
}


INT Set_ATE_Write_RF4_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UINT32 value = (UINT32) simple_strtol(arg, 0, 16);

		pAd->LatchRfRegs.R4 = value;
		RtmpRfIoWrite(pAd);

	return TRUE;
}


/* 
==========================================================================
    Description:
        Load and Write EEPROM from a binary file prepared in advance.
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT Set_ATE_Load_E2P_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	BOOLEAN		    	ret = FALSE;
	PSTRING			src = EEPROM_BIN_FILE_NAME;
	RTMP_OS_FD		srcf;
	INT32 			retval;
	USHORT 			WriteEEPROM[(EEPROM_SIZE/2)];
	INT				FileLength = 0;
	UINT32 			value = (UINT32) simple_strtol(arg, 0, 10);
	RTMP_OS_FS_INFO	osFSInfo;

	DBGPRINT(RT_DEBUG_ERROR, ("===> %s (value=%d)\n\n", __FUNCTION__, value));

	if (value > 0)
	{
		/* zero the e2p buffer */
		NdisZeroMemory((PUCHAR)WriteEEPROM, EEPROM_SIZE);

		RtmpOSFSInfoChange(&osFSInfo, TRUE);

		do
		{
			/* open the bin file */
			srcf = RtmpOSFileOpen(src, O_RDONLY, 0);

			if (IS_FILE_OPEN_ERR(srcf)) 
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s - Error opening file %s\n", __FUNCTION__, src));
				break;
			}

			/* read the firmware from the file *.bin */
			FileLength = RtmpOSFileRead(srcf, (PSTRING)WriteEEPROM, EEPROM_SIZE);

			if (FileLength != EEPROM_SIZE)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("%s: error file length (=%d) in e2p.bin\n",
					   __FUNCTION__, FileLength));
				break;
			}
			else
			{
				/* write the content of .bin file to EEPROM */
				rt_ee_write_all(pAd, WriteEEPROM);
				ret = TRUE;
			}
			break;
		} while(TRUE);

		/* close firmware file */
		if (IS_FILE_OPEN_ERR(srcf))
		{
				;
		}
		else
		{
			retval = RtmpOSFileClose(srcf);			

			if (retval)
			{
				DBGPRINT(RT_DEBUG_ERROR, ("--> Error %d closing %s\n", -retval, src));
				
			} 
		}

		/* restore */
		RtmpOSFSInfoChange(&osFSInfo, FALSE);		
	}

    DBGPRINT(RT_DEBUG_ERROR, ("<=== %s (ret=%d)\n", __FUNCTION__, ret));

    return ret;
	
}




INT Set_ATE_Read_E2P_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	USHORT buffer[EEPROM_SIZE/2];
	USHORT *p;
	int i;
	
	rt_ee_read_all(pAd, (USHORT *)buffer);
	p = buffer;
	for (i = 0; i < (EEPROM_SIZE/2); i++)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%4.4x ", *p));
		if (((i+1) % 16) == 0)
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		p++;
	}
	return TRUE;
}




/* 
==========================================================================
    Description:
        Enable ATE auto Tx alc (Tx auto level control).
        According to the chip temperature, auto adjust the transmit power.  
        
        0: disable
        1: enable
        
        Return:
        	TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_AUTO_ALC_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UINT32 value = simple_strtol(arg, 0, 10);

	if (value > 0)
	{
		pAd->ate.bAutoTxAlc = TRUE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATEAUTOALC = TRUE , auto alc enabled!\n"));
	}
	else
	{
		pAd->ate.bAutoTxAlc = FALSE;
		DBGPRINT(RT_DEBUG_TRACE, ("ATEAUTOALC = FALSE , auto alc disabled!\n"));
	}	


	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE Tx frame IPG
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_IPG_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
    UINT32           data, value;

	pAd->ate.IPG = simple_strtol(arg, 0, 10);
	value = pAd->ate.IPG;

	RTMP_IO_READ32(pAd, XIFS_TIME_CFG, &data);

    if (value <= 0)
	{
		DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_IPG_Proc::IPG is disabled(IPG == 0).\n"));
		return TRUE;
	}

	ASSERT(value > 0);

    if ((value > 0) && (value < 256))
    {               
        RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &data);
        data &= 0x0;
        RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC1_CFG, &data);
        data &= 0x0;
        RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC2_CFG, &data);
        data &= 0x0;
        RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC3_CFG, &data);
        data &= 0x0;
        RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, data);
    }
    else
    {
        UINT32 aifsn, slottime;

        RTMP_IO_READ32(pAd, BKOFF_SLOT_CFG, &slottime);
        slottime &= 0x000000FF;

        aifsn = value / slottime;                  
        value = value % slottime;

        RTMP_IO_READ32(pAd, EDCA_AC0_CFG, &data);
        data &= 0x0;
        data |= (aifsn << 8);
        RTMP_IO_WRITE32(pAd, EDCA_AC0_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC1_CFG, &data);
        data &= 0x0;
        data |= (aifsn << 8);
        RTMP_IO_WRITE32(pAd, EDCA_AC1_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC2_CFG, &data);
        data &= 0x0;
        data |= (aifsn << 8);
        RTMP_IO_WRITE32(pAd, EDCA_AC2_CFG, data);

        RTMP_IO_READ32(pAd, EDCA_AC3_CFG, &data);
        data &= 0x0;
        data |= (aifsn << 8);
        RTMP_IO_WRITE32(pAd, EDCA_AC3_CFG, data);
    }

    data = (value & 0xFFFF0000) | value | (value << 8);
	RTMP_IO_WRITE32(pAd, XIFS_TIME_CFG, data);
	
	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_IPG_Proc (IPG = %u)\n", pAd->ate.IPG));


	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_IPG_Proc Success\n"));
	
	return TRUE;
}


/* 
==========================================================================
    Description:
        Set ATE payload pattern for TxFrame
        
    Return:
        TRUE if all parameters are OK, FALSE otherwise
==========================================================================
*/
INT	Set_ATE_Payload_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	PSTRING				value;

	value = arg;

	/* only one octet acceptable */	
	if (strlen(value) != 2)  
		return FALSE;

	AtoH(value, &(pAd->ate.Payload), 1);

	DBGPRINT(RT_DEBUG_TRACE, ("Set_ATE_Payload_Proc (repeated pattern = 0x%2x)\n", pAd->ate.Payload));


	DBGPRINT(RT_DEBUG_TRACE, ("Ralink: Set_ATE_Payload_Proc Success\n"));
	
	return TRUE;
}


INT	Set_ATE_Show_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_OFF, ("Mode=%u\n", pAd->ate.Mode));
	DBGPRINT(RT_DEBUG_OFF, ("TxPower0=%d\n", pAd->ate.TxPower0));
	DBGPRINT(RT_DEBUG_OFF, ("TxPower1=%d\n", pAd->ate.TxPower1));
	DBGPRINT(RT_DEBUG_OFF, ("TxAntennaSel=%d\n", pAd->ate.TxAntennaSel));
	DBGPRINT(RT_DEBUG_OFF, ("RxAntennaSel=%d\n", pAd->ate.RxAntennaSel));
	DBGPRINT(RT_DEBUG_OFF, ("BBPCurrentBW=%u\n", pAd->ate.TxWI.BW));
	DBGPRINT(RT_DEBUG_OFF, ("GI=%u\n", pAd->ate.TxWI.ShortGI));
	DBGPRINT(RT_DEBUG_OFF, ("MCS=%u\n", pAd->ate.TxWI.MCS));
	DBGPRINT(RT_DEBUG_OFF, ("TxMode=%u\n", pAd->ate.TxWI.PHYMODE));
	DBGPRINT(RT_DEBUG_OFF, ("Addr1=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr1[0], pAd->ate.Addr1[1], pAd->ate.Addr1[2], pAd->ate.Addr1[3], pAd->ate.Addr1[4], pAd->ate.Addr1[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Addr2=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr2[0], pAd->ate.Addr2[1], pAd->ate.Addr2[2], pAd->ate.Addr2[3], pAd->ate.Addr2[4], pAd->ate.Addr2[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Addr3=%02x:%02x:%02x:%02x:%02x:%02x\n",
		pAd->ate.Addr3[0], pAd->ate.Addr3[1], pAd->ate.Addr3[2], pAd->ate.Addr3[3], pAd->ate.Addr3[4], pAd->ate.Addr3[5]));
	DBGPRINT(RT_DEBUG_OFF, ("Channel=%u\n", pAd->ate.Channel));
	DBGPRINT(RT_DEBUG_OFF, ("TxLength=%u\n", pAd->ate.TxLength));
	DBGPRINT(RT_DEBUG_OFF, ("TxCount=%u\n", pAd->ate.TxCount));
	DBGPRINT(RT_DEBUG_OFF, ("RFFreqOffset=%u\n", pAd->ate.RFFreqOffset));
	DBGPRINT(RT_DEBUG_OFF, ("IPG=%u\n", pAd->ate.IPG));
	DBGPRINT(RT_DEBUG_OFF, ("Payload=0x%02x\n", pAd->ate.Payload));

	DBGPRINT(RT_DEBUG_OFF, ("Set_ATE_Show_Proc Success\n"));
	return TRUE;
}


INT	Set_ATE_Help_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_OFF, ("ATE=ATESTART, ATESTOP, TXCONT, TXCARR, TXFRAME, RXFRAME\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEDA\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATESA\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEBSSID\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATECHANNEL, range:0~14(unless A band !)\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXPOW0, set power level of antenna 1.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXPOW1, set power level of antenna 2.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXANT, set TX antenna. 0:all, 1:antenna one, 2:antenna two.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERXANT, set RX antenna.0:all, 1:antenna one, 2:antenna two, 3:antenna three.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXFREQOFFSET, set frequency offset, range 0~63\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXBW, set BandWidth, 0:20MHz, 1:40MHz.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXLEN, set Frame length, range 24~%d\n", (MAX_FRAME_SIZE - 34/* == 2312 */)));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXCNT, set how many frame going to transmit.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXMCS, set MCS, reference to rate table.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXMODE, set Mode 0:CCK, 1:OFDM, 2:HT-Mix, 3:GreenField, reference to rate table.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATETXGI, set GI interval, 0:Long, 1:Short\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERXFER, 0:disable Rx Frame error rate. 1:enable Rx Frame error rate.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERRF, show all RF registers.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF1, set RF1 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF2, set RF2 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF3, set RF3 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEWRF4, set RF4 register.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATELDE2P, load EEPROM from .bin file.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATERE2P, display all EEPROM content.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEIPG, set ATE Tx frame IPG.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEPAYLOAD, set ATE payload pattern for TxFrame.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATESHOW, display all parameters of ATE.\n"));
	DBGPRINT(RT_DEBUG_OFF, ("ATEHELP, online help.\n"));

	return TRUE;
}




/*
==========================================================================
    Description:

	AsicSwitchChannel() dedicated for ATE.
    
==========================================================================
*/

VOID ATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd) 
{
	UINT32 R2 = 0, R3 = DEFAULT_RF_TX_POWER, R4 = 0, Value = 0;
	RTMP_RF_REGS *RFRegTable = NULL;
	UCHAR index = 0, BbpValue = 0, R66 = 0x30, Channel = 0;
	CHAR TxPwer = 0, TxPwer2 = 0;


#ifdef RALINK_QA
	// for QA mode, TX power values are passed from UI
	if ((pAd->ate.bQATxStart == TRUE) || (pAd->ate.bQARxStart == TRUE))
	{
		if (pAd->ate.Channel != pAd->LatchRfRegs.Channel)			
		{
			pAd->ate.Channel = pAd->LatchRfRegs.Channel;
		}
		return;
	}
	else
#endif // RALINK_QA //
	Channel = pAd->ate.Channel;


	// fill Tx power value
	TxPwer = pAd->ate.TxPower0;
	TxPwer2 = pAd->ate.TxPower1;

	{
#if defined(RT28xx) || defined(RT2880) || defined(RT2883)
		/* RT28xx */
		RFRegTable = RF2850RegTable;

		switch (pAd->RfIcType)
		{
#if defined(RT28xx) || defined(RT2880)
			/* But only 2850 and 2750 support 5.5GHz band... */
			case RFIC_2820:
			case RFIC_2850:
			case RFIC_2720:
			case RFIC_2750:
#endif // defined(RT28xx) || defined(RT2880) //
				for (index = 0; index < NUM_OF_2850_CHNL; index++)
				{
					if (Channel == RFRegTable[index].Channel)
					{
						R2 = RFRegTable[index].R2;

						// If TX path is 1, bit 14 = 1;
						if (pAd->Antenna.field.TxPath == 1)
						{
							R2 |= 0x4000;	
						}
						if (pAd->Antenna.field.TxPath == 2)
						{
							if (pAd->ate.TxAntennaSel == 1)
							{
								// If TX Antenna select is 1 , bit 14 = 1; Disable Ant 2
								R2 |= 0x4000;	
								ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpValue);
								BbpValue &= 0xE7;		// 11100111B
								ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpValue);
							}
							else if (pAd->ate.TxAntennaSel == 2)
							{
								// If TX Antenna select is 2 , bit 15 = 1; Disable Ant 1
								R2 |= 0x8000;	
								ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpValue);
								BbpValue &= 0xE7;	
								BbpValue |= 0x08;
								ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpValue);
							}
							else
							{
								ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R1, &BbpValue);
								BbpValue &= 0xE7;
								BbpValue |= 0x10;
								ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R1, BbpValue);
							}
						}

						if (pAd->Antenna.field.RxPath == 2)
						{
							switch (pAd->ate.RxAntennaSel)
							{
								case 1:
									R2 |= 0x20040;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x00;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);								
									break;
								case 2:
									R2 |= 0x10040;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x01;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);									
									break;
								default:	
									R2 |= 0x40;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									/* Only enable two Antenna to receive. */
									BbpValue |= 0x08;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);								
									break;
							}
						}
						else if (pAd->Antenna.field.RxPath == 1)
						{
							// write 1 to off RxPath
							R2 |= 0x20040;	
						}

						if (pAd->Antenna.field.RxPath == 3)
						{
							switch (pAd->ate.RxAntennaSel)
							{
								case 1:
									R2 |= 0x20040;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x00;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);								
									break;
								case 2:
									R2 |= 0x10040;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x01;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);									
									break;
								case 3:	
									R2 |= 0x30000;
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x02;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);
									break;								
								default:	
									ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R3, &BbpValue);
									BbpValue &= 0xE4;
									BbpValue |= 0x10;
									ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R3, BbpValue);								
									break;
							}
						}
						
						if (Channel > 14)
						{
							// initialize R3, R4
							R3 = (RFRegTable[index].R3 & 0xffffc1ff);
							R4 = (RFRegTable[index].R4 & (~0x001f87c0)) | (pAd->ate.RFFreqOffset << 15);

		                    /*
			                    According the Rory's suggestion to solve the middle range issue.

								5.5G band power range : 0xF9~0X0F, TX0 Reg3 bit9/TX1 Reg4 bit6="0"
														means the TX power reduce 7dB.
							*/
							// R3
							if ((TxPwer >= -7) && (TxPwer < 0))
							{
								TxPwer = (7+TxPwer);
								TxPwer = (TxPwer > 0xF) ? (0xF) : (TxPwer);
								R3 |= (TxPwer << 10);
								DBGPRINT(RT_DEBUG_TRACE, ("ATEAsicSwitchChannel: TxPwer=%d \n", TxPwer));
							}
							else
							{
								TxPwer = (TxPwer > 0xF) ? (0xF) : (TxPwer);
								R3 |= (TxPwer << 10) | (1 << 9);
							}

							// R4
							if ((TxPwer2 >= -7) && (TxPwer2 < 0))
							{
								TxPwer2 = (7+TxPwer2);
								TxPwer2 = (TxPwer2 > 0xF) ? (0xF) : (TxPwer2);
								R4 |= (TxPwer2 << 7);
								DBGPRINT(RT_DEBUG_TRACE, ("ATEAsicSwitchChannel: TxPwer2=%d \n", TxPwer2));
							}
							else
							{
								TxPwer2 = (TxPwer2 > 0xF) ? (0xF) : (TxPwer2);
								R4 |= (TxPwer2 << 7) | (1 << 6);
							}
						}
						else
						{
							// Set TX power0.
							R3 = (RFRegTable[index].R3 & 0xffffc1ff) | (TxPwer << 9);
							// Set frequency offset and TX power1.
							R4 = (RFRegTable[index].R4 & (~0x001f87c0)) | (pAd->ate.RFFreqOffset << 15) | (TxPwer2 <<6);

						}

						// based on BBP current mode before changing RF channel
						if (pAd->ate.TxWI.BW == BW_40)
						{
							R4 |=0x200000;
						}
						
						// Update variables.
						pAd->LatchRfRegs.Channel = Channel;
						pAd->LatchRfRegs.R1 = RFRegTable[index].R1;
						pAd->LatchRfRegs.R2 = R2;
						pAd->LatchRfRegs.R3 = R3;
						pAd->LatchRfRegs.R4 = R4;

						RtmpRfIoWrite(pAd);
						
						break;
					}
				}
				break;

			default:
				break;
		}
#endif // defined(RT28xx) || defined(RT2880) || defined(RT2883) //
	}

	// Change BBP setting during switch from a->g, g->a
	if (Channel <= 14)
	{
	    UINT32 TxPinCfg = 0x00050F0A;// 2007.10.09 by Brian : 0x0005050A ==> 0x00050F0A
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - GET_LNA_GAIN(pAd)));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - GET_LNA_GAIN(pAd)));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - GET_LNA_GAIN(pAd)));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);//(0x44 - GET_LNA_GAIN(pAd)));	// According the Rory's suggestion to solve the middle range issue.
		//ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);

		/* For 1T/2R chip only... */
	    if (pAd->NicConfig2.field.ExternalLNAForG)
	    {
	        ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x62);
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
	    }
	    else
	    {
	        ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0x84);
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
		}

		// 5G band selection PIN, bit1 and bit2 are complement
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x6);
		Value |= (0x04);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

        // Turn off unused PA or LNA when only 1T or 1R.
		if (pAd->Antenna.field.TxPath == 1)
		{
			TxPinCfg &= 0xFFFFFFF3;
		}
		if (pAd->Antenna.field.RxPath == 1)
		{
			TxPinCfg &= 0xFFFFF3FF;
		}

		/* calibration power unbalance issues */
		if (pAd->Antenna.field.TxPath == 2)
		{
			if (pAd->ate.TxAntennaSel == 1)
			{
				TxPinCfg &= 0xFFFFFFF7;
			}
			else if (pAd->ate.TxAntennaSel == 2)
			{
				TxPinCfg &= 0xFFFFFFFD;
			}
		}

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}
	/* channel > 14 */
	else
	{
	    UINT32	TxPinCfg = 0x00050F05;// 2007.10.09 by Brian : 0x00050505 ==> 0x00050F05
		
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R62, (0x37 - GET_LNA_GAIN(pAd)));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R63, (0x37 - GET_LNA_GAIN(pAd)));
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R64, (0x37 - GET_LNA_GAIN(pAd)));
		{
		// According the Rory's suggestion to solve the middle range issue.
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R86, 0);//(0x44 - GET_LNA_GAIN(pAd)));        
				ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R82, 0xF2);
		}

		// Rx High power VGA offset for LNA select
		if (pAd->NicConfig2.field.ExternalLNAForA)
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x46);
		}
		else
		{
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R75, 0x50);
		}

		ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R91, &BbpValue);
		ASSERT((BbpValue == 0x04));

		// 5 G band selection PIN, bit1 and bit2 are complement
		RTMP_IO_READ32(pAd, TX_BAND_CFG, &Value);
		Value &= (~0x6);
		Value |= (0x02);
		RTMP_IO_WRITE32(pAd, TX_BAND_CFG, Value);

		// Turn off unused PA or LNA when only 1T or 1R.
		if (pAd->Antenna.field.TxPath == 1)
		{
			TxPinCfg &= 0xFFFFFFF3;
		}
		if (pAd->Antenna.field.RxPath == 1)
		{
			TxPinCfg &= 0xFFFFF3FF;
		}

		RTMP_IO_WRITE32(pAd, TX_PIN_CFG, TxPinCfg);
	}


    // R66 should be set according to Channel and use 20MHz when scanning
	if (Channel <= 14)
	{	
		// BG band
		R66 = 0x2E + GET_LNA_GAIN(pAd);
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
	}
	else
	{	
		// 5.5 GHz band
		if (pAd->ate.TxWI.BW == BW_20)
		{
			R66 = (UCHAR)(0x32 + (GET_LNA_GAIN(pAd)*5)/3);

	    		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
		}
		else
		{
			R66 = (UCHAR)(0x3A + (GET_LNA_GAIN(pAd)*5)/3);
			ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, R66);
		}
	}

	/*
		On 11A, We should delay and wait RF/BBP to be stable
		and the appropriate time should be 1000 micro seconds. 

		2005/06/05 - On 11G, We also need this delay time. Otherwise it's difficult to pass the WHQL.
	*/
	RTMPusecDelay(1000);  

	if (Channel > 14)
	{
		// When 5.5GHz band the LSB of TxPwr will be used to reduced 7dB or not.
		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, %dT) to , R1=0x%08x, R2=0x%08x, R3=0x%08x, R4=0x%08x\n",
								  Channel, 
								  pAd->RfIcType, 
								  pAd->Antenna.field.TxPath,
								  pAd->LatchRfRegs.R1, 
								  pAd->LatchRfRegs.R2, 
								  pAd->LatchRfRegs.R3, 
								  pAd->LatchRfRegs.R4));
	}
	else
	{
		DBGPRINT(RT_DEBUG_TRACE, ("SwitchChannel#%d(RF=%d, Pwr0=%u, Pwr1=%u, %dT) to , R1=0x%08x, R2=0x%08x, R3=0x%08x, R4=0x%08x\n",
								  Channel, 
								  pAd->RfIcType, 
								  (R3 & 0x00003e00) >> 9,
								  (R4 & 0x000007c0) >> 6,
								  pAd->Antenna.field.TxPath,
								  pAd->LatchRfRegs.R1, 
								  pAd->LatchRfRegs.R2, 
								  pAd->LatchRfRegs.R3, 
								  pAd->LatchRfRegs.R4));
    }
}


/*
==========================================================================
	Description:
		Gives CCK TX rate 2 more dB TX power.
		This routine works only in ATE mode.

		calculate desired Tx power in RF R3.Tx0~5,	should consider -
		0. if current radio is a noisy environment (pAd->DrsCounters.fNoisyEnvironment)
		1. TxPowerPercentage
		2. auto calibration based on TSSI feedback
		3. extra 2 db for CCK
		4. -10 db upon very-short distance (AvgRSSI >= -40db) to AP

	NOTE: Since this routine requires the value of (pAd->DrsCounters.fNoisyEnvironment),
		it should be called AFTER MlmeDynamicTxRateSwitching()
==========================================================================
*/
VOID ATEAsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd) 
{
	INT			i, j, maxTxPwrCnt;
	CHAR		DeltaPwr = 0;
	BOOLEAN		bAutoTxAgc = FALSE;
	UCHAR		TssiRef, *pTssiMinusBoundary, *pTssiPlusBoundary, TxAgcStep;
	UCHAR		BbpR49 = 0, idx;
	PCHAR		pTxAgcCompensate;
	ULONG		TxPwr[7];	// NOTE: the TxPwr array size should be the maxima value of all supported chipset!!!!
	CHAR		Value;

		maxTxPwrCnt = 5;

	/* no one calls this procedure so far */
	if (pAd->ate.TxWI.BW == BW_40)
	{
		if (pAd->ate.Channel > 14)
		{
			for (i =0 ; i < maxTxPwrCnt; i ++)
			{
				TxPwr[i] = pAd->Tx40MPwrCfgABand[i];	
			}
		}
		else
		{
			for (i =0 ; i < maxTxPwrCnt; i ++)
			{
				TxPwr[i] = pAd->Tx40MPwrCfgGBand[i];	
			}
		}
	}
	else
	{
		if (pAd->ate.Channel > 14)
		{
			for (i =0 ; i < maxTxPwrCnt; i ++)
			{
				TxPwr[i] = pAd->Tx20MPwrCfgABand[i];	
			}
		}
		else
		{
			for (i =0 ; i < maxTxPwrCnt; i ++)
			{
				TxPwr[i] = pAd->Tx20MPwrCfgGBand[i];	
			}
		}
	}

	// TX power compensation for temperature variation based on TSSI.
	// Do it per 4 seconds.
	if (pAd->Mlme.OneSecPeriodicRound % 4 == 0)
	{
		if (pAd->ate.Channel <= 14)
		{
			/* bg channel */
			bAutoTxAgc         = pAd->bAutoTxAgcG;
			TssiRef            = pAd->TssiRefG;
			pTssiMinusBoundary = &pAd->TssiMinusBoundaryG[0];
			pTssiPlusBoundary  = &pAd->TssiPlusBoundaryG[0];
			TxAgcStep          = pAd->TxAgcStepG;
			pTxAgcCompensate   = &pAd->TxAgcCompensateG;
		}
		else
		{
			/* a channel */
			bAutoTxAgc         = pAd->bAutoTxAgcA;
			TssiRef            = pAd->TssiRefA;
			pTssiMinusBoundary = &pAd->TssiMinusBoundaryA[0];
			pTssiPlusBoundary  = &pAd->TssiPlusBoundaryA[0];
			TxAgcStep          = pAd->TxAgcStepA;
			pTxAgcCompensate   = &pAd->TxAgcCompensateA;
		}

		if (bAutoTxAgc)
		{
			/* BbpR49 is unsigned char. */
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpR49);

			/* (p) TssiPlusBoundaryG[0] = 0 = (m) TssiMinusBoundaryG[0] */
			/* compensate: +4     +3   +2   +1    0   -1   -2   -3   -4 * steps */
			/* step value is defined in pAd->TxAgcStepG for tx power value */

			/* [4]+1+[4]   p4     p3   p2   p1   o1   m1   m2   m3   m4 */
			/* ex:         0x00 0x15 0x25 0x45 0x88 0xA0 0xB5 0xD0 0xF0
			   above value are examined in mass factory production */
			/*             [4]    [3]  [2]  [1]  [0]  [1]  [2]  [3]  [4] */

			/* plus is 0x10 ~ 0x40, minus is 0x60 ~ 0x90 */
			/* if value is between p1 ~ o1 or o1 ~ s1, no need to adjust tx power */
			/* if value is 0x65, tx power will be -= TxAgcStep*(2-1) */

			if (BbpR49 > pTssiMinusBoundary[1])
			{
				// Reading is larger than the reference value.
				// Check for how large we need to decrease the Tx power.
				for (idx = 1; idx < 5; idx++)
				{
					// Found the range.
					if (BbpR49 <= pTssiMinusBoundary[idx])  
						break;
				}

				// The index is the step we should decrease, idx = 0 means there is nothing to compensate.
//				if (R3 > (ULONG) (TxAgcStep * (idx-1)))
					*pTxAgcCompensate = -(TxAgcStep * (idx-1));
//				else
//					*pTxAgcCompensate = -((UCHAR)R3);
				
				DeltaPwr += (*pTxAgcCompensate);
				DBGPRINT(RT_DEBUG_TRACE, ("-- Tx Power, BBP R1=%x, TssiRef=%x, TxAgcStep=%x, step = -%d\n",
					BbpR49, TssiRef, TxAgcStep, idx-1));                    
			}
			else if (BbpR49 < pTssiPlusBoundary[1])
			{
				// Reading is smaller than the reference value.
				// Check for how large we need to increase the Tx power.
				for (idx = 1; idx < 5; idx++)
				{
					// Found the range.
					if (BbpR49 >= pTssiPlusBoundary[idx])   
						break;
				}

				// The index is the step we should increase, idx = 0 means there is nothing to compensate.
				*pTxAgcCompensate = TxAgcStep * (idx-1);
				DeltaPwr += (*pTxAgcCompensate);
				DBGPRINT(RT_DEBUG_TRACE, ("++ Tx Power, BBP R1=%x, TssiRef=%x, TxAgcStep=%x, step = +%d\n",
					BbpR49, TssiRef, TxAgcStep, idx-1));
			}
			else
			{
				*pTxAgcCompensate = 0;
				DBGPRINT(RT_DEBUG_TRACE, ("   Tx Power, BBP R1=%x, TssiRef=%x, TxAgcStep=%x, step = +%d\n",
					BbpR49, TssiRef, TxAgcStep, 0));
			}
		}
	}
	else
	{
		if (pAd->ate.Channel <= 14)
		{
			bAutoTxAgc         = pAd->bAutoTxAgcG;
			pTxAgcCompensate   = &pAd->TxAgcCompensateG;
		}
		else
		{
			bAutoTxAgc         = pAd->bAutoTxAgcA;
			pTxAgcCompensate   = &pAd->TxAgcCompensateA;
		}

		if (bAutoTxAgc)
			DeltaPwr += (*pTxAgcCompensate);
	}

	/* Calculate delta power based on the percentage specified from UI. */
	// E2PROM setting is calibrated for maximum TX power (i.e. 100%)
	// We lower TX power here according to the percentage specified from UI.
	if (pAd->CommonCfg.TxPowerPercentage == 0xffffffff)       // AUTO TX POWER control
		;
	else if (pAd->CommonCfg.TxPowerPercentage > 90)  // 91 ~ 100% & AUTO, treat as 100% in terms of mW
		;
	else if (pAd->CommonCfg.TxPowerPercentage > 60)  // 61 ~ 90%, treat as 75% in terms of mW
	{
		DeltaPwr -= 1;
	}
	else if (pAd->CommonCfg.TxPowerPercentage > 30)  // 31 ~ 60%, treat as 50% in terms of mW
	{
		DeltaPwr -= 3;
	}
	else if (pAd->CommonCfg.TxPowerPercentage > 15)  // 16 ~ 30%, treat as 25% in terms of mW
	{
		DeltaPwr -= 6;
	}
	else if (pAd->CommonCfg.TxPowerPercentage > 9)   // 10 ~ 15%, treat as 12.5% in terms of mW
	{
		DeltaPwr -= 9;
	}
	else                                           // 0 ~ 9 %, treat as MIN(~3%) in terms of mW
	{
		DeltaPwr -= 12;
	}

	/* Reset different new tx power for different TX rate. */
	for (i=0; i<maxTxPwrCnt; i++)
	{
		if (TxPwr[i] != 0xffffffff)
		{
			for (j=0; j<8; j++)
			{
				Value = (CHAR)((TxPwr[i] >> j*4) & 0x0F); /* 0 ~ 15 */

				if ((Value + DeltaPwr) < 0)
				{
					Value = 0; /* min */
				}
				else if ((Value + DeltaPwr) > 0xF)
				{
					Value = 0xF; /* max */
				}
				else
				{
					Value += DeltaPwr; /* temperature compensation */
				}

				/* fill new value to CSR offset */
				TxPwr[i] = (TxPwr[i] & ~(0x0000000F << j*4)) | (Value << j*4);
			}

			/* write tx power value to CSR */
			/* TX_PWR_CFG_0 (8 tx rate) for	TX power for OFDM 12M/18M
											TX power for OFDM 6M/9M
											TX power for CCK5.5M/11M
											TX power for CCK1M/2M */
			/* TX_PWR_CFG_1 ~ TX_PWR_CFG_4 */
			RTMP_IO_WRITE32(pAd, TX_PWR_CFG_0 + i*4, TxPwr[i]);

			
		}
	}

}


/*
========================================================================
	Routine Description:
		Write TxWI for ATE mode.
		
	Return Value:
		None
========================================================================
*/


#ifdef RTMP_MAC_USB
static VOID ATEWriteTxWI(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXWI_STRUC 	pTxWI,
	IN	BOOLEAN			FRAG,	
	IN	BOOLEAN			InsTimestamp,
	IN	BOOLEAN 		AMPDU,
	IN	BOOLEAN 		Ack,
	IN	BOOLEAN 		NSeq,		// HW new a sequence.
	IN	UCHAR			BASize,
	IN	UCHAR			WCID,
	IN	ULONG			Length,
	IN	UCHAR 			PID,
	IN	UCHAR			MIMOps,
	IN	UCHAR			Txopmode,	
	IN	BOOLEAN			CfAck,	
	IN	HTTRANSMIT_SETTING	Transmit)
{
	//
	// Always use Long preamble before verifiation short preamble functionality works well.
	// Todo: remove the following line if short preamble functionality works
	//
	OPSTATUS_CLEAR_FLAG(pAd, fOP_STATUS_SHORT_PREAMBLE_INUSED);
	pTxWI->FRAG= FRAG;
	pTxWI->TS= InsTimestamp;
	pTxWI->AMPDU = AMPDU;

	pTxWI->MIMOps = PWR_ACTIVE;
	pTxWI->MpduDensity = 4;
	pTxWI->ACK = Ack;
	pTxWI->txop = Txopmode;
	pTxWI->NSEQ = NSeq;
	pTxWI->BAWinSize = BASize;	

	pTxWI->WirelessCliID = WCID;
	pTxWI->MPDUtotalByteCount = Length; 
	pTxWI->PacketId = PID; 
	
	pTxWI->BW = Transmit.field.BW;
	pTxWI->ShortGI = Transmit.field.ShortGI;
	pTxWI->STBC= Transmit.field.STBC;
	
	pTxWI->MCS = Transmit.field.MCS;
	pTxWI->PHYMODE= Transmit.field.MODE;
	pTxWI->CFACK = CfAck;

	return;
}
#endif // RTMP_MAC_USB //


/*
========================================================================

	Routine Description:
		Disable protection for ATE.
========================================================================
*/
VOID ATEDisableAsicProtect(
	IN		PRTMP_ADAPTER	pAd)
{
	PROT_CFG_STRUC	ProtCfg, ProtCfg4;
	UINT32 Protect[6];
	USHORT			offset;
	UCHAR			i;
	UINT32 MacReg = 0;

	// Config ASIC RTS threshold register
	RTMP_IO_READ32(pAd, TX_RTS_CFG, &MacReg);
	MacReg &= 0xFF0000FF;
	MacReg |= (0xFFF << 8);
	RTMP_IO_WRITE32(pAd, TX_RTS_CFG, MacReg);

	// Initial common protection settings
	RTMPZeroMemory(Protect, sizeof(Protect));
	ProtCfg4.word = 0;
	ProtCfg.word = 0;
	ProtCfg.field.TxopAllowGF40 = 1;
	ProtCfg.field.TxopAllowGF20 = 1;
	ProtCfg.field.TxopAllowMM40 = 1;
	ProtCfg.field.TxopAllowMM20 = 1;
	ProtCfg.field.TxopAllowOfdm = 1;
	ProtCfg.field.TxopAllowCck = 1;
	ProtCfg.field.RTSThEn = 1;
	ProtCfg.field.ProtectNav = ASIC_SHORTNAV;

	// Handle legacy(B/G) protection
	ProtCfg.field.ProtectRate = pAd->CommonCfg.RtsRate;
	ProtCfg.field.ProtectCtrl = 0;
	Protect[0] = ProtCfg.word;
	Protect[1] = ProtCfg.word;
	/* CTS-self is not used */
	pAd->FlgCtsEnabled = 0; 

	// NO PROTECT 
	// 1.All STAs in the BSS are 20/40 MHz HT
	// 2. in ai 20/40MHz BSS
	// 3. all STAs are 20MHz in a 20MHz BSS
	// Pure HT. no protection.

	// MM20_PROT_CFG
	//	Reserved (31:27)
	// 	PROT_TXOP(25:20) -- 010111
	//	PROT_NAV(19:18)  -- 01 (Short NAV protection)
	//  PROT_CTRL(17:16) -- 00 (None)
	// 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)
	Protect[2] = 0x01744004;	

	// MM40_PROT_CFG
	//	Reserved (31:27)
	// 	PROT_TXOP(25:20) -- 111111
	//	PROT_NAV(19:18)  -- 01 (Short NAV protection)
	//  PROT_CTRL(17:16) -- 00 (None) 
	// 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)
	Protect[3] = 0x03f44084;

	// CF20_PROT_CFG
	//	Reserved (31:27)
	// 	PROT_TXOP(25:20) -- 010111
	//	PROT_NAV(19:18)  -- 01 (Short NAV protection)
	//  PROT_CTRL(17:16) -- 00 (None)
	// 	PROT_RATE(15:0)  -- 0x4004 (OFDM 24M)
	Protect[4] = 0x01744004;

	// CF40_PROT_CFG
	//	Reserved (31:27)
	// 	PROT_TXOP(25:20) -- 111111
	//	PROT_NAV(19:18)  -- 01 (Short NAV protection)
	//  PROT_CTRL(17:16) -- 00 (None)
	// 	PROT_RATE(15:0)  -- 0x4084 (duplicate OFDM 24M)
	Protect[5] = 0x03f44084;

	pAd->CommonCfg.IOTestParm.bRTSLongProtOn = FALSE;
	
	offset = CCK_PROT_CFG;
	for (i = 0;i < 6;i++)
		RTMP_IO_WRITE32(pAd, offset + i*4, Protect[i]);

}


#ifdef RTMP_MAC_USB
/*
========================================================================
	Routine	Description:
		Write TxInfo for ATE mode.
		
	Return Value:
		None
========================================================================
*/
static VOID ATEWriteTxInfo(
	IN	PRTMP_ADAPTER	pAd,
	IN	PTXINFO_STRUC 	pTxInfo,
	IN	USHORT		USBDMApktLen,
	IN	BOOLEAN		bWiv,
	IN	UCHAR			QueueSel,
	IN	UCHAR			NextValid,
	IN	UCHAR			TxBurst)
{
	pTxInfo->USBDMATxPktLen = USBDMApktLen;
	pTxInfo->QSEL = QueueSel;

	if (QueueSel != FIFO_EDCA)
		DBGPRINT(RT_DEBUG_TRACE, ("=======> QueueSel != FIFO_EDCA<=======\n"));

	pTxInfo->USBDMANextVLD = NextValid;
	pTxInfo->USBDMATxburst = TxBurst;
	pTxInfo->WIV = bWiv;
	pTxInfo->SwUseLastRound = 0;
	pTxInfo->rsv = 0;
	pTxInfo->rsv2 = 0;

	return;
}
#endif // RTMP_MAC_USB //


CHAR ATEConvertToRssi(
	IN PRTMP_ADAPTER pAd,
	IN	CHAR	Rssi,
	IN  UCHAR   RssiNumber)
{
	UCHAR	RssiOffset, LNAGain;

	// Rssi equals to zero should be an invalid value
	if (Rssi == 0)
		return -99;
	
	LNAGain = GET_LNA_GAIN(pAd);
	if (pAd->LatchRfRegs.Channel > 14)
	{
		if (RssiNumber == 0)
			RssiOffset = pAd->ARssiOffset0;
		else if (RssiNumber == 1)
			RssiOffset = pAd->ARssiOffset1;
		else
			RssiOffset = pAd->ARssiOffset2;
	}
	else
	{
		if (RssiNumber == 0)
			RssiOffset = pAd->BGRssiOffset0;
		else if (RssiNumber == 1)
			RssiOffset = pAd->BGRssiOffset1;
		else
			RssiOffset = pAd->BGRssiOffset2;
	}

	return (-12 - RssiOffset - LNAGain - Rssi);
}


/*
========================================================================

	Routine Description:
		Set Japan filter coefficients if needed.
	Note:
		This routine should only be called when
		entering TXFRAME mode or TXCONT mode.
				
========================================================================
*/
static VOID SetJapanFilter(
	IN		PRTMP_ADAPTER	pAd)
{
	UCHAR			BbpData = 0;
	
	//
	// If Channel=14 and Bandwidth=20M and Mode=CCK, set BBP R4 bit5=1
	// (Japan Tx filter coefficients)when (TXFRAME or TXCONT).
	//
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R4, &BbpData);

    if ((pAd->ate.TxWI.PHYMODE == MODE_CCK) && (pAd->ate.Channel == 14) && (pAd->ate.TxWI.BW == BW_20))
    {
        BbpData |= 0x20;    // turn on
        DBGPRINT(RT_DEBUG_TRACE, ("SetJapanFilter!!!\n"));
    }
    else
    {
		BbpData &= 0xdf;    // turn off
		DBGPRINT(RT_DEBUG_TRACE, ("ClearJapanFilter!!!\n"));
    }

	ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R4, BbpData);
}


VOID ATESampleRssi(
	IN PRTMP_ADAPTER	pAd,
	IN PRXWI_STRUC		pRxWI)
{
	if (pRxWI->RSSI0 != 0)
	{
		pAd->ate.LastRssi0	= ATEConvertToRssi(pAd, (CHAR) pRxWI->RSSI0, RSSI_0);
		pAd->ate.AvgRssi0X8	= (pAd->ate.AvgRssi0X8 - pAd->ate.AvgRssi0) + pAd->ate.LastRssi0;
		pAd->ate.AvgRssi0  	= pAd->ate.AvgRssi0X8 >> 3;
	}
	if (pRxWI->RSSI1 != 0)
	{
		pAd->ate.LastRssi1	= ATEConvertToRssi(pAd, (CHAR) pRxWI->RSSI1, RSSI_1);
		pAd->ate.AvgRssi1X8	= (pAd->ate.AvgRssi1X8 - pAd->ate.AvgRssi1) + pAd->ate.LastRssi1;
		pAd->ate.AvgRssi1	= pAd->ate.AvgRssi1X8 >> 3;
	}
	if (pRxWI->RSSI2 != 0)
	{
		pAd->ate.LastRssi2	= ATEConvertToRssi(pAd, (CHAR) pRxWI->RSSI2, RSSI_2);
		pAd->ate.AvgRssi2X8	= (pAd->ate.AvgRssi2X8 - pAd->ate.AvgRssi2) + pAd->ate.LastRssi2;
		pAd->ate.AvgRssi2	= pAd->ate.AvgRssi2X8 >> 3;
	}

	pAd->ate.LastSNR0 = (CHAR)(pRxWI->SNR0);// CHAR ==> UCHAR ?
	pAd->ate.LastSNR1 = (CHAR)(pRxWI->SNR1);// CHAR ==> UCHAR ?

	pAd->ate.NumOfAvgRssiSample ++;
}


#ifdef CONFIG_STA_SUPPORT
VOID RTMPStationStop(
    IN  PRTMP_ADAPTER   pAd)
{
	
    DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPStationStop\n"));

	// For rx statistics, we need to keep this timer running.
//	RTMPCancelTimer(&pAd->Mlme.PeriodicTimer,      &Cancelled);

    DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPStationStop\n"));
}


VOID RTMPStationStart(
    IN  PRTMP_ADAPTER   pAd)
{
    DBGPRINT(RT_DEBUG_TRACE, ("==> RTMPStationStart\n"));


	DBGPRINT(RT_DEBUG_TRACE, ("<== RTMPStationStart\n"));
}
#endif // CONFIG_STA_SUPPORT //


/* 
==========================================================================
	Description:
		Setup Frame format.
	NOTE:
		This routine should only be used in ATE mode.
==========================================================================
*/


#ifdef RTMP_MAC_USB
/*======================Start of RTMP_MAC_USB ======================*/
static INT ATESetUpFrame(
	IN PRTMP_ADAPTER pAd,
	IN UINT32 TxIdx)
{
	UINT j;
	PTX_CONTEXT	pNullContext;
	PUCHAR			pDest;
	HTTRANSMIT_SETTING	TxHTPhyMode;
	PTXWI_STRUC		pTxWI;
	PTXINFO_STRUC		pTxInfo;	
	UINT32			TransferBufferLength, OrgBufferLength = 0;
	UCHAR			padLen = 0;
#ifdef RALINK_QA
	PHEADER_802_11	pHeader80211 = NULL;
#endif // RALINK_QA //
	
	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BULKOUT_RESET)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_HALT_IN_PROGRESS)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		return -1;
	}

	/* We always use QID_AC_BE and FIFO_EDCA in ATE mode. */

	pNullContext = &(pAd->NullContext);
	ASSERT(pNullContext != NULL);
	
	if (pNullContext->InUse == FALSE)
	{
		// set the in use bit
		pNullContext->InUse = TRUE;
		NdisZeroMemory(&(pAd->NullFrame), sizeof(HEADER_802_11));
		
		// fill 802.11 header
#ifdef RALINK_QA
		if (pAd->ate.bQATxStart == TRUE) 
		{
			pHeader80211 = NdisMoveMemory(&(pAd->NullFrame), pAd->ate.Header, pAd->ate.HLen);
//			pDest = NdisMoveMemory(&(pAd->NullFrame), pAd->ate.Header, pAd->ate.HLen);
//			pHeader80211 = (PHEADER_802_11)pDest;
		}
		else
#endif // RALINK_QA //
		{
			// fill 802.11 header
			NdisMoveMemory(&(pAd->NullFrame), TemplateFrame, sizeof(HEADER_802_11));
		}

#ifdef RT_BIG_ENDIAN
		RTMPFrameEndianChange(pAd, (PUCHAR)&(pAd->NullFrame), DIR_READ, FALSE);
#endif // RT_BIG_ENDIAN //

#ifdef RALINK_QA
		if (pAd->ate.bQATxStart == TRUE) 
		{
			/* modify sequence number... */
			if (pAd->ate.TxDoneCount == 0)
			{
				pAd->ate.seq = pHeader80211->Sequence;
			}
			else
			{
				pHeader80211->Sequence = ++pAd->ate.seq;
			}
			/* We already got all the address fields from QA GUI. */
		}
		else
#endif // RALINK_QA //
		{
			COPY_MAC_ADDR(pAd->NullFrame.Addr1, pAd->ate.Addr1);
			COPY_MAC_ADDR(pAd->NullFrame.Addr2, pAd->ate.Addr2);
			COPY_MAC_ADDR(pAd->NullFrame.Addr3, pAd->ate.Addr3);
		}

		RTMPZeroMemory(&pAd->NullContext.TransferBuffer->field.WirelessPacket[0], TX_BUFFER_NORMSIZE);
		pTxInfo = (PTXINFO_STRUC)&pAd->NullContext.TransferBuffer->field.WirelessPacket[0];

#ifdef RALINK_QA
		if (pAd->ate.bQATxStart == TRUE) 
		{
			// Avoid to exceed the range of WirelessPacket[].
			ASSERT(pAd->ate.TxInfo.USBDMATxPktLen <= (MAX_FRAME_SIZE - 34/* == 2312 */));
			NdisMoveMemory(pTxInfo, &(pAd->ate.TxInfo), sizeof(pAd->ate.TxInfo));
		}
		else
#endif // RALINK_QA //
		{
			// Avoid to exceed the range of WirelessPacket[].
			ASSERT(pAd->ate.TxLength <= (MAX_FRAME_SIZE - 34/* == 2312 */));

			// pTxInfo->USBDMATxPktLen will be updated to include padding later
			ATEWriteTxInfo(pAd, pTxInfo, (USHORT)(TXWI_SIZE + pAd->ate.TxLength)
			, TRUE, EpToQueue[MGMTPIPEIDX], FALSE,  FALSE);
			pTxInfo->QSEL = FIFO_EDCA;
		}

		pTxWI = (PTXWI_STRUC)&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE];

		// fill TxWI
		if (pAd->ate.bQATxStart == TRUE) 
		{
			TxHTPhyMode.field.BW = pAd->ate.TxWI.BW;
			TxHTPhyMode.field.ShortGI = pAd->ate.TxWI.ShortGI;
			TxHTPhyMode.field.STBC = pAd->ate.TxWI.STBC;
			TxHTPhyMode.field.MCS = pAd->ate.TxWI.MCS;
			TxHTPhyMode.field.MODE = pAd->ate.TxWI.PHYMODE;
			ATEWriteTxWI(pAd, pTxWI, pAd->ate.TxWI.FRAG, pAd->ate.TxWI.TS,
				pAd->ate.TxWI.AMPDU, pAd->ate.TxWI.ACK, pAd->ate.TxWI.NSEQ, 
				pAd->ate.TxWI.BAWinSize, BSSID_WCID,
				pAd->ate.TxWI.MPDUtotalByteCount/* include 802.11 header */,
				pAd->ate.TxWI.PacketId,
				0, pAd->ate.TxWI.txop/*IFS_HTTXOP*/, pAd->ate.TxWI.CFACK
				/*FALSE*/, TxHTPhyMode);
		}
		else
		{
			TxHTPhyMode.field.BW = pAd->ate.TxWI.BW;
			TxHTPhyMode.field.ShortGI = pAd->ate.TxWI.ShortGI;
			TxHTPhyMode.field.STBC = 0;
			TxHTPhyMode.field.MCS = pAd->ate.TxWI.MCS;
			TxHTPhyMode.field.MODE = pAd->ate.TxWI.PHYMODE;

			ATEWriteTxWI(pAd, pTxWI,  FALSE, FALSE, FALSE, FALSE
				/* No ack required. */, FALSE, 0, BSSID_WCID, pAd->ate.TxLength,
				0, 0, IFS_HTTXOP, FALSE, TxHTPhyMode);// "MMPS_STATIC" instead of "MMPS_DYNAMIC" ???
		}

		RTMPMoveMemory(&pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE+TXWI_SIZE],
			&pAd->NullFrame, sizeof(HEADER_802_11));

		pDest = &(pAd->NullContext.TransferBuffer->field.WirelessPacket[TXINFO_SIZE+TXWI_SIZE+sizeof(HEADER_802_11)]);

		/* prepare frame payload */
#ifdef RALINK_QA
		if (pAd->ate.bQATxStart == TRUE) 
		{
			/* copy the pattern one by one to the frame payload */
			if ((pAd->ate.PLen != 0))
			{
				for (j = 0; j < pAd->ate.DLen; j+=pAd->ate.PLen)
				{
					RTMPMoveMemory(pDest, pAd->ate.Pattern, pAd->ate.PLen);
					pDest += pAd->ate.PLen;
				}
			}
			TransferBufferLength = TXINFO_SIZE + TXWI_SIZE + pAd->ate.TxWI.MPDUtotalByteCount;
		}
		else
#endif // RALINK_QA //
		{
		    for (j = 0; j < (pAd->ate.TxLength - sizeof(HEADER_802_11)); j++)
		    {
				/* default payload is 0xA5 */
				*pDest = pAd->ate.Payload;
				pDest += 1;
		    }
			TransferBufferLength = TXINFO_SIZE + TXWI_SIZE + pAd->ate.TxLength;
		}

		OrgBufferLength = TransferBufferLength;
		TransferBufferLength = (TransferBufferLength + 3) & (~3);

		/* Always add 4 extra bytes at every packet. */
		padLen = TransferBufferLength - OrgBufferLength + 4;/* 4 == last packet padding */

		/* 
			RTMP_PKT_TAIL_PADDING == 11.
			[11 == 3(max 4 byte padding) + 4(last packet padding) + 4(MaxBulkOutsize align padding)]		
		*/
		ASSERT((padLen <= (RTMP_PKT_TAIL_PADDING - 4/* 4 == MaxBulkOutsize alignment padding */)));

		/* Now memzero all extra padding bytes. */
		NdisZeroMemory(pDest, padLen);
		pDest += padLen;

		/* Update pTxInfo->USBDMATxPktLen to include padding. */
		pTxInfo->USBDMATxPktLen = TransferBufferLength - TXINFO_SIZE;

		TransferBufferLength += 4;

		/* If TransferBufferLength is multiple of 64, add extra 4 bytes again. */
		if ((TransferBufferLength % pAd->BulkOutMaxPacketSize) == 0)
		{
			NdisZeroMemory(pDest, 4);
			TransferBufferLength += 4;
		}

		/* Fill out frame length information for global Bulk out arbitor. */
		pAd->NullContext.BulkOutSize = TransferBufferLength;
	}

#ifdef RT_BIG_ENDIAN
	RTMPWIEndianChange((PUCHAR)pTxWI, TYPE_TXWI);
	RTMPFrameEndianChange(pAd, (((PUCHAR)pTxInfo)+TXWI_SIZE+TXINFO_SIZE), DIR_WRITE, FALSE);
	RTMPDescriptorEndianChange((PUCHAR)pTxInfo, TYPE_TXINFO);
#endif // RT_BIG_ENDIAN //

	return 0;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	Note:
	
========================================================================
*/
VOID ATE_RTUSBBulkOutDataPacket(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			BulkOutPipeId)
{
	PTX_CONTEXT		pNullContext = &(pAd->NullContext);
	PURB			pUrb;
	INT			ret = 0;
	ULONG			IrqFlags;


	ASSERT(BulkOutPipeId == 0);

	/* Build up the frame first. */
//	ATESetUpFrame(pAd, 0);
	
	BULK_OUT_LOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

	if (pAd->BulkOutPending[BulkOutPipeId] == TRUE)
	{
		BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);
		return;
	}

	pAd->BulkOutPending[BulkOutPipeId] = TRUE;
	BULK_OUT_UNLOCK(&pAd->BulkOutLock[BulkOutPipeId], IrqFlags);

	// Increase Total transmit byte counter.
	pAd->RalinkCounters.OneSecTransmittedByteCount +=  pNullContext->BulkOutSize; 
	pAd->RalinkCounters.TransmittedByteCount +=  pNullContext->BulkOutSize;

	// Clear ATE frame bulk out flag.
	RTUSB_CLEAR_BULK_FLAG(pAd, fRTUSB_BULK_OUT_DATA_ATE);

	// Init Tx context descriptor.
	pNullContext->IRPPending = TRUE;
	RTUSBInitTxDesc(pAd, pNullContext, BulkOutPipeId, (usb_complete_t)RTUSBBulkOutDataPacketComplete);
	pUrb = pNullContext->pUrb;

	if ((ret = RTUSB_SUBMIT_URB(pUrb))!=0)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("ATE_RTUSBBulkOutDataPacket: Submit Tx URB failed %d\n", ret));
		return;
	}

	pAd->BulkOutReq++;

	return;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	Note:
	
========================================================================
*/
VOID ATE_RTUSBCancelPendingBulkInIRP(
	IN	PRTMP_ADAPTER	pAd)
{
	PRX_CONTEXT		pRxContext = NULL;
	UINT			rx_ring_index;

	DBGPRINT(RT_DEBUG_TRACE, ("--->ATE_RTUSBCancelPendingBulkInIRP\n"));

	for (rx_ring_index = 0; rx_ring_index < (RX_RING_SIZE); rx_ring_index++)
	{
		pRxContext = &(pAd->RxContext[rx_ring_index]);

		if (pRxContext->IRPPending == TRUE)
		{
			RTUSB_UNLINK_URB(pRxContext->pUrb);
			pRxContext->IRPPending = FALSE;
			pRxContext->InUse = FALSE;
//			NdisInterlockedDecrement(&pAd->PendingRx);
//			pAd->PendingRx--;
		}
	}

	DBGPRINT(RT_DEBUG_TRACE, ("<---ATE_RTUSBCancelPendingBulkInIRP\n"));

	return;
}




/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	Note:
	
========================================================================
*/
VOID ATEResetBulkIn(
	IN PRTMP_ADAPTER	pAd)
{
	if ((pAd->PendingRx > 0) && (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_NIC_NOT_EXIST)))
	{
		DBGPRINT(RT_DEBUG_ERROR, ("ATE : BulkIn IRP Pending!!!\n"));
		ATE_RTUSBCancelPendingBulkInIRP(pAd);
		RTMPusecDelay(100000);
		pAd->PendingRx = 0;
	}

	return;
}


/*
========================================================================
	
	Routine Description:

	Arguments:

	Return Value:

	Note:
	
========================================================================
*/
INT ATEResetBulkOut(
	IN PRTMP_ADAPTER	pAd)
{
	PTX_CONTEXT	pNullContext = &(pAd->NullContext);
	INT ret=0;

	pNullContext->IRPPending = TRUE;

	/*
		If driver is still in ATE TXFRAME mode, 
		keep on transmitting ATE frames.
	*/
	DBGPRINT(RT_DEBUG_TRACE, ("pAd->ate.Mode == %d\npAd->ContinBulkOut == %d\npAd->BulkOutRemained == %d\n",
		pAd->ate.Mode, pAd->ContinBulkOut, atomic_read(&pAd->BulkOutRemained)));

	if ((pAd->ate.Mode == ATE_TXFRAME) && ((pAd->ContinBulkOut == TRUE) || (atomic_read(&pAd->BulkOutRemained) > 0)))
    {
		DBGPRINT(RT_DEBUG_TRACE, ("After CMDTHREAD_RESET_BULK_OUT, continue to bulk out frames !\n"));

		// Init Tx context descriptor.
		RTUSBInitTxDesc(pAd, pNullContext, 0/* pAd->bulkResetPipeid */, (usb_complete_t)RTUSBBulkOutDataPacketComplete);
		
		if ((ret = RTUSB_SUBMIT_URB(pNullContext->pUrb))!=0)
		{
			DBGPRINT(RT_DEBUG_ERROR, ("ATE_RTUSBBulkOutDataPacket: Submit Tx URB failed %d\n", ret));
		}

		pAd->BulkOutReq++;
	}

	return ret;
}
#endif // RTMP_MAC_USB //


VOID rt_ee_read_all(PRTMP_ADAPTER pAd, USHORT *Data)
{
	USHORT i;
	USHORT value;


	for (i = 0 ; i < EEPROM_SIZE/2 ; )
	{
		/* "value" is especially for some compilers... */
		RT28xx_EEPROM_READ16(pAd, i*2, value);
		Data[i] = value;
		i++;
	}
}


VOID rt_ee_write_all(PRTMP_ADAPTER pAd, USHORT *Data)
{
	USHORT i;
	USHORT value;


	for (i = 0 ; i < EEPROM_SIZE/2 ; )
	{
		/* "value" is especially for some compilers... */
		value = Data[i];
		RT28xx_EEPROM_WRITE16(pAd, i*2, value);
		i++;
	}
}


#ifdef RALINK_QA
VOID ATE_QA_Statistics(
	IN PRTMP_ADAPTER			pAd,
	IN PRXWI_STRUC				pRxWI,
	IN PRT28XX_RXD_STRUC		pRxD,
	IN PHEADER_802_11			pHeader)
{

	// update counter first
	if (pHeader != NULL)
	{
		if (pHeader->FC.Type == BTYPE_DATA)
		{
			if (pRxD->U2M)
			{
				pAd->ate.U2M++;
			}
			else
				pAd->ate.OtherData++;
		}
		else if (pHeader->FC.Type == BTYPE_MGMT)
		{
			if (pHeader->FC.SubType == SUBTYPE_BEACON)
				pAd->ate.Beacon++;
			else
				pAd->ate.OtherCount++;
		}
		else if (pHeader->FC.Type == BTYPE_CNTL)
		{
			pAd->ate.OtherCount++;
		}
	}
	pAd->ate.RSSI0 = pRxWI->RSSI0; 
	pAd->ate.RSSI1 = pRxWI->RSSI1; 
	pAd->ate.RSSI2 = pRxWI->RSSI2; 
	pAd->ate.SNR0 = pRxWI->SNR0;
	pAd->ate.SNR1 = pRxWI->SNR1;
}




INT RtmpDoAte(
	IN	PRTMP_ADAPTER	pAdapter, 
	IN	struct iwreq	*wrq)
{
	USHORT Command_Id;
	INT32 Status = NDIS_STATUS_SUCCESS;
	struct ate_racfghdr *pRaCfg;
	UINT32 RangeLimitIndex = 0;
	UINT32 TableIndex = 0;

	/* check if the ioctl from ated is for me */
	if (strcmp(wrq->ifr_name, pAdapter->net_dev->name) != 0)
	{
		/* not for me... */
		Status = -EINVAL;
		return Status;
	}
	
	pRaCfg = kmalloc(sizeof(struct ate_racfghdr), GFP_KERNEL);

	if (!pRaCfg)
	{
		Status = -ENOMEM;
		goto ERROR0;
	}
				
	NdisZeroMemory(pRaCfg, sizeof(struct ate_racfghdr));

	Status = copy_from_user((PUCHAR)pRaCfg, wrq->u.data.pointer, wrq->u.data.length);
	if (Status)
	{
		Status = -EFAULT;
		goto ERROR1;
	}
    
	Command_Id = OS_NTOHS(pRaCfg->command_id);
	DBGPRINT(RT_DEBUG_TRACE,("\n%s: Command_Id = 0x%04x !\n", __FUNCTION__, Command_Id));
	
	while (TableIndex < (sizeof(RACFG_CMD_TABLES) / sizeof(RACFG_CMD_TABLE)))
	{
		int cmd_index = 0;
		cmd_index = Command_Id - RACFG_CMD_TABLES[TableIndex].cmdOffset;
		if ((cmd_index >=0) && (cmd_index < RACFG_CMD_TABLES[TableIndex].cmdSetSize))
		{
			RACFG_CMD_HANDLER *pCmdSet;

			pCmdSet = RACFG_CMD_TABLES[TableIndex].cmdSet;
			if (pCmdSet[cmd_index] != NULL)
				Status = (*pCmdSet[cmd_index])(pAdapter, wrq, pRaCfg);
			break;
		}
		RangeLimitIndex++;
		TableIndex++;
	}
	
	// In passive mode only allow commands that read registers
	if (pAdapter->ate.PassiveMode)
	{
		int i, allowCmd = FALSE;
		static int allowedCmds[] = {
				RACFG_CMD_E2PROM_READ16, RACFG_CMD_E2PROM_READ_ALL,
				RACFG_CMD_IO_READ, RACFG_CMD_IO_READ_BULK,
				RACFG_CMD_BBP_READ8, RACFG_CMD_BBP_READ_ALL,
				RACFG_CMD_ATE_E2PROM_READ_BULK,
				RACFG_CMD_ATE_BBP_READ_BULK,
				RACFG_CMD_ATE_RF_READ_BULK,
				};

		for (i=0; i<sizeof(allowedCmds)/sizeof(allowedCmds[0]); i++)
		{
			if (Command_Id==allowedCmds[i])
			{
				allowCmd = TRUE;
				break;
			}
		}

		// Also allow writes to BF profile registers
		if (Command_Id==RACFG_CMD_BBP_WRITE8)
		{
			int offset = ntohs(pRaCfg->status);
			if (offset==BBP_R27 || (offset>=BBP_R174 && offset<=BBP_R182))
				allowCmd = TRUE;
		}

		// If not allowed then ignore the command
		if (!allowCmd)
		{
			ResponseToGUI(pRaCfg, wrq, sizeof(pRaCfg->status), NDIS_STATUS_SUCCESS);
			if (pRaCfg != NULL)
			{
				kfree(pRaCfg);
			}
			return NDIS_STATUS_FAILURE;
		}
	}
	
 ERROR1:
 	kfree(pRaCfg);
 ERROR0:
	return Status;
}


VOID BubbleSort(INT32 n, INT32 a[])
{ 
	INT32 k, j, temp;

	for (k = n-1;  k>0;  k--)
	{
		for (j = 0; j<k; j++)
		{
			if (a[j] > a[j+1])
			{
				temp = a[j]; 
				a[j]=a[j+1]; 
				a[j+1]=temp;
			}
		}
	}
} 


VOID CalNoiseLevel(PRTMP_ADAPTER pAd, UCHAR channel, INT32 RSSI[3][10])
{
	INT32		RSSI0, RSSI1, RSSI2;
 	CHAR		Rssi0Offset, Rssi1Offset, Rssi2Offset;
	UCHAR		BbpR50Rssi0 = 0, BbpR51Rssi1 = 0, BbpR52Rssi2 = 0;
	UCHAR		Org_BBP66value = 0, Org_BBP69value = 0, Org_BBP70value = 0, data = 0;
	USHORT		LNA_Gain = 0;
	INT32       j = 0;
	UCHAR		Org_Channel = pAd->ate.Channel;
	USHORT	    GainValue = 0, OffsetValue = 0;

	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R66, &Org_BBP66value);
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R69, &Org_BBP69value);	
	ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R70, &Org_BBP70value);

	//**********************************************************************
	// Read the value of LNA gain and Rssi offset 
	//**********************************************************************
	RT28xx_EEPROM_READ16(pAd, EEPROM_LNA_OFFSET, GainValue);

	// for Noise Level
	if (channel <= 14)
	{
		LNA_Gain = GainValue & 0x00FF;		 

		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_BG_OFFSET, OffsetValue);
		Rssi0Offset = OffsetValue & 0x00FF;
		Rssi1Offset = (OffsetValue & 0xFF00) >> 8;

		RT28xx_EEPROM_READ16(pAd, (EEPROM_RSSI_BG_OFFSET + 2)/* 0x48 */, OffsetValue);
		Rssi2Offset = OffsetValue & 0x00FF;
	}
	else
	{
		LNA_Gain = (GainValue & 0xFF00) >> 8;
		RT28xx_EEPROM_READ16(pAd, EEPROM_RSSI_A_OFFSET, OffsetValue);
		Rssi0Offset = OffsetValue & 0x00FF;
		Rssi1Offset = (OffsetValue & 0xFF00) >> 8;
		RT28xx_EEPROM_READ16(pAd, (EEPROM_RSSI_A_OFFSET + 2)/* 0x4C */, OffsetValue);
		Rssi2Offset = OffsetValue & 0x00FF;
	}
	//**********************************************************************	
	{
		pAd->ate.Channel = channel;
		ATEAsicSwitchChannel(pAd);
		mdelay(5);

		data = 0x10;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, data);	

		data = 0x40;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, data);
		data = 0x40;
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, data);
		mdelay(5);

		// start Rx
		pAd->ate.bQARxStart = TRUE;
		Set_ATE_Proc(pAd, "RXFRAME");

		mdelay(5);

		for (j = 0; j < 10; j++)
		{
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R50, &BbpR50Rssi0);
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R51, &BbpR51Rssi1);	
			ATE_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R52, &BbpR52Rssi2);

			mdelay(10);

			// calculate RSSI 0
			if (BbpR50Rssi0 == 0)
			{
				RSSI0 = -100;
			}
			else
			{
				RSSI0 = (INT32)(-12 - BbpR50Rssi0 - LNA_Gain - Rssi0Offset);
			}
			RSSI[0][j] = RSSI0;

			if ( pAd->Antenna.field.RxPath >= 2 ) // 2R
			{
				// calculate RSSI 1
				if (BbpR51Rssi1 == 0)
				{
					RSSI1 = -100;
				}
				else
				{
					RSSI1 = (INT32)(-12 - BbpR51Rssi1 - LNA_Gain - Rssi1Offset);
				}
				RSSI[1][j] = RSSI1;
			}

			if ( pAd->Antenna.field.RxPath >= 3 ) // 3R
			{
				// calculate RSSI 2
				if (BbpR52Rssi2 == 0)
					RSSI2 = -100;
				else
					RSSI2 = (INT32)(-12 - BbpR52Rssi2 - LNA_Gain - Rssi2Offset);

				RSSI[2][j] = RSSI2;
			}
		}

		// stop Rx
		Set_ATE_Proc(pAd, "RXSTOP");

		mdelay(5);

		BubbleSort(10, RSSI[0]);	// 1R		

		if ( pAd->Antenna.field.RxPath >= 2 ) // 2R
		{
			BubbleSort(10, RSSI[1]);
		}

		if ( pAd->Antenna.field.RxPath >= 3 ) // 3R
		{
			BubbleSort(10, RSSI[2]);
		}	
	}

	pAd->ate.Channel = Org_Channel;
	ATEAsicSwitchChannel(pAd);
	
	// restore original value
    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R66, Org_BBP66value);
    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R69, Org_BBP69value);
    ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R70, Org_BBP70value);

	return;
}


BOOLEAN SyncTxRxConfig(PRTMP_ADAPTER pAd, USHORT offset, UCHAR value)
{ 
	UCHAR tmp = 0, bbp_data = 0;

	if (ATE_ON(pAd))
	{
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, offset, &bbp_data);
	}
	else
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, offset, &bbp_data);
	}

	/* confirm again */
	ASSERT(bbp_data == value);

	switch (offset)
	{
		case BBP_R1:
			/* Need to synchronize tx configuration with legacy ATE. */
			tmp = (bbp_data & ((1 << 4) | (1 << 3))/* 0x18 */) >> 3;
		    switch (tmp)
		    {
				/* The BBP R1 bit[4:3] = 2 :: Both DACs will be used by QA. */
		        case 2:
					/* All */
					pAd->ate.TxAntennaSel = 0;
		            break;
				/* The BBP R1 bit[4:3] = 0 :: DAC 0 will be used by QA. */
		        case 0:
					/* Antenna one */
					pAd->ate.TxAntennaSel = 1;
		            break;
				/* The BBP R1 bit[4:3] = 1 :: DAC 1 will be used by QA. */
		        case 1:
					/* Antenna two */
					pAd->ate.TxAntennaSel = 2;
		            break;
		        default:
		            DBGPRINT(RT_DEBUG_TRACE, ("%s -- Sth. wrong!  : return FALSE; \n", __FUNCTION__));    
		            return FALSE;
		    }
			break;/* case BBP_R1 */

		case BBP_R3:
			/* Need to synchronize rx configuration with legacy ATE. */
			tmp = (bbp_data & ((1 << 1) | (1 << 0))/* 0x03 */);
		    switch(tmp)
		    {
				/* The BBP R3 bit[1:0] = 3 :: All ADCs will be used by QA. */
		        case 3:
					/* All */
					pAd->ate.RxAntennaSel = 0;
		            break;
				/*
					The BBP R3 bit[1:0] = 0 :: ADC 0 will be used by QA,
					unless the BBP R3 bit[4:3] = 2
				*/
		        case 0:
					/* Antenna one */
					pAd->ate.RxAntennaSel = 1;
					tmp = ((bbp_data & ((1 << 4) | (1 << 3))/* 0x03 */) >> 3);
					if (tmp == 2)// 3R
					{
						/* Default : All ADCs will be used by QA */
						pAd->ate.RxAntennaSel = 0;
					}
		            break;
				/* The BBP R3 bit[1:0] = 1 :: ADC 1 will be used by QA. */
		        case 1:
					/* Antenna two */
					pAd->ate.RxAntennaSel = 2;
		            break;
				/* The BBP R3 bit[1:0] = 2 :: ADC 2 will be used by QA. */
		        case 2:
					/* Antenna three */
					pAd->ate.RxAntennaSel = 3;
		            break;
		        default:
		            DBGPRINT(RT_DEBUG_ERROR, ("%s -- Impossible!  : return FALSE; \n", __FUNCTION__));    
		            return FALSE;
		    }
			break;/* case BBP_R3 */

        default:
            DBGPRINT(RT_DEBUG_ERROR, ("%s -- Sth. wrong!  : return FALSE; \n", __FUNCTION__));    
            return FALSE;
		
	}
	return TRUE;
} 


static VOID memcpy_exl(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, ULONG len)
{
	ULONG i, Value = 0;
	ULONG *pDst, *pSrc;
	UCHAR *p8;
	
	p8 = src;
	pDst = (ULONG *) dst;
	pSrc = (ULONG *) src;
	
	for (i = 0 ; i < (len/4); i++)
	{
		/* For alignment issue, we need a variable "Value". */
		memmove(&Value, pSrc, 4);
		Value = OS_HTONL(Value); 
		memmove(pDst, &Value, 4);		
		pDst++;
		pSrc++;
	}
	if ((len % 4) != 0)
	{
		/* wish that it will never reach here */
		memmove(&Value, pSrc, (len % 4));
		Value = OS_HTONL(Value); 
		memmove(pDst, &Value, (len % 4));
	}
}


static VOID memcpy_exs(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, ULONG len)
{
	ULONG i;
	{
		USHORT *pDst, *pSrc;
		
		pDst = (USHORT *) dst;
		pSrc = (USHORT *) src;

		for (i =0; i < (len/2); i++)
		{
			*pDst = OS_NTOHS(*pSrc);
			pDst++;
			pSrc++;
		}
		
		if ((len % 2) != 0)
		{
			memcpy(pDst, pSrc, (len % 2));
			*pDst = OS_NTOHS(*pDst);
		}
	}

}


static VOID RTMP_IO_READ_BULK(PRTMP_ADAPTER pAd, UCHAR *dst, UCHAR *src, UINT32 len)
{
	UINT32 i, Value;
	UINT32 *pDst, *pSrc;
	
	pDst = (UINT32 *) dst;
	pSrc = (UINT32 *) src;

	for (i = 0 ; i < (len/4); i++)
	{
		RTMP_IO_READ32(pAd, (ULONG)pSrc, &Value);
		Value = OS_HTONL(Value);
		memmove(pDst, &Value, 4);
		pDst++;
		pSrc++;
	}
	return;	
}


INT Set_TxStop_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_TRACE,("Set_TxStop_Proc\n"));

	if (Set_ATE_Proc(pAd, "TXSTOP"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


INT Set_RxStop_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	DBGPRINT(RT_DEBUG_TRACE,("Set_RxStop_Proc\n"));

	if (Set_ATE_Proc(pAd, "RXSTOP"))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


#ifdef DBG
INT Set_EERead_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	USHORT buffer[EEPROM_SIZE/2];
	USHORT *p;
	INT i;
	
	rt_ee_read_all(pAd, (USHORT *)buffer);
	p = buffer;

	for (i = 0; i < (EEPROM_SIZE/2); i++)
	{
		DBGPRINT(RT_DEBUG_OFF, ("%4.4x ", *p));
		if (((i+1) % 16) == 0)
			DBGPRINT(RT_DEBUG_OFF, ("\n"));
		p++;
	}

	return TRUE;
}


INT Set_EEWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	USHORT offset = 0, value;
	PSTRING p2 = arg;
	
	while ((*p2 != ':') && (*p2 != '\0'))
	{
		p2++;
	}
	
	if (*p2 == ':')
	{
		A2Hex(offset, arg);
		A2Hex(value, p2 + 1);
	}
	else
	{
		A2Hex(value, arg);
	}
	
	if (offset >= EEPROM_SIZE)
	{
		DBGPRINT(RT_DEBUG_ERROR, ("Offset can not exceed EEPROM_SIZE( == 0x%04x)\n", EEPROM_SIZE));	
		return FALSE;
	}
	
	RT28xx_EEPROM_WRITE16(pAd, offset, value);

	return TRUE;
}


INT Set_BBPRead_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	UCHAR value = 0, offset;

	A2Hex(offset, arg);	
			
	if (ATE_ON(pAd))
	{
		ATE_BBP_IO_READ8_BY_REG_ID(pAd, offset,  &value);
	}
	else
	{
		RTMP_BBP_IO_READ8_BY_REG_ID(pAd, offset,  &value);
	}

	DBGPRINT(RT_DEBUG_OFF, ("%x\n", value));
		
	return TRUE;
}


INT Set_BBPWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	USHORT offset = 0;
	PSTRING p2 = arg;
	UCHAR value;
	
	while ((*p2 != ':') && (*p2 != '\0'))
	{
		p2++;
	}
	
	if (*p2 == ':')
	{
		A2Hex(offset, arg);	
		A2Hex(value, p2 + 1);	
	}
	else
	{
		A2Hex(value, arg);	
	}

	if (ATE_ON(pAd))
	{
		ATE_BBP_IO_WRITE8_BY_REG_ID(pAd, offset,  value);
	}
	else
	{
		RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, offset,  value);
	}

	return TRUE;
}


INT Set_RFWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{
	PSTRING p2, p3, p4;
	UINT32 R1, R2, R3, R4;
	
	p2 = arg;

	while ((*p2 != ':') && (*p2 != '\0'))
	{
		p2++;
	}
	
	if (*p2 != ':')
		return FALSE;
	
	p3 = p2 + 1;

	while((*p3 != ':') && (*p3 != '\0'))
	{
		p3++;
	}

	if (*p3 != ':')
		return FALSE;
	
	p4 = p3 + 1;

	while ((*p4 != ':') && (*p4 != '\0'))
	{
		p4++;
	}

	if (*p4 != ':')
		return FALSE;

		
	A2Hex(R1, arg);	
	A2Hex(R2, p2 + 1);	
	A2Hex(R3, p3 + 1);	
	A2Hex(R4, p4 + 1);	
	
	RTMP_RF_IO_WRITE32(pAd, R1);
	RTMP_RF_IO_WRITE32(pAd, R2);
	RTMP_RF_IO_WRITE32(pAd, R3);
	RTMP_RF_IO_WRITE32(pAd, R4);
	
	return TRUE;
}
#endif // DBG //
#endif // RALINK_QA //



#ifdef RTMP_INTERNAL_TX_ALC
INT Set_ATE_TSSI_CALIBRATION_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg)
{    
	UCHAR inputDAC;
	inputDAC = simple_strtol(arg, 0, 10);

	if(!IS_RT3390(pAd) || !(pAd->TxPowerCtrl.bInternalTxALC) )                  
	{
               DBGPRINT(RT_DEBUG_TRACE, ("Not support TSSI calibration since not 3390 chip or EEPROM not set!!!\n"));
               return(FALSE);
	}
	else
	{                              
	        UCHAR        	BbpData, RFValue, RF27Value, RF28Value, BBP49Value;
	        USHORT		EEPData;                              
	        UCHAR         BSSID_ADDR[MAC_ADDR_LEN] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

	        //set RF R27 bit[7][3] = 11
	        RT30xxReadRFRegister(pAd, RF_R27, &RFValue);
	        RF27Value = RFValue;
	        RFValue |= 0x88; 
	        RT30xxWriteRFRegister(pAd, RF_R27, RFValue);

	        //set RF R28 bit[6][5] = 00
	        RT30xxReadRFRegister(pAd, RF_R28, &RFValue);
	        RF28Value = RFValue;
	        RFValue &= 0x9f; 
	        RT30xxWriteRFRegister(pAd, RF_R28, RFValue);

	        //set BBP R49[7] = 1
	        RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
		
	        BBP49Value = BbpData;
	        BbpData |= 0x80;
	        RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R49, BbpData);

		 //start TX at 54Mbps
		 NdisZeroMemory(&pAd->ate, sizeof(struct _ATE_INFO));
	      	 pAd->ate.TxCount = 1000;
	      	 pAd->ate.TxLength = 1024;
	     	 pAd->ate.Channel = 1;
		 pAd->ate.TxWI.PHYMODE= 1; 	// MODE_OFDM
		 pAd->ate.TxWI.MCS = 7; 		// 54Mbps
		 pAd->ate.TxWI.BW = 0; 		// 20MHz
	        COPY_MAC_ADDR(pAd->ate.Addr1, BROADCAST_ADDR);
	        COPY_MAC_ADDR(pAd->ate.Addr2, pAd->PermanentAddress);                                                     
	        COPY_MAC_ADDR(pAd->ate.Addr3, BSSID_ADDR);                     

	        //set power value calibrated DAC 
	        pAd->ate.TxPower0 = inputDAC;
	        
	        //read frequency offset from EEPROM                         
	        RT28xx_EEPROM_READ16(pAd, EEPROM_FREQ_OFFSET, EEPData);
	        pAd->ate.RFFreqOffset = (UCHAR) (EEPData & 0xff);

		 Set_ATE_Proc(pAd, "TXFRAME"); 
	        RTMPusecDelay(200000);
			
	        //read BBP R49[4:0] and write to EEPROM 0x6E
	        DBGPRINT(RT_DEBUG_TRACE, ("Read  BBP_R49\n")); 
	        RTMP_BBP_IO_READ8_BY_REG_ID(pAd, BBP_R49, &BbpData);
	        DBGPRINT(RT_DEBUG_TRACE, ("BBP R49 = 0x%x\n", BbpData)); 
	        BbpData &= 0x1f;
	        RT28xx_EEPROM_READ16(pAd, EEPROM_TSSI_OVER_OFDM_54, EEPData);
	        EEPData &= 0xff00;
	        EEPData |= BbpData;
	        DBGPRINT(RT_DEBUG_TRACE, ("Write  E2P 0x6e: 0x%x\n", EEPData)); 
	        if(pAd->bUseEfuse)
	        {
	                if(pAd->bFroceEEPROMBuffer)
	                        NdisMoveMemory(&(pAd->EEPROMImage[EEPROM_TSSI_OVER_OFDM_54]), (PUCHAR) (&EEPData) ,2);
	                else
	                        eFuseWrite(pAd, EEPROM_TSSI_OVER_OFDM_54, (PUCHAR) (&EEPData), 2);
	            }
		 else
	      	 {
	                RT28xx_EEPROM_WRITE16(pAd, EEPROM_TSSI_OVER_OFDM_54, EEPData);
	                RTMPusecDelay(10);
	      	 }                              
	        
	        // restore RF R27 and R28, BBP R49
	        RT30xxWriteRFRegister(pAd, RF_R27, RF27Value);                          
	        RT30xxWriteRFRegister(pAd, RF_R28, RF28Value);
	        RTMP_BBP_IO_WRITE8_BY_REG_ID(pAd, BBP_R49, BBP49Value);     
	}

	return TRUE;
	
}
#endif // RTMP_INTERNAL_TX_ALC //
