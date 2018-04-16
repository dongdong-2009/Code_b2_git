/*************************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------------
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
* ����MPɨ��洢�豸���� \n
* @param [IN]  pstUserLogIDInfo  �û���¼ID��Ϣ��ʶ
* @param [IN]  szStoreDevCode    �洢�豸����
* @param [IN]  ulStoreDevType    �洢�豸���ͣ�ö��ֵ: STORE_DEV_TYPE_E
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_DM_DEV_NOT_ONLINE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ScanStoreDev
(
    IN  USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
	IN  CHAR                           szStoreDevCode[IMOS_DEVICE_CODE_LEN],
	IN  ULONG                          ulStorageType
);


/**
* ��ѯ�û���ָ����Դ��Ȩ���б�. \n
* ����û���ϵͳ����Ա,��ֻ����"�Ƿ���ϵͳ����Ա"��ʶ,������Ȩ���б�;
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
* @note ����û�Ϊ�ǵ�½�û����û���¼ ID���û�IPΪ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ULONG                   ulResType,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    OUT BOOL_T                  *pbIsSystemMgr,
    OUT ORG_AUTHORITY_V2_S         *pstAuthorityInfo
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
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL)
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstCamShareInfo    �����������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.����ͨ�ò�ѯ����:
*     #NAME_TYPE[���������]
*     #IS_QUERY_SHARED[��ѯ�ѹ����δ����] = 0 δ����; = 1 �ѹ���
*   2.�����ѯ������������й�����ˣ�����а�����֯����������������ѯ�����������(δ���� + �ѹ���)
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
* @param [IN]   ulUAType                �豸����,ȡֵ��Χ��#IMOS_TYPE_E.ע:��ѯ�����ع�ϵʱ,�Ϸ���ȡֵΪIMOS_TYPE_XP(XP)
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
* @param [OUT]  pstAlarmSrcExList      �澯Դ��Ϣ�б�
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
    OUT ALARM_SRC_QUERY_ITEM_EX_S   *pstAlarmSrcExList
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
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#USER_NAME_TYPE[ȷ���û�����];
* -                                 #NAME_TYPE[�澯Դ����]; #EVENT_TYPE[�澯�¼�����]; #EVENT_SECURITY[�澯�¼�����];
* -                                 #EVENT_STATUS_TYPE[�澯״̬]; #EVENT_TIME[�澯ʱ��]; #IS_PLAN_ALARM[�Ƿ�澯Ԥ��];
* -                                 #IS_ONCE_PLAN_ALARM[�Ƿ�������Ԥ��]; #IS_MISINFORM[�Ƿ���] #CODE_TYPE[�澯Դ����];
* -                                 #CHK_USER_NAME_TYPE[�˾��û�����];)
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
* @param [IN]   pstQueryPageInfo   �����ҳ��
* @param [IN]   ULONG �����������ͣ�0:���У�1:��Ҫ������2:����Ҫ������
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
    IN  ULONG                                 ulAdviceType,
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
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[����]; #CODE_TYPE[����]; #PHY_DEV_IP[IP��ַ])
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
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
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
* @param [IN]   ulShareAll              ����ʽ: 0Ϊ��������֯, 1Ϊ��ͬ���е�������Դһ������
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
    IN  ULONG                   ulShareAll,
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
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareOrgList         ��ѯ������֯�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"��֯����"ģ����ѯ������:#NAME_TYPE;
* - 2.֧�ְ�"��֯����"ģ����ѯ������:#CODE_TYPE;
* - 3.���������������Ĭ�ϰ�����֯������������;
* - 4.����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareOrgList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_ORG_INFO_S            *pstShareOrgList
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
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL)
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstAlmSrcShareList �澯Դ������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* - 1.����ͨ�ò�ѯ����:
*     #NAME_TYPE[�澯Դ����]
*     #IS_QUERY_SHARED[��ѯ�ѹ����δ����] = 0 δ����; = 1 �ѹ���
*   2.�����ѯ������������й�����ˣ�����а�����֯����������������ѯ�����ĸ澯Դ(δ���� + �ѹ���)
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
* ��ѯ���������/������ĸ澯Դ�б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulQueryFlag             ��ѯ��ǣ�0��ʾ�鱾���������ĸ澯Դ��1��ʾ�������������ĸ澯Դ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareAlmSrcList      ��ѯ�����澯Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"�澯Դ����"ģ����ѯ������:#NAME_TYPE;
* - 2.֧�ְ�"�澯Դ����"ģ����ѯ������:#CODE_TYPE;
* - 3.���������������Ĭ�ϰ��ո澯Դ������������;
* - 4.����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_ALARMSRC_INFO_S       *pstShareAlmSrcList
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
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL)
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstMapShareList    ��ͼ������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.����ͨ�ò�ѯ����:
*     #NAME_TYPE[��ͼ����]
*     #IS_QUERY_SHARED[��ѯ�ѹ����δ����] = 0 δ����; = 1 �ѹ���
*   2.�����ѯ������������й�����ˣ�����а�����֯����������������ѯ�����ĵ�ͼ(δ���� + �ѹ���)
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
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareMapList         ��ѯ������ͼ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"��ͼ����"ģ����ѯ������:#NAME_TYPE;
* - 2.֧�ְ�"��ͼ����"ģ����ѯ������:#CODE_TYPE;
* - 3.���������������Ĭ�ϰ��յ�ͼ������������;
* - 4.����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareMapList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_MAP_INFO_S            *pstShareMapList
);

/***************************************************************************
��Դ�����ϵ��ѯ
****************************************************************************/
/**
* ��ѯĳ����Դ�ϴ��ڵĹ����ϵ \n
* @param [IN]   pstUserLoginIDInfo       �û���¼ID��Ϣ
* @param [IN]   pstResQueryInfo          ��Դ��ѯ������Ϣ(���������)
* @param [IN]   pstQueryCondition        ͨ�ò�ѯ����(����ΪNULL)
* @param [IN]   pstQueryPageInfo         �����ҳ��Ϣ
* @param [OUT]  pstResShareInRecord      ��Դ�������¼(IN)
* @param [OUT]  pstRspPageInfo           ���ط�ҳ��Ϣ
* @param [OUT]  pstResShareOutRecList    ��Դ�����¼�б�(OUT)
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* 1.���Ǳ�����Դ,��Դ�������¼(IN)����д�������ƺͱ���
* 2.pstQueryCondition��#QUERY_CONDITION_ITEM_S�Ĳ�ѯ������ö��#QUERY_TYPE_E,
*   Ŀǰ֧�ֵ�ֻ��#CODE_TYPE��#NAME_TYPE��#EXT_DOMAIN_TPYE
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResShareRelation
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLoginIDInfo,
    IN  QUERY_RES_REQUEST_INFO_S            *pstResQueryInfo,
    IN  COMMON_QUERY_CONDITION_S            *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S                   *pstQueryPageInfo,
    OUT RES_SHARE_IN_RECORD_INFO_S          *pstResShareInRecord,
    OUT RSP_PAGE_INFO_S                     *pstRspPageInfo,
    OUT RES_SHARE_OUT_RECORD_INFO_S         *pstResShareOutRecList
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
* @param [IN]   szExtInfo               ��չ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰֻ֧���������/�������������
* - ����Ϊ"ͼ��ƴ��"ʱ,��չ��ϢΪ"�궨����Ϣ"
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyResGroup
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szResGroupCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szGroupName[IMOS_NAME_LEN],
    IN  ULONG                       ulGroupResType,
    IN  ULONG                       ulResNum,
    IN  RESOURCE_GROUP_UNIT_S       *pstResGroupUnit,
    IN  CHAR                        szExtInfo[IMOS_RES_GROUP_EXT_LEN]
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
* @param [OUT]  szOrgCode               ��֯����
* @param [OUT]  szGroupName             ��Դ������
* @param [OUT]  pulResNum               ������Դ��Ŀ
* @param [OUT]  pstResGroupUnit         ������Դ��Ԫ��Ϣ
* @param [OUT]  szExtInfo               ��չ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����Ϊ"ͼ��ƴ��"ʱ,��չ��ϢΪ"�궨����Ϣ"
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResGroup
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szResGroupCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulGroupResType,
    OUT CHAR                            szOrgCode[IMOS_CODE_LEN],
    OUT CHAR                            szGroupName[IMOS_NAME_LEN],
    OUT ULONG                           *pulResNum,
    OUT RESOURCE_GROUP_UNIT_S           *pstResGroupUnit,
    OUT CHAR                            szExtInfo[IMOS_RES_GROUP_EXT_LEN]
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

#if 0
#endif

/**
* ����������ԴV2
* @param [IN]  pstUserLogIDInfo �û���¼ID��Ϣ��ʶ
* @param [IN]  szOrgCode        ��֯����
* @param [IN]  pstSwitchInfo    ������Դ��Ϣ
* @param [OUT] szSwitchResCode  ������Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_GUARDTOUR_TIMESLOT_EXCEEDED
* - ����,�μ�����������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddSwitchResourceV2
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                  szOrgCode[IMOS_CODE_LEN],
    IN  SWITCH_RESOURE_V2_S  *pstSwitchInfo,
    OUT CHAR                  szSwitchResCode[IMOS_CODE_LEN]
);

/**
* �޸�������ԴV2
* @param [IN] pstUserLogIDInfo �û���¼ID��Ϣ��ʶ
* @param [IN] szSwitchResCode  ������Դ����
* @param [IN] pstSwitchInfo    ������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_GUARDTOUR_TIMESLOT_EXCEEDED
* - #ERR_DAO_SWITCH_ID_NOT_EXIST
* - ����,�μ�����������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySwitchResourceV2
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                  szSwitchResCode[IMOS_CODE_LEN],
    IN  SWITCH_RESOURE_V2_S  *pstSwitchInfo
);

/**
* ��ѯ������ԴV2
* @param [IN]  pstUserLogIDInfo �û���¼ID��Ϣ��ʶ
* @param [IN]  szSwitchResCode  ������Դ����
* @param [OUT] pstSwitchInfo    ������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SWITCH_ID_NOT_EXIST
* - ����,�μ�����������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySwitchResourceV2
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                  szSwitchResCode[IMOS_CODE_LEN],
    OUT SWITCH_RESOURE_V2_S  *pstSwitchInfo
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
    IN    EVENT_TYPE_S                    *pstEventTypeInfo
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
    OUT  EVENT_TYPE_S                     *pstEventTypeInfo
);

/**
* ��ѯ�¼������б� \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   ulDevTypeId        �豸����ID,ȡֵ��Χ��IMOS_TYPE_E
* @param [IN]   ulDevSubTypeId     �豸��Ϣ������ID,���������ȡֵ��CAMERA_TYPE_E,��֯����ȡֵ��Χ��mm_def.h��6��MM_DOMAIN_SUBTYPE_XXX_XXX�ĺ궨��,
*                                      ����ʾ����ȡֵ��SALVO_TYPE_E,����Ѳ����ȡֵ��GROUP_SWITCH_TYPE_E,EC��DC����ȡֵ��IMOS_DEVICE_TYPE_E
* @param [IN]   szDevCode          �豸����
* @param [IN]   ulParentDevType    ���豸����ID,ȡֵ��Χ��IMOS_TYPE_E
* @param [IN]   ulParentSubDevType ���豸��Ϣ������ID,���������ȡֵ��CAMERA_TYPE_E,��֯����ȡֵ��Χ��mm_def.h��6��MM_DOMAIN_SUBTYPE_XXX_XXX�ĺ궨��,
*                                      ����ʾ����ȡֵ��SALVO_TYPE_E,����Ѳ����ȡֵ��GROUP_SWITCH_TYPE_E,EC��DC����ȡֵ��IMOS_DEVICE_TYPE_E
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ������Ŀǰֻ֧�������Ͳ�����ֻ֧�ֵ��ڣ�
* @param [IN]   pstQueryPageInfo   ��ѯ��ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ��ѯ���ط�ҳ��Ϣ
* @param [OUT]  pstEventTypeExtInfoList  �¼�������Ϣ�б�
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
    IN  CHAR                            szDevCode[IMOS_CODE_LEN],
    IN  ULONG                           ulParentDevType,
    IN  ULONG                           ulParentSubDevType,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_TYPE_EXT_INFO_S           *pstEventTypeExtInfoList
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
* ����ϵͳʵ����������������Ӧƥ�����
* @param [in] pstUserLogIDInfo          �û���¼ID��Ϣ��ʶ
* @param [in] ulMonStreamMatchPolicy    ϵͳʵ����������������Ӧƥ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigSysMonStreamMatchPolicy
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulMonStreamMatchPolicy
);

/**
* ��ѯϵͳʵ����������������Ӧƥ�����
* @param [IN]  pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [OUT] pulMonStreamMatchPolicy    ϵͳʵ����������������Ӧƥ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySysMonStreamMatchPolicy
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulMonStreamMatchPolicy
);

/**
* ����ϵͳ�鲥��ַ��������ѡ�����
* @param [in] pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [in] ulMCastAddrStreamPolicy    ϵͳ�鲥��ַ��������ѡ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigSysMCastAddrStreamPolicy
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulMCastAddrStreamPolicy
);

/**
* ��ѯϵͳ�鲥��ַ��������ѡ�����
* @param [IN]  pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [OUT] pulMCastAddrStreamPolicy    ϵͳ�鲥��ַ��������ѡ�����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySysMCastAddrStreamPolicy
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulMCastAddrStreamPolicy
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
* @note
* - 1.����޲�ѯ����,ָ��pstQueryCondition����ΪNULL
* - ����
* - 1.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB,1 ��,0 ��
* - 2.���ڸշ�����Ĵ洢��Դ���ӿڷ�����Դ״̬Ϊ����������Ҫ��ʱһ��ʱ��(������)����ִ��¼��洢
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

/**
* ��ѯ��ĳ���洢�豸(Ŀǰֻ֧��IPSAN��VX500)��������ֱ���ת���������б�  \n
* @param [IN]   *pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   szStorDevCode           �洢�豸����
* @param [IN]   *pstQueryPageInfo       �����ҳ��Ϣ
* @param [OUT]  *pstRspPageInfo         ��Ӧ��ҳ��Ϣ
* @param [OUT]  *pstSDKCamList          ������б�
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* 1.����ʽΪ"�����������������"
* 2.���ӿ���ֱ��ѯ���ű�,�ݲ�֧��ͨ�ò�ѯ����
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamListByStoreDev
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szStoreDevCode[IMOS_CODE_LEN],
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT CAM_STORE_BASIC_QUERY_ITEM_S    *pstSDKCamList
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
* @param [IN]   pstQueryCondition      ͨ�ò�ѯ����(����NULL;��������:NAME_TYPE[BM����] CODE_TYPE[BM����] PHY_DEV_IP[BM IP])
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
* �����������ת������ \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   CAM_STREAM_STOR_INFO_S  *pstCamStreamStorInfo   �������ת��������Ϣ
* @return ULONG �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignCamStreamStorInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_STREAM_STOR_INFO_S  *pstCamStreamStorInfo
);

/**
* �޸��������ת��������� \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   CAM_STREAM_STOR_INFO_S  *pstCamStreamStorInfo   �������ת��������Ϣ
* @return ULONG �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note: ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCamStreamStorInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_STREAM_STOR_INFO_S  *pstCamStreamStorInfo
);

/**
* ɾ���������ת������ \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo                  �û���¼ID��Ϣ��ʶ
* @param [IN]   CHAR                    szCamCode[IMOS_CODE_LEN]           ���������
* @return ULONG �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCamStreamStorInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCamCode[IMOS_CODE_LEN]
);

/**
* ��ѯ�������ת��������Ϣ \n
* @param [IN]   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   CHAR                        szCamCode[IMOS_CODE_LEN]    ���������
* @param [OUT]  CAM_STREAM_STOR_INFO_S      *pstCamStreamStorInfo       �������ת����Ϣ�б�
* @return ULONG �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamStreamStorInfo
(
    IN  USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN  CHAR                     szCamCode[IMOS_CODE_LEN],
    OUT CAM_STREAM_STOR_INFO_S   *pstCamStreamStorInfo
);

/**
* ��ҳ��ѯĳ����֯���������ת��������Ϣ�б� \n
* ����Ϊ��ʱ,������,��Ĭ��"������������"
* @param [IN]  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]  CHAR                         szOrgCode[IMOS_CODE_LEN]    ��֯����
* @param [IN]  COMMON_QUERY_CONDITION_S     *pstQueryCondition          ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[���������])
* @param [IN]  QUERY_PAGE_INFO_S            *pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT] RSP_PAGE_INFO_S              *pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT] CAM_STREAM_STOR_QUERY_ITEM_S *pstCamStreamStorList       �����������Դ������Ϣ�б�
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamStreamStorInfoList
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  CHAR                             szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S         *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S                *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                  *pstRspPageInfo,
    OUT CAM_STREAM_STOR_QUERY_ITEM_S     *pstCamStreamStorList
);

/**
* ����������ת��ļƻ����� \n
* @param [IN]   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo                       �û���¼ID��Ϣ��ʶ
* @param [IN]   STREAM_STOR_PLAN_INFO_S     *pstStreamStorPlanInfo                  ��ת��ƻ���Ϣ
* @param [OUT]  CHAR                        szStreamStorPlanCode[IMOS_CODE_LEN]     ���ص���ת��ƻ�����
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddCamStreamStorPlan
(
    IN  USER_LOGIN_ID_INFO_S                 *pstUserLogIDInfo,
    IN  STREAM_STOR_PLAN_INFO_S              *pstStreamStorPlanInfo,
    OUT CHAR                                 szStreamStorPlanCode[IMOS_CODE_LEN]
);

/**
* �޸���ת��ƻ�
* @param [IN]   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]  STREAM_STOR_PLAN_INFO_S      *pstStreamStorPlanInfo  ��ת��ƻ���Ϣ
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModCamStreamStorPlan
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  STREAM_STOR_PLAN_INFO_S    *pstStreamStorPlanInfo
);

/**
* ɾ����ת��ƻ�
* @param [IN]   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo                         �û���¼ID��Ϣ��ʶ
* @param [IN]   CHAR                       szStreamStorPlanCode[IMOS_CODE_LEN]       ��ת��ƻ�����
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCamStreamStorPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szStreamStorPlanCode[IMOS_CODE_LEN]
);

/**
* ��ѯ��ת��ƻ���Ϣ \n
* @param [IN]   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo                         �û���¼ID��Ϣ��ʶ
* @param [IN]   CHAR                       szStreamStorPlanCode[IMOS_CODE_LEN]       ��ת��ƻ�����
* @param [OUT]  STREAM_STOR_PLAN_INFO_S    *pstStreamStorPlanInfo                    ��ת��ƻ���Ϣ
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamStreamStorPlan
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  CHAR                       szStreamStorPlanCode[IMOS_CODE_LEN],
    OUT STREAM_STOR_PLAN_INFO_S    *pstStreamStorPlanInfo
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
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
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
* ��ѯ��ǰ��¼�û�������Ȩ��(V2). \n
* �����ǰ��¼�û���ϵͳ����Ա,��ֻ����"�Ƿ���ϵͳ����Ա"��ʶ,������Ȩ���б�;
* �������ϵͳ����Ա,�򷵻�Ȩ���б�(����ϵͳȨ�޺���ԴȨ��).
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [OUT]  pbIsSystemMgr       "�Ƿ���ϵͳ����Ա"��ʶ, BOOL_TRUEΪϵͳ����Ա; BOOL_FALSEΪ��ϵͳ����Ա
* @param [OUT]  pstAuthorityInfo    Ȩ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_MM_AUTHORITY_NOT_DEFINED
* -     ���������,�μ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByUserV2
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    OUT BOOL_T                  *pbIsSystemMgr,
    OUT ORG_AUTHORITY_V2_S         *pstAuthorityInfo
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

/**
* ����ϵͳҵ����ռ����
* @param [IN] pstUserLogIDInfo      �û���¼��Ϣ
* @param [IN] pstSrvRevStrategy     ҵ����ռ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز��������������˵���ĵ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetReaveStrategy
(
    IN  USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN  SRV_REV_STRATEGY_S     *pstSrvRevStrategy
);

/**
* ��ѯϵͳҵ����ռ����
* @param [IN]  pstUserLogIDInfo       �û���¼��Ϣ
* @param [OUT] pstSrvRevStrategy      ҵ����ռ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - ���ز��������������˵���ĵ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryReaveStrategy
(
    IN  USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    OUT SRV_REV_STRATEGY_S       *pstSrvRevStrategy
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
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;֧�ֵĲ�ѯ��������:#NAME_TYPE[���洢�ı���������] #CODE_TYPE[���洢�ı���������] #EXT_DOMAIN_IP[���洢�ı�������ַ])
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
* @param [IN]   szECRCode               ECR����
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

/**
* ��ѯECR�ĸ澯Դ�б�, ����ɾ��ECRʱ�Ĳ���; ��֧��HFϵ��.
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szECRCode               ECR����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstECRAlmSrcList        ��ѯ�����澯Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRAlarmSrcList
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN],
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT ECR_ALARMSRC_INFO_S     *pstECRAlmSrcList
);

/**
* ɾ��ECR�澯Դ, ����ɾ��ECRʱ�Ĳ���; ��֧��HFϵ��.
* @param [IN]   pstUserLogIDInfo    �û���Ϣ��ʶ
* @param [IN]   szAlarmSrcCode      ��ɾ���ĸ澯Դ����
* @param [IN]   szECRCode           ECR����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmSrcForECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szAlarmSrcCode[IMOS_CODE_LEN],
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN]
);
/** @} */ /* end of PartI��VM����ECR */

#if 0
#endif

/***************************************************************************
PartII ECR�豸����
****************************************************************************/
/**
* @PartII ECR�豸����
* @{    ECR�豸����
/**
* �޸�ϵͳʱ��. \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo       �û���Ϣ
* @param [IN]   CHAR                    szTime[IMOS_TIME_LEN]   �޸ĺ��ʱ�䣬��ʽΪ"YYYY-MM-DD HH:MM:SS"
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* - #ERR_AS_GET_TIME_FAIL
* - #ERR_AS_SET_TIME_FAIL
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySystemTime
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  CHAR                         szTime[IMOS_TIME_LEN]
);

/**
* ��ȡϵͳʱ��. \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo       �û���Ϣ
* @param [OUT]  CHAR                    szTime[IMOS_TIME_LEN]   ϵͳʱ�䣬��ʽΪ"YYYY-MM-DD HH:MM:SS"
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* - #ERR_AS_GET_TIME_FAIL
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetSystemTime
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    OUT CHAR                         szTime[IMOS_TIME_LEN]
);

/**
* ���洢�ı��������������ڴ��洢�ı���������. \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo   �û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_RebootECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo
);

/**
* ���洢�ı������ػ������ڴ��洢�ı���������. \n
* @param [IN]   USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo   �û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShutdownECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo
);

/* begin added by gaoshufeng-gKF0050 for VVD65569,2012.05.08 */
/*begin added by panchen-pKF0016 for ���������ʱ��������,2012.02.02*/
/**
* �����豸�����������( ��ʱ��)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [IN]   SemaOutputInfo          ��������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfOutSwitchWithTimeForECR
(
    IN  USER_LOGIN_ID_INFO_S              *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S               *pstChannelIndex,
    IN  SEMA_OUTPUT_INFO_WITHTIME_FOR_ECR_S       *pstSemaOutputInfoWithTime
);
/*end added by panchen-pKF0016 for ���������ʱ��������,2012.02.02*/

/*begin added by panchen-pKF0016 for ���������ʱ��������,2012.02.02*/
/**
* ��ѯ�����������Ϣ (��ʱ��)
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [OUT]  SemaInputInfo           �����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SEMAPHORE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOutSwitchWithTimeForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT SEMA_OUTPUT_INFO_WITHTIME_FOR_ECR_S     *pstSemaOutputInfo
);
/*end added by panchen-pKF0016 for ���������ʱ��������,2012.02.02*/
/* end added by gaoshufeng-gKF0050 for VVD65569,2012.05.08 */

/**
* ���洢�ı������ָ�Ĭ������, ��Mware��CAPI�ӿ����ʹ��. \n
* ֻ����������ص����ú��豸��ͨ�����룬�����洢, ����ָ�ȱʡֵ;
* ���ڴ��洢�ı��������������øýӿں�Ὣ����ϵͳ����.
* @param [IN]  USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo   �û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_RestoreECRDefaultConfig
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo
);

/**
* ECR�����������豸������Ϣ, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   pstDeviceBasicInfo �豸������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note
* 1. �޸����ײ�����ʽ��ҪSDK�û����������豸�Ľӿں��豸������������Ч.
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDevBasicInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ECR_DEVICE_BASIC_INFO_S *pstDeviceBasicInfo
);

/**
* ECR�����ϲ�ѯ�豸������Ϣ, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [OUT]  pstDeviceBasicInfo �豸������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevBasicInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    OUT	ECR_DEVICE_BASIC_INFO_S *pstDeviceBasicInfo
);

/**
* ECR���������÷�����������Ϣ, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo  �û���Ϣ, ֻ��ӵ��ϵͳ����Ȩ�޵��û����Խ��в���
* @param [IN]   pstServerMgrInfo  ������������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note
* 1. ����������ģʽ���л�������ģʽ�����޸ķ�������Ϣ��ֹͣԭ���������ڽ��е�ʵʱҵ��.
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetServerMgrInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ECR_SERVER_MGR_INFO_S       *pstServerMgrInfo
);

/**
* ECR�����ϲ�ѯ������������Ϣ, ��֧��HFϵ��. \n
* @param [IN]   pstUserLogIDInfo  �û���Ϣ, ��Ȩ�޹���, ����Ȩ����SDK�û�����
* @param [OUT]  pstServerMgrInfo  ������������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServerMgrInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    OUT ECR_SERVER_MGR_INFO_S       *pstServerMgrInfo
);

/**
* ���Ӹ澯����(for ECR&NVR buzzer)
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
IMOS_EXPORT ULONG STDCALL IMOS_AddAlarmLinkageForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_FOR_ECR_S          *pstAlarmAction
);


/**
* ɾ���澯����(for ECR&NVR buzzer)
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
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmLinkageForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType
);


/**
* �޸ĸ澯������Ϣ(for ECR&NVR buzzer)
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
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmLinkageForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_FOR_ECR_S          *pstAlarmAction
);


/**
* ��ѯ�澯������Ϣ(for ECR&NVR buzzer)
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
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmLinkageForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    OUT ALARM_ACTION_FOR_ECR_S          *pstAlarmAction
);

/**
* ��������󶨵���������ĳ��ͨ���� for ECR&NVR
* @param [IN]  pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]  pstVinChnlAndCamInfo         ����������ͨ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* - #ERR_DAO_DEVICE_CODE_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.��������벻��Ϊ��,�Ҳ��ܰ����ո�
*/
IMOS_EXPORT ULONG STDCALL IMOS_BindCameraToVideoInChannelForECR
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  VINCHNL_BIND_CAMERA_FOR_ECR_S   *pstVinChnlAndCamInfo
);
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

/**
* ���ö�λ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [IN]   pstLocateInfo           ��λ��Ϣ
* @return ULONG �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - ����,�μ�������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigLocateInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    IN  LOCATE_INFO_S               *pstLocateInfo
);

/**
* ��ѯ��λ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [OUT]  pstLocateInfo           ��λ��Ϣ
* @return ULONG �������½��:
* - �ɹ�:
* - #ERR_COMMON_SUCCEED
* - ʧ��:
* - ����,�μ�������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryLocateInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    OUT LOCATE_INFO_S               *pstLocateInfo
);

#if 0
#endif

/***************************************************************************
���ó���
****************************************************************************/
/**
* ���ӳ�����Դ
* @param [IN]     pstUserLogIDInfo        �û���Ϣ
* @param [IN]     szOrgCode               ��֯����
* @param [INOUT]  pstSceneInfo            ������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ������Դ��Ϣ�г�����Դ�������Ϊ��;����ε�����ʾ����Ϊ��, ��Я�����ڲ����ɵ�����ʾ����)
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddScene
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN      CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    INOUT   SCENE_INFO_S                *pstSceneInfo
);

/**
* �޸ĳ�����Դ
* @param [IN]     pstUserLogIDInfo        �û���Ϣ
* @param [IN]     szOrgCode               ��֯����
* @param [INOUT]  pstSceneInfo            ������Դ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyScene
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN      CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    INOUT   SCENE_INFO_S                *pstSceneInfo
);

/**
* ɾ��������Դ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSceneCode             ����ʾ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelScene
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szSceneCode[IMOS_CODE_LEN]
);


/**
* ��ѯ����ʾ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szSceneCode             ����ʾ����
* @param [OUT]  pstSceneInfo            ����ʾ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryScene
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSceneCode[IMOS_CODE_LEN],
    OUT SCENE_INFO_S                    *pstSceneInfo
);

/**
* ��ҳ��ѯĳ����֯�µ�����ʾ�б�  \n
* ����Ϊ��ʱ,��ѯ��֯����������ʾ�б�,��Ĭ�ϰ���"����ʾ��������"����.
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* - ����NULL
* - ��������:�ݲ�ʵ��
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstSceneList            ����ʾ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySceneList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SCENE_QUERY_ITEM_S          *pstSceneList
);

/***************************************************************************
������
****************************************************************************/
/**
* @name �����������
* @{    �����������
*/
/**
* �������������Թ���Ҳ����ȡ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulShareFlag             �����־: 0Ϊȡ������1Ϊ����
* @param [IN]   ulShareSceneNum         �����ĸ���
* @param [IN]   pstShareSceneList       ����/ȡ�������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����һ�ι���Ҫ����8������
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareScene
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareSceneNum,
    IN  SHARE_SCENE_BASE_S      *pstShareSceneList
);

/**
* ��ѯ��֯����ĳ���������Ϣ�б�ֻ��ȡһ�� \n
* �����ѯ����ʱ,������,��Ĭ������(������������)
* @param [IN]   pstUserLogIDInfo   �û���Ϣ
* @param [IN]   szOrgCode          ��֯�ڵ����
* @param [IN    szExDomainCode     �������
* @param [IN]   pstQueryCondition  ͨ�ò�ѯ����(����NULL)
* @param [IN]   pstQueryPageInfo   �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo     ���ط�ҳ��Ϣ
* @param [OUT]  pstSceneShareList  ����������Ϣ�б�ָ��
* @return ULONG �������½����
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.����ͨ�ò�ѯ����:
*     #NAME_TYPE[��������]
*     #IS_QUERY_SHARED[��ѯ�ѹ����δ����] = 0 δ����; = 1 �ѹ���
*   2.�����ѯ������������й�����ˣ�����а�����֯����������������ѯ�����ĳ���(δ���� + �ѹ���)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySceneShareInfoList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_SCENE_INFO_S              *pstSceneShareList
);

/**
* ��ѯ���������/������ĳ����б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szExDomainCode          �������
* @param [IN]   ulQueryFlag             ��ѯ��ǣ�0��ʾ�鱾���������ĳ�����1��ʾ�������������ĳ���
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstShareSceneList       ��ѯ���������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"��������"ģ����ѯ������:#NAME_TYPE;
* - 2.֧�ְ�"��������"ģ����ѯ������:#CODE_TYPE;
* - 3.���������������Ĭ�ϰ��ճ���������������;
* - 4.����޲�ѯ������ָ��pstQueryCondition����ΪNULL;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareSceneList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_SCENE_INFO_S          *pstShareSceneList
);

/**
* �����ʼ����͵����� \n
* @param [IN]    pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]    pstEmailSendingConfig        Email������ص�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyEmailSendingConfig
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  EMAIL_SENDING_CONFIG_S       *pstEmailSendingConfig
);

/**
* ��ȡ�ʼ����͵����� \n
* @param [IN]    pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [OUT]   pstEmailSendingConfig        Email������ص�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEmailSendingConfig
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    OUT EMAIL_SENDING_CONFIG_S       *pstEmailSendingConfig
);

/**
* ���Ͳ����ʼ��� ��֤�ʼ����͵������Ƿ���ȷ \n
* @param [IN]    pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]    pstEmailSendingConfig        Email������ص�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendTestingEmail
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  EMAIL_SENDING_CONFIG_S       *pstEmailSendingConfig
);

/**
* ���ö���ҵ����������  \n
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       pstSMSSendingConfig     ����ҵ������������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySMSSendingConfig
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SMS_SENDING_CONFIG_S            *pstSMSSendingConfig
);

/**
* ��ȡ����ҵ��������Ϣ����  \n
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [OUT]      pstSMSSendingConfig     ����ҵ������������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySMSSendingConfig
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT SMS_SENDING_CONFIG_S            *pstSMSSendingConfig
);

/**
* ���Է��Ͷ��ţ���֤���ŷ��͵������Ƿ���ȷ \n
* @param [IN]    pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]    pstSMSTestSendingConfig      ���Ų��Է�������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendTestingSMS
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  SMS_TEST_SENDING_CONFIG_S    *pstSMSTestSendingConfig
);

#if 0
#endif

/***************************************************************************
���мƻ�V2�汾
****************************************************************************/

/**
* �������мƻ�V2�汾
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstSwitchPlanInfo       ���мƻ���Ϣ
* @param [OUT]  szSwitchPlanCode        ���мƻ�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddSwitchPlanV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SWITCH_PLAN_INFO_V2_S           *pstSwitchPlanInfo,
    OUT CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);

/**
* �޸����мƻ�V2�汾
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSwitchPlanCode        ���мƻ�����
* @param [IN]   pstSwitchPlanInfo       ���мƻ���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySwitchPlanV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    IN  SWITCH_PLAN_INFO_V2_S           *pstSwitchPlanInfo
);

/**
* ɾ�����мƻ�V2�汾
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSwitchPlanCode        ���мƻ�����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelSwitchPlanV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);

/**
* ��ѯ���мƻ�V2�汾
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szSwitchPlanCode        ���мƻ�����
* @param [OUT]  pstSwitchPlanInfo       ���мƻ���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySwitchPlanV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    OUT SWITCH_PLAN_INFO_V2_S              *pstSwitchPlanInfo
);

/**
* ��ѯ���мƻ��б�V2�汾�����Ը��������飬Ҳ���Բ����еġ�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstSwitchPlanList       ���мƻ��б���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.����޲�ѯ����,ָ��pstQueryCondition����ΪNULL
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySwitchPlanListV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SWITCH_PLAN_QUERY_ITEM_S    *pstSwitchPlanList
);

/**
* ������Ϊ������Ϣ
* @param [IN]  pstUserLogIDInfo     �û���¼ID��Ϣ��ʶ
* @param [IN]  pstChannelIndex      �豸ͨ��������Ϣ
* @param [IN]  pstBehaviorAnalyse   ��Ϊ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigBehaviorAnalyse
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    IN  IMOS_BEHAVIOR_ANALYSE_INFO_S    *pstBehaviorAnalyse
);

/**
* ��ѯ��Ϊ������Ϣ
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex     �豸ͨ��������Ϣ
* @param [OUT]  pstBehaviorAnalyse  ��Ϊ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBehaviorAnalyse
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT IMOS_BEHAVIOR_ANALYSE_INFO_S    *pstBehaviorAnalyse
);

#if 0
#endif

/*************************************************************************************
�ʲ���Ϣ���ã��������ӣ��޸ģ�ɾ����
**************************************************************************************/
/**
* �����ʲ�
* @param [IN]     pstUserLogIDInfo        �û���Ϣ
* @param [IN]     szOrgCode               �ʲ�������֯����
* @param [IN]     pstAssetInfo            �ʲ���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddAsset
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN      CHAR                        szOrgCode[IMOS_CODE_LEN],
    INOUT   ASSET_INFO_S                *pstAssetInfo
);

/**
* �޸��ʲ�
* @param [IN]   pstUserLogIDInfo       �û���Ϣ
* @param [IN]   pstAssetInfo           �ʲ���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAsset
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   ASSET_INFO_S               *pstAssetInfo
);


/**
* ɾ���ʲ�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szAssetCode             �ʲ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAsset
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szAssetCode[IMOS_CODE_LEN]
);

/**
* ��ѯ�ʲ�
* @param [IN]   pstUserLogIDInfo     �û���Ϣ
* @param [IN]   ulQueryType          ��ѯ��ʽ��ȡֵ#ASSET_QUERY_TYPE_E
* @param [IN]   szQryCode            ��ѯ����
* @param [OUT]  pstAssetList         �ʲ���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ֧�ְ��豸����/�ʲ���Ų�ѯ�ʲ���Ϣ
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAsset
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   ULONG                      ulQueryType,
    IN   CHAR                       szQryCode[IMOS_CODE_LEN],
    OUT  ASSET_INFO_S               *pstAssetInfo
);

/**
* ��ѯ��֯���ʲ���Ϣ�б�
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                  ��֯����
* @param [IN]   pstQueryCondition          ��ѯ����
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstAssetList               �ʲ���Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - ����
* - 1.��������ΪNULL,��ʾ��ָ����֯�������ʲ�(��������֯)
* - 2.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 3.֧�ְ�"�ʲ�����"ģ����ѯ:#NAME_TYPE
* - 3.֧�ְ��ʲ�������ģ����ѯ:��Χ#ASSET_TYPE----#ASSET_PRODEALER
* - ����
* - 1.��ָ���������������,Ĭ�ϰ�"�ʲ�����"��������
* - 2.֧�ֵ���������,����:#NAME_TYPE(�ʲ�����),#CODE_TYPE(�ʲ�����)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAssetList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT ASSET_INFO_S                    *pstAssetList
);

/**
* ��ѯ�ʲ�ͳ��
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       szOrgCode               ��֯����
* @param [IN]       ulClassification        ͳ�Ʒ��࣬ȡֵ#STATISTIC_CLASSIFICATION_TYPE_E��Ĭ��Ϊ���ʲ����ͷ���ͳ��
* @param [IN]       pstQueryCondition       ͨ�ò�ѯ���������޲�ѯ��������ΪNULL
* @param [IN]       pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]      pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]      pstAssetStatistic       �ʲ�ͳ������
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 2.֧�ְ��ʲ������Բ�ѯ:#ASSET_TYPE,#ASSET_MANUFACTURE,#ASSET_PURCHASE_TIME,#ASSET_WARRANT_TIME,
                           #ASSET_INSTALL_TIME,#ASSET_PRODEALER
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAssetStatistic
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    CHAR                           szOrgCode[IMOS_CODE_LEN],
    IN    ULONG                          ulClassification,
    IN    COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN    QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    OUT   RSP_PAGE_INFO_S                *pstRspPageInfo,
    OUT   ASSET_STATISTIC_S              *pstAssetStatistic
);

/**
* �����豸�ʲ���¼�����
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   ulBindPolicy            ��¼����ԣ�ȡֵ#DEVASSET_BINDPOLICY_E
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDevAssetBindPolicy
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   ULONG                       ulBindPolicy
);

/**
* ��ѯ�豸�ʲ���¼�����
* @param [IN]   pstUserLogIDInfo       �û���Ϣ
* @param [OUT]  pulBindPolicy          ��¼����ԣ�ȡֵ#DEVASSET_BINDPOLICY_E
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QryDevAssetBindPolicy
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    OUT  ULONG                      *pulBindPolicy
);

/**
* ��ѯ��֯�¹����豸�б�
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                  ��֯����
* @param [IN]   pstQueryCondition          ��ѯ����
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstFaultDevList            �����豸��Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - ����
* - 1.��������ΪNULL,��ʾ��ָ����֯�����й����豸�б�(��������֯)
* - 2.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 3.֧�ְ�"�豸����"ģ����ѯ:#NAME_TYPE
* - 3.֧�ְ��豸����#DEV_TYPE������ѯ��,
            ��������#FAULT_TYPE������ѯ��ȡֵ#IMOS_DEV_FAULT_STATUS_E
            ����ͳ��ʱ��#FAULT_STAT_TIME������ѯ
* - ����
* - 1.��ָ���������������,Ĭ�ϰ�"�豸����"��������
* - 2.֧�ֵ���������,����:#NAME_TYPE(�豸����),#CODE_TYPE(�豸����)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryFaultDevList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT FAULT_DEV_INFO_S                *pstFaultDevList
);

/**
* ��ѯ�����豸ͳ��
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       szOrgCode               ��֯����
* @param [IN]       pstQueryCondition       ͨ�ò�ѯ���������޲�ѯ��������ΪNULL
* @param [IN]       pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]      pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]      pstFaultStatistic       ����ͳ���б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 2.֧�ְ��豸���Ͳ�ѯ#DEV_TYPE
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryFaultDevStatistic
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    CHAR                           szOrgCode[IMOS_CODE_LEN],
    IN    COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN    QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    OUT   RSP_PAGE_INFO_S                *pstRspPageInfo,
    OUT   FAULT_STATISTIC_S              *pstFaultStatistic
);


/**
* �����������ļ���Ϣ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ
* @param [IN]   szParentDomainCode      �������
* @param [IN]   szSubDomainCode         �������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_UpdateTopologyInfo
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  CHAR                       szParentDomainCode[IMOS_CODE_LEN],
    IN  CHAR                       szSubDomainCode[IMOS_CODE_LEN]
);

/**
* ��ѯ������֮���ҵ����Ϣ \n
* @param [IN]   pstUserLoginIDInfo      �û���¼ID��Ϣ
* @param [IN]   szParentDomainCode      �������
* @param [IN]   szSubDomainCode         �������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServiceInfoBetweenDomain
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  CHAR                       szParentDomainCode[IMOS_CODE_LEN],
    IN  CHAR                       szSubDomainCode[IMOS_CODE_LEN]
);

/**
* �ֹ�ͬ��ҵ��ͳ������
* @param [IN]   pstUserLogIDInfo      �û���Ϣ
* @param [IN]   pstSyncSrvInfo        ͬ��ҵ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ֧��ȫ���ʲ�/�����豸ͳ�������ֶ�ͬ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ManualSyncStatData
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   SYNC_SERVICE_INFO          *pstSyncSrvInfo
);

/**
* ���ÿ���λ
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szCamCode           ����ͷ����
* @param [IN]   pstGuardPos         ����λ��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ�˽ӿ�ֻ֧�ֶ�������������ÿ���λ
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetGuardPosition
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCamCode[IMOS_CODE_LEN],
    IN  GUARD_POSITION_INFO_S   *pstGuardPos
);

/**
* ɾ������λ
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szCamCode           ����ͷ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ�˽ӿ�ֻ֧�ֶ����������ɾ������λ
*/
ULONG STDCALL IMOS_DelGuardPosition
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCamCode[IMOS_CODE_LEN]
);

#if 0
#endif

/**
* ��ѯ��������û���Ϣ
* @param [IN]   *pstQueryCondition        ͨ�ò�ѯ����
* @param [OUT]  pstUserPasswordInfo       ��������û���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* -��Ӧͨ�ò�ѯ�����еĲ�ѯ�����ͷֱ�Ϊ:#USER_CODE_TYPE��#USER_NAME_TYPE��#USER_DESC
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUserInfoByCondition
(
    IN   COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    OUT  USER_INFO_PASSWORD_S        *pstUserPasswordInfo
);

/**
* ���������¼� \n
* @param [IN]    pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]    szDeviceCode       �豸����
* @param [IN]    ulAlarmType        �澯����(Ŀǰ��֧��#AlARM_TYPE_IMPERATIVE_EVENT)
* @param [IN]    ulXmlLen           xml�ַ�������
* @param [IN]    pcXml              xml�ַ���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendImperativeEvent
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                 szDeviceCode[IMOS_CODE_LEN],
    IN  ULONG                ulAlarmType,
    IN  ULONG                ulXmlLen,
    IN  CHAR                 *pcXml
);

/**
* ˢ��������洢����
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szCamCode               ���������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_RefreshStoreIdx
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szCamCode[IMOS_CODE_LEN]
);

#if 0
#endif

/**
* ���ü�����������Ϣ
* @param [IN] pstUserLogIDInfo �û���Ϣ��ʶ
* @param [IN] szScrCode        ����������
* @param [IN] pstSplitScrInfo  ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_DEVICE_OFFLINE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigSplitScrInfo
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR                 szScrCode[IMOS_CODE_LEN],
    IN SPLIT_SCR_INFO_S     *pstSplitScrInfo
);

/**
* ��ѯ������������Ϣ
* @param [IN]  pstUserLogIDInfo �û���Ϣ��ʶ
* @param [IN]  szScrCode        ����������
* @param [OUT] pstSplitScrInfo  ������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_AS_DEVICE_OFFLINE
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySplitScrInfo
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                 szScrCode[IMOS_CODE_LEN],
    OUT SPLIT_SCR_INFO_S     *pstSplitScrInfo
);

/**
* �����Դ���ղؼ� \n
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   ulResType                   ��Դ����
* @param [IN]   szResCode                   ��Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddResToFavorite
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ULONG                       ulResType,
    IN  CHAR                        szResCode[IMOS_CODE_LEN]
);

/**
* ���ղؼ�ɾ����Դ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   ulResType                   ��Դ����
* @param [IN]   szResCode                   ��Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/

IMOS_EXPORT ULONG STDCALL IMOS_DelResFromFavorite
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ULONG                       ulResType,
    IN  CHAR                        szResCode[IMOS_CODE_LEN]
);

/**
* ��ѯ�ղؼ���Դ�б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstResList              �ղؼ�����Դ�б���Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryFavoriteResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT RES_ITEM_V2_S               *pstResList
);
/**
* ���ӵ�����IPC \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   pstThirdPartyIPCInfo         ������IPC��Ϣ
* @param [IN]   pstVINBindCamBaseInfo        ����������ͨ���Ļ�����Ϣ
* @param [IN]   pstVINFullVarietyElement     ��Ƶͨ��ȫ���䳤Ԫ�ؽṹ��
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddThirdPartyIPC
(
    IN USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN THIRDPARTY_IPC_S               *pstThirdPartyIPCInfo,
    IN VINCHNL_BIND_CAMERA_BASE_S     *pstVINBindCamBaseInfo,
    IN VINCHNL_FULL_VARIETY_ELEMENT_S *pstVINFullVarietyElement
);

/**
* �޸ĵ�����IPC \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   pstThirdPartyIPCInfo         ������IPC��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyThirdPartyIPC
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN THIRDPARTY_IPC_S     *pstThirdPartyIPCInfo
);

/**
* ɾ��������IPC \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   szThirdPartyIPCCode          ������IPC����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelThirdPartyIPC
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR                 szThirdPartyIPCCode[IMOS_CODE_LEN]
);

/**
* ��ѯһ��������IPC�ľ�����Ϣ \n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   szThirdPartyIPCCode          ������IPC����
* @param [OUT]  pstThirdPartyIPCInfo         ������IPC��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryThirdPartyIPC
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                 szThirdPartyIPCCode[IMOS_CODE_LEN],
    OUT THIRDPARTY_IPC_S     *pstThirdPartyIPCInfo
);

/**
* ��ѯ������IPC�б�
* �����ѯ����ʱ,������,Ĭ������(IPC��������)
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                   ��֯�����
* @param [IN]   pstQueryCondition           ͨ�ò�ѯ����(��ΪNULL, ��������: #NAME_TYPE[IPC����];)
* @param [IN]   pstQueryPageInfo            �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo              ���ط�ҳ��Ϣ
* @param [OUT]  pstThirdPartyIPCInfoList    ������IPC�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryThirdPartyIPCListByOrg
(
    IN USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN CHAR                     szOrgCode[IMOS_CODE_LEN],
    IN COMMON_QUERY_CONDITION_S *pstQueryCondition,
    IN QUERY_PAGE_INFO_S        *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT THIRDPARTY_IPC_S        *pstThirdPartyIPCInfoList
);

/**
* ��Դͳ��������ѯ
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                   ��֯����
* @param [IN]   ulResType                   ��Դ���ͣ�ȡֵΪ#IMOS_TYPE_CAMERA��#IMOS_TYPE_GUARD_TOUR_RESOURCE��#IMOS_TYPE_ALARM_SOURCE��#IMOS_TYPE_ALARM_OUTPUT��#IMOS_TYPE_SALVO_RESOURCE��#IMOS_TYPE_GROUP_SWITCH_RESOURCE��#IMOS_TYPE_SCENE��#IMOS_TYPE_MAP,
* #IMOS_TYPE_BROADCAST_GROUP
* @param [IN]   ulResStatus                 ��Դ״̬��ֻ��IMOS_TYPE_CAMERA��Ч��ȡֵΪ#IMOS_DEV_FAULT_STATUS_E, ��ȡֵΪ#IMOS_DEV_FAULT_STATUS_ALLʱ��ʾ����״̬
* @param [IN]   bQuerySubOrg                �Ƿ��ѯ����֯
* @param [OUT]  pulResNum                   ��Դ����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResourceStatistics
(
    IN  USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN  CHAR                           szOrgCode[IMOS_CODE_LEN],
    IN  ULONG                          ulResType,
    IN  ULONG                          ulResStatus,
    IN  BOOL_T                         bQuerySubOrg,
    OUT ULONG                          *pulResNum
);

/**
* ������ı��ô洢¼����� \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bExtDomainDev           �Ƿ����������͵������
* @param [IN]   pstSDKRecQueryInfo      �طż�����Ϣ���ݽṹ�����������������szCamCodeΪ������������
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstSDKRecordFileInfo    ¼���ļ���Ϣ���ݽṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��ѯʱ���Ȳ��ܴ���24Сʱ������¼�����������ʱ���ʽΪ��"%Y-%m-%d %H:%M:%S"��ʽ ����Ϣ�����޶�Ϊ24�ַ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_SlaveRecordRetrieval
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BOOL_T                      bExtDomainDev,
    IN  REC_QUERY_INFO_S            *pstSDKRecQueryInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT RECORD_FILE_INFO_S          *pstSDKRecordFileInfo
);

/**
* ��ȡ������ı��ô洢¼���URL��Ϣ \n
* @param[IN]    pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   bExtDomainDev               �Ƿ����������͵������
* @param[IN]    pstSDKGetUrlInfo            ��ȡURL������Ϣ���ݽṹ�����������������szCamCodeΪ������������
* @param[OUT]    pstSDKURLInfo               URL��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetSlaveRecordFileURL
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BOOL_T                      bExtDomainDev,
    IN  GET_URL_INFO_S              *pstSDKGetUrlInfo,
    OUT URL_INFO_S                  *pstSDKURLInfo
);

/**
* ��ѯ��������ԵĽӿ� \n
* @param [IN]  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]  CHAR                         szCamCode[IMOS_CODE_LEN]    ���������
* @param [OUT] ULONG		                *pulAttribute               �����������Ϣ��0�������������2����DVR���������������
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ����ز�������룺��������ļ�sdk_err.h
* @note ��
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamAttribute
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  CHAR                             szCamCode[IMOS_CODE_LEN],
    OUT ULONG                            *pulAttribute
);

#if 0
#endif

/**
* ����ϵͳ������Ϣ
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       ulSysParamNum           ϵͳ��������, ���Ϊ#IMOS_SYS_PARAM_MAX_NUM
* @param [IN][OUT]  pstSysParamListInfos    ϵͳ�����б���Ϣ, ������Ϣ�������ͱ�����д
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetSystemParamInfos
(
    IN      USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN      ULONG                   ulSysParamNum,
    INOUT   IMOS_SYS_PARAM_INFO_S   *pstSysParamList
);

/**
* ��ѯϵͳ������Ϣ
* @param [IN]       pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]       ulSysParamNum           ϵͳ��������, ���Ϊ#IMOS_SYS_PARAM_MAX_NUM
* @param [IN][OUT]  pstSysParamListInfos    ϵͳ�����б���Ϣ, ������Ϣ�������ͱ�����д
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySystemParamInfos
(
    IN      USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN      ULONG                   ulSysParamNum,
    INOUT   IMOS_SYS_PARAM_INFO_S   *pstSysParamList
);

/**
* ��ѯϵͳʱ�䣬CPU���ڴ�ʹ���ʵ���Ϣ
* @param [IN]       pstUserLogIDInfo     �û���¼ID��Ϣ��ʶ
* @param [IN]       ulQuerySysInfoNum    ��ѯ��Ϣ���� (С��#QUERY_SYS_INFO_NAME_MAX)
* @param [IN][OUT]  pstQuerySysInfoList  ��ѯϵͳ��Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_PARAM_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySysInfo
(
    IN     USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN     ULONG                 ulQuerySysInfoNum,
    INOUT  QUERY_SYS_INFO_S      *pstQuerySysInfoList
);

/**
* ��ѯ��֯���豸�Ĺ�����ϸ��Ϣ�б�
* @param [IN]   pstUserLogIDInfo           �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode                  ��֯����
* @param [IN]   pstQueryCondition          ��ѯ����
* @param [IN]   pstQueryPageInfo           �����ҳ��Ϣ
* @param [IN]   pstQueryTimeSlice          ��ѯʱ��Ƭ��Ϣ
* @param [OUT]  pstRspPageInfo             ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstDevFaultDetailList      �豸������ϸ��Ϣ�б�
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - ����
* - 1.��������ΪNULL,��ʾ��ָ����֯�����й����豸�б�(��������֯)
* - 2.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 3.֧�ְ�"�豸����"ģ����ѯ:#NAME_TYPE
* - 4.֧�ְ�"�豸����"��ȷ��ѯ:#CODE_TYPE
* - 5.֧�ְ��豸����#DEV_TYPE������ѯ��,
            ��������#FAULT_TYPE������ѯ��ȡֵ#IMOS_DEV_FAULT_STATUS_E
            ����ͳ��ʱ��#FAULT_STAT_TIME������ѯ
* - ����
* - 1.��ָ���������������,Ĭ�ϰ�"�豸����"��������
* - 2.֧�ֵ���������,����:#NAME_TYPE(�豸����),#CODE_TYPE(�豸����),���Ϸ���ʱ�䣬���ϻָ�ʱ�䣬����ʱ����������������
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevFaultDetailList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    IN  QUERY_TIME_SLICE_S              *pstQueryTimeSlice,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT DEV_FAULT_DETAIL_S              *pstDevFaultDetailList
);

/**
* ��ѯ�豸�Ĺ�����ϸ��Ϣͳ��
* @param [IN]       pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]       szOrgCode                   ��֯����
* @param [IN]       pstQueryCondition           ͨ�ò�ѯ���������޲�ѯ��������ΪNULL
* @param [IN]       pstQueryPageInfo            �����ҳ��Ϣ
* @param [IN]       pstQueryTimeSlice           ��ѯʱ��Ƭ��Ϣ
* @param [OUT]      pstRspPageInfo              ��Ӧ��ҳ��Ϣ
* @param [OUT]      pstDevFaultDetailStat       �豸������ϸ��Ϣͳ��
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.֧�ְ�"�Ƿ������֯"������ѯ:#IS_QUERY_SUB = 1 ��;= 0 ��
* - 2.֧�ְ��豸���Ͳ�ѯ#DEV_TYPE
* - 3.֧�ְ�"�豸����"��ȷ��ѯ:#CODE_TYPE
* - 3.֧�ְ����Ϸ���ʱ��ͽ���ʱ���ѯ

* - ����
* - 1.��ָ���������������,Ĭ�ϰ�"�豸����"��������
* - 2.֧�ֵ���������,����:#NAME_TYPE(�豸����),#CODE_TYPE(�豸����),���Ϸ���ʱ�䣬���ϻָ�ʱ�䣬����ʱ����������������

*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevFaultDetailStatistic
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    CHAR                           szOrgCode[IMOS_CODE_LEN],
    IN    COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN    QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    IN    QUERY_TIME_SLICE_S             *pstQueryTimeSlice,
    OUT   RSP_PAGE_INFO_S                *pstRspPageInfo,
    OUT   DEV_FAULT_DETAIL_STAT_S        *pstDevFaultDetailStat
);

/**
* ��ѯ��Դ·����������Դ��Ϣ�� \n
* @param [IN]       pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]       szOrgCode		    ������֯����
* @param [IN]       szResCode			��Դ����
* @param [IN]       ulResType			��Դ����
* @param [IN][OUT]  pulTierNum		    ��Դ·������
* @param [OUT]      pstResList		    ��Դ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.���ص��б���Ҫ���մӶ�����֯(������Root�ڵ�)�����ѯ��Դ�����˳������
*       2.����Դ����Ϊ��������������(��֯)ʱ, ulResTypeͳһ��IMOS_TYPE_ORG
*       3.����������Դ����Ϊ��֯��������֯�������Ϊ��'\0'����������NULL
*       4.��Դ�����������������ֵIMOS_RES_TREE_LEVEL
*       5.������Դ��Ϣ�ṹ���ײͲ���(ulDevEncodeSet)��Ч������������Ч������Դ�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResPathWithInfo
(
    IN    USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN    CHAR                       szOrgCode[IMOS_CODE_LEN],
    IN    CHAR                       szResCode[IMOS_CODE_LEN],
    IN    ULONG                      ulResType,
    INOUT ULONG                      *pulTierNum,
    OUT   RES_ITEM_V2_S              *pstResList
);

/**
* ��ȡ��Դ��·����Ϣ
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode           ��֯�ڵ����
* @param [IN]   szResCode           ��Դ����
* @param [IN]   ulResType           ��Դ���ͣ�ȡֵΪ#IMOS_TYPE_E
* @param [OUT]  szResPath           ��Դ����·����Ϣ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.�����֯����δ��ֵ������Դ������ʱ������������һ������·�������û�л����򷵻���Դԭʼ����λ������·��
*       2.����Դ����Ϊ��������������(��֯)ʱ, ulResTypeͳһ��IMOS_TYPE_ORG
*       3.·���в����������Root�ڵ㣬��������Դ����
*       4.���ص�·���и�����֯ͨ��" >> "���ӣ��磺���� >> �㽭ʡ >> ������ >> ������ >> �����ɳ���
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResourcePath
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                            szResCode[IMOS_CODE_LEN],
    IN  ULONG                           ulResType,
    OUT CHAR                            szResPath[IMOS_RES_PATH_LEN]
);

/**
* ��ȡ��Դ��������� \n
* @param [IN]     pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]     szOrgCode           ����Դ��֯����
* @param [IN]     szResCode           ��Դ����
* @param [IN]     ulResType           ��Դ���ͣ�ȡֵΪ#IMOS_TYPE_E
* @param [OUT]    pulOrderNumber      ��Դ�������ֵ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.����Դ����Ϊ�����֯ʱ������Դ��֯��������Ϊ��
*       2.����Դ����Ϊ��������������(��֯)ʱ, ulResTypeͳһ��IMOS_TYPE_ORG
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResourceOrderNumber
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                                szResCode[IMOS_CODE_LEN],
    IN  ULONG                               ulResType,
    OUT ULONG                               *pulOrderNumber
);

/**
* �޸���Դ��������� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szOrgCode           ����Դ��֯����
* @param [IN]   szResCode           ��Դ����
* @param [IN]   ulResType           ��Դ���ͣ�ȡֵΪ#IMOS_TYPE_E
* @param [IN]   ulOrderNumber       ��Դ�������ֵ
* @return ULONG �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.����Դ����Ϊ�����֯ʱ������Դ��֯��������Ϊ��
*       2.����Դ����Ϊ��������������(��֯)ʱ, ulResTypeͳһ��IMOS_TYPE_ORG
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyResourceOrderNumber
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                                szResCode[IMOS_CODE_LEN],
    IN  ULONG                               ulResType,
    IN  ULONG                               ulOrderNumber
);

/**
* ��ѯ�澯�����¼
* �����ѯ����ʱ,������,��Ĭ������(ִ��ʱ������)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szEventRecordCode       �澯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;��������:#USER_NAME_TYPE[�û�����];#OPER_IP[IP��ַ];#OPER_TYPE[��������];#OPER_TIME[����ʱ��])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstAlarmRecordInfoList  �澯��¼��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmRecordInfoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szEventRecordCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_RECORD_INFO_S         *pstAlarmRecordInfoList
);

/**
* �޸�Ԥ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstActionPlanInfo       Ԥ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ͨ��Ԥ����֧��ת��ԭ�澯�Ͳ����¸澯������������ͨ��Ԥ��������ִ�в���ֻ�����Զ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyActionPlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ACTION_PLAN_INFO_S          *pstActionPlanInfo
);

/**
* ɾ��Ԥ��
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szActionPlanCode    Ԥ������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelActionPlan
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szActionPlanCode[IMOS_CODE_LEN]
);

/**
* ��ѯԤ���б�
* �����ѯ����ʱ,������,��Ĭ������(Ԥ����������)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;��������:#NAME_TYPE[Ԥ������];#TYPE_ACTION_PLAN[Ԥ������])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstActionPlanList       Ԥ���б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryActionPlanList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ACTION_PLAN_BASE_INFO_S     *pstActionPlanList
);

/**
* ��ѯ�������б�
* �����ѯ����ʱ,������,��Ĭ������(������ID����)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   ulActionPlanType        Ԥ�����ͣ�ȡֵΪ#ACTION_PLAN_TYPE_E
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;��������:#NAME_TYPE[����������];#TRIGGER_ID[������ID])
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstTriggerList          �������б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTriggerList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ULONG                       ulActionPlanType,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TASK_TRIGGER_INFO_S         *pstTriggerList
);

/**
* ��������\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventRecordCode   �澯����
* @param [IN]   ulTaskNum           ������Ŀ
* @param [IN]   pstPlanTaskInfoList ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DealWithTask
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szEventRecordCode[IMOS_CODE_LEN],
    IN  ULONG                   ulTaskNum,
    IN  PLAN_TASK_BASE_INFO_S   *pstPlanTaskInfoList
);

/**
* ������������ȷ�ϸ澯����ԭ�Ƚӿڣ�����������ʹ�ô˽ӿڣ�\n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventRecordCode   �澯����
* @param [IN]   ulTriggerID         ������ID
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DealWithTrigger
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szEventRecordCode[IMOS_CODE_LEN],
    IN  ULONG                   ulTriggerID
);

/**
* ���ݸ澯�����ѯ�������б�
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szEventRecordCode       �澯����
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstTaskTriggerList      ��������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmTriggerList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szEventRecordCode[IMOS_CODE_LEN],
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TASK_TRIGGER_INFO_S         *pstTaskTriggerList
);

/**
* ���ݸ澯�����ѯ�����б�
* @param [IN]       pstUserLogIDInfo   �û���Ϣ
* @param [IN]       szEventRecordCode  �澯����
* @param [IN][OUT]  pulTaskNum         ������(��ν���Ϊ#IMOS_MAX_TASK_NUM)
* @param [OUT]      pstTaskInfoList    ������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmTaskInfoList
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szEventRecordCode[IMOS_CODE_LEN],
    INOUT ULONG                 *pulTaskNum,
    OUT   PLAN_TASK_BASE_INFO_S *pstTaskInfoList
);

/**
* ����Ԥ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   szOrgCode               ��֯����
* @param [IN]   pstActionPlanInfo       Ԥ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ͨ��Ԥ����֧��ת��ԭ�澯�Ͳ����¸澯������������ͨ��Ԥ��������ִ�в���ֻ�����Զ�
*       Ԥ�����봫��Ϊ��Чֵ
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddActionPlan
(
    IN      USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN      CHAR                    szOrgCode[IMOS_CODE_LEN],
    IN      ACTION_PLAN_INFO_S      *pstActionPlanInfo
);

/**
* ��ѯԤ��
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szActionPlanCode    Ԥ������
* @param [OUT]  pstActionPlanInfo   Ԥ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryActionPlan
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szActionPlanCode[IMOS_CODE_LEN],
    OUT   ACTION_PLAN_INFO_S    *pstActionPlanInfo
);

/**
* ����Ԥ��
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szActionPlanCode    Ԥ������
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartActionPlan
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szActionPlanCode[IMOS_CODE_LEN]
);

#if 0
#endif

/**
* �û�����/����˫�������Խ� \n
* @param [IN]   pstOperUserInfo      �����û���Ϣ
* @param [IN]   pstDstInfo           Ŀ����Ϣ
* @param [IN]   ulVocTalkApplySeq    �����Խ��������кţ����з���������ʱ��д
* @param [IN]   ulSrvType            ����ҵ�����ͣ�ȡֵ�μ�SRV_TYPE_E
* @param [IN]   ulOperateCode        ����ԭ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E��
*     �����û���дUSER_OPERATE_SERVICE��
*     �����û���дCALLEE_USER_OPERATE_SERVICE
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartUserVoiceTalk
(
    IN  USER_LOGIN_ID_INFO_S            *pstOperUserInfo,
    IN  VOC_SRV_DST_INFO_S              *pstDstInfo,
    IN  ULONG                            ulVocTalkApplySeq,
    IN  ULONG                            ulSrvType,
    IN  ULONG                            ulOperateCode
);

/**
* �û�ֹͣ/�ܾ�˫�������Խ� \n
* @param [IN]   pstOperUserInfo      �����û���Ϣ
* @param [IN]   pstDstInfo           Ŀ���û���Ϣ
* @param [IN]   ulVocTalkApplySeq    �����Խ��������кţ����з���д
* @param [IN]   ulOperateCode        ����ԭ����
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E��
*     �����û���дUSER_OPERATE_SERVICE��
*     �����û���дCALLEE_USER_OPERATE_SERVICE,
*     ��������û���֧�ִ�ҵ��Ŀ���дCALLEE_NOT_SUPPORT_SERVICE
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopUserVoiceTalk
(
    IN  USER_LOGIN_ID_INFO_S            *pstOperUserInfo,
    IN  VOC_SRV_DST_INFO_S              *pstDstInfo,
    IN  ULONG                            ulVocTalkApplySeq,
    IN  ULONG                            ulOperateCode
);

/**
* ��ѯ����ҵ����Ϣ \n
* @param [IN]   pstOperUserInfo     �����û���Ϣ
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����, ֻ�������㲥������
* @param [IN]   pstQueryPageInfo    �����ҳ��Ϣ, ֻ�������㲥������
* @param [OUT]  pstRspPageInfo      ���ط�ҳ��Ϣ
* @param [OUT]  pstRecordInfo       ����������¼��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUserVoiceSrvInfo
(
    IN   USER_LOGIN_ID_INFO_S        *pstSrcUserInfo,
    IN   COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN   QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT  RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT  VOCSRV_RECORD_INFO_S        *pstVocRecordInfo
);

/**
* �û�����/���������㲥 \n
* @param [IN]       pstOperUserInfo      �����û���Ϣ
* @param [IN]       ulVocBrdApplySeq     �����㲥�������кţ����з���������ʱ��д
* @param [IN]       ulSrvType            ����ҵ�����ͣ�ȡֵ�μ�SRV_TYPE_E
* @param [IN]       ulOperateCode        ����ԭ����
* @param [IN]       ulVocBrdUnitNum      �㲥��Ԫ����
* @param [IN][OUT]  pstVocBrdUnit        �����㲥��Ԫ�б���Ϊ����ʱ����д״̬
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E��
*     �����û���дUSER_OPERATE_SERVICE��
*     �����û���дCALLEE_USER_OPERATE_SERVICE
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartUserVoiceBrd
(
    IN      USER_LOGIN_ID_INFO_S            *pstOperUserInfo,
    IN      ULONG                            ulVocBrdApplySeq,
    IN      ULONG                            ulSrvType,
    IN      ULONG                            ulOperateCode,
    IN      ULONG                            ulVocBrdUnitNum,
    INOUT   VOCSRV_RECORD_INFO_S            *pstVocBrdUnit
);

/**
* �û�ֹͣ/�ܾ������㲥 \n
* @param [IN]       pstOperUserInfo      �����û���Ϣ
* @param [IN]       ulVocBrdApplySeq     �����㲥�������кţ����з�ֹͣ/�ܾ�����ʱ��д
* @param [IN]       ulOperateCode        ����ԭ����
* @param [IN]       ulVocBrdUnitNum      �㲥��Ԫ����
* @param [IN][OUT]  pstVocBrdUnit        �����㲥��Ԫ�б���Ϊ����ʱ����д״̬
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note
* - 1.ulOperateCodeȡֵ�μ�CS_OPERATE_CODE_E��
*     �����û���дUSER_OPERATE_SERVICE��
*     �����û���дCALLEE_USER_OPERATE_SERVICE
*     ��������û���֧�ִ�ҵ��Ŀ���дCALLEE_NOT_SUPPORT_SERVICE
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopUserVoiceBrd
(
    IN      USER_LOGIN_ID_INFO_S            *pstOperUserInfo,
    IN      ULONG                            ulVocBrdApplySeq,
    IN      ULONG                            ulOperateCode,
    IN      ULONG                            ulVocBrdUnitNum,
    INOUT   VOCSRV_RECORD_INFO_S            *pstVocBrdUnit
);

/**
* ������֯�Զ������ʶ��Ϣ
* @param [IN] pstUserLogIDInfo �û���¼ID��Ϣ��ʶ
* @param [IN] pstAutoShareInfo �Զ�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetAutoShareOrg
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN EX_AUTO_SHARE_INFO_S *pstAutoShareInfo
);

/**
* ��ѯ��֯�Զ�������Ϣ
* @param [IN] pstUserLogIDInfo �û���¼ID��Ϣ��ʶ
* @param [IN] szExDomainCode �������
* @param [IN] szOrgCode ��֯����
* @param [OUT] pstAutoShareInfo �Զ�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAutoShareOrg
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR szExDomainCode[IMOS_CODE_LEN],
    IN CHAR szOrgCode[IMOS_CODE_LEN],
    OUT EX_AUTO_SHARE_INFO_S *pstAutoShareInfo
);

#if 0
#endif

/**
* ��ѯδ����ɹ���Դ��¼
* @param [IN]   pstUserLogIDInfo         �û���Ϣ
* @param [IN]   szExDomainCode           �������
* @param [IN]   pstQueryCondition        ��ѯ����
* @param [IN]   pstQueryPageInfo         �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo           ��Ӧ��ҳ��Ϣ
* @param [OUT]  pstShareUnsucceedResList δ����ɹ���Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note Ŀǰ���˲�ѯ����֧����Դ����(RES_TYPE)�ͽ����(RESULT_CODE)��֧�ְ���Դ�������������
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareUnsucceedRes
(
    IN     USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN     CHAR                      szExDomainCode[IMOS_CODE_LEN],
    IN     COMMON_QUERY_CONDITION_S *pstQueryCondition,
    IN     QUERY_PAGE_INFO_S        *pstQueryPageInfo,
    OUT    RSP_PAGE_INFO_S          *pstRspPageInfo,
    OUT    SHARE_UNSUCCEED_RES_S    *pstShareUnsucceedResList
);

/**
* ɾ��δ����ɹ���Դ��¼
* @param [IN]   pstUserLogIDInfo         �û���Ϣ
* @param [IN]   szExDomainCode           �������
* @param [IN]   ulDelResNum              ��ɾ����¼��Ŀ
* @param [IN]   pstShareUnsucceedResList δ����ɹ���Դ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ulDelResNumΪ0��ʾɾ����������δ����ɹ���¼
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelShareUnsucceedRes
(
    IN     USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN     CHAR                      szExDomainCode[IMOS_CODE_LEN],
    IN     ULONG                     ulDelResNum,
    IN     SHARE_UNSUCCEED_RES_S    *pstShareUnsucceedResList
);


/* Begin add by z07291 for VVD64632 */
/**
* �޸Ĺ���������������Ϣ(Ŀǰֻ�о�γ����Ϣ)
* @param [IN]   pstUserLogIDInfo            �û���Ϣ��ʶ
* @param [IN]   szOrgCode                   ��֯�ڵ����
* @param [IN]   szExDomainCode              �������
* @param [IN]   pstCamInfo                  �������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyShareCamera
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  CAMERA_INFO_S                   *pstCamInfo
);

/**
* ¼�����(V2) \n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSDKRecQueryInfo      �طż�����Ϣ���ݽṹ
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  pstSDKRecordFileInfo    ¼���ļ���Ϣ���ݽṹ��
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1����ѯʱ���Ȳ��ܴ���24Сʱ������¼�����������ʱ���ʽΪ��"%Y-%m-%d %H:%M:%S"��ʽ ����Ϣ�����޶�Ϊ24�ַ�.
*       2���ļ����ַ���������󳤶�ΪIMOS_FILE_NAME_LEN_V2
*/
IMOS_EXPORT ULONG STDCALL IMOS_RecordRetrievalV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  REC_QUERY_INFO_S            *pstSDKRecQueryInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT RECORD_FILE_INFO_V2_S       *pstSDKRecordFileInfo
);

/**
* ��ȡ¼���ļ���URL��Ϣ(V2) \n
* @param[IN]    pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param[IN]    pstSDKGetUrlInfo            ��ȡURL������Ϣ���ݽṹ
* @param[IN]    pstSDKURLInfo               URL��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1������SOI��Ϣ��IE�ṹʹ��SOI_IE_GET_REC_URL_V2_S������SOI_IE_GET_URL_S
*          ע����SDK�ӿ�IMOS_GetBakRecordFileURL������
*          AS_RQģ���ڴ���GetURLҵ��ʱ���ὫSOI_IE_GET_REC_URL_V2_S�е���Ϣת��ΪSOI_IE_GET_URL_S
*       2���ļ����ַ���������󳤶�ΪIMOS_FILE_NAME_LEN_V2
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetURLV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  GET_URL_INFO_V2_S           *pstSDKGetUrlInfo,
    OUT URL_INFO_S                  *pstSDKURLInfo
);
/* End add by z07291 for VVD64632 */




/**
* ����һ���¼�ʹ�ܲ���
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szAlarmSrcCode      �澯Դ���룬����Ϊ�գ���ʾϵͳ�澯���͵�ʹ�ܲ���
* @param [IN]   ulEventType         �¼�����ID
* @param [IN]   pstEventEnableInfo  �¼�ʹ�ܲ�����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szAlarmSrcCode[IMOS_CODE_LEN],
    IN    ULONG                 ulEventType,
    IN    EVENT_ENABLE_INFO_S   *pstEventEnableInfo
);

/**
* ����һ���¼�ʹ�ܲ���
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szAlarmSrcCode      �澯Դ���룬����Ϊ�գ���ʾϵͳ�澯���͵�ʹ�ܲ���
* @param [IN]   ulEventType         �¼�����ID
* @param [IN]   pstEventEnableInfo  �¼�ʹ�ܲ�����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_UpdateEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szAlarmSrcCode[IMOS_CODE_LEN],
    IN    ULONG                 ulEventType,
    IN    EVENT_ENABLE_INFO_S   *pstEventEnableInfo
);

/**
* ��ѯһ���¼�ʹ�ܲ���
* @param [IN]     pstUserLogIDInfo      �û���Ϣ
* @param [IN]     szAlarmSrcCode        �澯Դ���룬����Ϊ�գ���ʾϵͳ�澯���͵�ʹ�ܲ���
* @param [IN]     ulEventType           �¼�ID
* @param [OUT]    pstEventEnableInfo    �¼�ʹ�ܲ�����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szAlarmSrcCode[IMOS_CODE_LEN],
    IN    ULONG                 ulEventType,
    OUT   EVENT_ENABLE_INFO_S   *pstEventEnableInfo
);

/**
* ɾ��һ���¼�ʹ�ܲ���
* @param [IN]   pstUserLogIDInfo    �û���Ϣ
* @param [IN]   szAlarmSrcCode      �澯Դ���룬����Ϊ�գ���ʾϵͳ�澯���͵�ʹ�ܲ���
* @param [IN]   ulEventType         �¼�����ID
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN    CHAR                  szAlarmSrcCode[IMOS_CODE_LEN],
    IN    ULONG                 ulEventType
);

/**
* ���������澯Դ���¼�ʹ�ܲ���
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstBathEventEnableInfo  ���������豸��Ϣ
* @param [IN]   pstEventEnableInfo      �¼�ʹ�ܲ�����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_BathAddEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    BATH_EVENT_ENABLE_INFO_S  *pstBathEventEnableInfo,
    IN    EVENT_ENABLE_INFO_S       *pstEventEnableInfo
);

/**
* ����ɾ���澯Դ���¼�ʹ�ܲ���
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstBathEventEnableInfo  ���������豸��Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_BathDelEventStrategy
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    BATH_EVENT_ENABLE_INFO_S  *pstBathEventEnableInfo
);

/**
* ��ѯϵͳ�澯������Ϣ
* �����ѯ����ʱ,������,��Ĭ������(�澯��������)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ
* @param [IN]   pstQueryCondition       ͨ�ò�ѯ����(��ΪNULL;#EVENT_TYPE_NAME[�澯�¼�����]; #EVENT_SECURITY[�澯�¼�����];#NAME_TYPE[�澯�¼���������];)
* @param [IN]   pstQueryPageInfo        �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo          ���ط�ҳ��Ϣ
* @param [OUT]  psEventTypeInfoList     �澯������Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ֻ֧���¼����ơ��¼��������Ƶ�ģ����ѯ�Լ��¼������ѯ
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySystemEventTypeInfoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT EVENT_TYPE_EXT_INFO_S       *pstEventTypeInfoList
);

/**
* ��ѯ����Э��澯���Ͷ�Ӧ��ϵ��Ϣ\n
* @param [IN]   pstUserLogIDInfo              �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventTypeL2ORelCode         ����Э��澯���Ͷ�Ӧ��ϵID����
* @param [OUT]   pstEventTypeL2Orel           ����Э��澯���Ͷ�Ӧ��ϵ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventTypeL2ORel
(
    IN   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN   CHAR                        szEventTypeL2ORelCode[IMOS_CODE_LEN],
    OUT  EVENT_TYPE_L2OREL_S         *pstEventTypeL2Orel
);

/**
* ��ӱ���Э��澯���Ͷ�Ӧ��ϵ��Ϣ\n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventTypeL2Orel           ����Э��澯���Ͷ�Ӧ��ϵ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddEventTypeL2ORel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_TYPE_L2OREL_S         *pstEventTypeL2Orel
);

/**
* ɾ������Э��澯���Ͷ�Ӧ��ϵ��Ϣ\n
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szEventTypeL2ORelCode   ����Э��澯���Ͷ�Ӧ��ϵID����
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelEventTypeL2ORel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szEventTypeL2ORelCode[IMOS_CODE_LEN]
);

/**
* �޸ı���Э��澯���Ͷ�Ӧ��ϵ��Ϣ\n
* @param [IN]   pstUserLogIDInfo             �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventTypeL2Orel           ����Э��澯���Ͷ�Ӧ��ϵ��Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyEventTypeL2ORel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_TYPE_L2OREL_S         *pstEventTypeL2Orel
);

/**
* ��ҳ��ѯ����Э��澯���͹�ϵ�б�
* �����ѯ����ʱ,������
* @param [IN]   pstUserLogIDInfo              �û���Ϣ
* @param [IN]   pstQueryCondition             ͨ�ò�ѯ����(��ΪNULL;��������:#EVENT_TYPE[�¼�����])
* @param [IN]   pstQueryPageInfo              �����ҳ��Ϣ
* @param [OUT]  pstRspPageInfo                ���ط�ҳ��Ϣ
* @param [OUT]  pstEventTypeL2OrelList        ����Э��澯���Ͷ�Ӧ��ϵ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventTypeL2ORelList
(
    IN  USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                *pstRspPageInfo,
    OUT EVENT_TYPE_L2OREL_S            *pstEventTypeL2OrelList
);

/**
* ��ѯ�����������ϢV2
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [OUT]  pstSemaOutputInfo       �����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SEMAPHORE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOutSwitch_V2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    OUT SEMA_OUTPUT_INFO_V2_S       *pstSemaOutputInfo
);

/**
* �����豸�����������V2
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   pstChannelIndex         �豸ͨ��������Ϣ
* @param [IN]   pstSemaOutputInfo         �����������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* - #ERR_DAO_SEMAPHORE_NOT_EXIST
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfOutSwitch_V2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    IN  SEMA_OUTPUT_INFO_V2_S       *pstSemaOutputInfo
);

/**
* SDK�ͻ��˲��˳�����£�ֻ��ձ����û���¼��Ϣ�ӿڣ���Ҫʵ�ַ����û��˳���Ϣ�Լ�����SDKΪ���û�ҵ�����������Դ�� \n
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @return ULONG  �������½��:
* - �ɹ���ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룬��������Ĵ������ļ���
* @note
* 1��ֻ���gstAutoKeepAliveCB��������Ϣ��gstClibSysCB�е��û���¼��Ϣ�Լ�gpUICallBackFuncָ��;
* 2��SDK�û�����ֱ���ٴε�¼�󣬶���������Ϣ��ע��ص�����, ���ɽ�������SDKҵ��;
* 3�����ӿڲ�У���û���Ϣ����Ҫ�û��Լ�ע��֮ǰ��¼���û������ӿڻ����gpUICallBackFunc֪ͨ�û��˳�������ֻ������BM
ʹ��;
*/
IMOS_EXPORT ULONG STDCALL IMOS_CleanUpUserInfoOnlyforClientNotExit
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo
);
/**
* ��ӵ������ļ���ʼ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   pstLayerInfo             ���ļ���Ϣ
* @param [OUT]  pstFtpInfo              FTP��Ϣ
* @param [OUT]  pulSequenceNumber       ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisLayerStart
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_CODE_LEN],
    IN  LAYER_BASIC_INFO_S              *pstLayerInfo,
    OUT FTP_INFO_S                      *pstFtpInfo,
    OUT ULONG                           *pulSequenceNumber
);

/**
* ���ϴ���ͼ���ļ��Ľ��, ֪ͨ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess              �ϴ��ļ��Ľ��, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   ulSequenceNumber        ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisLayerEnd
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BOOL_T                      bIsSuccess,
    IN  ULONG                       ulSequenceNumber
);

/**
* ���µ������ļ���ʼ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   pstOldLayerInfo         ����ǰ���ļ���Ϣ
* @param [IN]   pstNewLayerInfo         ���º���ļ���Ϣ
* @param [OUT]  pstFtpInfo              FTP��Ϣ
* @param [OUT]  pulSequenceNumber       ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_UpdateGisLayerStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szGISMapCode[IMOS_CODE_LEN],
    IN  LAYER_BASIC_INFO_S          *pstOldLayerInfo,
    OUT FTP_INFO_S                  *pstFtpInfo,
    OUT ULONG                       *pulSequenceNumber
);

/**
* ���ϴ����ļ��Ľ��, ֪ͨ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess              �ϴ��ļ��Ľ��, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   ulSequenceNumber        ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_UpdateGisLayerEnd
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  BOOL_T                  bIsSuccess,
    IN  ULONG                   ulSequenceNumber
);

/**
* ���ز��ļ��б�ʼ
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [INOUT]  pulLayerNumber          ���ļ�����
* @param [INOUT]  pstFtpInfo              FTP��Ϣ�б�
* @param [OUT]    pulSequenceNumber       ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisLayersStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szGISMapCode[IMOS_CODE_LEN],
    INOUT ULONG                       *pulLayerNumber,
    INOUT FTP_INFO_S                  *pstFtpInfo,
    OUT   ULONG                       *pulSequenceNumber
);

/**
* �����ز��ļ��Ľ��, ֪ͨ������
* @param [IN]   pstUserLogIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN]   bIsSuccess              �����ļ��Ľ��, BOOL_TRUEΪ�ɹ�; BOOL_FALSEΪʧ��
* @param [IN]   ulSequenceNumber        ����FTP�����������к�
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisLayersEnd
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BOOL_T                      bIsSuccess,
    IN  ULONG                       ulSequenceNumber
);

/**
* ��ȡGISͼ�������Ϣ(������ͼ��)
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [OUT]  pulLayerNumber          ͼ����Ŀ
* @param [OUT]  pstLayerInfo            ͼ����Ϣ�б�
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLayerMapInfo
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_CODE_LEN],
    OUT ULONG                        *pulLayerNumber,
    OUT LAYER_BASIC_INFO_S           *pstLayerInfo
);

/**
* ɾ��GISͼ��
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [IN]   szLayerName             ͼ���ļ���
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ע�⣬���ɾ�����ļ���Ҳ��Ҫ���سɹ���
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGisLayer
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szGISMapCode[IMOS_CODE_LEN],
    IN  CHAR                    szLayerName[IMOS_NAME_LEN]
);

/**
* ��ȡGIS��ͼ��ɫ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [OUT]  pstMapColors            GIS��ɫ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
ULONG STDCALL IMOS_GetMapColors
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_CODE_LEN],
    OUT FILE_MAP_COLORS_S            *pstMapColors
);

/**
* ����GIS��ͼ��ɫ��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szGISMapCode            GIS��ͼ����
* @param [OUT]  pstMapColors            GIS��ɫ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
ULONG STDCALL IMOS_SetMapColors
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_CODE_LEN],
    IN  FILE_MAP_COLORS_S            *pstMapColors
);

/**
* ����¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventType       �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��ӵ��¼�����ID��ΧΪ5--30
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddEventSecurityType
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_SECURITY_TYPE_S       *pstEventSecurityType
);

/**
* ɾ���¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo       �û���¼ID��Ϣ��ʶ
* @param [IN]   ulEventSecurityType    �¼�����ID
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ����ɾ�����¼�����ID��ΧΪ5-30
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelEventSecurityType
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ULONG                       ulEventSecurityType
);

/**
* �޸��¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo   �û���¼ID��Ϣ��ʶ
* @param [IN]   pstEventType       �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyEventSecurityType
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_SECURITY_TYPE_S       *pstEventSecurityType
);

/**
* ��ѯ�¼�������Ϣ \n
* @param [IN]   pstUserLogIDInfo          �û���¼ID��Ϣ��ʶ
* @param [IN]   ulEventSecurityType       �¼�����ID
* @param [OUT]  pstEventSecurityType      �¼�������Ϣ
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventSecurityType
(
    IN   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN   ULONG                       ulEventSecurityType,
    OUT  EVENT_SECURITY_TYPE_S       *pstEventSecurityType
);

/**
* ��ѯ�¼����������б� \n
* @param [IN]   pstUserLogIDInfo    �û���¼ID��Ϣ��ʶ
* @param [IN]   pstQueryCondition   ͨ�ò�ѯ����
* @param [IN]   pstQueryPageInfo    ��ѯ��ҳ��Ϣ
* @param [OUT]  pstRspPageInfo      ��ѯ���ط�ҳ��Ϣ
* @param [OUT]  pstSecurityTypeList ������Ϣ�б�
* @return �������½����
* - �ɹ���
* - #ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note 1.��������ΪNULL,��ʾ��ѯ�����¼�������Ϣ
*     - 2.֧�ְ�"�¼���������"ģ����ѯ:#NAME_TYPE;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventSecurityTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_SECURITY_EXT_TYPE_S       *pstSecurityTypeExtList
);

/**
* ���������������\n
* @param [IN]   pstUserLoginIDInfo  �û���¼ID��Ϣ��ʶ
* @param [IN]   ulPlayWndNum        ���Ŵ���������
* @param [INOUT]   pstPlayWndInfo      ���Ŵ������ṹ��ָ�롣
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartPlayerEx
(
     IN USER_LOGIN_ID_INFO_S     *pstUserLoginIDInfo,
     IN ULONG                    ulPlayWndNum,
     INOUT PLAY_WND_INFO_S       *pstPlayWndInfo
);

/**
* ��ȡ�������DecoderTag��Ϣ
* @param [IN]   pstUserLogIDInfo        �û���Ϣ��ʶ
* @param [IN]   szCamCode               ���������
* @param [OUT]  szDecoderTag            DecoderTag
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetDecoderTag
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  CHAR                         szCamCode[IMOS_CODE_LEN],
    OUT CHAR                         szDecoderTag[IMOS_STRING_LEN_64]
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

/**
* ����PTZ����
* @param [IN] pstUserLogIDInfo �û���Ϣ��ʶ
* @param [IN] szCameCode       ���������
* @param [IN] pcData           �����͵�����
* @param [IN] ulDataLen        �����͵����ݳ���, ��󳤶�Ϊ#IMOS_STRING_LEN_1024
* @return �������½����
* - �ɹ���#ERR_COMMON_SUCCEED
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendPtzData
(
    IN  USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN  CHAR                  szCameCode[IMOS_CODE_LEN],
    IN  CHAR                 *pcData,
    IN  ULONG                 ulDataLen
);

/**
* ����ECʵ��������Ӧ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [IN]   pstSetNetworkAdapt       ʵ��������Ӧ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetNetworkAdapt
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    CHAR                       szEcCode[IMOS_CODE_LEN],
    IN    NETWORK_ADAPT_INFO_S       *pstSetNetworkAdapt
);

/**
* ��ѯECʵ��������Ӧ��
* @param [IN]   pstUserLogIDInfo            �û���¼ID��Ϣ��ʶ
* @param [OUT]   pstSetNetworkAdapt       ʵ��������Ӧ����Ϣ
* @return �������½����
* - �ɹ���
* - ʧ�ܣ�
* -     ���ز�������룺��������ļ�sdk_err.h
* @note ��
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryNetworkAdapt
(
    IN    USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN    CHAR                        szEcCode[IMOS_CODE_LEN],
    OUT   NETWORK_ADAPT_INFO_S        *pstSetNetworkAdapt
);

#endif


