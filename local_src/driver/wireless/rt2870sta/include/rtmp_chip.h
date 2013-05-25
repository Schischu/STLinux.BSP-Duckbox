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
 *************************************************************************

	Module Name:
	rtmp_chip.h

	Abstract:
	Ralink Wireless Chip related definition & structures

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
*/

#ifndef	__RTMP_CHIP_H__
#define	__RTMP_CHIP_H__

#include "rtmp_type.h"




#ifdef RT2870
#include "chip/rt2870.h"
#endif // RT2870 //










#define IS_RT3090A(_pAd)				((((_pAd)->MACVersion & 0xffff0000) == 0x30900000))

// We will have a cost down version which mac version is 0x3090xxxx
#define IS_RT3090(_pAd)				((((_pAd)->MACVersion & 0xffff0000) == 0x30710000) || (IS_RT3090A(_pAd)))

#define IS_RT3070(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x30700000)
#define IS_RT3071(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x30710000)
#define IS_RT2070(_pAd)		(((_pAd)->RfIcType == RFIC_2020) || ((_pAd)->EFuseTag == 0x27))

#define IS_RT2860(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x28600000)
#define IS_RT2872(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x28720000)

#define IS_RT30xx(_pAd)		(((_pAd)->MACVersion & 0xfff00000) == 0x30700000||IS_RT3090A(_pAd)||IS_RT3390(_pAd))
//#define IS_RT305X(_pAd)		((_pAd)->MACVersion == 0x28720200)

/* RT3572, 3592, 3562, 3062 share the same MAC version */
#define IS_RT3572(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x35720000)

#define IS_RT2883(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x28830000)
#define IS_RT3883(_pAd)		(((_pAd)->MACVersion & 0xffff0000) == 0x38830000)
#define IS_VERSION_BEFORE_F(_pAd)			(((_pAd)->MACVersion&0xffff) <= 0x0211)
// F version is 0x0212, E version is 0x0211. 309x can save more power after F version.
#define IS_VERSION_AFTER_F(_pAd)			((((_pAd)->MACVersion&0xffff) >= 0x0212) || (((_pAd)->b3090ESpecialChip == TRUE)))

/* 3593 */
#define IS_RT3593(_pAd) (((_pAd)->MACVersion & 0xFFFF0000) == 0x35930000)

/* RT3593 over PCIe bus */
#define RT3593OverPCIe(_pAd) (IS_RT3593(_pAd) && (_pAd->CommonCfg.bPCIeBus == TRUE))

/* RT3593 over PCI bus */
#define RT3593OverPCI(_pAd) (IS_RT3593(_pAd) && (_pAd->CommonCfg.bPCIeBus == FALSE))

//RT3390,RT3370
#define IS_RT3390(_pAd)				(((_pAd)->MACVersion & 0xFFFF0000) == 0x33900000)

// ------------------------------------------------------
// PCI registers - base address 0x0000
// ------------------------------------------------------
#define CHIP_PCI_CFG		0x0000
#define CHIP_PCI_EECTRL		0x0004
#define CHIP_PCI_MCUCTRL	0x0008

#define OPT_14			0x114

#define RETRY_LIMIT		10



// ------------------------------------------------------
// BBP & RF	definition
// ------------------------------------------------------
#define	BUSY		                1
#define	IDLE		                0


//-------------------------------------------------------------------------
// EEPROM definition
//-------------------------------------------------------------------------
#define EEDO                        0x08
#define EEDI                        0x04
#define EECS                        0x02
#define EESK                        0x01
#define EERL                        0x80

#define EEPROM_WRITE_OPCODE         0x05
#define EEPROM_READ_OPCODE          0x06
#define EEPROM_EWDS_OPCODE          0x10
#define EEPROM_EWEN_OPCODE          0x13

#define NUM_EEPROM_BBP_PARMS		19			// Include NIC Config 0, 1, CR, TX ALC step, BBPs
#define NUM_EEPROM_TX_G_PARMS		7

#define VALID_EEPROM_VERSION        1
#define EEPROM_VERSION_OFFSET       0x02
#define EEPROM_NIC1_OFFSET          0x34		// The address is from NIC config 0, not BBP register ID
#define EEPROM_NIC2_OFFSET          0x36		// The address is from NIC config 1, not BBP register ID


#define EEPROM_COUNTRY_REGION			0x38

#define EEPROM_DEFINE_MAX_TXPWR			0x4e

#define EEPROM_FREQ_OFFSET			0x3a
#define EEPROM_LED1_OFFSET			0x3c
#define EEPROM_LED2_OFFSET			0x3e
#define EEPROM_LED3_OFFSET			0x40

#define EEPROM_LNA_OFFSET			0x44

#define EEPROM_RSSI_BG_OFFSET			0x46
#define EEPROM_RSSI_A_OFFSET			0x4a
#define EEPROM_TXMIXER_GAIN_2_4G		0x48
#define EEPROM_TXMIXER_GAIN_5G			0x4c

#define EEPROM_TXPOWER_DELTA			0x50	// 20MHZ AND 40 MHZ use different power. This is delta in 40MHZ.


#define EEPROM_G_TX_PWR_OFFSET			0x52
#define EEPROM_G_TX2_PWR_OFFSET			0x60

#define EEPROM_G_TSSI_BOUND1			0x6e
#define EEPROM_G_TSSI_BOUND2			0x70
#define EEPROM_G_TSSI_BOUND3			0x72
#define EEPROM_G_TSSI_BOUND4			0x74
#define EEPROM_G_TSSI_BOUND5			0x76

#define EEPROM_A_TX_PWR_OFFSET      		0x78
#define EEPROM_A_TX2_PWR_OFFSET			0xa6

#define EEPROM_A_TSSI_BOUND1		0xd4
#define EEPROM_A_TSSI_BOUND2		0xd6
#define EEPROM_A_TSSI_BOUND3		0xd8
#define EEPROM_A_TSSI_BOUND4		0xda
#define EEPROM_A_TSSI_BOUND5		0xdc

#define EEPROM_ITXBF_CAL_RX0			0x1a0
#define EEPROM_ITXBF_CAL_TX0			0x1a2
#define EEPROM_ITXBF_CAL_RX1			0x1a4
#define EEPROM_ITXBF_CAL_TX1			0x1a6
#define EEPROM_ITXBF_CAL_RX2			0x1a8
#define EEPROM_ITXBF_CAL_TX2			0x1aa

#define EEPROM_TXPOWER_BYRATE 			0xde	// 20MHZ power. 
#define EEPROM_TXPOWER_BYRATE_20MHZ_2_4G	0xde	// 20MHZ 2.4G tx power.
#define EEPROM_TXPOWER_BYRATE_40MHZ_2_4G	0xee	// 40MHZ 2.4G tx power.
#define EEPROM_TXPOWER_BYRATE_20MHZ_5G		0xfa	// 20MHZ 5G tx power.
#define EEPROM_TXPOWER_BYRATE_40MHZ_5G		0x10a	// 40MHZ 5G tx power.

#define EEPROM_BBP_BASE_OFFSET			0xf0	// The address is from NIC config 0, not BBP register ID

//
// Bit mask for the Tx ALC and the Tx fine power control
//
#define GET_TX_ALC_BIT_MASK					0x1F	// Valid: 0~31, and in 0.5dB step
#define GET_TX_FINE_POWER_CTRL_BIT_MASK	0xE0	// Valid: 0~4, and in 0.1dB step
#define NUMBER_OF_BITS_FOR_TX_ALC			5 // The length, in bit, of the Tx ALC field

//#define EEPROM_Japan_TX_PWR_OFFSET      0x90 // 802.11j
//#define EEPROM_Japan_TX2_PWR_OFFSET      0xbe
//#define EEPROM_TSSI_REF_OFFSET	0x54
//#define EEPROM_TSSI_DELTA_OFFSET	0x24
//#define EEPROM_CCK_TX_PWR_OFFSET  0x62
//#define EEPROM_CALIBRATE_OFFSET	0x7c

#define EEPROM_NIC_CFG1_OFFSET		0
#define EEPROM_NIC_CFG2_OFFSET		1
#define EEPROM_NIC_CFG3_OFFSET		2
#define EEPROM_COUNTRY_REG_OFFSET	3
#define EEPROM_BBP_ARRAY_OFFSET		4

#ifdef RTMP_INTERNAL_TX_ALC
//
// The TSSI over OFDM 54Mbps
//
#define EEPROM_TSSI_OVER_OFDM_54		0x6E

//
// The TSSI value/step (0.5 dB/unit)
//
#define EEPROM_TSSI_STEP_OVER_2DOT4G	0x77

//
// Tx power configuration (bit3:0 for Tx0 power setting and bit7:4 for Tx1 power setting)
//
#define EEPROM_CCK_MCS0_MCS1				0xDE
#define EEPROM_CCK_MCS2_MCS3				0xDF
#define EEPROM_OFDM_MCS0_MCS1				0xE0
#define EEPROM_OFDM_MCS2_MCS3				0xE1
#define EEPROM_OFDM_MCS4_MCS5				0xE2
#define EEPROM_OFDM_MCS6_MCS7				0xE3
#define EEPROM_HT_MCS0_MCS1				0xE4
#define EEPROM_HT_MCS2_MCS3				0xE5
#define EEPROM_HT_MCS4_MCS5				0xE6
#define EEPROM_HT_MCS6_MCS7				0xE7
#define EEPROM_HT_USING_STBC_MCS0_MCS1	0xEC
#define EEPROM_HT_USING_STBC_MCS2_MCS3	0xED
#define EEPROM_HT_USING_STBC_MCS4_MCS5	0xEE
#define EEPROM_HT_USING_STBC_MCS6_MCS7	0xEF

//
// Bit mask for the Tx ALC and the Tx fine power control
//
#define GET_TX_ALC_BIT_MASK				0x1F	// Valid: 0~31, and in 0.5dB step
#define GET_TX_FINE_POWER_CTRL_BIT_MASK	0xE0	// Valid: 0~4, and in 0.1dB step
#define NUMBER_OF_BITS_FOR_TX_ALC			5 // The length, in bit, of the Tx ALC field

#define DEFAULT_BBP_TX_FINE_POWER_CTRL 0

#endif // RTMP_INTERNAL_TX_ALC //



/*
  *   EEPROM operation related marcos
  */
#define RT28xx_EEPROM_READ16(_pAd, _offset, _value)			\
	(_pAd)->chipOps.eeread((RTMP_ADAPTER *)(_pAd), (USHORT)(_offset), (PUSHORT)&(_value))

#define RT28xx_EEPROM_WRITE16(_pAd, _offset, _value)		\
	(_pAd)->chipOps.eewrite((RTMP_ADAPTER *)(_pAd), (USHORT)(_offset), (USHORT)(_value))



// -------------------------------------------------------------------
//  E2PROM data layout
// -------------------------------------------------------------------

//
// MCU_LEDCS: MCU LED Control Setting.
//
typedef union  _MCU_LEDCS_STRUC {
	struct	{
#ifdef RT_BIG_ENDIAN
		UCHAR		Polarity:1;
		UCHAR		LedMode:7;		
#else
		UCHAR		LedMode:7;		
		UCHAR		Polarity:1;
#endif // RT_BIG_ENDIAN //
	} field;
	UCHAR				word;
} MCU_LEDCS_STRUC, *PMCU_LEDCS_STRUC;


//
// EEPROM antenna select format
//
#ifdef RT_BIG_ENDIAN
typedef	union	_EEPROM_ANTENNA_STRUC	{
	struct	{
		USHORT      Rsv:4;              
		USHORT      RfIcType:4;             // see E2PROM document		
		USHORT		TxPath:4;	// 1: 1T, 2: 2T
		USHORT		RxPath:4;	// 1: 1R, 2: 2R, 3: 3R
	}	field;
	USHORT			word;
}	EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#else
typedef	union	_EEPROM_ANTENNA_STRUC	{
	struct	{
		USHORT		RxPath:4;	// 1: 1R, 2: 2R, 3: 3R
		USHORT		TxPath:4;	// 1: 1T, 2: 2T
		USHORT      RfIcType:4;             // see E2PROM document		
		USHORT      Rsv:4;              
	}	field;
	USHORT			word;
}	EEPROM_ANTENNA_STRUC, *PEEPROM_ANTENNA_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef	union _EEPROM_NIC_CINFIG2_STRUC	{
	struct	{
		USHORT		DACTestBit:1;			// control if driver should patch the DAC issue
		USHORT		CoexBit:1;
		USHORT		bInternalTxALC:1; // Internal Tx ALC
		USHORT		AntOpt:1; // Fix Antenna Option: 0:Main; 1: Aux
		USHORT		AntDiversity:1;			// Antenna diversity
		USHORT		Rsv1:1;					// must be 0
		USHORT		BW40MAvailForA:1;			// 0:enable, 1:disable
		USHORT		BW40MAvailForG:1;			// 0:enable, 1:disable
		USHORT		EnableWPSPBC:1;                 // WPS PBC Control bit
		USHORT		BW40MSidebandForA:1;
		USHORT		BW40MSidebandForG:1;
		USHORT		CardbusAcceleration:1;	// !!! NOTE: 0 - enable, 1 - disable		
		USHORT		ExternalLNAForA:1;			// external LNA enable for 5G		
		USHORT		ExternalLNAForG:1;			// external LNA enable for 2.4G
		USHORT		DynamicTxAgcControl:1;			//
		USHORT		HardwareRadioControl:1;	// Whether RF is controlled by driver or HW. 1:enable hw control, 0:disable
	}	field;
	USHORT			word;
}	EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#else
typedef	union _EEPROM_NIC_CINFIG2_STRUC	{
	struct {
		USHORT		HardwareRadioControl:1;	// 1:enable, 0:disable
		USHORT		DynamicTxAgcControl:1;			//
		USHORT		ExternalLNAForG:1;				//
		USHORT		ExternalLNAForA:1;			// external LNA enable for 2.4G
		USHORT		CardbusAcceleration:1;	// !!! NOTE: 0 - enable, 1 - disable		
		USHORT		BW40MSidebandForG:1;
		USHORT		BW40MSidebandForA:1;
		USHORT		EnableWPSPBC:1;                 // WPS PBC Control bit
		USHORT		BW40MAvailForG:1;			// 0:enable, 1:disable
		USHORT		BW40MAvailForA:1;			// 0:enable, 1:disable
		USHORT		Rsv1:1;					// must be 0
		USHORT		AntDiversity:1;			// Antenna diversity
		USHORT		AntOpt:1; // Fix Antenna Option: 0:Main; 1: Aux
		USHORT		bInternalTxALC:1; // Internal Tx ALC
		USHORT		CoexBit:1;
		USHORT		DACTestBit:1;			// control if driver should patch the DAC issue
	}	field;
	USHORT			word;
}	EEPROM_NIC_CONFIG2_STRUC, *PEEPROM_NIC_CONFIG2_STRUC;
#endif


//
// TX_PWR Value valid range 0xFA(-6) ~ 0x24(36)
//
#ifdef RT_BIG_ENDIAN
typedef	union	_EEPROM_TX_PWR_STRUC	{
	struct	{
		signed char	Byte1;				// High Byte
		signed char	Byte0;				// Low Byte
	}	field;
	USHORT	word;
}	EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#else
typedef	union	_EEPROM_TX_PWR_STRUC	{
	struct	{
		signed char	Byte0;				// Low Byte
		signed char	Byte1;				// High Byte
	}	field;
	USHORT	word;
}	EEPROM_TX_PWR_STRUC, *PEEPROM_TX_PWR_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef	union	_EEPROM_VERSION_STRUC	{
	struct	{
		UCHAR	Version;			// High Byte
		UCHAR	FaeReleaseNumber;	// Low Byte
	}	field;
	USHORT	word;
}	EEPROM_VERSION_STRUC, *PEEPROM_VERSION_STRUC;
#else
typedef	union	_EEPROM_VERSION_STRUC	{
	struct	{
		UCHAR	FaeReleaseNumber;	// Low Byte
		UCHAR	Version;			// High Byte
	}	field;
	USHORT	word;
}	EEPROM_VERSION_STRUC, *PEEPROM_VERSION_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef	union	_EEPROM_LED_STRUC	{
	struct	{
		USHORT	Rsvd:3;				// Reserved
		USHORT	LedMode:5;			// Led mode.
		USHORT	PolarityGPIO_4:1;	// Polarity GPIO#4 setting.
		USHORT	PolarityGPIO_3:1;	// Polarity GPIO#3 setting.
		USHORT	PolarityGPIO_2:1;	// Polarity GPIO#2 setting.
		USHORT	PolarityGPIO_1:1;	// Polarity GPIO#1 setting.
		USHORT	PolarityGPIO_0:1;	// Polarity GPIO#0 setting.
		USHORT	PolarityACT:1;		// Polarity ACT setting.
		USHORT	PolarityRDY_A:1;		// Polarity RDY_A setting.
		USHORT	PolarityRDY_G:1;		// Polarity RDY_G setting.
	}	field;
	USHORT	word;
}	EEPROM_LED_STRUC, *PEEPROM_LED_STRUC;
#else
typedef	union	_EEPROM_LED_STRUC	{
	struct	{
		USHORT	PolarityRDY_G:1;		// Polarity RDY_G setting.
		USHORT	PolarityRDY_A:1;		// Polarity RDY_A setting.
		USHORT	PolarityACT:1;		// Polarity ACT setting.
		USHORT	PolarityGPIO_0:1;	// Polarity GPIO#0 setting.
		USHORT	PolarityGPIO_1:1;	// Polarity GPIO#1 setting.
		USHORT	PolarityGPIO_2:1;	// Polarity GPIO#2 setting.
		USHORT	PolarityGPIO_3:1;	// Polarity GPIO#3 setting.
		USHORT	PolarityGPIO_4:1;	// Polarity GPIO#4 setting.
		USHORT	LedMode:5;			// Led mode.
		USHORT	Rsvd:3;				// Reserved		
	}	field;
	USHORT	word;
}	EEPROM_LED_STRUC, *PEEPROM_LED_STRUC;
#endif

#ifdef RT_BIG_ENDIAN
typedef	union	_EEPROM_TXPOWER_DELTA_STRUC	{
	struct	{
		UCHAR	TxPowerEnable:1;// Enable
		UCHAR	Type:1;			// 1: plus the delta value, 0: minus the delta value
		UCHAR	DeltaValue:6;	// Tx Power dalta value (MAX=4)
	}	field;
	UCHAR	value;
}	EEPROM_TXPOWER_DELTA_STRUC, *PEEPROM_TXPOWER_DELTA_STRUC;
#else
typedef	union	_EEPROM_TXPOWER_DELTA_STRUC	{
	struct	{
		UCHAR	DeltaValue:6;	// Tx Power dalta value (MAX=4)
		UCHAR	Type:1;			// 1: plus the delta value, 0: minus the delta value
		UCHAR	TxPowerEnable:1;// Enable
	}	field;
	UCHAR	value;
}	EEPROM_TXPOWER_DELTA_STRUC, *PEEPROM_TXPOWER_DELTA_STRUC;
#endif

#endif	// __RTMP_CHIP_H__ //
