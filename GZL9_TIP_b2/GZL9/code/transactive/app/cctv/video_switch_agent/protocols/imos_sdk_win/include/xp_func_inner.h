/*******************************************************************************
Copyright (c) 2008, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
            xp_func_inner.h

Project Code: IMOS XP������
Module Name: IMOS XP������
Date Created: 2010-04-14
Author: shiyouhua/04562
Description: ������XP�ڲ��ӿں���ͷ�ļ�

--------------------------------------------------------------------------------
Modification History
DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_
#define _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_

/* ��������ƽ̨��ͷ�ļ�*/
#include "imos_public.h"
#include "imos_errcode.h"
#include "imos_def.h"

/* ����������SDK���ݽṹͷ�ļ� */
#include "xp_datatype.h"
#include "xp_func.h"

#ifdef  __cplusplus
extern "C"{
#endif

/**
* ��ʼ����������Դ��\n
* @param [IN] pcBaseModuleCode ����ƽ̨BPģ��ı��롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED          �ɹ�
* -         #ERR_COMMON_INVALID_PARAM    ��������Ƿ�
* -         #ERR_XP_FAIL_TO_REG_BP_CMD   ע�����ƽ̨BP��������ģ��IDʧ��
* -         #ERR_XP_FAIL_TO_INIT_XP      ��ʼ��������XP��Դʧ��
* @note
* - ʹ�ò����������������Ҫ���õĺ�����
*/
IMOS_EXPORT ULONG STDCALL IMOS_InitXP(IN const CHAR *pcBaseModuleCode);

/**
* �ͷŲ�������Դ��\n
* @param [IN] �ޡ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED          �ɹ�
* -         #ERR_XP_FAIL_TO_CLEANUP_XP   �ͷŲ�����XP��Դʧ��
* @note �˳���������������һ����Ҫ���ú�����
*/
IMOS_EXPORT ULONG STDCALL IMOS_CleanupXP();

/**
* ע�Ქ������\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pstXPInfo          ��������Ϣ�ṹ�塣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_REGIST_XP        ע�Ქ����XPʧ��
* @note �ޡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_RegisterXP(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const XP_INFO_S *pstXPInfo);

/**
* ע����������\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note �ޡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_UnregisterXP(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo);

/**
* ��Ӳ��Ŵ�����Դ���룬һ�ο���Ӷ�����Ŵ�����Դ���롣\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pstWndCode         ���Ŵ������ṹ��ָ�롣
* @param [IN] ulPlayWndNum       ���Ŵ���������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_EXCEED_MAX_PLAY_PORT_NUM ע��Ĳ���ͨ�����ѳ��������֧�ֵĲ���ͨ����  
* -         #ERR_XP_FAIL_TO_SET_PARAM        ���ò�����XP�����ò���ʧ��    
* @note һ�����Ŵ�����Դ�����Ӧһ������ͨ��������ͬһ���û���½ID��ͬʱ���֧�ֵĲ���ͨ����Ϊ128��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddPlayWndInfo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const PLAY_WND_INFO_S *pstWndCode,
                                              IN ULONG ulPlayWndNum);

/**
* ɾ�����Ŵ�����Դ���룬һ�ο�ɾ��������Ŵ�����Դ���롣\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pstWndCode         ���Ŵ������ṹ��ָ�롣
* @param [IN] ulPlayWndNum       ���Ŵ���������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note �ޡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeletePlayWndInfo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const PLAY_WND_INFO_S *pstWndCode,
                                                 IN ULONG ulPlayWndNum);

/**
* ��֡���˲��š�\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode      ����ͨ�����롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_REGISTER            ����ͨ��û��ע��
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý�����Ͳ�֧�ִ˲���
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       ��ǰ���ڱ���¼��
* -         #ERR_XP_FAIL_TO_STEP_PLAY            ��֡���˲���ʧ��
* @note ���ڱ���¼��ʱ����֧�ֵ�֡���˲��Ų�����
*/
IMOS_EXPORT ULONG STDCALL IMOS_OneByOneBack(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                            IN const CHAR *pcChannelCode);

/**
* ����ý����������ʽ��\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode      ����ͨ�����롣
* @param [IN] ulMediaDataformat  ý����������ʽ�����#XP_MEDIA_DATA_FORMAT_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_NOT_SUPPORT_MEDIA_STREAM_TYPE    ��֧�ֵ���������
* -         #ERR_XP_PORT_NOT_REGISTER                ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* @note �ޡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaDataFormat(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                  IN const CHAR *pcChannelCode,
                                                  IN ULONG ulMediaDataformat);

/**
* ����ָ��ͨ���ļ�ʱ�طš�\n
* @param [IN] pstUserLoginIDInfo       �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode            ����ͨ�����롣
* @param [IN] pcReplayChannelCode      ���Żط���Ƶ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER            ͨ��û��ע��
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_NOT_START_PLAY               ָ���Ĳ���ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò��Ŵ�����Դʧ��
* -         #ERR_XP_FAIL_TO_START_REALREPLAY     ��ʼ��ʱ�ط�ʧ��
* @note     pcReplayChannelCode ����Դ����ӦΪ��Ч��ͨ������
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartRealReplay(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,   
                                               IN const CHAR *pcReplayChannelCode);

/**
* ����ָ��ͨ���ļ�ʱ�ط�ʱ����\n
* @param [IN] pstUserLoginIDInfo       �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode            ����ͨ�����롣
* @param [IN] ulTime                   ��ʱ�ط�ʱ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER            ͨ��û��ע��
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_NOT_START_PLAY               ָ���Ĳ���ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò��Ŵ�����Դʧ��
* -         #ERR_XP_SET_REALREPLAY_TIME_LARGE    ��ʱ�ط�����ʱ��̫��            
* @note     ʱ��ȡֵ��Χ[0 - 60]s
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetRealReplayTime(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN ULONG ulTime);

/* BEGIN: Added by jinqifeng j06715, 2010/12/15   PN:VVD51508 */
/**
* ��������ý�����Ķ˿ڷ�Χ��\n
* @param [IN] usMinPort            ��ʼ�˿ڡ�
* @param [IN] usMaxPort            �����˿ڡ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* @note
* -    1����Χ�������õĶ˿ڡ�
* -    2���ڵ��� IMOS_InitXP֮ǰ���á�
* -    3����ξ�Ϊ��ʱ�򲻵���ʱ������Ĭ��ֵ��
* -    4��Ĭ��ֵ��ΧΪ32768��65535֮���ż����
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaPortRange(IN USHORT usMinPort, 
                                                 IN USHORT usMaxPort);

/**
* ��������Ķ˿ڷ�Χ��\n
* @param [IN] usMinPort         ��ʼ�˿ڡ�
* @param [IN] usMaxPort         �����˿ڡ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* @note
* -    1����Χ�������õĶ˿ڡ�
* -    2���ڵ��� IMOS_InitXP֮ǰ���á�
* -    3����ξ�Ϊ��ʱ�򲻵���ʱ������Ĭ��ֵ��
* -    4��Ĭ��ֵ��ΧΪ6001��10000��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetProtlPortRange(IN USHORT usMinPort, 
                                                 IN USHORT usMaxPort);
/* END:   Added by jinqifeng j06715, 2010/12/15 */

/**
* ��������ý�������ݣ������յ���ý�������ݻص����û���\n
* @param [IN]  pstUserLoginIDInfo       �û���¼ID��Ϣ��ʶ��
* @param [IN]  pcDownUrl                ������ý������Դ��URL��
* @param [IN]  pcServerIP               ý�����ط�������IP��
* @param [IN]  usServerPort             ý�����ط������Ķ˿ںš�
* @param [IN]  ulProtl                  ����ý������Э�����ͣ����tagProtocolö����ȡֵ��������Ŀǰ��֧��TCPЭ�飩��
* @param [IN]  ulDownMediaSpeed         ý�����������ٶȣ����tagDownMediaSpeedö����ȡֵ��
* @param [IN]  pfnSourceMediaDataCBFun  ����ý�������ݻص�������ָ�롣
* @param [IN]  lUserParam               �û����ò�����
* @param [OUT] pcDownloadID             ����ͨ���ţ�����������Ӧ��С��IMOS_RES_CODE_LEN��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_DOWNLOAD_PORT_NUM_LIMIT      ��������ͨ����
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         �������ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SETUP_PROTOCOL       ��������Э��ʧ��
* -         #ERR_XP_FAIL_TO_PLAY_PROTOCOL        ����Э�̲��ţ����أ�ʧ��
* -         #ERR_XP_FAIL_TO_OPEN_NET_MEDIA       ����ý�崦��ʧ��
* -         #ERR_XP_FAIL_TO_START_DOWNLOAD       ��������ʧ��
* -         #ERR_XP_RTSP_REQ_TIMEOUT             ����ҵ����ʱ
* -         #ERR_XP_RTSP_NOT_ENOUGH_BANDWIDTH    ����ҵ�������
* -         #ERR_XP_RTSP_SERVINTERERR            ����ҵ���ڲ�����
* @note
* -         1���ú������óɹ��󣬷��ظ��û�����ͨ���ţ����ڱ�ʶһ·����ͨ����
* -         2�����ͬʱ֧������32·����ͨ����
* -         3��Ŀǰ��֧��TCPЭ������ý������
* -         4��ֹͣý�������ر��������IMOS_StopDownMediaStreamCallback�ӿڡ�
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartDownMediaStreamCallback(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                            IN const CHAR *pcDownUrl,
                                                            IN const CHAR *pcServerIP,
                                                            IN USHORT usServerPort,
                                                            IN ULONG ulProtl,
                                                            IN ULONG ulDownMediaSpeed,
                                                            IN XP_SOURCE_MEDIA_DATA_CALLBACK_PF pfnSourceMediaDataCBFun, 
                                                            IN LONG lUserParam, 
                                                            OUT CHAR *pcDownloadID);

/**
* ֹͣ����ý�������ݡ�\n
* @param [IN]  pstUserLoginIDInfo  �û���¼ID��Ϣ��ʶ��
* @param [IN]  pcDownloadID        ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         �������ͨ����Դʧ��
* @note �ýӿ���IMOS_StartDownMediaStreamCallback�ӿ�ƥ�䡣
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopDownMediaStreamCallback(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                           IN const CHAR *pcDownloadID);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_ */



