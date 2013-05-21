/**********************************�ļ�ͷ��ע��************************************/
//
//
//                      Copyright (C), 2005-2010, AV Frontier Tech. Co., Ltd.
//
//
// �ļ�����     ywhal_tuner.h
//
// �����ߣ�     CS
//
// ����ʱ�䣺   2008.03.26
//
// �ļ�������   Tuner������ͷ�ļ���
//
// �޸ļ�¼��   ��       ��      ��      ��       �汾      �޶�
//              ---------         ---------       -----     -----
//              2008.03.26        CS              0.01      �½�
//
/*****************************************************************************************/

#ifndef __YWTUNER_EXT_H
#define __YWTUNER_EXT_H

#ifdef __cplusplus
extern "C" {
#endif
/************************************�궨��*******************************************/

#define YWHAL_MODULE_TUNER_ID 	16
#define YW_MODULE_SET_ID(a) 	(a << 16)

/************************************���ݽṹ****************************************/

/************************************��������****************************************/

/************************************��������****************************************/

typedef U32   YWTUNER_Handle_T;

/*Error Type*/
enum
{
    YWTUNER_ERROR_INVALID_FREQ = YW_MODULE_SET_ID(YWHAL_MODULE_TUNER_ID),
    YWTUNER_ERROR_INIT,
    YWTUNER_ERROR_CTRL,
    YWTUNER_ERROR_OPEN,
    YWTUNER_ERROR_TERM,
    YWTUNER_ERROR_POS_OUT_OF_RANGE
};


//����ģʽ����
typedef enum YWTUNER_DeliverType_e
{
    YWTUNER_DELIVER_SAT        = (1 << 0),  //���Ǵ���ģʽ
    YWTUNER_DELIVER_CAB        = (1 << 1),  //���ߴ���ģʽ
    YWTUNER_DELIVER_TER        = (1 << 2)   //���洫��ģʽ
}YWTUNER_DeliverType_T;

/*Code Rate*/
typedef enum YWTUNER_CodeRate_e
{
    YWTUNER_FEC_NONE    = (1 << 0),
    YWTUNER_FEC_1_2     = (1 << 1),
    YWTUNER_FEC_2_3     = (1 << 2),
    YWTUNER_FEC_3_4     = (1 << 3),
    YWTUNER_FEC_4_5     = (1 << 4),
    YWTUNER_FEC_5_6     = (1 << 5),
    YWTUNER_FEC_6_7     = (1 << 6),
    YWTUNER_FEC_7_8     = (1 << 7),
    YWTUNER_FEC_8_9     = (1 << 8),
    YWTUNER_FEC_1_4     = (1 << 9),
    YWTUNER_FEC_1_3     = (1 << 10),
    YWTUNER_FEC_2_5     = (1 << 11),
    YWTUNER_FEC_3_5     = (1 << 12),
    YWTUNER_FEC_9_10    = (1 << 13),
    YWTUNER_FEC_AUTO    = (1 << 14)
}YWTUNER_CodeRate_T;



typedef enum YWTUNER_DiseqcToneBurst_e
{
    YWTUNER_DISEQC_BURST_NONE,
    YWTUNER_DISEQC_BURST_A,
    YWTUNER_DISEQC_BURST_B
}YWTUNER_DiseqcToneBurst_T;

/*Sat Rotate Direction*/
typedef enum YWTUNER_SatRotateDirection_e
{
    YWTUNER_SAT_EASTERN,        /*��*/
    YWTUNER_SAT_WESTERN        /*��*/
}YWTUNER_SatRotateDirection_T;

/*Diseqc Type*/
typedef enum YWTUNER_DiseqcType_e
{
    YWTUNER_DISEQC_NONE     = (1 << 0),
    YWTUNER_DISEQC_1020     = (1 << 1),
    YWTUNER_DISEQC_1121     = (1 << 2),
    YWTUNER_DISEQC_1222     = (1 << 3),
    YWTUNER_DISEQC_USALS    = (1 << 4)
}YWTUNER_DiseqcType_T;


/*DISEQC10 port*/
typedef enum  YWTUNER_Diseqc10Port_e
{
    YWTUNER_DISEQC10_OFF     = 0 ,
    YWTUNER_DISEQC10_4_A,
    YWTUNER_DISEQC10_4_B,
    YWTUNER_DISEQC10_4_C,
    YWTUNER_DISEQC10_4_D,
    YWTUNER_DISEQC10_2_A,
    YWTUNER_DISEQC10_2_B
}YWTUNER_Diseqc10Port_T;


/*DISEQC11 port*/
typedef enum  YWTUNER_Diseqc11Port_e
{
    YWTUNER_DISEQC11_OFF = 0 ,
    YWTUNER_DISEQC11_1,
    YWTUNER_DISEQC11_2,
    YWTUNER_DISEQC11_3,
    YWTUNER_DISEQC11_4,
    YWTUNER_DISEQC11_5,
    YWTUNER_DISEQC11_6,
    YWTUNER_DISEQC11_7,
    YWTUNER_DISEQC11_8,
    YWTUNER_DISEQC11_9,
    YWTUNER_DISEQC11_10,
    YWTUNER_DISEQC11_11,
    YWTUNER_DISEQC11_12,
    YWTUNER_DISEQC11_13,
    YWTUNER_DISEQC11_14,
    YWTUNER_DISEQC11_15,
    YWTUNER_DISEQC11_16
}YWTUNER_Diseqc11Port_T;


/*Repeat Time*/
typedef enum YWTUNER_RepeatTime_e
{
    YWTUNER_NOREPEAT,
    YWTUNER_ONCEREPEAT,
    YWTUNER_TWICEREPEAT
} YWTUNER_RepeatTime_T;

/* SAT 12V*/
typedef enum YWTUNER_SAT12V_e
{
    YWTUNER_12V_OFF         = ( 1 << 0 ),
    YWTUNER_12V_ON          = ( 1 << 1 )
}YWTUNER_SAT12V_T;

/*SAT 22K*/
typedef enum YWTUNER_SAT22K_e
{
    YWTUNER_22K_OFF     = ( 1 << 0 ),
    YWTUNER_22K_ON      = ( 1 << 1 )

}YWTUNER_SAT22K_T;

/*SAT Motor State*/
typedef enum YWTUNER_MotorState_e
{
    YWTUNER_MOTORState_MOVING ,
    YWTUNER_MOTORState_STOP
}YWTUNER_MotorState_T;

/* Motor  operation (sat) */
/*Motor Go to position*/
typedef enum YWTUNER_MotorGotoCMD_e
{
    YWTUNER_MOTOR_GO_CENTER = 0 ,
    YWTUNER_MOTOR_GO_POSITION
}YWTUNER_MotorGotoCMD_T;

/*Motor Move west/east */
typedef enum YWTUNER_MotorMoveCMD_e
{
    YWTUNER_MOTOR_STOP = 0 ,
    YWTUNER_MOTOR_STEP_WEST  ,
    YWTUNER_MOTOR_WEST ,
    YWTUNER_MOTOR_STEP_EAST ,
    YWTUNER_MOTOR_EAST
}YWTUNER_MotorMoveCMD_T;

/*Motor Set Limit*/
typedef enum YWTUNER_MotorSetLimitCMD_e
{
    YWTUNER_MOTOR_WEST_LIMIT = 0 ,
    YWTUNER_MOTOR_EAST_LIMIT ,
    YWTUNER_MOTOR_DISABLE_LIMIT
}YWTUNER_MotorSetLimitCMD_T;

/*Motor CMD*/
typedef enum YWTUNER_MotorCMD_e
{
    YWTUNER_MOTOR_CMD_GOTO    = 0,
    YWTUNER_MOTOR_CMD_MOVE ,
    YWTUNER_MOTOR_CMD_SETLIMIT,
    YWTUNER_MOTOR_CMD_SAVEPOS,
    YWTUNER_MOTOR_CMD_AUTOMOVE
}YWTUNER_MotorCMD_T;

/*LNB Polarisation*/
typedef enum YWTUNER_LNBPolarisation_e
{
    YWTUNER_LNB_POLARISATION_H         = ( 1 << 0 ),
    YWTUNER_LNB_POLARISATION_V         = ( 1 << 1 )
}YWTUNER_LNBPolarisation_T;

/*LNBVoltage*/
typedef enum YWTUENR_LNBVoltage_e
{
    YWTUNER_LNB_VOLTAGE_OFF         = ( 1 << 0 ),
    YWTUNER_LNB_VOLTAGE_13          = ( 1 << 1 ),
    YWTUNER_LNB_VOLTAGE_18          = ( 1 << 2 ),
    YWTUNER_LNB_VOLTAGE_13_18_AUTO  = ( 1 << 3 ),

    YWTUNER_LNB_VOLTAGE_13_5        = ( 1 << 5 ),
    YWTUNER_LNB_VOLTAGE_18_5        = ( 1 << 6 ),
    YWTUNER_LNB_VOLTAGE_13_5_18_5_AUTO  = ( 1 << 7 )
}YWTUNER_LNBVoltage_T;


/*Modulation*/
typedef enum YWTUNER_Modulation_e
{
    YWTUNER_MOD_NONE        = (1 << 0),
    YWTUNER_MOD_QAM_16      = (1 << 1),
    YWTUNER_MOD_QAM_32      = (1 << 2),
    YWTUNER_MOD_QAM_64      = (1 << 3),
    YWTUNER_MOD_QAM_128     = (1 << 4),
    YWTUNER_MOD_QAM_256     = (1 << 5),
    YWTUNER_MOD_QPSK        = (1 << 6),
    YWTUNER_MOD_QPSK_S2     = (1 << 7),
    YWTUNER_MOD_8PSK        = (1 << 8),
    YWTUNER_MOD_16APSK      = (1 << 9),
    YWTUNER_MOD_32APSK      = (1 << 10),

    YWTUNER_MOD_BPSK        = (1 << 11),
    YWTUNER_MOD_8VSB        = (1 << 12),
    YWTUNER_MOD_AUTO        = (1 << 13)

}YWTUNER_Modulation_T;


//lwj add begin
/*SearchAlgo*/
typedef enum YWTUNER_SearchAlgo_e
{
    YWTUNER_SearchAlgo_COLD_START,
    YWTUNER_SearchAlgo_BLIND_SEARCH,
    YWTUNER_SearchAlgo_WARM_START
}YWTUNER_SearchAlgo_T;
//lwj add end


/*Bandwidth*/
typedef enum YWTUNER_TERBandwidth_e
{
    YWTUNER_TER_BANDWIDTH_8_MHZ    = (1 << 0),
    YWTUNER_TER_BANDWIDTH_7_MHZ    = (1 << 1),
    YWTUNER_TER_BANDWIDTH_6_MHZ    = (1 << 2)
}YWTUNER_TERBandwidth_T;

/*TUNER�ĵ�ǰ״̬*/
typedef enum YWTUNER_STATUS_e
{
    YWTUNER_STATUS_UNKNOWN= 0,  /*δ֪״̬*/
    YWTUNER_STATUS_UNLOCK,      /*Ƶ������ʧ��*/
    YWTUNER_STATUS_LOCKED       /*Ƶ�������ɹ�*/
}YWTUNER_STATUS_T;

/*TUNER��Standbyģʽ*/
typedef enum YWTUNER_STANDBYMODE_e
{
     YWTUNER_STANDBYMODE_WITHLOOP = 0, /*��LOOP��Standby*/
     YWTUNER_STANDBYMODE_WITHOUTLOOP   /*����LOOP��Standby*/
}YWTUNER_STANDBYMODE_T;

/************************************���ݽṹ****************************************/

//���Ǵ���ģʽ�µ�Ƶ��
typedef struct YWTUNER_SatelliteTransponder_s
{
    U32 Frequency;        /*Ƶ�ʣ���λKHz*/
    U32 SymbolRate;       /*�����ʣ���λKHz*/
    U32 FrequencyMin;     /*��СƵ�ʣ���λKHz��ע����ΪäɨӦ�о����Ƶ��ֵ��Ϊ��FrequencyMaxͬΪ0����ʾ��äɨ. */
    U32 FrequencyMax;     /*���Ƶ�ʣ���λKHz��ע����ΪäɨӦ�о����Ƶ��ֵ��Ϊ��FrequencyMinͬΪ0����ʾ��äɨ��*/
    U32 LNBFreq;          /*LNB�ı���Ƶ��*/
    YWTUNER_CodeRate_T      FECRate;
    YWTUNER_Modulation_T    Modulation;  /*���Ʒ�ʽ*/
    YWTUNER_SearchAlgo_T    SearchAlgo;  //lwj add for blind search and ordinary search
    U32 Reserved;
}YWTUNER_SatelliteTransponder_T;

/*Satellite Params*/
typedef struct YWTUNER_SatelliteParams_s
{
    S32                             OrbitalPosition;     /*�������ڵľ���λ��*/
    YWTUNER_SatRotateDirection_T    WestEastFlag;        /*�������ڵ�λ�õĶ�������־��0 - EASTERN, 1 - WESTERN */
    S32                             Reserved;
}YWTUNER_SatelliteParams_T;


/*Cable Deliver*/
//����Ƶ��
typedef struct YWTUNER_CableTransponder_s
{
    U32 Frequency;        /*Ƶ�ʣ���λKHz*/
    U32 SymbolRate;       /*�����ʣ���λKHz*/
    YWTUNER_Modulation_T Modulation;        /*���Ʒ�ʽ,AUTO,16QAM,32QAM,64QAM,128QAM,256QAM*/
    U32 Reserved;
}YWTUNER_CableTransponder_T;


/*Terrestrial Deliver*/
//����Ƶ��
typedef struct YWTUNER_TerrestrialTransponder_s
{
    U32 Frequency;                            /*Ƶ�ʣ���λKHz*/
    YWTUNER_TERBandwidth_T Banwith;           /*���Σ�0 - 8M, 1 - 7M, 2 - 6M */
    U32 Reserved;
}YWTUNER_TerrestrialTransponder_T;

/*DiSEqC 1020*/
typedef struct YWTUNER_DiSEqC1020_s
{
    U32 Toneburst;                        /* Tone Burst����: TONEBURST_NONE,TONEBURST_A,TONEBURST_B*/
    YWTUNER_Diseqc10Port_T Committed;     /* DiSEqC1.0����:    */
    U32 Reserved;
}YWTUNER_DiSEqC1020_T;

/*DiSEqC 1121*/
typedef struct YWTUNER_DiSEqC1121_s
{
    YWTUNER_Diseqc11Port_T Committed;    /* DiSEqC1.1����: */
    U32 RepeatedTimes;                   /*�ظ�������*/
    U32 Reserved;
}YWTUNER_DiSEqC1121_T;

typedef struct YWTUNER_MotorMove_s
{
    YWTUNER_MotorMoveCMD_T     CMDMotorMove;     /*��ﶫ����ת������*/
    U32                        StepNum;          /*���ת���Ĳ���*/
}YWTUNER_MotorMove_T;

typedef struct YWTUNER_MotorGoto_s
{
    YWTUNER_MotorGotoCMD_T     CMDMotorGoto;     /*���λ��ת��������*/
    U16                        Position;         /*�����ת����λ��*/
}YWTUNER_MotorGoto_T;

typedef struct YWTUNER_MotorSavePos_s
{
    U16 Pos;
}YWTUNER_MotorSavePos_T;

typedef struct YWTUNER_MotorAutoMove_s
{
    S32         LoclLatitudePos;         /*����γ��λ��*/
    S32         LoclLongitudePos;        /*���ؾ���λ��*/
    YWTUNER_SatelliteParams_T SatellitePos;             /*���ǵ�λ��*/
}YWTUNER_MotorAutoMove_T;

/*DiSEqC 1222*/
typedef struct YWTUNER_DiSEqC1222_s
{
    BOOL                        Committed;          /*DiSEqC1.2����:*/
    YWTUNER_MotorCMD_T          CMDType;            /*�����������*/
    union
    {
        YWTUNER_MotorMove_T            MotorMove;

        YWTUNER_MotorGoto_T            MotorGoto;

        YWTUNER_MotorSetLimitCMD_T     MotorSetLimit;    /*������ö�����λ������*/

        YWTUNER_MotorSavePos_T         MotorSavePos;
    }MotorCMD;
    U32                             Reserved;
}YWTUNER_DiSEqC1222_T;

/*DiSEqC1323*/
typedef struct YWTUNER_DiSEqC1323_s
{
    BOOL                      Committed;               /*DiSEqC1.3�Ŀ���*/
    YWTUNER_MotorCMD_T        CMDType;                 /*�����������*/
    union
    {
        YWTUNER_MotorMove_T            MotorMove;

        YWTUNER_MotorGoto_T            MotorGoto;

        YWTUNER_MotorSetLimitCMD_T     MotorSetLimit;    /*������ö�����λ������*/

        YWTUNER_MotorSavePos_T         MotorSavePos;

        YWTUNER_MotorAutoMove_T        MotorAutoMove;   //����Զ�ת����λ
    }MotorCMD;
    U32                       Reserved;
}YWTUNER_DiSEqC1323_T;

/*SourceDeliver*/
/*���ǡ����ߡ��������Ƶ����������*/
typedef union YWTUNER_SourceDeliver_u
{
    YWTUNER_CableTransponder_T             CableDeliver;               /*���߻�Ƶ����Ϣ�ṹ�塣*/
    YWTUNER_SatelliteTransponder_T         SatelliteDeliver;           /*���ǻ�Ƶ����Ϣ�ṹ�塣*/
    YWTUNER_TerrestrialTransponder_T       TerrestrialDeliver;         /*�����Ƶ����Ϣ�ṹ�塣*/
}YWTUNER_SourceDeliver_T;

typedef struct YWTUNER_SignalInfo_s
{
	 U32				QualityValue;   /*�ź�����ֵ*/
	 U32				StrengthValue;  /*�ź�ǿ��ֵ*/
	 U32 			    ErrorRate;	    /*�ź�������ֵ*/
} YWTUNER_SignalInfo_T;

#if 0
/*Feature ������*/
typedef struct YWTUNER_TunerDeviceList_e
{
    YWLIB_ListHead_T         YWTUNER_TunerList;
    YWTUNER_Modulation_T     Modulation;        /*֧�ֵĵ��Ʒ�ʽ*/
    U32                      TunerIndex;
    YWTUNER_DeliverType_T    TunerType;
    BOOL                     IsBlindScan;     /*�Ƿ�֧��äɨ*/
    void *                   PrivateData;
}YWTUNER_TunerDeviceList_T;


/*Tuner feature*/
typedef struct YWTUNER_Feature_s
{
    U32                    TunerNum;        /*Tuner ����*/
    YWLIB_ListHead_T       *TunerListHead;    /*Tuner ������   ++++++++++++++++*/
}YWTUNER_Feature_T;
#endif
/*Tuner �豸����*/
typedef U32 YWTUNER_Index_T;


/*Tuner Open Params*/
typedef struct YWTUNER_OpenParams_s
{
    YWTUNER_Index_T            TunerIndex;
    YWTUNER_DeliverType_T      TunerType;
    void *                     PrivateData;
 } YWTUNER_OpenParams_T;


/*�ص�����������
typedef  void ( *YWTUNER_EventProc_T)( YWTUNER_Handle_T  Handle, YW_EventType_T EventType, void* EventData, void * UserData);
*/
/************************************��������****************************************/

/************************************��������****************************************/

/************************************��������****************************************/
YW_ErrorType_T YWTUNER_Init(void);
YW_ErrorType_T YWTUNER_Term( void);
#if 0
YW_ErrorType_T YWTUNER_GetFeature( YWTUNER_Feature_T *TunerFeature);
#endif
YW_ErrorType_T YWTUNER_Open ( YWTUNER_Handle_T  * pTunerHandle,
                                        YWTUNER_OpenParams_T *TunerOpenParams);
YW_ErrorType_T YWTUNER_Close( YWTUNER_Handle_T  TunerHandle);
YW_ErrorType_T YWTUNER_ConnectTune( YWTUNER_Handle_T  TunerHandle,
                                                const YWTUNER_SourceDeliver_T* DeliverPar);
YW_ErrorType_T  YWTUNER_BreakTune(YWTUNER_Handle_T TunerHandle);

U32            YWTUNER_GetVersion( S8 *pchVer,U32 nSize  );

YW_ErrorType_T  YWTUNER_Sleep( YWTUNER_Handle_T  TunerHandle );  /*cab sat ter*/

YW_ErrorType_T  YWTUNER_SatSetPol( YWTUNER_Handle_T  TunerHandle,
                                            YWTUNER_LNBPolarisation_T Pol);
YW_ErrorType_T  YWTUNER_SatSet22K( YWTUNER_Handle_T  TunerHandle,
                                            YWTUNER_SAT22K_T   e22K);
YW_ErrorType_T  YWTUNER_SatSetPower( YWTUNER_Handle_T  TunerHandle,
                                                YWTUNER_LNBVoltage_T  Power);
YW_ErrorType_T  YWTUNER_SatSet12V( YWTUNER_Handle_T  TunerHandle,
                                            YWTUNER_SAT12V_T  e12V);
YW_ErrorType_T  YWTUNER_SatGetMotorState(  YWTUNER_Handle_T  TunerHandle,
                                                     YWTUNER_MotorState_T MotorState );
YW_ErrorType_T  YWTUNER_SatSetDiSEqC1020(  YWTUNER_Handle_T  TunerHandle,
                                                        YWTUNER_DiSEqC1020_T  *DiSEqC);
YW_ErrorType_T  YWTUNER_SatSetDiSEqC1121(  YWTUNER_Handle_T  TunerHandle,
                                                        YWTUNER_DiSEqC1121_T  *DiSEqC);
YW_ErrorType_T  YWTUNER_SatSetDiSEqC1222(  YWTUNER_Handle_T  TunerHandle,
                                                    YWTUNER_DiSEqC1222_T  *DiSEqC  );
YW_ErrorType_T  YWTUNER_SatSetDiSEqC1323(  YWTUNER_Handle_T  TunerHandle,
                                                        YWTUNER_DiSEqC1323_T  *DiSEqC );
YW_ErrorType_T  YWTUNER_Reset( YWTUNER_Handle_T  TunerHandle );

YW_ErrorType_T  YWTUNER_StandBy( YWTUNER_Handle_T  TunerHandle ,
                                    YWTUNER_STANDBYMODE_T StandbyMode);

YW_ErrorType_T  YWTUNER_WakeUp( YWTUNER_Handle_T  TunerHandle );

YW_ErrorType_T  YWTUNER_GetStatus( YWTUNER_Handle_T  TunerHandle ,
                                        YWTUNER_STATUS_T *Status);

YW_ErrorType_T  YWTUNER_GetSignalInfo( YWTUNER_Handle_T  TunerHandle ,
                                        YWTUNER_SignalInfo_T *SignalInfo);
#ifdef __cplusplus
}
#endif


#endif
