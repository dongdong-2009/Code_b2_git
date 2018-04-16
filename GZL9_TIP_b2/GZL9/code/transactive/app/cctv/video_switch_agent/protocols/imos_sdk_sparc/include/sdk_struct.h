/************************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
-------------------------------------------------------------------------------------
                              sdk_struct.h

  Project Code: SDK
   Module Name: SDK
  Date Created: 2009-02-27
        Author: yangbin (07022)
                sunyifan (s06966)
                zhuyi (KF1397)
   Description: ���ݽṹ�����ĵ�

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_SDK_STRUCT_H_
#define _IMOS_SDK_STRUCT_H_

#ifdef  __cplusplus
extern "C"{
#endif

/***************************************************************************
���ýṹ
****************************************************************************/

/***************************�û���¼����Բ���ʱЯ������Ϣ******************/

/**
 * @struct tagUserLoginIDInfo
 * @brief �û���¼ID��Ϣ�ṹ
 * @attention
 */
typedef struct tagUserLoginIDInfo
{
    /** �û����� */
    CHAR szUserCode[IMOS_USER_CODE_LEN];

    /** �û���¼ID�����û���¼�����������ģ����Ǳ��һ���û���¼��Ψһ��ʶ */
    CHAR szUserLoginCode[IMOS_RES_CODE_LEN];

    /** �û���¼�Ŀͻ���IP��ַ */
    CHAR szUserIpAddress[IMOS_IPADDR_LEN];

} USER_LOGIN_ID_INFO_S;


/************************************ʱ��ṹ*****************************/

/***********************��ͨ��ʱ��ṹ����ҵ��Ϊ���ģ��ṩ����ʱ��ƻ�***************************************/



/**
 * @struct tagTimeSlice
 * @brief ����ʱ��Ƭ��Ϣ
 * @attention 00:00:00~23:59:59��ʾһ�����ʱ��Ƭ
 */
typedef struct tagTimeSlice
{
    /** ��ʼʱ�� ��ʽΪ"hh:mm:ss"��"YYYY-MM-DD hh:mm:ss", ��ʹ��������� */
    CHAR        szBeginTime[IMOS_TIME_LEN];

    /** ����ʱ�� ��ʽΪ"hh:mm:ss"��"YYYY-MM-DD hh:mm:ss", ��ʹ��������� */
    CHAR        szEndTime[IMOS_TIME_LEN];

}TIME_SLICE_S;


/**
 * @struct tagTimeSliceInfo
 * @brief ʱ��Ƭ��Ϣ
 * @attention
 */
typedef struct tagTimeSliceInfo
{
    /** ����ʱ��Ƭ��Ϣ */
    TIME_SLICE_S    stTimeSlice;

}TIME_SLICE_INFO_S;

/**
 * @struct tagPlanWeekTime
 * @brief �ƻ�ʱ����Ϣ
 * @attention
 */
typedef struct tagPlanWeekTime
{
    /** �ܼ� */
    ULONG       ulWeekDay;

    /** ʱ����Ϣ, ���еĿ�ʼʱ��ͽ���ʱ���ʽΪ"hh:mm:ss" */
    TIME_SLICE_INFO_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM];

}PLAN_WEEKTIME_S;


/**
 * @struct tagPlanExtTime
 * @brief �ƻ�����ʱ����Ϣ, ����������ƻ�ʱ��ʱʹ��. ����ƻ����������ƻ��Ļ��������õ�����ִ�мƻ�.
 * @attention
 */
typedef struct tagPlanExtTime
{
    /** ������������, ȡֵΪ0~6: 0��ʾ����, 1~6��ʾ��һ������ */
    ULONG       ulDateIndex;

    /** ��������, ��ʽΪ"YYYY-MM-DD" */
    CHAR        szExtDate[IMOS_SIMPLE_DATE_LEN];

    /** ʱ��Ƭ������ʱ��Ƭ��ʵ������ */
    ULONG       ulTimeSliceNum;

    /** ʱ��Ƭ����, ���еĿ�ʼʱ��ͽ���ʱ��ĸ�ʽΪ"hh:mm:ss" */
    TIME_SLICE_INFO_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM];
}PLAN_EXTTIME_S;

/**
 * @struct tagPlanTime
 * @brief �ƻ�ʱ����Ϣ�ṹ
 * @attention
 */
typedef struct tagPlanTime
{
    /** ��ʱ��ƻ� */
    PLAN_WEEKTIME_S astPlanTime[IMOS_WEEK_DAYS];

    /** ����ʱ������������ʱ���ʵ������, ���ȡֵΪ#IMOS_EXCP_DAYS */
    ULONG       ulPlanExtTimeNum;

    /** ����ʱ������ */
    PLAN_EXTTIME_S astPlanExtTime[IMOS_EXCP_DAYS];

}PLAN_TIME_S;


/*******************��ҵ���ʱ���Ϊ���ģ�ÿ��ʱ���������ͬҵ��Ĳ�ͬ��Դ***************************************/

/**
 * @struct tagTimeSliceForRes
 * @brief ������Դ�������Դ���Ƶ�ʱ��Ƭ��Ϣ
 * @attention
 */
typedef struct tagTimeSliceForRes
{
    /** ��ʱ��ε���Դ���� */
    CHAR        szResCode[IMOS_RES_CODE_LEN];

    /** ��ʱ��ε���Դ���� */
    CHAR        szResName[IMOS_NAME_LEN];

    /** ʱ��Ƭ��Ϣ, ���еĿ�ʼʱ��ͽ���ʱ��ĸ�ʽΪ"hh:mm:ss" */
    TIME_SLICE_S    stTimeSlice;

}TIME_SLICE_FOR_RES_S;


/**
 * @struct tagPlanWeekTimeForRes
 * @brief ��ʱ��ƻ���Ϣ
 * @attention
 */
typedef struct tagPlanWeekTimeForRes
{
    /** �ܼ�, 0��ʾ����, 1~6��ʾ��һ������ */
    ULONG       ulWeekDay;

    /** ʱ����Ϣ */
    TIME_SLICE_FOR_RES_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM];

}PLAN_WEEKTIME_FOR_RES_S;


/**
 * @struct tagPlanExtTimeForRes
 * @brief ����ʱ����Ϣ
 * @attention
 */
typedef struct tagPlanExtTimeForRes
{
    /** �������� ��ʽΪ"YYYY-MM-DD" */
    CHAR        szExtDate[IMOS_TIME_LEN];

    /** ����ʱ��Ƭ����������ʱ��Ƭ��ʵ�ʸ��� */
    ULONG       ulTimeSliceNum;

    /** ����ʱ��Ƭ���� */
    TIME_SLICE_FOR_RES_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM];
}PLAN_EXTTIME_FOR_RES_S;

/**
 * @struct tagPlanTimeForRes
 * @brief �ƻ�ʱ����Ϣ
 * @attention
 */
typedef struct tagPlanTimeForRes
{
    /** ��ʱ��ƻ� */
    PLAN_WEEKTIME_FOR_RES_S astPlanTime[IMOS_WEEK_DAYS];

    /** ����ʱ��ƻ�������ʵ�ʵ�����ʱ��ƻ�����, ���ȡֵΪ#IMOS_EXCP_DAYS */
    ULONG       ulPlanExtTimeNum;

    /** ����ʱ��ƻ����� */
    PLAN_EXTTIME_FOR_RES_S  astPlanExtTime[IMOS_EXCP_DAYS];

}PLAN_TIME_FOR_RES_S;


/**
 * @struct tagTimeSliceForRes
 * @brief ������Դ�������Դ���Ƶ�ʱ��Ƭ��Ϣ
 * @attention
 */
typedef struct tagTimeSliceForResV2
{
    /** ��ʱ��ε���Դ���� */
    CHAR        szResCode[IMOS_RES_CODE_LEN];

    /** ��ʱ��ε���Դ���� */
    CHAR        szResName[IMOS_NAME_LEN];

    /** ʱ��Ƭ��Ϣ, ���еĿ�ʼʱ��ͽ���ʱ��ĸ�ʽΪ"hh:mm:ss" */
    TIME_SLICE_S    stTimeSlice;

}TIME_SLICE_FOR_RES_V2_S;


/**
 * @struct tagPlanWeekTimeForRes
 * @brief ��ʱ��ƻ���Ϣ
 * @attention
 */
typedef struct tagPlanWeekTimeForResV2
{
    /** �ܼ�, 0��ʾ����, 1~6��ʾ��һ������ */
    ULONG       ulWeekDay;

    /** ʱ��Ƭ��ʵ�ʸ��� */
    ULONG       ulTimeSliceNum;

    /** ʱ����Ϣ */
    TIME_SLICE_FOR_RES_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM_V2];

}PLAN_WEEKTIME_FOR_RES_V2_S;


/**
 * @struct tagPlanExtTimeForRes
 * @brief ����ʱ����Ϣ
 * @attention
 */
typedef struct tagPlanExtTimeForResV2
{
    /** �������� ��ʽΪ"YYYY-MM-DD" */
    CHAR        szExtDate[IMOS_TIME_LEN];

    /** ����ʱ��Ƭ����������ʱ��Ƭ��ʵ�ʸ��� */
    ULONG       ulTimeSliceNum;

    /** ����ʱ��Ƭ���� */
    TIME_SLICE_FOR_RES_S astTimeInfo[IMOS_MAX_TIME_SLICE_NUM_V2];
}PLAN_EXTTIME_FOR_RES_V2_S;

/**
 * @struct tagPlanTimeForRes
 * @brief �ƻ�ʱ����Ϣ
 * @attention
 */
typedef struct tagPlanTimeForRes_V2
{
    /** ��ʱ��ƻ� */
    PLAN_WEEKTIME_FOR_RES_V2_S astPlanTime[IMOS_WEEK_DAYS];

    /** ����ʱ��ƻ�������ʵ�ʵ�����ʱ��ƻ�����, ���ȡֵΪ#IMOS_EXCP_DAYS */
    ULONG       ulPlanExtTimeNum;

    /** ����ʱ��ƻ����� */
    PLAN_EXTTIME_FOR_RES_V2_S  astPlanExtTime[IMOS_EXCP_DAYS];

}PLAN_TIME_FOR_RES_V2_S;



/*******************************�ϴ����ع�����Ϣ***************************************/

/**
 * @struct tagFileBasicInfo
 * @brief �ļ�������Ϣ
 * @attention
 */
typedef struct tagFileBasicInfo
{
    /** �ļ�����, ���������û�����, ���ڱ�ʶ���ļ� */
    CHAR            szName[IMOS_NAME_LEN];

    /** �ļ���ʵ������, �����Ƽ��ļ����ļ�ϵͳ�е����� */
    CHAR            szFileName[IMOS_FILE_NAME_LEN];

    /** �ļ��ϴ�ʱ�䣬���ڱȶԿͻ����ļ��Ƿ������µ��ļ� */
    CHAR            szUploadTime[IMOS_TIME_LEN];
}FILE_BASIC_INFO_S;

/**
 * @struct tagLayerBasicInfo
 * @brief �ļ�������Ϣ
 * @attention
 */
typedef struct tagLayerBasicInfo
{
    /** �ļ���ʵ������, �����Ƽ��ļ����ļ�ϵͳ�е����� */
    CHAR            szLayerName[IMOS_NAME_LEN];

    /** �ļ��ϴ�ʱ�䣬���ڱȶԿͻ����ļ��Ƿ������µ��ļ� */
    CHAR            szUploadTime[IMOS_TIME_LEN];
}LAYER_BASIC_INFO_S;

/**
 * @struct tagFtpInfo
 * @brief FTP������Ϣ
 * @attention
 */
typedef struct tagFtpInfo
{
    /** FTP�û��� */
    CHAR    szUserName[IMOS_NAME_LEN];

    /** FTP���� */
    CHAR    szPassword[IMOS_PASSWD_LEN];

    /** FTP����������ļ�·����������"/Program_Files/H3C/IMOS"�ķ�ʽ��ʾ */
    CHAR    szFtpFilePath[IMOS_FILE_PATH_LEN];
}FTP_INFO_S;

/**
 * @struct tagFileOnServerInfo
 * @brief �ļ���FTP�������ϵ��й���Ϣ
 * @attention
 */
typedef struct tagFileOnServerInfo
{
    /** �ļ����ƣ��ļ�ʵ�ʱ�������� */
    CHAR            szFileName[IMOS_FILE_PATH_LEN];

    /** FTP������Ϣ */
    FTP_INFO_S  stFtpInfo;
}FILE_ON_SERVER_INFO_S;


/*********************************��ѯ����**********************************/
/**
 * @struct tagQueryPageInfo
 * @brief ��ҳ������Ϣ
 * @brief ����ѯ���ݵ�ÿ���������Ӧһ����š���Ŵ�1��ʼ���������ӡ�
 * - ��ѯ���Ľ����ҳ����ʽ���أ�ÿ�β�ѯֻ�ܷ���һҳ��ҳ������������ulPageRowNum�趨����ΧΪ1~200��
 * - ÿ�β�ѯ�������ôӴ���ѯ�������ض���ţ�ulPageFirstRowNumber����ʼ
 * @attention
 */
typedef struct tagQueryPageInfo
{
    /** ��ҳ��ѯ��ÿҳ�������Ŀ��, ����Ϊ0, Ҳ���ܴ���#IMOS_PAGE_QUERY_ROW_MAX_NUM */
    ULONG ulPageRowNum;

    /** ��ҳ��ѯ�е�һ�����ݵ����(����ѯ�ӵ�ulPageFirstRowNumber�����ݿ�ʼ�ķ�������������), ȡֵ����ULONG���͵ķ�Χ���� */
    ULONG ulPageFirstRowNumber;

    /** �Ƿ��ѯ��Ŀ����, BOOL_TRUEʱ��ѯ; BOOL_FALSEʱ����ѯ */
    BOOL_T bQueryCount;
}QUERY_PAGE_INFO_S;


/**
 * @struct tagRspPageInfo
 * @brief ��ҳ��Ӧ��Ϣ
 * @attention
 */
typedef struct tagRspPageInfo
{
    /** ʵ�ʷ��ص���Ŀ�� */
    ULONG ulRowNum;

    /** ��������������Ŀ�� */
    ULONG ulTotalRowNum;
}RSP_PAGE_INFO_S;

/**
 * @struct tagQueryConditionInfo
 * @brief �ƻ����ѯ�����������ƣ���ʱ���ѯ��
 * @attention
 */
typedef struct tagQueryConditionInfo
{
    /** ����ö�ٱ��λ:#NAME_CODE_FLAG_E */
    ULONG   ulNameCodeFlag;

    /** ������ģʽ:#SEARCH_MODE_E */
    ULONG   ulSearchMode;

    /** �豸���ƻ���Դ���� */
    CHAR    szNameCode[IMOS_NAME_LEN];

    /** ʱ��ö�ٱ��λ:#TIME_FLAG_E */
    ULONG   ulTimeFlag;

    /** ʱ�����Ϣ, ���п�ʼʱ��ͽ���ʱ��ĸ�ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S    stTimeSlice;

} QUERY_CONDITION_INFO_S;


/**
 * @struct tagQueryByName
 * @brief �����ƽ���ģ����ѯ��
 * @attention
 */
typedef struct tagQueryByName
{
    /** �豸���ƻ���Դ���� */
    CHAR    szName[IMOS_NAME_LEN];
} QUERY_BY_NAME_S;


/***************************************************************************************
��Ƶ�������
***********************************************************************************/


#if 0 /** ��Ƶ */
#endif
/**
 * @struct tagAudioInput
 * @brief ��Ƶ����ͨ����Ϣ
 * @attention
 */
typedef struct tagAudioInput
{
    /** �Ƿ�ʹ�� 0/1:��ʹ��/ʹ�� */
    ULONG       ulAudioEnabled;

    /** ��Ƶ���룬ȡֵ��ΧΪ #IMOS_AUDIO_FORMAT_E */
    ULONG       ulEncodeType;

    /** ��Ƶ������ȡֵ��ΧΪ #IMOS_AUDIO_CHANNEL_TYPE_E  */
    ULONG       ulTrack;

    /** �����ʣ�ȡֵ��ΧΪ #IMOS_AUDIO_SAMPLING_E */
    ULONG       ulSamplingRate;

    /** ���� */
    ULONG       ulEncodeRate;

    /** ��������ֵ��ȡֵ��ΧΪ0-255��Ĭ��0  */
    ULONG       ulIncrement;

    /** ��Ƶ�������� */
    CHAR        szDesc[IMOS_DESC_LEN];

}AUDIO_INPUT_INFO_S;



/**
 * @struct tagAudioOutput
 * @brief ��Ƶ���ͨ����Ϣ
 * @attention
 */
typedef struct tagAudioOutput
{
    /** ʹ�ܱ�ʶ */
    ULONG       ulAudioEnabled;

    /** ��Ƶ���룬ȡֵ��ΧΪ #IMOS_AUDIO_FORMAT_E */
    ULONG       ulEncodeType;

    /** ��Ƶ������ȡֵ��ΧΪ #IMOS_AUDIO_CHANNEL_TYPE_E */
    ULONG       ulTrack;

    /** ����ֵ */
    ULONG       ulVolume;

    /** ��Ƶ������� */
    CHAR        szDesc[IMOS_DESC_LEN];

}AUDIO_OUTPUT_INFO_S;

/************************************************************************************
�û���¼
*************************************************************************************/
#if 0
/**
* @struct  tagLoginInfo
* @brief �û���¼��Ϣ�ṹ��
* @attention ��
*/
typedef struct tagLoginInfo
{
    /** �û���¼��Ϣ */
    USER_LOGIN_ID_INFO_S    stUserLoginIDInfo;

    /** �û�������֯���� */
    CHAR szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �û����������� */
    CHAR szDomainName[IMOS_NAME_LEN];

    /** �û����������� */
    ULONG ulDomainType;

    /** XP���� */
    CHAR szXpCode[IMOS_DEVICE_CODE_LEN];

    /** XP��һ������� */
    CHAR szXpFirstWndCode[IMOS_RES_CODE_LEN];

    /** �����Խ����� */
    CHAR szVoiceTalkCode[IMOS_RES_CODE_LEN];

    /** �����㲥���� */
    CHAR szVoiceBroadcastCode[IMOS_RES_CODE_LEN];

    /** SIPͨ�ŵ�ַ���ͣ�ֵΪ:IMOS_IPADDR_TYPE_IPV4/IMOS_IPADDR_TYPE_IPV6 */
    ULONG ulSipAddrType;

    /** SIP������ͨ��IP��ַ������ʹ��XP��ʱ����Ч */
    CHAR szSipIpAddress[IMOS_IPADDR_LEN];

    /** SIP������ͨ�Ŷ˿ں� */
    ULONG ulSipPort;

    /** ������������� */
    CHAR szServerCode[IMOS_DEVICE_CODE_LEN];
}LOGIN_INFO_S;

#endif

/**
* @struct tagLoginInfo
* @brief �û���¼��Ϣ�ṹ��
* @attention ��
*/
typedef struct tagLoginInfo
{
    /** �û���¼ID��Ϣ */
    USER_LOGIN_ID_INFO_S    stUserLoginIDInfo;

    /** �û�������֯���� */
    CHAR szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �û����������� */
    CHAR szDomainName[IMOS_NAME_LEN];

    /** �û�����������, ȡֵΪ#MM_DOMAIN_SUBTYPE_LOCAL_PHYSICAL��#MM_DOMAIN_SUBTYPE_LOCAL_VIRTUAL */
    ULONG ulDomainType;
}LOGIN_INFO_S;

/**
* @struct tagXpInfo
* @brief XP��Ϣ�ṹ��
* @attention ��
*/
typedef struct tagXpInfo
{
    /** XP���� */
    CHAR szXpCode[IMOS_DEVICE_CODE_LEN];

    /** ���� */
    ULONG ulScreenIndex;

    /** XP��һ������� */
    CHAR szXpFirstWndCode[IMOS_RES_CODE_LEN] ;

    /** �����Խ����� */
    CHAR szVoiceTalkCode[IMOS_RES_CODE_LEN] ;

    /** �����㲥���� */
    CHAR szVoiceBroadcastCode[IMOS_RES_CODE_LEN] ;

    /** SIPͨ�ŵ�ַ���ͣ�#IMOS_IPADDR_TYPE_IPV4ΪIPv4����; #IMOS_IPADDR_TYPE_IPV6ΪIPv6���� */
    ULONG ulSipAddrType ;

    /** SIP������ͨ��IP��ַ������ʹ��XP��ʱ����Ч */
    CHAR szSipIpAddress[IMOS_IPADDR_LEN] ;

    /** SIP������ͨ�Ŷ˿ں� */
    ULONG ulSipPort;

    /** ������������� */
    CHAR szServerCode[IMOS_DEVICE_CODE_LEN] ;

} XP_INFO_S;

/**
 * @struct tagXpStreamInfo
 * @brief XPʵʱ�������Ϣ�ṹ
 * @attention
 */
typedef struct tagXpStreamInfo
{
    /** ֧�ֵĵ��鲥���ͣ�0Ϊ��֧�ֵ�����1Ϊ��֧�ֵ���Ҳ֧���鲥 */
    ULONG ulStreamType;

    /** ֧�ֵ�������Э�� �μ�#IMOS_TRANS_TYPE_E��ĿǰXPֻ֧������Ӧ��TCP */
    ULONG ulStreamTransProtocol;

    /** ֧�ֵ������䷽ʽ �μ�#IMOS_STREAM_SERVER_MODE_E��ĿǰXPֻ֧������Ӧ��ֱ������ */
    ULONG ulStreamServerMode;

    /** �����ֶ� */
    CHAR  szReserve[32];
}XP_STREAM_INFO_S;

/**
* @struct tagUserLoginIDInfo
* @brief ������������Ϣ�ṹ��
* @attention
*/
typedef struct tagPlayWndInfo
{
    /** ���Ŵ�����Դ���� */
    CHAR szPlayWndCode[IMOS_RES_CODE_LEN];
}PLAY_WND_INFO_S;

/************************************************************************************
��ɫ
*************************************************************************************/
/**
 * @struct tagUserRoleInfo
 * @brief ��ɫ��ϸ��Ϣ
 * @attention
 */
typedef struct tagUserRoleInfo
{
    /** ��ɫ���� */
    CHAR        szRoleName[IMOS_NAME_LEN];

    /** ��ɫ���ȼ���ȡֵ��ΧΪ1-9��1�����ȼ���ߣ�9�����ȼ���� */
    ULONG       ulRolePriority;

    /** ��ɫ���������Բ��� */
    CHAR        szRoleDesc[IMOS_DESC_LEN];

    /** ��ɫ������֯����� */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��ɫ���ͣ�����ӱ����ɫʱ��ֵΪ1 */
    ULONG       ulRoleType;

    /** ��չ��Ϣ���û������ֶΣ����Բ��� */
    CHAR        szExternInfo[IMOS_STRING_LEN_32];
}USER_ROLE_INFO_S;

/**
 * @struct tagRoleNameQueryItem
 * @brief ��ɫ���롢������Ϣ�����ͨ������Ϣ����ý�ɫ�ľ�����Ϣ
 * @attention
 */
typedef struct tagRoleNameQueryItem
{
    /** ��ɫ���� */
    CHAR        szRoleName[IMOS_NAME_LEN];

    /** ��ɫ���룬��ɫ��Ψһ��ʶ */
    CHAR        szRoleCode[IMOS_RES_CODE_LEN];
}ROLE_NAME_QUERY_ITEM_S;

/**
 * @struct tagRoleQueryItem
 * @brief ��ɫ��Ϣ��(��ѯ��ɫ�б�ʱ����)
 * @attention
 */
typedef struct tagRoleQueryItem
{
    /** ��ɫ���롢������Ϣ */
    ROLE_NAME_QUERY_ITEM_S  stRoleIndexItem;

    /** ��ɫ���ȼ���ȡֵ��ΧΪ1-9��1�����ȼ���ߣ�9�����ȼ���� */
    ULONG                       ulRolePriority;

    /** ��ɫ���� */
    CHAR                        szRoleDesc[IMOS_DESC_LEN];

    /** ��ɫ������֯���� */
    CHAR                        szOrgName[IMOS_NAME_LEN];

    /** ��ɫ������֯���� */
    CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN];
}ROLE_QUERY_ITEM_S;

/**
* @struct tagAuthorityItem
* @brief ��ͨȨ����Ϣ
* @attention ��
*/
typedef struct tagAuthorityItem
{
    /** Ȩ�����֣�ÿ��ȡֵΨһ����һ��Ȩ�����ͣ����ɲ�Ʒ�趨Ȩ��ʱȷ�� */
    ULONG ulAuthorityName;

    /** �Ƿ�߱�Ȩ�����ֶ�Ӧ��Ȩ��, BOOL_TRUEΪ���и�Ȩ��, BOOL_FALSEΪ�����и�Ȩ�� */
    BOOL_T bHasAuthority;

} AUTHORITY_ITEM_S;

/**
* @struct tagOrgAuthority
* @brief ��֯���Ȩ����Ϣ
* @attention ��
*/
typedef struct tagOrgAuthority
{
    /** Ȩ����Ϣ���� */
    AUTHORITY_ITEM_S astAuthorities[IMOS_AUTHORITY_MAX_NUM];

    /** Ȩ����Ϣ������ʵ�ʵ�Ȩ����Ϣ����, ��ȡ�����ֵΪ#IMOS_AUTHORITY_MAX_NUM */
    ULONG ulAuthCount;

} ORG_AUTHORITY_S;

/**
* @struct tagOrgAuthorityV2
* @brief ��֯���Ȩ����Ϣ
* @attention ��
*/
typedef struct tagOrgAuthorityV2
{
    /** Ȩ����Ϣ���� */
    AUTHORITY_ITEM_S astAuthorities[IMOS_AUTHORITY_MAX_NUM_EX];

    /** Ȩ����Ϣ������ʵ�ʵ�Ȩ����Ϣ����, ��ȡ�����ֵΪ#IMOS_AUTHORITY_MAX_NUM_EX */
    ULONG ulAuthCount;

} ORG_AUTHORITY_V2_S;




/**
 * @struct tagQueryRoleConditionInfo
 * @brief ��ѯ���µĽ�ɫ�б�
 * @attention ROLE_ORDER_INFO_S��ҪUI�ṩ
*/
typedef struct tagQueryRoleConditionInfo
{
    /** ��֯����� */
    CHAR    szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��ɫ���� */
    CHAR    szRoleName[IMOS_NAME_LEN];

}QUERY_ROLE_CONDITION_INFO_S;

/**
* @struct tagResAuthority
* @brief ��Դ��Ȩ����Ϣ
* @attention ��
*/
typedef struct tagResAuthority
{
    /** Ȩ���б� */
    ORG_AUTHORITY_V2_S  stAuthorities;

    /** ��ԴID����,ȫ��Ȩ��Ϊ�ַ���"0" */
    CHAR            szResIdCode[IMOS_CODE_LEN];

} RES_AUTHORITY_S;

/**
* @struct tagResInfo
* @brief ��Դ��Ϣ
* @attention ��
*/
typedef struct tagResInfo
{
    /** ��ԴID���ַ���"0"��ʾϵͳ */
    CHAR    szResIdCode[IMOS_CODE_LEN];

    /** ��Դ���� */
    CHAR    szResCode[IMOS_CODE_LEN];

    /** ��Դ���� */
    CHAR    szResName[IMOS_NAME_LEN];

    /** ��Դ���ͣ�ȡֵ��ΧΪ#IMOS_TYPE_E */
    ULONG   ulResType;

    /** ��Դ������,Ŀǰ��Դ������ֻ�������/��֯/����ʾ/����Ѳ��Ч�������������Ϊ��̨/����̨;
        ����֯����Ϊ:1-��������2-�����������3-�����������. 4-�ϼ���������.
        5-�¼���������.6-ƽ����������.
        ������ʾ��Դ��������ʾ���ͣ�ȡֵΪ#SALVO_TYPE_E
        ������Ѳ��Դ��������Ѳ���ͣ�ȡֵΪ#GROUP_SWITCH_TYPE_E */
    ULONG   ulResSubType;

    /** ������֯���� */
    CHAR        szOrgCode[IMOS_CODE_LEN];

    /** ������֯���� */
    CHAR        szOrgName[IMOS_NAME_LEN];

} RES_INFO_S;

/**
* @struct tagResAuthorityQueryItem
* @brief ��Դ��Ȩ���б�
* @attention ��
*/
typedef struct tagResAuthorityQueryItem
{
    /** Ȩ���б� */

    ORG_AUTHORITY_V2_S stAuthorities;

    /** ��Դ��Ϣ */
    RES_INFO_S stResInfo;
} RES_AUTHORITY_QUERY_ITEM_S;


/***************************************************************************************
�û�����
***********************************************************************************/

/**
 * @struct tagUserInfo
 * @brief �û���Ϣ
 * @attention
 */
typedef struct tagUserInfo
{

    /** �û����룬�û���Ψһ��ʶ */
    CHAR szUserCode[IMOS_USER_CODE_LEN];

    /** �û���¼�� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �û�ȫ�������Բ��� */
    CHAR szUserFullName[IMOS_NAME_LEN];

    /** �û�������֯������ */
    CHAR szOrgName[IMOS_NAME_LEN];

    /** �û������绰���룬���Բ��� */
    CHAR    szTelePhone[IMOS_PHONE_LEN];

    /** �û��ƶ��绰���룬���Բ��� */
    CHAR    szMobilePhone[IMOS_MOBILE_LEN];

    /** �û�������룬���Բ��� */
    CHAR    szFax[IMOS_PHONE_LEN];

    /** �û������ʼ������Բ��� */
    CHAR    szEmail[IMOS_MAIL_LEN];

    /** �û������Ƿ�����Ч��ʶ, 1Ϊ������Ч; 0Ϊ�ǳ�����Ч, Ŀǰ���ֶα��� */
    ULONG   ulPasswdPermanetFlag;

    /** �û�������Ч����, Ŀǰ���ֶα���, ���Բ��� */
    CHAR    szPasswdValidDate[IMOS_TIME_LEN];

    /** �û�������Ч����, Ŀǰ���ֶα��� */
    ULONG   ulPasswdValidDays;

    /** �û������Ƿ���Ч, 1Ϊ��Ч; 0Ϊ��Ч */
    ULONG   ulPasswdValidFlag;

    /** �û��Ƿ�����, 1Ϊ������; 0Ϊδ������ */
    ULONG   ulUserLockFlag;

    /** �û�����, Ŀǰ�̶�Ϊ#MM_USER_TYPE_LOCAL_DOMAIN */
    ULONG   ulUserType;

    /** �û����������Բ��� */
    CHAR    szUserDes[IMOS_DESC_LEN];

    /** �û���չ��Ϣ�����Բ��� */
    CHAR    szReserve[96];
}USER_INFO_S;



/**
 * @struct tagUserPasswordInfo
 * @brief �û���Ϣ(�����û���¼����)
 * @attention
 */
typedef struct tagUserPasswordInfo
{
    /** �û���Ϣ */
    USER_INFO_S   stUserInfo;

    /** �û���¼����,����Ϊ�� */
    CHAR    szPassword[IMOS_PASSWD_ENCRYPT_LEN];
}USER_INFO_PASSWORD_S;


/**
 * @struct  tagUserModifyPassword
 * @brief �û�������Ϣ
 * @attention
 */
typedef struct tagUserModifyPassword
{
    /** �û���¼�� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �û������� */
    CHAR    szOldPassword[IMOS_PASSWD_ENCRYPT_LEN];

    /** �û������� */
    CHAR    szNewPassword[IMOS_PASSWD_ENCRYPT_LEN];
}USER_MODIFY_PASSWORD_S;


/**
 * @struct  tagUserQueryItem
 * @brief �û���Ϣ��(��ѯ�û��б�ʱ����)
 * @attention
 */
typedef struct tagUserQueryItem
{
    /** �û����� */
    CHAR szUserCode[IMOS_USER_CODE_LEN];

    /** �û���¼ID(���ڲ�����)����һ���û���¼��Ψһ��ʶ */
    CHAR szUserLoginCode[IMOS_RES_CODE_LEN];

    /** �û���¼�� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �û�ȫ�� */
    CHAR szUserFullName[IMOS_NAME_LEN];

    /** �û�������֯������ */
    CHAR szOrgName[IMOS_NAME_LEN];

    /** �û������绰���� */
    CHAR    szTelePhone[IMOS_PHONE_LEN];

    /** �û��ƶ��绰���� */
    CHAR    szMobilePhone[IMOS_MOBILE_LEN];

    /** �û������ʼ� */
    CHAR    szEmail[IMOS_MAIL_LEN];

    /** �û��Ƿ�����, 1Ϊ����; 2Ϊδ���� */
    ULONG   ulUserLockFlag;

    /** �û���¼�Ŀͻ��˵�ַ */
    CHAR szUserAddr[IMOS_IPADDR_LEN];

    /** �û���չ��Ϣ */
    CHAR    szExternInfo[IMOS_STRING_LEN_32];

    /** �û���¼ʱ�� */
    CHAR  szUserLoginTime[IMOS_TIME_LEN];

} USER_QUERY_ITEM_S;



/***************************************************************************************
��֯�������������
***********************************************************************************/

/**
 * @struct tagOrgBasicInfo
 * @brief ��֯�ڵ������Ϣ
 * @attention
 */
typedef struct tagOrgBasicInfo
{
     /** ��֯�ڵ���� */
    CHAR szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��֯�ڵ����� */
    CHAR szOrgName[IMOS_NAME_LEN];

    /** ��(�ϼ�)��֯�ڵ���� */
    CHAR szOrgParentCode[IMOS_DOMAIN_CODE_LEN];

    /** ��֯�ڵ����� ,ȡֵΪ #MM_DOMAINTYPE_E */
    ULONG ulOrgType;

    /** ��֯�ڵ��Ƿ��Ǳ����, 1Ϊ�Ǳ����; 0Ϊ���Ǳ���� */
    ULONG ulIsLocalDomain;

    /** ��֯�ڵ�����,���Բ��� */
    CHAR szOrgDesc[IMOS_DESC_LEN];

    /** ��չ��Ϣ, ���Բ��� */
    CHAR szUserInfo[IMOS_STRING_LEN_256];
}ORG_BASIC_S;



/**
 * @struct tagOrgContactInfo
 * @brief ��֯�ڵ���ϵ��ʽ��Ϣ
 * @attention
 */
typedef struct tagOrgContactInfo
{
    /** ��ϵ�ˣ����Բ��� */
    CHAR szOrgContszt[IMOS_NAME_LEN];

    /** �ƶ��绰���룬���Բ��� */
    CHAR szOrgMobile[IMOS_MOBILE_LEN];

    /** ��ϵ��ַ�����Բ��� */
    CHAR szOrgAddr[IMOS_STRING_LEN_256];

    /** ������룬���Բ��� */
    CHAR szOrgFax[IMOS_PHONE_LEN];

    /** �������룬���Բ��� */
    CHAR szOrgTel[IMOS_PHONE_LEN];

    /** �����ʼ������Բ��� */
    CHAR szOrgEmail[IMOS_MAIL_LEN];

    /** �������룬���Բ��� */
    CHAR szOrgPostalCode[IMOS_MAIL_CODE_LEN];
}ORG_CONTACT_INFO_S;



/**
 * @struct tagOrgNodeInfo
 * @brief ��֯��ڵ���Ϣ����֯������Ϣ+��ϵ��ʽ��Ϣ��
 * @attention
 */
typedef struct tagOrgNodeInfo
{
    /** ��֯�ڵ������Ϣ */
    ORG_BASIC_S         stOrgBasicInfo;

    /** ��֯�ڵ���ϵ��ʽ��Ϣ */
    ORG_CONTACT_INFO_S  stOrgContactInfo;

}ORG_NODE_INFO_S;



/**
 * @struct tagAuthorizationInfo
 * @brief Ȩǩ��Ϣ
 * @attention
 */
typedef struct tagAuthorizationInfo
{
    /** �û��� */
    CHAR    szUserName[IMOS_NAME_LEN];

    /** ���� */
    CHAR    szPassword[IMOS_PASSWD_LEN];

    /** ���� */
    CHAR    szRealm[IMOS_STRING_LEN_64];

}AUTHORIZATION_INFO_S;

/**
 * @struct tagExDomain
 * @brief ����������Ϣ
 * @attention
 */
typedef struct tagExDomain
{
    /** �������, ����Ψһ��ʶ */
    CHAR    szDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �������� */
    CHAR    szDomainName[IMOS_NAME_LEN];

    /** ��������(�ϼ���/�¼���/ƽ����), ȡֵΪ#EX_DOMAIN_TYPE_E */
    ULONG   ulOuterDomainType;

    /** ����ڵ���롣����޸��ڵ㣬����մ� */
    CHAR    szDomainParentCode[IMOS_DOMAIN_CODE_LEN];

    /** �������ȼ���ȡֵ��ΧΪ1-9��1�����ȼ���ߣ�9�����ȼ���� */
    ULONG   ulPriority;

    /** �������λ��ȡֵΪ#DOMAIN_STREAM_PULL_TYPE  */
    ULONG   ulIsSession;

    /** SIP�˿ڣ�H3C��Ϊ5061���������̰������� */
    ULONG   ulOuterDomainPort;

    /** ����鲥֧�ֲ���. 1Ϊ֧��; 0Ϊ��֧��, �������� */
    ULONG   ulBroadcastStrategy;

    /** �����ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG   uladdrtype;

    /** �����ַ */
    CHAR    szDomainAddr[IMOS_IPADDR_LEN];

     /** �����������, ȡֵ��ΧΪ0~9999 */
    ULONG ulInTrunkNum;

    /** ��������
        ���ڷ�OEM�汾(VM8500��H3C�豸),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_VMISC
            #IMOS_MANUFACTURER_TYPE_DA
            #IMOS_MANUFACTURER_TYPE_OTHER
        ����OEM�汾(PL/PL-S),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_PL
            #IMOS_MANUFACTURER_TYPE_PLS
            #IMOS_MANUFACTURER_TYPE_OTHER */
    CHAR szManufacturerType[IMOS_STRING_LEN_64];

    /** �����е���ȼ�����Ч��ΧΪ1��7 */
    ULONG ulDomainLevel;

    /** ���ͨѶЭ�����ͣ�ȡֵΪ#INTERDOMAIN_PROTOCOL_TYPE_E */
    ULONG ulProtocolType;

    /** ����ע��״̬��ָ������ǰ�������ע���Ƿ�ɹ�, 1Ϊ�ɹ�; 0Ϊʧ�� */
    ULONG                     ulActiveRegSta;

    /** ����ע��״̬��ָ��ǰ�����������ע���Ƿ�ɹ�, 1Ϊ�ɹ�; 0Ϊʧ�� */
    ULONG                     ulRecvRegSta;

    /** ����������룬Ŀǰֻ֧��DB33��׼������룬�����޸� */
    CHAR  szInterDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �������û����룬��Ҫ���������õĻ����û�����һ�£������޸� */
    CHAR  szInterUserCode[IMOS_USER_CODE_LEN];

    /** ����ʹ��MS���ԣ�BOOL_TRUEΪ����Ӧ��BOOL_FALSEΪָ�� */
    BOOL_T bIsAutofit;

    /** ����ָ����MS���� */
    CHAR  szMSCode[IMOS_DEVICE_CODE_LEN];

    /** �����ý���Э������,ȡֵΪ#IMOS_TRANS_TYPE_E��Ŀǰ����ֻ֧������Ӧ��TCP */
    ULONG ulStreamTransProtocol;

    /** ������ֱ��ý��������,ȡֵΪ��1~2048��ȱʡΪ1024 */
    ULONG ulStreamByPassMaxNum;

    /** ���ʵ����������������Ӧƥ�����,ȡֵ�μ�#MON_STREAM_ANY_MATCH_POLICY_E */
    ULONG ulMonStreamMatchPolicy;

    /** �����ֶ� */
    CHAR szReserve[20];
}EX_DOMAIN_S;


/**
 * @struct tagExDomainV2
 * @brief ���������ݽṹ������������ϢV2
 * @attention
 */
typedef struct tagExDomainV2
{
    EX_DOMAIN_S        stExDomainInfo;

    /** ����ע���Ƿ���ҪȨǩ��1��ʾ��Ҫ��0��ʾ�� */
    BOOL_T             bNeedAuthorize;

    /** Ȩǩ��Ϣ */
    AUTHORIZATION_INFO_S    stAuthorization;

    /** DecoderTag��Ϣ */
    CHAR szDecoderTag[IMOS_DECODERTAG_LEN];

    /** ����ģʽ ��ͨ����ģʽ��#CTRL_MODE_NORMAL����ǿ����ģʽ��#CTRL_MODE_FULL�� */
    ULONG ulCtrlMode;

    /** �����URL */
    CHAR  szDomainURL[IMOS_URL_LEN];

}EX_DOMAIN_V2_S;

/**
 * @struct tagExDomainV3
 * @brief ���������ݽṹ������������ϢV3
 * @attention
 */
typedef struct tagExDomainV3
{
    EX_DOMAIN_V2_S  stExDomainInfoV2;

    /** ����������,��ΪDVR:[MM_DOMAIN_SUBTYPE_FOREIGN_DVR3308, MM_DOMAIN_SUBTYPE_FOREIGN_DVR_MAX) */
    ULONG           ulExDomainSubType;

}EX_DOMAIN_V3_S;

/**
 * @struct tagLoDomain
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagLoDomain
{
    /** �������, �����Ψһ��ʶ */
    CHAR  szDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �������� */
    CHAR  szDomainName[IMOS_NAME_LEN];

    /** ��������
        ���ڷ�OEM�汾(VM8500��H3C�豸),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_VMISC
        ����OEM�汾(PL/PL-S),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_PL
            #IMOS_MANUFACTURER_TYPE_PLS */
    CHAR szManufacturerType[IMOS_STRING_LEN_64];

    /** NTP��������ַ����, ֧�ֵ�����ַ��Ϊ#IMOS_NTP_SERVER_MAXNUM */
    CHAR szNTPServerAddr[IMOS_NTP_SERVER_MAXNUM][IMOS_IPADDR_LEN];
}LO_DOMAIN_S;


/**
 * @struct tagOrgResQueryItem
 * @brief ��֯�ڵ�����Դ��Ϣ��(��ѯ��Դ�б�ʱ����)
 * @attention
 */
typedef struct tagOrgResQueryItem
{
    /** ��Դ���� */
    CHAR  szResCode[IMOS_RES_CODE_LEN];

    /** ��Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ��Դ���ͣ�ȡֵ��ΧΪ#IMOS_TYPE_E */
    ULONG  ulResType;

    /** ��Դ������,Ŀǰ��Դ������ֻ�������/��֯/����ʾ/����Ѳ��Ч�������������Ϊ��̨/����̨;
        ����֯����Ϊ:1-��������2-�����������3-�����������. 4-�ϼ���������.
        5-�¼���������.6-ƽ����������.
        ������ʾ��Դ��������ʾ���ͣ�ȡֵΪ#SALVO_TYPE_E
        ������Ѳ��Դ��������Ѳ���ͣ�ȡֵΪ#GROUP_SWITCH_TYPE_E */
    ULONG  ulResSubType;

    /** ��Դ״̬,ȡֵΪ:#IMOS_DEV_STATUS_ONLINE,#IMOS_DEV_STATUS_OFFLINE
        - �������Դ,���������豸(�����/������/EC/DC)������(��������˵,���ֶδ������ע��״̬) */
    ULONG  ulResStatus;

    /** ��Դ����״̬
        - ���������,���ֶ��������߲���һ�������ϱ�����Ƶ״̬��Ϣ��(����Ƶ��ʧ�澯/����Ƶ�ָ��澯),��������,ȡֵΪ:#IMOS_DEV_EXT_STATUS_E
        - ��������,���ֶδ�������ע��״̬,ȡֵΪ:#IMOS_DEV_STATUS_ONLINE,#IMOS_DEV_STATUS_OFFLINE */
    ULONG  ulResExtStatus;

    /** ����Դ�Ƿ��Ǳ��������Դ, 1Ϊ���������Դ; 0Ϊ�ǻ������Դ */
    ULONG  ulResIsBeShare;

    /** ��Դ������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ֧�ֵ�����Ŀ��������Դ����Ϊ�����ʱ��Ч��0:��Чֵ��1:������2:˫�� 3:���� */
    ULONG ulStreamNum;

    /** �Ƿ�Ϊ������Դ��1Ϊ������Դ; 0Ϊ��������Դ */
    ULONG  ulResIsForeign;
}ORG_RES_QUERY_ITEM_S;


/**
 * @struct tagResItemV2
 * @brief ��Դ��Ϣ��(V2,��չ����"��Դ������֯������")
 * @attention
 */
typedef struct tagResItemV2
{
    /** V1��Դ��Ϣ�� */
    ORG_RES_QUERY_ITEM_S stResItemV1;

    /** ��Դ������֯������ */
    CHAR  szOrgName[IMOS_NAME_LEN];

    /** ��Դ������Ϣ��������Դ�����������ʱ��ȡֵΪ#CAMERA_ATTRIBUTE_E��������Դ���͸��ֶ���δʹ�� */
    ULONG  ulResAttribute;

    /** �����ECR HFϵ�е���������߼��������ڵ��豸�����ײͣ�
        ������Դ����,����ͨ�ò�ѯ����IS_QUERY_ENCODESETû����д, ��������"����ѯ", ���ֶξ�Ϊ��Чֵ#IMOS_SR_INVALID
        ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG  ulDevEncodeSet;

    /** �����ֶ� */
    CHAR   szReserve[184];

}RES_ITEM_V2_S;


/**
 * @struct tagQueryConditionItem
 * @brief ��ѯ������
 * @attention
 */
typedef struct tagQueryConditionItem
{
    /** ��ѯ��������: #QUERY_TYPE_E */
    ULONG ulQueryType;

    /** ��ѯ�����߼���ϵ����: #LOGIC_FLAG_E */
    ULONG ulLogicFlag;

    /** ��ѯ���� */
    CHAR  szQueryData[IMOS_QUERY_DATA_MAX_LEN];
}QUERY_CONDITION_ITEM_S;


/**
 * @struct tagCommonQueryCondition
 * @brief ͨ�ò�ѯ����
 * @attention
 */
typedef struct tagCommonQueryCondition
{
    /** ��ѯ���������в�ѯ������ʵ�ʸ���, ���ȡֵΪ#IMOS_QUERY_ITEM_MAX_NUM */
    ULONG ulItemNum;

    /** ��ѯ�������� */
    QUERY_CONDITION_ITEM_S astQueryConditionList[IMOS_QUERY_ITEM_MAX_NUM];

 }COMMON_QUERY_CONDITION_S;



/***************************************************************************************
EC,DC����
***********************************************************************************/


/**
 * @struct tagDevChannelIndex
 * @brief ͨ��������Ϣ
 * @attention
 */
typedef struct tagDevChannelIndex
{
    /** �豸���� */
    CHAR    szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���ͣ���ȡֵΪ������#IMOS_TYPE_EC; ������#IMOS_TYPE_DC; ����洢���ܵı�����#IMOS_TYPE_ECR */
    ULONG ulDevType;

    /** ͨ�������ţ���Ϊ:��Ƶ��Ƶͨ��������ͨ����������ͨ��(����/���), ȡֵ�Ӿ�������� */
    ULONG   ulChannelIndex;
}DEV_CHANNEL_INDEX_S;



/**
 * @struct tagECInfo
 * @brief EC��Ϣ
 * @attention
 */
typedef struct tagECInfo
{
    /** EC����, EC��Ψһ��ʶ */
    CHAR  szECCode[IMOS_DEVICE_CODE_LEN];

    /** EC���� */
    CHAR  szECName[IMOS_NAME_LEN];

    /** EC���ͣ�ȡֵΪ#IMOS_DEVICE_TYPE_E, �Ϸ�ȡֵ�μ�#ulChannum������˵�� */
    ULONG ulECType;

    /** ECͨ������:
        ���ֳ���EC���Ͷ�Ӧ��ͨ����������:
        EC1101(#IMOS_DT_EC1101_HF)/EC1001(#IMOS_DT_EC1001)/EC1001-HF(#IMOS_DT_EC1001_HF): 1
        EC1501(#IMOS_DT_EC1501_HF)/R1000 (#IMOS_DT_R1000)/#IMOS_DT_EC1501_IHF : 1
        EC2004(#IMOS_DT_EC2004_HF)/VR2004(#IMOS_DT_VR2004): 4
        EC1102(#IMOS_DT_EC1102_HF)/VR1102(#IMOS_DT_VR1102): 2
        EC1801(#IMOS_DT_EC1801_HH): 1
        EC2016(#IMOS_DT_EC2016_HC): 16
        EC2016[8CH](#IMOS_DT_EC2016_HC_8CH): 8
        EC2016[4CH](#IMOS_DT_EC2016_HC_4CH): 4
        HIC5201-H(#IMOS_DT_HIC5201)/HIC5221-H(#IMOS_DT_HIC5221): 1
        ECR3316(#IMOS_DT_ECR3316_HC): 16
        ECR3316[8CH](#IMOS_DT_ECR3316_HC_8CH): 8
        ECR3316[4CH](#IMOS_DT_ECR3316_HC_4CH): 4
        ECR3308(#IMOS_DT_ECR3308_HD): 8
        ECR3308[4CH](#IMOS_DT_ECR3308_HD_4CH): 4
        HIC5421-H(#IMOS_DT_HIC5421)/HIC5401-H(#IMOS_DT_HIC5401)/#IMOS_DT_SIC/#IMOS_DT_HIC: 1
        EC1504(#IMOS_DT_EC1504_HF): 4
        EC2516(#IMOS_DT_EC2516_HF): 16
        EC2508(#IMOS_DT_EC2508_HF): 8
    */
    ULONG ulChannum;

    /** �Ƿ�֧���鲥, 1Ϊ֧��; 0Ϊ��֧�� */
    ULONG ulIsMulticast;

    /** ���¸澯�¶�����, ȡֵΪ50~100 */
    LONG lTemperatureMax;

    /** ���¸澯�¶�����, ȡֵΪ-100~49 */
    LONG lTemperatureMin;

    /** �澯ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG ulEnableAlarm;

    /** EC������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ʱ��ͬ����ʽ��Ĭ��Ϊ1����ʾʹ��H3C��˽��ͬ����ʽ��2��ʾNTP��ͬ����ʽ(Ŀǰ�ݲ�֧��) */
    ULONG ulTimeSyncMode;

    /** ʱ��, ȡֵΪ-12~12 */
    LONG lTimeZone;

    /** �������ã������ķ����������ã�ȡֵΪ:#TD_LANGUAGE_E */
    ULONG ulLanguage;

    /** �Ƿ����ñ��ػ��棬1��ʾ����; 0��ʾ��������Ĭ��ֵΪ0 */
    ULONG ulEnableLocalCache;

    /** ���ײ�, ȡֵΪ:#IMOS_STREAM_RELATION_SET_E
        0��MPEG4+MPEG4(#IMOS_SR_MPEG4_MPEG4)
        1��H264������(#IMOS_SR_H264_SHARE)
        2��MPEG2+MPEG4(#IMOS_SR_MPEG2_MPEG4)
        3��H264+MJPEG(#IMOS_SR_H264_MJPEG)
        4��MPEG4������(#IMOS_SR_MPEG4_SHARE)
        5��MPEG2������(#IMOS_SR_MPEG2_SHARE)
        8: MPEG4������_D1(#IMOS_SR_STREAM_MPEG4_8D1)
        9��MPEG2+MPEG2(#IMOS_SR_MPEG2_MPEG2)
        11��H264+H264(#IMOS_SR_H264_H264)
    */
    ULONG ulEncodeSet;

    /** ��ʽ, ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** ��Ƶ����Դ��ȡֵΪ#IMOS_AUDIO_INPUT_SOURCE_E */
    ULONG ulAudioinSource;

    /** �����Խ���Դ���� */
    CHAR szAudioCommCode[IMOS_RES_CODE_LEN];

    /** �����㲥��Դ���� */
    CHAR szAudioBroadcastCode[IMOS_RES_CODE_LEN];

    /** �豸�������� */
    CHAR szDevPasswd[IMOS_PASSWD_ENCRYPT_LEN];

    /** �豸����, Ŀǰ���ֶ�δʹ�� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** EC��IP��ַ, ��Ӽ��޸�EC������д�ò���, ��ѯEC��Ϣʱ���ظ��ֶ� */
    CHAR szECIPAddr[IMOS_IPADDR_LEN];

    /** EC������״̬,��Ӽ��޸�EC������д�ò���, ��ѯEC��Ϣʱ���ظ��ֶ�, 1Ϊ����; ����Ϊ���� */
    ULONG ulIsECOnline;

    /** ��ý���Э������,ȡֵΪ#IMOS_TRANS_TYPE_E��ĿǰECֻ֧��UDP��TCP */
    ULONG ulStreamTransProtocol;

    /** �豸�汾��Ϣ */
    CHAR szECVer[IMOS_STRING_LEN_64];

    /** �����ֶ� */
    CHAR szReserve[28];
}EC_INFO_S;


/**
 * @struct tagECQueryItem
 * @brief ��������Ϣ��(��ѯ�������б�ʱ����)
 * @attention
 */
typedef struct tagECQueryItem
{
    /** EC���� */
    CHAR  szECCode[IMOS_DEVICE_CODE_LEN];

    /** EC���� */
    CHAR  szECName[IMOS_NAME_LEN];

    /** EC���ͣ�ȡֵΪ#IMOS_DEVICE_TYPE_E */
    ULONG ulECType;

    /** �豸��ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG ulDevaddrtype;

    /** �豸��ַ */
    CHAR szDevAddr[IMOS_IPADDR_LEN];

    /** ������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸�Ƿ�����ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ȡֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** �Ƿ�֧���鲥, 1Ϊ֧���鲥; 0Ϊ��֧���鲥 */
    ULONG ulIsMulticast;

    /** �澯ʹ��, 1Ϊʹ�ܸ澯; 0Ϊ��ʹ�ܸ澯 */
    ULONG ulEnableAlarm;

    /** ���ײ����ͣ�ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG ulEncodeType;

    /** ��ʽ��ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** �����ֶ� */
    CHAR szReserve[32];
}EC_QUERY_ITEM_S;


/**
 * @struct tagECAndAudioInfo
 * @brief EC������Ϣ
 * @attention
 */
typedef struct tagECAndAudioInfo
{
    /**  EC������Ϣ */
    EC_INFO_S           stEcInfo;

    /** ������Ƶ��Ϣ */
    AUDIO_INPUT_INFO_S  stAudioInPutInfo;
}EC_AND_AUDIO_INFO_S;


/**
 * @struct tagCamAndChannelQueryItem
 * @brief ������Լ�ͨ����Ϣ��(��ѯ������Լ�ͨ���б�ʱ����)
 * @attention
 */
typedef struct tagCamAndChannelQueryItem
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stECChannelIndex;

    /** ��������� */
    CHAR    szCamCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR    szCamName[IMOS_NAME_LEN];

    /** ���������, ȡֵΪ#CAMERA_TYPE_E */
    ULONG   ulCamType;

    /** ��̨����Э�� */
    CHAR    szPtzProtocol[IMOS_STRING_LEN_32];

    /** ��̨��ַ��, ȡֵΪ0~255, ����ȡֵ����̨�������ʵ�ʵ�ַ����� */
    ULONG   ulPtzAddrCode;

    /** �鲥��ַ */
    CHAR    szMulticastAddr[IMOS_IPADDR_LEN];

    /** �鲥�˿�, ��ΧΪ��10002-65534 */
    ULONG   ulMulticastPort;

    /** �����ֶ� */
    CHAR szReserve[32];
}CAM_AND_CHANNEL_QUERY_ITEM_S;



/**
 * @struct tagDCInfo
 * @brief DC��Ϣ
 * @attention
 */
typedef struct tagDCInfo
{
    /** DC���� */
    CHAR  szDCCode[IMOS_DEVICE_CODE_LEN];

    /** DC���� */
    CHAR  szDCName[IMOS_NAME_LEN];

    /** DC����, ȡֵΪ#IMOS_DEVICE_TYPE_E, �Ϸ�ȡֵ�μ�#ulChannum������˵�� */
    ULONG ulDCType;

    /** DCͨ������:
        ���ֳ���DC���Ͷ�Ӧ��ͨ����������:
        DC1001(#IMOS_DT_DC1001): 1
        DC2004(#IMOS_DT_DC2004_FF)/VL2004(#IMOS_DT_VL2004): 4
        DC1801(#IMOS_DT_DC1801_FH): 1
    */
    ULONG ulChannum;

    /** �Ƿ�֧���鲥, 1Ϊ֧���鲥; 0Ϊ��֧���鲥 */
    ULONG ulIsMulticast;

    /** ���¸澯�¶�����, ȡֵΪ50~100 */
    LONG lTemperatureMax;

    /** ���¸澯�¶�����, ȡֵΪ-100~49 */
    LONG lTemperatureMin;

    /** �澯ʹ��, 1Ϊʹ�ܸ澯; 0Ϊ��ʹ�ܸ澯 */
    ULONG ulEnableAlarm;

    /** ������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ʱ��ͬ����ʽ��Ĭ��Ϊ1����ʾʹ��H3C��˽��ͬ����ʽ��2��ʾNTP��ͬ����ʽ(Ŀǰ�ݲ�֧��) */
    ULONG ulTimeSyncMode;

    /** ʱ��, ȡֵΪ-12~12 */
    LONG lTimeZone;

    /** �������ã������ķ����������ã�ȡֵΪ:#TD_LANGUAGE_E */
    ULONG ulLanguage;

    /** ��ʽ, ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** ���ײͣ�ȡֵΪ#IMOS_STREAM_RELATION_SET_E
        ����Ϊ���������ײ�ֵ��
        1��H264(#IMOS_SR_H264_SHARE)
        3: MJPEG(#IMOS_SR_H264_MJPEG)
        4��MEPG4(#IMOS_SR_MPEG4_SHARE)
        5��MEPG2(#IMOS_SR_MPEG2_SHARE)
    */
    ULONG ulEncodeSet;

    /** �豸�������� */
    CHAR szDevPasswd[IMOS_PASSWD_ENCRYPT_LEN];

    /** �豸����, Ŀǰ���ֶ�δʹ�� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** DC��IP��ַ,��Ӽ��޸�DC������д�ò���,��ѯDC��Ϣʱ�᷵�ظ��ֶ� */
    CHAR szDCIPAddr[IMOS_IPADDR_LEN];

    /** DC������״̬,��Ӽ��޸�DC�ò�����Ч, ��ѯDC��Ϣʱ���ظ��ֶ�, 1Ϊ����; ����Ϊ���� */
    ULONG ulIsDCOnline;

    /** ��ý���Э������,ȡֵΪ#IMOS_TRANS_TYPE_E��ĿǰDCֻ֧������Ӧ��TCP */
    ULONG ulStreamTransProtocol;

    /** ��ý��ķ�ʽ����,ȡֵΪ#IMOS_STREAM_SERVER_MODE_E��ĿǰDCֻ֧������Ӧ��ֱ������ */
    ULONG ulStreamServerMode;

    /** �����ֶ� */
    CHAR szReserve[88];
}DC_INFO_S;


/**
 * @struct tagDCQueryItem
 * @brief ��������Ϣ��(��ѯ�������б�ʱ����)
 * @attention
 */
typedef struct tagDCQueryItem
{
    /** DC���� */
    CHAR  szDCCode[IMOS_DEVICE_CODE_LEN];

    /** DC���� */
    CHAR  szDCName[IMOS_NAME_LEN];

    /** DC���ͣ�ȡֵΪ#IMOS_DEVICE_TYPE_E */
    ULONG ulDCType;

    /** DC�豸��ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG ulDevaddrtype;

    /** DC�豸��ַ */
    CHAR szDevAddr[IMOS_IPADDR_LEN];

    /** DC������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸�Ƿ�����, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ö��ֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** �Ƿ�֧���鲥, 1Ϊ֧���鲥; 0Ϊ��֧���鲥 */
    ULONG ulIsMulticast;

    /** �澯ʹ��, 1Ϊʹ�ܸ澯; 0Ϊ��ʹ�ܸ澯 */
    ULONG ulEnableAlarm;

    /** ���ײ����ͣ�ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG ulEncodeType;

    /** ��ʽ, ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** �����ֶ� */
    CHAR szReserve[32];
}DC_QUERY_ITEM_S;


/**
 * @struct tagScrAndChannelQueryItem
 * @brief �������Լ�ͨ����Ϣ��(��ѯ�������Լ�ͨ���б�ʱ����)
 * @attention
 */
typedef struct tagScrAndChannelQueryItem
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stDCChannelIndex;

    /** ���������� */
    CHAR    szScrCode[IMOS_RES_CODE_LEN];

    /** ���������� */
    CHAR    szScrName[IMOS_NAME_LEN];

    /** �������� */
    ULONG ulSplitScrNum;

    /** �����ֶ� */
    CHAR szReserve[28];
}SCR_AND_CHANNEL_QUERY_ITEM_S;

/***************************************************************************************
���������
***********************************************************************************/

/**
 * @struct tagSCameraInfo
 * @brief �������Ϣ
 * @attention
 */
typedef struct tagSCameraInfo
{
    /** ��������� */
    CHAR    szCameraCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];

    /** ���������, ȡֵΪ#CAMERA_TYPE_E */
    ULONG   ulCameraType;

    /** ���������, �ɲ��� */
    CHAR    szCameraDesc[IMOS_DESC_LEN];

    /** ��̨����Э��, Ŀǰ֧�ֵİ���:PELCO-D, PELCO-P, ALEC, VISCA, ALEC_PELCO-D, ALEC_PELCO-P, MINKING_PELCO-D, MINKING_PELCO-P */
    CHAR    szPtzProtocol[IMOS_STRING_LEN_32];

    /** ��̨��ַ��, ȡֵΪ0~255, ����ȡֵ����̨�������ʵ�ʵ�ַ����� */
    ULONG   ulPtzAddrCode;

    /** ��̨Э�鷭��ģʽ,����ȡֵΪ#PTZ_CMDPARSEMODE_E */
    ULONG   ulPtzTranslateMode;

    /** ���� */
    CHAR szLongitude[IMOS_STRING_LEN_32];

    /** γ�� */
    CHAR szLatitude[IMOS_STRING_LEN_32];

    /** ����λ�����趨��Ԥ��λ�ı�����Ӧ */
    ULONG   ulGuardPosition;

    /** �Զ�����ʱ��, ��λΪ��, ��󲻳���3600��, 0��ʾ������ */
    ULONG   ulAutoGuard;

    /** �豸����, �ɲ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** ����EC���������EC�ı���, ����ECR���������ECR�ı���, ��������ʱ����ı��� */
    CHAR    szECCode[IMOS_DEVICE_CODE_LEN];

    /** �ڰ󶨼��޸�Cameraʱ,������д,��ѯCamera��Ϣʱ�᷵�ظ��ֶ�, ����EC���������EC��IP, ����ECR���������ECR��IP, ���������������IP */
    CHAR    szECIPAddr[IMOS_IPADDR_LEN];

    /** ����ECͨ��������, �Ӿ����������, ����EC��ECR HCϵ�е�������Ǿ����ͨ������, ����ECR HFϵ�к�������������Чֵ */
    ULONG   ulChannelIndex;

    /* ����Ŵ�Э�� ȡֵ#ZOOM_PROTOCOL_E*/
    ULONG   ulZoomProtocol;

    /** ���Ŀ������, ȡֵΪTARGET_TYPE_E�е�һ��������ɵ��ַ��� */
    CHAR    szTargetType[IMOS_STRING_LEN_14];

    /** ���Ŀ�������ͣ�Ŀǰ�����������պ���չ */
    CHAR    szTargetSubType[IMOS_STRING_LEN_14];
}CAMERA_INFO_S;



/***************************************************************************************
����������
***********************************************************************************/

/**
 * @struct tagScreenInfo
 * @brief ��������Ϣ
 * @attention
 */
typedef struct tagScreenInfo
{
    /** ���������� */
    CHAR        szScreenCode[IMOS_RES_CODE_LEN];

    /** ���������� */
    CHAR        szScreenName[IMOS_NAME_LEN];

    /** ����������, �ɲ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /**  DC��IP��ַ, �ڰ󶨼��޸�Screenʱ, ������д; ��ѯScreen��Ϣʱ�᷵�ظ��ֶ� */
    CHAR    szDCIPAddr[IMOS_IPADDR_LEN];

    /** �����ֶ� */
    CHAR szReserve[32];
}SCREEN_INFO_S;

/**
 * @struct tagScreenRelatedDC
 * @brief ����������Ӧ�Ľ�������Ϣ�ṹ��
 * @attention
 */
typedef struct tagScreenRelatedDC
{
    /** ���������� */
    CHAR szScreenCode[IMOS_CODE_LEN];

    /** ���������� */
    CHAR szScreenName[IMOS_NAME_LEN];

    /** ����������, �ɲ��� */
    CHAR szDevDesc[IMOS_DESC_LEN];

    /** DC���� */
    CHAR szDCCode[IMOS_CODE_LEN];

    /** DC���� */
    CHAR szDCName[IMOS_NAME_LEN];

    /** DC����, ȡֵΪ#IMOS_DEVICE_TYPE_E */
    ULONG ulDCType;

    /** ����ӦDC��IP��ַ, �ڰ󶨼��޸�Screenʱ, ������д; ��ѯScreen��Ϣʱ�᷵�ظ��ֶ� */
    CHAR szDCIPAddr[IMOS_IPADDR_LEN];

    /** �����ֶ� */
    CHAR szReserve[32];
}SCREEN_RELATED_DC_S;

/***************************************************************************
EC,DC��Ƶͨ��
****************************************************************************/

/**
 * @struct tagVideoInChannel
 * @brief ����Ƶ����ͨ����Ϣ
 * @attention
 */
typedef struct tagVideoInChannel
{

    /** ��Ƶ����ͨ������ */
    CHAR szVinChannelDesc[IMOS_DESC_LEN];

    /** �鲥��ַ */
    CHAR szMulticastAddr[IMOS_IPADDR_LEN];

    /** �鲥�˿�,��ΧΪ��10002-65534���ұ���Ϊż�� */
    ULONG   ulMulticastPort;

    /** MSѡ�����Ӧ����, 1Ϊ����Ӧ; 0Ϊָ��MS��2Ϊ������MS, ȡֵΪ#IMOS_STREAM_SERVER_MODE_E */
    ULONG ulIsAutofit;

    /** ʹ��MS��Ŀ, ��ʵ���������, ����Ӧ����#ulIsAutofitΪ����Ӧ, ��ֵΪ0;
        ����Ӧ����#ulIsAutofitΪ������Ӧ(��ָ��), ��ֵΪ1 */
    ULONG ulUseMSNum;

    /** MS�����б� */
    CHAR aszMSCode[IMOS_MS_MAX_NUM_PER_CHANNEL][IMOS_DEVICE_CODE_LEN];

    /** �Ƿ�����ͼ���ڵ����澯, 1Ϊ����; 0Ϊ������ */
    ULONG ulEnableKeepout;

    /** �Ƿ������˶����澯, 1Ϊ�����˶����澯; 0Ϊ�������˶����澯 */
    ULONG ulEnableMotionDetect;

    /** �Ƿ�������Ƶ��ʧ�澯, 1Ϊ������Ƶ��ʧ�澯; 0Ϊ��������Ƶ��ʧ�澯 */
    ULONG ulEnableVideoLost;

    /** �󶨵Ĵ��ڱ�ţ���������дΪ��Чֵ(255)#PTZ_INVALID_SERIAL_INDEX_NUM */
    ULONG ulSerialIndex;

    /** ���ȣ�ȡֵΪ��0~255�� */
    ULONG ulBrightness;

    /** �Աȶȣ�ȡֵΪ��0~255�� */
    ULONG ulContrast;

    /** ���Ͷȣ�ȡֵΪ��0~255�� */
    ULONG ulSaturation;

    /** ɫ����ȡֵΪ��0~255�� */
    ULONG ulTone;

    /** �Ƿ���������, 1Ϊ����������; 0Ϊ�������� */
    ULONG ulAudioEnabled;

    /** ��Ƶ����, ȡֵΪ#IMOS_AUDIO_FORMAT_E */
    ULONG ulAudioCoding;

    /** ��Ƶ����, ȡֵΪ#IMOS_AUDIO_CHANNEL_TYPE_E */
    ULONG ulAudioTrack;

    /** ��Ƶ������, ȡֵΪ#IMOS_AUDIO_SAMPLING_E */
    ULONG ulSamplingRate;

    /** ��Ƶ����, �������� */
    ULONG ulAudioCodeRate;

    /** ��Ƶ����ֵ��ȡֵΪ��0~255�� */
    ULONG ulIncrement;
}VIN_CHANNEL_S;


/**
 * @struct tagVideoOutChannel
 * @brief �߼����ͨ����Ϣ
 * @attention
 */
typedef struct tagVideoOutChannel
{
    /** �߼����ͨ������, ȡֵΪ1~(#SPLIT_SCR_MODE_MAX - 1) */
    ULONG   ulVoutChannelindex;

    /** �߼����ͨ������, �ɲ��� */
    CHAR szVoutChannelDesc[IMOS_DESC_LEN];

    /** �Ƿ�ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG ulEnable;

    /** ��������, ȡֵΪ#IMOS_STREAM_TYPE_E, Ŀǰ��֧��#IMOS_ST_TS */
    ULONG ulStreamType;

    /** ������ģʽ, ȡֵΪ#IMOS_TRANS_TYPE_E */
    ULONG   ulTranType;

    /** �Ƿ�����������, 1Ϊ����; 0Ϊ������ */
    ULONG   ulEnableJitterBuff;

}VOUT_CHANNEL_S;


/**
 * @struct tagPhyOutChannel
 * @brief �������ͨ����Ϣ
 * @attention
 */
typedef struct tagPhyOutChannel
{

    /** ����ģʽ��ȡֵΪ1��4������BNC�ڵķ����� */
    ULONG ulPhyoutMode;

    /** ��Ƶ�����ʽ��ȡֵΪ#IMOS_VIDEO_FORMAT_E */
    ULONG ulDecodeFormat;

    /** ��Ƶ��ʽ��ȡֵΪ#IMOS_AUDIO_FORMAT_E */
    ULONG ulAudioFormat;

    /** �������ã�ȡֵΪ#IMOS_AUDIO_CHANNEL_TYPE_E */
    ULONG ulAudioTrack;

    /** �Ƿ�������������, 1Ϊ����; 0Ϊ������ */
    ULONG ulAudioEnabled;

    /** �������, ȡֵΪ1~7 */
    ULONG ulVolume;

    /** ��Ƶ���ѡ��, �ӹ���ģʽ����#ulPhyoutMode�������������ģʽȡֵΪ1, ���ֵΪ1; �������ģʽȡֵΪ4, ���ֵȡֵΪ1~4 */
    ULONG ulOutputIndex ;

    /** ������������, ��ʾ������ͨ�����ɰ󶨵ļ���������, Ŀǰ�̶�Ϊ1 */
    ULONG ulMaxScreenNum;
}PHYOUT_CHANNEL_S;



/**
 * @struct tagVideoStream
 * @brief ��Ƶ����Ϣ
 * @attention
 */
typedef struct tagVideoStream
{

    /** ��������, ȡֵΪ#IMOS_STREAM_TYPE_E, Ŀǰ��֧��#IMOS_ST_TS */
    ULONG       ulStreamType;

    /** ��������1Ϊ������2Ϊ������3Ϊ�������� */
    ULONG       ulStreamIndex;

    /** ��ʹ�ܱ�ʶ, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

    /** �����䷽ʽ, ȡֵΪ#IMOS_TRANS_TYPE_E */
    ULONG       ulTranType;

    /** �����ʽ, ȡ���ھ�������ײ�ֵ, ȡֵΪ#IMOS_VIDEO_FORMAT_E */
    ULONG       ulEncodeFormat;

    /** �ֱ���, ȡֵΪ#IMOS_PICTURE_SIZE_E */
    ULONG       ulResolution;

    /** ���� */
    ULONG       ulBitRate;

    /** ֡��,��ȡ��ֵ��1, 3, 5, 8, 10, 15, 20, 25, 30, 60
        ������ʽ#IMOS_PF_720P60HZ,Ĭ��ֵΪ60 */
    ULONG       ulFrameRate;

    /** GOPģʽ, ȡֵΪ#IMOS_GOP_TYPE_E */
    ULONG       ulGopMode;

    /** I֡���, ȡ����GOPģʽֵ, ��GOPģʽΪ#IMOS_GT_Iʱ, I֡���Ϊ1; ��GOPģʽΪ#IMOS_GT_IPʱ, I֡���Ϊ10~60
        ������ʽ#IMOS_PF_720P60HZ,Ĭ��ֵΪ60 */
    ULONG       ulIFrameInterval;

    /** ͼ������, ȡֵΪ#IMOS_VIDEO_QUALITY_E */
    ULONG       ulImageQuality;

    /** ������ģʽ, ȡֵΪ#IMOS_ENC_MODE_E */
    ULONG       ulEncodeMode;

    /** ���ȼ�, ���ڱ���ģʽΪ#IMOS_EM_CBRʱ�����ø�ֵ, ȡֵΪ#IMOS_CBR_ENC_MODE_E */
    ULONG       ulPriority;

    /** ����ƽ����ȡֵΪ#IMOS_STREAM_SMOOTH_E */
    ULONG       ulSmoothValue;

}VIDEO_STREAM_S;

/**
 * @struct tagAreaScope
 * @brief ����Χ��������������(�����Ǿ��ε�)
 * @attention ԭ��Ϊ���úͽ�����Ҫͳһ�������ģ��
 */
typedef struct tagAreaScope
{
    /** ���Ͻ�x���� */
    ULONG       ulTopLeftX;

    /** ���Ͻ�y���� */
    ULONG       ulTopLeftY;

    /** ���½�x���� */
    ULONG       ulBottomRightX;

    /** ���½�y���� */
    ULONG       ulBottomRightY;

}AREA_SCOPE_S;

/**
 * @struct tagVideoArea
 * @brief ��Ƶ������Ϣ
 * @attention
 */
typedef struct tagVideoArea
{
    /** ��������, ȡֵΪ1~4 */
    ULONG       ulAreaIndex;

    /** �Ƿ�ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

    /** ������, 1��5����1����������ߡ���ֵ�����˶����������Ч */
    ULONG       ulSensitivity;

    /** �������� */
    AREA_SCOPE_S stAreaScope;

}VIDEO_AREA_S;



/**
 * @struct tagDetectArea
 * @brief �ڵ����˶��������
 * @attention
 */
typedef struct tagDetectArea
{
    /** �ڵ�������� */
    VIDEO_AREA_S       astCoverDetecArea[IMOS_DETECT_AREA_MAXNUM];

    /** �˶�������� */
    VIDEO_AREA_S       astMotionDetecArea[IMOS_DETECT_AREA_MAXNUM];
}DETECT_AREA_S;

/**
 * @struct tagOsdMaskArea
 * @brief �ڸ�����
 * @attention
 */
typedef struct tagOsdMaskArea
{
    /** �ڸ����� */
    VIDEO_AREA_S       astMaskArea[IMOS_MASK_AREA_MAXNUM];
}OSD_MASK_AREA_S;




/**
 * @struct tagOsdTime
 * @brief OSDʱ�����
 * @attention
 */
typedef struct tagOsdTime
{
    /** ʱ��OSD����, �̶�Ϊ1 */
    ULONG       ulOsdTimeIndex;

    /** ʱ��OSDʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnableFlag;

    /** ʱ��OSDʱ���ʽ */
    ULONG       ulOsdTimeFormat;

    /** ʱ��OSD���ڸ�ʽ */
    ULONG       ulOsdDateFormat;

    /** ʱ��OSD��ɫ, ȡֵΪ#IMOS_OSD_COLOR_E */
    ULONG       ulOsdColor;

    /** ʱ��OSD͸����, ȡֵΪ#IMOS_OSD_ALPHA_E */
    ULONG       ulTransparence;

    /** ʱ��OSD�������� */
    AREA_SCOPE_S stAreaScope;

}OSD_TIME_S;

/**
 * @struct tagOsdName
 * @brief OSD������Ϣ
 * @attention
 */
typedef struct tagOsdName
{
    /** �Ƿ�ʹ�ܳ���OSD, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

    /** ����OSD����, �̶�Ϊ1 */
    ULONG       ulOsdNameIndex;

    /** ����OSD��ɫ, ȡֵΪ#IMOS_OSD_COLOR_E */
    ULONG       ulOsdColor;

    /** ����OSD͸����, ȡֵΪ#IMOS_OSD_ALPHA_E */
    ULONG       ulTransparence;

    /** ����OSD�������� */
    AREA_SCOPE_S stAreaScope;

    /** ��һ��(��)����OSD����, ȡֵΪ#IMOS_INFO_OSD_TYPE_E */
    ULONG       ulOsdType1;

    /** ��һ��(��)����OSD���ݣ������֣���ֵΪ�ַ������Ϊ20�ַ�����ͼƬ����ֵΪOSDͼƬ���� */
    CHAR        szOsdString1[IMOS_NAME_LEN];

    /** �ڶ���(��)����OSD����, ȡֵΪ#IMOS_INFO_OSD_TYPE_E */
    ULONG       ulOsdType2;

    /** �ڶ���(��)����OSD���ݣ������֣���ֵΪ�ַ������Ϊ20�ַ�����ͼƬ����ֵΪOSDͼƬ���� */
    CHAR        szOsdString2[IMOS_NAME_LEN];

    /** (��һ���͵ڶ���)����OSD֮����л�ʱ��, ��λΪ��, ȡֵΪ0~300��ȡֵΪ0, ��ʾֻ��ʾ��һ��(��)OSD */
    ULONG       ulSwitchIntval;

    /** ����OSD����,ȡֵΪ:0 ��ͨ����OSD,1 ��̨�����û�OSD */
    ULONG ulOsdNameType;

    /** �����ֶ� */
    CHAR szReserve[12];
}OSD_NAME_S;


/**
 * @struct tagOsdInfo
 * @brief OSD��Ϣ
 * @attention
 */
typedef struct tagOsdInfo
{
    /** ʱ��OSD */
    OSD_TIME_S      stOSDTime;

    /** ����OSD */
    OSD_NAME_S      astOSDName[IMOS_OSD_NAME_MAXNUM];

    /** �ڸ����� */
    OSD_MASK_AREA_S stOSDMaskArea;
}OSD_INFO_S;



/**
 * @struct tagVinChnlBindCamera
 * @brief ����������ͨ������Ϣ
 * @attention
 */
typedef struct tagVinChnlBindCamera
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stECChannelIndex;

    /** �������Ϣ */
    CAMERA_INFO_S       stCameraInfo;

    /** ��Ƶ����ͨ����Ϣ */
    VIN_CHANNEL_S       stVinChannel;

    /** OSD��Ϣ */
    OSD_INFO_S          stOSDInfo;

    /** ��Ƶ����������Ƶ����ʵ����Ŀ, ���ȡֵΪ#IMOS_STREAM_MAXNUM, �Ӿ������ײ�ֵ���� */
    ULONG               ulVideoStreamNum;

    /** ��Ƶ����Ϣ���� */
    VIDEO_STREAM_S      astVideoStream[IMOS_STREAM_MAXNUM];

    /** ������򣬰����˶�����Լ��ڵ�������� */
    DETECT_AREA_S       stDetectArea;
}VINCHNL_BIND_CAMERA_S;

/**
 * @struct tagMultiOsdInfo
 * @brief OSD��Ϣ(֧�ֶ���OSD)
 * @attention
 */
typedef struct tagMultiOsdInfo
{
    /** ʱ��OSD */
    OSD_TIME_S      stOSDTime;

    /** ����OSD */
    OSD_NAME_S      astOSDName[IMOS_OSD_NAME_MAX];

    /** �ڸ����� */
    OSD_MASK_AREA_S stOSDMaskArea;
}MULTI_OSD_INFO_S;

/**
* @struct tagOpticsParam
* @brief ��ѧ����
* @attention
*/
typedef struct tagOpticsParam
{
    /** ��ѧͨ����Ŀǰֻ��һ��ͨ����Զ��1 */
    ULONG ulOpticsIndex;

    /** �ڰ�ģʽ��ȡֵΪ#IMOS_BLACKWHIT_MODE_E 0 �Զ�ģʽ��ȱʡ�� */
    ULONG ulBlackWhiteMode;

    /** �ڰ�ģʽ�����ȣ�ֻ�кڰ�ģʽ���Զ�ģʽ��������Ч 0(���) - 2(���) Ĭ��1 */
    ULONG ulBlackWhiteSense;

    /** �Զ��ع⣬2���ر��Զ��ع� 1��ʹ���Զ��ع⣨ȱʡ�� */
    ULONG ulAutoExposal;

    /** ���ţ�ȡֵΪ#IMOS_SHUTTER_E��ȱʡΪ50 */
    ULONG ulShutter;

    /** �Զ�������ƣ���Χ0-36��ȱʡ0  */
    ULONG ulOpticsAGC;

    /** �����ƣ�ȡֵΪ#IMOS_AEMETERING_E 0 ����Ȩ�أ�ȱʡ�� */
    ULONG ulVideoAEMetering;

    /** ��ȣ�ȡֵΪ# ��Χ0-6  Ĭ��0 */
    ULONG ulVideoSharpness;

    /** ��ƽ�⣬ȡֵΪ#IMOS_AWB_E 0 �Զ�ģʽ��ȱʡ�� */
    ULONG ulWhiteBalance;

    /** ͼ����ȡֵΪ#IMOS_MIRRORTYPE_E 0 ������ȱʡ�� */
    ULONG ulVideoMirror;

    /** ����Ƶ�ʣ�ȡֵΪ#IMOS_FLICKER_MODE_E 0 �رգ�ȱʡ�� */
    ULONG ulVideoFlicker;

    /** ��̬(#BOOL_FALSE �ر�, #BOOL_TRUE ����) */
    BOOL_T bEnableWDR;

    /** ���ⲹ��(#BOOL_FALSE �ر�, #BOOL_TRUE ����) */
    BOOL_T bEnableBLC;

    /** �����ֶ�3�� ȱʡΪ0 */
    ULONG ulReserved3;
} OPTICS_PARAM_S;

/**
 * @struct tagPTZParam
 * @brief ��̨����
 * @attention
*/
typedef struct tagPTZParam
{
    /** ���������ٶ�(��/��),ȡֵ��Χ[30,360] */
    ULONG ulXMaxAngleSpeed;

    /** ���������ٶ�(��/��),ȡֵ��Χ[30,360] */
    ULONG ulYMaxAngleSpeed;

    /** ��������ӽ�(��),ȡֵ��Χ[30,360] */
    ULONG ulXMaxViewAngle;

    /** ��������ӽ�(��),ȡֵ��Χ[30,360] */
    ULONG ulYMaxViewAngle;

    /** �䱶ʱ��(����),ȡֵ��Χ[100,60000] */
    ULONG ulZoomTime;

    /** �䱶����,ȡֵ��Χ[1,35] */
    ULONG ulZoomMultiple;
} PTZ_PARAMETER_S;

/**
 * @struct tagReviseParam
 * @brief �㷨У������
 * @attention
*/
typedef struct tagReviseParam
{
    /** ����ת������ϵ��(%),ȡֵ��Χ[50,200] */
    ULONG ulXTurnCoefficient;

    /** ����ת������ϵ��(%),ȡֵ��Χ[50,200] */
    ULONG ulYTurnCoefficient;

    /** �佹����ϵ��(%),ȡֵ��Χ[50,200] */
    ULONG ulFocusCoefficient;
} REVISE_PARAMETER_S;

/**
 * @struct tagLocateInfo
 * @brief ��λ��Ϣ
 * @attention
*/
typedef struct tagLocateInfo
{
    /** �Ƿ�����(#BOOL_FALSE ������,#BOOL_TURE ����) */
    BOOL_T  bIsEnabled;

    /** ��̨���� */
    PTZ_PARAMETER_S stPTZParam;

    /** �㷨У������ */
    REVISE_PARAMETER_S  stReviseParam;
} LOCATE_INFO_S;

/**
 * @struct tagIMOSAnalyseSizeFilter
 * @brief ��Ϊ���������С������Ϣ
 * @attention
 */
typedef struct tagIMOSAnalyseSizeFilter
{
    /** �Ƿ�ʹ��(BOOL_FALSE ��,BOOL_TRUE ��) */
    BOOL_T bEnable;

    /** ��С����,��ʽΪ"x1,y1;x2,y2" */
    CHAR szAreaMin[IMOS_COORDINATE_STR_LEN_16];

    /** �������,��ʽΪ"x1,y1;x2,y2" */
    CHAR szAreaMax[IMOS_COORDINATE_STR_LEN_16];

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_ANALYSE_SIZE_FILTER_S;

/**
 * @struct tagIMOSBehaviorThruLine
 * @brief "������Ϊ"��Ϣ
 * @attention
 */
typedef struct tagIMOSBehaviorThruLine
{
    /** ��,��ʽΪ"x1,y1;x2,y2" */
    CHAR szLine[IMOS_COORDINATE_STR_LEN_16];

    /** ����,ȡֵΪ#IMOS_THRU_LINE_DIRECTION_E */
    ULONG ulDirection;

    /** �߶� */
    ULONG ulHeight;

    /** ��ɫ */
    ULONG ulColor;

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_BEHAVIOR_THRU_LINE_S;

/**
 * @struct tagIMOSBehaviorArea
 * @brief "������Ϊ"��Ϣ
 * @attention
 */
typedef struct tagIMOSBehaviorArea
{
    /** �������ζ�������,ȡֵΪ[3-6] */
    ULONG ulPointNum;

    /** ��������:�ɶ���������ɵ��ַ���,��ʽΪ"x1,y1;x2,y2;......" */
    CHAR szAreaPolygon[IMOS_COORDINATE_STR_LEN_56];

    /** ����,ȡֵΪ#IMOS_AREA_DIRECTION_E */
    ULONG ulDirection;

    /** ��ɫ */
    ULONG ulColor;

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_BEHAVIOR_AREA_S;

/**
 * @struct tagIMOSBehavior
 * @brief ��Ϊ��Ϣ
 * @attention
 */
typedef union tagIMOSBehavior
{
    /** ������Ϊ */
    IMOS_BEHAVIOR_THRU_LINE_S stThruLine;

    /** ������Ϊ */
    IMOS_BEHAVIOR_AREA_S stArea;
} IMOS_BEHAVIOR_U;

/**
 * @struct tagIMOSBehaviorAnalyseRule
 * @brief ��Ϊ��������
 * @attention
 */
typedef struct tagIMOSBehaviorAnalyseRule
{
    /** ����,ȡֵΪ[1-8] */
    ULONG  ulIndex;

    /** �Ƿ�ʹ��(BOOL_FALSE ��,BOOL_TRUE ��) */
    BOOL_T bEnable;

    /** ���������С������Ϣ */
    IMOS_ANALYSE_SIZE_FILTER_S  stFilter;

    /** ��Ϊ����,ȡֵΪ#IMOS_BEHAVIOR_TYPE_E */
    ULONG ulBehaviorType;

    /** ��Ϊ��Ϣ */
    IMOS_BEHAVIOR_U unBehavior;

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_BEHAVIOR_ANALYSE_RULE_S;

/**
 * @struct tagIMOSBehaviorAnalyseArithmetic
 * @brief ��Ϊ�����㷨��Ϣ
 * @attention
 */
typedef struct tagIMOSBehaviorAnalyseArithmetic
{
    /** ֡��,ȡֵΪ[1-60] */
    ULONG  ulFrameRate;

    /** �Ƿ�ȥ��Ӱ(BOOL_FALSE ��,BOOL_TRUE ��) */
    BOOL_T bShadow;

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_BEHAVIOR_ANALYSE_ARITHMETIC_S;

/**
 * @struct tagIMOSBehaviorAnalyseInfo
 * @brief ��Ϊ������Ϣ
 * @attention
 */
typedef struct tagIMOSBehaviorAnalyseInfo
{
    /** �Ƿ�ʹ��(BOOL_FALSE ��,BOOL_TRUE ��) */
    BOOL_T bEnable;

    /** �㷨 */
    IMOS_BEHAVIOR_ANALYSE_ARITHMETIC_S stArithmetic;

    /** ���� */
    IMOS_BEHAVIOR_ANALYSE_RULE_S astRule[IMOS_BEHAVIOR_RULE_MAX];

    /** �����ֶ� */
    CHAR szReserve[128];
} IMOS_BEHAVIOR_ANALYSE_INFO_S;

/**
 * @struct tagVinChnlBindCameraBase
 * @brief ����������ͨ���Ļ�����Ϣ
 * @attention
 */
typedef struct tagVinChnlBindCameraBase
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stECChannelIndex;

    /** �������Ϣ */
    CAMERA_INFO_S       stCameraInfo;

    /** ��Ƶ����ͨ����Ϣ */
    VIN_CHANNEL_S       stVinChannel;

    /** ʱ��OSD */
    OSD_TIME_S      stOSDTime;

    /** ��ѧ���ò��� */
    OPTICS_PARAM_S      stOpticsParam;

    /** ��λ��Ϣ */
    LOCATE_INFO_S stLocateInfo;
}VINCHNL_BIND_CAMERA_BASE_S;

/**
 * @struct tagVinChnlBindCameraElement
 * @brief ��Ƶͨ��ȫ���䳤Ԫ�ؽṹ��
 * @attention
 */
typedef struct tagVinChnlFullVarietyElement
{
    /** ����OSD���� */
    ULONG           ulOSDNameNum;

    /** ����OSD */
    OSD_NAME_S      *pstOSDName;

    /** �ڸ�������� */
    ULONG           ulMaskAreaNum;

    /** �ڸ����� */
    VIDEO_AREA_S     *pstMaskArea;

    /** ��Ƶ������ */
    ULONG               ulVideoStreamNum;

    /** ��Ƶ����Ϣ���� */
    VIDEO_STREAM_S      *pstVideoStream;

    /** �ڵ����������� */
    ULONG           ulCoverDetecAreaNum;

    /** �ڵ�������� */
    VIDEO_AREA_S       *pstCoverDetecArea;

    /** �˶����������� */
    ULONG           ulMotionDetecAreaNum;

    /** �˶�������� */
    VIDEO_AREA_S       *pstMotionDetecArea;
}VINCHNL_FULL_VARIETY_ELEMENT_S;

/**
 * @struct tagVinChnlBindCameraV2
 * @brief ����������ͨ������Ϣ(֧�ֶ���OSD����ѧ��������)
 * @attention
 */
typedef struct tagVinChnlBindCameraV2
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stECChannelIndex;

    /** �������Ϣ */
    CAMERA_INFO_S       stCameraInfo;

    /** ��Ƶ����ͨ����Ϣ */
    VIN_CHANNEL_S       stVinChannel;

    /** OSD��Ϣ */
    MULTI_OSD_INFO_S    stOSDInfo;

    /** ��Ƶ����������Ƶ����ʵ����Ŀ, ���ȡֵΪ#IMOS_STREAM_MAXNUM_II, �Ӿ������ײ�ֵ���� */
    ULONG               ulVideoStreamNum;

    /** ��Ƶ����Ϣ���� */
    VIDEO_STREAM_S      astVideoStream[IMOS_STREAM_MAXNUM_II];

    /** ������򣬰����˶�����Լ��ڵ�������� */
    DETECT_AREA_S       stDetectArea;

    /** ��ѧ���ò��� */
    OPTICS_PARAM_S      stOpticsParam;
}VINCHNL_BIND_CAMERA_V2_S;

/**
 * @struct tagVinChnlBindCameraForECR
 * @brief ����������ͨ������Ϣfor ECR&NVR
 * @attention
 */
typedef struct tagVinChnlBindCameraForECR
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stECChannelIndex;

    /** �������Ϣ */
    CAMERA_INFO_S       stCameraInfo;

    /** ��Ƶ����ͨ����Ϣ */
    VIN_CHANNEL_S       stVinChannel;

    /** OSD��Ϣ */
    MULTI_OSD_INFO_S    stOSDInfo;

    /** ��Ƶ����������Ƶ����ʵ����Ŀ, ���ȡֵΪ#IMOS_STREAM_MAXNUM, �Ӿ������ײ�ֵ���� */
    ULONG               ulVideoStreamNum;

    /** ��Ƶ����Ϣ���� */
    VIDEO_STREAM_S      astVideoStream[IMOS_STREAM_MAXNUM];

    /** ������򣬰����˶�����Լ��ڵ�������� */
    DETECT_AREA_S       stDetectArea;

    /** ��ѧ���ò��� */
    OPTICS_PARAM_S      stOpticsParam;
}VINCHNL_BIND_CAMERA_FOR_ECR_S;

/**
 * @struct tagSemaOutputInfoSdkWithTime
 * @brief �����������Ϣ(  ��ʱ��)
 * @attention
 */
typedef struct tagSemaOutputInfoSdkWithTimeForECR
{
    /** ������������� */
    CHAR        szSemaDesc[IMOS_DESC_LEN];

     /** ����������澯������ʽ, ȡֵ1Ϊ����; 2Ϊ���� */
    ULONG       ulTriggerMode;

    /** ����������澯����ʱ�䣬��λΪ�� */
    ULONG       ulOutPutTime;

} SEMA_OUTPUT_INFO_WITHTIME_FOR_ECR_S;

/**
 * @struct tagVoutChnlBindScreen
 * @brief �����������ͨ������Ϣ
 * @attention
 */
typedef struct tagVoutChnlBindScreen
{
    /** �豸ͨ��������Ϣ */
    DEV_CHANNEL_INDEX_S stDCChannelIndex;

    /** ��������Ϣ */
    SCREEN_INFO_S       stScreenInfo;

    /** �߼����ͨ����Ϣ */
    VOUT_CHANNEL_S      stVoutChannel;

    /** OSD��Ϣ */
    OSD_INFO_S          stOSDInfo;

    /** �������ͨ����Ϣ */
    PHYOUT_CHANNEL_S    stPhyoutChannel;
}VOUTCHNL_BIND_SCREEN_S;


/***************************************************************************************
���ڹ���
***********************************************************************************/

/**
 * @struct tagSerialInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagSerialInfo
{
    /** ����ģʽ, ȡֵΪ#IMOS_SERIAL_MODE_E */
    ULONG       ulSerialMode;

    /** ��������, ȡֵΪ#SERIAL_TYPE_E */
    ULONG       ulSerialType;

    /** ������, ȡֵΪ
        #IMOS_BUADRATE_1200
        #IMOS_BUADRATE_2400
        #IMOS_BUADRATE_4800
        #IMOS_BUADRATE_9600
        #IMOS_BUADRATE_19200
        #IMOS_BUADRATE_38400
        #IMOS_BUADRATE_57600
        #IMOS_BUADRATE_115200
    */
    ULONG       ulBaudRate;

    /** ����λ, ȡֵΪ5~8 */
    ULONG       ulDataBit;

    /** ֹͣλ, ȡֵΪ1~2 */
    ULONG       ulStopBit;

    /** У��λ, ȡֵΪ
        #IMOS_PARITY_NULL
        #IMOS_PARITY_ODD
        #IMOS_PARITY_EVEN
    */
    ULONG       ulParityBit;

    /** ����, 0Ϊ������; 1Ϊ�������; 2ΪӲ������ */
    ULONG       ulFlowControl;

} SERIAL_INFO_S;


/**
 * @struct tagSerialQueryItem
 * @brief ���ڲ�ѯ��Ϣ�б�Ԫ��Ϣ
 * @attention
 */
typedef struct tagSerialQueryItem
{
    /** �������� */
    ULONG   ulSerialIndex;

    /** �������� */
    ULONG   ulSerialType;

    /* ����ģʽ */
    ULONG   ulSerialMode;
} SERIAL_QUERY_ITEM_S;


/**
 * @struct tagSerialRelationItem
 * @brief ���������Դ��Ϣ��(��ѯ���������Դ�б�ʱ����)
 * @attention
 */
typedef struct tagSerialRelationItem
{
    /** ��Դ���� */
    CHAR    szResCode[IMOS_RES_CODE_LEN];

    /** ��Դ���� */
    CHAR    szResName[IMOS_NAME_LEN];

    /** ��Դ����, ȡֵΪ#IMOS_TYPE_E*/
    ULONG ulResType;

}SERIAL_RELATION_ITEM_S;


/***************************************************************************************
����������
***********************************************************************************/
/**
 * @struct tagSemaInputInfo
 * @brief ���뿪������Ϣ
 * @attention
 */
typedef struct tagSemaInputInfo
{
    /** ���뿪��������, �ɲ��� */
    CHAR        szSemaDesc[IMOS_DESC_LEN];

    /** ���뿪�����澯������ʽ, ȡֵ1Ϊ����; 2Ϊ���� */
    ULONG       ulTriggerMode;

    /** �Ƿ�ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

    /** ��·���ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnableDetectFlag;
} SEMA_INPUT_INFO_S;


/**
 * @struct tagSemaOutputInfoSdk
 * @brief �����������Ϣ
 * @attention
 */
typedef struct tagSemaOutputInfoSdk
{
    /** ������������� */
    CHAR        szSemaDesc[IMOS_DESC_LEN];

     /** ����������澯������ʽ, ȡֵ1Ϊ����; 2Ϊ���� */
    ULONG       ulTriggerMode;

} SEMA_OUTPUT_INFO_S;

/**
 * @struct tagSemaOutputInfoV2
 * @brief �����������ϢV2
 * @attention
 */
typedef struct tagSemaOutputInfoV2
{
    /** �����������Ϣ */
    SEMA_OUTPUT_INFO_S  stBaseOutputInfo;

    /** ����������澯����ʱ�䣬��λΪ�� */
    ULONG               ulOutPutTime;

    /**���� */
    CHAR                szRetention[128];
}SEMA_OUTPUT_INFO_V2_S;


/**
 * @struct tagSemaQueryItem
 * @brief ��������ѯ��Ϣ��(��ѯ�������б�ʱ����)
 * @attention
 */
typedef struct tagSemaQueryItem
{
    /** ����������, Ϊ�������ı�� */
    ULONG       ulSemaIndex;

    /** ����������, ȡֵ0Ϊ����; 1Ϊ��� */
    ULONG       ulSemaType;

    /** ���뿪�����澯������ʽ, ȡֵ1Ϊ����; 2Ϊ���� */
    ULONG       ulTriggerMode;

    /** �Ƿ�ʹ�ܣ�1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

} SEMA_QUERY_ITEM_S;

/**
 * @struct tagSemaQueryItemV2
 * @brief ��������ѯ��Ϣ��V2(��ѯ�������б�ʱ����)
 * @attention
 */
typedef struct tagSemaQueryItemV2
{
    /** ��������ѯ��Ϣ�� */
    SEMA_QUERY_ITEM_S stSemaQueryItemInfo;

    /** �����������豸���� */
    CHAR    szDevCode[IMOS_CODE_LEN];

    /** ���������� */
    CHAR    szSemaDesc[IMOS_DESC_LEN];

    /** �澯Դ���߸澯����Ƿ�ʹ��, 1Ϊʹ��, 0Ϊ��ʹ�� */
    ULONG   ulAlarmIOEnableFlag;

    /** �澯Դ���߸澯�������, �澯Դ���߸澯�����ʹ��ʱ��Ч */
    CHAR    szAlarmIOCode[IMOS_CODE_LEN];

    /** �澯Դ���߸澯�������, �澯Դ���߸澯�����ʹ��ʱ��Ч */
    CHAR    szAlarmIOName[IMOS_NAME_LEN];

    /** ���뿪��������·���ʹ��, 1Ϊʹ��; 0Ϊ��ʹ��. ��Ϊ���뿪����ʱ��Ч */
    ULONG   ulEnableDetectFlag;

    /** �澯����򿪵�ʱ��. ���澯���ʹ��ʱ��Ч */
    ULONG   ulOutputTime;

    /** �����ֶ� */
    CHAR    szReserve[64];
} SEMA_QUERY_ITEM_V2_S;


/************************************************************************************************************
OSD
************************************************************************************************************/

/**
 * @struct tagOsdImageListItem
 * @brief OSDͼƬ��Ϣ
 * @attention
 */
typedef struct tagOsdImageListItem
{
    /** OSDͼƬ���� */
    CHAR szOsdImageCode[IMOS_RES_CODE_LEN];

    /** OSDͼƬ�� */
    CHAR    szName[IMOS_NAME_LEN];
}OSD_IMAGE_LIST_ITEM_S;



/*******************************************************************************
ģ�����
********************************************************************************/
/**
 * @struct tagVideoInChannelParam
 * @brief ��Ƶ�������ģ��
 * @attention
 */
typedef struct tagVideoInChannelParam
{
    /** �鲥��ַ */
    CHAR szMulticastAddr[IMOS_IPADDR_LEN];

    /** �鲥�˿�, ȡֵΪ10002-65534, �ұ���Ϊż�� */
    ULONG ulMulticastPort;

    /** MSѡ�����Ӧ����, 1Ϊ����Ӧ; 0Ϊָ��MS��2Ϊ������MS, ȡֵΪ#IMOS_STREAM_SERVER_MODE_E */
    ULONG ulIsAutofit;

    /** �Ƿ�����ͼ���ڵ����澯, 1Ϊ����; 0Ϊ������ */
    ULONG ulEnableKeepout;

    /** �Ƿ������˶����澯, 1Ϊ�����˶����澯; 0Ϊ�������˶����澯 */
    ULONG ulEnableMotionDetect;

    /** �Ƿ�������Ƶ��ʧ�澯, 1Ϊ������Ƶ��ʧ�澯; 0Ϊ��������Ƶ��ʧ�澯 */
    ULONG ulEnableVideoLost;

    /** �󶨵Ĵ��ڱ��, ��������д0 */
    ULONG ulSerialIndex;

    /** ����, ȡֵΪ��0~255�� */
    ULONG ulBrightness;

    /** �Աȶ�, ȡֵΪ��0~255�� */
    ULONG ulContrast;

    /** ���Ͷ�, ȡֵΪ��0~255�� */
    ULONG ulSaturation;

    /** ɫ��, ȡֵΪ��0~255�� */
    ULONG ulTone;

    /** �Ƿ���������, 1Ϊ��������; 0Ϊ���������� */
    ULONG ulIsMute;

    /** ��Ƶ����, ȡֵΪ#IMOS_AUDIO_FORMAT_E */
    ULONG ulAudioCoding;

    /** ��Ƶ����, ȡֵΪ#IMOS_AUDIO_CHANNEL_TYPE_E */
    ULONG ulAudioTrack;

    /** ��Ƶ������, ȡֵΪ#IMOS_AUDIO_SAMPLING_E */
    ULONG ulSamplingRate;

    /** ��Ƶ����, �������� */
    ULONG ulAudioCodeRate;

    /** ��Ƶ����ֵ, ȡֵΪ(0-255) */
    ULONG ulIncrement;
}VIN_CHANNEL_PARAM_S;

/**
 * @struct tagVideoStreamParam
 * @brief ��Ƶ����������ģ��
 * @attention
 */
typedef struct tagVideoStreamParam
{
    /** �������� */
    ULONG       ulStreamType;

    /** ������ */
    ULONG       ulStreamIndex;

    /** ��ʹ�ܱ�ʶ */
    ULONG       ulEnabledFlag;

    /** �����䷽ʽ */
    ULONG       ulTranType;

    /** �����ʽ,ö��ֵΪIMOS_VIDEO_FORMAT_E */
    ULONG       ulEncodeFormat;

    /** �ֱ��� */
    ULONG       ulResolution;

    /** ���� */
    ULONG       ulBitRate;

    /** ֡�� */
    ULONG       ulFrameRate;

    /** GOPģʽ */
    ULONG       ulGopMode;

    /** I֡��� */
    ULONG       ulIFrameInterval;

    /** ͼ������ */
    ULONG       ulImageQuality;

    /** ������ģʽ CBR VBR */
    ULONG       ulEncodeMode;

    /** ���ȼ������ڱ���ģʽΪCBRʱ�����á�0-�����Է�ʽ��1-�����Է�ʽ */
    ULONG       ulPriority;

    /** ����ƽ����ö��ֵΪIMOS_STREAM_SMOOTH_E��Ŀǰֻ֧��0��5������ */
    ULONG       ulSmoothValue;

}VIDEO_STREAM_PARAM_S;

/**
 * @struct tagVideoAreaParam
 * @brief ��Ƶ�����������
 * @attention
 */
typedef struct tagVideoAreaParam
{
    /** �������� */
    ULONG       ulAreaIndex;

    /** �Ƿ�ʹ�� */
    ULONG       ulEnabledFlag;

    /** ������ */
    ULONG       ulSensitivity;

    /** �������� */
    AREA_SCOPE_S stAreaScope;

    /** ��Ƶ��������: �ڵ����ƶ����ڸǣ��μ�ö��ֵ:VIDEO_AREA_TYPE_E */
    ULONG       ulAreaType;

}VIDEO_AREA_PARAM_S;

/**
 * @struct tagVinChannelTemplet
 * @brief ��Ƶ����ͨ��ģ��
 * @attention
 */
typedef struct tagVinChannelTemplet
{
    /** ��Ƶ�������ģ�� */
    VIN_CHANNEL_PARAM_S stVinChannel;

    /** �豸�ͺš����豸������ص�ģ��, ��Ҫ�õ����ֶ� */
    ULONG ulDevType;

    /** dsp imageѡ��ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG ulEncodeSet;

    /** ��ʽ ��ȡֵΪIMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** ��Ƶ����������ģ�� */
    VIDEO_STREAM_PARAM_S astVideoStream[IMOS_STREAM_MAXNUM_II];
}VIN_CHANNEL_TEMPLET_S;


/**
 * @struct tagChannelTempletBasic
 * @brief ͨ��ģ�������Ϣ
 * @attention
 */
typedef struct tagChannelTempletBasic
{
    /** ģ������ */
    CHAR szTempletName[IMOS_NAME_LEN];

    /** ģ��������ʱ�� */
    CHAR  szLastUpdateTime[IMOS_TIME_LEN];
}CHANNEL_TEMPLET_BASIC_S;


/**
 * @struct tagChannelTempQueryItem
 * @brief ͨ��ģ����Ϣ��(��ѯͨ��ģ���б�ʱ����)
 * @attention
 */
typedef struct tagChannelTempQueryItem
{
    /** ģ����� */
    CHAR        szTempletCode[IMOS_RES_CODE_LEN];

    /** ģ������ */
    CHAR        szTempletName[IMOS_NAME_LEN];

    /** ģ��������ʱ�� */
    CHAR        szLastUpdateTime[IMOS_TIME_LEN];

    /** �豸�ͺ�, ȡֵΪ#IMOS_DEVICE_TYPE_E */
    ULONG       ulDevType;

    /** ���ײ�, ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG       ulEncodeSet;

    /** ��ʽ, ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG       ulStandard;
}CHANNEL_TEMP_QUERY_ITEM_S;


/**
 * @struct tagECChannelTemplet
 * @brief ECͨ��ģ��
 * @attention
 */
typedef struct tagECChannelTemplet
{
    /** ģ�������Ϣ */
    CHANNEL_TEMPLET_BASIC_S     stTempBasicInfo;

    /** ��Ƶ�������ģ�� */
    VIN_CHANNEL_TEMPLET_S       stVinChannelTemp;
}EC_CHANNEL_TEMPLET_S;



/**
 * @struct tagVideoOutChannelParam
 * @brief ��Ƶ���ͨ����Ϣ
 * @attention
 */
typedef struct tagVideoOutChannelParam
{
    /** ��������, ȡֵΪ#IMOS_STREAM_TYPE_E, Ŀǰ��֧��#IMOS_ST_TS */
    ULONG ulStreamType;

    /** �Ƿ�����������, 1Ϊ����; 0Ϊ������ */
    ULONG ulEnableJitterBuff;
}VOUT_CHANNEL_PARAM_S;


/**
 * @struct tagPhyOutChannelParam
 * @brief ��Ƶ�������
 * @attention
 */
typedef struct tagPhyOutChannelParam
{
    /** ����ģʽ��ȡֵΪ1��4������BNC�ڵķ����� */
    ULONG ulPhyoutMode;

    /** �����ʽ��ȡֵΪ#IMOS_VIDEO_FORMAT_E */
    ULONG ulDecodeFormat;

    /** ��Ƶ��ʽ��ȡֵΪ#IMOS_AUDIO_FORMAT_E */
    ULONG ulAudioFormat;

    /** �������ã�ȡֵΪ#IMOS_AUDIO_CHANNEL_TYPE_E */
    ULONG ulAudioTrack;

    /** �Ƿ�������������, 1Ϊ����; 0Ϊ������ */
    ULONG ulAudioEnabled;

    /** �������, ȡֵΪ1~7 */
    ULONG ulVolume;

    /** ��Ƶ���ѡ��, �ӹ���ģʽ����#ulPhyoutMode�������������ģʽȡֵΪ1, ���ֵΪ1; �������ģʽȡֵΪ4, ���ֵȡֵΪ1~4 */
    ULONG ulOutputIndex ;
}PHYOUT_CHANNEL_PARAM_S;


/**
 * @struct tagVoutChannelTemplet
 * @brief ��Ƶ���ͨ��ģ��
 * @attention
 */
typedef struct tagVoutChannelTemplet
{

    /** ��Ƶ���ͨ��ģ�� */
    VOUT_CHANNEL_PARAM_S    stVoutChannel;

    /** �豸�ͺţ�ȡֵΪ#IMOS_DEVICE_TYPE_E */
    ULONG ulDevType;

    /** ���ײ�ѡ��ȡֵΪ#IMOS_STREAM_RELATION_SET_E, Ŀǰ��Ƶ���ͨ����֧��
        #IMOS_SR_H264_SHARE��
        #IMOS_SR_MPEG4_SHARE
        #IMOS_SR_MPEG2_SHARE
        #IMOS_SR_H264_MJPEG
    */
    ULONG ulEncodeSet;

    /** ��ʽ��ȡֵΪ#IMOS_PICTURE_FORMAT_E */
    ULONG ulStandard;

    /** ��Ƶ�������ͨ��ģ�� */
    PHYOUT_CHANNEL_PARAM_S stPhyVoutChannel;
}VOUT_CHANNEL_TEMPLET_S;


/**
 * @struct tagDCChannelTemplet
 * @brief DCͨ��ģ��
 * @attention
 */
typedef struct tagDCChannelTemplet
{
    /** ģ�������Ϣ */
    CHANNEL_TEMPLET_BASIC_S     stTempBasicInfo;

    /** ��Ƶ�������ģ�� */
    VOUT_CHANNEL_TEMPLET_S      stVoutChannelTemp;

}DC_CHANNEL_TEMPLET_S;


/**
 * @struct tagPlanTemplet
 * @brief ʱ��ƻ�ģ��
 * @attention
 */
typedef struct tagPlanTemplet
{
    /** ģ�������Ϣ */
    CHANNEL_TEMPLET_BASIC_S     stTempBasicInfo;

    /** ��ѭ��������ѭ��, ȡֵΪ:MM_JOBSCHDTYPE_E */
    ULONG                       ulCycleType;

    /** ʱ��ƻ��ṹ�� */
    PLAN_TIME_S                 stPlanTimeInfo;
}PLAN_TEMPLET_S;

/*------------------------------------------------------------------------------------------*/

/**
 * @struct tagSnmpV1Special
 * @brief SNMP_V1�����Բ���
 * @attention
 */
typedef struct tagSnmpV1Special
{
    /** ��ȡ������ */
    CHAR  szRoCommunity[IMOS_STRING_LEN_64];

    /** д�������� */
    CHAR  szRwCommunity[IMOS_STRING_LEN_64];
}SNMPV1_SPECIAL_S;

/**
 * @struct tagSnmpV3Special
 * @brief SNMP_V3�����Բ���
 * @attention
 */
typedef struct tagSnmpV3Special
{
    /** ��ȫ���� */
    ULONG ulSecurityMode;

    /** ��֤���� */
    ULONG ulAuthScheme;

    /** ��֤���� */
    CHAR  szAuthPassword[IMOS_STRING_LEN_64];

    /** �������� */
    ULONG ulPrivScheme;

    /** �������� */
    CHAR  szPrivPassword[IMOS_STRING_LEN_64];

}SNMPV3_SPECIAL_S;

/**
 * @struct tagSnmpSpecial
 * @brief SNMP������ģ��
 * @attention
 */
typedef struct tagSnmpSpecial
{
    /** SNMP_V1�����Բ��� */
    SNMPV1_SPECIAL_S stSnmpV1Special;

    /** SNMP_V3�����Բ��� */
    SNMPV3_SPECIAL_S stSnmpV3Special;

}SNMP_SPECIAL_S;

/**
 * @struct tagSnmpTemp
 * @brief SNMPģ��
 * @attention
 */
typedef struct tagSnmpTemp
{
    /** Э��˿ں� */
    ULONG ulSnmpPort;

    /** �����ϻ�ʱ�� */
    ULONG ulTimeout;

    /** �������Դ��� */
    ULONG ulRetries;

    /** ���Ķ�Ӧ�汾�� */
    ULONG ulSnmp_version;

    /** ���������� */
    CHAR  szContextName[IMOS_STRING_LEN_64];

    /** ��ȫģ������ */
    CHAR  szSecurityName[IMOS_STRING_LEN_64];

    /** SNMP������ģ�� */
    SNMP_SPECIAL_S              stSnmpSpecial;

    /** ģ�������Ϣ */
    CHANNEL_TEMPLET_BASIC_S     stTempBasicInfo;

}SNMP_TEMP_S;


/*******************************************************************************
DM
********************************************************************************/

/**
* @struct tagDMInfo
* @brief DM��Ϣ�ṹ��
* @attention ��
*/
typedef struct tagDMInfo
{
    /** DM����, DM��Ψһ��ʶ */
    CHAR  szDMCode[IMOS_DEVICE_CODE_LEN];

    /** DM���� */
    CHAR  szDMName[IMOS_NAME_LEN];

    /** DM���ͣ�ĿǰֵΪ0 */
    ULONG ulDMType;

    /** DM������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** DM����, �ɲ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** �����ֶ� */
    CHAR szReserve[32];
}DM_INFO_S;


/**
* @struct tagDMQueryItem
* @brief DM��Ϣ��(��ѯDM�б�ʱ����)
* @attention ��
*/
typedef struct tagDMQueryItem
{
    /** DM����, DM��Ψһ��ʶ */
    CHAR  szDMCode[IMOS_DEVICE_CODE_LEN];

    /** DM���� */
    CHAR  szDMName[IMOS_NAME_LEN];

    /** DM����, Ŀǰ�޾��庬��, ֵΪ0 */
    ULONG ulDMType;

    /** DM������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** DM������֯���� */
    CHAR  szOrgName[IMOS_NAME_LEN];

    /** �豸��ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG ulDevaddrtype;

    /**�豸��ַ */
    CHAR szDevAddr[IMOS_IPADDR_LEN];

    /** �豸�Ƿ�����, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ö��ֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** �����ֶ� */
    CHAR szReserve[32];
}DM_QUERY_ITEM_S;


/***************************************************************************
�豸-�洢�豸
****************************************************************************/


/**
 * @struct tagStoreDevInfo
 * @brief �洢�豸(IPSAN)��Ϣ
 * @attention
 */
typedef struct tagStoreDevInfo
{
    /** �洢�豸���� */
    CHAR  szStoreDevCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸���� */
    CHAR  szStoreDevName[IMOS_NAME_LEN];

    /** IPSAN �洢�豸���� ��1-VX 2-EX */
    ULONG ulStoreDevType;

    /** �洢�豸��ַ */
    CHAR    szDevAddr[IMOS_IPADDR_LEN];

    /** �洢�豸����DM���� */
    CHAR  szDMCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸��¼�û��� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �洢�豸��¼�û����� */
    CHAR    szPassword[IMOS_PASSWD_LEN];

    /** �洢�豸����, �ɲ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** �����ֶ� */
    CHAR szReserve[64];
}STORE_DEV_INFO_S;


/**
 * @struct tagStoreDevQueryItem
 * @brief �洢�豸��Ϣ��(��ѯ�洢�豸�б�ʱ����)
 * @attention
 */
typedef struct tagStoreDevQueryItem
{
    /** �洢�豸���� */
    CHAR    szStoreDevCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸���� */
    CHAR    szStoreDevName[IMOS_NAME_LEN];

    /** IPSAN �洢�豸���ͣ�1-VX; 2-EX */
    ULONG   ulStoreDevType;

    /** ����DM���� */
    CHAR    szDMCode[IMOS_DEVICE_CODE_LEN];

    /** ����DM���� */
    CHAR    szDMName[IMOS_NAME_LEN];

    /** �豸��ַ���ͣ�1-IPv4; 2-IPv6 */
    ULONG   ulDevaddrtype;

    /** �豸��ַ */
    CHAR    szDevAddr[IMOS_IPADDR_LEN];

    /** �洢�豸������,��λ:M(��)  */
    ULONG   ulStoreDevTotalSize;

    /** �洢�豸��ʹ������,��λ:M(��)  */
    ULONG   ulStoreDevUsedSize;

    /** �洢����״̬, �μ�mp_ism_pub.h�е�ö��"�豸�洢����״̬",
        0-�豸�޷����ʣ�1-�豸�洢������2-�豸˥��(������Դ�޷����ʣ�������Դ�洢����) */
    ULONG   ulStoreRunStatus;

    /** �豸�Ƿ�����, ȡֵ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ȡֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** �����ֶ� */
    CHAR szReserve[64];
}STORE_DEV_QUERY_ITEM_S;



/**
 * @struct tagStoreDevSimpleQueryItem
 * @brief �洢�豸����Ϣ��(��ѯ�洢�豸����Ϣʱ����)
 * @attention
 */
typedef struct tagStoreDevSimpleQueryItem
{
    /** �洢�豸���� */
    CHAR    szStoreDevCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸���ͣ�ȡֵΪ#STORE_DEV_TYPE_E */
    ULONG       ulStoreDevType;

    /** �洢�豸���� */
    CHAR        szStoreDevName[IMOS_NAME_LEN];

    /** �洢�豸������,��λ:M(��) */
    ULONG       ulStoreDevTotalSize;

    /** �洢�豸������������,��λ:M(��) */
    ULONG       ulStoreDevUsedSize;

}STORE_DEV_SIMPLE_QUERY_ITEM_S;

/***************************************************************************
�豸-VX500����
****************************************************************************/

/**
 * @struct tagVX500Info
 * @brief VX500�豸��Ϣ
 * @attention
 */
typedef struct tagVX500Info
{
    /** VX500�豸����, VX500��Ψһ��ʶ */
    CHAR  szVX500Code[IMOS_DEVICE_CODE_LEN];

    /** VX500�豸���� */
    CHAR  szVX500Name[IMOS_NAME_LEN];

    /** VX500�豸����, �Ǳ�����, ĿǰĬ��Ϊ0 */
    ULONG ulVX500Type;

    /** VX500������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸����, ���Բ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

    /** �����ֶ� */
    CHAR szReserve[32];
}VX500_INFO_S;


/**
 * @struct tagVX500QueryItem
 * @brief VX500��Ϣ��(��ѯVX500�б�ʱ����)
 * @attention
 */
typedef struct tagVX500QueryItem
{
    /** VX500�豸���� */
    CHAR    szVX500Code[IMOS_DEVICE_CODE_LEN];

    /** VX500�豸���� */
    CHAR    szVX500Name[IMOS_NAME_LEN];

    /** VX500�豸���ͣ��Ǳ����ĿǰĬ����дΪ0 */
    ULONG   ulVX500Type;

    /** VX500������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

     /** VX500������֯���� */
    CHAR  szOrgName[IMOS_NAME_LEN];

    /** �豸��ַ���ͣ�1-IPv4; 2-IPv6 */
    ULONG   ulDevaddrtype;

    /** �豸��ַ */
    CHAR    szDevAddr[IMOS_IPADDR_LEN];

    /** VX500�豸������,��λ:M(��) */
    ULONG   ulVX500DevTotalSize;

    /** VX500�豸��ʹ������,��λ:M(��) */
    ULONG   ulVX500DevUsedSize;

    /** �洢����״̬, �μ�mp_ism_pub.h�е�ö��"�豸�洢����״̬",
        0-�豸�޷����ʣ�1-�豸�洢������2-�豸˥��(������Դ�޷����ʣ�������Դ�洢����) */
    ULONG   ulStoreRunStatus;


    /** �豸�Ƿ�����, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ȡֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** �����ֶ� */
    CHAR szReserve[32];
}VX500_QUERY_ITEM_S;


/**
 * @struct tagArrayInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagArrayInfo
{
    /** �������� */
    CHAR       szArrayName[IMOS_NAME_LEN];

    /** ��λ��������ʵ�ʵĲ�λ����, ���ֵΪ#IMOS_ARRAY_SLOT_MAXNUM */
    ULONG       ulNumber;

    /** ��λ������ */
    ULONG       aulSlotNum[IMOS_ARRAY_SLOT_MAXNUM];

    /** �������ͣ�0-JBOD 1-RAID0 2-RAID1 3-RAID5 4-RAID10 */
    ULONG       ulArrayType;
} ARRAY_INFO_S;


/**
 * @struct tagArrayStatusInfo
 * @brief ����״̬��Ϣ
 * @attention
 */
typedef struct tagArrayStatusInfo
{
    /** ����״̬,0-���� 1-�ؽ� 2-˥�� 3-�޷�ʹ�� 4-��ʼ�� */
    ULONG   ulArrayStatus;

    /** ����������,��λ:M(��) */
    ULONG   ulArrayTotleSize;

    /** ��������,��λ:M(��) */
    ULONG   ulArrayUsedSize;

    /** �Ƿ��Ѿ����⻯ */
    BOOL_T  bisVirtualized;

    /** ������Ϣ */
    ARRAY_INFO_S    stArrayInfo;

    /** �ؽ����� */
    ULONG   ulRebuildProgress;
} ARRAY_STATUS_INFO_S;


/**
 * @struct tagSlotInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagSlotInfo
{
    /** ��λ�� */
    ULONG       ulSlotNum;

    /** �Ƿ��Ѿ���Ӳ�� */
    ULONG       ulHasDisk;

    /** ��������,��λ:M(��)  */
    ULONG       ulUsedSize;

    /** ����������,��λ:M(��)  */
    ULONG       ulDiskCapacity;

    /** ����״̬��ö��ֵΪ#SS_DISK_STATE_E */
    ULONG       ulStatus;

    /** ���̳��� */
    CHAR        szManufacturer[IMOS_NAME_LEN];
} SLOT_INFO_S;


/*******************************************************************************
MS
********************************************************************************/

/**
 * @struct tagMSInfo
 * @brief MS�豸��Ϣ
 * @attention
 */
typedef struct tagMSInfo
{
    /** MS�豸����, MS��Ψһ��ʶ */
    CHAR  szMSCode[IMOS_DEVICE_CODE_LEN];

    /** MS�豸���� */
    CHAR  szMSName[IMOS_NAME_LEN];

    /** MS������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸����, �ɲ��� */
    CHAR    szDevDesc[IMOS_DESC_LEN];

}MS_INFO_S;


/**
 * @struct tagMSQueryItem
 * @brief MS��Ϣ��(��ѯMS�б�ʱ����)
 * @attention
 */
typedef struct tagMSQueryItem
{
    /** MS���� */
    CHAR    szMSCode[IMOS_DEVICE_CODE_LEN];

    /** MS���� */
    CHAR    szMSName[IMOS_NAME_LEN];

    /** MS������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

     /** MS������֯���� */
    CHAR  szOrgName[IMOS_NAME_LEN];

    /** MS�豸��ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG   ulDevaddrtype;

    /** �豸��ַ */
    CHAR    szDevAddr[IMOS_IPADDR_LEN];

    /** �豸�Ƿ�����, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ȡֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

}MS_QUERY_ITEM_S;


/***************************************************************************
���������
****************************************************************************/

/**
 * @struct tagShareCameraBase
 * @brief ���������������Ϣ
 * @attention
 */
typedef struct tagShareCameraBase
{
    /** ��������� */
    CHAR   szCamName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR   szCamCode[IMOS_RES_CODE_LEN];

    /** ������������� */
    CHAR   szCamShareCode[IMOS_RES_CODE_LEN];

    /** ����������Ȩ�� */
    ORG_AUTHORITY_S             stAuthority;
 }SHARE_CAMERA_BASE_S;

/**
 * @struct tagShareCameraBaseV2
 * @brief ���������������ϢV2
 * @attention
 */
typedef struct tagShareCameraBaseV2
{
    SHARE_CAMERA_BASE_S   stShareCamera;

    /** ����֯�ڵ���� */
    CHAR szParentOrgCode[IMOS_DOMAIN_CODE_LEN];
 }SHARE_CAMERA_BASE_V2_S;

/**
* @struct tagShareCamInfo
* @brief �����������Ϣ��(��ѯ����������б�ʱ����)
*/
typedef struct tagShareCamInfo
{

    /** ���������������Ϣ */
    SHARE_CAMERA_BASE_S stShareCamBaseInfo;

    /** ���������״̬,ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE */
    ULONG                   ulDevStatus;

    /** �Ƿ�Ϊ�����豸��1Ϊ�����豸��0Ϊ�Ǳ����豸 */
    BOOL_T                  bIsLocalDev;

    /** ���������������룬�����Ϊ��������ʱʹ�� */
    CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** ���������EC���� */
    CHAR                    szECCode[IMOS_DEVICE_CODE_LEN];
}SHARE_CAM_INFO_S;
/**
* @struct tagShareCamInfoV2
* @brief �����������Ԫ��ϢV2
*/
typedef struct tagShareCamInfoV2
{
    /** ���������������Ϣ */
    SHARE_CAMERA_BASE_V2_S     stShareCamBaseInfo;

    /** �����״̬,ȡֵΪIMOS_DEV_STATUS_ONLINE/IMOS_DEV_STATUS_OFFLINE */
    ULONG                   ulDevStatus;

    /** �Ƿ�Ϊ�����豸��1��ʾ�ǣ�0��ʾ�� */
    BOOL_T                  bIsLocalDev;

    /** �Ƿ��ѹ��������1��ʾ�ǣ�0��ʾ�� */
    BOOL_T                  bIsShared;

    /** ���������������룬�����Ϊ��������ʱʹ�� */
    CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** ���������EC���� */
    CHAR                    szECCode[IMOS_DEVICE_CODE_LEN];
}SHARE_CAM_INFO_V2_S;


/**
* @struct tagShareCamList
* @brief ����������б���Ϣ
*/
typedef struct tagShareCamList
{
    /** �������������� */
    CHAR                    szDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** ��������������� */
    CHAR                    szDomainName[IMOS_NAME_LEN];

    /** �������ͣ�ȡֵΪ AS_DEVSHARE_OPERATION_TYPE_E */
    ULONG                   ulOperType;

    /** �������͵��豸��Ŀ */
    ULONG                   ulDevNum;

    /** �����������Ϣ�б� */
    SHARE_CAM_INFO_S    astCamInfo[IMOS_ONCE_SHARE_RES_MAXNUM];
}SHARE_CAM_LIST_S;


/***************************************************************************
������֯
****************************************************************************/

/**
 * @struct tagShareOrgBase
 * @brief ������֯������Ϣ
 * @attention
 */
typedef struct tagShareOrgBase
{
     /** ��֯�ڵ���� */
    CHAR szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��֯�ڵ����� */
    CHAR szOrgName[IMOS_NAME_LEN];

    /** ����֯�ڵ���� */
    CHAR szParentOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��֯���������ʱ�Ķ������ */
    CHAR szOrgShareCode[IMOS_RES_CODE_LEN];

 }SHARE_ORG_BASE_S;


/**
* @struct tagShareOrgInfo
* @brief ������֯��Ԫ��Ϣ
*/
typedef struct tagShareOrgInfo
{
    /** ��֯����������룬��֯Ϊ��������ʱʹ�� */
    CHAR                szExDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �Ƿ��ѹ��������1��ʾ�ǣ�0��ʾ�� */
    BOOL_T              bIsShared;

    /** �Ƿ��Զ����������#BOOL_TRUE��ʾ�Զ���������򣬷�֮ȡֵΪ#BOOL_FALSE */
    BOOL_T bIsAutoShare;

    /** ������֯������Ϣ */
    SHARE_ORG_BASE_S    stShareOrgBaseInfo;

}SHARE_ORG_INFO_S;

/***************************************************************************
����澯Դ
****************************************************************************/

/**
 * @struct tagShareAlarmSrcBase
 * @brief ����澯Դ������Ϣ
 * @attention
 */
typedef struct tagShareAlarmSrcBase
{
    /** �澯Դ���� */
    CHAR        szAlarmSrcCode[IMOS_RES_CODE_LEN];

    /** �澯Դ������ */
    CHAR        szAlarmSrcName[IMOS_NAME_LEN];

    /** ����֯�ڵ���� */
    CHAR szParentOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �澯Դ������� */
    CHAR   szAlarmSrcShareCode[IMOS_RES_CODE_LEN];

 }SHARE_ALARMSRC_BASE_S;


/**
* @struct tagShareAlarmSrcInfo
* @brief ����澯Դ��Ԫ��Ϣ
*/
typedef struct tagShareAlarmSrcInfo
{
    /** �澯Դ����������룬�澯ԴΪ��������ʱʹ�� */
    CHAR                szExDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �Ƿ��ѹ��������1��ʾ�ǣ�0��ʾ�� */
    BOOL_T              bIsShared;

    /** ����澯Դ������Ϣ */
    SHARE_ALARMSRC_BASE_S    stShareAlmSrcBaseInfo;

}SHARE_ALARMSRC_INFO_S;

/***************************************************************************
�����ͼ
****************************************************************************/

/**
 * @struct tagShareMapBase
 * @brief �����ͼ������Ϣ
 * @attention
 */
typedef struct tagShareMapBase
{
     /** ��ͼ���� */
    CHAR szMapCode[IMOS_CODE_LEN];

    /** ��ͼ���� */
    CHAR szMapName[IMOS_NAME_LEN];

    /** ����֯�ڵ���� */
    CHAR szParentOrgCode[IMOS_CODE_LEN];

    /** ��ͼ���������ʱ�Ķ������ */
    CHAR szMapShareCode[IMOS_CODE_LEN];

 }SHARE_MAP_BASE_S;


/**
* @struct tagShareMapInfo
* @brief �����ͼ��Ϣ
*/
typedef struct tagShareMapInfo
{
    /** ��ͼ����������룬��ͼΪ��������ʱʹ�� */
    CHAR                szExDomainCode[IMOS_CODE_LEN];

    /** �Ƿ��ѹ��������1��ʾ�ǣ�0��ʾ�� */
    BOOL_T              bIsShared;

    /** �����ͼ������Ϣ */
    SHARE_MAP_BASE_S    stShareMapBaseInfo;

}SHARE_MAP_INFO_S;

/***************************************************************************
Ԥ��λ
****************************************************************************/

/**
 * @struct tagPresetInfo
 * @brief Ԥ��λ��Ϣ
 * @attention
 */
typedef struct tagPresetInfo
{
    /** Ԥ��λֵ, ȡֵ��ΧΪ#PTZ_PRESET_MINVALUE~�����������ļ������õ�Ԥ��λ���ֵ */
    ULONG       ulPresetValue;

    /** Ԥ��λ����, ��Ҫ��д */
    CHAR        szPresetDesc[IMOS_DESC_LEN];
 }PRESET_INFO_S;


/***************************************************************************
Ѳ��·��
****************************************************************************/

/**
 * @struct tagCruisePathItem
 * @brief Ѳ��·���е�Ԥ��λ��Ϣ
 * @attention
 */
typedef struct tagCruisePathItem
{
    /** ��ţ���ʶ��Ԥ��λ��Ѳ��·���е�ִ��˳�� */
    ULONG       ulSeqID;

    /** �����Ԥ��λֵ����1��2�ȣ�ȡֵ��ΧΪ#PTZ_PRESET_MINVALUE~�����������ļ������õ�Ԥ��λ���ֵ
        ��չ֧�ֹ켣Ѳ��
            901��909,�����˶�,�ٶ�Ϊ1��9
            911��919,�����˶�,�ٶ�Ϊ1��9
            921��929,�����˶�,�ٶ�Ϊ1��9
            931��939,�����˶�,�ٶ�Ϊ1��9 */
    ULONG       ulPresetValue;

    /** Ԥ��λ���� */
    CHAR        szPresetDesc[IMOS_DESC_LEN];

    /** Ԥ��λͣ��ʱ��, ���ֵΪ64 */
    ULONG       ulPreSetStopTime;
}CRUISE_PATH_ITEM_S;

/**
 * @struct tagCruisePathInfo
 * @brief Ѳ��·����Ϣ
 * @attention
 */
typedef struct tagCruisePathInfo
{
    /** ��������� */
    CHAR                        szCamName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR                        szCamCode[IMOS_RES_CODE_LEN];

    /** Ѳ��·������ */
    CHAR                        szCruisePathName[IMOS_NAME_LEN];

    /** Ѳ��·��Ԥ��λ������Ԥ��λ��ʵ����Ŀ, ȡֵ������#IMOS_CRUISE_PATH_PRESET_MAX_COUNT */
    ULONG                       ulCruisePathItemNum;

    /** Ѳ��·��Ԥ��λ���� */
    CRUISE_PATH_ITEM_S          astCruisePathItemList[IMOS_CRUISE_PATH_PRESET_MAX_COUNT];

    /** �����ֶ� */
    CHAR szReserve[8];
}CRUISE_PATH_INFO_S;

/**
 * @struct tagCruisePathQueryItem
 * @brief Ѳ��·����Ϣ��(��ѯѲ��·���б�ʱ����)
 * @attention
 */
typedef struct tagCruisePathQueryItem
{
   /** Ѳ��·�߱��� */
   CHAR     szCruisePathCode[IMOS_RES_CODE_LEN];

   /** Ѳ��·������ */
   CHAR     szCruisePathName[IMOS_NAME_LEN];
}CRUISE_PATH_QUERY_ITEM_S;


/***************************************************************************
Ѳ���ƻ�
****************************************************************************/

/**
 * @struct tagCruisePlanBase
 * @brief Ѳ���ƻ�������Ϣ
 * @attention
 */
typedef struct tagCruisePlanBase
{
    /** �ƻ����� */
    CHAR        szCruisePlanName[IMOS_NAME_LEN];

    /** �ƻ����������� */
    CHAR        szCreatorName[IMOS_NAME_LEN];

    /** �ƻ��������� */
    CHAR        szCreatTime[IMOS_TIME_LEN];

    /** ��������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** Ѳ���ƻ�������֯���� */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ƻ�����, ȡֵΪ#PLAN_TYPE_E */
    ULONG       ulPlanType;

    /** �ƻ�����, �ɲ��� */
    CHAR        szPlanDesc[IMOS_DESC_LEN];
}CRUISE_PLAN_BASE_S;

/**
 * @struct tagCruisePlanInfo
 * @brief Ѳ���ƻ�������Ϣ
 * @attention
 */
typedef struct tagCruisePlanInfo
{
    /** Ѳ���ƻ�������Ϣ */
    CRUISE_PLAN_BASE_S  stCruiseBaseInfo;

    /** Ѳ���ƻ�ʱ����Ϣ */
    PLAN_TIME_FOR_RES_S stCruiseTimeInfo;
}CRUISE_PLAN_INFO_S;

/**
 * @struct tagCruisePlanQueryItem
 * @brief Ѳ���ƻ���Ϣ��(��ѯѲ���ƻ��б�ʱ����)
 * @attention
 */
typedef struct tagCruisePlanQueryItem
{
    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** Ѳ���ƻ����� */
    CHAR        szCruisePlanName[IMOS_NAME_LEN];

    /** Ѳ���ƻ����� */
    CHAR        szCruisePlanCode[IMOS_RES_CODE_LEN];

    /** Ѳ���ƻ����������� */
    CHAR        szCreatorName[IMOS_NAME_LEN];

    /** Ѳ���ƻ��������� */
    CHAR        szCreatTime[IMOS_TIME_LEN];

    /** Ѳ���ƻ�״̬, BOOL_TRUEΪ�ƻ��Ѿ�����; BOOL_FALSEΪ�ƻ�δ���� */
    ULONG       ulPlanStatus;

     /** �����ֶ� */
     CHAR szReserve[8];
 }CRUISE_PLAN_QUERY_ITEM;



/***************************************************************************
 �洢��Դ
****************************************************************************/

/**
 * @struct taAlarmStorageTime
 * @brief ��ǰ����洢ʱ��
 * @attention
 */
typedef struct taAlarmStorageTime
{
    /** ��ǰ¼��ʱ��, ��ֵ��������, ����д����ULONGֵ */
    ULONG       ulPreAlarmTime;

    /** ����¼��ʱ��, ȡֵ��ΧΪ30~1800 */
    ULONG       ulAfterAlarmTime;

}ALARM_STORAGE_TIME_S;


/**
 * @struct tagStorageInfo
 * @brief ͨ���洢��Ϣ
 * @attention
 */
typedef struct tagStorageInfo
{
    /** �洢����,ö��ֵΪAS_DEVCM_STORAGE_TYPE_E, 0-IPSAN, 1-NAS, 2-���ش洢 */
    ULONG       ulStorageType;

    /** �洢��������, ȡֵ��Χ(0-2), 0��ʾipsan�洢, 1��ʾnas�洢, 2��ʾlocal�洢 */
    ULONG       ulStorageMedium;

    /** �洢ģʽ,��3��bit��ʾ��
        ��0λ���ƻ�
        ��1λ���ֶ�
        ��2λ���澯
    */
    ULONG       ulStorageMode;

    /** �ƻ������� */
    ULONG       ulPlanStreamType;

    /** �ֹ������� */
    ULONG       ulManualStreamType;

    /** �¼������� */
    ULONG       ulEventStreamType;

    /** ������������ԣ�ȡֵ0��1, 0��ʾ��ֹͣ,1����  */
    ULONG       ulDiskFullPolicy;

}STORAGE_INFO_S;

/**
 * @struct tagStoreResInfo
 * @brief �洢��Դ��Ϣ
 * @attention ��IMOS_ExpandStoreRes��: �����洢�豸��Ϣ,��ulStoreDevType, szStoreDevCode, szStoreDevName����.
 */
typedef struct tagStoreResInfo
{

    /** �洢����, ȡֵΪ#AS_DEVCM_STORAGE_TYPE_E */
    ULONG       ulStorageType;

    /** �洢�豸����, ȡֵΪ#STORE_DEV_TYPE_E */
    ULONG       ulStoreDevType;

    /** �洢�豸���� */
    CHAR        szStoreDevCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸���� */
    CHAR        szStoreDevName[IMOS_NAME_LEN];

    /** ָ���洢�豸�ռ䲻��ʱ,�Ƿ�֧���Զ�ѡ��洢�豸(BOOL_TRUEΪ֧��; BOOL_FALSEΪ��֧��) */
    BOOL_T  bStoreDevAutoSelect;

    /** �ƻ�������, ��ֵ�����������ECѡ������ײ�ֵ�й�, �����ײ�ֵΪ�����ײ�ʱ, ���������ֵΪ1; �����ײ�ֵΪ˫���ײ�ʱ, ���������ֵΪ2 */
    ULONG       ulPlanStreamType;

    /** �ֹ�������, ȡֵ�μ�#ulPlanStreamType��˵�� */
    ULONG       ulManualStreamType;

    /** �¼�������, ȡֵ�μ�#ulPlanStreamType��˵�� */
    ULONG       ulEventStreamType;

    /** ������������ԣ�ȡֵΪ#AS_DEVCM_DISK_FULL_POLICY_E */
    ULONG       ulDiskFullPolicy;

    /** �洢�ռ�(��IMOS_AssignStoreRes��,ָ��һ�η���Ĵ洢�ռ�; ��IMOS_ExpandStoreRes��,ָ��չ�ռ�[�����ѷ���ռ�]), ��ֵȡֵ������, ��λΪMB */
    ULONG       ulStoreSize;

    /** ��ǰ����¼��ʱ�� */
    ALARM_STORAGE_TIME_S stAlarmStorageTime;

}STORE_RES_INFO_S;



/**
 * @struct tagStoreResQueryItem
 * @brief �洢��Դ��Ϣ��(��ѯ�洢��Դ�б�ʱ����)
 * @attention �Դ洢�豸�������ж��Ƿ�����˴洢��Դ, ������洢�豸����Ϊ��, ����û�з���洢��Դ��
 */
typedef struct tagStoreResQueryItem
{

    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** �洢�豸���� */
    CHAR        szStoreDevName[IMOS_NAME_LEN];

    /** �洢�豸��DM���� */
    CHAR        szDMName[IMOS_NAME_LEN];

    /** �Ƿ��ƶ��洢�ƻ�, BOOL_TRUEΪ�ƶ��洢�ƻ�; BOOL_FALSEΪδ�ƶ��洢�ƻ� */
    BOOL_T      bHasStorePlan;

    /** �洢�ƻ����� */
    CHAR        szStorePlanCode[IMOS_RES_CODE_LEN];

    /** �ƻ�״̬, BOOL_TRUEΪ�ƻ�����; BOOL_FALSEΪ�ƻ�δ���� */
    BOOL_T      bPlanStatus;

    /** �洢��Դ״̬(#ISM_RES_FAILUREΪ����,1 ����,2 δ֪) */
    ULONG   ulResStatus;

    /** "�ƻ��洢"״̬(0 δ���ƻ��洢,1 ���ƻ��洢,2 δ֪) */
    ULONG   ulPlanStoreStatus;
}STORE_RES_QUERY_ITEM_S;

/**
 * @struct tagStoreResExtInfo
 * @brief �洢��Դ��չ��Ϣ
 * @attention
 */
typedef struct tagStoreResExtInfo
{
    /** �������豸���� */
    CHAR    szParentDevCode[IMOS_DEVICE_CODE_LEN];

    /** �������豸����,ȡֵ��ΧΪ #IMOS_TYPE_EC��#IMOS_TYPE_ECR */
    ULONG   ulParentDevType;

    /** �������豸������,��ȡֵ����Ϊ#IMOS_DEVICE_TYPE_E�еı���������洢���ܵı��������� */
    ULONG   ulParentSubDevType;

    /** �����ֶ�1 */
    CHAR    szReserved[IMOS_STRING_LEN_32];

    /** �����ֶ�2��ĿǰĬ��Ϊ0 */
    ULONG   ulReserved;
}STORE_RES_EXT_INFO_S;

/**
 * @struct tagStoreResQueryItemV2
 * @brief �洢��Դ��Ϣ��V2(��ѯ�洢��Դ�б�ʱ����)
 * @attention
 */
typedef struct tagStoreResQueryItemV2
{
    /** �洢��Դ��Ϣ */
    STORE_RES_QUERY_ITEM_S stStoreResQryItem;

    /** �洢��Դ��չ��Ϣ */
    STORE_RES_EXT_INFO_S   stStoreResExtInfo;
}STORE_RES_QUERY_ITEM_V2_S;
/***************************************************************************
�洢�ƻ�
****************************************************************************/
/**
 * @struct tagStorePlanBase
 * @brief �洢�ƻ�������Ϣ
 * @attention
 */
typedef struct tagStorePlanBase
{
    /** �洢�ƻ����� */
    CHAR    szStorePlanName[IMOS_NAME_LEN];

    /** �ƻ�����ʱ�� */
    CHAR    szPlanCreateTime[IMOS_TIME_LEN];

    /** �ƻ�����, ȡֵΪ#PLAN_TYPE_E */
    ULONG   ulPlanType;

    /** ��֯���� */
    CHAR    szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ƻ�����, �ɲ��� */
    CHAR    szPlanDesc[IMOS_DESC_LEN];

    /** �Ƿ���洢, BOOL_TRUEΪ����洢; BOOL_FALSEΪ�ǰ���洢 */
    BOOL_T  bIsDayStore;

    /** ����洢������(�ݲ�ʹ��) */
    ULONG   ulStoreDays;
 }STORY_PLAN_BASE_S;


/**
 * @struct tagStoragePlan
 * @brief �洢�ƻ���Ϣ
 * @attention
 */
typedef struct tagStoragePlan
{
    /** �洢�ƻ�������Ϣ */
    STORY_PLAN_BASE_S   stStorageBasicInfo;

    /** �洢�ƻ�ʱ����Ϣ */
    PLAN_TIME_S         stStorageTimeInfo;

}STORAGE_PLAN_INFO_S;


/**
 * @struct tagStoryPlanQueryItem
 * @brief ��ѯ�洢�ƻ�ʱ�õ���Ϣ���洢�ƻ�������Ϣ����������ֹ�����Ϣ�����ڽ����ѯ��ʾ
 * @attention
 */
typedef struct tagStoryPlanQueryItem
{

    /** �洢�ƻ����� */
    CHAR        szStorePlanCode[IMOS_RES_CODE_LEN];

    /** �洢�ƻ����� */
    CHAR    szStorePlanName[IMOS_NAME_LEN];

    /** �ƻ�����ʱ�� */
    CHAR    szPlanCreateTime[IMOS_TIME_LEN];

    /** �ƻ�״̬��BOOL_TRUE ��ʾ�ƻ��Ѿ����� */
    ULONG       ulPlanStatus;

    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** �����������֯���� */
    CHAR        szOrgName[IMOS_NAME_LEN];

 }STORY_PLAN_QUERY_ITEM;


/***************************************************************************
������Դ����
****************************************************************************/
/**
 * @struct tagSwitchBase
 * @brief ������Դ������Ϣ
 * @attention
 */
typedef struct tagSwitchBase
{
    /** �������� */
    CHAR                szSwitchName[IMOS_NAME_LEN];

    /** ��֯���� */
    /* CHAR                szOrgCode[IMOS_DOMAIN_CODE_LEN]; */

    /** ��������, �ɲ��� */
    CHAR                szSwitchDesc[IMOS_DESC_LEN];
}SWITCH_BASE_S;


/**
 * @struct tagSwitchUnit
 * @brief ������Դ�е��������Ϣ
 * @attention
 */
typedef struct tagSwitchUnit
{
    /** ��������� */
    CHAR        szCameraCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR        szCameraName[IMOS_NAME_LEN];

    /** ��� */
    ULONG       ulSequence;

     /** �л�ʱ�� */
    ULONG       ulInterval;
}SWITCH_UNIT_S;

/**
 * @struct tagSwitchResoure
 * @brief ������Դ��Ϣ
 * @attention
 */
typedef struct tagSwitchResoure
{
    /** ���л�����Ϣ */
    SWITCH_BASE_S   stSwitchBaseInfo;

    /** ������Դ������������Դ��ʵ����Ŀ, ���ȡֵΪ#IMOS_SWITCH_CAM_MAXNUM */
    ULONG               ulSwitchResNum;

    /** ������Դ���� */
    SWITCH_UNIT_S    astSwitchUnitList[IMOS_SWITCH_CAM_MAXNUM];
}SWITCH_RESOURE_S;

/**
 * @struct tagSwitchUnitV2
 * @brief ������Դ��V2
 * @attention ��
 */
typedef struct tagSwitchUnitV2
{
    /** ������Դ��V1 */
    SWITCH_UNIT_S stSwitchUnitV1;

    /** ������,ȡֵΪ#IMOS_FAVORITE_STREAM_E */
    ULONG ulStreamType;

    /** �����ֶ� */
    CHAR szReserve[128];
} SWITCH_UNIT_V2_S;

/**
 * @struct tagSwitchResoureV2
 * @brief ������ԴV2
 * @attention ��
 */
typedef struct tagSwitchResoureV2
{
    /** ������Դ������Ϣ */
    SWITCH_BASE_S stSwitchBaseInfo;

    /** ������Դ����Ŀ,���ȡֵΪ#IMOS_SWITCH_CAM_MAXNUM_V2 */
    ULONG ulSwitchResNum;

    /** ������Դ���б� */
    SWITCH_UNIT_V2_S astSwitchUnitList[IMOS_SWITCH_CAM_MAXNUM_V2];
} SWITCH_RESOURE_V2_S;

/**
 * @struct tagSwitchResQueryItem
 * @brief ������Դ��Ϣ��(��ѯ������Դ�б�ʱ����)
 * @attention
 */
typedef struct tagSwitchResQueryItem
{
    /** ������Դ���� */
    CHAR                szSwitchResCode[IMOS_RES_CODE_LEN];

    /** ������Դ���� */
    CHAR                szSwitchName[IMOS_NAME_LEN];

    /** ������Դ������֯���� */
    CHAR                szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ������Դ������֯���� */
    CHAR                szOrgName[IMOS_NAME_LEN];

}SWITCH_RES_QUERY_ITEM_S;



/***************************************************************************
���мƻ�
****************************************************************************/

/**
 * @struct tagSwitchPlanBase
 * @brief ���мƻ�������Ϣ
 * @attention
 */
typedef struct tagSwitchPlanBase
{
    /** ���мƻ����� */
    CHAR        szPlanName[IMOS_NAME_LEN];

    /** ���мƻ��������� */
    CHAR        szCreatTime[IMOS_TIME_LEN];

    /** ���������� */
    CHAR        szScrCode[IMOS_RES_CODE_LEN];

    /** ���������� */
    CHAR        szScrName[IMOS_NAME_LEN];

    /** �ƻ�����, ȡֵΪ#PLAN_TYPE_E */
    ULONG       ulPlanType;

    /** ���мƻ�������֯���� */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ƻ�����, �ɲ��� */
    CHAR        szPlanDesc[IMOS_DESC_LEN];
}SWITCH_PLAN_BASE_S;


/**
 * @struct tagSwitchPlanInfo
 * @brief ���мƻ���Ϣ
 * @attention
 */
typedef struct tagSwitchPlanInfo
{
    /** ���мƻ�������Ϣ */
    SWITCH_PLAN_BASE_S  stSwitchBaseInfo;

    /** ���мƻ�ʱ����Ϣ */
    PLAN_TIME_FOR_RES_S stSwitchTimeInfo;
}SWITCH_PLAN_INFO_S;

/**
 * @struct tagSwitchPlanInfo
 * @brief ���мƻ���Ϣ
 * @attention
 */
typedef struct tagSwitchPlanInfoV2
{
    /** ���мƻ�������Ϣ */
    SWITCH_PLAN_BASE_S  stSwitchBaseInfo;

    /** ���мƻ�ʱ����Ϣ */
    PLAN_TIME_FOR_RES_V2_S stSwitchTimeInfo;
}SWITCH_PLAN_INFO_V2_S;


/**
 * @struct tagSwitchPlanQueryItem
 * @brief ���мƻ���Ϣ��(��ѯ���мƻ��б�ʱ����)
 * @attention
 */
typedef struct tagSwitchPlanQueryItem
{
    /** ���мƻ����� */
    CHAR            szSwitchPlanCode[IMOS_RES_CODE_LEN];

    /** ���мƻ����� */
    CHAR            szPlanName[IMOS_NAME_LEN];

    /** ���мƻ��������� */
    CHAR            szCreatTime[IMOS_TIME_LEN];

    /** ���������� */
    CHAR            szScrName[IMOS_NAME_LEN];

    /** �ƻ�״̬��BOOL_TRUEΪ�ƻ��Ѿ�����, BOOL_FALSEΪ�ƻ�δ���� */
    ULONG           ulPlanStatus;

}SWITCH_PLAN_QUERY_ITEM_S;




/***************************************************************************
�澯
****************************************************************************/

/**
 * @struct tagAlarmSrc
 * @brief �澯Դ��Ϣ
 * @attention
 */
typedef struct tagAlarmSrc
{

    /** �澯Դ������ */
    CHAR        szAlarmSrcName[IMOS_NAME_LEN];

    /** �澯Դ������, ȡֵΪ#ALARM_DEV_TYPE_E */
    ULONG       ulAlarmSrcType;

    /** �����˸澯Դ��ͨ�������豸�ı��� */
    CHAR        szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �����˸澯Դ��ͨ�������豸������ */
    CHAR        szDevName[IMOS_NAME_LEN];

    /** �澯Դ�󶨵�ͨ������, ȡֵ�Ӹ澯Դ�����豸���Ͷ��� */
    ULONG       ulChannelIndex;
}ALARM_SRC_S;

/**
 * @struct tagAlarmSrcQueryItem
 * @brief �澯Դ��ѯ��Ϣ�б�Ԫ��Ϣ
 * @attention
 */
typedef struct tagAlarmSrcQueryItem
{

    /** �澯Դ������ */
    CHAR        szAlarmSrcName[IMOS_NAME_LEN];

    /** �澯Դ�ı��� */
    CHAR        szAlarmSrcCode[IMOS_RES_CODE_LEN];

    /** �豸���ͣ�����ö��ȡֵ�μ�#IMOS_TYPE_E�� */
    ULONG       ulDevType;

    /** �豸�����ͣ�����ö��ȡֵ�μ�#IMOS_DEVICE_TYPE_E�� */
    ULONG       ulDevSubType;

    /** �澯Դ������֯����(Ŀǰ������,����ʹ��) */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �澯Դ������֯����(Ŀǰ������,����ʹ��) */
    CHAR        szOrgName[IMOS_NAME_LEN];

}ALARM_SRC_QUERY_ITEM_S;

/**
* @struct tagAlarmSrcQueryItemEx
* @brief �澯Դ��ѯ��Ϣ�б�Ԫ��չ��Ϣ
* @attention
*/
typedef struct tagAlarmSrcQueryItemEx
{
    /* �澯Դ��ѯ��Ϣ�б�Ԫ��Ϣ */
    ALARM_SRC_QUERY_ITEM_S stAlarmSrcQueryItem;

    /** ���豸���ͣ�����ö��ȡֵ�μ�#IMOS_TYPE_E�� */
    ULONG ulParentDevType;

    /** ���豸�����ͣ�����ö��ȡֵ�μ�#IMOS_DEVICE_TYPE_E�� */
    ULONG ulParentSubDevType;

    /* �Ƿ�֧������ */
    BOOL_T bHasLinkage;

    /* �Ƿ�֧�ֲ��� */
    BOOL_T bHasGuard;

    /* �Ƿ���ڸ澯�����Զ��� */
    BOOL_T bHasAlarmEnabled;

}ALARM_SRC_QUERY_ITEM_EX_S;

/**
 * @struct tagAlarmExporter
 * @brief �澯����豸��Ϣ
 * @attention
 */
typedef struct tagAlarmExporter
{

    /** �澯����豸������ */
    CHAR        szAlarmExporterName[IMOS_NAME_LEN];

    /** �澯����豸������, ȡֵΪ#ALARM_DEV_TYPE_E */
    ULONG       ulAlarmExporterType;

    /** �澯����豸�󶨵�ͨ�������豸�ı��� */
    CHAR        szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �澯����豸�󶨵�ͨ�������豸������ */
    CHAR        szDevName[IMOS_NAME_LEN];

    /** �澯����豸�󶨵�ͨ������, ȡֵ�Ӹ澯Դ�����豸���Ͷ��� */
    ULONG       ulChannelIndex;
}ALARM_EXPORTER_S;

/**
 * @struct tagAlarmExporterQueryItem
 * @brief �澯����豸��ѯ��Ϣ�б�Ԫ��Ϣ
 * @attention
 */
typedef struct tagAlarmExporterQueryItem
{

    /** �澯����豸������ */
    CHAR        szAlarmExporterName[IMOS_NAME_LEN];

    /** �澯����豸�ı��� */
    CHAR        szAlarmExporterCode[IMOS_RES_CODE_LEN];

    /** �澯����豸������,ö������Ϊ:ALARM_DEV_TYPE_E */
    ULONG       ulAlarmExporterType;

    /** �澯Դ������֯ */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ������֯���� */
    CHAR        szOrgName[IMOS_NAME_LEN];

}ALARM_EXPORTER_QUERY_ITEM_S;

/**
 * @struct tagEventRecord
 * @brief �澯��¼��Ϣ
 * @attention
 */
typedef struct tagEventRecord
{
    /** �澯����*/
    CHAR szEventRecordCode[IMOS_RES_CODE_LEN];

    /** �澯����, ȡֵΪ#AlARM_TYPE_E */
    ULONG ulEventType;

    /** �豸����*/
    CHAR szDeviceCode[IMOS_DEVICE_CODE_LEN];

    /** �豸����*/
    CHAR  szDeviceName[IMOS_NAME_LEN];

    /** �澯����*/
    CHAR szActiveName[IMOS_NAME_LEN];

    /** �澯����, ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    ULONG ulEventSecurity;

    /** �澯����ʱ��*/
    CHAR szEventTime[IMOS_TIME_LEN];

    /** �澯״̬, ȡֵΪ#ALARM_STATUS_E */
    ULONG ulAlarmStatus;

    /** �¼��������� */
    CHAR   szSecurityName[IMOS_NAME_LEN];

    /** �¼�������ʾ��ɫ */
    CHAR   szSecurityColor[IMOS_STRING_LEN_14];

    /** �澯�˾�ʱ��*/
    CHAR szCheckTime[IMOS_TIME_LEN];

    /** �澯�˾��û�*/
    CHAR szCheckUser[IMOS_NAME_LEN];

    /** �澯ȷ��ʱ��*/
    CHAR szAckTime[IMOS_TIME_LEN];

    /** �澯ȷ���û�*/
    CHAR szAckUser[IMOS_NAME_LEN];

    /** ������ֵ��*/
    CHAR szKeyValue[IMOS_STRING_LEN_128];

    /** �澯������Ϣ*/
    CHAR szEventDesc[IMOS_DESC_LEN];

    /** �澯ȷ������*/
    CHAR szAckSuggestion[IMOS_DESC_LEN];

    /** �Ƿ��� */
    ULONG ulIsMisinform;

    /** �Ƿ�Ԥ���澯 */
    ULONG ulIsPlanAlarm;

    /** �Ƿ�������Ԥ������ѯ�澯�������̼�¼�ã� */
    ULONG ulIsOncePlanAlarm;

    /** �Ƿ�����Ҫ�ֹ���������*/
    ULONG ulIsManualTask;
}EVENT_RECORD_S;

/**
 * @struct tagStoreAction
 * @brief �����洢
 * @attention
 */
typedef struct tagStoreAction
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szCamName[IMOS_NAME_LEN];

}STORE_ACTION_S;


/**
 * @struct tagPresetAction
 * @brief ����Ԥ��λ
 * @attention
 */
typedef struct tagPresetAction
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** Ԥ��λֵ, ȡֵ��ΧΪ#PTZ_PRESET_MINVALUE~�����������ļ������õ�Ԥ��λ���ֵ */
    ULONG       ulPresetValue;

    /** Ԥ��λ������ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szPresetDesc[IMOS_DESC_LEN];
}PRESET_ACTION_S;


/**
 * @struct tagScreenLiveAction
 * @brief ����ʵ��������
 * @attention
 */
typedef struct tagScreenLiveAction
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ�*/
    CHAR        szCamName[IMOS_NAME_LEN];

    /** �������û�����ʱ���û����� */
    CHAR        szUserCode[IMOS_USER_CODE_LEN];

    /** �������û�����ʱ���û����ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szUserName[IMOS_NAME_LEN];

    /** �������û�����Ļ���, ȡֵΪ1 */
    ULONG       ulScreenIndex ;

    /** �������û��Ĵ������ */
    CHAR        szXPPaneCode[IMOS_RES_CODE_LEN];

    /** �Ƿ�ץ��, ȡֵΪ0��1 */
    ULONG       ulSnatchImage;

    /** ץ�ļ����ȡֵ��λ���� */
    ULONG       ulInterval;

    /** ץ������ */
    ULONG       ulSnatchCounter;

    /** �Ƿ����þ�ǰ¼��ȡֵ0����1���� */
    ULONG       ulIsBefAlarmReplay;

    /** ��ǰ¼��ʱ������ȡֵ��λ�� */
    ULONG       ulBefAlarmRepInterval;

    /** ������ǰ¼����û���Ļ���, ȡֵΪ1 */
    ULONG       ulBefAlarmRepScreenIndex;

    /** �������û��Ĵ������ */
    CHAR        szBefAlarmRepXPPaneCode[IMOS_CODE_LEN];

}SCREENLIVE_ACTION_S;

/**
 * @struct tagScreenLiveActionForECR
 * @brief ����ʵ��������
 * @attention
 */
typedef struct tagScreenLiveActionForECR
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ�*/
    CHAR        szCamName[IMOS_NAME_LEN];

    /** �������û�����ʱ���û����� */
    CHAR        szUserCode[IMOS_USER_CODE_LEN];

    /** �������û�����ʱ���û����ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szUserName[IMOS_NAME_LEN];

    /** �������û�����Ļ���, ȡֵΪ1 */
    ULONG       ulScreenIndex ;

    /** �������û��Ĵ������ */
    CHAR        szXPPaneCode[IMOS_RES_CODE_LEN];

}SCREENLIVE_ACTION_FOR_ECR_S;


/**
 * @struct tagMonitorLiveAction
 * @brief ����ʵ����������
 * @attention
 */
typedef struct tagMonitorLiveAction
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** ��������������豸�ı������ײ�, ֻ���ڲ�ѯ, ����ʱ��Ч, ȡֵΪ#IMOS_STREAM_RELATION_SET_E */
    ULONG       ulEncodeSet;

    /** ���������ʵ��ʱ��������, ȡֵΪ#IMOS_FAVORITE_STREAM_E */
    ULONG       ulStreamType;

    /** �������������� */
    CHAR        szSrcCode[IMOS_RES_CODE_LEN];

    /** �������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szSrcName[IMOS_NAME_LEN];

    /** ���������������豸�Ľ������ײ�, ֻ���ڲ�ѯ, ����ʱ��Ч, ȡֵΪ#IMOS_STREAM_RELATION_SET_E�е�
        ����ֵΪ���������ײͣ�
        1��H264(#IMOS_SR_H264_SHARE)
        3: MJPEG(#IMOS_SR_H264_MJPEG)
        4��MEPG4(#IMOS_SR_MPEG4_SHARE)
        5��MEPG2(#IMOS_SR_MPEG2_SHARE)
    ��ȡֵΪ#IMOS_SR_INVALID, ���Ƿ�ֵ, ��˵����֧�ֵĲ�ѯ: ��������ѱ�ɾ���������ECR HF������� */
    ULONG       ulDecodeSet;

    /* ������,0Ϊ��Чֵ */
    ULONG      ulScreenIndex;


}MONITORLIVE_ACTION_S;

/**
 * @struct tagMonitorLiveActionForECR
 * @brief ����ʵ����������
 * @attention
 */
typedef struct tagMonitorLiveActionForECR
{

    /** ������������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** �������������� */
    CHAR        szSrcCode[IMOS_RES_CODE_LEN];

    /** �������������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szSrcName[IMOS_NAME_LEN];

}MONITORLIVE_ACTION_FOR_ECR_S;

/**
 * @struct tagGISAction
 * @brief ������ͼ��˸
 * @attention
 */
typedef struct tagGISAction
{
    /** ��˸ʹ�ܱ��, BOOL_TRUEΪʹ��; BOOL_FALSEΪ��ʹ�� */
    BOOL_T  bIsEnabled;

}GIS_ACTION_S;


/**
 * @struct tagSwitchOUTAction
 * @brief �������������
 * @attention
 */
typedef struct tagSwitchOUTAction
{

    /** �豸���� */
    CHAR        szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szDevName[IMOS_NAME_LEN];
}SWITCHOUT_ACTION_S;


/**
 * @struct tagBackupAction
 * @brief ��������
 * @attention
 */
typedef struct tagBackupAction
{

    /** ��������� */
    CHAR        szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ��������ƣ�ֻ���ڸ澯�������Բ�ѯ�ӿڣ� */
    CHAR        szCamName[IMOS_NAME_LEN];
}BACKUP_ACTION_S;

/**
* @struct tagReceiverAction
* @brief ���������ʼ�
* @attention ��
*/
typedef struct tagReceiverAction
{
    /** ������ʵ����Ŀ */
    ULONG ulReceiverNum;

    /** ��������Ϣ(����ʱ�û�������Ч��������Ч����ѯʱ�û���¼ID���û���¼��ַ���û���½ʱ�䡢�û��Ƿ�������Ч��������Ч) */
    USER_QUERY_ITEM_S astReceiver[IMOS_ALARM_ACTION_RECEIVER_MAXNUM];

    /** �ʼ����⣨���ŵĻ����ֶ���Ч�� */
    CHAR        szTitle[IMOS_NAME_LEN];

    /** ���� */
    CHAR        szContent[IMOS_DESC_LEN];

}RECEIVER_ACTION_S;

/**
 * @struct tagBuzzerAction
 * @brief �����������澯
 * @attention
 */
typedef struct tagBuzzerAction
{

    /** �����������豸���� */
    CHAR        szDevCode[IMOS_CODE_LEN];

    /** ������������ Ԥ��������չ:����ʱ����Ƶ�ʵ� */
    CHAR        szParam[IMOS_STRING_LEN_64];
}BUZZER_ACTION_S;

/**
* @struct tagActionPlanBaseInfo
* @brief Ԥ��������Ϣ
* @attention
*/
typedef struct tagActionPlanBaseInfo
{
    /** Ԥ������ */
    CHAR szActionPlanCode[IMOS_CODE_LEN];

    /** Ԥ������ */
    CHAR szActionPlanName[IMOS_NAME_LEN];

    /** Ԥ������ */
    CHAR szActionPlanDesc[IMOS_DESC_LEN];

    /** Ԥ�����ͣ�ȡֵΪ#ACTION_PLAN_TYPE_E */
    ULONG ulActionPlanType;
}ACTION_PLAN_BASE_INFO_S;

/**
 * @struct tagAlarmAction
 * @brief �澯��������
 * @attention
 */
typedef struct tagAlarmAction
{

    /** �洢���������д洢������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_STORE_ACTION_MAXNUM */
    ULONG                       ulStoreActionNum;

    /** �洢�������� */
    STORE_ACTION_S          astStoreAction[IMOS_ALARM_STORE_ACTION_MAXNUM];

    /** Ԥ��λ����������Ԥ��λ������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_PRESET_ACTION_MAXNUM */
    ULONG                       ulPresetActionNum;

    /** Ԥ��λ�������� */
    PRESET_ACTION_S         astPresetAction[IMOS_ALARM_PRESET_ACTION_MAXNUM];

    /** ����ʵ��������������������ʵ������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_SCREENLIVE_ACTION_MAXNUM */
    ULONG                       ulScreenLiveActionNum;

    /** ����ʵ�������������� */
    SCREENLIVE_ACTION_S     astScreenLiveAction[IMOS_ALARM_SCREENLIVE_ACTION_MAXNUM];

    /** ����ʵ������������������������ʵ����������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_MONITORLIVE_ACTION_MAXNUM */
    ULONG                       ulMonitorLiveActionNum;

    /** ����ʵ������������������ */
    MONITORLIVE_ACTION_S    astMonitorLiveAction[IMOS_ALARM_MONITORLIVE_ACTION_MAXNUM];

    /** ������ͼ��˸����������������ͼ��˸������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_GIS_ACTION_MAXNUM */
    ULONG                       ulGISActionNum;

    /** ������ͼ��˸�������� */
    GIS_ACTION_S            astGISAction[IMOS_ALARM_GIS_ACTION_MAXNUM];

    /** ������������������������������������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_SWITCHOUT_ACTION_MAXNUM */
    ULONG                       ulSwitchOUTActionNum;

    /** ��������������������� */
    SWITCHOUT_ACTION_S      astSwitchOUTAction[IMOS_ALARM_SWITCHOUT_ACTION_MAXNUM];

    /** �������ݶ����������������ݶ�����ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_BACKUP_ACTION_MAXNUM */
    ULONG                       ulBackupActionNum;

    /** �������ݶ������� */
    BACKUP_ACTION_S         astBackupAction[IMOS_ALARM_BACKUP_ACTION_MAXNUM];

    /** ���� */
    RECEIVER_ACTION_S        stReceiverSMSAction;

    /** �ʼ� */
    RECEIVER_ACTION_S        stReceiverEmailAction;

    /** ������Ԥ��ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_ACTION_PLAN_MAXNUM */
    ULONG ulActionPlanBaseNum;

    /** Ԥ����Ϣ */
    ACTION_PLAN_BASE_INFO_S  astActionPlanBaseInfo[IMOS_ALARM_ACTION_PLAN_MAXNUM];

}ALARM_ACTION_S;

/**
 * @struct tagAlarmActionForECR
 * @brief �澯��������(for ECR&NVR buzzer)
 * @attention
 */
typedef struct tagAlarmActionForECR
{

    /** �洢���������д洢������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_STORE_ACTION_MAXNUM */
    ULONG                       ulStoreActionNum;

    /** �洢�������� */
    STORE_ACTION_S          astStoreAction[IMOS_ALARM_STORE_ACTION_MAXNUM];

    /** Ԥ��λ����������Ԥ��λ������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_PRESET_ACTION_MAXNUM */
    ULONG                       ulPresetActionNum;

    /** Ԥ��λ�������� */
    PRESET_ACTION_S         astPresetAction[IMOS_ALARM_PRESET_ACTION_MAXNUM];

    /** ����ʵ��������������������ʵ������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_SCREENLIVE_ACTION_MAXNUM */
    ULONG                       ulScreenLiveActionNum;

    /** ����ʵ�������������� */
    SCREENLIVE_ACTION_FOR_ECR_S     astScreenLiveAction[IMOS_ALARM_SCREENLIVE_ACTION_MAXNUM];

    /** ����ʵ������������������������ʵ����������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_MONITORLIVE_ACTION_MAXNUM */
    ULONG                       ulMonitorLiveActionNum;

    /** ����ʵ������������������ */
    MONITORLIVE_ACTION_FOR_ECR_S    astMonitorLiveAction[IMOS_ALARM_MONITORLIVE_ACTION_MAXNUM];

    /** ������ͼ��˸����������������ͼ��˸������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_GIS_ACTION_MAXNUM */
    ULONG                       ulGISActionNum;

    /** ������ͼ��˸�������� */
    GIS_ACTION_S            astGISAction[IMOS_ALARM_GIS_ACTION_MAXNUM];

    /** ������������������������������������������ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_SWITCHOUT_ACTION_MAXNUM */
    ULONG                       ulSwitchOUTActionNum;

    /** ��������������������� */
    SWITCHOUT_ACTION_S      astSwitchOUTAction[IMOS_ALARM_SWITCHOUT_ACTION_MAXNUM];

    /** �������ݶ����������������ݶ�����ʵ����Ŀ, ȡֵ���ֵΪ#IMOS_ALARM_BACKUP_ACTION_MAXNUM */
    ULONG                       ulBackupActionNum;

    /** �������ݶ������� */
    BACKUP_ACTION_S         astBackupAction[IMOS_ALARM_BACKUP_ACTION_MAXNUM];

	/** �����������澯������Ŀ */
    ULONG                   ulBuzzerActionNum;

    /** �����������澯�����б� */
    BUZZER_ACTION_S      	astBuzzerAction[IMOS_ALARM_BUZZER_ACTION_MAXNUM];

}ALARM_ACTION_FOR_ECR_S;



/**
 * @struct tagAlarmLinkageInfo
 * @brief �澯������Ϣ�ṹ��
 * @attention
 */
typedef struct tagAlarmLinkageInfo
{

    /** �澯Դ�ı��� */
    CHAR                    szAlarmSrcCode[IMOS_RES_CODE_LEN];


    /** �澯���� */
    ULONG                   ulAlarmType;

    /** �澯Դ��Ӧ�澯�����������Ķ��� */
    ALARM_ACTION_S      stAction;

}ALARM_LIKNAGE_INFO_S;

/**
* @struct tagAlarmEventQueryCondition
* @brief �澯�¼���ѯ����
*/
typedef struct tagAlarmEventQueryCondition
{
    /** ���������*/
    CHAR                szCamCode[IMOS_CODE_LEN];

    /** ��������ʼ/����ʱ�� */
    TIME_SLICE_S    stQueryTimeSlice;

    /** �����ֶ� */
    CHAR szReserve[64];
}ALARM_EVENT_QUERY_CONDITION_S;


/**
* @struct tagAlarmEventInfo
* @brief �澯�¼���Ϣ(��ѯ�澯�¼���Ϣ�б�ʱ����)
*/
typedef struct tagAlarmEventInfo
{
    /** �澯���� */
    ULONG ulAlarmType;

    /** �澯Դ���� */
    CHAR szAlarmSrcName[IMOS_NAME_LEN];

    /** �澯����ʱ��, ����"%Y-%m-%d %H:%M:%S"��ʽ, �����޶�Ϊ24�ַ� */
    CHAR szAlarmTime[IMOS_TIME_LEN];

    /** �洢������������� */
    CHAR szCameraCode[IMOS_CODE_LEN];

}ALARM_EVENT_INFO_S;



/*******************************************************************************
�����ƻ� GuardPlan , Edited By sunyifan(06966) Completed
********************************************************************************/
/**
 * @struct tagGuardStratageBasicInfo
 * @brief �������Ի�����Ϣ
 * @attention
 */
typedef struct tagGuardStratageBasicInfo
{

    /** ������������ */
    CHAR        szPolicyName[IMOS_NAME_LEN];

    /** �ƻ��������� */
    CHAR        szCreatTime[IMOS_TIME_LEN];

    /** �Ƿ�ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG       ulEnabledFlag;

    /** ������ѭ�����߰�����ѭ��, ȡֵΪ#MM_JOBSCHDTYPE_E */
    ULONG       ulCycleType;

    /** ����������֯���� */
    CHAR    szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ƻ�����, �ɲ��� */
    CHAR        szPlanDesc[IMOS_DESC_LEN];

}GUARD_STRATAGE_BASIC_INFO_S;


/**
 * @struct tagGuardPlanInfo
 * @brief �����ƻ���Ϣ
 * @attention
 */
typedef struct tagGuardPlanInfo
{
    /** �������Ի�����Ϣ */
    GUARD_STRATAGE_BASIC_INFO_S stGuardStratageBasicInfo;

    /** �����ƻ�ʱ����Ϣ */
    PLAN_TIME_S     stGuardTimeInfo;

}GUARD_PLAN_INFO_S;

/**
 * @struct tagGuardPlanQueryItem
 * @brief �������Լƻ���Ϣ��(��ѯ�������Լƻ��б�ʱ����)
 * @attention
 */
typedef struct tagGuardPlanQueryItem
{

    /** �澯Դ���� */
    CHAR            szAlarmSrcName[IMOS_NAME_LEN];

    /** �澯����, ȡֵΪ#AlARM_TYPE_E */
    ULONG           ulAlarmType;

    /** �澯Դ������֯���� */
    CHAR            szOrgName[IMOS_NAME_LEN];

    /** �����ƻ����� */
    CHAR        szPlanName[IMOS_NAME_LEN];

    /** �����ƻ��������� */
    CHAR            szCreatTime[IMOS_TIME_LEN];

    /** �����ƻ�״̬, 1Ϊ�ƻ��Ѿ�����; 0Ϊ�ƻ�δ���� */
    ULONG           ulPlanStatus;

    /** �����ƻ����� */
    CHAR            szGuardPlanCode[IMOS_RES_CODE_LEN];

}GUARD_PLAN_QUERY_ITEM_S;


/***************************************************************************
GIS
****************************************************************************/

/**
 * @struct tagFileGISInfo
 * @brief GISͼƬ��Ϣ
 * @attention
 */
typedef struct tagFileGISInfo
{
    /** ��ͼ����(����ͼ�ļ���չ��)ȡֵ�μ�#MAP_TYPE_E */
    ULONG                   ulGISMapType;

    /** ��ͼ������֯���� */
    CHAR                    szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** GIS�ļ�������Ϣ */
    FILE_BASIC_INFO_S   stFileBasicInfo;
}FILE_GIS_INFO_S;



/**
 * @struct tagGISQueryItem
 * @brief GISͼƬ��Ϣ��(���ڲ�ѯʱ����)
 * @attention
 */
typedef struct tagGISQueryItem
{
    /** ��ͼ���� */
    CHAR        szGISMapCode[IMOS_RES_CODE_LEN];

    /** ��ͼ����, Ϊ���ӵ�ͼʱ�û�ָ�������� */
    CHAR        szName[IMOS_NAME_LEN];

    /** GIS��ͼ������֯���� */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

}GIS_QUERY_ITEM_S;


/**
 * @struct tagGisHotZone
 * @brief GIS������Ϣ
 * @attention
 */
typedef struct tagGisHotZone
{
    /** �������� */
    CHAR        szHotZoneName[IMOS_NAME_LEN];

    /** �������ӵĵ�ͼ���� */
    CHAR        szGISMapCode[IMOS_RES_CODE_LEN];

    /** ����ͼ�α߽��ַ�����֧�ֵ�Ͷ���� */
    CHAR        szBorderline[IMOS_STRING_LEN_256];

    /** ��������, �ɲ��� */
    CHAR        szHotZoneDesc[IMOS_DESC_LEN];

}GIS_HOTZONE_S;

/**
 * @struct tagGisHotZoneWithCode
 * @brief �������������GIS����ͼ����Ϣ(���ڲ�ѯʱ����)
 * @attention
 */
typedef struct tagGisHotZoneWithCode
{
    /** �������� */
    CHAR            szHotZoneCode[IMOS_RES_CODE_LEN];

    /** ����������Ϣ */
    GIS_HOTZONE_S   stHotZoneInfo;

}GIS_HOTZONE_WITH_CODE_S;

/**
 * @struct tagGisHotSPOT
 * @brief GIS�ȵ���Ϣ
 * @attention
 */
typedef struct tagGisHotSPOT
{
    /** �ȵ����ӵ�����֯���� */
    CHAR            szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ȵ�λ����Ϣ����, ���Բ��� */
    CHAR            szSiteDesc[IMOS_STRING_LEN_32];

}GIS_HOTSPOT_S;

/**
 * @struct tagGisHotSpotWithCode
 * @brief �����ȵ�����GIS�ȵ�ͼ��(���ڲ�ѯʱ����)
 * @attention
 */
typedef struct tagGisHotSpotWithCode
{
    /** �ȵ���� */
    CHAR            szHotSpotCode[IMOS_RES_CODE_LEN];

    /** �ȵ����ӵ�����֯���� */
    CHAR            szOrgName[IMOS_NAME_LEN];

    /** �ȵ�������Ϣ */
    GIS_HOTSPOT_S   stHotSpotInfo;

}GIS_HOTSPOT_WITH_CODE_S;

/**
 * @struct tagGisTextbox
 * @brief GIS�ı���
 * @attention
 */
typedef struct tagGisTextbox
{
    /** �ı���Ϣ */
    CHAR    szTextInfo[IMOS_DESC_LEN];

    /** �ı���λ����Ϣ���� */
    CHAR    szSiteDesc[IMOS_STRING_LEN_32];

    /** ��������, �����ȡֵ��ʹ��������� */
    ULONG   ulTextType;

    /** ���ִ�С, �����ȡֵ��ʹ��������� */
    ULONG   ulTextSize;

    /** ������ɫ, �����ȡֵ��ʹ��������� */
    ULONG   ulTextColor;

    /** �����Ƿ�Ӵ�, 1Ϊ�Ӵ�; 0Ϊ���Ӵ� */
    ULONG   ulBoldFlag;

}GIS_TEXTBOX_S;


/**
 * @struct tagGisTextBoxWithCode
 * @brief �������ı�������GIS�ı�����Ϣ
 * @attention
 */
typedef struct tagGisTextBoxWithCode
{
    /** �ı������ */
    CHAR            szTextBoxCode[IMOS_RES_CODE_LEN];

    /** �ı���������Ϣ */
    GIS_TEXTBOX_S   stTextBoxInfo;

}GIS_TEXTBOX_WITH_CODE_S;


/**
 * @struct tagGisCamIcon
 * @brief GIS�����ͼ����Ϣ
 * @attention
 */
typedef struct tagGisCamIcon
{
    /** ��������� */
    CHAR        szCamCode[IMOS_DEVICE_CODE_LEN];

    /** �����ͼ��λ����Ϣ����, �ɲ��� */
    CHAR        szSiteDesc[IMOS_STRING_LEN_32];

}GIS_CAM_ICON_S;


/**
 * @struct tagGisCamIconWithCode
 * @brief ������ͼ������GIS�����ͼ����Ϣ
 * @attention
 */
typedef struct tagGisCamIconWithCode
{
    /** �����ͼ����� */
    CHAR            szCamIconCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR            szCamName[IMOS_NAME_LEN];

    /** �������״̬, ȡֵΪ#AS_CAMERA_STATUS_E */
    ULONG           ulCamState;

    /** ������Ķ���״̬, ȡֵΪ#DEV_EXT_STATUS_E */
    ULONG           ulCamExtState;

    /** ������ĸ澯���س̶�, ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    ULONG           ulCamAlarmLevel;

    /** ���������, ȡֵΪ#CAMERA_TYPE_E */
    ULONG           ulCameraType;

    /** ������Ƿ���������: 1 ��,0 �� */
    ULONG           ulIsForeign;

    /** �����ͼ��������Ϣ */
    GIS_CAM_ICON_S  stCamIconInfo;

    /** ������������豸������,ȡֵΪ#IMOS_TYPE_E */
    ULONG ulParentType;

}GIS_CAM_ICON_WITH_CODE_S;


/**
 * @struct tagGisAlarmSrcIcon
 * @brief GIS�澯Դͼ����Ϣ
 * @attention
 */
typedef struct tagGisAlarmSrcIcon
{
    /** �澯Դ���� */
    CHAR        szAlarmSrcCode[IMOS_DEVICE_CODE_LEN];

    /** �澯Դͼ��λ����Ϣ����, �ɲ��� */
    CHAR        szSiteDesc[IMOS_STRING_LEN_32];



}GIS_ALARM_SRC_ICON_S;


/**
 * @struct tagGisAlarmSrcIconWithCode
 * @brief �����澯Դͼ������GIS�澯Դͼ����Ϣ
 * @attention
 */
typedef struct tagGisAlarmSrcIconWithCode
{
    /** �澯Դͼ����� */
    CHAR                    szAlarmSrcIconCode[IMOS_RES_CODE_LEN];

    /** �澯Դ���� */
    CHAR                    szAlarmSrcName[IMOS_NAME_LEN];

    /** �澯���س̶�, ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    ULONG                   ulAlarmLevel;

    /** �澯Դͼ��������Ϣ */
    GIS_ALARM_SRC_ICON_S    stAlarmSrcIconInfo;

}GIS_ALARM_SRC_ICON_WITH_CODE_S;


/**
 * @struct tagGisAlarmDstIcon
 * @brief GIS�澯���ͼ����Ϣ
 * @attention
 */
typedef struct tagGisAlarmDstIcon
{
    /** �澯������� */
    CHAR        szAlarmDstCode[IMOS_DEVICE_CODE_LEN];

    /** �澯���ͼ��λ����Ϣ����, �ɲ��� */
    CHAR        szSiteDesc[IMOS_STRING_LEN_32];

}GIS_ALARM_DST_ICON_S;


/**
 * @struct tagGisAlarmDstIconWithCode
 * @brief �����澯���ͼ������GIS�澯���ͼ����Ϣ
 * @attention
 */
typedef struct tagGisAlarmDstIconWithCode
{
    /** �澯���ͼ����� */
    CHAR                    szAlarmDstIconCode[IMOS_RES_CODE_LEN];

    /** �澯������� */
    CHAR                    szAlarmDstName[IMOS_NAME_LEN];

    /** �澯���ͼ��������Ϣ */
    GIS_ALARM_DST_ICON_S    stAlarmDstIconInfo;

}GIS_ALARM_DST_ICON_WITH_CODE_S;


/***************************************************************************************************
͸��ͨ��
****************************************************************************************************/
/**
 * @struct tagTransparentChannel
 * @brief ͸��ͨ��������Ϣ
 * @attention
 */
typedef struct tagTransparentChannel
{
    /** ͸��ͨ������ */
    CHAR        szTransChnlName[IMOS_NAME_LEN];

    /** ͸��ͨ�����ͣ����ɸ���,ȡֵΪ#TL_CHANNEL_MODE_E */
    ULONG       ulTransChnlType;

    /** ͸��ͨ�����ݳ��ط�ʽ(UDP/TCP/SIP) **/
    ULONG       ulDataTransType;

}TRANSPARENT_CHANNEL_S;

/**
 * @struct tagTransparentSerial
 * @brief ͸��ͨ��������Ϣ
 * @attention
 */
typedef struct tagTransparentSerial
{

    /** �豸���� */
    CHAR        szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR        szDevName[IMOS_NAME_LEN];

    /** �豸����, ȡֵΪ#IMOS_TYPE_EC��#IMOS_TYPE_DC */
    ULONG       ulDevType;

    /** ��������, ȡֵ��1��ʼ, ���ֵΪ#IMOS_TRANSPARENT_SERIAL_NUM */
    ULONG       ulSerialIndex;

}TRANSPARENT_SERIAL_S;

/**
 * @struct tagTransparentIP
 * @brief ͸��ͨ����IP��ַ��Ϣ
 * @attention
 */
typedef struct tagTransparentIP
{
    /** ͨ��������(������),�ɸ��� */
    CHAR szChannelDesc[IMOS_DESC_LEN];

    /** IP��ַ */
    CHAR szTCIPAddress[IMOS_IPADDR_LEN];

    /** ͸��ͨ���˿ں� */
    ULONG ulTCPort;
}TRANSPARENT_IP_S;



/**
 * @struct tagTransparentInfo
 * @brief ͸��ͨ����Ϣ
 * @attention
 */
typedef struct tagTransparentInfo
{
    /** ͸��ͨ��������Ϣ */
    TRANSPARENT_CHANNEL_S   stTransparentChannel;

    /** ͸��ͨ�����������д��ڵ�ʵ����Ŀ, ���ȡֵΪ#IMOS_TRANSPARENT_SERIAL_NUM */
    ULONG                   ulSerialNum;

    /** ͸��ͨ���������� */
    TRANSPARENT_SERIAL_S    astTransparentSerial[IMOS_TRANSPARENT_SERIAL_NUM];

    /** ͸��ͨ����IP��ַ��Ϣ������IP��ַ��Ϣ��ʵ����Ŀ, ���ȡֵΪ#IMOS_TRANSPARENT_IP_NUM */
    ULONG                   ulIPNum;

    /** ͸��ͨ����IP��ַ��Ϣ���� */
    TRANSPARENT_IP_S        astTransparentIP[IMOS_TRANSPARENT_IP_NUM];

}TRANSPARENT_INFO_S;


/**
 * @struct tagTransparentQueryItem
 * @brief ͸��ͨ����Ϣ��(��ѯ͸��ͨ���б�ʱ����)
 * @attention
 */
typedef struct tagTransparentQueryItem
{
    /** ͸��ͨ������ */
    CHAR                    szTransChnlCode[IMOS_RES_CODE_LEN];

    /** ʹ��״̬, ȡֵΪ#TL_TRANS_CHANNEL_STATUS_E */
    ULONG                   ulEnabledFlag;

    /** ͸��ͨ����Ϣ */
    TRANSPARENT_INFO_S  stTransparentInfo;

}TRANSPARENT_QUERY_ITEM_S;




/***************************************************************************************************
��̨����
****************************************************************************************************/
/**
 * @struct tagPTZCtrlCommand
 * @brief ��̨����ָ��
 * @attention
 */
typedef struct  tagPTZCtrlCommand
{
    /** ��̨������������, ȡֵΪ#MW_PTZ_CMD_E */
    ULONG    ulPTZCmdID ;

    /** ��������Ĳ���ֵ1, ���ò�������Ϊ��̨ת��ʱȡֵ���ܳ���#MAX_PTZ_SPEED */
    ULONG   ulPTZCmdPara1;

    /** ��������Ĳ���ֵ2, ���ò�������Ϊ��̨ת��ʱȡֵ���ܳ���#MAX_PTZ_SPEED */
    ULONG   ulPTZCmdPara2;

    /** ��������Ĳ���ֵ3,�����ֶ� */
    ULONG   ulPTZCmdPara3;

}PTZ_CTRL_COMMAND_S;



/**************************************************************************************************************
�����ط� Record Retrieval, Edited by sunyifan (06966) 13.Mar.2009 Complete
************************************************************************************************************/

/**
* @struct tagRecQueryInfo
* @brief �ط���ؼ�����Ϣ���ݽṹ
*/
typedef struct tagRecQueryInfo
{
    /** ����ͷ����*/
    CHAR                szCamCode[IMOS_RES_CODE_LEN];

    /** ��������ʼ/����ʱ�� */
    TIME_SLICE_S    stQueryTimeSlice;

    /** �����ֶ� */
    CHAR szReserve[32];
}REC_QUERY_INFO_S;


/**
* @struct tagRecordFileInfo
* @brief ¼���ļ���Ϣ(��ѯ¼���ļ��б�ʱ����)
*/
typedef struct tagRecordFileInfo
{
    /** �ļ��� */
    CHAR szFileName[IMOS_FILE_NAME_LEN];

    /** �ļ���ʼʱ��, ����"%Y-%m-%d %H:%M:%S"��ʽ, �����޶�Ϊ24�ַ� */
    CHAR szStartTime[IMOS_TIME_LEN];

    /** �ļ�����ʱ��, ����"%Y-%m-%d %H:%M:%S"��ʽ, �����޶�Ϊ24�ַ� */
    CHAR szEndTime[IMOS_TIME_LEN];

    /** �ļ���С, Ŀǰ�ݲ�ʹ�� */
    ULONG ulSize;

    /** ������Ϣ, �ɲ��� */
    CHAR szSpec[IMOS_DESC_LEN];

}RECORD_FILE_INFO_S;

/**
* @struct tagRecordFileInfoV2
* @brief ¼���ļ���Ϣ(��ѯ¼���ļ��б�ʱ����)(V2)
*/
typedef struct tagRecordFileInfoV2
{
    /** �ļ��� */
    CHAR szFileName[IMOS_FILE_NAME_LEN_V2];

    /** �ļ���ʼʱ��, ����"%Y-%m-%d %H:%M:%S"��ʽ, �����޶�Ϊ24�ַ� */
    CHAR szStartTime[IMOS_TIME_LEN];

    /** �ļ�����ʱ��, ����"%Y-%m-%d %H:%M:%S"��ʽ, �����޶�Ϊ24�ַ� */
    CHAR szEndTime[IMOS_TIME_LEN];

    /** �ļ���С, Ŀǰ�ݲ�ʹ�� */
    ULONG ulSize;

    /** ������Ϣ, �ɲ��� */
    CHAR szSpec[IMOS_DESC_LEN];

}RECORD_FILE_INFO_V2_S;

/**
* @struct tagGetUrlInfo
* @brief ��ȡ¼���ļ�URL������ṹ
*/
typedef struct tagGetUrlInfo
{
    /** ��������� */
    CHAR            szCamCode[IMOS_RES_CODE_LEN];

    /** ¼���ļ��� */
    CHAR            szFileName[IMOS_FILE_NAME_LEN];

    /** ¼�����ʼ/����ʱ��, ���е�ʱ���ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S    stRecTimeSlice;

    /** �ͻ���IP��ַ */
    CHAR            szClientIp[IMOS_IPADDR_LEN];

}GET_URL_INFO_S;

/**
* @struct tagGetUrlInfoV2
* @brief ��ȡ¼���ļ�URL������ṹ(V2)
*/
typedef struct tagGetUrlInfoV2
{
    /** ��������� */
    CHAR            szCamCode[IMOS_RES_CODE_LEN];

    /** ¼���ļ��� */
    CHAR            szFileName[IMOS_FILE_NAME_LEN_V2];

    /** ¼�����ʼ/����ʱ��, ���е�ʱ���ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S    stRecTimeSlice;

    /** �ͻ���IP��ַ */
    CHAR            szClientIp[IMOS_IPADDR_LEN];

}GET_URL_INFO_V2_S;

/**
* @struct tagAddTagInfo
* @brief ¼���ǩ��Ϣ
*/
typedef struct tagAddTagInfo
{

    /** ��������� */
    CHAR                szCamCode[IMOS_RES_CODE_LEN];

    /** ��ǩ��ʱ���, ���еĿ�ʼʱ��ͽ���ʱ��ĸ�ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S    stTagTime;

    /** ��ǩ���� */
    CHAR                szTag[IMOS_RECORD_LABEL_LEN];

}ADD_TAG_INFO_S;


/**
* @struct tagTagQueryCondition
* @brief ��ǩ��������
*/
typedef struct tagTagQueryCondition
{
    /** ��������� */
    CHAR                szCamCode[IMOS_RES_CODE_LEN];

    /** ��������ʼ/����ʱ��, ʱ���ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S        stQueryTimeSlice;

    /** ��ѯ�����Ƿ������ǩ����, BOOL_TRUEΪ������ǩ����; BOOL_FALSEΪ��������ǩ���� */
    BOOL_T  bUseTag;

    /** ��ǩ���� */
    CHAR                szTag[IMOS_RECORD_LABEL_LEN];

}TAG_QUERY_CONDITION_S;


/**
* @struct tagTagQueryItem
* @brief ��ǩ��ѯ��Ϣ�б�Ԫ��Ϣ
*/
typedef struct tagTagQueryItem
{
    /** ��ǩ���� */
    CHAR            szTagCode[IMOS_RES_CODE_LEN];

    /** ��ǩʱ��� */
    CHAR            szTagTime[IMOS_TIME_LEN];

    /** ��ǩ�ַ���*/
    CHAR            szTag[IMOS_RECORD_LABEL_LEN];

}TAG_QUERY_ITEM_S;

/**
* @struct tagVODSeverIpAddr
* @brief �㲥��������IP��ַ�Ͷ˿�
*/
typedef struct tagVODSeverIpAddr
{
    /** RTSP������IP��ַ */
    CHAR   szServerIp[IMOS_IPADDR_LEN];

    /** RTSP�������˿� */
    USHORT  usServerPort;

    /** ����λ, �����ֽڶ���, ��ʵ�ʺ��� */
    CHAR    acReserved[2];
}VOD_SEVER_IPADDR_S;


/**
* @struct tagURLInfo
* @brief URL��Ϣ���ݽṹ
*/
typedef struct tagURLInfo
{
    /** URL��ַ*/
    CHAR                        szURL[IMOS_IE_URL_LEN];

    /** �㲥��������IP��ַ�Ͷ˿� */
    VOD_SEVER_IPADDR_S      stVodSeverIP;

    /** ���������� */
    CHAR szDecoderTag[IMOS_STRING_LEN_64];

}URL_INFO_S;



/***************************************************************************
CSҵ��
****************************************************************************/

/**
 * @struct tagMonitorUnit
 * @brief ʵʱ��ؼ�¼
 * @attention
 */
typedef struct tagMonitorUnit
{
    /** ��ع�ϵ�����Ķ���ҵ������: ʵʱ ���� �ƻ�����, ȡֵΪ#SRV_TYPE_E */
    ULONG ulTopSrvType;

    /** ���������� */
    CHAR  szMonitorCode[IMOS_RES_CODE_LEN];

    /** ����������#IMOS_TYPE_XP_WIN������, #IMOS_TYPE_MONITOR��������� */
    ULONG ulMonitorType;

    /** ����������, ��ΪXP����ʱ, �ṩ������� */
    CHAR  szMonitorName[IMOS_NAME_LEN];

    /** ��������������� */
    CHAR  szMonitorDomain[IMOS_DOMAIN_CODE_LEN];

    /** ��������� */
    CHAR  szCameraCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR  szCameraName[IMOS_NAME_LEN];

    /** ����������� */
    CHAR  szCameraDomain[IMOS_DOMAIN_CODE_LEN];

    /** �û����� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �û���¼IP��ַ */
    CHAR szUserIpAddress[IMOS_IPADDR_LEN];

    /** ��ع�ϵ״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulSrvState;
}MONITOR_UNIT_S;

/**
 * @struct tagSrvStateInfo
 * @brief ҵ��״̬��Ϣ
 * @attention ״̬��Χ#SRV_STATE_E
 */
typedef struct tagSrvStateInfo
{
    /** ʵ��ҵ��״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulMonitorState;

    /** ����ҵ��״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulSwitchState;

    /** �ƻ�����״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulPlanSwitchState;

    /** �����Խ�״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulVocTalkState;

    /** �����㲥״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulVocBrdState;

    /* added by z06806 for vodwall prj 20101215 begin */
    /** �ط���ǽ״̬��ȡֵΪ#SRV_STATE_E */
    ULONG ulVod2TVState;
    /* added by z06806 for vodwall prj 20101215 end */
}SRV_STATE_INFO_S;

/**
 * @struct tagResSrvStateInfo
 * @brief ��Դ��ҵ��״̬��Ϣ
 * @attention ״̬��Χ#SRV_STATE_E
 */
typedef struct tagResSrvStateInfo
{
    /** ����ʾ״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulSalvoState;

    /** ����Ѳ״̬, ȡֵΪ#SRV_STATE_E */
    ULONG ulGroupsalvoState;

}RES_SRV_STATE_INFO_S;

/**
 * @struct tagSwitchResUnit
 * @brief ������Դ��Ԫ��Ϣ
 * @attention
 */
typedef struct tagSwitchResUnit
{
    /** ��������� */
    CHAR    szCameraCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];

    /** ����ʱ��, ȡֵ��ΧΪ#MIN_TIME_SWITCH_INTERVAL~#MAX_TIME_SWITCH_INTERVAL */
    ULONG   ulSwitchTime;
}SWITCH_RES_UNIT_S;

/**
 * @struct tagSwitchResBase
 * @brief ������Դ������Ϣ
 * @attention
 */
typedef struct tagSwitchResBase
{
    /** ������Դ���� */
    CHAR    szSwitchResCode[IMOS_RES_CODE_LEN];

    /** ������Դ���� */
    CHAR       szSwitchResName[IMOS_NAME_LEN];

    /** �û����� */
    CHAR szUserName[IMOS_NAME_LEN];

    /** �û���¼IP��ַ */
    CHAR szUserIpAddress[IMOS_IPADDR_LEN];

}SWITCH_RES_BASE_S;

/**
 * @struct tagVocBrdUnit
 * @brief �����㲥��Ԫ��Ϣ
 * @attention
 */
typedef struct tagVocBrdUnit
{
    /** ��������� */
    CHAR        szCameraCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR        szCameraName[IMOS_NAME_LEN];

    /** �㲥״̬, ȡֵΪ#VOCBRD_STATUS_E */
    ULONG       ulState;
}VOC_BRD_UNIT_S;





/***************************************************************************************
��־����
��***********************************************************************************/

/**
 * @struct tagOperateLogInfo
 * @brief ������־��Ϣ
 * @attention
 */
typedef struct tagOperateLogInfo
{
    /** �������� */
    CHAR    szOperatorName[IMOS_NAME_LEN];

    /** ����ʱ�� */
    CHAR    szTime[IMOS_TIME_LEN];

    /** ������IP��ַ */
    CHAR    szIpAddress[IMOS_IPADDR_LEN];

    /** ��������, ȡֵΪ#OPERATE_TYPE_E */
    ULONG   ulOperateType;

    /** �������, ȡֵΪ�ò������صĴ����� */
    ULONG   ulResult;

    /** �������� */
    CHAR    szOperateDesc[IMOS_STRING_LEN_512];

    /** ����ʧ��ԭ�� */
    CHAR    szCause[IMOS_STRING_LEN_128];

    /** ҵ������, ȡֵΪ#SRV_TYPE_E */
    ULONG   ulOperationType;

    /** �������� */
    CHAR         szOperateObject[IMOS_STRING_LEN_512];
 }OPERATE_LOG_S;


/***************************************************************************
 ���͸�UI����Ϣ���ݽṹ
****************************************************************************/

/**
* @struct tagASStaPushUI
* @brief ���͸�UI���豸״̬��Ϣ���ݽṹ
*/
typedef struct tagASStaPushUI
{
    /** ���豸���� ��IMOS_TYPE_E,��sdk_def.h�ж��� */
    ULONG      ulParDevType;
    /** ���豸���� */
    CHAR    szParDevCode[IMOS_DEVICE_CODE_LEN];
    /** �豸״̬ 1-#IMOS_DEV_STATUS_ONLINE��2-#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж���
                 ������豸����Ϊ"�����",���豸״̬ȡֵΪ#AS_CAMERA_STATUS_E */
    ULONG   ulDevSta;
    /* �Ƿ������豸 */
    BOOL_T  bHasSubDev;
    /** ���豸���� */
    ULONG      ulSubDevType;
    /** ���豸���� */
    CHAR    szSubDevCode[IMOS_DEVICE_CODE_LEN];
}AS_STAPUSH_UI_S;


/**
* @struct tagASAlarmPushUI
* @brief ���͸�UI�ĸ澯��Ϣ���ݽṹ
*/
typedef struct tagASAlarmPushUI
{
    /** �澯�¼����� */
    CHAR  szAlarmEventCode[IMOS_RES_CODE_LEN];
    /** �澯Դ���� */
    CHAR  szAlarmSrcCode[IMOS_DEVICE_CODE_LEN];
    /** �澯Դ���� */
    CHAR  szAlarmSrcName[IMOS_NAME_LEN];
    /** ʹ�ܺ�����*/
    CHAR  szActiveName[IMOS_NAME_LEN];
    /** �澯���� AlARM_TYPE_E ��sdk_def.h�ж��� */
    ULONG  ulAlarmType;
    /** �澯���� ALARM_SEVERITY_LEVEL_E ��sdk_def.h�ж��� */
    ULONG  ulAlarmLevel;
    /** �澯����ʱ�� */
    CHAR   szAlarmTime[IMOS_TIME_LEN];
    /** �澯������Ϣ */
    CHAR  szAlarmDesc[IMOS_DESC_LEN];
}AS_ALARMPUSH_UI_S;

/* added by z06806 for vodwall prj 20101203 begin */
/**
* @struct tagCSVodwallReaveNotify
* @brief SOI ʵʱ����ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSVodwallReaveNotify
{
    ULONG   ulDeviceType;                             /* ����ռ���豸���� */
    CHAR    szMonitorCode[IMOS_CODE_LEN];             /* ���������� */
    CHAR    szMonitorName[IMOS_NAME_LEN];             /* ���������� */
    CHAR    szXPCode[IMOS_CODE_LEN];                  /* XP���� */
    CHAR    szXPName[IMOS_NAME_LEN];                  /* XP���� */
    CHAR    szDomainCode[IMOS_CODE_LEN];              /* ��������������� */
    CHAR    szDomainName[IMOS_NAME_LEN];              /* ���������������� */
    CHAR    szUserName[IMOS_NAME_LEN];                /* ��ռ�������û��� */
    CHAR    szUserLoginCode[IMOS_CODE_LEN];           /* ��ռ�����û���¼���� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];         /* ��ռ�����û��ĵ�¼IP */
}CS_VODWALL_REAVE_NOTIFY_S;
/* added by z06806 for vodwall prj 20101203 end */

/**
* @struct tagASShareDevBasicInfo
* @brief ���͸�UI�Ĺ����豸������Ϣ���ݽṹ
*/
typedef struct tagASShareDevBasicInfo
{
    /** �豸���� */
    CHAR    szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR    szDevName[IMOS_NAME_LEN];

    /** �豸״̬ 1���ߣ�2���� */
    ULONG             ulDevStatus;

    /** �豸����״̬  DEV_EXT_STATUS_E ��sdk_def.h�ж��� */
    ULONG             ulDevExtStatus;

    /** �豸���ͣ�ȡֵIMOS_TYPE_E */
    ULONG   ulDevType;

    /** �豸�����ͣ�Ŀǰֻ�������ȡCAMERA_TYPE_E ��sdk_def.h�ж��� */
    ULONG   ulDevSubType;

    /** ֧�ֵ�����Ŀ��ֻ�������������Ч��0:��Чֵ��1:������2:˫�� */
    ULONG  ulStreamNum;              /* Added for DoubleStream */
}AS_SHAREDEV_BASIC_INFO_S;


/**
* @struct tagASDevPushUI
* @brief ���͸�UI�Ĺ����豸��Ϣ���ݽṹ
*/
typedef struct tagASDevPushUI
{
    /** �豸��������� */
    CHAR   szDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸���������� */
    CHAR   szDomainName[IMOS_NAME_LEN];

    /** �������ͣ�ȡֵΪ AS_DEVSHARE_OPERATION_TYPE_E */
    ULONG                       ulOperType;

    /** �����豸������Ϣ */
    AS_SHAREDEV_BASIC_INFO_S    stDevContent;
}AS_DEVPUSH_UI_S;


/* Begin: Added by MultiDomain */
/**
* @struct tagASExdomainStaPushUI
* @brief ���͸�UI������״̬��Ϣ���ݽṹ
*/
typedef struct tagASExdomainStaPushUI
{
    /** ������� */
    CHAR   szDomainCode[IMOS_DOMAIN_CODE_LEN];

    /** �������� */
    CHAR   szDomainName[IMOS_NAME_LEN];

    /** ����ע��״̬��ָ�����������ע���Ƿ�ɹ� */
    ULONG                     ulActiveRegSta;

    /** ����ע��״̬��ָ����������ע���Ƿ�ɹ� */
    ULONG                     ulRecvRegSta;
}AS_EXDOMAIN_STAPUSH_UI_S;
/* End: Added by MultiDomain */


/**
* @struct tagCSMonitorReaveNotify
* @brief SOI ʵʱ����ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSMonitorReaveNotify
{
    ULONG   ulDeviceType;                             /* ����ռ���豸���� */
    CHAR    szMonitorCode[IMOS_DEVICE_CODE_LEN];      /* ������/XP������� */
    CHAR    szMonitorName[IMOS_NAME_LEN];             /* ������/XP�������� */
    CHAR    szCameraCode[IMOS_DEVICE_CODE_LEN];       /* ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];              /* ��������� */
    CHAR    szDomainCode[IMOS_DOMAIN_CODE_LEN];       /* ������/XP������������� */
    CHAR    szDomainName[IMOS_NAME_LEN];              /* ������/XP�������������� */
    CHAR    szUserName[IMOS_NAME_LEN];                /* ��ռ�������û��� */
    CHAR    szUserLoginCode[IMOS_RES_CODE_LEN];       /* ��ռ�����û���¼���� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];         /* ��ռ�����û��ĵ�¼IP */
}CS_MONITOR_REAVE_NOTIFY_S;


/**
* @struct tagCSSwitchReaveNotify
* @brief SOI ���б���ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSSwitchReaveNotify
{
    ULONG   ulDeviceType;                             /* ����ռ���豸���� */
    CHAR    szMonitorCode[IMOS_DEVICE_CODE_LEN];      /* ������/XP������� */
    CHAR    szMonitorName[IMOS_NAME_LEN];             /* ������/XP�������� */
    CHAR    szSwitchResCode[IMOS_RES_CODE_LEN];       /* ������Դ���� */
    CHAR    szSwitchResName[IMOS_RES_CODE_LEN];       /* ������Դ���� */
    CHAR    szDomainCode[IMOS_DOMAIN_CODE_LEN];       /* ������/XP������������� */
    CHAR    szDomainName[IMOS_NAME_LEN];              /* ������/XP�������������� */
    CHAR    szUserName[IMOS_NAME_LEN];                /* ��ռ�������û��� */
    CHAR    szUserLoginCode[IMOS_RES_CODE_LEN];       /* ��ռ�����û���¼���� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];         /* ��ռ�����û��ĵ�¼IP */
}CS_SWITCH_REAVE_NOTIFY_S;

/**
* @struct tagCSVocSrvReaveNotify
* @brief SOI ����ҵ����ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSVocSrvReaveNotify
{
    CHAR    szUserName[IMOS_NAME_LEN];                /* �û��� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];         /* ��ռ�����û��ĵ�¼IP */
    CHAR    szCameraCode[IMOS_DEVICE_CODE_LEN];       /* ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];              /* ������� */
    CHAR    szDomainCode[IMOS_DOMAIN_CODE_LEN];       /* ����� */
    CHAR    szDomainName[IMOS_NAME_LEN];              /* ���� */
    ULONG   ulPreSrvType;                             /* ��ռǰ�����ͣ��Խ���㲥����ö��ֵΪ:SRV_TYPE_E */
    ULONG   ulCurSrvType;                             /* ��ռ������ͣ��Խ���㲥����ö��ֵΪ:SRV_TYPE_E */
}CS_VOCSRV_REAVE_NOTIFY_S;


/**
* @struct tagCSPtzReaveNotify
* @brief SOI ��̨ҵ����ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSPtzReaveNotify
{
    CHAR    szUserName[IMOS_NAME_LEN];                  /* �û����� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];           /* ��ռ�����û��ĵ�¼IP */
    CHAR    szCameraCode[IMOS_DEVICE_CODE_LEN];         /* ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];                /* ��������� */
    ULONG   ulNotifyType;                               /* ֪ͨ���� ��Ӧö��:PTZ_NOTIFY_UI_EVENT_TYPE */
}CS_PTZ_REAVE_NOTIFY_S;

/**
* @struct tagCSNotifyUITrbEventProc
* @brief ֪ͨUI���ϴ����¼�����
* @attention ʵ�������С�����ҵ���ͷŸ��ýṹ�����������ֻ�ṩ������ʹ��
*/
typedef struct tagCSNotifyUITrbEventProc
{
    /** XP������� */
    CHAR szXPWinCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� ����ҵ��ʹ�� */
    CHAR szCameraCode[IMOS_DEVICE_CODE_LEN];

    /** ҵ������ */
    ULONG ulServiceType;

}CS_NOTIFY_UI_TRB_EVENT_PROC_S;

/**
* @struct tagCSNotifyCameraInfo
* @brief ҵ������Ϣ֪ͨ�������Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSNotifyCameraInfo
{
    /** ��������� */
    CHAR  szCameraCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR  szCameraName[IMOS_NAME_LEN];

    /** ��������� */
    ULONG ulCameraType;

}CS_NOTIFY_CAMERA_INFO_S;

/**
* @struct tagCSNotifySwitchResInfo
* @brief ҵ������Ϣ֪ͨ������Դ��Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSNotifySwitchResInfo
{
    /** ������Դ���� */
    CHAR  szSwitchResCode[IMOS_RES_CODE_LEN];

    /** ������Դ���� */
    CHAR szSwitchResName[IMOS_NAME_LEN];

}CS_NOTIFY_SWITCH_RES_INFO_S;

/**
* @struct tagCSNotifyReplayActionInfo
* @brief ҵ������Ϣ֪ͨ������ǰ¼��ṹ
* @attention ��
*/
typedef struct tagCSNotifyReplayActionInfo
{
    /** �Ƿ����þ�ǰ¼��ȡֵ0����1���� */
    ULONG  ulIsBefAlarmReplay;

    /** ��ǰ¼��ʱ������ȡֵ��λ�� */
    ULONG  ulBefAlarmRepInterval;

     /** �澯ʱ�� ��ʽΪ "YYYY-MM-DD hh:mm:ss" */
    CHAR   szAlarmTime[IMOS_TIME_LEN];

    /** ������ǰ¼���XP������� */
    CHAR   szRepXPWinCode[IMOS_CODE_LEN];

}CS_NOTIFY_REPLAY_ACTION_INFO_S;


/**
* @struct tagCSNotifyScreenActionInfo
* @brief ҵ������Ϣ֪ͨ����ץ����Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSNotifyScreenActionInfo
{
    /** �Ƿ�ץ��, ȡֵΪ0��1 */
    ULONG    ulSnatchImage;

    /** ץ�ļ����ȡֵ��λ���� */
    ULONG    ulInterval;

    /** ץ������ */
    ULONG    ulSnatchCounter;
    /* ������ǰ¼��ṹ */
    CS_NOTIFY_REPLAY_ACTION_INFO_S  stReplayActInfo;

    /* �����ֶ� */
    CHAR     szParam[IMOS_IDLE_LEN];

}CS_NOTIFY_SCREEN_ACTION_INFO_S;


/**
* @struct tagCSNotifyUISrvSetup
* @brief ֪ͨUIҵ������ʵʱ��أ����У�
* @attention ҵ��ָ����澯�������ýṹ���豸��Ϣ�ṹ����ʵ���ָ���������Դ��Ϣ�������лָ�
*/
typedef struct tagCSNotifyUISrvSetup
{
    /** XP������� */
    CHAR                        szXPWinCode[IMOS_DEVICE_CODE_LEN];

    /** �������Ϣ */
    CS_NOTIFY_CAMERA_INFO_S     stCameraInfo;

    /** ������Դ��Ϣ */
    CS_NOTIFY_SWITCH_RES_INFO_S stSwitchResInfo;

    /** ҵ������ */
    ULONG                       ulServiceType;

}CS_NOTIFY_UI_SRV_SETUP_S;

/***************************************************************************************
LICENCE
��***********************************************************************************/

/**
* @struct tagContactInfo
* @brief ��ϵ����Ϣ�ṹ
* @attention ��
*/
typedef struct tagContactInfo
{
    /** ��ϵ������ */
    CHAR szName[IMOS_NAME_LEN];

    /** ��ϵ�����ڹ�˾ */
    CHAR szCompany[IMOS_NAME_LEN];

    /** ��ϵ��email��ַ */
    CHAR szEmail[IMOS_MAIL_LEN];

    /** ��ϵ�˵绰 */
    CHAR szPhone[IMOS_PHONE_LEN];

    /** ��ϵ���ʱ� */
    CHAR szZipcode[IMOS_MAIL_CODE_LEN];

    /** ��ϵ�˵�ַ */
    CHAR szAddress[IMOS_CONTACT_ADDRESS_LEN];

} CONTACTINFO_S;

/**
* @struct tagClientInformation
* @brief �ͻ���Ϣ�ṹ
* @attention ��
*/
typedef struct tagClientInformation
{
    /** �ͻ����ƣ�һ��ͬ��˾���� */
    CHAR szClientName[IMOS_NAME_LEN];

    /** ���� */
    CHAR szCountry[IMOS_NAME_LEN];

    /** ʡ */
    CHAR szProvince[IMOS_NAME_LEN];

    /** ���� */
    CHAR szCity[IMOS_NAME_LEN];

    /** ��˾ */
    CHAR szCompany[IMOS_NAME_LEN];

    /** �ʱ� */
    CHAR szZipcode[IMOS_MAIL_CODE_LEN];

    /** ��ַ */
    CHAR szAddress[IMOS_CONTACT_ADDRESS_LEN];

    /** email��ַ */
    CHAR szEmail[IMOS_MAIL_LEN];

    /** �绰 */
    CHAR szPhone[IMOS_PHONE_LEN];

    /** ��ϵ����Ϣ */
    CONTACTINFO_S stContactInfo;

} CLIENTINFO_S;

/**
* @struct tagLicenseAuthorityInfo
* @brief ҵ������Ϣ(��ѯLicenseҵ������Ϣ���б�ʱ����)
* @attention ��
*/
typedef struct tagLicenseAuthorityInfo
{
    /** ҵ������, ȡֵΪ#LICENSE_TYPE_E */
    ULONG ulServiceType;

    /** ��Ȩ���, ���ݲ�ѯʱ�ķ��ؽ�� */
    ULONG ulServiceValue;

} LICENSE_AUTHORITYINFO_S;


/**
* @struct tagTimeLimit
* @brief ��Ч������Ϣ(��ѯLicenseʱ����Ϣʱ����)
* @attention ��
*/
typedef struct tagTimeLimit
{
    /** License�������� */
    CHAR szCreateDay[IMOS_TIME_LEN];

    /** License����Ч���� */
    ULONG ulTotalDay;

    /** ��ʹ������ */
    ULONG ulUsedDay;

}TIMELIMIT_S;


/***************************************************************************************************
��������
****************************************************************************************************/
/**
* @struct tagUpgTaskInfo
* @brief  ����������Ϣ
* @attention ��
*/
typedef struct tagUpgTaskInfo
{
    /** �Ƿ�����ִ��, BOOL_TRUEΪ����ִ��; BOOL_FALSEΪ������ִ�� */
    BOOL_T bIsImmediateExe;

    /** ���������� */
    CHAR szUpgJobName[IMOS_NAME_LEN];

    /** ������������, �ɲ��� */
    CHAR szUpgJobDesc[IMOS_DESC_LEN];

    /** �������񴴽���, Ϊ��ǰ�����û��� */
    CHAR szUpgJobCreator[IMOS_NAME_LEN];

    /** �����������ʱ�䣨����ʱ���룩, ��ʽΪ"YYYY-MM-DD HH:MM:SS" */
    CHAR szUpgJobSchdTime[IMOS_TIME_LEN];

    /** �����������ʱ��, ��ʽΪ"YYYY-MM-DD HH:MM:SS", ��ֵ�ڲ�ѯ����������Ϣʱ��Ч, �����������д */
    CHAR szUpgJobFinishTime[IMOS_TIME_LEN];

    /** ���������ƶ�ʱ��, �������������ʱ�򲻱���д */
    CHAR szUpgJobEstablishTime[IMOS_TIME_LEN];
} UPG_TASK_INFO_S;


/**
* @struct  tagUpgDevInfo
* @brief �����豸��Ϣ�ṹ
* @attention ��
*/
typedef struct tagUpgDevInfo
{
    /** �豸���� */
    CHAR szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR szDevName[IMOS_NAME_LEN];

    /** �豸����ǰ�汾�� */
    CHAR szDevOldVersion[IMOS_STRING_LEN_256];

    /** �豸�������汾�� */
    CHAR szDevUpgVersion[IMOS_STRING_LEN_256];

    /** �豸����״̬����ѯʱ��Ч��ȡֵΪ#MM_JOBDEVSTATUS_E */
    ULONG ulDevUpgStatus;

    /** �豸�����������ѯʱ��Ч��ȡֵΪ#MM_JOBDEVRESULT_E */
    ULONG ulUpgResult;

    /** �豸������չ�������ӳ���������������ѯʱ��Ч��ȡֵΪ#MM_UPGDEVEXTRESULT_E */
    ULONG ulDevUpgExtResult;
} UPG_DEV_INFO_S;


/**
* @struct tagUpgTaskQueryItem
* @brief  ����������Ϣ��(��ѯ���������б�ʱ����)
* @attention ��
*/
typedef struct tagUpgTaskQueryItem
{
    /** ����������� */
    CHAR szUpgTaskCode[IMOS_RES_CODE_LEN];

    /** ���������� */
    CHAR szUpgJobName[IMOS_NAME_LEN];

    /** �������񴴽��� */
    CHAR szUpgJobCreator[IMOS_NAME_LEN];

    /** �����������ʱ�䣨����ʱ���룩����ʽΪ"YYYY-MM-DD HH:MM:SS" */
    CHAR szUpgJobSchdTime[IMOS_TIME_LEN];

    /** �����������ʱ�� */
    CHAR szUpgJobFinishTime[IMOS_TIME_LEN];

    /** ���������ƶ�ʱ�� */
    CHAR szUpgJobEstablishTime[IMOS_TIME_LEN];

    /** ��������״̬, ȡֵΪ#MM_JOBDEVSTATUS_E  */
    ULONG ulUpgJobStatus;

    /** ����������, ȡֵΪ#MM_JOBDEVRESULT_E*/
    ULONG ulUpgJobResult;
} UPG_TASK_QUERY_ITEM_S;


/**
* @struct tagUpgAdviceInfo
* @brief ����һ���豸����������Ϣ�ṹ
* @attention ��
*/
typedef struct tagUpgAdviceInfo
{
    /** �豸���� */
    CHAR szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR szDevName[IMOS_NAME_LEN];

    /** �豸�������� */
    CHAR szDevTypeDesc[IMOS_DESC_LEN];

    /** �豸��ǰ�汾�� */
    CHAR szDevCurVersion[IMOS_STRING_LEN_256];

    /** �豸�������汾�� */
    CHAR szDevUpgVersion[IMOS_STRING_LEN_256];

    /** �豸��������
    * - 0 UPG_ADVICE_UNINITIAL �������������������
    * - 1 UPG_ADVICE_NEED      ��Ҫ����
    * - 2 UPG_ADVICE_NONEED    ��������*/
    ULONG  ulUpgAdvice;

    /** �豸����״̬ */
    ULONG ulDevOnlineStatus;
} UPG_ADVICE_INFO_S;

/*******************************************************************************
��Ϣͳ��
*******************************************************************************/

/**
* @struct tagSrvStatInfo
* @brief ҵ����Ϣ����ͳ�ƽṹ
* @attention ��
*/
typedef struct tagSrvStatInfo
{
    /** ��ǰʵ��ҵ��������ͳ�� */
    ULONG    ulMonitorNum;

    /** ��ǰ����ҵ��������ͳ�� */
    ULONG    ulSwitchNum;

    /** ��ǰ����ҵ��������ͳ�� */
    ULONG    ulVoiceNum;

    /** ��ǰ�����豸����ͳ�� */
    ULONG    ulDevOnlineNum;

    /** ��ǰ�����û�����ͳ�� */
    ULONG    ulUserOnlineNum;

    /** ��ǰ�ֶ��洢ҵ������ͳ�� */
    ULONG    ulManualRecordNum;
} SRV_STAT_INFO_S;

/**
 * @struct tagAlarmOutputDevQueryItem
 * @brief �澯����豸��ѯ��Ϣ�б�Ԫ��Ϣ
 * @attention
 */
typedef struct tagAlarmOutputDevQueryItem
{

    /** �澯����豸������ */
    CHAR        szAlarmOutputDevName[IMOS_NAME_LEN];

    /** �澯����豸�ı��� */
    CHAR        szAlarmOutputDevCode[IMOS_RES_CODE_LEN];

    /** �澯����豸������,ö������Ϊ:ALARM_OUTPUT_DEV_TYPE_E */
    ULONG       ulAlarmOutputDevType;

    /** �澯����豸������֯ */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ������֯���� */
    CHAR        szOrgName[IMOS_NAME_LEN];

}ALARM_OUTPUT_DEV_QUERY_ITEM_S;

/**
 * @struct tagAlarmOutputDev
 * @brief �澯����豸��Ϣ
 * @attention
 */
typedef struct tagAlarmOutputDev
{

    /** �澯����豸������ */
    CHAR        szAlarmOutputDevName[IMOS_NAME_LEN];

    /** �澯����豸������,ö������Ϊ:ALARM_OUTPUT_DEV_TYPE_E */
    ULONG       ulAlarmOutputDevType;

    /**�澯����豸��Ҫ�󶨵�ͨ�������豸�ı��� */
    CHAR        szDevCode[IMOS_DEVICE_CODE_LEN];

    /**�澯����豸��Ҫ�󶨵�ͨ�������豸������ */
    CHAR        szDevName[IMOS_NAME_LEN];

    /** �澯����豸��Ҫ�󶨵�ͨ������ */
    ULONG       ulChannelIndex;
}ALARM_OUTPUT_DEV_S;


/**
* @struct tagSDKCmdInfo
* @brief ������ִ�в���
* @attention ��
*/
typedef struct tagSDKCmdInfo
{
    /** ����ID */
    ULONG ulCmdId;

    /** ��������б�ÿ���������128�ֽڣ�Ŀǰ֧�ֵ�6���������м�û�еĲ�����Ϊ�մ����� */
    CHAR szParamList[IMOS_MAX_CMD_PARAM_NUM][IMOS_MAX_CMD_PARAM_LEN];

}SDK_CMD_INFO_S;

/**
* @struct tagCSNotifyUIMediaParamChangeProc
* @brief ֪ͨUIý������ı䴦��ṹ
* @attention XPWinCode����UI�ͷ�ʹ��
*/
typedef struct tagCSNotifyUIMediaParamChangeProc
{
    /** ý��ı����� 0:��/�鲥�޸� 1:MSѡ�����(����Ӧ/ָ��)�޸� 2:MSѡ���޸� 3: �鲥����(��ַ/�˿�)�޸� */
    ULONG ulMediaParamChangeType;

    /** �豸���� ����ֵ�������#IMOS_TYPE_E */
    ULONG ulDevType;

    /** �ı�ý��������豸����(������DC/EC/XP/CAMERA����) */
    CHAR szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR szDevName[IMOS_NAME_LEN];

    /* XP��������� */
    ULONG ulXPWinCount;

    /** XP������� */
    CHAR aszXPWinCode[IMOS_XP_WIN_MAX_NUM][IMOS_DEVICE_CODE_LEN];
}CS_NOTIFY_UI_MEDIA_PARAM_CHANGE_PROC_S;

/*******************************************************************************
����ǽ
*******************************************************************************/
/**
* @struct tagScreenUnitInfo
* @brief ��������Ԫ��Ϣ
* @attention
*/
typedef struct tagScreenUnitInfo
{
    /** ���������� */
    CHAR szScreenName[IMOS_NAME_LEN];

    /** ���������� */
    CHAR szScreenCode[IMOS_CODE_LEN];

    /** ״̬��Ϣ:����Ϊ#IMOS_DEV_STATUS_ONLINE������Ϊ#IMOS_DEV_STATUS_OFFLINE������޸�ʱ״̬��Ϣ������д */
    ULONG ulStatus;

    /** ������������Ŀ����ֵ���ڲ�ѯʱ��Ч */
    ULONG ulSplitScreenNum;

    /** ��չ״̬��Ϣ:Ŀǰ�ݲ�ʹ�ã�Ĭ��Ϊ0 */
    ULONG ulExtStatus;
}SCREEN_UNIT_INFO_S;


/**
* @struct tagScreenLayoutInfo
* @brief ������������Ϣ
* @attention
*/
typedef struct tagScreenLayoutInfo
{
    /** ÿ���������ĵ�Ԫ��Ϣ */
    SCREEN_UNIT_INFO_S stScreenUnitInfo;

    /** ������������Ϣ */
    AREA_SCOPE_S stScreenLayout;
}SCREEN_LAYOUT_INFO_S;

/**
* @struct tagTVWallInfo
* @brief ����ǽ��Ϣ
* @attention ����ǽ������Ϣ�Լ�������������������Ϣ
*/
typedef struct tagTVWallInfo
{
    /** ����ǽ���� */
    CHAR szTVWallName[IMOS_NAME_LEN];

    /** ����ǽ���룬���ӵ���ǽʱ��дΪ����ƽ̨�Լ����ɣ����ӵ���ǽ�ɹ��󷵻�; ����д������д�ı������� */
    CHAR szTVWallCode[IMOS_CODE_LEN];

    /** ������֯����룬�����޸�ʱ��Ҫ��д��ƽ̨���ز������ֵ */
    CHAR szOrgCode[IMOS_CODE_LEN];

    /** ����ǽ�����ļ��� */
    CHAR szBackGroundFileName[IMOS_FILE_NAME_LEN];

    /** ���ֱ��룬������д����ѯ����ǽʱ��ƽ̨���� */
    CHAR szLayoutCode[IMOS_CODE_LEN];

    /** ����ǽ�����ļ�����ʵ������, ȡֵ���ֵΪ#IMOS_MONITOR_MAXNUM_PER_WALL */
    ULONG ulScreenCount;

    /** ����ǽ�ֱ���, ȡֵΪ#TW_RESOLUTION_E */
    ULONG ulResolution;

    /** ������������Ϣ���� */
    SCREEN_LAYOUT_INFO_S astTVWallLayoutList[IMOS_MONITOR_MAXNUM_PER_WALL];
}TV_WALL_INFO_S;

/**
* @struct tagTVWallQueryItem
* @brief ����ǽ��Ϣ��(��ѯ����ǽ�б�ʱ����)
* @attention
*/
typedef struct tagTVWallQueryItem
{
    /** ����ǽ���� */
    CHAR szTVWallName[IMOS_NAME_LEN];

    /** ����ǽ���룬�����ʱ��дΪ������ƽ̨�Լ����ɵı���; ����д���ѯ���������ʱ��д���� */
    CHAR szTVWallCode[IMOS_CODE_LEN];

    /** ������֯�����,���ڲ�ѯ���Լ������µĵ���ǽ�б�ƽ̨���ز������ֵ */
    CHAR szOrgCode[IMOS_CODE_LEN];

    /** ����ǽ�ֱ���, ȡֵΪ#TW_RESOLUTION_E */
    ULONG ulResolution;

    /** ����ǽ�����ļ��������� */
    ULONG ulScreenCount;
}TV_WALL_QUERY_ITEM_S;

/**
* @struct tagDevStatusStatistic
* @brief �豸״̬ͳ��(��ѯ�豸����ͳ�Ƶ�ʱ�򷵻�)
* @attention
*/
typedef struct tagDevStatusStatistic
{
    /** �豸���ͣ�ȡֵ�μ�#IMOS_TYPE_E */
    ULONG  ulDevType;

    /** �������� */
    ULONG  ulOnlineNum;

    /** �������� */
    ULONG ulFaultNum;

    /** �������� */
    ULONG ulOfflineNum;
}DEV_STATUS_STATISTIC_S;

/**
* @struct tagDevBaseInfo
* @brief �����豸������Ϣ
* @attention
*/
typedef struct tagDevBaseInfo
{
    /** �豸���� */
    CHAR  szDevCode[IMOS_DEVICE_CODE_LEN];

    /** �豸���� */
    CHAR  szDevName[IMOS_NAME_LEN];

    /** �豸���ͣ�ȡֵΪ#IMOS_TYPE_DM, #IMOS_TYPE_MS, #IMOS_TYPE_VX500, #IMOS_TYPE_MONITOR, #IMOS_TYPE_EC, #IMOS_TYPE_DC, #IMOS_TYPE_CAMERA */
    ULONG  ulDevType;

    /** �豸������, �������ȡֵ��#CAMERA_TYPE_E; �Լ�����������;
    �������豸(EC/DC/MS/DM/VX500)ȡֵ��#IMOS_DEVICE_TYPE_E. */
    ULONG  ulDevSubType;

    /** �豸״̬��ȡֵΪ#IMOS_DEV_FAULT_STATUS_E */
    ULONG  ulDevStatus;

    /** �豸IP��ַ, �������豸(EC/DC/MS/DM/VX500)��Ч */
    CHAR szIPAddr[IMOS_IPADDR_LEN];

    /** ����� */
    CHAR  szDomainCode[IMOS_CODE_LEN];
}DEV_BASIC_INFO_S;


/**
* @struct tagCSBackupFinishInfo
* @brief �������֪ͨ
*/
typedef struct tagCSBackupFinishInfo
{
    /** ��������,ȡֵ��Χ��#BP_BACKUPTYPE_E */
    ULONG   ulBackupType;

    /** ���ݽ��: 0Ϊ���ݳɹ�; ����Ϊ����ʧ�ܡ�*/
    ULONG   ulResult;
}CS_BACKUP_FINISH_INFO_S;


/**
 * @struct tagGeneralDevInfo
 * @brief ͨ���豸������Ϣ
 * @attention
 */
typedef struct tagGeneralDevInfo
{
    /** ͨ���豸���룬���û����룬��ƽ̨���� */
    CHAR  szGeneralDevCode[IMOS_DEVICE_CODE_LEN];

    /** ͨ���豸���� */
    CHAR  szGeneralDevName[IMOS_NAME_LEN];

     /** �豸���� */
    ULONG ulDevType;

    /** �豸������ */
    ULONG ulDevSubType;

    /** �豸�������� */
    CHAR szDevTypeDesc[IMOS_DESC_LEN];

    /** ��ַ���ͣ�1-IPV4 2-IPV6 */
    ULONG ulAddrType;

    /** ��ַ */
    CHAR  szAddress[IMOS_IPADDR_LEN];

    /** �˿� */
    ULONG ulAddrPort;

    /** ������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �豸���� , ����ֻ����дΪ���ַ���*/
    CHAR  szDevDesc[IMOS_DESC_LEN];

    /** �豸��չ��Ϣ */
    CHAR  szDevReserved[IMOS_RESERVED_LEN];
}GENERAL_DEV_INFO_S;

/**
 * @struct tagInterconnectInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagInterconnectInfo
{
    /** ������Ϣ������Դ���� */
    CHAR  szResCode[IMOS_RES_CODE_LEN];

    /** ������Ϣ���ͣ�ȡֵΪ#INTERCONNECT_INFO_TYPE_E */
    ULONG       ulInterInfoType;

    /** ����Э�����ͣ�ȡֵΪ#INTERDOMAIN_PROTOCOL_TYPE_E */
    ULONG      ulProtocolType;

    /** ��ȼ�, ĿǰȡֵΪ1~7 */
    ULONG    ulDomainLevel;

    /** ��������Դ���� */
    CHAR  szInterResCode[IMOS_RES_CODE_LEN];

    /** �������û����� */
    CHAR  szInterUserCode[IMOS_USER_CODE_LEN];

 }INTERCONNECT_INFO_S;


/**
 * @struct tagResourceGroupUnit
 * @brief ��Դ�鵥Ԫ��Ϣ
 * @attention
 */
typedef struct tagResourceGroupUnit
{
    /** ��Դ���� */
    CHAR        szResCode[IMOS_RES_CODE_LEN];

    /** ��Դ���� */
    CHAR        szResName[IMOS_NAME_LEN];

    /** �����ֶ� */
    CHAR szReserve[128];
}RESOURCE_GROUP_UNIT_S;

/**
 * @struct tagResourceGroupInfo
 * @brief ��Դ����Ϣ
 * @attention
 */
typedef struct tagResourceGroupInfo
{
    /** ��Դ����� */
    CHAR        szResGroupCode[IMOS_RES_CODE_LEN];

    /** ��Դ������ */
    CHAR        szResGroupName[IMOS_NAME_LEN];

    /** ��Դ������ */
    ULONG       ulGroupResType;

    /** ���ڵ���Դ��Ŀ */
    ULONG       ulResNum;

}RESOURCE_GROUP_INFO_S;


/**
 * @struct tagLayoutInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagLayoutInfo
{
    /** ���ֱ��� */
    CHAR        szLayoutCode[IMOS_CODE_LEN];

    /** �������� */
    CHAR        szLayoutName[IMOS_NAME_LEN];

    /** �����������ɲ��� */
    CHAR        szLayoutDes[IMOS_DESC_LEN];

    /** �������ͣ�ȡֵΪ#LAYOUT_TYPE_E */
    ULONG       ulLayoutType;

    /** ��Ļ����ʱΪ��Ļ������������������ʱΪ��������Ŀ */
    ULONG       ulScreenCount;

    /** ������������Ϣ���飬����������ʱʹ�� */
    AREA_SCOPE_S astScreenLayoutList[IMOS_DISPLAY_MAXNUM_PER_LAYOUT];
}LAYOUT_INFO_S;

/**
* @struct tagLayoutInfoQueryItem
* @brief ������Ϣ��(��ѯ�����б�ʱ����)
* @attention
*/
typedef struct tagLayoutInfoQueryItem
{
    /** �������� */
    CHAR szLayoutName[IMOS_NAME_LEN];

    /** ���ֱ��� */
    CHAR szLayoutCode[IMOS_CODE_LEN];

    /** �������ͣ�ȡֵΪ#LAYOUT_TYPE_E */
    ULONG       ulLayoutType;
}LAYOUT_INFO_QUERY_ITEM_S;


/**
 * @struct tagSalvoUnit
 * @brief ����ʾ��Ԫ��Ϣ
 * @attention
 */
typedef struct tagSalvoUnit
{
    /** ��������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** ����1:����������ʾ������������� */
    CHAR        szParam1[IMOS_STRING_LEN_128];

    /** ����2:����������ʾ������������� */
    CHAR        szParam2[IMOS_STRING_LEN_128];

    /** ����3:��Ļ����ʾ������Ļ���, ȡֵΪ1 */
    ULONG       ulParam3;

    /** ����4:��Ļ����ʾ������� */
    ULONG       ulParam4;

    /** ����5:���� */
    ULONG       ulParam5;

    /** �������ķ�����Ŀ�����ڲ�ѯʱ��Ч */
    ULONG       ulSplitScreenNum;
}SALVO_UNIT_S;

/**
 * @struct tagSalvoInfo
 * @brief ����ʾ��Ϣ
 * @attention
 */
typedef struct tagSalvoInfo
{
    /** ����ʾ���ͣ�ȡֵΪ#SALVO_TYPE_E */
    ULONG       ulSalvoType;

    /** ����ʾ���� */
    CHAR        szSalvoCode[IMOS_CODE_LEN];

    /** ����ʾ���� */
    CHAR        szSalvoName[IMOS_NAME_LEN];

    /** ���ֱ��룬��ѡ */
    CHAR        szLayoutCode[IMOS_CODE_LEN];

    /** ����ʾ��������ʾ��Ԫʵ������, ���ֵΪ#IMOS_DISPLAY_MAXNUM_PER_SALVO */
    ULONG       ulSalvoUnitNum;

    /** ����ʾ��Ԫ���� */
    SALVO_UNIT_S astSalvoUnitList[IMOS_DISPLAY_MAXNUM_PER_SALVO];
}SALVO_INFO_S;


/**
* @struct tagSalvoQueryItem
* @brief ����ʾ��Ϣ��(��ѯ����ʾ�б�ʱ����)
* @attention
*/
typedef struct tagSalvoQueryItem
{
    /** ����ʾ���� */
    CHAR szSalvoName[IMOS_NAME_LEN];

    /** ����ʾ���룬�����ʱ��дΪ������ƽ̨�Լ����ɵı���; ����д���ѯ���������ʱ��д���� */
    CHAR szSalvoCode[IMOS_CODE_LEN];

    /** ������֯�����,���ڲ�ѯ���Լ������µ�����ʾ�б�ƽ̨���ز������ֵ */
    CHAR szOrgCode[IMOS_CODE_LEN];

    /** ����ʾ��������ʾ��Ԫ���� */
    ULONG ulSalvoUnitNum;

    /** ����ʾ���ͣ�ȡֵΪ#SALVO_TYPE_E */
    ULONG       ulSalvoType;

}SALVO_QUERY_ITEM_S;


/**
 * @struct tagCommonSwitchUnit
 * @brief ͨ��������Դ�е���Դ��Ϣ
 * @attention
 */
typedef struct tagCommonSwitchUnit
{
    /** ��Դ���� */
    CHAR        szResCode[IMOS_RES_CODE_LEN];

    /** ��Դ���� */
    CHAR        szResName[IMOS_NAME_LEN];

    /** ��Դ���� */
    ULONG       ulResType;

    /** ��� */
    ULONG       ulSequence;

     /** �л�ʱ�� */
    ULONG       ulInterval;
}COMMON_SWITCH_UNIT_S;

/**
 * @struct tagCommonSwitchResource
 * @brief ͨ��������Դ��Ϣ
 * @attention
 */
typedef struct tagCommonSwitchResource
{
    /** ͨ���������ͣ�ȡֵΪ#COMMON_SWITCH_TYPE_E */
    ULONG                   ulSwitchType;

    /** ͨ�����������ͣ���������Ѳ��������Ѳ���ͣ�ȡֵΪ#GROUP_SWITCH_TYPE_E */
    ULONG                   ulSwitchSubType;

    /** ���л�����Ϣ */
    SWITCH_BASE_S           stSwitchBaseInfo;

    /** ͨ��������Դ������������Դ��ʵ����Ŀ, ���ȡֵΪ#IMOS_COMMON_SWITCH_RES_MAXNUM */
    ULONG                   ulSwitchResNum;

    /** ͨ��������Դ���� */
    COMMON_SWITCH_UNIT_S    astSwitchUnitList[IMOS_COMMON_SWITCH_RES_MAXNUM];
}COMMON_SWITCH_RESOURCE_S;

/**
 * @struct tagCommonSwitchPlanBase
 * @brief ͨ�����мƻ�������Ϣ
 * @attention
 */
typedef struct tagCommonSwitchPlanBase
{
    /** ͨ�����мƻ����ͣ�ȡֵΪ#COMMON_SWITCH_PLANTYPE_E */
    ULONG       ulSwitchPlanType;

    /** ���мƻ����� */
    CHAR        szPlanName[IMOS_NAME_LEN];

    /** ���мƻ��������� */
    CHAR        szCreatTime[IMOS_TIME_LEN];

    /** ����1�����������Ϊ�û����� */
    CHAR        szParam1[IMOS_STRING_LEN_128];

    /** ����2�����������Ϊ�û����� */
    CHAR        szParam2[IMOS_STRING_LEN_128];

    /** ����3: ��������д�����Ļ���, ȡֵΪ1 */
    ULONG       ulParam3;

    /** ����4�����������Ϊ����� */
    ULONG       ulParam4;

    /** �ƻ�����, ȡֵΪ#PLAN_TYPE_E */
    ULONG       ulPlanType;

    /** ���мƻ�������֯���� */
    CHAR        szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** �ƻ�����, �ɲ��� */
    CHAR        szPlanDesc[IMOS_DESC_LEN];
}COMMON_SWITCH_PLAN_BASE_S;

/**
 * @struct tagSwitchPlanInfo
 * @brief ͨ�����мƻ���Ϣ
 * @attention
 */
typedef struct tagCommonSwitchPlanInfo
{
    /** ���мƻ�������Ϣ */
    COMMON_SWITCH_PLAN_BASE_S   stSwitchBaseInfo;

    /** ���мƻ�ʱ����Ϣ */
    PLAN_TIME_FOR_RES_S         stSwitchTimeInfo;
}COMMON_SWITCH_PLAN_INFO_S;


/**
 * @struct tagCommonSwitchPlanQueryItem
 * @brief ͨ�����мƻ���Ϣ��(��ѯͨ�����мƻ��б�ʱ����)
 * @attention
 */
typedef struct tagCommonSwitchPlanQueryItem
{
    /** ͨ�����мƻ����� */
    CHAR            szSwitchPlanCode[IMOS_RES_CODE_LEN];

    /** ͨ�����мƻ����� */
    CHAR            szPlanName[IMOS_NAME_LEN];

    /** ͨ�����мƻ��������� */
    CHAR            szCreatTime[IMOS_TIME_LEN];

    /** ͨ�����мƻ����ͣ�ȡֵΪ#COMMON_SWITCH_PLANTYPE_E */
    ULONG       ulSwitchPlanType;

    /** ����1�����������Ϊ�û����� */
    CHAR        szParam1[IMOS_STRING_LEN_128];

    /** ����2�����������Ϊ�û����� */
    CHAR        szParam2[IMOS_STRING_LEN_128];

    /** ����3: ��������д�����Ļ���, ȡֵΪ1 */
    ULONG       ulParam3;

    /** ����4�����������Ϊ����� */
    ULONG       ulParam4;

    /** �ƻ�״̬��BOOL_TRUEΪ�ƻ��Ѿ�����, BOOL_FALSEΪ�ƻ�δ���� */
    ULONG           ulPlanStatus;

}COMMONSWITCH_PLAN_QUERY_ITEM_S;

/*******************************************************************************
����ʾ
*******************************************************************************/

typedef struct tagCSSalvoStartSalvoUnit
{
    /** ����������/XP������� */
    CHAR szScreenCode[IMOS_DEVICE_CODE_LEN];

    /** ����������/XP����� */
    CHAR szScreenName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR szCameraCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR szCameraName[IMOS_NAME_LEN];

    /** ��������� */
    ULONG ulCameraType;

    /** ʵ��������� */
    ULONG ulResultCode;

}CS_SALVO_START_SALVO_UNIT_S;

/**
* @struct tagCSNofityScreenInfo
* @brief ҵ������Ϣ֪ͨ�������Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSNofityScreenInfo
{
    /** ��ʾ������ */
    CHAR  szScreenCode[IMOS_DEVICE_CODE_LEN];

    /** ��ʾ������, �����XP����, ��д����� */
    CHAR  szScreenName[IMOS_NAME_LEN];

    /** ��ʾ������ */
    ULONG ulScreenType;

}CS_NOTIFY_SCREEN_INFO_S;

/* ����ʾ��Ԫ��Ϣ */
/**
* @struct tagCSNotifySalvoUnitItem
* @brief ֪ͨUI����ʾ��Ԫʵ���������������ʾ��Ԫ��Ϣ
* @attention
*/
typedef struct tagCSNotifySalvoUnitItem
{
    /** ��ʾ����Ϣ */
    CS_NOTIFY_SCREEN_INFO_S stScreenInfo;

    /** �������Ϣ */
    CS_NOTIFY_CAMERA_INFO_S stCameraInfo;

}CS_NOTIFY_SALVO_UNIT_ITEM_S;

typedef struct tagCSNotifySalvoInfo
{
    /** ����ʾ���� */
    CHAR szSalvoCode[IMOS_RES_CODE_LEN];

    /** ����ʾ���� */
    CHAR szSalvoName[IMOS_NAME_LEN];

    /** ����ʾ��Ԫ���� */
    ULONG ulSalvoUnitNum;

    /** ����ʾ��Ԫ�б� */
    CS_NOTIFY_SALVO_UNIT_ITEM_S astNotifySalvoUnitItem[IMOS_DISPLAY_MAXNUM_PER_SALVO];

}CS_NOTIFY_SALVO_INFO_S;

typedef struct tagCSNotifyGroupsalvoInfo
{
    /** ����Ѳ��� */
    ULONG ulGroupsalvoHandle;

    /** ����Ѳ���ƿ�Ψһ����� */
    ULONG ulGroupsalvoCBUniqSeq;

    /** ����ѲʱUI��������ʾ����Ϣϵ�к� */
    ULONG ulGroupsalvoNotifyUISeq;

    /** ����Ѳ��Դ���� */
    CHAR szGroupsalvoCode[IMOS_RES_CODE_LEN];

    /** ����Ѳ��Դ���� */
    CHAR szGroupsalvoName[IMOS_NAME_LEN];

}CS_NOTIFY_GROUPSALVO_INFO_S;

typedef struct tagCSNotifyGroupsalvoPlanInfo
{
    /** ����Ѳ�ƻ����� */
    CHAR szGroupsalvoPlanCode[IMOS_RES_CODE_LEN];

    /** ����Ѳ�ƻ����� */
    CHAR szGroupsalvoPlanName[IMOS_NAME_LEN];

}CS_NOTIFY_GROUPSALVO_PLAN_INFO_S;

typedef struct tagCSNotifyOperateUserInfo
{
    /* ��ռ�������û��� */
    CHAR szUserName[IMOS_NAME_LEN];

    /* ��ռ�����û���¼���� */
    CHAR szUserLoginCode[IMOS_RES_CODE_LEN];

    /* ��ռ�����û��ĵ�¼IP */
    CHAR szUserIpAddress[IMOS_IPADDR_LEN];

}CS_NOTIFY_OPERATE_USER_INFO_S;

/**
* @struct tagCSNotifySalvoUnitEvent
* @brief ֪ͨUI����ʾ��Ԫ�¼�
* @attention
*   ����ʾ��Ԫ�Ķ���ҵ�����������ʾ/����Ѳ/����Ѳ�ƻ�, ͨ���ṹ�ж���ҵ����������
*/
typedef struct tagCSNotifySalvoUnitEvent
{
    /** ����ʾ���� */
    CHAR szSalvoCode[IMOS_RES_CODE_LEN];

    /** ����ʾ��Ԫ��Ϣ */
    CS_NOTIFY_SALVO_UNIT_ITEM_S stNotifySalvoUnitItem;

    /** �����û���Ϣ */
    CS_NOTIFY_OPERATE_USER_INFO_S stOperUserInfo;

    /** ����ҵ������ */
    ULONG ulTopServiceType;

    /** ֪ͨ��Ϣ����: SALVO_UNIT_NOTIFY_UI_EVENT_TYPE_E */
    ULONG ulNotifyType;

}CS_NOTIFY_SALVO_UNIT_EVENT_S;

/**
* @struct tagCSNotifyUISalvoEvent
* @brief ֪ͨUI����ҵ�����(����ʾ/����Ѳ/����Ѳ�ƻ�)
* @attention
*   ���ݶ���ҵ�����ͺ���ҵ��������Ψһȷ����ǰҵ��
*/
typedef struct tagCSNotifyUISalvoEvent
{
    /** ����ʾ��Ϣ, ��ҵ������������ʾʱ��Ч */
    CS_NOTIFY_SALVO_INFO_S stSalvoInfo;

    /** ����Ѳ��Ϣ, ����ҵ�����ͻ���ҵ������������Ѳʱ��Ч */
    CS_NOTIFY_GROUPSALVO_INFO_S stGroupsalvoInfo;

    /** ����Ѳ�ƻ���Ϣ, ����ҵ������������Ѳ�ƻ�ʱ��Ч */
    CS_NOTIFY_GROUPSALVO_PLAN_INFO_S stGroupsalvoPlanInfo;

    /** �����û���Ϣ */
    CS_NOTIFY_OPERATE_USER_INFO_S stOperUserInfo;

    /** ���ֱ���, ��ҵ������������ʾʱ��Ч */
    CHAR szLayoutCode[IMOS_CODE_LEN];

    /** ����ҵ������ */
    ULONG ulTopServiceType;

    /** ��ҵ������ */
    ULONG ulSubServiceType;

    /** ֪ͨ��Ϣ����: SALVO_NOTIFY_EVENT_TYPE_E */
    ULONG ulNotifyType;

}CS_NOTIFY_UI_SALVO_EVENT_S;

/**
* @struct tagCSNotifyStartXPSalvo
* @brief ֪ͨUI������Ļ����ʾ
* @attention
*/
typedef struct tagCSNotifyStartXPSalvo
{
    /** ����Ѳ���� */
    CHAR szGroupsalvoCode[IMOS_RES_CODE_LEN];

    /** ����ʾ���� */
    CHAR szSalvoCode[IMOS_RES_CODE_LEN];

    /** ���ֱ��� */
    CHAR szLayoutCode[IMOS_RES_CODE_LEN];

    /** ֪ͨ����� */
    ULONG ulSrcHandle;

    /** ��Ϣ���к� */
    ULONG ulMsgSequence;

    /** ����ԭ����: #CS_OPERATE_CODE_E */
    ULONG ulOperateCode;

}CS_NOTIFY_START_XP_SALVO_S;

/**
* @struct tagCSTLReaveNotify
* @brief  ͸��ͨ��ҵ����ռ��Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSTLReaveNotify
{
    CHAR    szUserName[IMOS_NAME_LEN];                  /* ��ռ�����û����� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];           /* ��ռ�����û��ĵ�¼IP */
    CHAR    szChannelCode[IMOS_RES_CODE_LEN];           /* ͸��ͨ������ */
    CHAR    szChannelName[IMOS_NAME_LEN];               /* ͸��ͨ������ */
    ULONG   ulNotifyType;                               /* ֪ͨ���� ��Ӧö��:TL_NOTIFY_UI_EVENT_TYPE */
}CS_TL_REAVE_NOTIFY_S;

/*******************************************************************************
                                         �������豸����
*******************************************************************************/

/**
 * @struct tagFactoryTypeInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagFactoryTypeInfo
{
    /** ��������ID, ȡֵ��Χ��1��65535 */
    ULONG ulTypeId;

    /** �������� */
    CHAR  szTypeName[IMOS_NAME_LEN];

    /** �������� */
    CHAR  szDesc[IMOS_DESC_LEN];

    /** �����ֶΣ���ʱ���� */
    CHAR  szReserved[IMOS_STRING_LEN_64];
}MANUFACTURER_TYPE_INFO_S;

/**
 * @struct tagDevTypeInfo
 * @brief �豸������Ϣ
 * @attention
 */
typedef struct tagDevTypeInfo
{
    /** �豸���� */
    ULONG ulDevType;

    /** �豸������ */
    ULONG ulDevSubType;

    /** �������� */
    ULONG ulManuFactureType;

    /** �������� */
    CHAR szTypeDesc[IMOS_DESC_LEN];

}DEV_TYPE_INFO_S;

/**
 * @struct tagEventTypeInfo
 * @brief �¼����ͣ���Ӧ��tbl_eventtype
 * @attention
 */
typedef struct tagEventTypeInfo
{
    /** �¼����ͣ�ȡֵΪ#AlARM_TYPE_E */
    ULONG ulEventType;

    /** �¼����� */
    CHAR    szEventName[IMOS_NAME_LEN];

    /** �¼����� */
    ULONG   ulEventSecurity;

    /** �¼����� */
    ULONG   ulEventCategory;

    /** ������ */
    CHAR    szEventSuggestion[IMOS_STRING_LEN_256];

    /** ά������ */
    CHAR    szEventExperience[IMOS_STRING_LEN_256];
}EVENT_TYPE_INFO_S;

/**
* @struct tagEventTypeExtInfo
* @brief �¼����ͣ���Ӧ��tbl_eventtype,tbl_event_enable
* @attention
*/
typedef struct tagEventTypeExtInfo
{
    /** �¼���ʾ��Ϣ */
    EVENT_TYPE_INFO_S  stEventTypeInfo;

    /** ϵͳĬ���¼����� */
    CHAR    szPreEventName[IMOS_NAME_LEN];

    /** ϵͳĬ���¼����� */
    ULONG   ulPreEventSecurity;

    /** ϵͳ�Զ����¼����� */
    CHAR    szSysEventName[IMOS_NAME_LEN];

    /** ϵͳ�Զ����¼����� */
    ULONG   ulSysEventSecurity;

    /* �Ƿ���ϵͳԤ����澯���� */
    BOOL_T  bSysEventType;

}EVENT_TYPE_EXT_INFO_S;

/**
* @struct tagEventTypeInfo
* @brief �¼��������豸���͹�����ϵ
* @attention
*/
typedef struct tagEventTypeRel
{
    /** �¼����� */
    ULONG  ulEventType;

    /** �豸���� */
    DEV_TYPE_INFO_S stDevTypeInfo;
}EVENT_TYPE_REL_S;

/**
* @struct tagEventTypeInfo
* @brief �¼��ۺ���Ϣ
* @attention
*/
typedef struct tagEventType
{
    /** �¼����� */
    EVENT_TYPE_INFO_S  stEventTypeInfo;

    /** �¼������������� */
    ULONG              ulEventTypeRelInfoNum;

    /** �¼�����������Ϣ */
    EVENT_TYPE_REL_S   astEventTypeRelInfo[IMOS_EVENT_TYPE_REL_MAX_NUM];

    /** �¼������Ƿ���ڱ��λ,���ھ�Ϊ1�������ھ�Ϊ0 */
    BOOL_T  bEventTypeIsExist;
}EVENT_TYPE_S;
/***************************************************************************
 ����
****************************************************************************/
/**
 * @struct tagConditionItem
 * @brief ��������ѡ��
 * @attention
 */
typedef struct tagSubscConditionItem
{
    /** ���Ĺ����������ͣ��Ͷ��������йأ���Ӧö�ٲμ���������˵�� */
    ULONG ulSubscConType;

    /** ���������߼���ϵ����: LOGIC_FLAG_E��Ŀǰֻ֧�ֵ��� */
    ULONG ulLogicFlag;

    /** �������ݣ����Ϊ��ֵ����ͨ����׼ת������ת�����ַ��� */
    CHAR  szConData[IMOS_STRING_LEN_64];
}SUBSC_CONDITION_ITEM_S;

/**
 * @struct tagSubscCondition
 * @brief ���Ĺ�������
 * @attention
 */
typedef struct tagSubscscCondition
{
    /** �������� */
    ULONG ulItemNum;

    /** ���Ĺ��������б� */
    SUBSC_CONDITION_ITEM_S astQueryConditionList[IMOS_SUBSC_ITEM_MAX_NUM];

 }SUBSC_CONDITION_S;

/**
* @struct tagSubscBaseInfo
* @brief ���ĸ澯�Ļ�����Ϣ
*/
typedef struct tagSubscBaseInfo
{
    CHAR                szResCode[IMOS_RES_CODE_LEN];       /** ���ĵ���Դ���� */
    ULONG               ulResType;                          /** ��Դ����:ö��ֵΪ: #IMOS_TYPE_E.Ŀǰ֧��:IMOS_TYPE_CAMERA��IMOS_TYPE_ALARM_SOURCE��IMOS_TYPE_ORG��Դ���� */
    ULONG               ulSubscType;                        /** �������ͣ�ö��ֵΪ:#SUBSC_TYPE_E */
    SUBSC_CONDITION_S   stSubscCon;                         /** ���Ĺ������������ݶ������Ͳ�ͬ����ͬ���μ�ö��ֵ#SUBSC_TYPE_E�е������� */
    ULONG               ulSubscExpires;                     /** ����ʱ��������Ϊ��λ��������Сʱ��1Сʱ���ϣ��Ϊ8760��Сʱ */
}SUBSC_BASE_INFO_S;


/**
* @struct tagSubscFullInfo
* @brief ���ĵ�ȫ����Ϣ
*/
typedef struct tagSubscFullInfo
{
    SUBSC_BASE_INFO_S   stSubscBaseInfo;            /** ���Ļ�����Ϣ */
    CHAR    szSubscResName[IMOS_NAME_LEN];          /** ���ĵ���Դ���� */
    CHAR    szSessionID[IMOS_SSN_ID_LEN];           /** ����ҵ��ID */
    CHAR    szEndTime[IMOS_TIME_LEN];               /** ���Ľ���ʱ�� */
}SUBSC_FULL_INFO_S;

/**
* @struct tagTrunkInfo
* @brief ������Ϣ
*/
typedef struct tagTrunkInfo
{
    /** ���߱��룬����ʱ��Ч */
    CHAR    szTrunkCode[IMOS_RES_CODE_LEN];

    /** �������� */
    CHAR    szTrunkName[IMOS_NAME_LEN];

    /** ���߹������ͣ�ȡֵ��Χ���:TRUNK_TYPE_E */
    ULONG   ulTrunkType;

    /** �����������ֵ */
    ULONG   ulTrunkMaxNum;

    /** ������ʹ����������ѯʱ��Ч */
    ULONG   ulTrunkUsedNum;

    /** ���ߴ������ֵ(��λK) */
    ULONG   ulTrunkMaxBandWidth;

    /** ������ʹ�ô�����ѯʱ��Ч */
    ULONG   ulTrunkUsedBandWidth;

    /** �����Ƿ����ã���ѯʱ��Ч */
    BOOL_T  bTrunkEnable;

}TRUNK_INFO_S;

/**
* @struct tagTrunkContentInfo
* @brief ����������Ϣ
*/
typedef struct tagTrunkContentInfo
{
    /** �������ݱ��룬����ʱ��Ч */
    CHAR    szTrunkContentCode[IMOS_RES_CODE_LEN];

    /** �����������ͣ�ȡֵ��Χ���:TRUNK_CONTENT_TYPE_E */
    ULONG   ulTrunkContentType;

    /** ������Դ���� */
    CHAR    szPhyResCode[IMOS_RES_CODE_LEN];

    /** ���Ӿ�����Դ���ƣ�����ʱ��Ч */
    CHAR    szPhyResName[IMOS_NAME_LEN];

    /** ����Χ��־��ȡֵ��Χ���:TRUNK_CONTENT_RANGE_TYPE_E */
    ULONG   ulTrunkRangeType;

}TRUNK_CONTENT_INFO_S;

/**
* @struct tagSysStreamFormat
* @brief ϵͳ��װ����ʽ
* @attention
*/
typedef struct tagSysStreamFormat
{
    /** �����䷽ʽ#IMOS_TRANS_TYPE_E */
    ULONG ulTransType;

    /** ��������#IMOS_STREAM_TYPE_E */
    ULONG ulStreamType;
}SYS_STREAM_FORMAT_S;
/**
 * @struct tagDevStatusInfo
 * @brief �豸�澯״̬��Ϣ
 * @attention
 */
typedef struct tagDevStatusInfo
{
    /** �豸���� */
    CHAR szDevCode[IMOS_CODE_LEN];

    /** �豸���� */
    CHAR szDevName[IMOS_NAME_LEN];

    /** �豸���� ȡֵ�μ�#IMOS_TYPE_E */
    ULONG ulDevType;

    /** �豸������, �������ȡֵ��#CAMERA_TYPE_E; �Լ�����������;
    �������豸(EC/DC/MS/DM/VX500)ȡֵ��#IMOS_DEVICE_TYPE_E. */
    ULONG ulDevSubType;

    /** �豸״̬��4��ULONG�ֶΣ�ÿ������λ��ʾһ���豸״̬(��λȡ˳��ӵ�λ��ʼ);
    �ܹ���֧��128λ�豸״̬, ������λ�����Ӧ���豸״̬�����#DEV_STATUS_BIT_E */
    ULONG aulDevStatus[IMOS_DEV_STATUS_MAX_NUM];

}DEV_STATUS_INFO_S;


/*******************************************************************************
BM
********************************************************************************/

/**
* @struct tagBMInfo
* @brief BM��Ϣ�ṹ��
* @attention �޸�BM��Ϣʱ��ֻ�����޸�BM���ƣ���������ԣ�����ʱ�䴰�ڣ�BM����
*/
typedef struct tagBMInfo
{
    /** BM����, BM��Ψһ��ʶ */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� -- ���޸� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** BM���ͣ�ֵΪ0����ʾBM8500 */
    ULONG ulBMType;

    /** BM������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** ��������ԣ�0-��ֹͣ��1-������ -- ���޸� */
    ULONG ulFullPolicy;

    /** ���ݿ�ʼʱ�� -- ���޸� */
    CHAR  szBkStartTime[IMOS_SIMPLE_TIME_LEN];

    /** ���ݽ���ʱ�� -- ���޸� */
    CHAR  szBkEndTime[IMOS_SIMPLE_TIME_LEN];

    /** BM����, �ɲ��� -- ���޸� */
    CHAR  szDevDesc[IMOS_DESC_LEN];

    /* ��ת��ý�������ԣ���IMOS_STREAM_SERVER_MODE_E */
    ULONG  ulStreamStorSvrPolicy;

    /** �����ֶ� */
    CHAR  szReserve[32];
}BM_INFO_S;

/**
* @struct tagBMQueryItem
* @brief BM����Ϣ��(��ѯBM�б�ʱ����)
* @attention ��
*/
typedef struct tagBMQueryItem
{
    /** BM����, BM��Ψһ��ʶ */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** BM����, ���ֶ���Ч, ȱʡֵΪ0 */
    ULONG ulBMType;

    /** BM������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /** BM������֯���� */
    CHAR  szOrgName[IMOS_NAME_LEN];

    /** �豸��ַ���ͣ�1-IPv4��2-IPv6 */
    ULONG ulDevaddrtype;

    /** �豸��ַ */
    CHAR  szDevAddr[IMOS_IPADDR_LEN];

    /** �豸�Ƿ�����, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE����imos_def.h�ж��� */
    ULONG ulIsOnline;

    /** �豸��չ״̬��ö��ֵΪ#DEV_EXT_STATUS_E */
    ULONG ulDevExtStatus;

    /** ��������ԣ�0-��ֹͣ��1-������ */
    ULONG ulFullPolicy;

    /** ���ݿ�ʼʱ�� */
    CHAR  szBkStartTime[IMOS_SIMPLE_TIME_LEN];

    /** ���ݽ���ʱ�� */
    CHAR  szBkEndTime[IMOS_SIMPLE_TIME_LEN];

    /* ��ת��ý�������ԣ�0 ����Ӧ  1 ֱ������ */
    ULONG  ulStreamStorSvrPolicy;

    /** �����ֶ� */
    CHAR  szReserve[32];
}BM_QUERY_ITEM_S;

/**
* @struct tagDMResForBMInfo
* @brief ΪBM�����DM��Դ����Ϣ�ṹ��
* @attention
*/
typedef struct tagDMResForBMInfo
{
    /** BM����, BM��Ψһ��ʶ */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** ��Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ������С����MBΪ��λ */
    ULONG ulCapacity;

    /** DM����, DM��Ψһ��ʶ */
    CHAR  szDMCode[IMOS_DEVICE_CODE_LEN];

    /** �洢�豸���� */
    CHAR  szStoreDevCode[IMOS_DEVICE_CODE_LEN];

    /** �����ֶ� */
    CHAR  szReserve[32];
}DM_RES_FOR_BM_INFO_S;

/**
* @struct tagThirdResForBMInfo
* @brief ΪBM����ĵ�������Դ����Ϣ�ṹ��
* @attention
*/
typedef struct tagThirdResForBMInfo
{
    /** BM����, BM��Ψһ��ʶ */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** ��Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ��������Դȫ·�� */
    CHAR  szLocalPath[IMOS_FILE_PATH_LEN];

    /** �����ֶ� */
    CHAR  szReserve[32];
} THIRD_RES_FOR_BM_INFO_S;

/**
* @struct tagBMResQueryItem
* @brief BM������Դ����Ϣ��(��ѯBM������Դ�б�ʱ����)
* @attention ��
*/
typedef struct tagBMResQueryItem
{
    /** ������Դ���� */
    CHAR  szResCode[IMOS_CODE_LEN];

    /** BM����, BM��Ψһ��ʶ */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ��Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ������Դ����#BAK_RES_TYPE_E */
    ULONG ulResType;

    /** �洢�豸���� */
    CHAR  szStoreDevName[IMOS_NAME_LEN];

    /** ������Դ״̬#BAK_RES_STATUS_E */
    ULONG ulResStatus;

    /** ��������С����MBΪ��λ */
    ULONG ulTotalCapacity;

    /** ʣ��������С����MBΪ��λ */
    ULONG ulSpareCapacity;

    /** ������Դȫ·��-��������Դ����Ч */
    CHAR  szResPath[IMOS_FILE_PATH_LEN];
} BM_RES_QUERY_ITEM_S;

/**
* @struct tagCamBakResInfo
* @brief Ϊ���������BM��Դ����Ϣ�ṹ��(��ѯ�����������Դʱ���ص���Ϣ��)
* @attention
*/
typedef struct tagCamBakResInfo
{
    /** ��������� */
    CHAR  szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** BM���� */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ���ݸ澯ǰһ��¼���ʱ��, ȡֵ��Χ[0~15����] -- ���޸� */
    ULONG ulAlarmPreRecLen;

    /** ���ݸ澯��һ��¼���ʱ��, ȡֵ��Χ[1~30����] -- ���޸� */
    ULONG ulAlarmPostRecLen;

    /** ����ģʽ��־λ#BAK_MODE_E */
    ULONG ulCamOwnFlag;

    /** ����������С����MBΪ��λ(������ֶ���Ч) -- ���޸� */
    ULONG ulCapacity;

    /** ����ʣ��������С����MBΪ��λ(������ֶ���Ч)-��ѯʱ����Ч */
    ULONG ulSpareCapacity;

    /** ���������(������ֶ���Ч)��0-��ֹͣ��1-������ -- ���޸� */
    ULONG ulFullPolicy;

    /** ������Դ����(������ֶ���Ч) */
    CHAR szBakResCode[IMOS_CODE_LEN];

    /** ������Դ����(������ֶ���Ч) */
    CHAR szBakResName[IMOS_NAME_LEN];

    /*ת��ģʽ��0 ��֧�֣�Ĭ��ֵ��  1 ��ת��*/
    ULONG    ulBkMode;

    /** �����ֶ� */
    CHAR  szReserve[32];
}CAM_BAK_RES_INFO_S;

/**
* @struct tagCamBakResQueryItem
* @brief �����������Դ�����������Ϣ��(��ѯ�����������Դ��������б�ʱ����)
* @attention ��
*/
typedef struct tagCamBakResQueryItem
{
    /** ��������� */
    CHAR  szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** BM���� */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ����ģʽ��־λ#BAK_MODE_E */
    ULONG ulCamOwnFlag;

    /** �Ƿ��Ѿ��ƶ��˱��ݼƻ�,#BOOL_TRUE��ʾ�ƶ���BOOL_FALSE��ʾδָ�� */
    BOOL_T bPlanFlag;

    /** ���ݼƻ����� */
    CHAR szPlanCode[IMOS_RES_CODE_LEN];

    /** ���ݼƻ�״̬ */
    ULONG ulPlanStatus;

    /*ת��ģʽ��0 ��֧�֣�Ĭ��ֵ��  1 ��ת��*/
    ULONG    ulBkMode;

    /** �����ֶ� */
    CHAR  szReserve[32];
} CAM_BAK_RES_QUERY_ITEM_S;

/**
* @struct tagBakTaskInfo
* @brief �����������Ϣ�ṹ��
* @attention ��
*/
typedef struct tagManulBakInfo
{
    /** ��������� */
    CHAR  szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ���ݲ���#BAK_POLICY_E */
    ULONG ulBakPolicy;

    /** ����ʱ��:��ʼ������ */
    TIME_SLICE_S stBakTime;

    /** ����·��(����DVR��ʱ��ʹ��) */
    CHAR szBakPath[IMOS_FILE_PATH_LEN];

    /** �������� */
    CHAR szCaseDesc[IMOS_BAK_CASE_LEN];

    /** �����ֶ� */
    CHAR  szReserve[32];
} MANUL_BAK_INFO_S;

/**
* @struct tagBakTaskQueryItem
* @brief �����������Ϣ��(��ѯ��������ʱ����)
* @attention ��
*/
typedef struct tagBakTaskQueryItem
{
    /** ����������� */
    CHAR  szBakTaskCode[IMOS_CODE_LEN];

    /** ������������#BAK_TYPE_E */
    ULONG ulBakType;

    /** ��������״̬#BAK_TASK_STATUS_E */
    ULONG ulBakTaskStatus;

    /** ���ݲ���#BAK_POLICY_E */
    ULONG ulBakPolicy;

    /** �������һ�ι���ʱ�� */
    CHAR  szLastHangupTime[IMOS_TIME_LEN];

    /** ����ʱ��:�ύ������ */
    TIME_SLICE_S stTaskTime;

    /** BM���� */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR  szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** ¼��ʱ��:��ʼ������ */
    TIME_SLICE_S stRecTime;

    /** ����ʱ��:��ʼ���������ݲ�ʹ�� */
    TIME_SLICE_S stBakTime;

    /** ����Ŀ��·�����ֶ��������Ч */
    CHAR szBakPath[IMOS_FILE_PATH_LEN];

    /** �û����� */
    CHAR  szUserCode[IMOS_USER_CODE_LEN];

    /** �û����� */
    CHAR  szUserName[IMOS_NAME_LEN];

    /** ��������-�ֶ������������Ч */
    CHAR szCaseDesc[IMOS_BAK_CASE_LEN];
} BAK_TASK_QUERY_ITEM_S;

/**
* @struct tagBakFileHandleInfo
* @brief �����ļ��Ĳ�����Ϣ�ṹ��
* @attention ��
*/
typedef struct tagBakFileHandleInfo
{
    /** �����ļ��洢·�� */
    CHAR szBakFilePath[IMOS_FILE_PATH_LEN];

    /** BM���� */
    CHAR szBMCode[IMOS_DEVICE_CODE_LEN];

    /** �����ļ���������#BAK_FILE_OPER_E */
    ULONG ulHandleType;
} BAK_FILE_HANDLE_INFO_S;

/**
* @struct tagBakFileQueryItem
* @brief �����ļ�����Ϣ��(��ѯ����������ļ�ʱ����)
* @attention ��
*/
typedef struct tagBakFileQueryItem
{
    /** �����ļ����� */
    CHAR  szBakFileCode[IMOS_RES_CODE_LEN];

    /** BM���� */
    CHAR  szBMCode[IMOS_DEVICE_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR  szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** ���������, ȡֵΪ#CAMERA_TYPE_E */
    ULONG ulCameraType;

    /** �����ļ��洢·�� */
    CHAR szBakFilePath[IMOS_FILE_PATH_LEN];

    /** �ļ�����ʱ�� */
    CHAR  szFileCreateTime[IMOS_TIME_LEN];

    /** ����ʱ��:��ʼ������ */
    TIME_SLICE_S stBakTime;

    /** �ļ���������MBΪ��λ */
    ULONG ulFileCapacity;

    /** ������Դ���� */
    CHAR  szBakResCode[IMOS_CODE_LEN];

    /** �ļ�����#BAK_TYPE_E */
    ULONG ulFileType;

    /** �ļ������ʽ��0-TS��1-h3crd */
    ULONG ulFileFormat;

    /** �����ļ�״̬��ʶ��0-�����ļ���1-�����ļ� */
    ULONG ulFileStatus;

    /** �Ƿ�������ʶ��0-δ������1-���� */
    ULONG ulLockFlag;

    /** �������� */
    CHAR szCaseDesc[IMOS_BAK_CASE_LEN];
} CAM_BAK_FILE_QUERY_ITEM_S;


/**
 * @struct tagBackupBase
 * @brief ���ݼƻ�������Ϣ
 * @attention
 */
typedef struct tagBackupBase
{
    /** ���ݼƻ����룬���������� */
    CHAR    szBakPlanCode[IMOS_RES_CODE_LEN];

    /** ���ݼƻ����� */
    CHAR    szBakPlanName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR    szCameraCode[IMOS_DEVICE_CODE_LEN];

    /** ��������ƣ���ѯ�ƻ���Ϣʱ���� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** �û����� */
    CHAR    szUserCode[IMOS_USER_CODE_LEN];

    /** �ƻ�����ʱ�� */
    CHAR    szCreateTime[IMOS_TIME_LEN];

    /** ���ݼ���ǰ������ */
    ULONG   ulBeforeDays;

    /** �ƻ����ڣ�0-�ռƻ���1-�ܼƻ� */
    ULONG   ulPlanCycle;

    /** ���ݿ�ʼʱ�䣬���ֶα������ݲ�ʹ�� */
    CHAR    szBakStartTime[IMOS_TIME_LEN];

    /** ���ݽ���ʱ�䣬���ֶα������ݲ�ʹ�� */
    CHAR    szBakEndTime[IMOS_TIME_LEN];

    /** ���ݲ���#BAK_POLICY_E */
    ULONG   ulBakPolicy;

    /** �ƻ�״̬#PLAN_OPER_MODE_E���޸ļƻ�ʱ���ֶ���Ч */
    ULONG   ulPlanStatus;
}BAK_PLAN_BASE_S;

/**
 * @struct tagBackupPlan
 * @brief ���ݼƻ���Ϣ
 * @attention
 */
typedef struct tagBackupPlan
{
    /** ���ݼƻ�������Ϣ */
    BAK_PLAN_BASE_S   stBakPlanBasicInfo;

    /** ���ݼƻ�ʱ����Ϣ����ʱ��ƻ� */
    PLAN_WEEKTIME_S   astPlanTime[IMOS_WEEK_DAYS];
}BAK_PLAN_INFO_S;

/**
* @struct tagGetBakUrlInfo
* @brief ��ȡ����¼���ļ�URL������ṹ
*/
typedef struct tagGetBakUrlInfo
{
    /** ��������� */
    CHAR            szCamCode[IMOS_DEVICE_CODE_LEN];

    /** ¼���ļ��� */
    CHAR            szFileName[IMOS_FILE_PATH_LEN];

    /** ¼�����ʼ/����ʱ��, ���е�ʱ���ʽΪ"YYYY-MM-DD hh:mm:ss" */
    TIME_SLICE_S    stRecTimeSlice;

    /** �ͻ���IP��ַ */
    CHAR            szClientIp[IMOS_IPADDR_LEN];

}GET_BAK_URL_INFO_S;

/************************************************************************/
/* ��ת��                                                             */
/************************************************************************/
/**
* @struct tagCamStreamStorInfo
* @brief �������ת��������Ϣ�ṹ��
* @attention */
typedef struct tagCamStreamStorInfo
{
    /** ��������� */
    CHAR  szCamCode[IMOS_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** BM���� */
    CHAR  szBMCode[IMOS_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ������Դ����*/
    CHAR szBakResCode[IMOS_CODE_LEN];

    /** ������Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ռ��������С����MBΪ��λ */
    ULONG ulCapacity;

    /** ���������(������ֶ���Ч)��0-��ֹͣ��1-������ */
    ULONG ulFullPolicy;

    /* �������ͣ�1 ������  2 ������ */
    ULONG   ulStreamType;

    /** �����ֶ� */
    CHAR  szReserve[32];
}CAM_STREAM_STOR_INFO_S;

/**
* @struct tagCamStreamStorQueryItem
* @brief �������ת�������������Ϣ��
* @attention ��
*/
typedef struct tagCamStreamStorQueryItem
{
    /** ��������� */
    CHAR  szCamCode[IMOS_CODE_LEN];

    /** ��������� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** BM���� */
    CHAR  szBMCode[IMOS_CODE_LEN];

    /** BM���� */
    CHAR  szBMName[IMOS_NAME_LEN];

    /** ������Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ռ������ ��MBΪ��λ */
    ULONG ulCapacity;

    /** �Ƿ��Ѿ��ƶ���ת��ƻ�,#BOOL_TRUE��ʾ�ƶ���BOOL_FALSE��ʾδָ�� */
    BOOL_T bPlanFlag;

    /** ת��ƻ����� */
    CHAR szPlanCode[IMOS_CODE_LEN];

    /** ת��ƻ�״̬ */
    ULONG ulPlanStatus;

    /** ��Դ������Ϣ��0�������������2����DVR��������������� */
    ULONG ulResAttribute;

    /** �����ֶ� */
    CHAR  szReserve[32];
} CAM_STREAM_STOR_QUERY_ITEM_S;

 /**
 * @struct tagStreamStorBase
 * @brief ��ת��ƻ�������Ϣ
 * @attention
 */
typedef struct tagStreamStorBase
{
    /** ��ת��ƻ����룬���������� */
    CHAR    szStreamStorPlanCode[IMOS_CODE_LEN];

    /** ��ת��ƻ����� */
    CHAR    szStreamStorPlanName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR    szCameraCode[IMOS_CODE_LEN];

    /** ��������ƣ���ѯ�ƻ���Ϣʱ���� */
    CHAR  szCamName[IMOS_NAME_LEN];

    /** �û����� */
    CHAR    szUserCode[IMOS_CODE_LEN];

    /** �ƻ�����ʱ�� */
    CHAR    szCreateTime[IMOS_TIME_LEN];

    /** �ƻ����ڣ�0-�ռƻ���1-�ܼƻ� */
    ULONG   ulPlanCycle;

    /** �Ƿ���洢(1 ��,0 ��) */
    BOOL_T  bIsDayStore;

    /** ����洢������ */
    ULONG   ulStoreDays;

    /** �ƻ�״̬#PLAN_OPER_MODE_E���޸ļƻ�ʱ���ֶ���Ч */
    ULONG   ulPlanStatus;
}STREAM_STOR_PLAN_BASE_S;

/**
 * @struct tagStreamStorPlan
 * @brief ��ת��ƻ���Ϣ
 * @attention
 */
typedef struct tagStreamStorPlan
{
    /** ��ת��ƻ�������Ϣ */
    STREAM_STOR_PLAN_BASE_S   stStreamStorPlanBasicInfo;

    /** ��ת��ƻ�ʱ����Ϣ����ʱ��ƻ� */
    PLAN_WEEKTIME_S   astPlanTime[IMOS_WEEK_DAYS];
}STREAM_STOR_PLAN_INFO_S;

/************************************************************************/
/* �澯����                                                             */
/************************************************************************/
/**
* @struct tagAlarmSendingRuleBaseInfo
* @brief �澯���Ĺ��������Ϣ
* @attention
*/
typedef struct tagAlarmSendingRuleBaseInfo
{
    /** ���� */
    CHAR                   szRuleCode[IMOS_CODE_LEN];

    /** ���� */
    CHAR                   szRuleName[IMOS_NAME_LEN];

    /** �Ƿ�ϵͳԤ�������BOOL_TRUE��ʾϵͳԤ�������BOOL_FALSE��ϵͳԤ������� */
    BOOL_T                 bSystemRule;

    /** �澯���ࣺĿǰ��δʹ�ã���0 */
    ULONG                  ulEventCategory;

    /** �澯���𣺵�0λ��ʾ��������1λ��ʾ��Ҫ����2λ��ʾ��Ҫ����3λ��ʾ���棬��4λ��ʾ��ʾ */
    ULONG                  ulEventLevel;

    /** �澯���ͷ�ʽ����0λ��ʾUI����1λ��ʾEmail����2λ��ʾSMS */
    ULONG                  ulSendEventMode;

}ALARM_SENDING_RULE_BASE_INFO_S;

/**
* @struct tagAlarmSendingRule
* @brief �澯���Ĺ���
* @attention
*/
typedef struct tagAlarmSendingRule
{

    /**�澯���Ĺ��������Ϣ */
    ALARM_SENDING_RULE_BASE_INFO_S stBaseInfo;

    /** �澯Դ��Ŀ */
    ULONG                          ulAlarmSrcNum;

    /** �澯Դ�����б� */
    CHAR                           aszAlarmSrcList[IMOS_ALARM_SEND_SRC_MAXNUM][IMOS_CODE_LEN];

    /** �û���Ŀ */
    ULONG                          ulUserNum;

    /** �û��б� */
    CHAR                           aszUserList[IMOS_ALARM_SEND_USER_MAXNUM][IMOS_CODE_LEN];

}ALARM_SENDING_RULE_S;

/**
* @struct tagSDKInit
* @brief SDK ��ʼ��ʱ�õĽṹ
*/
typedef struct tagSDKInit
{
    /** �������� */
    CHAR szProcName[IMOS_PROC_NAME_MAX_LEN];

    /** ����procID��IMOSϵͳ��ͳһ���䣬��ͬ���ͽ���ʹ����ͬ��procID�� */
    USHORT usProcID;

    /** �Ƿ�UI��ı�� */
    BOOL_T bUIFlag;

    /** �Ƿ����XP��� */
    BOOL_T bXPFlag;

    ULONG  ulSDKInitFlag;

    /** �ص�����ָ�� */
    SDK_CALL_BACK_PROC_PF pfnCallBackProc;

    /** �ص��������������ڻص���������θ�ֵ */
    VOID *pParam;

}SDK_INIT_S;

/**
* @struct tagPortRange
* @brief �˿ڷ�Χ
* @attention
*/
typedef struct tagPortRange
{
    /** �˿���ʼ��Χ */
    ULONG  ulPortBegin;

    /** �˿ڽ�����Χ */
    ULONG  ulPortEnd;

    /** Ϊ����Ԥ�� */
    ULONG ulType;
}PORT_RANGE_S;

/**
* @struct tagUniversalPage
* @brief ͨ�÷�ҳ��Ϣ
* @attention
*/
typedef struct tagUniversalPage
{
    ULONG ulFirstRow;               /**< ��ҳ�ӵڼ��п�ʼ��ѯ */
    ULONG ulPageRowNum;             /**< ��ҳҪ��ѯ������ */
    BOOL_T bQueryCount;             /**< �Ƿ��ѯ������ */
    ULONG ulActualRowCount;         /**< ʵ�ʷ��ص����� */
    ULONG ulTotalRowCount;          /**< ���������������ܼ� */
}UNI_PAGE_S;
/**
* @struct tagUniversalLogicPiece
* @brief ͨ��Լ���߼���
* @attention
*/
typedef struct tagUniversalLogicPiece
{
    ULONG ulPiece;                  /**< ������ */
    LOGIC_FLAG_E enLogic;           /**< �����߼�#LOGIC_FLAG_E */
    CHAR szValues[64];              /**< ����ֵ */
}UNI_LOGIC_PIECE_S;

/**
* @struct tagUniversalOrder
* @brief ͨ��������
* @attention
*/
typedef struct tagUniversalOrder
{
    ULONG ulPiece;
    LOGIC_FLAG_E enLogic;           /**< �����ǽ���#LOGIC_FLAG_E */
}UNI_ORDER_S;
/**
* @struct tagUniversalConditon
* @brief ͨ��Լ������
* @attention
*/
typedef struct tagUniversalConditon
{
    ULONG ulFlag;                                   /**< ��ѯ�߼���־λ#UNI_CONDITION_FLAG_E */
    ULONG ulOrderNum;                               /**< ���������ĸ��� */
    UNI_ORDER_S *pstOrderPiece;                     /**< ������ */
    ULONG ulANDCondNum;                             /**< ANDԼ�������ĸ��� */
    UNI_LOGIC_PIECE_S *pstANDCondtion;              /**< ANDԼ�������б� */
    ULONG ulORCondNum;                              /**< ORԼ�������ĸ��� */
    UNI_LOGIC_PIECE_S *pstORCondtion;               /**< ORԼ�������б� */
}UNI_CONDITION_S;

/**
 * @struct tagOutChannelInfo
 * @brief ���ͨ����Ϣ
 * @attention
 */
typedef struct tagOutChannelInfo
{
    /** ���ͨ������,ȡֵΪ#OUT_CHANNEL_TYPE_E */
    ULONG ulOutType;

    /** ���ͨ��ģʽ,ȡֵΪ#OUT_CHANNEL_MODE_E */
    ULONG ulOutMode;
} OUT_CHANNEL_INFO_S;

/**
 * @struct tagOutChannelSplitScrInfo
 * @brief ���ͨ��������Ϣ
 * @attention
 */
typedef struct tagOutChannelSplitScrInfo
{
    /** ����ģʽ,ȡֵΪ#OUT_CHANNEL_MODE_E */
    ULONG ulSplitScrMode;

    /** ��Ƶ���ͨ����(����ģʽΪ"ȫ��"ʱ��Ч) */
    ULONG ulVideoOutChannelNo;

    /** �����ֶ� */
    CHAR szReserve[128];
} OUT_CHANNEL_SPLIT_SCR_INFO_S;

/**
 * @struct tagSplitScrInfo
 * @brief ������������Ϣ
 * @attention
 */
typedef struct tagSplitScrInfo
{
    /** ����ģʽ,ȡֵΪ#SPLIT_SCR_MODE_E */
    ULONG ulSplitScrMode;

    /** ��������(ȫ��ʱ��Ч) */
    CHAR szSplitScrCode[IMOS_CODE_LEN];

    /** �Ƿ�"�Զ��л�������"(#BOOL_TRUE ��,#BOOL_FALSE ��) */
    BOOL_T bSwitchStream;

    /** Ԥ���ֶ� */
    CHAR szReserve[128];
} SPLIT_SCR_INFO_S;

/*******************************************************************************
ECR33**-HF
********************************************************************************/

/**
 * @struct tagECRBasicInfo
 * @brief ECR������Ϣ
 * @attention
 */
typedef struct tagECRBasicInfo
{
    /** ECR�豸���� */
    CHAR    szECRCode[IMOS_ECR_CODE_LEN];

    /** ECR���� */
    CHAR    szECRName[IMOS_NAME_LEN];

    /** ��֯����� */
    CHAR    szOrgCode[IMOS_CODE_LEN];

    /** ECR�豸�ͺ�, ȡֵΪ#IMOS_DEVICE_TYPE_E�е�#IMOS_DT_ECR3308_HF, #IMOS_DT_ECR3316_HF, #IMOS_DT_ECR3308_HF_E, #IMOS_DT_ECR3316_HF_E*/
    ULONG   ulECRDevType;

    /** IP��ַ����:IPV4ȡֵΪ#IMOS_IPADDR_TYPE_IPV4, IPV6ȡֵΪ#IMOS_IPADDR_TYPE_IPV6 */
    ULONG   ulIPAddrType;

    /** IP��ַ */
    CHAR    szIPAddress[IMOS_IPADDR_LEN];

    /** ECR����״̬, ȡֵΪ#IMOS_DEV_STATUS_ONLINE��#IMOS_DEV_STATUS_OFFLINE. ���ڲ�ѯECR��Ϣ */
    ULONG   ulOnlineStatus;

    /** �������λ��ȡֵΪ#DOMAIN_STREAM_PULL_TYPE  */
    ULONG ulIsSession;

    /** �����ֶ� */
    CHAR szReserve[252];

}ECR_BASIC_INFO_S;

/**
 * @struct tagECRCameraInfo
 * @brief ECR Camera�б��ѯ������Ϣ
 * @attention
 */
typedef struct tagECRCamInfo
{
    /** Camera����*/
    CHAR  szCameraCode[IMOS_CODE_LEN];

    /** Camera���� */
    CHAR  szCameraName[IMOS_NAME_LEN];

    /** �����ֶ� */
    CHAR szReserve[128];
}ECR_CAM_INFO_S;

/**
* @struct tagECRAlarmSrcInfo
* @brief ECR�澯Դ�б��ѯ���صĵ�Ԫ��Ϣ
*/
typedef struct tagECRAlarmSrcInfo
{
    /** �澯Դ����*/
    CHAR  szAlarmSrcCode[IMOS_CODE_LEN];

    /** �澯Դ���� */
    CHAR  szAlarmSrcName[IMOS_NAME_LEN];

    /** �����ֶ� */
    CHAR szReserve[128];

}ECR_ALARMSRC_INFO_S;

/**
 * @struct tagECRDeviceBasicInfo
 * @brief ECR�����豸������Ϣ
 * @attention
 */
typedef struct tagECRDeviceBasicInfo
{
    /** �Ƿ�֧���鲥, 1Ϊ֧��; 0Ϊ��֧�� */
    ULONG ulIsMulticast;

    /** �澯ʹ��, 1Ϊʹ��; 0Ϊ��ʹ�� */
    ULONG ulEnableAlarm;

    /** ���¸澯�¶�����, ȡֵΪ-100~49 */
    LONG lTemperatureMax;

    /** ���¸澯�¶�����, ȡֵΪ50~100 */
    LONG lTemperatureMin;

    /** ���ײ�, ȡֵΪ:#IMOS_STREAM_RELATION_SET_E,
        ĿǰECR֧�ֵ���:
        1��H264������(#IMOS_SR_H264_SHARE)
        11��H264+H264(#IMOS_SR_H264_H264)
    */
    ULONG ulEncodeSet;

    /** ��ʽ, ȡֵΪ#IMOS_PICTURE_FORMAT_E, Ŀǰ��֧��#IMOS_PF_PAL */
    ULONG ulStandard;

    /** ��Ƶ����Դ��ȡֵΪ#IMOS_AUDIO_INPUT_SOURCE_E, Ŀǰ��֧��#IMOS_AI_LR */
    ULONG ulAudioinSource;

    /** �����ֶ� */
    CHAR szReserve[512];
}ECR_DEVICE_BASIC_INFO_S;

/**
 * @struct tagECRVMServerInfo
 * @brief ECR�豸�ķ�������Ϣ
 * @attention
 */
typedef struct tagECRVMServerInfo
{
    /** ��������ַ����, 1-IPv4 2-IPv6 */
    ULONG   ulServerAddrType;

    /** ��������ַ */
    CHAR    szServerAddr[IMOS_IPADDR_LEN];

    /** ����������״̬, ָ��������ǰ���������ע���Ƿ�ɹ�, 1Ϊ�ɹ�; 0����2Ϊʧ��, ����ʱ��Ч, ���û���ѯ */
    ULONG   ulServerRegStatus;

    /** ����������ͨ��Э��, ȡֵΪ#INTERDOMAIN_PROTOCOL_TYPE_E, Ŀǰֻ֧��IMOS��׼ */
    ULONG   ulInterProtocolType;

    /** �����������������, ��Ҫ�ͷ��������õĻ��������һ��, Ŀǰֻ֧��IMOS��׼������� */
    CHAR    szInterDomainCode[IMOS_CODE_LEN];

    /** �������û�����, ��Ҫ�ͷ��������õĻ����û�����һ�� */
    CHAR    szInterUserCode[IMOS_CODE_LEN];

    /** �����ֶ� */
    CHAR    szReserve[512];
}ECR_VMSERVER_INFO_S;

/**
 * @struct tagECRServerMgrInfo
 * @brief ECR�豸������������Ϣ
 * @attention
 */
typedef struct tagECRServerMgrInfo
{
    /** ECR�豸����, ����Ψһ��ʶ */
    CHAR  szECRDevCode[IMOS_ECR_CODE_LEN];

    /** NVR�����û�����, ����Ψһ��ʶ��ECR��Ч��Ϊ0���� */
    CHAR  szECRInterUserCode[IMOS_CODE_LEN];

    /** ECR�豸����, ���ڱ�ʾECR�����豸, ������IPC�� */
    CHAR  szECRDevName[IMOS_NAME_LEN];

    /** ����������ģʽ����, ȡֵΪ#ECR_SERVER_MODE_TYPE_E */
    ULONG ulServerModeType;

    /** ��������Ϣ, ��Ϊ��������ģʽʱ������д */
    ECR_VMSERVER_INFO_S stVMserverInfo;

    /** �����ֶ� */
    CHAR szReserve[64];
}ECR_SERVER_MGR_INFO_S;

/**
 * @struct tagRevStrategyInfo
 * @brief ��ռ������Ϣ�ṹ
 * @attention
 */
typedef struct tagRevStrategyInfo
{
    /** ��ռҵ������, ȡֵΪ# REAVE_SRV_TYPE_E  */
    ULONG ulRevSrvType;

    /** ��ռ��������, ȡֵΪ# REAVE_STRATEGY_E */
    ULONG ulRevStrategy;
}REV_STRATEGY_INFO_S;

/**
 * @struct tagSrvRevStrategy
 * @brief ҵ����ռ���Խṹ
 * @attention
 */
typedef struct tagSrvRevStrategy
{
    /** ��ռ�����������ֵ���ܳ���REV_SRV_MAX */
    ULONG ulRevStrategyNum;

    /** ��ռ������Ϣ*/
    REV_STRATEGY_INFO_S astRevStrategyInfo[REV_SRV_MAX];
}SRV_REV_STRATEGY_S;

/**
* @struct tagCSVodReaveNotify
* @brief SOI �طű���ռ��Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSVodReaveNotify
{
    CHAR    szSessionId[IMOS_SSN_ID_LEN];        /* �ط�ҵ��ỰID�����������ʱ���ص�SessionIdƥ�� */
    CHAR    szUserName[IMOS_NAME_LEN];           /* ��ռ�������û��� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];    /* ��ռ�����û��ĵ�¼IP */
}CS_VOD_REAVE_NOTIFY_S;
/**
 * @struct tagSceneUnit
 * @brief ������Ԫ��Ϣ
 * @attention
 */
typedef struct tagSceneUnit
{
    /** ��������� */
    CHAR        szCamCode[IMOS_RES_CODE_LEN];

    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** ��������ͣ���ѯʱ��Ч */
    ULONG       ulCamType;

    /** ��̨Ԥ��λ */
    ULONG       ulPresetValue;

    /** ����:���� */
    ULONG       ulParam1;

    /** ����:���� */
    CHAR        szParam2[IMOS_STRING_LEN_128];

    /** ����:���� */
    CHAR        szParam3[IMOS_STRING_LEN_128];
}SCENE_UNIT_S;

/**
 * @struct tagSceneInfo
 * @brief ������Ϣ
 * @attention
 */
typedef struct tagSceneInfo
{
    /** ����������ͣ�ĿǰΪ�����ֶ� */
    ULONG       ulSceneType;

    /** ������ر��� */
    CHAR        szSceneCode[IMOS_CODE_LEN];

    /** ����������� */
    CHAR        szSceneName[IMOS_NAME_LEN];

    /** ������ذ����ĵ�Ԫʵ������, ���ֵΪ#IMOS_DISPLAY_MAXNUM_PER_SALVO */
    ULONG       ulSceneUnitNum;

    /** ����ʾ��Ԫ���� */
    SCENE_UNIT_S astSceneUnitList[IMOS_SCENE_MAXNUM_RES];
}SCENE_INFO_S;

/**
* @struct tagSceneQueryItem
* @brief ������Ϣ��(��ѯ�����б�ʱ����)
* @attention
*/
typedef struct tagSceneQueryItem
{
    /** ������ر��� */
    CHAR        szSceneCode[IMOS_CODE_LEN];

    /** ����������� */
    CHAR        szSceneName[IMOS_NAME_LEN];

    /** ������֯�����,���ڲ�ѯ���Լ������µ�����ʾ�б�ƽ̨���ز������ֵ */
    CHAR szOrgCode[IMOS_CODE_LEN];

    /** ������ذ����ĵ�Ԫʵ������, ���ֵΪ#IMOS_DISPLAY_MAXNUM_PER_SCENE */
    ULONG       ulSceneUnitNum;

}SCENE_QUERY_ITEM_S;

/**
 * @struct tagShareSceneBase
 * @brief ������������Ϣ
 * @attention
 */
typedef struct tagShareSceneBase
{
     /** �������� */
    CHAR szSceneCode[IMOS_CODE_LEN];

    /** �������� */
    CHAR szSceneName[IMOS_NAME_LEN];

    /** ����֯�ڵ���� */
    CHAR szParentOrgCode[IMOS_CODE_LEN];

    /** �������������ʱ�Ķ������ */
    CHAR szSceneShareCode[IMOS_CODE_LEN];

 }SHARE_SCENE_BASE_S;

/**
* @struct tagShareMapInfo
* @brief ��������Ϣ
*/
typedef struct tagShareSceneInfo
{
    /** ��������������룬����Ϊ��������ʱʹ�� */
    CHAR                szExDomainCode[IMOS_CODE_LEN];

    /** �Ƿ��ѹ��������1��ʾ�ǣ�0��ʾ�� */
    BOOL_T              bIsShared;

    /** ������������Ϣ */
    SHARE_SCENE_BASE_S    stShareSceneBaseInfo;

}SHARE_SCENE_INFO_S;

/**
* @struct tagEmailSendingConfig
* @brief �ʼ�������ص�����
* @attention
*/
typedef struct tagEmailSendingConfig
{
    /** �ʼ��ķ����� */
    CHAR    szSenderName[IMOS_NAME_LEN];

    /** �û������ʼ� */
    CHAR    szEmail[IMOS_MAIL_LEN];

    /** SMTP�ʼ���������ַ */
    CHAR    szSMTPMailServer[IMOS_URL_LEN];

    /** SMTP�ʼ��������˿� */
    ULONG   ulPortNum;

    /** SMTP�ʼ��������Ƿ�Ҫ����֤ */
    BOOL_T  bRequireAuthentication;

    /** ��֤ʹ�õ��û��� */
    CHAR    szUserName[IMOS_NAME_LEN];

    /** ��֤ʹ�õ����� */
    CHAR    szPassword[IMOS_PASSWD_LEN];

    /** ����Ⱥ����������� */
    ULONG   ulEmailSenderGroupNum;
} EMAIL_SENDING_CONFIG_S;

/**
* @struct tagSMSSendingConfig
* @brief ����ҵ������������Ϣ
* @attention
*/
typedef struct tagSMSSendingConfig
{
    /** IP��ַ����:IPV4ȡֵΪ#IMOS_IPADDR_TYPE_IPV4, IPV6ȡֵΪ#IMOS_IPADDR_TYPE_IPV6 */
    ULONG   ulIPAddrType;

    /** ���ŷ�����IP��ַ */
    CHAR    szIPAddress[IMOS_IPADDR_LEN];

    /** ���ŷ������˿� */
    USHORT  usServerPort;

    /** ����λ, �����ֽڶ���, ��ʵ�ʺ��� */
    CHAR    acReserved[2];
}SMS_SENDING_CONFIG_S;

/**
* @struct tagSMSTestSendingConfig
* @brief ���Ų��Է��͵��������
* @attention
*/
typedef struct tagSMSTestSendingConfig
{
    /** ����ҵ������������Ϣ */
    SMS_SENDING_CONFIG_S  stSMSSendingConfig;

    /** �������ֻ����� */
    CHAR  szSMSRecvMobilePhone[IMOS_MOBILE_LEN];

} SMS_TEST_SENDING_CONFIG_S;

/**
* @struct tagAssetInfo
* @brief �ʲ���Ϣ�ṹ
* @attention
*/
typedef struct tagAssetInfo
{
    /** �ʲ���� */
    CHAR    szAssetCode[IMOS_CODE_LEN];

    /** �ʲ����� */
    CHAR    szAssetName[IMOS_NAME_LEN];

    /** �ʲ����ͣ�ȡֵΪ#ASSET_TYPE_E */
    ULONG   ulAssetType;

    /** �ʲ�״̬��ȡֵΪ#ASSET_STATUS_E */
    ULONG   ulAssetStatus;

    /** �ʲ������豸���� */
    CHAR    szDevCode[IMOS_CODE_LEN];

    /** �ʲ������豸���� */
    CHAR    szDevName[IMOS_NAME_LEN];

    /** �ʲ�ֱ����֯���� */
    CHAR    szParOrgCode[IMOS_CODE_LEN];

    /** �ʲ�ֱ����֯���� */
    CHAR    szParOrgName[IMOS_NAME_LEN];

    /** �ʲ��ͺ� */
    CHAR    szAssetModel[IMOS_NAME_LEN];

    /** �ʲ����� */
    CHAR    szManufacturer[IMOS_NAME_LEN];

    /** �ʲ�IP */
    CHAR    szIpAddress[IMOS_IPADDR_LEN];

    /** ����ʱ�� */
    CHAR    szPurchaseTime[IMOS_TIME_LEN];

    /** ��װʱ�� */
    CHAR    szInstallTime[IMOS_TIME_LEN];

    /** ����ʱ�� */
    CHAR    szWarrantTime[IMOS_TIME_LEN];

    /** ������ */
    CHAR    szProdealer[IMOS_NAME_LEN];

    /** ��װ�ص� */
    CHAR    szLocation[IMOS_NAME_LEN];

    /** MAC��ַ */
    CHAR    szMacAddr[IMOS_MAC_ADDR_LEN];

    /** ά���� */
    CHAR    szMPerson[IMOS_NAME_LEN];

    /** ά����ϵ�绰 */
    CHAR    szTelephone[IMOS_PHONE_LEN];

    /** ���Ŀ�� */
    CHAR    szTarget[IMOS_NAME_LEN];

    /** ���� */
    CHAR    szLongitude[IMOS_STRING_LEN_32];

    /** γ�� */
    CHAR    szLatitude[IMOS_STRING_LEN_32];

    /** ���� */
    CHAR    szaltitude[IMOS_STRING_LEN_32];
}ASSET_INFO_S;

/**
* @struct tagAssetStatistic
* @brief �ʲ�ͳ��(��ѯ�ʲ�ͳ�Ƶ�ʱ�򷵻�)
* @attention
*/
typedef struct tagAssetStatistic
{
    /** ������Ŀ(����)�����ʲ�����(ȡֵ�μ�#ASSET_TYPE_E )����ͳ��ʱʹ�� */
    ULONG  ulCatalog;

    /** ������Ŀ(�ַ���)�����ʲ��ͺŷ���ͳ��ʱʹ�� */
    CHAR  szCatalog[IMOS_STRING_LEN_64];

    /** ���� */
    ULONG  ulTotalNum;

    /** �������� */
    ULONG  ulNormalNum;

    /** ���ϸ��� */
    ULONG  ulFaultNum;

    /** ά�޸��� */
    ULONG  ulMaintenanceNum;

    /** ���ϸ��� */
    ULONG  ulRetirementNum;

    /** ������ */
    ULONG  ulStockNum;

    /** ����װ���� */
    ULONG  ulToInstallNum;

    /** �Ѱ�װ���� */
    ULONG  ulInstalledNum;

    /** �ѵ��Ը��� */
    ULONG  ulDebuggedNum;
}ASSET_STATISTIC_S;

/**
* @struct tagFaultDevInfo
* @brief �����豸��Ϣ(��ѯ�����豸�б��ʱ�򷵻�)
* @attention
*/
typedef struct tagFaultDevInfo
{
    /** �豸���� */
    CHAR  szDevCode[IMOS_CODE_LEN];

    /** �豸���� */
    CHAR  szDevName[IMOS_NAME_LEN];

    /** �豸ֱ����֯���� */
    CHAR  szParOrgCode[IMOS_CODE_LEN];

    /** �豸ֱ����֯���� */
    CHAR  szParOrgName[IMOS_NAME_LEN];

    /** �豸���� */
    ULONG  ulDevType;

    /** �豸�ͺ� */
    CHAR  szDevModel[IMOS_NAME_LEN];

    /** �豸����״̬��ȡֵΪ#IMOS_DEV_FAULT_STATUS_E */
    ULONG  ulDevStatus;

    /** �豸IP��ַ */
    CHAR szIPAddr[IMOS_IPADDR_LEN];

    /** ����ʱ�� */
    CHAR szFaultTime[IMOS_TIME_LEN];
}FAULT_DEV_INFO_S;


/**
* @struct tagFaultStatistic
* @brief ����ͳ��(��ѯ����ͳ�Ƶ�ʱ�򷵻�)
* @attention
*/
typedef struct tagFaultStatistic
{
    /** �豸���ͣ�ȡֵ�μ�#IMOS_TYPE_E */
    ULONG  ulDevType;

    /** ���� */
    ULONG  ulTotalNum;

    /** ���ϸ��� */
    ULONG  ulFaultNum;

    /** ����ʱ�� */
    CHAR szFaultTime[IMOS_TIME_LEN];
}FAULT_STATISTIC_S;
/**
 * @struct tagSThirdpartyIPC
 * @brief ������IPC��Ϣ
 * @attention
 */
typedef struct tagThirdpartyIPC
{
    /* �����������Ϣ */
    CAMERA_INFO_S  stBaseCameraInfo;

    /* ��������, 0 onvif1.0 1 onvif2.0 2 onvif2.1 */
    ULONG  ulCameraManutype;

    /*�豸��ַ*/
    CHAR  szDevAddr[IMOS_IPADDR_LEN];

    /*�豸�˿�*/
    ULONG  ulDevPort;

    /* �������ͣ�0 ����ģ������� 1 IPC   */
    ULONG  ulCameraAccessType;

    /* �û��� */
    CHAR szUserName[IMOS_NAME_LEN];

    /* ���� */
    CHAR szPassword[IMOS_PASSWD_LEN];

    /* ���DVRͨ��ID,���� */
    ULONG ulCameraChnId;

    /* IPC״̬��1��ʾ���ߣ�0��ʾ���� */
    ULONG ulStatus;

    /* �鲥��ַ */
    CHAR szMulticastAddr[IMOS_IPADDR_LEN];

    /* �鲥�˿�, ��ΧΪ: 10002-65534, �ұ���Ϊż�� */
    ULONG   ulMulticastPort;

    /* IPC������֯���� */
    CHAR  szOrgCode[IMOS_DOMAIN_CODE_LEN];

    /* MSѡ�����Ӧ����, 1Ϊ����Ӧ; 0Ϊָ��MS, 2Ϊ������MS, ȡֵΪ#IMOS_STREAM_SERVER_MODE_E */
    ULONG ulIsAutofit;

    /* ʹ��MS��Ŀ, ��ʵ���������, ����Ӧ����#ulIsAutofitΪ����Ӧ, ��ֵΪ0; ����Ӧ����#ulIsAutofitΪ������Ӧ(��ָ��), ��ֵΪ1 */
    ULONG ulUseMSNum;

    /* MS�����б� */
    CHAR szMSCode[IMOS_MS_MAX_NUM_PER_CHANNEL][IMOS_DEVICE_CODE_LEN];

    /* ���ֱ��ý������Ŀ */
    ULONG ulMaxStreamNum;

    /* �����ֶ� */
    CHAR szReserve[32];
}THIRDPARTY_IPC_S;
/**
 * @struct tagCamStoreBasicQueryItem
 * @brief ������洢��ػ�����Ϣ��(��ѯ�洢�豸��������б�ʱ����)
 * @attention
 */
typedef struct tagCamStoreBasicQueryItem
{
    /** ��������� */
    CHAR        szCamName[IMOS_NAME_LEN];

    /** ��������� */
    CHAR        szCamCode[IMOS_CODE_LEN];

    /** ���������(��̨���̶�)#CAMERA_TYPE_E */
    ULONG       ulCamType;

    /** �洢����(ֱ��,ת��)#STORE_CONFIG_TYPE_E */
    ULONG       ulStoreType;

    /** �洢�����豸����(ֱ��Ĵ洢�����豸Ϊ�洢�豸,ת��Ĵ洢�����豸Ϊ�洢��Դ����BM����) */
    CHAR        szStoreCfgDevCode[IMOS_CODE_LEN];

    /** �Ƿ��ƶ�ֱ���ת��ƻ�, #BOOL_TRUEΪ�ƶ�, #BOOL_FALSEΪδ�ƶ� */
    BOOL_T      bHasStorePlan;

    /** ֱ���ת��ƻ����� */
    CHAR        szStorePlanCode[IMOS_CODE_LEN];

    /** �ƻ�״̬, #BOOL_TRUEΪ�ƻ�����, #BOOL_FALSEΪ�ƻ�δ���� */
    BOOL_T      bPlanStatus;

    /** Ԥ���ֶ� */
    CHAR szReserve[128];
}CAM_STORE_BASIC_QUERY_ITEM_S;

/**
 * @struct tagASDomainSynPushUI
 * @brief ���ͬ��������͸�UI
 * @attention
 */
typedef struct tagASDomainSynPushUI
{
    /** ����������*/
    ULONG ulErrorCode;
    /** ��Ϣ��ҵ������#DOMAIN_SYN_SRVTYPE_E*/
    ULONG ulServiceType;
    /** Ŀ����*/
    CHAR  szDomainCode[IMOS_CODE_LEN];
    /** ʵ������*/
    ULONG ulServiceNum;
}AS_DOMAIN_SYN_PUSHUI_S;

/**
 * @struct tagTopologyServiceInfo
 * @brief VM�ļ������ҵ����Ϣ
 * @attention
 */
typedef struct tagTopologyServiceInfo
{
    /*�������������ʵ������*/
    ULONG ulServiceMonitorNum;
}TOPOLOGY_SERVICE_INFO_S;

typedef struct tagTopologySrvDomain
{
    /** �û���¼��Ϣ */
    USER_LOGIN_ID_INFO_S stUserLogIDInfo;

    /** ����� */
    CHAR szParentDomainCode[IMOS_CODE_LEN];

    /** ����� */
    CHAR szSubDomainCode[IMOS_CODE_LEN];
}TOPOLOGY_SRV_DOMAIN_S;

/**
* @struct tagIMOSSystemParamInfo
* @brief ϵͳ������Ϣ
* @attention
*/
typedef struct tagIMOSSystemParamInfo
{
    /** ϵͳ�������ͣ�ȡֵΪ#IMOS_SYS_PARAM_TYPE_E */
    ULONG  ulSysParamType;

    /** ϵͳ����ֵ */
    CHAR  szSysParamValue[IMOS_SYS_PARAM_LEN];

    /** ϵͳ���������ֶ� */
    CHAR szReserv[64];
}IMOS_SYS_PARAM_INFO_S;

/**
* @struct tagSyncServiceInfo
* @brief ͬ��ҵ����Ϣ
* @attention
*/
typedef struct tagSyncServiceInfo
{
    /** ͬ��ҵ�����ͣ�ȡֵ�μ�#SYNCSRV_TYPE_E */
    ULONG  ulSyncSrvType;

    /** ͬ��ģʽ��ȡֵ�μ�#SYNCSRV_MODE_E */
    ULONG  ulSyncMode;

}SYNC_SERVICE_INFO;

/**
 * @struct tagGuardPositionInfo
 * @brief ����λ��Ϣ
 * @attention
 */
typedef struct tagGuardPositionInfo
{
    /** ����λ�����趨��Ԥ��λ�ı�����Ӧ */
    ULONG   ulGuardPosition;

    /** �Զ�����ʱ��, ��λΪ��, ��󲻳���3600��, 0��ʾ������ */
    ULONG   ulAutoGuard;
 }GUARD_POSITION_INFO_S;
/**
* @struct tagDevFaultDetailStatistic
* @brief �豸 ������ϸ��Ϣͳ��
* @attention
*/
typedef struct tagDevFaultDetailStatistic
{
    /** �豸���� */
    CHAR  szDevCode[IMOS_CODE_LEN];

    /** �豸���� */
    CHAR  szDevName[IMOS_NAME_LEN];

    /** �豸���� */
    ULONG  ulDevType;

    /** �豸�ͺ� */
    CHAR  szDevModel[IMOS_NAME_LEN];

    /** �豸IP��ַ */
    CHAR szIPAddr[IMOS_IPADDR_LEN];

    /** ����ʱ��(��λ����) */
    ULONG ulFaultInterval;

    /** ���ϴ��� */
    ULONG ulFaultCount;

    /** �豸ֱ������֯���� */
    CHAR szParentCode[IMOS_CODE_LEN];

    /** �豸ֱ������֯���� */
    CHAR szParentName[IMOS_NAME_LEN];
}DEV_FAULT_DETAIL_STAT_S;

/**
* @struct tagDevFaultDetail
* @brief �豸������ϸ��Ϣ
* @attention
*/
typedef struct tagDevFaultDetail
{
    /** �豸���� */
    CHAR  szDevCode[IMOS_CODE_LEN];

    /** �豸���� */
    CHAR  szDevName[IMOS_NAME_LEN];

    /** �豸���� */
    ULONG  ulDevType;

    /** �豸�ͺ� */
    CHAR  szDevModel[IMOS_NAME_LEN];

    /** �豸����״̬��ȡֵΪ#IMOS_DEV_FAULT_STATUS_E */
    ULONG  ulDevStatus;

    /** �豸IP��ַ */
    CHAR szIPAddr[IMOS_IPADDR_LEN];

    /** ����ʱ�� */
    CHAR szFaultTime[IMOS_TIME_LEN];

    /** ���ϻָ�ʱ�� */
    CHAR szRecoverTime[IMOS_TIME_LEN];

    /** ����ʱ��(��λ����) */
    ULONG ulFaultInterval;

    /** �豸ֱ������֯���� */
    CHAR szParentCode[IMOS_CODE_LEN];

    /** �豸ֱ������֯���� */
    CHAR szParentName[IMOS_NAME_LEN];
}DEV_FAULT_DETAIL_S;

/**
 * @struct tagTimeSlice
 * @brief ����ʱ��Ƭ��Ϣ
 * @attention 00:00:00~23:59:59��ʾһ�����ʱ��Ƭ
 */
typedef struct tagQueryTimeSlice
{
    /* �Ƿ���Ҫ����ʱ��Ƭɸѡ */
    BOOL_T bIsNeed;

    /** ��ʼʱ�� ��ʽΪ"hh:mm:ss" */
    CHAR        szBeginTime[IMOS_TIME_LEN];

    /** ����ʱ�� ��ʽΪ"hh:mm:ss" */
    CHAR        szEndTime[IMOS_TIME_LEN];
}QUERY_TIME_SLICE_S;

/**
* @union tagVocDstInfo
* @brief ����Ŀ�����Ͻṹ
* @attention ��
*/
typedef union tagVocDstInfo
{
    /** Ŀ���û���Ϣ */
    USER_LOGIN_ID_INFO_S stDstUserInfo;

    /** Ŀ����������� */
    CHAR szCameraCode[IMOS_CODE_LEN];
}VOC_DST_INFO_U;

/**
* @struct tagVocSrvDstInfo
* @brief ����ҵ��Ŀ����Ϣ�ṹ
* @attention ��
*/
typedef struct tagVocSrvDstInfo
{
     /** Ŀ�����ͣ���μ�IMOS_TYPE_E */
     ULONG  ulDstType;

     /** Ŀ�ı����й���Ϣ */
     VOC_DST_INFO_U unDstInfo;
}VOC_SRV_DST_INFO_S;

/**
* @struct tagVocSrvRecordInfo
* @brief ����ҵ���¼��Ϣ�ṹ
* @attention ��
*/
typedef struct tagVocSrvRecordInfo
{
     /** Ŀ����Ϣ */
     VOC_SRV_DST_INFO_S  stDstInfo;

     /** Ŀ������ */
     CHAR   szDstName[IMOS_NAME_LEN];

     /** Ŀ���Ƿ������з� */
     BOOL_T bCaller;

     /** ����ҵ���������к� */
     ULONG  ulReqSeq;

     /** ����ҵ�����ͣ���μ�SRV_TYPE_E */
     ULONG  ulVocSrvType;

     /** ����ҵ��״̬����μ�VOCSRV_STATUS_E */
     ULONG  ulVocSrvState;

     /** ����ҵ�������� */
     ULONG  ulVocOperResult;

}VOCSRV_RECORD_INFO_S;

/**
* @struct tagCSVocReqNotify
* @brief SOI ����ҵ������֪ͨ��Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSVocReqNotify
{
    BOOL_T  bNewReq;                     /* �Ƿ����½�����֪ͨ */
    ULONG   ulReqSequence;               /* �������к� */
    ULONG   ulVocType;                   /* ����ҵ������, ֵ�����SRV_TYPE_E */
    USER_LOGIN_ID_INFO_S  stCallerInfo;  /* �����û���Ϣ */
    CHAR    szCallerName[IMOS_NAME_LEN]; /* �����û����� */
}CS_VOC_REQ_NOTIFY_S;

/**
* @struct tagCSVocStateNotify
* @brief SOI ����ҵ��״̬ͬ־��Ϣ�ṹ
* @attention ��
*/
typedef struct tagCSVocStateNotify
{
    ULONG   ulEventType;                               /* �¼��������ͣ�ö��ֵΪ:VOC_EVENT_TYPE_E */
    ULONG   ulTroubleCode;                             /* �쳣�¼���ϸ�����룬����������뺬�� */
    CHAR    szCreatUserName[IMOS_NAME_LEN];            /* �����¼����û������� */
    CHAR    szCreatUserIpAddress[IMOS_IPADDR_LEN];     /* �����¼����û��ĵ�¼IP */
    CHAR    szCreatUserLoginCode[IMOS_CODE_LEN];       /* �����¼����û��ĵ�¼ID */

    ULONG   ulOrgSrvType;                              /* �¼�����ǰԭ��ҵ�����ͣ�ö��ֵΪ:SRV_TYPE_E */
    CHAR    szOrgDstCode[IMOS_CODE_LEN];               /* ԭҵ������������ */
    CHAR    szOrgDstName[IMOS_NAME_LEN];               /* ԭҵ����������� */
    CHAR    szOrgDstDomainCode[IMOS_CODE_LEN];         /* ԭҵ������������������ */
    CHAR    szOrgDstDomainName[IMOS_NAME_LEN];         /* ԭҵ����������������� */

    ULONG   ulCurSrvType;                              /* �¼��������ҵ�����ͣ�ö��ֵΪ:SRV_TYPE_E */
    CHAR    szCurDstCode[IMOS_CODE_LEN];               /* ��ҵ������������ */
    CHAR    szCurDstName[IMOS_NAME_LEN];               /* ��ҵ����������� */
    CHAR    szCurDstDomainCode[IMOS_CODE_LEN];         /* ��ҵ������������������ */
    CHAR    szCurDstDomainName[IMOS_NAME_LEN];         /* ��ҵ����������������� */
}CS_VOC_STATE_NOTIFY_S;

/**
* @struct tagQueryResRequestInfo
* @brief ��Դ��ѯ������Ϣ�ṹ��
* @attention
*/
typedef struct tagQueryResRequestInfo
{
    /** ��Դ���� */
    CHAR                    szResCode[IMOS_CODE_LEN];

    /** ��Դ����,ȡֵ��ΧΪ#IMOS_TYPE_E */
    ULONG                   ulResType;
}QUERY_RES_REQUEST_INFO_S;

/**
 * @struct tagShareResBase
 * @brief ������Դ������Ϣ
 * @attention
 */
typedef union tagShareResBase
{
    /** ������֯������Ϣ */
    SHARE_ORG_BASE_S  stOrgShareInfo;

    /** ���������������Ϣ */
    SHARE_CAMERA_BASE_V2_S stCamShareInfo;

    /** ����澯Դ������Ϣ */
    SHARE_ALARMSRC_BASE_S stAlaramSrcShareInfo;

    /** �����ͼ������Ϣ */
    SHARE_MAP_BASE_S    stMapShareInfo;

    /** ������������Ϣ */
    SHARE_SCENE_BASE_S   stShareSceneInfo;
} SHARE_RES_BASE_U;

/**
* @struct tagResShareInRecordInfo
* @brief ��Դ����������(IN)�ṹ��
* @attention
* 1.���Ǳ�����Դ����д�������ƺͱ���
*/
typedef struct tagResShareInRecordInfo
{
    /** ��Դ���������� */
    CHAR                    szResDomainName[IMOS_NAME_LEN];

    /** ��Դ��������� */
    CHAR                    szResDomainCode[IMOS_CODE_LEN];
}RES_SHARE_IN_RECORD_INFO_S;

/**
* @struct tagResShareOutRecordInfo
* @brief ��Դ��������(OUT)�ṹ��
*/
typedef struct tagResShareOutRecordInfo
{
    /** ������Դ������Ϣ */
    SHARE_RES_BASE_U        unShareResBaseInfo;

    /** ��Դ������(OUT)���� */
    CHAR                    szDomainName[IMOS_NAME_LEN];

    /** ��Դ������(OUT)���� */
    CHAR                    szDomainCode[IMOS_CODE_LEN];

    /** ��������(ֻ������:�ϼ���/ƽ����), ȡֵΪ#EX_DOMAIN_TYPE_E */
    ULONG   ulOuterDomainType;

    /** �����ַ���ͣ�1-IPv4 2-IPv6 */
    ULONG   uladdrtype;

    /** �����ַ */
    CHAR    szDomainAddr[IMOS_IPADDR_LEN];

    /** ��������
        ���ڷ�OEM�汾(VM8500��H3C�豸),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_VMISC
            #IMOS_MANUFACTURER_TYPE_DA
            #IMOS_MANUFACTURER_TYPE_OTHER
        ����OEM�汾(PL/PL-S),ȡֵΪ
            #IMOS_MANUFACTURER_TYPE_PL
            #IMOS_MANUFACTURER_TYPE_PLS
            #IMOS_MANUFACTURER_TYPE_OTHER */
    CHAR szManufacturerType[IMOS_STRING_LEN_64];

    /** �����е���ȼ�����Ч��ΧΪ1��7 */
    ULONG ulDomainLevel;

    /** ���ͨѶЭ�����ͣ�ȡֵΪ#INTERDOMAIN_PROTOCOL_TYPE_E */
    ULONG ulProtocolType;

    /** ����ע��״̬��ָ������ǰ�������ע���Ƿ�ɹ�, 1Ϊ�ɹ�; 0Ϊʧ�� */
    ULONG                     ulActiveRegSta;

    /** ����ע��״̬��ָ��ǰ�����������ע���Ƿ�ɹ�, 1Ϊ�ɹ�; 0Ϊʧ�� */
    ULONG                     ulRecvRegSta;

    /** �����ֶ� */
    CHAR szReserve[128];
}RES_SHARE_OUT_RECORD_INFO_S;

/**
* @struct tagAlarmRecordInfo
* @brief �澯��¼
* @attention
*/
typedef struct tagAlarmRecordInfo
{
    /** �澯���� */
    CHAR szEventCode[IMOS_CODE_LEN];

    /** �澯��¼���ͣ�ȡֵΪ#ALARM_RECORD_TYPE_E */
    ULONG ulAlarmRecordType;

    /** ִ���û����� */
    CHAR szExecuteUserName[IMOS_NAME_LEN];

    /** ִ��ʱ�� */
    CHAR szExecuteTime[IMOS_TIME_LEN];

    /** �û�IP��ַ */
    CHAR szUserAddr[IMOS_IPADDR_LEN];

    /** ��¼���� */
    CHAR szExecuteDesc[IMOS_STRING_LEN_512];

    /** �¸澯���룬Ϊ�ձ�ʾ���ǲ����¸澯���¸澯û�и澯�����¼ */
    CHAR szNewEventRecordCode[IMOS_CODE_LEN];
}ALARM_RECORD_INFO_S;

/**
* @struct tagEndTaskBase
* @brief �������������Ϣ
* @attention
*/
typedef struct tagEndTaskBase
{
    /** ������ţ�Ϊ0��ʾ������������ */
    ULONG ulTaskSequence;

    /** ������ */
    CHAR  szTaskName[IMOS_NAME_LEN];
}END_TASK_BASE_S;

/**
* @struct tagEndTaskAction
* @brief ��������������
* @attention
*/
typedef struct tagEndTaskAction
{
    /** ������Ŀ */
    ULONG ulEndTaskNum;

    /** �������������Ϣ */
    END_TASK_BASE_S astEndTaskBase[IMOS_MAX_TASK_NUM];
}END_TASK_ACTION_S;

/**
* @struct tagStartTaskBase
* @brief �������������Ϣ
* @attention
*/
typedef struct tagStartTaskBase
{
    /** ������� */
    ULONG ulTaskSequence;

    /** ������ */
    CHAR  szTaskName[IMOS_NAME_LEN];

    /** �ȴ�ʱ�����������������λ:�룩*/
    ULONG ulTime;
}START_TASK_BASE_S;

/**
* @struct tagStartTaskAction
* @brief ��������������
* @attention
*/
typedef struct tagStartTaskAction
{
    /** ������Ŀ */
    ULONG ulStartTaskNum;

    /** �������������Ϣ */
    START_TASK_BASE_S astStartTaskBase[IMOS_MAX_TASK_NUM];
}START_TASK_ACTION_S;

/**
* @struct tagTranAlarmAction
* @brief ����ת��ԭ�澯
* @attention
*/
typedef struct tagTranAlarmAction
{
    /** ת���û���Ŀ */
    ULONG ulTranUserNum;

    /** ת���û���Ϣ(����ʱ�û�������Ч��������Ч����ѯʱ�û���¼ID���û���¼��ַ���û���½ʱ�䡢�û��Ƿ�������Ч��������Ч) */
    USER_QUERY_ITEM_S astTranUserInfo[IMOS_ALARM_ACTION_TRAN_MAXNUM];
}TRAN_ALARM_ACTION_S;

/**
* @struct tagCreateNewAlarmAction
* @brief ���������¸澯
* @attention
*/
typedef struct tagCreateNewAlarmAction
{
     /** �¸澯��Ŀ */
    ULONG ulNewAlarmNum;

    /** �¸澯��Ϣ */
    EVENT_TYPE_INFO_S astNewAlarmBase[IMOS_MAX_NEW_ALARM_NUM];
}CREATE_NEW_ALARM_ACTION_S;

/**
* @struct tagPlanAction
* @brief Ԥ��������������
* @attention
*/
typedef struct tagPlanAction
{
    /** ������������ */
    END_TASK_ACTION_S           stEndTaskAction;

    /** ������������ */
    START_TASK_ACTION_S         stStartTaskAction;

    /** ת��ԭ�澯 */
    TRAN_ALARM_ACTION_S         stTranAlarmAction;

    /** �����¸澯 */
    CREATE_NEW_ALARM_ACTION_S   stCreateNewAlarmAction;
}TASK_ACTION_S;

/**
* @struct tagTaskActionInfo
* @brief ��������
* @attention
*/
typedef struct tagTaskActionInfo
{
    /** �澯�������� */
    ALARM_ACTION_S stNormalAlarmAction;

    /** Ԥ�������еĶ��� */
    TASK_ACTION_S stPlanAction;
}TASK_ACTION_INFO_S;

/**
* @struct tagTaskTriggerInfo
* @brief �������ṹ
* @attention
*/
typedef struct tagTaskTriggerInfo
{
    /** ������ID��ȡֵΪ#TRIGGER_ID_E */
    ULONG ulTriggerID;

    /** ���������� */
    CHAR szTriggerName[IMOS_NAME_LEN];

    /** �������������� */
    ULONG ulParamNum;

    /** ��������������(��Ӣ��;�ָ�*/
    CHAR szParamName[IMOS_DESC_LEN];

    /** ����������ֵ(��Ӣ��;�ָ� */
    CHAR szParamValue[IMOS_DESC_LEN];
}TASK_TRIGGER_INFO_S;

/**
* @struct tagActionPlanTaskInfo
* @brief ����ṹ
* @attention
*/
typedef struct tagPlanTaskInfo
{
    /** ������� */
    ULONG ulTaskSequence;

    /** �������� */
    CHAR szTaskName[IMOS_NAME_LEN];

    /** ����ִ�в��ԣ�ȡֵΪ#TASK_EXEC_STRATEGY_E */
    ULONG ulTaskExecStrategy;

    /** �ֶ�ִ�г�ʱʱ��(��λ:�룬�ֶ�ִ��ʱ���ֶβ���Ч) */
    ULONG ulOverTime;

    /** ���������� */
    ULONG ulTriggerNum;

    /** �������б� */
    TASK_TRIGGER_INFO_S astTriggerList[IMOS_MAX_TRIGGER_NUM];

    /** �������ṹ */
    TASK_ACTION_INFO_S stTaskActionInfo;
}ACTION_PLAN_TASK_INFO_S;

/**
* @struct tagActionPlanInfo
* @brief Ԥ����Ϣ
* @attention
*/
typedef struct tagActionPlanInfo
{
    /** Ԥ������ */
    CHAR szActionPlanCode[IMOS_CODE_LEN];

    /** Ԥ������ */
    CHAR szActionPlanName[IMOS_NAME_LEN];

    /** Ԥ�����ͣ�ȡֵΪ#ACTION_PLAN_TYPE_E */
    ULONG ulActionPlanType;

    /** Ԥ�����ԣ�ȡֵΪ#PLAN_EXEC_STRATEGY_E */
    ULONG ulExecStrategy;

    /** Ԥ������ */
    CHAR szActionPlanDesc[IMOS_DESC_LEN];

    /** Ԥ���������� */
    ULONG ulTaskNum;

    /** Ԥ�������б� */
    ACTION_PLAN_TASK_INFO_S astTaskList[IMOS_MAX_TASK_NUM];
}ACTION_PLAN_INFO_S;

/**
* @struct tagPlanTaskBaseInfo
* @brief ��������ṹ
* @attention
*/
typedef struct tagPlanTaskBaseInfo
{
    /** ������� */
    ULONG ulTaskSequence;

    /** �������� */
    CHAR szTaskName[IMOS_NAME_LEN];

    /** ������ʽ��ȡֵΪ#TASK_DEAL_MODE_E */
    ULONG ulTaskDealMode;
}PLAN_TASK_BASE_INFO_S;

/**
* @struct tagPlanAlarmChgPushUI
* @brief ���͸�UI��Ԥ���澯�仯���ͽṹ
* @attention
*/
typedef struct tagPlanAlarmChgPushUI
{
    /** �澯��¼��Ϣ */
    EVENT_RECORD_S stEventRecord;

    /** Ԥ���澯�仯���ͣ�ȡֵΪ#PLAN_ALARM_CHG_TYPE_E */
    ULONG ulChgType;
}PLAN_ALARM_CHG_PUSH_UI_S;

/**
* @struct tagPlanAlarmChgExPushUI
* @brief Ԥ���澯�仯��չ������Ϣ
* @attention
*/
typedef struct tagPlanAlarmChgExPushUI
{
    /** Ԥ���澯�仯������Ϣ */
    PLAN_ALARM_CHG_PUSH_UI_S stPlanAlarmChgPushUI;

    /** �û���½��Ϣ��id */
    CHAR    szUserLoginId[IMOS_STRING_LEN_32];

    /** SDK Module Name */
    CHAR    szSDKModuleName[IMOS_NAME_LEN];

}PLAN_ALARM_CHG_EX_PUSH_UI_S;

/**
* @struct tagQuerySysInfo
* @brief ��ѯϵͳ��Ϣ
* @attention
*/
typedef struct tagQuerySysInfo
{
    /** ��ѯ��Ϣ���֣�ȡֵΪ# QUERY_SYS_INFO_NAME_E */
    ULONG ulQuerySysInfoName;

    /** ��ѯֵ */
    CHAR szQueryValue[IMOS_DESC_LEN];
}QUERY_SYS_INFO_S;

/**
* @struct tagExAutoShareInfo
* @brief ��֯�Զ������������Ϣ�ṹ
*/
typedef struct tagExAutoShareInfo
{
    /** ������� */
    CHAR szExDomainCode[IMOS_CODE_LEN];

    /** ��֯���� */
    CHAR szOrgCode[IMOS_CODE_LEN];

    /** �Ƿ��Զ����������#BOOL_TRUEΪ�Զ���������򣬷�֮ȡֵΪ#BOOL_FALSE */
    BOOL_T bIsAutoShare;

    /** (�����Ƿ��Զ����������ʱ)�Ƿ��������֯��#BOOL_TRUEΪ��������֯����֮ȡֵΪ#BOOL_FALSE */
    BOOL_T bDoesContainSub;

    /** �����ֶ� */
    CHAR szReserved[128];
} EX_AUTO_SHARE_INFO_S;

/**
 * @struct tagShareUnSucceedRes
 * @brief δ����ɹ���Դ��Ϣ
 * @attention
 */
typedef struct tagShareUnSucceedRes
{
    /** δ�ɹ�ԭ���룬#ERR_COMMON_SUCCEED��ʾ���������У�����ֵ����ʾ����ʧ�ܣ�ֵ��Ӧ�Ĵ������ͼ��������б�*/
    ULONG ulDetailCode;

    /** ��Դ���� */
    ULONG ulResType;

    /** ��Դ���� */
    CHAR  szResCode[IMOS_CODE_LEN];

    /** ��Դ���� */
    CHAR  szResName[IMOS_NAME_LEN];

    /** ��Դ�������� */
    CHAR  szResShareCode[IMOS_CODE_LEN];

    /** ��Դ������ */
    CHAR  szResParentCode[IMOS_CODE_LEN];

    /** ��Դ���� */
    CHAR  szResParentName[IMOS_NAME_LEN];

    /** ��Դ�����Ȩ�� */
    ORG_AUTHORITY_V2_S  stAuthority;
}SHARE_UNSUCCEED_RES_S;

/**
* @struct tagEventEnableInfo
* @brief �¼�ʹ�ܲ�����Ϣ�ṹ
* @attention
*/
typedef struct tagEventEnableInfo
{

    /** �¼�ϵͳĬ������ */
    CHAR    szPreEventName[IMOS_NAME_LEN];

    /** �¼�ϵͳĬ�ϼ���, ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    ULONG   ulPreEventSecurity;

    /** �¼�ϵͳ�Զ�������, ϵͳ�¼�ʹ�ܲ�����ʾ�¼�ʹ������ */
    CHAR    szSysEventName[IMOS_NAME_LEN];

    /** �¼�ϵͳ�Զ��弶��, ȡֵΪ#ALARM_SEVERITY_LEVEL_E, ϵͳ�¼�ʹ�ܲ�����ʾ�¼�ʹ�ܼ��� */
    ULONG   ulSysEventSecurity;

    /** �¼�ʹ�������� */
    CHAR    szEventName[IMOS_NAME_LEN];

    /** �¼�ʹ�ܼ���, ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    ULONG   ulEventSecurity;

    /** �Ƿ��Զ�ȷ��, #BOOL_TRUEΪ�Զ�ȷ��, #BOOL_FALSE���Զ�ȷ��, Ĭ��ΪBOOL_TRUE */
    BOOL_T   bAutoAck;

    /** �Ƿ�ʹ��, #BOOL_TRUEΪʹ��, #BOOL_FALSEΪδʹ��, Ĭ��ΪBOOL_TRUE */
    BOOL_T   bEnable;

}EVENT_ENABLE_INFO_S;

/**
* @struct tagBathEventEnableInfo
* @brief  ���������¼�ʹ�ܲ�����Ϣ�ṹ
* @attention
*/
typedef struct tagBathEventEnableInfo
{
    /** ��Ҫ�Զ���ĸ澯����, ȡֵΪ#AlARM_TYPE_E, 0��ʾΪȫ�� */
    ULONG       ulEventType;

    /** ������ĸ澯Դ���� */
    ULONG       ulDevNum;

    /** ������澯Դ��Ϣ */
    ALARM_SRC_QUERY_ITEM_S astAlarmSrcList[IMOS_PAGE_QUERY_ROW_MAX_NUM];

} BATH_EVENT_ENABLE_INFO_S;
/**
 * @struct tagSetNetworkAdaptInfo
 * @briefʵ��������Ӧ����Ϣ
 * @attention
 */
typedef struct tagSetNetworkAdaptInfo
{
     /** ʵ��������Ӧ���� 1�� 2�� */
    ULONG  ulMonitorNetworkAdaptEnable;

    /**�洢 ������Ӧ���� 1�� 2�� */
    ULONG  ulStoreNetworkAdaptEnable;

    /** ������Ӧģʽ   0:�ر� 1��ͼ������2��֡������ */
    ULONG  ulNetworkAdaptMode;

    /** ʵ����������Ӧ�ָ�ʱ�� */
    ULONG  ulNetworkAdaptCount;

     /**����  */
    UCHAR   szReserved[128];
} NETWORK_ADAPT_INFO_S;

/**
* @struct tagEventTypeL2oRel
* @brief ������澯���Ͷ�Ӧ��ϵ
* @attention
*/
typedef struct tagEventTypeL2oRel
{
    /** ��ϵ����ID */
    CHAR szEventTypeL2ORelCode[IMOS_CODE_LEN];

	 /* Э������ */
    ULONG ulProtocolType;

    /** �����¼����� */
    ULONG ulH3cEventType;

    /** Э���¼����� */
    ULONG  ulProtocolEventType;

    /** �澯״̬ */
    ULONG   ulEventStatus;

    /** ��ϵ���� */
    CHAR   szRelDesc[IMOS_DESC_LEN];

}EVENT_TYPE_L2OREL_S;

/**
 * @struct tagMapColors
 * @brief GIS��ͼ��ɫ
 * @attention
 */
typedef struct tagMapColors
{
	/** ��ͼ�����ɫ */
    ULONG                   ulFillZone;

	/** ��ͼ������ɫ */
    ULONG                   ulLine;

	/** ɽ��������ɫ */
    ULONG                   ulHills;

	/** ��ˮ������ɫ */
    ULONG                   ulDeepWater;

	/** ����������ɫ */
	ULONG                   ulCity;

	/** ǳˮ������ɫ */
	ULONG                   ulShallowWater;
}FILE_MAP_COLORS_S;

/**
* @struct tagEventSecurityType
* @brief �¼�����
* @attention
*/
typedef struct tagEventSecurityType
{
    /** �������� */
    ULONG  ulSecurityType;

    /** �������� */
    CHAR   szSecurityName[IMOS_NAME_LEN];

    /** ������ʾ��ɫ����ɫRGBֵ��������(#)�� */
    CHAR  szSecurityColor[IMOS_STRING_LEN_14];

    /** ����������Ϣ */
    CHAR   szDesc[IMOS_DESC_LEN];

    /** ���� */
    UCHAR   szReserved[128];

} EVENT_SECURITY_TYPE_S;

/**
* @struct tagEventSecurityExtType
* @brief �¼�������չ��Ϣ
* @attention
*/
typedef struct tagEventSecurityExtType
{
    /** ������Ϣ */
    EVENT_SECURITY_TYPE_S stEventSecurityType;

    /** �Ƿ���ϵͳԤ����澯���� */
    BOOL_T                bSysEventType;

} EVENT_SECURITY_EXT_TYPE_S;

/**
* @struct tagEventSecurityChgPushUI
* @brief ���͸�UI�ļ�����Ϣ�仯���ͽṹ
* @attention
*/
typedef struct tagEventSecurityChgPushUI
{
    /** �澯������Ϣ */
    EVENT_SECURITY_TYPE_S stEventSecurity;

    /** �澯����仯���ͣ�ȡֵΪ#EVENT_SECURITY_CHG_TYPE_E */
    ULONG ulChgType;
} EVENT_SECURITY_CHG_PUSH_UI_S;

/**
* @struct tagEventSecurityChgExPushUI
* @brief �¼�����仯��չ������Ϣ
* @attention
*/
typedef struct tagEventSecurityChgExPushUI
{
    /** �¼�����仯������Ϣ */
    EVENT_SECURITY_CHG_PUSH_UI_S stEventSecurityChgPushUI;

    /** �û���½��Ϣ��id */
    CHAR    szUserLoginId[IMOS_STRING_LEN_32];

    /** SDK Module Name */
    CHAR    szSDKModuleName[IMOS_NAME_LEN];

} EVENT_SECURITY_CHG_EX_PUSH_UI_S;

/**
/**
* @struct tagCSPtzStatusNotify
* @brief SOI ��̨״̬�����Ϣ �ṹ
* @attention ��
*/
typedef struct tagCSPtzStatusNotify
{
    CHAR    szUserName[IMOS_NAME_LEN];                  /* �û����� */
    CHAR    szUserIpAddress[IMOS_IPADDR_LEN];           /* ��ռ�����û��ĵ�¼IP */
    CHAR    szCameraCode[IMOS_DEVICE_CODE_LEN];         /* ��������� */
    CHAR    szCameraName[IMOS_NAME_LEN];                /* ��������� */
    ULONG   ulNotifyType;                               /* ֪ͨ���� ��Ӧö��:PTZ_NOTIFY_UI_EVENT_TYPE */
	ULONG   ulAutoGuard;                                /* ��̨���ؿ���λʱ�� */
}CS_PTZ_STATUS_NOTIFY_S;


#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif

