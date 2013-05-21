

/* define to prevent recursive inclusion */
#ifndef __TUNER_H
#define __TUNER_H

/* constants --------------------------------------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif

/* common */
#define TUNER_DEMOD_BASE   10 /* to 50  */
#define TUNER_TUNER_BASE   60 /* to 100 */

/* satellite */
#define TUNER_LNB_BASE    210 /* to 250 */

#define STB0899_ExternalClock   4000000

#define TUNER_MAX_HANDLES 6

/* cable */

//#define USE_DCT70704

//#define USE_LG031

//#define USE_CD1616
//#define ADDR_CD1616 (0xc6)
/* terrestrial */

/* enumerated system types ------------------------------------------------- */

/* enumerated hardware selection ------------------------------------------- */

/* demod device-driver (common) */
typedef enum TUNER_DemodType_e
{
    TUNER_DEMOD_NONE = TUNER_DEMOD_BASE, /* common: null demodulation driver use with e.g. STV0399 demod driver*/
    TUNER_DEMOD_STV0299,                 /* sat: demodulation, fec & digital interface chip, */
    TUNER_DEMOD_MT312,                   /* sat: demodulation, fec & digital interface chip, */
    TUNER_DEMOD_STX0288,                 /* sat: demodulation, fec & digital interface chip, */
    TUNER_DEMOD_GX1101,                  /* sat: demodulation, fec & digital interface chip, */
    TUNER_DEMOD_10046,                   /* terrestrial */
    TUNER_DEMOD_MT352,                   /* terrestrial */
    TUNER_DEMOD_MT353,                   /* terrestrial */
    TUNER_DEMOD_PN2020,                  /* terrestrial */
    TUNER_DEMOD_STV0297,                 /* Cable : demodulation, fec & digital interface chip, J83 A/C */
    TUNER_DEMOD_10023,
    TUNER_DEMOD_STB0899,
    TUNER_DEMOD_STB0903,
    TUNER_DEMOD_STV07111,
    TUNER_DEMOD_M3501,
    TUNER_DEMOD_STI7167TER,
    TUNER_DEMOD_STI7167CAB
} TUNER_DemodType_T;       ///////////11111/////////////


/* tuner device-driver (common) */
typedef enum TUNER_TunerType_e
{
    TUNER_TUNER_NONE = TUNER_TUNER_BASE, /* null tuner driver e.g. use with 399 which has no external tuner driver */
    TUNER_TUNER_STB6000,  /* satellite */

    TUNER_TUNER_VZ0194,  /* satellite 2005.07.01.chm*/
    TUNER_TUNER_VZ0294,  /* satellite 2005.07.01.chm*/
    TUNER_TUNER_IX2410,  /* satellite 2005.07.01.chm*/
    TUNER_TUNER_STB6100,
    TUNER_TUNER_STB6110,
    TUNER_TUNER_SHARP7306,
    TUNER_TUNER_SHARP7803,

    TUNER_TUNER_TDTE9251,  /* terr 2005.10.11.chm*/
    TUNER_TUNER_SHARP6055,  /* terr 2005.10.11.chm*/
    TUNER_TUNER_SHARP6465,
    TUNER_TUNER_TDTMG252D,  /* terr 2006.06.13.chm*/
    TUNER_TUNER_DTOS446IH241F,  /* terr 2006.06.13.chm*/
    TUNER_TUNER_TD1311ALF,  /* terr 2006.06.13.chm*/
    TUNER_TUNER_EDTPH42PRF,  /* terr 2005.10.11.chm*/
    TUNER_TUNER_ED5265,  /* terr 2005.10.11.chm*/
    TUNER_TUNER_STV4100,

    TUNER_TUNER_MT2060,  /* cab 2006.04.13.chm*/
    TUNER_TUNER_TDA6651,  /* cab 2006.04.13.chm*/
    TUNER_TUNER_DCT70704,
    TUNER_TUNER_LG031,
    TUNER_TUNER_CD1616,
    TUNER_TUNER_SHARP5469C
} TUNER_TunerType_T;


/* lnb device-driver (sat) */
typedef enum TUNER_LnbType_e
{
    TUNER_LNB_NONE = TUNER_LNB_BASE,    /* null LNB driver */
    TUNER_LNB_STV0299,                  /* built in control of the LNB  */
    TUNER_LNB_MT312,
    TUNER_LNB_STX0288,
    TUNER_LNB_GX1101,
    TUNER_LNB_LNBH21,
    TUNER_LNB_STB0899
} TUNER_LnbType_T;


/* enumerated configuration get/set ---------------------------------------- */

/*  note: not all devices/devices support all configuration options, also with enumerations
   note that the values assigned are not to be taken as a direct mapping of the low level
   hardware registers (individual device drivers may translate these values) */

 /* mode of OFDM signal (ter) */
typedef enum TUNER_Mode_e
{
    TUNER_MODE_2K,
    TUNER_MODE_8K
} TUNER_Mode_T;


 /* guard of OFDM signal (ter) */
typedef enum TUNER_Guard_e
{
    TUNER_GUARD_1_32,               /* Guard interval = 1/32 */
    TUNER_GUARD_1_16,               /* Guard interval = 1/16 */
    TUNER_GUARD_1_8,                /* Guard interval = 1/8  */
    TUNER_GUARD_1_4                 /* Guard interval = 1/4  */
} TUNER_Guard_T;


 /* hierarchy (ter) */
typedef enum TUNER_Hierarchy_e
{
    TUNER_HIER_NONE,              /* Regular modulation */
    TUNER_HIER_1,                 /* Hierarchical modulation a = 1 */
    TUNER_HIER_2,                 /* Hierarchical modulation a = 2 */
    TUNER_HIER_4                  /* Hierarchical modulation a = 4 */
} TUNER_Hierarchy_T;

 /* hierarchy (ter) */
typedef enum TUNER_Priority_e
{
    TUNER_PRIORITY_H,
    TUNER_PRIORITY_L
} TUNER_Priority_T;


 /* (ter & cable) */
typedef enum TUNER_Spectrum_e
{
    TUNER_INVERSION_NONE = 0,
    TUNER_INVERSION      = 1,
    TUNER_INVERSION_AUTO = 2,
    TUNER_INVERSION_UNK  = 4
} TUNER_Spectrum_T;

typedef enum TUNER_J83_e
{
    TUNER_J83_NONE   = 0x00,
    TUNER_J83_ALL    = 0xFF,
    TUNER_J83_A      = 1,
    TUNER_J83_B      = (1 << 1),
    TUNER_J83_C      = (1 << 2)
} TUNER_J83_T;

/* (sat) */
typedef enum TUNER_IQMode_e
{
    TUNER_IQ_MODE_NORMAL          = 0,
    TUNER_IQ_MODE_INVERTED       = 1,
    TUNER_IQ_MODE_AUTO              = 2
} TUNER_IQMode_T;


/* (ter) */
typedef enum TUNER_FreqOff_e
{
    TUNER_OFFSET_NONE = 0,
    TUNER_OFFSET      = 1,
    TUNER_OFFSET_POSITIVE = 2,
    TUNER_OFFSET_NEGATIVE = 3
} TUNER_FreqOff_T;


/* (ter) */
typedef enum TUNER_Force_e
{
    TUNER_FORCE_NONE  = 0,
    TUNER_FORCE_M_G   = 1
} TUNER_Force_T;


/* FEC Modes (sat & terr) */
typedef enum TUNER_FECMode_e
{
    TUNER_FEC_MODE_DEFAULT,   /* Use default FEC mode */
    TUNER_FEC_MODE_DVB,       /* DVB mode */
    TUNER_FEC_MODE_DIRECTV    /* DirecTV mode */
} TUNER_FECMode_T;

/* FEC  Type detail (sat - 899) */
/* Needed only in 899 driver*/
typedef enum TUNER_FECType_e
{
    TUNER_FEC_MODE_DVBS1,
    TUNER_FEC_MODE_DVBS2          /* used for DVBS2 based Devices driver only, for 899 and ADV1*/
} TUNER_FECType_T;


/* LNB tone state (sat)
    typedef enum TUNER_LNBToneState_e
    {
        TUNER_LNB_TONE_DEFAULT,
         TUNER_LNB_TONE_OFF,
         TUNER_LNB_TONE_22KHZ
    } TUNER_LNBToneState_T;
*/
/* LNB DiSEqC state (sat)
    typedef enum TUNER_LNBDiSEqCState_e
    {
        TUNER_LNB_DISEQC_OFF,
         TUNER_LNB_DISEQC_A,
         TUNER_LNB_DISEQC_B,
        TUNER_LNB_DISEQC_C,
        TUNER_LNB_DISEQC_D
    } TUNER_LNBDiSEqCState_T;
*/
typedef enum TUNER_DiSEqCCommand_e
{
    TUNER_DiSEqC_TONE_BURST_OFF,                    /* Generic */
    TUNER_DiSEqC_TONE_BURST_OFF_F22_HIGH,            /* f22 pin high after tone off */
    TUNER_DiSEqC_TONE_BURST_OFF_F22_LOW,          /* f22 pin low after tone off*/
    TUNER_DiSEqC_TONE_BURST_SEND_0_UNMODULATED,   /* send of 0 for 12.5 ms ;continuous tone*/
    TUNER_DiSEqC_TONE_BURST_SEND_0_MODULATED,     /* 0-2/3 duty cycle tone*/
    TUNER_DiSEqC_TONE_BURST_SEND_1,                 /* 1-1/3 duty cycle tone*/
    TUNER_DiSEqC_COMMAND                            /* DiSEqC (1.2/2)command   */
} TUNER_DiSEqCCommand_T;

/* LNB  state (sat) */
typedef enum TUNER_MotorOperation_e
{
    TUNER_MOTO_OFF,       /* LNB disabled */
    TUNER_MOTO_STEP_WEST,       /* LNB  */
    TUNER_MOTO_WEST,       /* LNB  */
    TUNER_MOTO_STEP_EAST,       /* LNB  */
    TUNER_MOTO_EAST,
    TUNER_MOTO_STOP,
    TUNER_MOTO_GO_CENTER,
    TUNER_MOTO_GO_POSITION,
    TUNER_MOTO_WEST_LIMIT,
    TUNER_MOTO_EAST_LIMIT,
    TUNER_MOTO_DISABLE_LIMIT,
    TUNER_MOTO_SAVE_POSITION,
    TUNER_MOTO_USALS_POSITION

} TUNER_MotorOperation_T;

/* tuner status (common) */
typedef enum TUNER_Status_e
{
    TUNER_STATUS_UNLOCKED,
    TUNER_STATUS_SCANNING,
    TUNER_STATUS_LOCKED,
    TUNER_STATUS_NOT_FOUND,
    TUNER_STATUS_SLEEP,
    TUNER_STATUS_STANDBY
} TUNER_Status_T;

/*2005.07.11.chm
typedef enum TUNER_SCANTASK_Status_e
{
    TUNER_SCANTASK_RUNNING,
    TUNER_SCANTASK_SLEEP
}TUNER_SCANTASK_Status_T;
*/


/* transport stream output mode (common) */
typedef enum TUNER_TSOutputMode_e
{
    TUNER_TS_MODE_DEFAULT,    /* TS output not changeable */
    TUNER_TS_MODE_SERIAL,     /* TS output serial */
    TUNER_TS_MODE_PARALLEL,   /* TS output parallel */
    TUNER_TS_MODE_DVBCI       /* TS output DVB-CI */
} TUNER_TSOutputMode_T;

typedef enum TUNER_Standard_e
{
    TUNER_STANDARD_DVBS1,
    TUNER_STANDARD_DVBS2,
    TUNER_STANDARD_DSS
} TUNER_Standard_t;

typedef enum TUNER_TuneType_e
{
    TUNER_SET_NONE,
    TUNER_SET_LNB,
    TUNER_SET_FREQ,
    TUNER_SET_ALL
}TUNER_TuneType_t;


typedef enum TUNER_DataClockPolarity_e
{
    TUNER_DATA_CLOCK_POLARITY_DEFAULT = 0x00,   /*Clock polarity default*/
    TUNER_DATA_CLOCK_POLARITY_FALLING  ,        /*Clock polarity in rising edge*/
    TUNER_DATA_CLOCK_POLARITY_RISING  ,         /*Clock polarity in falling edge*/
    TUNER_DATA_CLOCK_NONINVERTED  ,             /*Non inverted*/
    TUNER_DATA_CLOCK_INVERTED                   /*inverted*/
}TUNER_DataClockPolarity_t;


//#define         TUNER_SET_NONE            0x00000000
//#define         TUNER_SET_LNB            0x00000001
//#define         TUNER_SET_FREQ            0x00000002
//#define         TUNER_SET_ALL            0xFFFFFFFF


/* data structures --------------------------------------------------------- */

/*-------------------------------------------------------------------------*/
#if 0
    /*�ź�ǿ���������ṹ��*/
    typedef struct TUNER_SignalInfo_s
    {
        U32 SignalQuality;        /*����*/
        U32 SignalIntensity;    /*ǿ��*/
        U32 SignalSNR;        /*�����*/
        U32 SignalBER;        /*������*/
    } TUNER_SignalInfo_T;
#endif

/*tuner �����ݿ��е�����
    typedef U32 U32 Index;*/


    /*tuner ��������豸����*/
typedef struct TUNER_IOParam_s
{
    TUNER_IORoute_t   Route;        /*���������ʽ()*/
    TUNER_IODriver_t  Driver;        /*������ʽ*/

    TUNER_DeviceName_t     DriverName;    /*�����豸����*/
    U32                    Address;    /*�豸��ַ*/
} TUNER_IOParam_T;


    /*DiSEqC �������ݽṹ*/
typedef struct TUNER_DiSEqCSendPacket_s
{
    U8  uc_TotalNoOfBytes ;            /*�����ֽ���*/
    U8  *pFrmAddCmdData ;            /*�������ݻ�����ָ��*/
    U8  uc_msecBeforeNextCommand;    /*�������η�������֮���ʱ����*/
    TUNER_DiSEqCCommand_T  DiSEqCCommandType;
} TUNER_DiSEqCSendPacket_T;

    /*DiSEqC �������ݽṹ(��ʱû��)*/
typedef struct TUNER_DiSEqCResponsePacket_s
{
    U8  uc_TotalBytesReceived ;
    U8  *ReceivedFrmAddData ;
    U8  uc_ExpectedNoOfBytes;
} TUNER_DiSEqCResponsePacket_T;

    /*�Զ����demod�Ľṹ��*/
typedef struct TUNER_DemodIdentifyDbase_s
{
    BOOL                    bEnableCheckTuner;    /*������ǰ��tuner*/
    TUNER_TunerType_T       *CheckTunerList;    /*��Ҫ����ǰ��tuner���б�*/
    U8                      CheckTunerNum;        /*��Ҫ����ǰ��tuner�ĸ���*/
    YWTUNER_DeliverType_T   DeviceType;            /*�豸����*/
    TUNER_DemodType_T       DemodType;            /*demod �ͺ�*/
    U8                      DemodID;            /*demod оƬID*/
    U8                      ucAddr;                /*demod оƬ��ַ*/
    /*��⹦�ܺ���*/
    YW_ErrorType_T          (*Demod_identify)(IOARCH_Handle_t   IOHandle,    /*demod io ���*/
                                                U8  ucID,                    /*Ҫ����оƬID*/
                                                U8  *pucActualID);            /*��ȡ����оƬID*/

    /*ת�����ܺ���*/
    YW_ErrorType_T            (*Demod_repeat)(IOARCH_Handle_t              DemodIOHandle, /*demod io ���*/
                                            IOARCH_Handle_t                TunerIOHandle, /*ǰ�� io ���*/
                                            TUNER_IOARCH_Operation_t       Operation,        /*��������*/
                                            U16                            SubAddr,        /*������ַ*/
                                            U8                            *Data,            /*ת������ָ��*/
                                            U32                            TransferSize,    /*ת�������ֽ���*/
                                            U32                            Timeout);        /*���ʳ�ʱʱ��ms*/
}TUNER_DemodIdentifyDbase_T;

    /*�Զ����ǰ�˵Ľṹ��*/
typedef struct  TUNER_TunerIdentifyDbase_s
{
    YWTUNER_DeliverType_T    DeviceType;        /*�豸����*/
    TUNER_TunerType_T        TunerType;        /*tuner �ͺ�*/
    U8                       ucAddr;            /*tuner оƬ��ַ*/
    YW_ErrorType_T           (*Tuner_identify)(IOARCH_Handle_t  IOHandle, TUNER_DemodType_T	DemodType);/*tuner ��⹦�ܺ���*/
}TUNER_TunerIdentifyDbase_T;

    /*lnb �����ýṹ��*/
typedef struct TUNER_SatLnbConfig_s
{
    U32                         Index;                /*����tuner ���*/
    BOOL                        ForceSetLnb;        /*�Ƿ�ǿ������lnb*/
    BOOL                        bSwitchDiSEqC_1_1;
    YWTUNER_LNBVoltage_T        Power;                /*lnb �Ĺ���״̬*/
    YWTUNER_LNBPolarisation_T    Polarization;            /*����*/
    YWTUNER_SAT12V_T            c12V;                /**/
    YWTUNER_SAT22K_T            ToneState;            /*22k ״̬*/
    YWTUNER_Diseqc10Port_T        DiSEqCState;        /*DiSEqC ״̬*/
    U8                            MotoDiSEqC;
    TUNER_MotorOperation_T        MotorOperation;        /*motor ��������*/
    U32                            MotorParam;        /*motor ��������*/
    BOOL                        bMotorMove;        /*motor �Ƿ����ƶ�*/
    U32                            TimeoutMotor;
} TUNER_SatLnbConfig_T;

    /*tuner �����ýṹ��*/
typedef struct TUNER_SatTunerConfig_s
{
    U32 Frequency;
    U32 TunerStep;
    U32 IF;
    U32 Bandwidth;
    S32 IQSense;
    U32 Reference;

    U32 FreqFactor;
    U8  IOBuffer[13];

} TUNER_SatTunerConfig_T;

/*sat �ĵ�г�����ṹ��*/
typedef struct TUNER_SatParam_s
{
    U32                         TuneType;        /*��г��������*/
    YWTUNER_LNBVoltage_T        Power;           /*lnb �Ĺ���״̬*/
    YWTUNER_LNBPolarisation_T   Polarization;    /*����*/
    YWTUNER_SAT12V_T            c12V;            /**/
    YWTUNER_SAT22K_T            ToneState;       /*22k ״̬*/
    YWTUNER_Diseqc10Port_T      DiSEqCState;     /*DiSEqC ״̬*/
    U8                          MotoDiSEqC;
    TUNER_MotorOperation_T      MotorOperation;  /*motor ��������*/
    U32                         MotorParam;      /*motor ��������*/

    YWTUNER_CodeRate_T          FECRates;        /*������*/
    YWTUNER_Modulation_T        Modulation;      /*����ģʽ*/

    U32                         MinSymbolRate;   /*��С������(ֻ����δ֪�����ʵ����)*/
    U32                         MaxSymbolRate;   /*��������(ֻ����δ֪�����ʵ����)*/
    U32                         LnbFreqKHz;      /*����Ƶ��*/
    U32                         FreqKHz;         /*����Ƶ��*/
    U32                         SymbolRateB;     /*������*/
    S32                         SymRateOffsetB;  /*������ƫ��*/
    S32                         FreqOffsetKHz;    /*����Ƶ��ƫ��*/
    TUNER_Standard_t            Standard;
    YWTUNER_SearchAlgo_T        SearchAlgo;     /* Search Algorithm*/  //lwj for blind search or cold search

}TUNER_SatParam_T;

/*sat demod �Ĺ��ܺ����ṹ��*/
typedef struct TUNER_SatDemodDriver_s
{
    /*�ر�demod*/
    YW_ErrorType_T        (*Demod_standy)(U8 Index );
    /*��λdemod*/
    YW_ErrorType_T        (*Demod_reset)(U8 Index );
    /*demod ת�����ܺ���*/
    YW_ErrorType_T        (*Demod_repeat)(IOARCH_Handle_t DemodIOHandle,
                                            IOARCH_Handle_t TunerIOHandle,
                                            TUNER_IOARCH_Operation_t Operation,
                                            U16 SubAddr,
                                            U8 *Data,
                                            U32 TransferSize,
                                            U32 Timeout);
    /*demod ��ȡ�ź�������ǿ��*/
    YW_ErrorType_T        (*Demod_GetSignalInfo)(U8 Index, U32  *Quality, U32 *Intensity, U32 *Ber);
    /*demod ��ȡ�ź��Ƿ�����*/
    YW_ErrorType_T        (*Demod_IsLocked)(U8 Index , BOOL  *IsLocked);
    /*demod ����һ��Ƶ�ʵ���������*/
    YW_ErrorType_T        (*Demod_ScanFreq)(U8 Index );
    /*demod diseqcָ��ķ��������*/
    YW_ErrorType_T        (*Demod_DiSEqC )    (U8 Index,
                                                 TUNER_DiSEqCSendPacket_T *pDiSEqCSendPacket,
                                                 TUNER_DiSEqCResponsePacket_T *pDiSEqCResponsePacket);
}TUNER_SatDemodDriver_T;

/*sat tuner �Ĺ��ܺ����ṹ��*/
typedef struct TUNER_SatTunerDriver_s
{
    /*tuner ������Ƶ��*/
    YW_ErrorType_T        (*tuner_SetFreq)(U8 Index ,
                                            U32  Frequency,
                                            U32  SymbolRate,
                                            U32  *NewFrequency);
    /*tuner ��ȡ���໷�Ƿ�����*/
    YW_ErrorType_T        (*tuner_IsLocked)(U8 Index , BOOL  *IsLocked);
    /*tuner ��ȡ���໷Ƶ��*/
    YW_ErrorType_T        (*tuner_GetFreq)(U8 Index, U32  *Frequency);
}TUNER_SatTunerDriver_T;

/*sat lnb �Ĺ��ܺ����ṹ��*/
typedef struct TUNER_SatLnbDriver_s
{
    /*lnb ������������*/
    YW_ErrorType_T        (*lnb_SetConfig)(U8 Index  , TUNER_SatParam_T  *TuneParam);
    /*lnb ��ȡ��������*/
    YW_ErrorType_T        (*lnb_GetConfig)(U8 Index  , TUNER_SatLnbConfig_T  *LnbConfig);
}TUNER_SatLnbDriver_T;


    /*  (sat) */

typedef struct TUNER_SatDriverParam_s
{
    TUNER_SatParam_T              Param;                /*��г����*/
    /*TUNER_SatParam_T            Result;                ��г���������*/

    TUNER_DemodType_T             DemodType;
    TUNER_IOREG_DeviceMap_t       Demod_DeviceMap;

    TUNER_SatDemodDriver_T        DemodDriver;        /*demod �Ĺ��ܺ���*/
    IOARCH_Handle_t               DemodIOHandle;        /*demod ��i2c���*/

    TUNER_TunerType_T             TunerType;
    TUNER_IOREG_DeviceMap_t       Tuner_DeviceMap;

    TUNER_SatTunerDriver_T        TunerDriver;        /*tuner �Ĺ��ܺ���*/
    IOARCH_Handle_t               TunerIOHandle;        /*tuner ��i2c���*/
    TUNER_SatTunerConfig_T        TunerConfig;        /*tuner �ĵ�ǰ����*/

    TUNER_SatLnbDriver_T          LnbDriver;            /*lnb �Ĺ��ܺ���*/
    IOARCH_Handle_t               LnbIOHandle;        /*lnb ��i2c���*/

    U32                           LNBPowerHandle;
    U32                           LNB1419VHandle;
    U32                           LNB1318VHandle;
    TUNER_SatLnbConfig_T        LnbConfig;            /*lnb �ĵ�ǰ����*/
}TUNER_SatDriverParam_T;

    /*tuner �����ýṹ��*/
typedef struct TUNER_TerTunerConfig_s
{
    U32 Frequency;
    U32 TunerStep;
    U32 IF;
    U32 Bandwidth;
    U32 Reference;

    U8  IOBuffer[5];

	U8	CalValue; //lwj add
	U8	CLPF_CalValue; //lwj add

} TUNER_TerTunerConfig_T;

/*ter���ĵ�г����*/
typedef struct TUNER_TerParam_s
{
    U32                        TuneType;/*��г��������*/
    U32                        FreqKHz;/*��гƵ��*/
    TUNER_Mode_T            Mode;/**/
    TUNER_Guard_T            Guard;
    TUNER_Force_T            Force;
    TUNER_Priority_T            Hierarchy;
    TUNER_Spectrum_T        Spectrum;
    TUNER_FreqOff_T            FreqOff;
    YWTUNER_TERBandwidth_T    ChannelBW;
    S32                        EchoPos;
    BOOL                    bAntenna;
	U8                    TuneMode;//jhy add for panosonic 6158 ,0=unkown,1=c;2=T;3=T2
}TUNER_TerParam_T;

/*ter demod �Ĺ��ܺ���*/
typedef struct TUNER_TerDemodDriver_s
{
    /*�ر�demod*/
    YW_ErrorType_T        (*Demod_standy)(U8 Index  );

    /*��λdemod*/
    YW_ErrorType_T        (*Demod_reset)(U8 Index  );
    /*demod ת�����ܺ���*/
    YW_ErrorType_T        (*Demod_repeat)(IOARCH_Handle_t DemodIOHandle,
                                            IOARCH_Handle_t TunerIOHandle,
                                            TUNER_IOARCH_Operation_t Operation,
                                            U16 SubAddr,
                                            U8 *Data,
                                            U32 TransferSize,
                                            U32 Timeout);
    /*demod ��ȡ�ź�������ǿ��*/
    YW_ErrorType_T        (*Demod_GetSignalInfo)(U8 Index  , U32  *Quality, U32  *Intensity, U32 *Ber);
    /*demod ��ȡ�ź��Ƿ�����*/
    YW_ErrorType_T        (*Demod_IsLocked)(U8 Index  , BOOL  *IsLocked);
    /*demod ����һ��Ƶ�ʵ���������*/
    YW_ErrorType_T        (*Demod_ScanFreq)(U8 Index  );
}TUNER_TerDemodDriver_T;

/*ter tuner �Ĺ��ܺ���*/
typedef struct TUNER_TerTunerDriver_s
{
    /*tuner ������Ƶ�ʺ���*/
    YW_ErrorType_T        (*tuner_SetFreq)(U8 Index,
                                            U32  Frequency,
                                            U32  BandWidth,
                                            U32  *NewFrequency);
    /*tuner ��ȡ���໷�Ƿ�����*/
    YW_ErrorType_T        (*tuner_IsLocked)(U8 Index, BOOL  *IsLocked);
    /*tuner ��ȡ���໷Ƶ��*/
    YW_ErrorType_T        (*tuner_GetFreq)(U8 Index,  U32  *Frequency);

    YW_ErrorType_T        (*tuner_SetBandWith)(U8 Index,  U32  BandWidth);
    YW_ErrorType_T        (*tuner_StartAndCalibrate)(U8 Index);
    YW_ErrorType_T        (*tuner_SetLna)(U8 Index);
    YW_ErrorType_T        (*tuner_AdjustRfPower) (U8 Index, S32 AGC2VAL2);
    YW_ErrorType_T        (*tuner_SetDVBC) (U8 Index);
    YW_ErrorType_T        (*tuner_SetDVBT) (U8 Index);
    YW_ErrorType_T        (*tuner_SetStandby)(U8 Index, U8 StandbyOn);
    S32                   (*tuner_GetRF_Level) (U8 Index,U16 RFAGC,U16 IFAGC);

}TUNER_TerTunerDriver_T;

/*ter lnb �Ĺ��ܺ����ṹ��*/
typedef struct TUNER_TerLnbDriver_s
{
    /*lnb ������������*/
    YW_ErrorType_T        (*lnb_SetConfig)(U8 Index  , TUNER_TerParam_T  *TuneParam);
}TUNER_TerLnbDriver_T;

typedef struct TUNER_TerDriverParam_s
{
    TUNER_TerParam_T            Param;            /*��г����*/
    /*TUNER_TerParam_T            Result;            ��г���������*/

    TUNER_DemodType_T            DemodType;
    TUNER_IOREG_DeviceMap_t        Demod_DeviceMap;

    TUNER_TerDemodDriver_T        DemodDriver;    /*demod �Ĺ��ܺ���*/
    IOARCH_Handle_t                DemodIOHandle; /*demod ��i2c���*/


    TUNER_TunerType_T            TunerType;
    TUNER_IOREG_DeviceMap_t        Tuner_DeviceMap;

    TUNER_TerTunerDriver_T        TunerDriver;    /*tuner �Ĺ��ܺ���*/
    IOARCH_Handle_t                TunerIOHandle;    /*tuner ��i2c���*/
    TUNER_TerTunerConfig_T        TunerConfig;    /*tuner �ĵ�ǰ����*/

    TUNER_TerLnbDriver_T        LnbDriver;        /*lnb �Ĺ��ܺ���*/

}TUNER_TerDriverParam_T;

/*cab*/
    /*tuner �����ýṹ��*/

#define TCDRV_IOBUFFER_MAX      21      /* 21 registers for MT2030/40 */
#define TCDRV_IOBUFFER_SIZE      4      /* 4 registers */
typedef struct TUNER_CabTunerConfig_ss
{
    U32            Frequency;
    U32            TunerStep;
    S32            IQSense;
    U32            IF;
    U32            FreqFactor;
    U32            BandWidth;
    U32            Reference;
	U8			   CalValue; //lwj add
	U8			   CLPF_CalValue; //lwj add

    U8            IOBuffer[TCDRV_IOBUFFER_MAX];    /* buffer for ioarch I/O */

} TUNER_CabTunerConfig_T;

typedef struct TUNER_CabParam_s
{
    U32                    TuneType;            /*��г��������*/
    U32                    FreqKHz;            /*��гƵ��*/
    U32                    SymbolRateB;    /*������*/

    TUNER_J83_T            J83;                 /* (cab) */
    TUNER_Spectrum_T       Spectrum;     /* (cable) */
    YWTUNER_Modulation_T   Modulation;

}TUNER_CabParam_T;

typedef struct TUNER_CabDemodDriver_s
{
    /*�ر�demod*/
    YW_ErrorType_T        (*Demod_standy)(U8 Index  );

    /*��λdemod*/
    YW_ErrorType_T        (*Demod_reset)(U8 Index  );
    /*demod ת�����ܺ���*/
    YW_ErrorType_T        (*Demod_repeat)(IOARCH_Handle_t DemodIOHandle,
                                            IOARCH_Handle_t TunerIOHandle,
                                            TUNER_IOARCH_Operation_t Operation,
                                            U16 SubAddr,
                                            U8 *Data,
                                            U32 TransferSize,
                                            U32 Timeout);
    /*demod ��ȡ�ź�������ǿ��*/
    YW_ErrorType_T        (*Demod_GetSignalInfo)(U8 Index  , U32  *Quality, U32  *Intensity, U32 *Ber);
    /*demod ��ȡ�ź��Ƿ�����*/
    YW_ErrorType_T        (*Demod_IsLocked)(U8 Index  , BOOL  *IsLocked);

    YW_ErrorType_T        (*Demod_ScanFreq)(U8 Index  );
}TUNER_CabDemodDriver_T;

typedef struct TUNER_CabTunerDriver_s
{
    /*tuner ������Ƶ�ʺ���*/
    YW_ErrorType_T        (*tuner_SetFreq)(U8 Index  ,
                                            U32  Frequency,
                                            U32  *NewFrequency);
    /*tuner ��ȡ���໷�Ƿ�����*/
    YW_ErrorType_T        (*tuner_IsLocked)(U8 Index  , BOOL  *IsLocked);
    /*tuner ��ȡ���໷Ƶ��*/
    YW_ErrorType_T        (*tuner_GetFreq)(U8 Index  , U32  *Frequency);
    YW_ErrorType_T        (*tuner_SetStandby)(U8 Index, U8 StandbyOn);



}TUNER_CabTunerDriver_T;

typedef struct TUNER_CabDriverParam_s
{
    TUNER_CabParam_T            Param;
    /*TUNER_CabParam_T            Result;*/

    TUNER_DemodType_T           DemodType;
    TUNER_IOREG_DeviceMap_t     Demod_DeviceMap;
    TUNER_CabDemodDriver_T      DemodDriver;
    IOARCH_Handle_t             DemodIOHandle;

    TUNER_TunerType_T           TunerType;
    TUNER_IOREG_DeviceMap_t     Tuner_DeviceMap;
    TUNER_CabTunerDriver_T      TunerDriver;
    IOARCH_Handle_t             TunerIOHandle;
    TUNER_CabTunerConfig_T      TunerConfig;/*tuner �ĵ�ǰ����*/

}TUNER_CabDriverParam_T;

/* initialization parameters, expand to incorporate cable & terrestrial parameters as needed (common) */
typedef struct TUNER_OpenParams_s
{
    /* (common) */
    U8                       TunerIndex;
    YWTUNER_DeliverType_T    Device;

    TUNER_DemodType_T        DemodType;
    TUNER_IOParam_T          DemodIO;

    TUNER_TunerType_T        TunerType; /*tuner ���ͺ�*/
    TUNER_IOParam_T          TunerIO;

    U32                      ExternalClock;
    TUNER_TSOutputMode_T     TSOutputMode;
    TUNER_FECMode_T          FECMode;

    U8                       I2CBusNo;

    /*  (sat) */
    BOOL                     IsOpened;//HandleIsValid;
    YWTUNER_Diseqc10Port_T   Committed10;
    YWTUNER_Diseqc11Port_T   Committed11;

}TUNER_OpenParams_T;


/*
typedef struct TUNER_TermParams_s
{
    BOOL ForceTerminate;
} TUNER_TermParams_T;
*/

/*��г���̽ṹ��*/
  #define    SCAN_TASK_STACK_SIZE        (4096)
  #define    SCAN_TASK_PRIORITY        5

#if 0
typedef struct TUNER_ScanTask_s
{
    U8              Index;
    U8              ScanTaskStack[SCAN_TASK_STACK_SIZE];
    YWOS_ThreadID_T ScanTaskID;
    YWOS_SemaphoreID_T        GuardSemaphore;
    YWOS_SemaphoreID_T        TimeoutSemaphore;

    char                      TaskName[16];
    U32                       TimeoutDelayMs;
    BOOL                      DeleteTask;
} TUNER_ScanTask_T;
#endif

typedef struct TUNER_TuneParams_s
{
    YWTUNER_DeliverType_T                Device;/*�豸����*/

    union
    {
        /* (sat) */
        TUNER_SatParam_T    Sat;
        /* (terrestrial) */
        TUNER_TerParam_T    Ter;
        /* (cable) */
        TUNER_CabParam_T    Cab;
    }Params;

}TUNER_TuneParams_T;

typedef enum STTUNER_OutputRateCompensationMode_e
{
    TUNER_COMPENSATE_DATACLOCK,
    TUNER_COMPENSATE_DATAVALID
}TUNER_OutputRateCompensationMode_T;

 typedef struct STTUNER_OutputFIFOConfig_s
{
    int CLOCKPERIOD;
    TUNER_OutputRateCompensationMode_T OutputRateCompensationMode;
}TUNER_OutputFIFOConfig_T;

typedef struct TUNER_ScanTaskParam_s
{
    /* (common) */
    U8                      TunerIndex;
    YWTUNER_DeliverType_T   Device;    /*�豸����*/
    TUNER_Status_T          Status;    /*�豸״̬*/
    /*TUNER_DeviceName_t    Name;    �豸����*/

    U32                        ExternalClock;
    TUNER_TSOutputMode_T       TSOutputMode;
    TUNER_FECMode_T            FECMode;
    TUNER_DataClockPolarity_t  ClockPolarity;
    TUNER_OutputFIFOConfig_T   OutputFIFOConfig;

    union
    {
        /* (sat) */
        TUNER_SatDriverParam_T    Sat;
        /* (terrestrial) */
        TUNER_TerDriverParam_T    Ter;
        /* (cable) */
        TUNER_CabDriverParam_T    Cab;
    }DriverParam;

    U8                        UnlockTimes;
    U8                        LockCount;

    //TUNER_ScanTask_T          ScanTask;            /*��������*/
    BOOL                      ForceSearchTerm;    /*�Ƿ�ǿ��ֹͣ��ǰ����*/

    void                      *userdata;

} TUNER_ScanTaskParam_T;





/* functions --------------------------------------------------------------- */

YW_ErrorType_T  TUNER_Open(U8 Index,  TUNER_OpenParams_T   *OpenParams);
YW_ErrorType_T  TUNER_Close(U8 Index  );

YW_ErrorType_T  TUNER_Tune(U8 Index  ,  TUNER_TuneParams_T   *TuneParams);
YW_ErrorType_T  TUNER_Sleep(U8 Index  );
YW_ErrorType_T  TUNER_Wakeup(U8 Index );

YW_ErrorType_T  TUNER_IDENTIFY_IoarchInit(void);
YW_ErrorType_T  TUNER_IDENTIFY_DemodInstall(TUNER_DemodIdentifyDbase_T  *Demod);
YW_ErrorType_T  TUNER_IDENTIFY_TunerInstall(TUNER_TunerIdentifyDbase_T  *Tuner);
YW_ErrorType_T  TUNER_IDENTIFY_DemodOpenIOHandle(IOARCH_Handle_t   *IOHandle, U8  ucAddr , U8 I2cIndex );
YW_ErrorType_T  TUNER_IDENTIFY_TunerOpenIOHandle(IOARCH_ExtHandle_t   *ExtIOHandle,
                                                IOARCH_Handle_t   *IOHandle,
                                                TUNER_IOARCH_RedirFn_t TargetFunction,
                                                U8  ucAddr,
                                                U8  I2cIndex
                                                );
YW_ErrorType_T  TUNER_IDENTIFY_CloseIOHandle(IOARCH_Handle_t   IOHandle);

YW_ErrorType_T  TUNER_ScanInfo_Open(U8  Index);

TUNER_ScanTaskParam_T *TUNER_GetScanInfo(U8  Index);

YW_ErrorType_T TUNER_Reset(U8  Index);

YW_ErrorType_T TUNER_Standby(U8  Index);

YW_ErrorType_T  TUNER_GetStatus(U8 Index , BOOL *IsLocked);

YW_ErrorType_T  TUNER_GetSignalInfo(U8 Index, YWTUNER_SignalInfo_T *SignalInfo);


#ifdef __cplusplus
}
#endif

#endif

/*eof--------------------------------------------------------------------------------------*/

