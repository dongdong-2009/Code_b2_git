/*******************************************************************************
 Copyright (c) 2008-2009, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              sdk_func_inner.h

  Project Code: IMOS_SDK
   Module Name: SDK
  Date Created: 2010-04-09
        Author: w05867
   Description: SDK�����ⷢ���ĺ����ӿ�
                ע��: ���з�ҳ��ѯʱ�������б�ռ����ⲿ���룬
                      ��С���ݷ�ҳ��ѯʱһҳ�ĸ����������롣


--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_SDK_FUNC_INNER_H_
#define _IMOS_SDK_FUNC_INNER_H_

#include <string>
#include <vector>

#ifdef  __cplusplus
extern "C"{
#endif

/**
* ɨ��IPSAN \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szIpsanCode         IPSAN����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_DM_DEV_NOT_ONLINE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ScanIpsan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szIpsanCode[IMOS_DEVICE_CODE_LEN]
);

/**
* ��ѯ��ǰ��¼�û���ָ����Դ��Ȩ���б�. \n
* �����ǰ��¼�û���ϵͳ����Ա,��ֻ����"�Ƿ���ϵͳ����Ա"��ʶ,������Ȩ���б�;
* �������ϵͳ����Ա,�򷵻�Ȩ���б�(����ϵͳȨ�޺���ԴȨ��).
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   ulResType           ��Դ����(ȡֵ#IMOS_TYPE_E)
* @param [IN]   szResCode           ��Դ����
* @param [OUT]  pbIsSystemMgr       "�Ƿ���ϵͳ����Ա"��ʶ, BOOL_TRUEΪϵͳ����Ա, BOOL_FALSEΪ��ϵͳ����Ա
* @param [OUT]  pstAuthorityInfo    Ȩ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_QUERY_AUTH_NUM_EXCEED_MAX
* -     ���������,�μ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ULONG                   ulResType,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    OUT BOOL_T                  *pbIsSystemMgr,
    OUT ORG_AUTHORITY_S         *pstAuthorityInfo
);

/**
* ������Դ��Ŀ����֯ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szDesOrgCode        Ŀ����֯����
* @param [IN]   szResCode           ��Դ����
* @param [IN]   ulResType           ��Դ����, ȡֵ��#IMOS_TYPE_E, Ŀǰ֧�ֵĺϷ�ȡֵΪ#IMOS_TYPE_EC; #IMOS_TYPE_DC; #IMOS_TYPE_CAMERA; #IMOS_TYPE_MONITOR; #IMOS_TYPE_DM; #IMOS_TYPE_MS; #IMOS_TYPE_VX500
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* - #ERR_DAO_DEVICE_NOT_EXIST
* - #ERR_MM_DEVICE_ALREADY_IN_DOMAIN
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. �������Դû�б��룬�����Դ������Ϊ��Դ����
* - 2. һ����Դ���Ի��鵽�����֯���������ظ����鵽ͬһ����֯
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShareResToDesOrg
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDesOrgCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulResType
);

/**
* ����Դ����֯�н������ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szDesOrgCode        Ŀ����֯����
* @param [IN]   szResCode           ��Դ����
* @param [IN]   ulResType           ��Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_CAMERA_ALREADY_SHARED
* - #ERR_MM_DEVICE_BELONG_TO_ORIGINAL_DOMAIN
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1���������Դû�б��룬�����Դ������Ϊ��Դ����
*/
IMOS_EXPORT ULONG STDCALL IMOS_ReclaimResFromDesOrg
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDesOrgCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulResType
);

/**
* ��ѯ��֯����������������Ϣ�б�ֻ��ȡһ�� \n
* �����ѯ����ʱ,������,��Ĭ������(�������������)
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   szOrgCode          ��֯�ڵ����
* @param [IN]   szExDomainCode     �������
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[���������])
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstCamShareInfo    �����������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCameraShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_CAM_INFO_V2_S             *pstCamShareInfo
);

/**
* ��ѯĳ������������Ӧ�Ľ�������Ϣ
* @param [IN] pstUserLogIDInfo              �û���¼��Ϣ��ʶ
* @param [IN] szScrCode                     ����������
* @param [OUT]pstScrRelatedDC               ��������Ӧ�Ľ������ṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryScreenRelatedDC
(
    IN    USER_LOGIN_ID_INFO_S          *pstUserLogIDInfo,
    IN    CHAR                          szScrCode[IMOS_CODE_LEN],
    OUT   SCREEN_RELATED_DC_S           *pstScrRelatedDC
);

/**
* ��ҳ��ѯ�������豸����������������������������ϵ�ʵʱ��ؼ�¼ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   ulUAType                �豸����,ȡֵ��Χ��#IMOS_TYPE_E
* @param [IN]   szUACode                �����/������/�������
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstMonitorUnit          ʵʱ��ؼ�¼�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_MONITOR_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMonitorRecordUnderUA
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  ULONG                       ulUAType,
    IN  CHAR                        szUACode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT MONITOR_UNIT_S              *pstMonitorUnit
);

/**
* ��ѯ���������������е�������Դ��Ϣ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szMonitorCode           ��Ӳ��ʵ����Ϊ����������; �����ʵ����Ϊ�������
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSwitchResBase        ������Դ������Ϣ
* @param [OUT]  pstSwitchResUnit        ������Դ��Ԫ��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySwitchResInfoUnderScreen
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLoginIDInfo,
    IN   CHAR                       szMonitorCode[IMOS_RES_CODE_LEN],
    IN   QUERY_PAGE_INFO_S          *pstQueryPageInfo,
    OUT  RSP_PAGE_INFO_S            *pstRspPageInfo,
    OUT  SWITCH_RES_BASE_S          *pstSwitchResBase,
    OUT  SWITCH_RES_UNIT_S          *pstSwitchResUnit
);


/**
* ��ѯ�豸�����ڽ��е�ҵ����Ϣ \n
* @param [IN]   pstUserLoginIDInfo  �û���¼ID��Ϣ��ʶ
* @param [IN]   szDevCode           �豸����
* @param [IN]   ulDevType           �豸����, ȡֵΪ#IMOS_TYPE_E, �Ϸ���ȡֵΪ#IMOS_TYPE_MONITOR(������), #IMOS_TYPE_XP_WIN(XP����)
* @param [IN]   szVirDomainCode     ���������
* @param [OUT]  pstSrvInfo          ҵ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻʵ�ֲ�ѯʵʱ��ء����е�ҵ��״̬
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServiceUnderDev
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                   ulDevType,
    IN  CHAR                    szVirDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT SRV_STATE_INFO_S        *pstSrvInfo
);

/**
* ��ѯ��Դ�����ڽ��е�ҵ����Ϣ \n
* @param [IN]   pstUserLoginIDInfo  �û���¼ID��Ϣ��ʶ
* @param [IN]   szResCode           ��Դ����
* @param [IN]   ulResType           ��Դ����, ȡֵΪ#IMOS_TYPE_E, �Ϸ���ȡֵΪ#IMOS_TYPE_SALVO_RESOURCE(����ʾ), #IMOS_TYPE_GROUP_SWITCH_RESOURCE(����Ѳ)
* @param [IN]   szVirDomainCode     ���������
* @param [OUT]  pstSrvInfo          ҵ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻʵ�ֲ�ѯ����ʾ��ҵ��״̬
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServiceUnderRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulResType,
    IN  CHAR                    szVirDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT RES_SRV_STATE_INFO_S    *pstSrvInfo
);

/**
* ��ҳ��ѯ�����㲥��Ϣ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szXPVocBrdCode          �����㲥����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstVocBrdUnit           �����㲥��Ԫ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlayerVoiceBroadcastInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szXPVocBrdCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT VOC_BRD_UNIT_S              *pstVocBrdUnit
);

/**
* ��ѯ����ҵ����Ϣ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szXPVocTalkCode         �����Խ�����
* @param [IN]   szXPVocBrdCode          �����㲥����
* @param [OUT]  szCameraCode            ���������
* @param [OUT]  szCameraName            ���������
* @param [OUT]  pulVoiceSrvType         ����ҵ������, ȡֵΪ#SRV_TYPE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_CS_VOCTALK_NOT_EXIST
* - #ERR_CS_VOCBRD_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryXPVoiceServiceInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  CHAR                            szXPVocTalkCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szXPVocBrdCode[IMOS_RES_CODE_LEN],
    OUT CHAR                            szCameraCode[IMOS_RES_CODE_LEN],
    OUT CHAR                            szCameraName[IMOS_RES_CODE_LEN],
    OUT ULONG                           *pulVoiceSrvType
);

/***************************************************************************
����XP���鲥�ӿ�
****************************************************************************/
/**
* @name ����XP���鲥�ӿڹ���
* @{    ����XP���鲥�ӿڹ���
*/
/**
* ����XP���鲥��Ϣ
* @param [IN]  pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]  ulStreamType                 ���鲥���ͣ�0Ϊ������1Ϊ�鲥
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_XP_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.����Ҳ���xp����Ϣ�����øýӿڻ᷵��ʧ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigXPStreamType
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulStreamType
);

/**
* ��ѯXP���鲥����Ϣ
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [OUT]  pulStreamType                ���鲥���ͣ�0Ϊ�鲥��1Ϊ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_XP_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryXPStreamType
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulStreamType
);

/**
* ���ñ���ǰ���豸���ֱ��ý��������ϵͳ���� \n
* @param [IN]   pstUserLogIDInfo                �û���½��Ϣ
* @param [IN]   ulMaxMediaByPassOfTerminalDev   ǰ���豸���ֱ��ý��������(ȡֵ��Χ1~3)
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. ֻ��admin�û�����Ȩ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CfgMaxMediaByPassOfTerminalDev
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulMaxMediaByPassOfTerminalDev
);

/**
* ��ѯ����ǰ���豸���ֱ��ý��������ϵͳ���� \n
* @param [IN]   pstUserLogIDInfo                �û���½��Ϣ
* @param [OUT]  pulMaxMediaByPassOfTerminalDev  ǰ���豸���ֱ��ý��������(ȡֵ��Χ1~3)
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMaxMediaByPassOfTerminalDev
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulMaxMediaByPassOfTerminalDev
);
/** @} */ /* end of XP STREAM MANAGEMENT MODULE */
/** @} */ /* end of INFO MANAGEMENT MODULE */
#if 0
#endif
/*******************************************************************************
 �豸-EC,DC���ڹ���
*******************************************************************************/
/**
* @name �豸(EC,DC)���ڹ���
* @{    �豸(EC,DC)���ڹ���
*/
/**
* �����豸�Ĵ�����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [IN]   pstSerialInfo           ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_SERIAL_USED
* - #ERR_DAO_SERIAL_PORT_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfSerial
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    IN  SERIAL_INFO_S               *pstSerialInfo
);


/**
* ��ҳ��ѯ�豸�Ĵ����б� \n
* ����Ϊ��ʱ,��ѯ���д���,��Ĭ�ϰ���"������������"����
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szDevCode              �豸����
* @param [IN]   ulDevType              �豸���ͣ�ȡֵ��Χ: #IMOS_TYPE_E
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:INDEX_TYPE[��������])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSerialList          �����б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySerialList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                           ulDevType,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SERIAL_QUERY_ITEM_S             *pstSerialList
);

/**
* ��ѯ������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [OUT]  pstSerialInfo           ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SERIAL_PORT_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySerialInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT SERIAL_INFO_S                   *pstSerialInfo
);

/**
* ��ҳ��ѯĳ��EC���ڱ������Դռ�õ���Ϣ�б�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         ����ͨ��������Ϣ
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ(��֧��ƫ��)
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSerialRelationList   ���ڱ������Դռ�õ���Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1��֧�ֲ�ѯ��EC����ģʽ��:��̨����(#IMOS_SE_PTZ)��͸��ͨ��(#IMOS_SE_TRANS)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECSerialUsedRelation
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SERIAL_RELATION_ITEM_S          *pstSerialRelationList
);
/** @} */ /* end of SERIAL MANAGEMENT MODULE */

#if 0
#endif
/************************************************************************************************************
 �豸-EC,DC OSDͼƬ
************************************************************************************************************/


#if 0
#endif
/**************************************************************************************************************
�豸-EC,DCͨ�����ƻ��Լ�SNMPģ�����
***************************************************************************************************************/
/** @defgroup groupTempletMg �豸(EC,DC)ģ���Լ��ƻ�ģ�����
*   �豸(EC,DC)ģ���Լ��ƻ�ģ�����
*   @{
*/
/**
* @name ECģ��
* @{    ECģ��
*/
/**
* ����ECͨ������ģ�� \n
* @param [IN]   pstUserLogIDInfo                �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSDKECChannelTemplet          ECͨ������ģ����Ϣ
* @param [OUT]  szECChannelTempletCode          ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  EC_CHANNEL_TEMPLET_S            *pstSDKECChannelTemplet,
    OUT CHAR                            szECChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* ɾ��EC����ģ�� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szECChannelTempletCode      ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szECChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* �޸�EC����ģ�� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szECChannelTempletCode      ģ�����
* @param [IN]   pstSDKECChannelTemplet      ECͨ������ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szECChannelTempletCode[IMOS_RES_CODE_LEN],
    IN  EC_CHANNEL_TEMPLET_S            *pstSDKECChannelTemplet
);


/**
* ��ҳ��ѯECͨ������ģ���б� \n
* ����Ϊ��ʱ,��ѯ����ģ��,��Ĭ�ϰ���"ͨ������������"����
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition          ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#DEV_SUB_TYPE[�豸������]; #NAME_TYPE[ģ������]; #TIME_TYPE[ģ�����ʱ��])
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKECChannelTempList    ECͨ������ģ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECChannelTempletList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S       *pstSDKECChannelTempList
);


/**
* ��ѯEC����ģ�������Ϣ \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szECChannelTempletCode      ģ�����
* @param [OUT]  pstSDKECChannelTemplet      ECͨ������ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECChannelTemplet
(
    IN     USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN     CHAR                         szECChannelTempletCode[IMOS_RES_CODE_LEN],
    OUT    EC_CHANNEL_TEMPLET_S         *pstSDKECChannelTemplet
);
/** @} */  /* ECģ����� */
/**
* @name DCģ��
* @{    DCģ��
*/
/**
* ����DCͨ������ģ�� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSDKDCChannelTemplet      DCͨ������ģ����Ϣ
* @param [OUT]  szDCChannelTempletCode      ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DC_CHANNEL_TEMPLET_S        *pstSDKDCChannelTemplet,
    OUT CHAR                        szDCChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* ɾ��DC����ģ�� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szDCChannelTempletCode      ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szDCChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* �޸�DC����ģ�� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szDCChannelTempletCode      ģ�����
* @param [IN]   pstSDKDCChannelTemplet      DC����ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szDCChannelTempletCode[IMOS_RES_CODE_LEN],
    IN  DC_CHANNEL_TEMPLET_S            *pstSDKDCChannelTemplet
);


/**
* ��ҳ��ѯDCͨ������ģ���б� \n
* ����Ϊ��ʱ,��ѯ����ģ��,��Ĭ�ϰ���"ͨ��������"����.
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition          ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#DEV_SUB_TYPE[�豸������]; #NAME_TYPE[ģ������]; #TIME_TYPE[ģ�����ʱ��])
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKDCChannelTempList    DCͨ������ģ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDCChannelTempletList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S       *pstSDKDCChannelTempList
);


/**
* ��ѯDC����ģ�������Ϣ \n
* @param [IN]   pstUserLogIDInfo                �û���¼ID��Ϣ��ʶ
* @param [IN]   szDCChannelTempletCode          ģ�����
* @param [OUT]  pstSDKDCChannelTemplet          DC����ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDCChannelTempletCode[IMOS_RES_CODE_LEN],
    OUT DC_CHANNEL_TEMPLET_S        *pstSDKDCChannelTemplet
);
/** @} */  /* DCģ����� */


/**
* @name �ƻ�ģ��
* @{    �ƻ�ģ��
*/
/**
* ���Ӽƻ�ģ�� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSDKPlanTemplet       �ƻ�ģ����Ϣ
* @param [OUT]  szPlanTempletCode       ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  PLAN_TEMPLET_S              *pstSDKPlanTemplet,
    OUT CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN]
);


/**
* ɾ���ƻ�ģ�� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szPlanTempletCode       ģ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeletePlanTemplet
(
    IN USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN CHAR                             szPlanTempletCode[IMOS_RES_CODE_LEN]
);


/**
* �޸ļƻ�ģ�� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szPlanTempletCode       ģ�����
* @param [IN]   pstSDKPlanTemplet       �ƻ�ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN],
    IN  PLAN_TEMPLET_S              *pstSDKPlanTemplet
);


/**
* ��ҳ��ѯ�ƻ�����ģ���б� \n
* ����Ϊ��ʱ,��ѯ����ģ��,��Ĭ�ϰ���"ģ����������"����
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[ģ������]; #TIME_TYPE[ģ�����ʱ��])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstPlanTempList        �ƻ�ģ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlanTempletList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S   *pstPlanTempList
);


/**
* ��ѯ�ƻ�����ģ�������Ϣ \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szPlanTempletCode       ģ������
* @param [OUT]  pstSDKPlanTemplet       �ƻ�ģ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN],
    OUT PLAN_TEMPLET_S              *pstSDKPlanTemplet
);
/** @} */  /* �ƻ�ģ����� */
/** @} */ /* end of TEMPLET MANAGEMENT MODULE */
#if 0
#endif
/** @defgroup groupCruise Ѳ��ҵ��
*   Ѳ��ҵ��
*   @{
*/
/**
* @name Ѳ��·�߹���
* @{    Ѳ��·�߹���
*/
/**
* ����Ѳ��·��\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstCruisePathInfo   Ѳ��·����Ϣ
* @param [OUT]  szCruisePathCode    Ѳ��·�߱���
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CAMERA_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCruisePath
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CRUISE_PATH_INFO_S      *pstCruisePathInfo,
    OUT CHAR                    szCruisePathCode[IMOS_RES_CODE_LEN]
);


/**
* ɾ��Ѳ��·�� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePathCode    Ѳ��·�߱���
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);


/**
* �޸�Ѳ��·�� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePathCode    Ѳ��·�߱���
* @param [IN]   pstCruisePathInfo   Ѳ��·����Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCruisePath
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCruisePathCode[IMOS_RES_CODE_LEN],
    IN  CRUISE_PATH_INFO_S      *pstCruisePathInfo
);


/**
* ��ҳ��ѯĳ���������Ѳ��·���б� \n
* ����Ϊ��ʱ,��������������Ѳ��·��,��Ĭ��"Ѳ��·��������������"
* @param [IN]   pstUserLogIDInfo       �û���Ϣ
* @param [IN]   szCamCode              ���������
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;��������:#NAME_TYPE[·������])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstCruisePathList      Ѳ��·���б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePathList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCamCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CRUISE_PATH_QUERY_ITEM_S    *pstCruisePathList
);


/**
* ��ѯĳ���������Ѳ��·�ߵ���ϸ��Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePathCode    Ѳ��·�߱���
* @param [OUT]  pstCruisePathInfo   Ѳ��·����Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_PRESET_VALUE_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePathInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN],
    OUT CRUISE_PATH_INFO_S          *pstCruisePathInfo
);

/**
* �ֶ�����Ѳ��·��
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePathCode    Ѳ��·�߱���
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_DAO_CRUISE_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - �������ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);

/**
* �ֶ�ֹͣѲ��·��
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePathCode    Ѳ��·�߱���
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_DAO_CRUISE_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - �������ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);

/**
* ֹͣ������µ������ֶ�Ѳ��·��
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCameraCode        ���������
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - �������ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopAllCruisePathByCam
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCameraCode[IMOS_DEVICE_CODE_LEN]
);
/** @} */ /* Ѳ��·�߽��� */

#if 0
#endif
/***************************************************************************
Ѳ���ƻ�
****************************************************************************/
/**
* @name Ѳ���ƻ�����
* @{    Ѳ���ƻ�����
*/
/**
* ����Ѳ���ƻ�\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstCruisePlanInfo   Ѳ���ƻ���Ϣ
* @param [OUT]  szCruisePlanCode    Ѳ���ƻ�����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CRUISE_PLAN_INFO_S          *pstCruisePlanInfo,
    OUT CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* �޸�Ѳ���ƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePlanCode    Ѳ���ƻ�����
* @param [IN]   pstCruisePlanInfo   Ѳ���ƻ���Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN],
    IN  CRUISE_PLAN_INFO_S          *pstCruisePlanInfo
);


/**
* ɾ��Ѳ���ƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePlanCode    Ѳ���ƻ�����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* ��ѯѲ���ƻ�\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePlanCode    Ѳ���ƻ�����
* @param [OUT]  pstCruisePlanInfo   Ѳ���ƻ���Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISEID_NOT_EXIST
* - #ERR_DAO_CAMERA_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN],
    OUT CRUISE_PLAN_INFO_S          *pstCruisePlanInfo

);

/**
* ��ҳ��ѯѲ���ƻ��б� \n
* ����Ϊ��ʱ,������Ѳ���ƻ�,��Ĭ��"Ѳ���ƻ���������"����
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode              ��֯����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[�ƻ�����]��
* -                                    #TIME_TYPE[����ʱ��]��#CODE_TYPE[���������])
* @param [IN    pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstCruisePlanList      Ѳ���ƻ��б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note Ŀǰ��֧�ְ���֯��ѯѲ���ƻ��б�
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePlanList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CRUISE_PLAN_QUERY_ITEM      *pstCruisePlanList
);


/**
* ʹ��Ѳ���ƻ� \n
* @param [IN    pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePlanCode    Ѳ���ƻ�����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* ȥʹ��Ѳ���ƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCruisePlanCode    Ѳ���ƻ�����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);
/** @} */  /* Ѳ���ƻ����� */
/** @} */ // end of groupCruise
#if 0
#endif
/***************************************************************************
�澯
****************************************************************************/
/** @defgroup groupAlarm �澯����
*   �澯��������澯Դ��ɾ�Ĳ�(�б�)���澯����豸��ɾ�Ĳ�(�б�)���澯������ɾ�Ĳ��
*   @{
*/
/**
* ������չ�澯Դ(������������,������,�������)
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstAlarmSrc                 �澯Դ��Ϣ
* @param [OUT]  szAlarmSrcCode              �澯Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* - #ERR_DAO_DEVICE_CODE_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.�澯Դ���벻����Ϊ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ALARM_SRC_S                         *pstAlarmSrc,
    OUT CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN]
);


/**
* ɾ����չ�澯Դ(������������,������,�������)
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode              �澯Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN]
);


/**
* �޸���չ�澯Դ��Ϣ(������������,������,�������)
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode              �澯Դ����
* @param [IN]   pstAlarmSrc                 �澯Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �澯Դ�ı��벻���޸�, �����޸ĸ澯Դ��Ϣʱ��Ҫ��д
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ALARM_SRC_S                         *pstAlarmSrc
);


/**
* ��ѯ��չ�澯Դ��Ϣ(������������,������,�������)
* @param [IN]   pstUserLoginIDInfo          �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode              �澯Դ����
* @param [OUT]  pstAlarmSrc                 �澯Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN],
    OUT ALARM_SRC_S                         *pstAlarmSrc

);


/**
* ��ҳ��ѯ��֯�µĸ澯Դ(����������,������,��������Լ���չ�澯Դ)�б�  \n
* ����Ϊ��ʱ,��ѯ���и澯Դ,��Ĭ�ϰ���"�澯Դ��������"����.
* @param [IN]   pstUserLogIDInfo       �û���Ϣ��ʶ
* @param [IN]   szOrgCode              ��֯����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[�澯Դ����]��#DOMAIN_TYPE[����������:0 ����,1 ����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmSrcList        �澯Դ��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_SRC_QUERY_ITEM_S      *pstAlarmSrcList

);


/**
* �����澯����豸
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstAlarmExporter            �澯����豸��Ϣ
* @param [OUT]  szAlarmExporterCode         �澯����豸����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.�澯����豸���벻����Ϊ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ALARM_EXPORTER_S                    *pstAlarmExporter,
    OUT CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN]
);


/**
* ɾ���澯����豸
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmExporterCode         �澯����豸����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN]
);


/**
* �޸ĸ澯����豸��Ϣ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmExporterCode         �澯����豸����
* @param [IN]   pstAlarmExporter            �澯����豸��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN],
    IN  ALARM_EXPORTER_S                    *pstAlarmExporter
);


/**
* ��ѯ�澯����豸��Ϣ
* @param [IN]   tUserLoginIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmExporterCode         �澯����豸����
* @param [OUT]  pstAlarmExporter            �澯����豸��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN],
    OUT ALARM_EXPORTER_S                    *pstAlarmExporter

);


/**
* ��ҳ��ѯ��֯�µĸ澯����豸�б�  \n
* ����Ϊ��ʱ,�����и澯����豸,��Ĭ��"�澯����豸������������"
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode              ��֯����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[�豸����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmExporterList   �澯����豸��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmExporterList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_EXPORTER_QUERY_ITEM_S *pstAlarmExporterList

);

/**
* ��ѯ�澯��Ϣ
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventRecordCode   �澯����
* @param [OUT]  pstEventRecord      �澯��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_NOT_EXIST
 * -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarm
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szEventRecordCode[IMOS_RES_CODE_LEN],
    OUT EVENT_RECORD_S          *pstEventRecord
);


/**
* ��ҳ��ѯ�澯��Ϣ�б�
* ����Ϊ��ʱ,��ѯ���н�ɫ,��Ĭ�ϰ���"�澯ʱ�併��"����.
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#USER_NAME_TYPE[�û�����];
* -                                 #TIME_TYPE[ʱ��]; #EVENT_TYPE[�澯�¼�]; #EVENT_SECURITY[�澯�¼�����];
* -                                 #EVENT_COMFIRMED[�澯�Ƿ���ȷ��]; #EVENT_TIME[�澯ʱ��])
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmList        �澯��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.�澯���ؼ��������Σ������ʾΪ����
* -     2.�澯���ؼ����������ʾ��Σ��Ϊ����
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_RECORD_S                  *pstAlarmList
);


/**
* ȷ�ϸ澯
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventRecordCode       �澯����
* @param [IN]   szAckSuggestion         �澯ȷ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_NOT_EXIST
* - #ERR_AS_ALARM_CONFIRMED
* -     ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfirmAlarm
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szEventRecordCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szAckSuggestion[IMOS_DESC_LEN]
);


/**
* ���Ӹ澯����
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode          �澯Դ����
* @param [IN]   ulAlarmType             �澯����, ȡֵΪ#AlARM_TYPE_E
* @param [IN]   pstAlarmAction          �澯����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_LINKAGE_EXIST
* -     ���������룺��������ļ�sdk_err.h
* @note ��������ȫ�ظ��Ķ�������ע�������е�������������ö��"ͬһ���������ͬһ��������", �������óɹ���������ֻ��
*       �������õĻ�ִ�гɹ�.
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_S                  *pstAlarmAction
);


/**
* ɾ���澯����
* @param [IN]  pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]  szAlarmSrcCode               �澯Դ����
* @param [IN]  ulAlarmType                  �澯����, ȡֵΪ#AlARM_TYPE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_LINKAGE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType
);


/**
* �޸ĸ澯������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode          �澯Դ����
* @param [IN]   ulAlarmType             �澯����, ȡֵΪ#AlARM_TYPE_E
* @param [IN]   pstAlarmAction          �澯����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_LINKAGE_NOT_EXIST
* -     ���������룺��������ļ�sdk_err.h
* @note ��������ȫ�ظ��Ķ�������ע�������е�������������ö��"ͬһ���������ͬһ��������", �������óɹ���������ֻ��
*       ���ȵ����û�ִ�гɹ�.
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_S                  *pstAlarmAction
);


/**
* ��ѯ�澯������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode          �澯Դ����
* @param [IN]   ulAlarmSrcType          �澯����, ȡֵΪ#AlARM_TYPE_E
* @param [OUT]  pstAlarmAction          �澯����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_LINKAGE_NOT_EXIST
* - #ERR_AS_LINKAGE_NUM
* -     ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    OUT ALARM_ACTION_S                  *pstAlarmAction
);

/**
* ��ѯ�澯�¼���Ϣ�б� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition       ��ѯ������֧�ְ�����ֹʱ���Լ��澯���������ѯ¼��
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmEventInfo       �澯������Ϣ���ݽṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��ѯʱ���Ȳ��ܴ���24Сʱ������¼�����������ʱ���ʽΪ��"%Y-%m-%d %H:%M:%S"��ʽ ����Ϣ�����޶�Ϊ24�ַ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmEventList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ALARM_EVENT_QUERY_CONDITION_S *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_EVENT_INFO_S          *pstAlarmEventInfo
);

/** @} */ /* end of ALARM MODULE */

#if 0
#endif
/**************************************************************************************************************
�����Լ������ƻ� GuardPlan Edited By sunyifan(06966)  Complete
**************************************************************************************************************/
/** @defgroup groupGPMg �����Լ������ƻ�����
*   �����Լ������ƻ�����
*   @{
*/
/**
* ���Ӳ����ƻ� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode          �澯Դ����
* @param [IN]   ulAlarmSrcType          �澯����, ȡֵΪ#AlARM_TYPE_E
* @param [IN]   pstGuardPlanInfo        �����ƻ���Ϣ
* @param [OUT]  szGuardPlanCode         �����ƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_CAM_NOT_LOCAL_DOMAIN
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  GUARD_PLAN_INFO_S               *pstGuardPlanInfo,
    OUT CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN]
);

/**
* �޸Ĳ����ƻ� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGuardPlanCode         �����ƻ�����
* @param [IN]   pstGuardPlanInfo        �����ƻ���Ϣ�ṹ��
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    IN  GUARD_PLAN_INFO_S               *pstGuardPlanInfo
);

/**
* ɾ�������ƻ� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGuardPlanCode         �����ƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ�����ƻ� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGuardPlanCode         �����ƻ�����
* @param [OUT]  pstGuardPlanInfo        �����ƻ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    OUT GUARD_PLAN_INFO_S               *pstGuardPlanInfo
);


/**
* ��ҳ��ѯ�����ƻ��б� \n
* ����Ϊ��ʱ,��ѯ���в����ƻ�,��Ĭ�ϰ���"�ƻ���������"����.
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode              ��֯����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;��������:#NAME_TYPE[�ƻ�����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstGuardPlanList       �����ƻ��б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlanList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GUARD_PLAN_QUERY_ITEM_S     *pstGuardPlanList
);

/**
* �����ƻ� - ��ѯ�б�(���澯Դ����) \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode      �澯Դ����
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstGuardPlanList    �����ƻ���Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlanListBySrc
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szAlarmSrcCode[IMOS_DEVICE_CODE_LEN],
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GUARD_PLAN_QUERY_ITEM_S     *pstGuardPlanList
);


/**
* ���Ʋ����ƻ�, ���������ƻ���������ֹͣ \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGuardPlanCode         �����ƻ�����
* @param [IN]   ulCtrlMode              ���Ʋ���������, 1Ϊ����; 0Ϊֹͣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CtrlGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulCtrlMode
);

/**
* �ֶ���������, �����ֶ��������ֶ����� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szAlarmSrcCode          �澯Դ����
* @param [IN]   ulAlarmSrcType          �澯����
* @param [IN]   ulCtrlMode              1Ϊ�ֶ�����, 0Ϊ�ֶ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ���������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CtrlManualGuard
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ULONG                           ulCtrlMode
);

/** @} */ /* end of GUARD PLAN MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
GIS
****************************************************************************/
/** @defgroup groupGIS  GIS��ͼ����
*   GIS��ͼ��������ϴ�,����,ɾ��,����GIS��ͼ�Ƚӿ�,�ȵ�,����,�ı���,������Դͼ�����ɾ�Ĳ�(�б�)��
*   @{
*/
/*--------------------------------------------GIS��ͼ����--------------------------------------*/

/**
* �ϴ�GIS��ͼʱ, ����ͼ�����Լ��ļ����ϴ�������������У��, �ɹ���, ����FTP�������û����������Լ��ļ����·��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstGISInfo              GIS��Ϣ("��ͼ�ļ���"�������ظ�,���������ظ��ϴ�"ͬ�ļ���"��ͼ)
* @param [OUT]  pstFtpInfo              FTP������Ϣ
* @param [OUT]  pulSequenceNumber       ����FTP�����������к�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_MAP_NUM_OVERFLOW
* - #ERR_AS_FIND_SAME_MAP_NAME
* - #ERR_AS_FIND_SAME_MAP_FILE_NAME
* -     ����,�μ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisMapStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  FILE_GIS_INFO_S             *pstGISInfo,
    OUT FTP_INFO_S                  *pstFtpInfo,
    OUT ULONG                       *pulSequenceNumber
);

/**
* ���ϴ�GIS��ͼ�Ľ��, ֪ͨ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess              �ϴ��ļ��Ľ��, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   ulSequenceNumber        ����FTP�����������к�
* @param [OUT]  szGISMapCode            GIS��ͼ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_UPLOAD_MAP_FAIL
* - #ERR_DAO_GIS_MAP_CODE_EXIST
* - #ERR_DAO_GIS_MAP_NAME_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisMapEnd
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  BOOL_T                        bIsSuccess,
    IN  ULONG                         ulSequenceNumber,
    OUT CHAR                          szGISMapCode[IMOS_RES_CODE_LEN]
);

/**
* ��ȡGIS��ͼ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [OUT]  pstGISInfo              GIS��ͼ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetGisMapInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    OUT FILE_GIS_INFO_S             *pstGISInfo
);



/**
* ����GIS��ͼ��ʱ������FTP�������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [OUT]  pstFtpInfo              FTP������Ϣ
* @param [OUT]  ulSequenceNumber        ����FTP�����������к�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_FIND_SAME_MAP_FILE_NAME
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisMapStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    OUT FTP_INFO_S                  *pstFtpInfo,
    OUT ULONG                       *ulSequenceNumber
);

/**
* ������GIS��ͼ�Ľ��֪ͨ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess              �ϴ��ļ��Ľ����BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   ulSequenceNumber        ����FTP�����������к�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisMapEnd
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  BOOL_T                        bIsSuccess,
    IN  ULONG                         ulSequenceNumber
);


/**
* ɾ��GIS��ͼ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MAP_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ע�⣬���ɾ�����ļ���Ҳ���سɹ���
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGisMap
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN]
);

/**
* �޸�GIS��ͼ����
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szGisMapName            GIS��ͼ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MAP_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGisMapName
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szGisMapName[IMOS_NAME_LEN]
);


/**
* ָ����֯��Ĭ�ϵ�ͼ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szDomainCode            ��֯����
* @param [IN]   szGISMapCode            GIS��ͼ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetOrgDefGISMap
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szGISMapCode[IMOS_RES_CODE_LEN]
);


/**
* ��ѯ��֯��Ĭ�ϵ�ͼ��Ϣ
* @param [IN]  pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN]  szDomainCode             ��֯����
* @param [OUT] pstGisMapItem            Ĭ�ϵ�ͼ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetOrgDefGISMap
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    OUT GIS_QUERY_ITEM_S            *pstGisMapItem
);

/*--------------------------------------------�ȵ�--------------------------------------*/


/**
* ��GIS��ͼ�ϴ�������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            ��������GIS��ͼ�ı���
* @param [IN]   pstHotZone              ������Ϣ
* @param [OUT]  szHotZoneCode           ��������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTZONE_CODE_EXIST
* - #ERR_DAO_GIS_HOTZONE_NAME_EXIST
* - #ERR_DAO_GIS_HOTZONE_LINK_MAP_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateHotZone
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTZONE_S                   *pstHotZone,
    OUT CHAR                            szHotZoneCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            ��������GIS��ͼ����
* @param [IN]   szHotZoneCode           ��������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelHotZone
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szHotZoneCode[IMOS_RES_CODE_LEN]
);


/**
* ��GIS��ͼ���޸�����
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            ��������GIS��ͼ����
* @param [IN]   szHotZoneCode           ��������
* @param [IN]   pstHotZone              ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* - #ERR_DAO_GIS_HOTZONE_NAME_EXIST
* - #ERR_DAO_GIS_HOTZONE_LINK_MAP_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyHotZone
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  CHAR                        szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szHotZoneCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTZONE_S              *pstHotZone
);


/**
* ��ѯ������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            ��������GIS��ͼ����
* @param [IN]   szHotZoneCode           ��������
* @param [OUT]  pstHotZone              ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotZone
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szHotZoneCode[IMOS_RES_CODE_LEN],
    OUT GIS_HOTZONE_WITH_CODE_S     *pstHotZone
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵ������б� \n
* ����Ϊ��ʱ,����������,��Ĭ��"����������������"
* @param [IN    pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode           ��������GIS��ͼ����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[��������])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstHotZoneList         �����б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotZoneList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_HOTZONE_WITH_CODE_S     *pstHotZoneList
);

/*--------------------------------------------�ȵ�--------------------------------------*/

/**
* ��GIS��ͼ�ϴ����ȵ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ȵ�����GIS��ͼ����
* @param [IN]   pstHotSpot              �ȵ���Ϣ
* @param [OUT]  szHotSpotCode           �ȵ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTSPOT_CODE_EXIST
* - #ERR_DAO_GIS_HOTSPOT_LINK_DOMAIN_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTSPOT_S                   *pstHotSpot,
    OUT CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ���ȵ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ȵ�����GIS��ͼ����
* @param [IN]   szHotSpotCode           �ȵ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN]
);



/**
* ��GIS��ͼ���޸��ȵ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ȵ�����GIS��ͼ����
* @param [IN]   szHotSpotCode           �ȵ����
* @param [IN]   pstHotSpot              �ȵ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* - #ERR_DAO_GIS_HOTSPOT_LINK_DOMAIN_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTSPOT_S                   *pstHotSpot
);


/**
* ��ѯ�ȵ���Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ȵ�����GIS��ͼ����
* @param [IN]   szHotSpotCode           �ȵ����
* @param [OUT]  pstHotSpot              �ȵ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN],
    OUT GIS_HOTSPOT_WITH_CODE_S         *pstHotSpot
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵ��ȵ��б�
* ����Ϊ��ʱ,�������ȵ�,��Ĭ��"�ȵ������������"
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode           ��������GIS��ͼ����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#CODE_TYPE[�ȵ����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstHotSpotList         �ȵ���Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotSpotList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_HOTSPOT_WITH_CODE_S     *pstHotSpotList
);

/*--------------------------------------------�ı���--------------------------------------*/

/**
* ��GIS��ͼ�ϴ����ı���
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ı�������GIS��ͼ����
* @param [IN]   pstTextBox              �ı�����Ϣ
* @param [OUT]  szTextBoxCode           �ı������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MSGBOX_CODE_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateTextBox
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_TEXTBOX_S                   *pstTextBox,
    OUT CHAR                             szTextBoxCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ���ı���
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ı�������GIS��ͼ����
* @param [IN]   szTextBoxCode           �ı������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTextBox
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szTextBoxCode[IMOS_RES_CODE_LEN]
);


/**
* ��GIS��ͼ���޸��ı���
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ı�������GIS��ͼ����
* @param [IN]   szTextBoxCode           �ı������
* @param [IN]   pstTextBox              �ı�����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTextBox
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szTextBoxCode[IMOS_RES_CODE_LEN],
    IN  GIS_TEXTBOX_S               *pstTextBox
);


/**
* ��ѯĳ���ı�����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �ı�������GIS��ͼ����
* @param [IN]   szTextBoxCode           �ı������
* @param [OUT]  pstTextBox              �ı�����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTextBox
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szTextBoxCode[IMOS_RES_CODE_LEN],
    OUT GIS_TEXTBOX_WITH_CODE_S     *pstTextBox
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵ��ı����б� \n
* ����Ϊ��ʱ,�������ı���,��Ĭ��"�ı����������"����
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode           �ı�������GIS��ͼ����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#CODE_TYPE[�ı������])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstTextBoxList         �ı�����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTextBoxList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_TEXTBOX_WITH_CODE_S     *pstTextBoxList
);


/*-------------------------------��Դͼ��(��������澯Դ���澯���)--------------------------------------*/

/**
* ��GIS��ͼ�ϴ��������ͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �����ͼ������GIS��ͼ����
* @param [IN]   pstCameraIcon           �����ͼ����Ϣ
* @param [OUT]  szCameraIconCode        �����ͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_CAM_ICON_S              *pstCamIcon,
    OUT CHAR                         szCamIconCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ�������ͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szCamIconCode           �����ͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN]
);


/**
* ��GIS��ͼ���޸������ͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szCamIconCode           �����ͼ�����
* @param [IN]   pstCamIcon              �����ͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_CAM_ICON_S              *pstCamIcon
);


/**
* ��ѯ�����ͼ����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szCamIconCode           �����ͼ�����
* @param [OUT]  pstCamIcon              �����ͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_CAM_ICON_WITH_CODE_S    *pstCamIcon
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵ������ͼ���б�
* ����Ϊ��ʱ,�����������ͼ�� ,��Ĭ��"�����ͼ�������������"
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode           �����ͼ������GIS��ͼ����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#CODE_TYPE[ͼ�����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstCameraIconList      �����ͼ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamIconList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_CAM_ICON_WITH_CODE_S    *pstCamIconList
);


/**
* ��GIS��ͼ�����Ӹ澯Դͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �澯Դͼ������GIS��ͼ����
* @param [IN]   pstAlarmSrcIcon         �澯Դͼ����Ϣ
* @param [OUT]  szAlarmSrcIconCode      �澯Դͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_SRC_ICON_S            *pstAlarmSrcIcon,
    OUT CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ���澯Դͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            �澯Դͼ������GIS��ͼ����
* @param [IN]   szAlarmSrcIconCode      �澯Դͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN]
);


/**
* ��GIS��ͼ���޸ĸ澯Դͼ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szAlarmSrcIconCode      �澯Դͼ�����
* @param [IN]   pstAlarmSrcIcon         �澯Դͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_SRC_ICON_S            *pstAlarmSrcIcon
);


/**
* ��ѯĳ���澯Դͼ����Ϣ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode                GIS��ͼ����
* @param [IN]   szAlarmSrcIconCode          �澯Դͼ�����
* @param [OUT]  pstAlarmSrcIcon             �澯Դͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_ALARM_SRC_ICON_WITH_CODE_S  *pstAlarmSrcIcon
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵĸ澯Դͼ���б�
* ����Ϊ��ʱ,�����и澯Դͼ��,��Ĭ��"�澯Դͼ�������������"
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode           �澯Դͼ������GIS��ͼ����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#CODE_TYPE[ͼ�����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmSrcIconList    �澯Դͼ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGISAlarmSrcIconList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GIS_ALARM_SRC_ICON_WITH_CODE_S  *pstAlarmSrcIconList
);


/**
* ��GIS��ͼ�ϴ����澯���ͼ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode                �澯���ͼ������GIS��ͼ����
* @param [IN]   pstAlarmDstIcon             �澯���ͼ����Ϣ
* @param [OUT]  szAlarmDstIconCode          �澯���ͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_DST_ICON_S            *pstAlarmDstIcon,
    OUT CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN]
);

/**
* ��GIS��ͼ��ɾ���澯���ͼ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode                GIS��ͼ����
* @param [IN]   szAlarmDstIconCode          �澯���ͼ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN]
);


/**
* ��GIS��ͼ���޸ĸ澯���ͼ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode                GIS��ͼ����
* @param [IN]   szAlarmDstIconCode          �澯���ͼ�����
* @param [IN]   pstAlarmDstIcon             �澯���ͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_DST_ICON_S            *pstAlarmDstIcon
);


/**
* ��ѯĳ���澯���ͼ����Ϣ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode                GIS��ͼ����
* @param [IN]   szAlarmDstIconCode          �澯���ͼ�����
* @param [OUT]  pstAlarmDstIcon             �澯���ͼ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_ALARM_DST_ICON_WITH_CODE_S  *pstAlarmDstIcon
);


/**
* ��ҳ��ѯĳ��GIS��ͼ�ϵĸ澯���ͼ���б� \n
* ����Ϊ��ʱ,�����и澯���ͼ��,��Ĭ��"�澯���ͼ�������������"
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode        �澯���ͼ������GIS��ͼ����
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����(����NULL;֧�ֵĲ�ѯ��������:#CODE_TYPE[ͼ�����])
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAlarmDstIconList �澯���ͼ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGISAlarmDstIconList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GIS_ALARM_DST_ICON_WITH_CODE_S  *pstAlarmDstIconList
);
/** @} */ /* end of GIS MODULE */
#if 0
#endif
/*******************************************************************************************************
͸��ͨ�� Transparent
********************************************************************************************************/
/** @defgroup groupTP ͸��ͨ������
*   ͸��ͨ������
*   @{
*/
/**
* ����͸��ͨ��,�������ڵ�ָ����ַ�ʹ��ڵ�����
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstTransChannelInfo     ͸��ͨ������Ϣ
* @param [OUT]  szTransChannelCode      ͸��ͨ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SERIAL_PORT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.��ֻ֧�ִ��ڵ�1��IP�ĵ�ַ��͸��ͨ������,
* -     2.����͸��ͨ�����֧��2������.
* -     3.ֻ֧��Э������ΪUDP��ʽ��͸��ͨ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  TRANSPARENT_INFO_S          *pstTransChannelInfo,
    OUT CHAR                        szTransChannelCode[IMOS_RES_CODE_LEN]
);

/**
* ɾ��͸��ͨ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode      ͸��ͨ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szTransChannelCode[IMOS_RES_CODE_LEN]
);


/**
* �޸�͸��ͨ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode          ͸��ͨ������
* @param [IN]   pstSDKTransparentChannel    ͸��ͨ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_TL_OPENED
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* - #ERR_DAO_SERIAL_PORT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.ֻ���޸�͸��ͨ��������ͨ���������ֶ�,
* -     2.ֻ֧��Э������ΪUDP��ʽ��͸��ͨ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN],
    IN  TRANSPARENT_INFO_S              *pstSDKTransparentChannel
);



/**
* ��ҳ��ѯĳ����֯�µ�͸��ͨ���б�  \n
* ����Ϊ��ʱ,��ѯ����͸��ͨ��,��Ĭ�ϰ���"͸��ͨ����������"����.
* @param [IN]   pstUserLogIDInfo         �û���Ϣ��ʶ
* @param [IN]   szDomainCode             ��֯����
* @param [IN]   pstQueryCondition        ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[͸��ͨ������]; CODE_TYPE[͸��ͨ������])
* @param [IN]   pstQueryPageInfo         �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo           ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKTransparentChannel ͸��ͨ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTransparentChannelList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRANSPARENT_QUERY_ITEM_S    *pstSDKTransparentChannel
);

/**
* ��ѯ͸��ͨ����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode      ͸��ͨ������
* @param [OUT]  pstSDKUpgTaskInfo       ͸��ͨ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTransparentChannelInfo
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  CHAR                             szTransChannelCode[IMOS_RES_CODE_LEN],
    OUT TRANSPARENT_INFO_S               *pstSDKTransparentInfo
);

/**
* ����͸��ͨ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode      ͸��ͨ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* - #ERR_AS_TL_DEVICE_OFFLINE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN]
);
/**
* ͣ��͸��ͨ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode      ͸��ͨ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN]
);

/**
* ͨ��͸��ͨ����������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szTransChannelCode      ͸��ͨ������
* @param [IN]   pData                   �����͵�����
* @param [IN]   ulDataLen               �����͵����ݳ���, ��󳤶�Ϊ#IMOS_STRING_LEN_1024
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_CHANNEL_NOT_ENABLE
* - #ERR_AS_TL_CLOSED
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendDataByChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN],
    IN  VOID                            *pData,
    IN  ULONG                           ulDataLen
);
/** @} */ /* end of TRANSPARENT MANAGEMENT MODULE */
#if 0
#endif
/*******************************************************************************
 ϵͳ����
*******************************************************************************/
/** @defgroup groupLogMg ϵͳ����
*   ϵͳ����ģ�������������,��־����,License����,��Ϣͳ�ƹ������ģ��
*   @{
*/
/**
* @name ��־����
* @{    ��־����
*/
/**
* ��ҳ��ѯ�û�������־
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstOperateLogInfo      ��־��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. �����ͨ�ò�ѯ������pstQueryCondition����ΪNULL
* - 2. ֧�ְ�����Ա���ơ���ѯ��/ֹʱ�䡢IP��ַ���������͡����������ҵ�����͡���������������������ݲ�ѯ,
* -    ��Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:
* -    #USER_NAME_TYPE��#OPER_TIME��#OPER_IP��#OPER_TYPE��#OPER_RESULT��#OPER_SERVICE_TYPE��#OPER_OBJ;
* - 3. ���������������Ĭ�ϰ��ղ���ʱ�併������;
* - 4. ����ʱ��Ĳ�ѯ��#OPER_TIME�����Բ�ѯĳ��ʱ�䣬#QUERY_CONDITION_ITEM_S��#ulLogicFlag����д�����#LOGIC_FLAG_E, ��:
* -    ��ѯ����ʱ��ΪA��B�Ĳ�����־����ô��ѯ��Ϊ2, ��ѯ���б��е�һ(���)����Ԫ���в�ѯ������Ϊ����ʱ�䣬
* -    ��ѯ�����߼���ϵ����Ϊ���ڵ���(��С�ڵ���)����ѯ����ΪA(��B);
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOperLog
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT OPERATE_LOG_S               *pstOperateLogInfo
);

/** @} */ /* end of ��־���� */
#if 0
#endif
/***************************************************************************
����
****************************************************************************/
/**
* @name ����
* @{    ����
*/
/**
* ������Ϣ������������Ϣ����־��Ϣ�����ݿ���Ϣ��, �����ر��ݵ�ִ�н���� \n
* @param [IN]    pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]    ulFlag                  �������ͱ��, ȡֵΪ#BP_BACKUPTYPE_E
* @param [OUT]   pulStatus               ��ǰ����״̬, ȡֵΪ#BACKUP_STATUS_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ÿ�α���ʱ����ɾ��֮ǰ�ı����ļ���
*/
IMOS_EXPORT ULONG STDCALL IMOS_BackupData
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulFlag,
    OUT ULONG                           *pulStatus
);

/**
* ��ѯ������Ϣ, ��ϵͳ���ڷǱ���״̬�����������FTP����Ϣ�� \n
* @param [IN]    pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]    ulFlag                  �������ͱ��, ȡֵΪ#BP_BACKUPTYPE_E
* @param [OUT]   pulStatus               ��ǰ����״̬, ȡֵΪ#BACKUP_STATUS_E
* @param [OUT]   pstFtpInfo              �����ļ���ص�FTP��Ϣ, ��Ϣ�е��ļ�·���������ļ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBackupInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulFlag,
    OUT ULONG                           *pulStatus,
    OUT FTP_INFO_S                      *pstFtpInfo
);

/**
* ִ��Ԥ���������ͨ�ýӿ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstCmdInfo              ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* - #ERR_AS_INVALID_NTP_IP_ADDRESS
 * -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ExecutePredefinedCmd
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SDK_CMD_INFO_S                   *pstCmdInfo
);


/**
* ����/ֹͣ������NTPʱ��ͬ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulSyncFlag              ͬ����ǣ�1��ʼ��2ֹͣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* - #ERR_COMMON_FAIL_CALL_SYSTEM
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessNTPSync
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSyncFlag
);

/** @} */ /* end of ADDITIONAL MODULE */
#if 0
#endif
/**
* У��License���кŵ���Ч�� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szLicenseSerial     License���к�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_CHECK_LICENSE_SERIAL_FAIL
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CheckLicenseSerial
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szLicenseSerial[IMOS_LICENSE_AUTHORITY_CODE_LENGTH]
);


/**
* ��һϵ��license��������һ��license�ļ� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   apcLicenseSerials       ��Ȩ���б�
* @param [IN]   ulLicenseSerialCount    ��Ȩ�����
* @param [IN]   pstClientInfo           �ͻ���Ϣ
* @param [IN]   szContractId            ��ͬ���
* @param [OUT]  pstFtpInfo              FTP������Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_BP_GENERATE_HOST_ID_FILE_FAIL
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_MakeHostLicenseFile
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        (*apcLicenseSerials)[IMOS_LICENSE_AUTHORITY_CODE_LENGTH],
    IN  ULONG                       ulLicenseSerialCount,
    IN  CLIENTINFO_S                *pstClientInfo,
    IN  CHAR                        szContractId[IMOS_CONTRACT_ID],
    OUT FTP_INFO_S                  *pstFtpInfo
);

/**
* ��ҳ��ȡ��ǰ��Ч��License��Ȩ���б� \n
* Ŀǰֻ֧��һ�β�ѯ�����б���һҳ���������Ϊ��Ȩ���������#IMOS_LICENSE_SERIALS_MAX_NUM
* ���ڷ�ҳƫ�������ٹ�ע��
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  apcSerials          ��Ȩ���б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseSerialsList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CHAR                        (*apcSerials)[IMOS_LICENSE_AUTHORITY_CODE_LENGTH]
);


/**
* ��ҳ��ȡ��ǰ��Ч��License�����Ĺ����Ϣ�б� \n
* Ŀǰֻ֧��һ�β�ѯ�����б���һҳ���������Ϊlicense��Ե�ҵ��������:IMOS_LICENSE_SERVICE_TYPE_NUM
* ���ڷ�ҳƫ�������ٹ�ע��
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstServiceInfoList     ҵ�����б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseInfoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT LICENSE_AUTHORITYINFO_S     *pstServiceInfoList
);


/**
* ��ȡ��ǰ��Ч��License�ĳ�ʱʱ�� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [OUT]  pstExpireDate       ��ʱʱ��
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseExpireTime
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    OUT TIMELIMIT_S                 *pstExpireDate
);
/** @} */ /* end of LICENCE MANAGEMENT MODULE */

#if 0
#endif
/********************************************************************************************************
�������� Edit By sunyifan (06966) Complete
*********************************************************************************************************/
/**
* @name ��������
* @{    ��������
*/
/**
* �����豸����ģʽ \n
* �����Զ�����ģʽ��˵������Ҫ�������κ���������
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   ulUpgMode           ����ģʽ, ȡֵΪ#MM_UPGMODE_E
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_UPG_MODE_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetUpgMode
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                            ulUpgMode
);

/**
* ��ѯ�豸����ģʽ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [OUT]  pulUpgMode         ����ģʽ, ȡֵΪ#MM_UPGMODE_E
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUpgMode
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulUpgMode
);

/**
* ������������ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstUpgTaskInfo      ����������Ϣ
* @param [IN]   ulDevNum            �������豸�����д������豸��ʵ������, ���ȡֵΪ#UM_MAX_DEVICENUM_INJOB
* @param [IN]   pstUpgDevInfo       �������豸����
* @param [OUT]  szUpgTaskCode       �����������
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_UPGRADE_DEV_NUM_ERR
* - #ERR_DAO_JOB_DEV_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  UPG_TASK_INFO_S             *pstUpgTaskInfo,
    IN  ULONG                       ulDevNum,
    IN  UPG_DEV_INFO_S              *pstUpgDevInfo,
    OUT CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);

/**
* �޸�����������Ϣ���������Ҫ�޸������豸�б���ulDevNum��pstUpgDevInfo��Ч \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @param [IN]   pstUpgTaskInfo      ����������Ϣ
* @param [IN]   bISModifyDevList    �Ƿ���Ҫ�޸��豸�б�, BOOL_TRUEΪ�޸�; BOOL_FALSEΪ���޸�
* @param [IN]   ulDevNum            �������豸�����д������豸��ʵ������, ���ȡֵΪ#UM_MAX_DEVICENUM_INJOB
* @param [IN]   pstUpgDevInfo       �������豸����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_UPGRADE_DEV_NUM_ERR
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note 1.��������״̬Ϊ�ȴ������ʱ, �������޸�����
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyUpgTaskInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    IN  UPG_TASK_INFO_S             *pstUpgTaskInfo,
    IN  BOOL_T                      bIsModifyDevList,
    IN  ULONG                       ulDevNum,
    IN  UPG_DEV_INFO_S              *pstUpgDevInfo
);

/**
* ɾ���������� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note ����ɾ���Ѿ���ɵ�, ����ĺʹ��ڵȴ�״̬������������ִ���е�������ɾ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);

/**
* �������� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note �����������¼���״̬ʱ���������������ʧ�ܣ�
* - ������״̬�������ڹ��𣬻��Ѿ�����ʱ
* - �������������豸���Ѿ�ִ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_PauseUpgJob
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);


/**
* �������� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note  �����������¼���״̬ʱ���������������ʧ�ܣ�
* - ���������ڷǹ���״̬���ȴ������У����ڹ���
* - ����������ڣ�����ģ����Զ�ɾ����������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ActiveUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);


/**
* ��ҳ��ѯ������������  \n
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKUpgTaskList      ���������ѯ��Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note
* - 1. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 2. ֧�ְ������������ơ�״̬�������ߡ��������������/���/�ƶ�ʱ�����򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:
* -    #NAME_TYPE��#JOB_STATUS��#JOB_CREATOR��#JOB_RESULT��#JOB_EXEC_TIME��#JOB_END_TIME��#JOB_ESTB_TIME;
* - 3. ���������������Ĭ�ϰ�����������������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgTaskList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_TASK_QUERY_ITEM_S       *pstSDKUpgTaskList
);


/**
* ��ѯ����������Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @param [OUT]  pstUpgTaskInfo      ����������Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_JOB_NOT_EXIST
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    OUT UPG_TASK_INFO_S             *pstUpgTaskInfo
);

/**
* ��ѯָ�����������е������豸�б� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szUpgTaskCode       �����������
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstUpgDevInfo       �������豸����
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note
* - 1. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 2. ֧�ְ������豸���롢�������򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ������Ϊ#CODE_TYPE��#NAME_TYPE;
* - 3. ���������������Ĭ�ϰ��������豸������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgDev
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_DEV_INFO_S              *pstUpgDevInfo
);


/**
* ��ҳ��ѯ��Ҫ�������豸���Լ��豸�����汾����������\n
* @param [IN]   pstUserLogIDInfo   �û���¼��Ϣ
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstSDKUpgDevInfo   ��Ҫ�������豸�Լ���Ӧ�汾��Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* @note
* - 1. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 2. ֧�ְ������豸���롢�������򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ������Ϊ#CODE_TYPE��#NAME_TYPE;
* - 3. ���������������Ĭ�ϰ��������豸������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetDevVersion
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_ADVICE_INFO_S           *pstSDKUpgDevInfo
);



/**
* �ϴ���������ʼ,�Ȼ�ȡ�ϴ�ʱ����Ҫ����Ϣ\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szFileName          �ļ�����
* @param [IN]   szDeviceType        �豸����
* @param [OUT]  pstFtpInfo          FTP������Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_COMMON_DIR_FAIL_MKDIR
* - #ERR_COMMON_DIR_FAIL_REMOVE
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUploadUpgPackInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szFileName[IMOS_FILE_PATH_LEN],
    IN  CHAR                    szDeviceType[IMOS_DESC_LEN],
    OUT FTP_INFO_S              *pstFtpInfo
);


/**
* �ϴ����������,֪ͨ�������ϴ��ɹ�\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess          �ɹ����ı�ʶ, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   szFilePath          �ļ�·��
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_COMMON_FILE_CANT_OPEN
* - #ERR_COMMON_DIR_FAIL_OPEN
* - #ERR_COMMON_FAIL_STAT
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_NotifyUploadUpgPackResult
(
    IN USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN BOOL_T                    bIsSuccess,
    IN CHAR                      szFilePath[IMOS_FILE_PATH_LEN]
);


/**
* �ϴ��������ױ�ʼ, �Ȼ�ȡ�ϴ�ʱ����Ҫ����Ϣ\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szFileName          �ļ���
* @param [OUT]  pstFtpInfo          FTP������Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_COMMON_DIR_FAIL_REMOVE
* - #ERR_COMMON_DIR_FAIL_MKDIR
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUploadUpgMatchListInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szFileName[IMOS_FILE_PATH_LEN],
    OUT FTP_INFO_S              *pstFtpInfo
);

/**
* �ϴ��������ױ����, ֪ͨ�������ϴ��ɹ�\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess          �ɹ����ı�־, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_MM_MATCHTBL_OPEN_FAIL
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_NotifyUploadUpgMatchListResult
(
    IN USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN BOOL_T                    bIsSuccess
);
/** @} */ /* end of UPGRADE MANAGEMENT MODULE */

#if 0
#endif
/*******************************************************************************
��Ϣͳ��
*******************************************************************************/
/**
* @name ��Ϣͳ�ƹ���
* @{    ��Ϣͳ�ƹ���
*/
/**
* ��ǰҵ����Ϣ����ͳ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [OUT]  pstSrvStatInfo          ͳ��ҵ����Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySrvStat
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    OUT SRV_STAT_INFO_S         *pstSrvStatInfo
);


/** @} */  /* end of ��Ϣͳ�ƹ���ģ�� */


/**
* ��ѯ�豸����ͳ��
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       szOrgCode               ��֯����
* @param [IN]       ulDevType               �豸����,ȡֵΪ#IMOS_TYPE_DM, #IMOS_TYPE_MS, #IMOS_TYPE_VX500, #IMOS_TYPE_MONITOR, #IMOS_TYPE_EC, #IMOS_TYPE_DC, #IMOS_TYPE_CAMERA, #IMOS_TYPE_ECR, #IMOS_TYPE_DEV_ALL����Ϊ#IMOS_TYPE_DEV_ALL��Ϊͳ�����е��豸����
* @param [IN]       ulDevBelAttr            �豸�������ԣ�ȡֵΪ#IMOS_DEV_BELONG_ATTR����ȡ#IMOS_DEV_BELONG_ALLʱ��ʾ���ֱ�������ͳ�����е��豸
* @param [IN]       pstQueryCondition       ͨ�ò�ѯ���������޲�ѯ��������ΪNULL
* @param [IN][OUT]  pulArraySize            �豸״̬ͳ�������С, ����Ҫͳ�����е��豸���ͣ��������СӦ�ô��ڵ���#IMOS_DEVICE_TYPE_NUM
* @param [OUT]      pstDevStatistic         �豸״̬ͳ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevStatusStatistic
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    CHAR                           szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN    ULONG                          ulDevType,
    IN    ULONG                          ulDevBelAttr,
    IN    COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    INOUT ULONG                          *pulArraySize,
    OUT   DEV_STATUS_STATISTIC_S         *pstDevStatistic
 );


/**
* �����豸״̬��ѯ�豸��Ϣ�б�
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                   ��֯����
* @param [IN]   ulDevStatus,                �豸״̬��ȡֵΪ#IMOS_DEV_FAULT_STATUS_E, ��ȡֵΪ#IMOS_DEV_FAULT_STATUS_ALLʱ��ʾ����״̬����ѯ���е��豸
* @param [IN]   ulDevType                   �豸���ͣ�ȡֵΪ#IMOS_TYPE_DM, #IMOS_TYPE_MS, #IMOS_TYPE_VX500, #IMOS_TYPE_MONITOR, #IMOS_TYPE_EC, #IMOS_TYPE_DC, #IMOS_TYPE_CAMERA, #IMOS_TYPE_ECR. ��֧�ֲ�ѯ����(ֵΪ#IMOS_TYPE_DEV_ALL)���豸���͵��豸��Ϣ�б�
* @param [IN]   ulDevBelAttr                �豸�������ԣ�ȡֵ��Χ��#IMOS_DEV_BELONG_ATTR, ֵΪIMOS_DEV_BELONG_ALL��ʾ���ֱ������򣬲�ѯ���е��豸
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ���������޲�ѯ����, ������ΪNULL
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstDevInfoArray             �豸������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevListByFaultStatus
(
    IN USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN CHAR                           szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN ULONG                          ulDevStatus,
    IN ULONG                          ulDevType,
    IN ULONG                          ulDevBelAttr,
    IN COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S               *pstRspPageInfo,
    OUT DEV_BASIC_INFO_S              *pstDevInfoArray
 );
#if 0
#endif
/*******************************************************************************
����ǽ
*******************************************************************************/
/** @defgroup groupTvWall ����ǽ
*   ��������ǽ����ɾ�Ĳ�
*   @{
*/
/**
* ���ӵ���ǽ \n
* @param [IN]       pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN][OUT]  pstTVWallInfo      ����ǽ��Ϣ
* @return �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - #ERR_MM_PARENT_RES_NOT_EXIST
* -      ���ز��������:��������ļ�sdk_err.h
* @note
* - 1��������ǽ��ϢpstTVWallInfo�е���ǽ�������ʱ��дΪ�գ���ƽ̨����ɹ��󷵻ر��룬����д������д�ı������;
* - 2��һ������ǽ����������������ĿΪ#IMOS_MONITOR_MAXNUM_PER_WALL,����������������Ϊ��;
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTVWall
(
    IN      USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    INOUT   TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* ɾ������ǽ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szTVWallCode        ����ǽ����
* @return �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      ���ز��������:��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTVWall
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTVWallCode[IMOS_CODE_LEN]
);

/**
* �޸ĵ���ǽ��Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstTVWallInfo      ����ǽ��Ϣ
* @return �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      ���ز��������:��������ļ�sdk_err.h
* @note
* - 1������ǽ��ϢpstTVWallInfo�е���ǽ���벻�����޸�,�����ܵ����޸�ʧ�ܻ��޸Ĵ���;
* - 2��һ������ǽ����������������ĿΪIMOS_MONITOR_MAXNUM_PER_WALL,����������������Ϊ��;
* - 3�������ǽ����Ӽ����������Բ��øýӿ�ʵ�ֵ���ǽ��Ϣ�ĸ���;
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTVWall
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* ��ѯ����ǽ��ϸ��Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szTVWallCode        ����ǽ����
* @param [OUT]  pstTVWallInfo       ����ǽ��Ϣ
* @return �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      ���ز��������:��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTVWallInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTVWallCode[IMOS_CODE_LEN],
    OUT TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* ��ҳ��ѯĳ����֯�µĵ���ǽ�б� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode           ��֯�����
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[����ǽ����])
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstTVWallList       ����ǽ��Ϣ�б�
* @return �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* -      ���ز��������:��������ļ�sdk_err.h
* @note
* - 1��pstTVWallList��ҳ��ÿҳ����ĳ��ȱ�����ڻ����pstQueryPageInfo�ڵ�ulPageRowNum�������Խ��;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTVWallList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TV_WALL_QUERY_ITEM_S        *pstTVWallList
);
/** @} */ /* end of TV Wall */
#if 0
#endif

/**
* SDK����֪ͨ��Ϣ�ӿ�
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szDevCode                   ͨ���豸ID
* @param [IN]   ulBufferLen                 ��Ϣ���ȣ���󳤶�Ϊ:#IMOS_MAX_NOTIFY_LEN
* @param [IN]   pcBuffer                    ��Ϣ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �˽ӿ�Ŀǰֻ���Ÿ��İ���Ŀʹ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_Notify
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                   ulBufferLen,
    IN  CHAR                    *pcBuffer
);

#if 0
#endif

/**
* ���ͨ���豸
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstGeneralDevInfo           ͨ���豸��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.ͨ���豸���벻�����,�Ҳ��ܰ����ո�
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    GENERAL_DEV_INFO_S        *pstGeneralDevInfo
);

/**
* �޸�ͨ���豸
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstGeneralDevInfo           ͨ���豸��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    GENERAL_DEV_INFO_S        *pstGeneralDevInfo
);

/**
* ɾ��ͨ���豸
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szGeneralDevCode            ͨ���豸ID
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    CHAR                      szGeneralDevCode[IMOS_DEVICE_CODE_LEN]
);

/**
* ��ѯĳ����֯�µ�ͨ���豸�б�  \n
* ֧�ְ�����ģ����ѯ��pstName����Ϊ�գ���ʾ��������Ч���������ֽ���ģ����ѯ
* @param [IN]   pstUserLogIDInfo            �û���Ϣ��ʶ
* @param [IN]   szOrgCode                   �����
* @param [IN]   pstName                     ������ģ����ѯ��Ϣ����
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ���ط�ҳ��Ϣ
* @param [OUT]  GENERAL_DEV_INFO_S          ͨ���豸��Ϣ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGeneralDevList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  QUERY_BY_NAME_S                 *pstName,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GENERAL_DEV_INFO_S              *pstGeneralDevList
);

/**
* ��ѯĳ��ͨ�õ���Ϣ
* @param [IN]   pstUserLogIDInfo            �û���Ϣ��ʶ
* @param [IN]   szGeneralDevCode            ͨ���豸����
* @param [OUT]  pstGeneralDevInfo           ͨ���豸��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGeneralInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGeneralDevCode[IMOS_DEVICE_CODE_LEN],
    OUT GENERAL_DEV_INFO_S              *pstGeneralDevInfo
);

/**
* ��ѯ������NTPʱ��ͬ����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [OUT]  pbNTPSyncStatus         ������NTPʱ��ͬ����Ϣ���  pbNTPSyncStatusΪBOOL_FALSE��ʾֹͣͬ����ΪBOOL_TRUE��ʾ����ͬ��
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryNTPSyncStatus
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT BOOL_T                          *pbNTPSyncStatus
);

#if 0
#endif

/**
* ���� - ���V2������Ȩǩ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   pstExDomain            ������Ϣ�ṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   EX_DOMAIN_V2_S             *pstExDomain
);


/**
* ���� - �޸�V2�����벻���޸ģ�����Ҫ����
* @param [IN]  pstUserLogIDInfo          �û���Ϣ��ʶ
* @param [IN]  pstExDomain               ������Ϣ�ṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   EX_DOMAIN_V2_S             *pstExDomain
);

/**
* ���� - ��ѯ��ĳ����V2
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szExDomainCode          �������
* @param [OUT]  pstExDomain             ������Ϣ�ṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT  EX_DOMAIN_V2_S             *pstExDomain
);

/**
* ���� - ��ѯ���б�V2 \n
* @param [IN]   pstUserLogIDInfo       �û���Ϣ��ʶ
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ���ط�ҳ��Ϣ
* @param [OUT]  pstExDomainList        �����ѯ��Ϣ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 2. ֧�ְ�������롢���ơ����͡�IP���˿ڡ���������������鲥���ԡ�������־���򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:
* -   #CODE_TYPE��#NAME_TYPE��#EXT_DOMAIN_TPYE��#EXT_DOMAIN_IP��#EXT_DOMAIN_TRUNK_NUM��#EXT_DOMAIN_TRUNK_NUM��
* -   #EXT_DOMAIN_MULTICAST��#EXT_DOMAIN_SESSION;
* - 3. ���������������Ĭ�ϰ�������������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExDomainListV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT EX_DOMAIN_V2_S              *pstExDomainList
);

/**
* �������������V2�����Թ���Ҳ����ȡ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulShareFlag             �����־: 0Ϊȡ������1Ϊ����
* @param [IN]   ulShareCamNum           ������ĸ���
* @param [IN]   pstShareCamList         ����/ȡ��������б�

* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareCameraV2
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareCamNum,
    IN  SHARE_CAMERA_BASE_V2_S  *pstShareCamList
);

/**
* ��ѯ�����������б�V2
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szExDomainCode      �������
* @param [IN]   ulQueryFlag         ��ѯ���
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ���ط�ҳ��Ϣ
* @param [OUT]  pstShareCamList     ��ѯ����������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. ulQueryFlag��Ϊ��ѯ���:0��ʾ�鱾�����������������1��ʾ�������������������
* - 2. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 3. ֧�ְ���������롢�������򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:#CODE_TYPE��#NAME_TYPE;
* - 4. ���������������Ĭ�ϰ��������������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareCameraV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_CAM_INFO_V2_S         *pstShareCamList
);

/**
* ɾ����������Դ
* @param [IN]   pstUserLogIDInfo            �û���Ϣ��ʶ
* @param [IN]   ulResType                   ��Դ���ͣ�ȡֵIMOS_TYPE_E
* @param [IN]   szResCode                   ��ɾ������Դ����
* @param [IN]   szExdomainCode              �������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelResSharedByExdomain
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulResType,
    IN  CHAR                            szResCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szExdomainCode[IMOS_DOMAIN_CODE_LEN]
);

/***************************************************************************
������֯����
****************************************************************************/
/**
* @name ��֯�������
* @{    ��֯�������
*/
/**
* ��֯���������Թ���Ҳ����ȡ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulShareFlag             �����־: 0Ϊȡ������1Ϊ����
* @param [IN]   ulShareOrgNum           ��֯�ĸ���
* @param [IN]   pstShareOrgList         ����/ȡ����֯�б�

* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareOrg
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareOrgNum,
    IN  SHARE_ORG_BASE_S        *pstShareOrgList
);

/**
* ��ѯ��֯���������֯�ڵ㹲����Ϣ�б�ֻ��ȡһ�� \n
* �����ѯ����ʱ,������,��Ĭ������(��֯��������)
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   szOrgCode          ��֯�ڵ����
* @param [IN]   szExDomainCode     �������
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[��֯����])
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstOrgShareList    ��֯�ڵ㹲����Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOrgShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_ORG_INFO_S                *pstOrgShareList
);


/**
* ��ѯ���������/���������֯�б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulQueryFlag             ��ѯ��ǣ�0��ʾ�鱾������������֯��1��ʾ����������������֯
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareOrgList         ��ѯ������֯�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareOrgList
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_ORG_INFO_S        *pstShareOrgList
);


/***************************************************************************
����澯Դ����
****************************************************************************/
/**
* @name �澯Դ�������
* @{    �澯Դ�������
*/
/**
* �澯Դ���������Թ���Ҳ����ȡ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulShareFlag             �����־: 0Ϊȡ������1Ϊ����
* @param [IN]   ulShareAlarmSrcNum      �澯Դ�ĸ���
* @param [IN]   pstShareAlarmSrcList    ����/ȡ���澯Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareAlarmSrcNum,
    IN  SHARE_ALARMSRC_BASE_S   *pstShareAlarmSrcList
);

/**
* ��ѯ��֯����ĸ澯Դ������Ϣ�б�ֻ��ȡһ�� \n
* �����ѯ����ʱ,������,��Ĭ������(�澯Դ��������)
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   szOrgCode          ��֯�ڵ����
* @param [IN]   szExDomainCode     �������
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[�澯Դ����])
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstAlmSrcShareList �澯Դ������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_ALARMSRC_INFO_S           *pstAlmSrcShareList
);

/**
* ��ѯ���������ĸ澯Դ�б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulQueryFlag             ��ѯ��ǣ�0��ʾ�鱾���������ĸ澯Դ��1��ʾ�������������ĸ澯Դ
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareAlmSrcList      ��ѯ�����澯Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_ALARMSRC_INFO_S   *pstShareAlmSrcList
);

/***************************************************************************
����GIS��ͼ����
****************************************************************************/
/**
* @name GIS��ͼ�������
* @{    GIS��ͼ�������
*/
/**
* ��ͼ���������Թ���Ҳ����ȡ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulShareFlag             �����־: 0Ϊȡ������1Ϊ����
* @param [IN]   ulShareMapNum           ��ͼ�ĸ���
* @param [IN]   pstShareMapList         ����/ȡ����ͼ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����һ�ι���Ҫ����8����ͼ
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareMap
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareMapNum,
    IN  SHARE_MAP_BASE_S        *pstShareMapList
);

/**
* ��ѯ��֯����ĵ�ͼ������Ϣ�б�ֻ��ȡһ�� \n
* �����ѯ����ʱ,������,��Ĭ������(��ͼ��������)
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   szOrgCode          ��֯�ڵ����
* @param [IN    szExDomainCode     �������
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[��ͼ����])
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstMapShareList    ��ͼ������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMapShareInfoList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_MAP_INFO_S                *pstMapShareList
);

/**
* ��ѯ���������/������ĵ�ͼ�б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulQueryFlag             ��ѯ��ǣ�0��ʾ�鱾���������ĵ�ͼ��1��ʾ�������������ĵ�ͼ
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareMapList         ��ѯ������ͼ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareMapList
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_MAP_INFO_S        *pstShareMapList
);

#if 0
#endif

/*************************************************************************************
��Դ�����ã��������ӣ��޸ģ�ɾ������ѯ����ѯ�б�
***************************************************************************************/
/**
* ������Դ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   szGroupName             ��Դ������
* @param [IN]   ulGroupResType          ��Դ�����ͣ�ö��ΪIMOS_TYPE_E�����������ΪIMOS_TYPE_CAMERA_GROUP
* @param [IN]   ulResNum                ������Դ��Ŀ
* @param [IN]   pstResGroupUnit         ������Դ��Ԫ��Ϣ
* @param [OUT]  szResGroupCode          ��Դ�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻ֧���������/�������������
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddResGroup
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szGroupName[IMOS_NAME_LEN],
    IN  ULONG                       ulGroupResType,
    IN  ULONG                       ulResNum,
    IN  RESOURCE_GROUP_UNIT_S       *pstResGroupUnit,
    OUT CHAR                        szResGroupCode[IMOS_RES_CODE_LEN]
);


/**
* �޸���Դ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   szResGroupCode          ��Դ�����
* @param [IN]   szGroupName             ��Դ������
* @param [IN]   ulGroupResType          ��Դ�����ͣ�ö��ΪIMOS_TYPE_E�����������ΪIMOS_TYPE_CAMERA_GROUP
* @param [IN]   ulResNum                ������Դ��Ŀ
* @param [IN]   pstResGroupUnit         ������Դ��Ԫ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻ֧���������/�������������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyResGroup
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szResGroupCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szGroupName[IMOS_NAME_LEN],
    IN  ULONG                       ulGroupResType,
    IN  ULONG                       ulResNum,
    IN  RESOURCE_GROUP_UNIT_S       *pstResGroupUnit
);

/**
* ɾ����Դ��
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulGroupResType          ��Դ�����ͣ�ö��ΪIMOS_TYPE_E�����������ΪIMOS_TYPE_CAMERA_GROUP
* @param [IN]   szResGroupCode          ��Դ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelResGroup
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulGroupResType,
    IN  CHAR                            szResGroupCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ��Դ����Ϣ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szResGroupCode          ��Դ�����
* @param [IN]   ulGroupResType          ��Դ������
* @param [OUT]  szGroupName             ��Դ������
* @param [OUT]  pulResNum               ������Դ��Ŀ
* @param [OUT]  pstResGroupUnit         ������Դ��Ԫ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResGroup
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szResGroupCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulGroupResType,
    OUT CHAR                            szGroupName[IMOS_NAME_LEN],
    OUT ULONG                           *pulResNum,
    OUT RESOURCE_GROUP_UNIT_S           *pstResGroupUnit
);

/**
* ��ҳ��ѯĳ����֯�µ���Դ���б�  \n
* ����Ϊ��ʱ,��ѯ��֯��������Դ���б�,��Ĭ�ϰ���"��Դ����������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   ulGroupResType          ��Դ������
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[��Դ������])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstResGroupList         ��Դ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResGroupList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulGroupResType,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT RESOURCE_GROUP_INFO_S       *pstResGroupList
);


#if 0
#endif

/*************************************************************************************
�������ã��������ӣ��޸ģ�ɾ������ѯ����ѯ�б�
***************************************************************************************/
#if 0/* ��ʱֻ�ṩ��ѯ���� */
/**
* ���Ӳ���
* @param [IN]     pstUserLogIDInfo        �û���Ϣ
* @param [INOUT]  pstLayoutInfo           ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
ULONG STDCALL IMOS_AddLayout
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    INOUT   LAYOUT_INFO_S               *pstLayoutInfo
);

/**
* �޸Ĳ���
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstLayoutInfo           ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
ULONG STDCALL IMOS_ModifyLayout
(
    IN   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN   LAYOUT_INFO_S               *pstLayoutInfo
);


/**
* ɾ������
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szLayoutCode            ���ֱ���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
ULONG STDCALL IMOS_DelLayout
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szLayoutCode[IMOS_RES_CODE_LEN]
);
#endif

/**
* ��ѯ������Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szLayoutCode            ���ֱ���
* @param [OUT]  pstLayoutInfo           ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryLayout
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szLayoutCode[IMOS_RES_CODE_LEN],
    OUT LAYOUT_INFO_S                   *pstLayoutInfo
);

/**
* ��ҳ��ѯ�����б�  \n
* ����Ϊ��ʱ,��ѯ���в����б�,��Ĭ�ϰ���"������������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* - ����NULL
* - ��������:NAME_TYPE(��������),LAYOUT_TYPE(�������ͣ�ȡֵLAYOUT_TYPE_E)
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstLayoutList           �����б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryLayoutList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT LAYOUT_INFO_QUERY_ITEM_S    *pstLayoutList
);

#if 0
#endif

/*************************************************************************************
����ʾ���ã��������ӣ��޸ģ�ɾ������ѯ����ѯ�б�
***************************************************************************************/
/**
* ��������ʾ
* @param [IN]       pstUserLogIDInfo        �û���Ϣ
* @param [IN]       szOrgCode               ��֯����
* @param [IN][OUT]  pstSalvoInfo            ����ʾ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����ʾ��Ϣ������ʾ�������Ϊ��;����ε�����ʾ����Ϊ��, ��Я�����ڲ����ɵ�����ʾ����)
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddSalvo
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN      CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    INOUT   SALVO_INFO_S                *pstSalvoInfo
);

/**
* �޸�����ʾ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstSalvoInfo            ����ʾ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySalvo
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN   SALVO_INFO_S               *pstSalvoInfo
);


/**
* ɾ������ʾ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSalvoCode             ����ʾ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelSalvo
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szSalvoCode[IMOS_CODE_LEN]
);

/**
* ��ѯ����ʾ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szSalvoCode             ����ʾ����
* @param [OUT]  pstSalvoInfo            ����ʾ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySalvo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSalvoCode[IMOS_CODE_LEN],
    OUT SALVO_INFO_S                    *pstSalvoInfo
);

/**
* ��ҳ��ѯĳ����֯�µ�����ʾ�б�  \n
* ����Ϊ��ʱ,��ѯ��֯����������ʾ�б�,��Ĭ�ϰ���"����ʾ��������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* - ����NULL
* - ��������:NAME_TYPE(����ʾ����),RES_SUB_TYPE(����ʾ���ͣ�ȡֵ#SALVO_TYPE_E)
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSalvoList            ����ʾ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySalvoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SALVO_QUERY_ITEM_S          *pstSalvoList
);

/**
* ��������ʾ \n
* @param [IN]   pstUserLoginIDInfo          �û���¼ID��Ϣ��ʶ
* @param [IN]   szSalvoCode                 ����ʾ����
* @param [IN]   ulStreamType                ������, ȡֵΪ#IMOS_FAVORITE_STREAM_E
* @param [IN]   ulOperateCode               ����ԭ����, ȡֵΪ#CS_OPERATE_CODE_E
* @param [IN]   ulSrcHandle                 ��������ʾԴ���
* @param [IN]   ulMsgSequence               ��Ϣ���к�
* @param [OUT]  pulSalvoUnitNum             ����ʾ��Ԫ��Ŀ
* @param [OUT]  pstSalvoUnitList            ����ʾ��Ԫ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �ֶ���������ʾ��ʹ��ulSrcHandle, ������ЧֵIMOS_INVALID_HANDLE;
        �ֶ���������ʾ��ʹ��ulMsgSequence, ������Чֵ0;
*       ����Ѳ��������ʾʱulSrcHandle��Ч, ��д����Ѳ�ϱ���Ϣ��Я����Դ���;
        ����Ѳ��������ʾʱulMsgSequence��Ч, ��д����Ѳ�ϱ���ϢЯ������Ϣ���к�
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartSalvo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szSalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulStreamType,
    IN  ULONG                       ulOperateCode,
    IN  ULONG                       ulSrcHandle,
    IN  ULONG                       ulMsgSequence,
    OUT ULONG                       *pulSalvoUnitNum,
    OUT CS_SALVO_START_SALVO_UNIT_S *pstSalvoUnitList
);

/**
* ֹͣ����ʾ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szSalvoCode             ����ʾ����
* @param [IN]   ulOperateCode           ����ԭ����, ȡֵΪ#CS_OPERATE_CODE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopSalvo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szSalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulOperateCode
);

/**
* ����/ֹͣ/ǰ��/����/�ָ�/��ͣ����Ѳ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szGroupsalvoCode        ����Ѳ����
* @param [IN]   ulProcessType           ��������: ����/ֹͣ/ǰ��/����/�ָ�/��ͣ #MANUAL_OPER_TYPE_E
* @param [IN]   ulOperateCode           ����ԭ����: #CS_OPERATE_CODE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessGroupsalvo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szGroupsalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulProcessType,
    IN  ULONG                   ulOperateCode
);

/**
* ����/ֹͣ����Ѳ�ƻ� \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szGroupsalvoPlanCode    ����Ѳ�ƻ�����
* @param [IN]   ulProcessType           ��������: ����/ֹͣ
* @param [IN]   ulOperateCode           ����ԭ����: #CS_OPERATE_CODE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessGroupsalvoPlan
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szGroupsalvoPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulProcessType,
    IN  ULONG                   ulOperateCode
);

#if 0
#endif

/*************************************************************************************
ͨ��������Դ���ã��������ӣ��޸ģ�ɾ������ѯ����ѯ�б�
***************************************************************************************/
/**
* ����ͨ��������Դ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstSwitchInfo           ͨ��������Դ��Ϣ
* @param [OUT]  szSwitchResCode         ͨ��������Դ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻ֧������Ѳ
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_SWITCH_RESOURCE_S    *pstSwitchInfo,
    OUT CHAR                        szSwitchResCode[IMOS_RES_CODE_LEN]
);

/**
* �޸�ͨ��������Դ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSwitchResCode         ͨ��������Դ����
* @param [IN]   pstSwitchInfo           ͨ��������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szSwitchResCode[IMOS_RES_CODE_LEN],
    IN  COMMON_SWITCH_RESOURCE_S    *pstSwitchInfo
);

/**
* ɾ��ͨ��������Դ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulSwitchType            �������ͣ�ȡֵΪ#COMMON_SWITCH_TYPE_E
* @param [IN]   szSwitchResCode         ͨ��������Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ULONG                   ulSwitchType,
    IN  CHAR                    szSwitchResCode[IMOS_RES_CODE_LEN]
);


/**
* ��ѯͨ��������Դ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulSwitchType            �������ͣ�ȡֵΪ#COMMON_SWITCH_TYPE_E
* @param [IN]   szSwitchResCode         ͨ��������Դ����
* @param [OUT]  pstSwitchInfo           ͨ��������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchType,
    IN  CHAR                            szSwitchResCode[IMOS_RES_CODE_LEN],
    OUT COMMON_SWITCH_RESOURCE_S        *pstSwitchInfo
);

/**
* ��ҳ��ѯĳ����֯�µ�ͨ��������Դ�б�  \n
* ����Ϊ��ʱ,��ѯ��֯������ͨ��������Դ�б�,��Ĭ�ϰ���"������Դ��������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   ulSwitchType            �������ͣ�ȡֵΪ#COMMON_SWITCH_TYPE_E
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* - ����NULL
* - ��������:NAME_TYPE(������Դ����),RES_SUB_TYPE(���������ͣ�������Ѳȡֵ#GROUP_SWITCH_TYPE_E)
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSwitchResList        ͨ��������Դ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulSwitchType,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SWITCH_RES_QUERY_ITEM_S     *pstSwitchResList
);

#if 0
#endif

/*************************************************************************************
ͨ�����мƻ����ã��������ӣ��޸ģ�ɾ������ѯ����ѯ�б�
***************************************************************************************/
/**
* ����ͨ�����мƻ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSwitchPlanInfo       ͨ�����мƻ���Ϣ
* @param [OUT]  szSwitchPlanCode        ͨ�����мƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo,
    OUT CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);


/**
* �޸�ͨ�����мƻ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szSwitchPlanCode        ���мƻ�����
* @param [IN]   pstSwitchPlanInfo       ���мƻ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    IN  COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo
);


/**
* ɾ��ͨ�����мƻ�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulSwitchPlanType        ͨ�����мƻ����ͣ�ȡֵΪ#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   szSwitchPlanCode        ͨ�����мƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchPlanType,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯͨ�����мƻ���Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   ulSwitchPlanType        ͨ�����мƻ����ͣ�ȡֵΪ#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   szSwitchPlanCode        ͨ�����мƻ�����
* @param [OUT]  pstSwitchPlanInfo       ͨ�����мƻ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchPlanType,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    OUT COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo
);

/**
* ��ҳ��ѯͨ�����мƻ��б�
* ����Ϊ��ʱ,��ѯ����ͨ�����мƻ�,��Ĭ�ϰ���"ͨ�����мƻ���������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   ulSwitchPlanType        ͨ�����мƻ����ͣ�ȡֵΪ#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[���мƻ�����])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSwitchPlanList       ͨ�����мƻ��б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchPlanList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                           ulSwitchPlanType,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT COMMONSWITCH_PLAN_QUERY_ITEM_S  *pstSwitchPlanList
);



/**
* ��ӳ�����Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstFactoryInfo      ����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddManufacturerType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  MANUFACTURER_TYPE_INFO_S            *pstManufacturerInfo
);

/**
* ɾ��������Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   ulFactoryTypeId     ����������ϢID
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelManufacturerType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulManufacturerTypeId
);

/**
* �޸ĳ�����Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstFactoryInfo      ����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyManufacturerType
(
    IN    USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN    MANUFACTURER_TYPE_INFO_S            *pstManufacturerInfo
);

/**
* ��ѯ������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulTypeId           ����ID
* @param [OUT]  pstFactoryInfo     ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryManufacturerType
(
    IN   USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  ULONG                              ulManufacturerTypeId,
    OUT  MANUFACTURER_TYPE_INFO_S          *pstManufacturerInfo
);

/**
* ��ѯ���������б� \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo   ��ѯ��ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ��ѯ���ط�ҳ��Ϣ
* @param [OUT]  pstFactoryInfoList ������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryManufacturerTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT MANUFACTURER_TYPE_INFO_S        *pstManufacturerInfoList
);

/**
* ����豸������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstDevTypeInfo     �豸������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEV_TYPE_NOT_EXIST �豸���Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDevType
(
    IN  USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN  DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* ɾ���豸������Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   ulDevTypeId         �豸��Ϣ����ID
* @param [IN]   ulDevSubTypeID      �豸��Ϣ������ID
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEV_TYPE_NOT_EXIST �豸���Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelDevType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulDevTypeId,
    IN  ULONG                               ulDevSubTypeID
);

/**
* �޸��豸������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstDevTypeInfo     �豸������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEV_TYPE_NOT_EXIST �豸���Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyDevType
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* ��ѯ�豸������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulTypeId           ����ID
* @param [IN]   ulDevSubTypeId     �豸��Ϣ������ID
* @param [OUT]  pstDevTypeInfo     �豸������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DEV_TYPE_NOT_EXIST �豸���Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevType
(
    IN   USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN   ULONG                          ulDevTypeId,
    IN   ULONG                          ulDevSubTypeId,
    OUT  DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* ��ѯ�豸�����б� \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo   ��ѯ��ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ��ѯ���ط�ҳ��Ϣ
* @param [OUT]  pstDevTypeInfoList ������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT DEV_TYPE_INFO_S                 *pstDevTypeInfoList
);

/**
* ����¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventType       �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_TYPE_ID_EXIST �¼�����ID�Ѵ���
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddEventType
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_TYPE_S                *pstEventType
);

/**
* ɾ���¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulDevTypeId        �¼���Ϣ����ID
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST �¼����Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelEventType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulEventTypeId
);

/**
* �޸��¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventType       �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST �¼����Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyEventType
(
    IN    USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN    EVENT_TYPE_INFO_S               *pstEventTypeInfo
);

/**
* ��ѯ�¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulDevTypeId        �¼���Ϣ����ID
* @param [OUT]  pstEventTypeInfo   �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST �¼����Ͳ�����
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventType
(
    IN   USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN   ULONG                            ulEventTypeId,
    OUT  EVENT_TYPE_INFO_S                *pstEventTypeInfo
);

/**
* ��ѯ�¼������б� \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulDevTypeId        �豸����ID
* @param [IN]   ulDevSubTypeId     �豸��Ϣ������ID
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo   ��ѯ��ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ��ѯ���ط�ҳ��Ϣ
* @param [OUT]  pstEventTypeInfo   �¼�������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulDevTypeId,
    IN  ULONG                           ulDevSubTypeId,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_TYPE_INFO_S               *pstEventTypeInfoList
);
/***************************************************************************
 ����
****************************************************************************/
/**
* ���ĸ澯 \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ��ʶ
* @param [IN]   pstSubscInfo        ������Ϣ
* @param [OUT]  szSessionID         ���ض��ĳɹ���ҵ��ID
* @return �������½��:
* - �ɹ�:
* - ʧ��:
* -      ���ز��������:��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_SubscribeEvent
(
    IN USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN SUBSC_BASE_INFO_S      *pstSubscInfo,
    OUT CHAR  szSessionID[IMOS_SSN_ID_LEN]
);

/**
* �����澯 \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ��ʶ
* @param [IN]   ulExpires           ����ʱ��
* @param [IN]   szSessionID         ���ĳɹ���ҵ��ID
* @return �������½��:
* - �ɹ�:
* - ʧ��:
* -      ���ز��������:��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_RenewSubscribe
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN ULONG  ulExpires,
    IN CHAR   szSessionID[IMOS_SSN_ID_LEN]
);

/**
* ȡ������ \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ��ʶ
* @param [IN]   szSessionID         ���ĳɹ���ҵ��ID
* @return �������½��:
* - �ɹ�:
* - ʧ��:
* -      ���ز��������:��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_CancelSubscribe
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR  szSessionID[IMOS_SSN_ID_LEN]
);

/**
* ��ѯ�����б� \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ��ʶ
* @param [IN]   pstQueryCondition   ��ѯ����
* @param [IN]   pstQueryPageInfo    ��ҳ������Ϣ
* @param [OUT]  pstRspPageInfo      ��ҳ���󷵻���Ϣ
* @param [OUT]  pstSubscList        ��ѯ�ĸ澯��Ϣ
* @return �������½��:
* - �ɹ�:
* - ʧ��:
* -      ���ز��������:��������ļ�sdk_err.h
* @note �ڲ�ѯ��������¼ʱpstSubscFullInfo��ָ���ѯ�б��׼�¼�ĵ�ַ
*       Ŀǰ��֧�ֲ�ѯ����
*/
IMOS_EXPORT ULONG IMOS_QuerySubscribeList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SUBSC_FULL_INFO_S           *pstSubscList
);

/**
* ��������
* @param [IN]   pstUserLogIDInfo �û���½��Ϣ
* @param [IN]   pstTrunkInfo     ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TRUNK_INFO_S            *pstTrunkInfo
);

/**
* �޸ĸ�����Ϣ
* @param [IN]   pstUserLogIDInfo �û���½��Ϣ
* @param [IN]   pstTrunkInfo     ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TRUNK_INFO_S            *pstTrunkInfo
);

/**
* ɾ��������Ϣ
* @param [IN]   pstUserLogIDInfo �û���½��Ϣ
* @param [IN]   szTrunkCode      ���߱���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ�����б���Ϣ
* @param [IN]   pstUserLogIDInfo     �û���½��Ϣ
* @param [IN]   pstQueryCondition    ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo     �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo       ���ط�ҳ��Ϣ
* @param [OUT]  pstTrunkList         �����б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �޲�ѯ������pstQueryCondition����ΪNULL,
*       Ŀǰֻ֧�����Ʋ�ѯ����.
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTrunkList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRUNK_INFO_S                *pstTrunkList
);

/**
* ��Ӹ���������Ϣ
* @param [IN]   pstUserLogIDInfo      �û���½��Ϣ
* @param [IN]   szTrunkCode           ���߱���
* @param [IN]   ulContentNum          ����������Ŀ,���ֵΪIMOS_TRUNK_CONTENT_MAXNUM
* @param [IN]   pstTrunkContentList   ���������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTrunkContent
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulContentNum,
    IN  TRUNK_CONTENT_INFO_S    *pstTrunkContentList
);

/**
* ɾ������������Ϣ
* @param [IN]   pstUserLogIDInfo    �û���½��Ϣ
* @param [IN]   szTrunkContentCode  �������ݱ���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteTrunkContent
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkContentCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ����������Ϣ
* @param [IN]   pstUserLogIDInfo     �û���½��Ϣ
* @param [IN]   szTrunkCode          ���߱���
* @param [IN]   pstQueryCondition    ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo     �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo       ���ط�ҳ��Ϣ
* @param [OUT]  pstTrunkContentList  ���������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ�ݲ�֧���κβ�ѯ����
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTrunkContentList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szTrunkCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRUNK_CONTENT_INFO_S        *pstTrunkContentList
);

/**
* �������߹���
* @param [IN]   pstUserLogIDInfo  �û���½��Ϣ
* @param [IN]   szTrunkCode       ���߱���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_EnableTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* ֹͣ���߹���
* @param [IN]   pstUserLogIDInfo  �û���½��Ϣ
* @param [IN]   szTrunkCode       ���߱���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DisableTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* �������
* @param [IN]   pstUserLogIDInfo  �û���½��Ϣ
* @param [IN]   szResCode         ��Դ����
* @param [OUT]  szSsnID           �ỰID
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �ú������ĳ�ֿ��ܲ����������ҵ�����Դʹ��:
* -     Ŀǰ�ڻط�/����¼��ǰ����ҪΪ�������������ߡ�
* -     ����ɹ������ҵ���ͷ���Ҫ�����ͷź����ͷ�����ĸ��ߡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_ApplyTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    OUT CHAR                    szSsnID[IMOS_SSN_ID_LEN]
);

/**
* �ͷŸ���
* @param [IN]   pstUserLogIDInfo   �û���½��Ϣ
* @param [IN]   szSsnID            �ỰID
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_FreeTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szSsnID[IMOS_SSN_ID_LEN]
);

/**
* ����ϵͳ����װ��ʽ
* @param [IN] pstUserLogIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN] pstSysStreamFormat    ϵͳ��װ����ʽ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* �������� UDP+TS , RTP+TS,RTP_NO_RTCP+ES
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigSysStreamFormat
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SYS_STREAM_FORMAT_S             *pstSysStreamFormat
);

/**
* ��ѯϵͳ����װ��ʽ
* @param [IN] pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [OUT] pstSysStreamFormat    ϵͳ��װ����ʽ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySysStreamFormat
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT SYS_STREAM_FORMAT_S             *pstSysStreamFormat
);

/**
* ��������󶨵�EC��ĳ��ͨ���ϣ���Ҫ����������Ϣ��֧�ֳ���OSD���ڸ�������Ƶ�����ڵ���������˶������������Ԫ�ر䳤
* @param [IN]   pstUserLogIDInfo             �û���Ϣ
* @param [IN]   pstVINBindCamBaseInfo        ����������ͨ���Ļ�����Ϣ
* @param [IN]   pstVINFullVarietyElement     ��Ƶͨ��ȫ���䳤Ԫ�ؽṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* 1.��������벻��Ϊ��,�Ҳ��ܰ����ո�
* 2.�䳤Ԫ�ظ����ͱ䳤Ԫ���ڴ泤���ɵ����߱�֤���䳤Ԫ�ظ������ɴ��ڱ䳤Ԫ��ʵ���ڴ泤��
*/
IMOS_EXPORT ULONG STDCALL IMOS_BindCameraToVideoInChannelV2
(
    IN  USER_LOGIN_ID_INFO_S                    *pstUserLogIDInfo,
    IN  VINCHNL_BIND_CAMERA_BASE_S              *pstVINBindCamBaseInfo,
    IN  VINCHNL_FULL_VARIETY_ELEMENT_S          *pstVINFullVarietyElement
);

/**
* ��ѯĳ����֯�µ�ĳ̨�豸��������洢��Դ��������б�V2
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstStoreResList         ������洢��Դ������Ϣ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.����޲�ѯ����,ָ��pstQueryCondition����ΪNULL
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamStoreResListV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT STORE_RES_QUERY_ITEM_V2_S   *pstStoreResList
);

#if 0
#endif
/**************************************************************************************************************
�豸-BM
***************************************************************************************************************/
/** @defgroup group BM ����������
*   �������������BM,������Դ���������񣬱����ļ������ݼƻ������
*   @{
*/
/**
* @name BM����
* @{    BM����
*/
/**
* ���BM������ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBMInfo           BM��Ϣ
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BM_INFO_S                   *pstBMInfo
);

/**
* �޸�BM������ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBMInfo           BM��Ϣ
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BM_INFO_S                   *pstBMInfo
);

/**
* ɾ��BM������ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szBMCode            BM����
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szBMCode[IMOS_DEVICE_CODE_LEN]
);

/**
* ��ѯBM�������б� \n
* szOrgCodeֵ��Ч�����κ�ֵ����ֻ��ѯ����BM�б�
* ����Ϊ��ʱ,������,��Ĭ��"������������"  \n
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode              ��֯����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[BM����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKBMList           BM�б�
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBMList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT BM_QUERY_ITEM_S             *pstSDKBMList
);
/** @} */ /* end of DEVICE BM MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
������Դ����
****************************************************************************/
/**
* @name ������Դ����
* @{    �漰BM������Դ����,�����������Դ����
*/
/**
* �Ӵ洢�豸ΪBM������������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBMResInfo        BM��Դ��Ϣ
* @param [OUT]  szBMResCode         BM��Դ����
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignBMResFromStoreDev
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  DM_RES_FOR_BM_INFO_S    *pstBMResInfo,
    OUT CHAR                    szBMResCode[IMOS_CODE_LEN]
);

/**
* �ӵ�������ԴΪBM������������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBMResInfo        BM��Դ��Ϣ
* @param [OUT]  szBMResCode         BM��Դ����
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignBMResFromThird
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  THIRD_RES_FOR_BM_INFO_S   *pstBMResInfo,
    OUT CHAR                      szBMResCode[IMOS_CODE_LEN]
);

/**
* ɾ��BM�������Ϸ���ı�����Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN    szResCode           BM��Դ����
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBMRes
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szResCode[IMOS_CODE_LEN]
);

/**
* ��ѯBM������Դ�б� \n
* Ŀǰֻ֧����֯����szOrgCode����Ϊ�մ��Ĳ�ѯ \n
* ����Ϊ��ʱ,������,��Ĭ��"������������"  \n
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szBMCode               BM����
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[BM��Դ����])
* @param [IN]   pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKBMResList        BM������Դ�б�
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBMResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szBMCode[IMOS_DEVICE_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT BM_RES_QUERY_ITEM_S         *pstSDKBMResList
);
#if 0
#endif
/**
* ���������������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstCamBakResInfo    �����������Դ��Ϣ
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_BAK_RES_INFO_S      *pstCamBakResInfo
);

/**
* �޸��������������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstCamBakResInfo    �����������Դ��Ϣ
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_BAK_RES_INFO_S      *pstCamBakResInfo
);

/**
* ɾ�������������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCamCode           ���������
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCamCode[IMOS_DEVICE_CODE_LEN]
);

/**
* ��ѯ�����������Դ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szCamCode           ���������
* @param [OUT]  ulItemNum           �����������Դ��Ϣ����
* @param [OUT]  pstCamBakResList    �����������Դ�б�
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ���������ڱ���ı�����Դ��ѯ
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szCamCode[IMOS_DEVICE_CODE_LEN],
    OUT ULONG                     *pulItemNum,
    OUT CAM_BAK_RES_INFO_S        *pstCamBakResList
);


/**
* ��ҳ��ѯĳ����֯�������������Դ��������б�
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[���������])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstCamBakResList        �����������Դ������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ���������ڱ���ı�����Դ��ѯ
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamBakResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CAM_BAK_RES_QUERY_ITEM_S    *pstCamBakResList
);

/** @} */ /* end of BACKUP RESOURCE MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
�����������
****************************************************************************/
/**
* @name �����������
* @{    �����������
*/
/**
* �����ֶ����� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstManulBakInfo     �ֶ�����������Ϣ
* @param [OUT]  szTaskCode          �����������
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�������������ֶ�����
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetManualBak
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  MANUL_BAK_INFO_S          *pstManulBakInfo,
    OUT CHAR                      szTaskCode[IMOS_CODE_LEN]
);

/**
* ɾ���������� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szTaskCode          �����������
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBakTask
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTaskCode[IMOS_CODE_LEN]
);

/**
* ��ѯ���������б� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   ulTaskType              ��������#BAK_TYPE_E
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKBakTaskList       ���������б�
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1��ͨ�ò�ѯ����֧��:����״̬[#TASK_STATUS]�������ύʱ��[#TASK_SUB_TIME]���������ʱ��[#TASK_END_TIME]�����������[#NAME_TYPE]��
* -    ¼��ʼʱ��[#REC_START_TIME]��¼�����ʱ��[#REC_END_TIME]���û�����[#USER_NAME_TYPE]����������[#CASE_DESC]�Ĳ�ѯ������;
* - 2��ͨ�ò�ѯ����ΪNULL,������������ʱ,Ĭ�ϰ�"�����ύʱ������"����;
* - 3����admin�û�����������ֻ�����ֶ�����[#BAK_MANUL];
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBakTaskList
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S   *pstQueryCondition,
    IN  ULONG                      ulTaskType,
    IN  QUERY_PAGE_INFO_S          *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S            *pstRspPageInfo,
    OUT BAK_TASK_QUERY_ITEM_S      *pstSDKBakTaskList
);
/** @} */ /* end of BACKUP TASK MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
�����ļ�����
****************************************************************************/
/**
* @name �����ļ�����
* @{    �����ļ�����
*/
/**
* ���������ļ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBakFileInfo      �����ļ�������Ϣ
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_HandleBakFile
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  BAK_FILE_HANDLE_INFO_S     *pstBakFileInfo
);

/**
* ����¼����� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSDKRecordFileInfo    �����ļ���Ϣ���ݽṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1��ͨ�ò�ѯ����֧��:���������[#CODE_TYPE]�����������[#NAME_TYPE]���ļ�����ʱ��[#FILE_CREATE_TIME]��
* -    ���ݿ�ʼʱ��[#BAK_START_TIME]�����ݽ���ʱ��[#BAK_END_TIME]���ļ�����[#FILE_CAPACITY]���ļ�����[#FILE_TYPE]��
* -    �ļ�������ʶ[#FILE_LOCK_FLAG]����������[#CASE_DESC]�Ĳ�ѯ������;
* - 2��ͨ�ò�ѯ����ΪNULL,������������ʱ,Ĭ�ϰ�"���ݿ�ʼʱ������"����;
* - 3��ͨ�ò�ѯ��������ӵı��ݿ�ʼʱ�������ʱ��֮���ʱ���Ȳ��ܿ��죬��ʱ���ʽΪ��"%Y-%m-%d %H:%M:%S"��ʽ ����Ϣ�����޶�Ϊ24�ַ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_BakRecordRetrieval
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CAM_BAK_FILE_QUERY_ITEM_S   *pstRecordFileInfo
);

/**
* ��ȡ����¼���ļ���URL��Ϣ \n
* @param[IN]    pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param[IN]    pstSDKGetBakUrlInfo     ��ȡURL������Ϣ���ݽṹ
* @param[OUT]    pstSDKURLInfo          URL��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetBakRecordFileURL
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  GET_BAK_URL_INFO_S          *pstSDKGetBakUrlInfo,
    OUT URL_INFO_S                  *pstSDKURLInfo
);
/** @} */ /* end of BACKUP FILE MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
���ݼƻ�
****************************************************************************/
/**
* @name ���ݼƻ�����
* @{    �漰���ݼƻ���ɾ�Ĳ�,�Լ���ѯ���ݼƻ��б�ӿ�
*/
/**
* ��ӱ��ݼƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBakPlanInfo      ���ݼƻ���Ϣ
* @param [OUT]  szBakPlanCode       ���ݼƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BAK_PLAN_INFO_S             *pstBakPlanInfo,
    OUT CHAR                        szBakPlanCode[IMOS_RES_CODE_LEN]
);

/**
* �޸ı��ݼƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstBakPlan          ���ݼƻ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note �������ݼƻ���Ҫ����IMOS_ProcessPlan�ӿ�; ��ͨ���޸ļƻ�ʱ��ķ�ʽ���ܹ��Զ�ʹ�ƻ�����/ֹͣ
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  BAK_PLAN_INFO_S                 *pstBakPlanInfo
);

/**
* ɾ�����ݼƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szBakPlanCode       ���ݼƻ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szBakPlanCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ���ݼƻ���Ϣ \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szBakPlanCode       ���ݼƻ�����
* @param [OUT]  pstBakPlanInfo      ���ݼƻ���Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szBakPlanCode[IMOS_RES_CODE_LEN],
    OUT BAK_PLAN_INFO_S                 *pstBakPlanInfo
);

/**
* ����/ֹͣ�ƻ� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   ulPlanType          �ƻ�����#PLAN_TYPE_E
* @param [IN]   szStorePlanCode     �ƻ�����
* @param [IN]   ulPlanMode          �ƻ�������, #PLAN_OPER_MODE_E
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_STORAGE_PLAN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention ���ӿ�֧�ֶԹ�����������б�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulPlanType,
    IN  CHAR                            szPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulPlanMode
);

/**
* ���ڳ�ʼ������ڡ� \n
* @param [IN]       szServerAdd     ������IP
* @param [IN]       ulServerPort    �������˿�
* @param [IN][OUT]  pstInit         SDK��ʼ������������pParam�ֶ�ΪINOUT��
* @return ULONG  �������½��:
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룬��������Ĵ������ļ���
* @note szServerAdd��ȡֵΪ127.0.0.0,127.0.0.1,VM��������ַ
*       ulServerPort��ȡֵΪ8800
*/
IMOS_EXPORT ULONG STDCALL IMOS_InterInitiate
(
    IN    CHAR       szServerAdd[IMOS_IPADDR_LEN],
    IN    ULONG      ulServerPort,
    INOUT SDK_INIT_S *pstInit
);

/* added by z06806 for vodwall prj 20101211 begin */
/**
* ��ʼ�ط���ǽ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ
* @param [IN]   szXPCode                XP����
* @param [IN]   szMonitorCode           ����������
* @param [IN]   ulOperateCode           ����ԭ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E
*       szMonitorCode�ڻط���ǽ��Ϊ��Ӧ����������
*       szXPCode�ڻط���ǽ��Ϊ��Ӧ�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartVodToTV
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szXPCode[IMOS_CODE_LEN],
    IN  CHAR                        szMonitorCode[IMOS_CODE_LEN],
    IN  ULONG                       ulOperateCode
);

/**
* ֹͣ�ط���ǽ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ
* @param [IN]   ulCodeType              �������ͣ���VODWALL_STOP_CODETYPE_E
* @param [IN]   szXPCode                XP����
* @param [IN]   szMonitorCode           ����������
* @param [IN]   ulOperateCode           ����ԭ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E
*       �����������Ϊ����������szMonitorCode����������룬szXPCode���Բ���
*       �����������ΪXP����szXPCode��XP���룬szMonitorCode���Բ���
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopVodToTV
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulCodeType,
    IN  CHAR                            szXPCode[IMOS_CODE_LEN],
    IN  CHAR                            szMonitorCode[IMOS_CODE_LEN],
    IN  ULONG                           ulOperateCode
);
/* added by z06806 for vodwall prj 20101211 end */

/**
* ��ѯ��֯����Դ��Ϣ�б�(V3������RES_INFO_S�ṹ��������ԴID��������Ȩ��) \n
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                  ��֯����
* @param [IN]   pstQueryCondition          ��ѯ����
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstResList                 ��Դ��Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - ����
* - 1.��������ΪNULL,��ʾ��ָ����֯��������Դ(��������֯)
* - 2.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 3.֧�ְ�"��Դ����"ģ����ѯ:#NAME_TYPE
* - 4.֧�ְ�"�Ƿ�������Դ"��ѯ:#DOMAIN_TYPE = 1 ��;= 0 ��
* - 5.֧�ְ�"��Դ����"��ѯ:#RES_TYPE,ȡֵΪ#IMOS_TYPE_E
* - 6.֧�ְ�"��Դ������"��ѯ:#RES_SUB_TYPE(��Դ����Ϊ�����ʱ,ȡֵΪ#CAMERA_TYPE_E)
* - ����
* - 1.��ָ���������������,Ĭ�ϰ�"��Դ����"��������
* - 2.֧�ֵ���������,����:#NAME_TYPE(��Դ����),#CODE_TYPE(��Դ����),#RES_TYPE(��Դ����),#RES_SUB_TYPE(��Դ������),
* -   #DOMAIN_TYPE(�Ƿ�������Դ)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResourceListNoAuth
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT RES_INFO_S                      *pstResList
);

/**
* ���ݽ�ɫ���룬���ý�ɫȨ��(V2) \n
* @param [IN]   pstUserLogIDInfo      �û���¼ID��Ϣ��ʶ
* @param [IN]   szRoleCode            ��ɫ���룬��ӦΨһһ����ɫ
* @param [IN]   ulResAuthorityNum     ��Դ��Ȩ��Ϣ����
* @param [IN]   pstResAuthorityList   ��Դ��Ȩ��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_ROLE_NOT_EXIST
* - #ERR_MM_AUTHORITY_NOT_DEFINED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* -     1.�ýӿ����õ���Դ��Χ�����ڽ�ɫ������֯������֯
* -     2.�����Ȩ�޿�����ϵͳȨ�ޣ�Ҳ�����������Դ��������֯������ͷ���������ȣ���Ȩ��
* -     3.���øýӿں�, �Ըôε��õ�Ȩ��Ϊ׼����ɫ֮ǰ��Ȩ�޻ᱻ����
* -     4.ulResAuthorityNum����Ϊ0��ͬʱpstResAuthorityList����ΪNULL����ʾ�����ɫ����Ȩ�ޡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetRoleAuthorityV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szRoleCode[IMOS_CODE_LEN],
    IN  ULONG                       ulResAuthorityNum,
    IN  RES_AUTHORITY_S             *pstResAuthorityList
);

/**
* ���ݽ�ɫ���룬��ѯĳ����ɫ��Ȩ����Ϣ(V2) \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   szRoleCode                   ��ɫ���룬��ӦΨһһ����ɫ
* @param [IN]   pstQueryCondition            ��ѯ����(֧�ֵĲ�ѯ��������:#RES_ID[��ԴID]
*                                                                        #RES_TYPE[��Դ����]��
*                                                                        #NAME_TYPE[��Դ����])
* @param [IN]   pstQueryPageInfo             �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo               ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstResAuthorityList          ��ɫȨ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @attention
* - ȫ��Ȩ�޺�����Ȩ�ޱ���ֿ���ѯ��
* - ��ѯȫ��Ȩ��ʱ����ѯ����ΪRES_ID����0��������������ѯ��������ѯ����Ȩ��ʱ��RES_ID����0���ҿ�����������ѯ������
* - ���Ȩ�޼�¼��Ӧ����Դ�����ڣ����ν�ɫȨ���б��ж�Ӧ����Դ��RES_INFO_S��szResIdCode��Ч�������ֶζ���Ч��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByRoleV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szRoleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT RES_AUTHORITY_QUERY_ITEM_S      *pstResAuthorityList
);

/**
* ɾ����ɫ��һ����Ȩ��¼ \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   szRoleCode                   ��ɫ���룬��ӦΨһһ����ɫ
* @param [IN]   szResIdCode                  ��ԴID���ַ���"0"��ʾɾ����ɫ��ϵͳȨ��
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAuthItemByRole
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szRoleCode[IMOS_CODE_LEN],
    IN  CHAR                    szResIdCode[IMOS_CODE_LEN]
);

#if 0
#endif
/***************************************************************************
IPC��ѧ��������
****************************************************************************/
/**
* ���ù�ѧ����
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex             �豸ͨ��������Ϣ
* @param [IN]   pstOpticsParam              ��ѧ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_OPT_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigOpticsParam
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    IN  OPTICS_PARAM_S                  *pstOpticsParam
);

/**
* ��ѯ��ѧ����
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex             �豸ͨ��������Ϣ
* @param [OUT]  pstOpticsParam              ��ѧ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_OPT_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOpticsParam
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT OPTICS_PARAM_S                  *pstOpticsParam
);

/** @} */ /* end of BACKUP PLAN MANAGEMENT MODULE */
/** @} */

/**
* ͨ��XML�����豸ͨ�ýӿ�
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   ulXmlLen               �豸��XML������Ϣ
* @param [IN]   pcXml                  �豸��XML��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* - #ERR_AS_EC_NUM_EXCEEDED
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* - #ERR_DAO_DEVICE_CODE_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note XML��ʽ���"��������.xml"�ļ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDevByXML
(
    IN    USER_LOGIN_ID_INFO_S          *pstUserLogIDInfo,
    IN    ULONG                         ulXmlLen,
    IN    CHAR                          *pcXml
);

/**
* �����Զ�����
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartAutoFind
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo
);

/**
* ֹͣ�Զ�����
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopAutoFind
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo
);

/**
* ��ѯ�Զ�����״̬
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [OUT]  pulstate            �Զ�����״̬��1����������0����ֹͣ��ϵͳÿ������Ĭ��ֹͣ״̬
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAutoFindState
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulstate
);

#if 0
#endif
/**************************************************************************************************************
ECR33**-HF
***************************************************************************************************************/
/** @defgroup group ECR33**-HF
*   ����VM����ECR, ECR�豸����������
*   @{
*/
/***************************************************************************
PartI VM����ECR
****************************************************************************/
/**
* @PartI VM����ECR
* @{    VM����ECR
*/
/**
* ��Ӵ��洢�ı�����, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   pstECRBasicInfo    ���洢�ı�����������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* �޸Ĵ��洢�ı�����, ��֧��HFϵ��. \n
* Ŀǰ��֧���޸�����.
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   pstECRBasicInfo    ���洢�ı�����������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* ɾ�����洢�ı�����, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szECRCode           ���洢�ı���������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN]
);

/**
* ��ѯ���洢�ı�����, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szECRCode           ���洢�ı���������
* @param [OUT]  pstECRBasicInfo     ���洢�ı�����������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN],
    OUT ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* ��ѯĳ����֯�´��洢�ı������б���Ϣ, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯�����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[���洢�ı���������])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstECRBasicInfoList     ���洢�ı�����������Ϣ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ECR_BASIC_INFO_S            *pstECRBasicInfoList
);

/**
* ��ѯECR��������б�, ��֧��HFϵ��.
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szECRCode               ECR����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstECRCamList           ��ѯ����������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1. ����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
* - 2. ֧�ְ���������롢�������򣬶�Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:#CODE_TYPE��#NAME_TYPE;
* - 3. ���������������Ĭ�ϰ��������������������;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRCameraList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szECRCode[IMOS_ECR_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ECR_CAM_INFO_S              *pstECRCamList
);

/**
* ɾ��ECR�����, �Ὣ��ǰҵ��ֹͣ; ����ɾ��ECRʱ�Ĳ���; ��֧��HFϵ��.
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szCamCode               ��ɾ�������������
* @param [IN]   szECRCode               �������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCameraForECR
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCamCode[IMOS_CODE_LEN],
    IN  CHAR                        szECRCode[IMOS_ECR_CODE_LEN]
);
/** @} */ /* end of PartI��VM����ECR */

/***************************************************************************
PartII ECR�豸����
****************************************************************************/
/**
* @PartII ECR�豸����
* @{    ECR�豸����
*/
/** @} */ /* end of PartII:ECR�豸���� */


/** @} *//* end of group ECR33**-HF */

#if 0
#endif
/***************************************************************************
�澯����
****************************************************************************/
/**
* ���Ӹ澯���Ĺ��򣨷�ϵͳԤ�������澯Դ����Ϊ�գ�ϵͳԤ�������澯Դ����Ϊ�գ�
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstRule                     �澯���Ĺ���
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����������ϵͳԤ�������
*/
IMOS_EXPORT ULONG IMOS_AddAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN  ALARM_SENDING_RULE_S   *pstRule
);

/**
* ɾ���澯���Ĺ���ϵͳԤ���������ɾ����
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstRule                     �澯���Ĺ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_DelAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN  CHAR                   szRuleCode[IMOS_CODE_LEN]
);

/**
* �޸ĸ澯���Ĺ��򣨷�ϵͳԤ�������澯Դ����Ϊ�գ�ϵͳԤ�������澯Դ����Ϊ�գ�
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstRule                     �澯���Ĺ���
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_ModifyAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN  ALARM_SENDING_RULE_S  *pstRule
);

/**
* ��ѯ�澯���Ĺ��������Ϣ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szRuleCode                  �澯���Ĺ������
* @param [OUT]  pstRule                     �澯���Ĺ���
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szRuleCode[IMOS_CODE_LEN],
    OUT ALARM_SENDING_RULE_BASE_INFO_S  *pstRule
);

/**
* ��ѯ�澯���Ĺ���澯Դ�б����ÿ�β�ѯIMOS_ALARM_SEND_SRC_MAXNUM����¼��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szRuleCode                  �澯���Ĺ������
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ���ط�ҳ��Ϣ
* @param [OUT]  pstAlarmSrcList             �澯Դ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ֧�ֵĲ�ѯ������NAME_TYPE[�澯Դ����]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleAlarmSrcList
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szRuleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S  *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S         *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S           *pstRspPageInfo,
    OUT ALARM_SRC_QUERY_ITEM_S    *pstAlarmSrcList
);

/**
* ��ѯ�澯���Ĺ����û��б����ÿ�β�ѯIMOS_ALARM_SEND_USER_MAXNUM����¼��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szRuleCode                  �澯���Ĺ������
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ���ط�ҳ��Ϣ
* @param [OUT]  pstAlarmSrcList             �澯Դ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ֧�ֵĲ�ѯ������USER_NAME_TYPE[�û�����]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleUserList
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szRuleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S  *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S         *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S           *pstRspPageInfo,
    OUT USER_QUERY_ITEM_S         *pstUserList
);

/**
* ��ѯ�澯���Ĺ����б�
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szRuleCode                  �澯���Ĺ������
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ���ط�ҳ��Ϣ
* @param [OUT]  pstAlarmSrcList             �澯���Ĺ����б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ֧�ֵĲ�ѯ������NAME_TYPE[��������]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleList
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S         *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S                *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                  *pstRspPageInfo,
    OUT ALARM_SENDING_RULE_BASE_INFO_S   *pstAlarmSendingRuleList
);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

/**
* ͨ�ò�ѯ�ӿ�
* @param [IN]       pstUserLoginID      �û���¼ID��Ϣ��ʶ
* @param [IN]       ulQryFlag           ��ѯ��־λ#UNI_QUERY_FLAG_E
* @param [IN]       ulQryPieceNum       ��ѯPIECE��Ŀ
* @param [IN]       pulQryPiecelist     ��ѯPIECE�б�
* @param [IN]       pstSdkConditon      ��ѯԼ����Ϣ
* @param [IN][OUT]  pstSdkPageInfo      ��ѯ��ҳ��Ϣ
* @param [OUT]      oQryInfo            ��ѯ����PIECE�����б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_Query
(
    IN USER_LOGIN_ID_INFO_S        *pstUserLoginID,
    IN ULONG                       ulQryFlag,
    IN ULONG                       ulQryPieceNum,
    IN ULONG                       *pulQryPiecelist,
    IN UNI_CONDITION_S             *pstConditon,
    INOUT UNI_PAGE_S               *pstPageInfo,
    OUT std::vector<std::string>   &oQryInfo
);

#endif


