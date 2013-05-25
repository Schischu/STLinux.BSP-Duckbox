#ifndef	__RTL871X_RF_H_ 
#define __RTL871X_RF_H_

#include <drv_conf.h>
#include <rtw_cmd.h>

#define OFDM_PHY		1
#define MIXED_PHY		2
#define CCK_PHY		3

#define NumRates	(13)


#define RTL8711_RF_MAX_SENS 6
#define RTL8711_RF_DEF_SENS 4


#define NUM_CHANNELS	15
//#define NUM_REGULATORYS	21
#define NUM_REGULATORYS	1

//Country codes
#define USA							0x555320
#define EUROPE						0x1 //temp, should be provided later	
#define JAPAN						0x2 //temp, should be provided later	

struct	regulatory_class {
	u32	starting_freq;					//MHz, 
	u8	channel_set[NUM_CHANNELS];
	u8	channel_cck_power[NUM_CHANNELS];//dbm
	u8	channel_ofdm_power[NUM_CHANNELS];//dbm
	u8	txpower_limit;  				//dbm
	u8	channel_spacing;				//MHz
	u8	modem;
};


enum	_REG_PREAMBLE_MODE{
	PREAMBLE_LONG	= 1,
	PREAMBLE_AUTO	= 2,
	PREAMBLE_SHORT	= 3,
};


enum _RTL8712_RF_MIMO_CONFIG_{
 RTL8712_RFCONFIG_1T=0x10,
 RTL8712_RFCONFIG_2T=0x20,
 RTL8712_RFCONFIG_1R=0x01,
 RTL8712_RFCONFIG_2R=0x02,
 RTL8712_RFCONFIG_1T1R=0x11,
 RTL8712_RFCONFIG_1T2R=0x12,
 RTL8712_RFCONFIG_TURBO=0x92,
 RTL8712_RFCONFIG_2T2R=0x22
};


// Bandwidth Offset
#define HAL_PRIME_CHNL_OFFSET_DONT_CARE	0
#define HAL_PRIME_CHNL_OFFSET_LOWER	1
#define HAL_PRIME_CHNL_OFFSET_UPPER	2

// Represent Channel Width in HT Capabilities
//
typedef enum _HT_CHANNEL_WIDTH {
	HT_CHANNEL_WIDTH_20 = 0,
	HT_CHANNEL_WIDTH_40 = 1,
}HT_CHANNEL_WIDTH, *PHT_CHANNEL_WIDTH;

//
// Represent Extention Channel Offset in HT Capabilities
// This is available only in 40Mhz mode.
//
typedef enum _HT_EXTCHNL_OFFSET{
	HT_EXTCHNL_OFFSET_NO_EXT = 0,
	HT_EXTCHNL_OFFSET_UPPER = 1,
	HT_EXTCHNL_OFFSET_NO_DEF = 2,
	HT_EXTCHNL_OFFSET_LOWER = 3,
}HT_EXTCHNL_OFFSET, *PHT_EXTCHNL_OFFSET;

/* 2007/11/15 MH Define different RF type. */
typedef	enum _RT_RF_TYPE_DEFINITION
{
	RF_1T2R = 0,
	RF_2T4R = 1,
	RF_2T2R = 2,
	RF_1T1R = 3,
	RF_2T2R_GREEN = 4,
	RF_819X_MAX_TYPE = 5,
}RT_RF_TYPE_DEF_E;

struct setphyinfo_parm;
void init_phyinfo(_adapter  *adapter, struct setphyinfo_parm* psetphyinfopara);
u8 writephyinfo_fw(_adapter *padapter, u32 addr);
u32 ch2freq(u32 ch);
u32 freq2ch(u32 freq);


#ifdef CONFIG_RTL8712
#include "rtl8712_rf.h"
#endif


#endif //_RTL8711_RF_H_

