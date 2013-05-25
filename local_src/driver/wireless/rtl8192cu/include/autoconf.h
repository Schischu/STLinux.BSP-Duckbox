/*
 * Automatically generated C config: don't edit
 */
#define AUTOCONF_INCLUDED
#define RTL871X_MODULE_NAME "92CU"

//#define CONFIG_DEBUG_RTL871X 1

#define CONFIG_USB_HCI	1
#undef  CONFIG_SDIO_HCI
#undef CONFIG_PCIE_HCI

#undef CONFIG_RTL8711
#undef  CONFIG_RTL8712
#define	CONFIG_RTL8192C 1
#define	CONFIG_RTL8192D 1


//#define CONFIG_LITTLE_ENDIAN 1 //move to Makefile depends on platforms
//#undef CONFIG_BIG_ENDIAN

#undef PLATFORM_WINDOWS
#undef PLATFORM_OS_XP 
#undef PLATFORM_OS_CE


#define PLATFORM_LINUX 1

//#define CONFIG_PWRCTRL	1
//#define CONFIG_H2CLBK 1

//#define CONFIG_MP_INCLUDED 1

//#undef CONFIG_EMBEDDED_FWIMG
#define CONFIG_EMBEDDED_FWIMG 1

#define CONFIG_R871X_TEST 1

#define CONFIG_80211N_HT 1

#define CONFIG_RECV_REORDERING_CTRL 1

//#define CONFIG_RTL8712_TCP_CSUM_OFFLOAD_RX 1

//#define CONFIG_DRVEXT_MODULE 1

#define CONFIG_IPS	1
#define CONFIG_LPS	1
#define CONFIG_PM 	1
#define CONFIG_BT_COEXIST  	1
//#define CONFIG_ANTENNA_DIVERSITY	1


#ifdef CONFIG_RTL8192C

	#define DBG 0

	#define CONFIG_DEBUG_RTL8192C		1

	#define PCI_INTERFACE				1
	#define USB_INTERFACE				2	

	#define RTL8192C_RX_PACKET_NO_INCLUDE_CRC	1

	#define SUPPORTED_BLOCK_IO
	
	#ifdef CONFIG_USB_HCI

		#define DEV_BUS_TYPE	USB_INTERFACE

		#define USB_TX_AGGREGATION_92C	1
		#define USB_RX_AGGREGATION_92C	1

		#define RTL8192CU_FW_DOWNLOAD_ENABLE	1

		#define CONFIG_ONLY_ONE_OUT_EP_TO_LOW	0
	
		#define CONFIG_OUT_EP_WIFI_MODE	0

		#define ENABLE_USB_DROP_INCORRECT_OUT	0

		#define RTL8192CU_ASIC_VERIFICATION	0	// For ASIC verification.

		#define RTL8192CU_ADHOC_WORKAROUND_SETTING 1


		#ifdef PLATFORM_LINUX			
			#define CONFIG_PREALLOC_RECV_SKB 1			
			#define CONFIG_REDUCE_USB_TX_INT 1	
		#endif

		#ifdef CONFIG_R871X_TEST

			//#define CONFIG_AP_MODE 1
			//#define CONFIG_NATIVEAP_MLME 1

			#ifdef CONFIG_AP_MODE

				#ifndef CONFIG_NATIVEAP_MLME
					#define CONFIG_HOSTAPD_MLME 1
				#endif
			
			#endif
		
		#endif
    
	
	#endif

	#ifdef CONFIG_PCIE_HCI

		#define DEV_BUS_TYPE	PCI_INTERFACE
		
	#endif

	
	#define DISABLE_BB_RF	0	

	#define RTL8191C_FPGA_NETWORKTYPE_ADHOC 0

	//#define FW_PROCESS_VENDOR_CMD 1

	#ifdef CONFIG_MP_INCLUDED
		#define MP_DRIVER 1
	#else
		#define MP_DRIVER 0
	#endif

#endif

