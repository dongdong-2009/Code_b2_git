/*******************************************************************************
 Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
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

/* begin add by shiyouhua04562 ��ʱ�ط���Ŀ16������ */
/**
* ��ʼ���ţ���չ�ӿڣ�����ָ����ʼ����ʱ��Ͳ������ʲ�������
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode      ����ͨ�����롣
* @param [IN] pcTime             �����ļ�����λ�õ�ָ��ʱ�䣨ʱ���ʽΪ��yyyy-mm-dd hh:mm:ss����
* @param [IN] ulPlaySpeed        �������ʣ����tagPlayStatusö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER            ָ���Ĳ���ͨ��û��ע��
* -         #ERR_XP_PORT_NOT_OPEN                ָ���Ĳ���ͨ��δ����ý��ҵ��
* -         #ERR_XP_FAIL_TO_PLAY_MEDIA           ý�岥�Ŵ���ʧ��
* -         #ERR_XP_FAIL_TO_PLAY_PROTOCOL        ����Э�̲���ʧ��
* -         #ERR_XP_FAIL_TO_START_PLAY           ��ʼ����ʧ��
* @note
* 1���ýӿ����û����õ�¼��ʱ�������ָ�����ٲ��ŵ㲥�طţ����ڹ��������ʱ�طź�ͼƬOSD�ҵ����⡣
* 2��SDK�û��ڵ���IMOS_OpenVodStream֮����øýӿڣ����IMOS_StartPlay��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartPlayEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                           IN const CHAR *pcChannelCode,
                                           IN const char *pcTime,
                                           IN ULONG ulPlaySpeed);
/* end add by shiyouhua04562 ��ʱ�ط���Ŀ16������ */

/**
* ����ָ��ͨ�������ַŴ��ܡ�\n
* @param [IN] pstUserLoginIDInfo       �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode            ����ͨ�����롣
* @param [IN] hWnd                     ���л����,hWnd = IMOS_INVALID_HANDLE��ʾ�رգ���IMOS_INVALID_HANDLE��ʾ����
* @param [IN] pstRect                  ԭʼͼ���������꣬pstRect = NULLΪȫ����ʾ��������ֵΪ������ͼ���еİٷֱ�ֵ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_REGISTER            ����ͨ��û��ע��
* -         #ERR_XP_NOT_SUPPORT                  ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_DIGITALZOOM      ���õ��ӷŴ�ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDigitalZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN IMOS_HANDLE hWnd,
                                              IN XP_RECT_S *pstRect);

/**
* ��ȡͼ���С��\n
* @param [IN] pstUserLoginIDInfo       �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode            ����ͨ�����롣
* @param [OUT] pulWidth                ͼ��Ŀ�
* @param [OUT] pulHeight               ͼ��ĳ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_GET_PIC_PARAM        ��õ�ǰͨ�������ͼƬ������Ϣʧ��
* -         #ERR_COMMON_INVALID_PARAM            ��������
* @note
* - 1���ýӿں�����֧��Windows��
*/

IMOS_EXPORT ULONG STDCALL IMOS_GetPicSize(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const CHAR *pcChannelCode,
                                          OUT ULONG *pulWidth,
                                          OUT ULONG *pulHeight);

/**
* ����UDP�����������ܡ�\n
* @param [IN] bAdjust                  �Ƿ�������������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_SET_PARAM            ���ò����������ò���ʧ��
* @note
* -  1����ǰֻ֧��UDP+TS�е���Ƶ��������
* -  2���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_AdjustPktSeq(IN BOOL_T bAdjust);


/**
* ��ָ��ͨ����ͼ������������
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode            ����ͨ������
* @param [IN] pDrawHandle              ��ͼ���
* @param [IN] ulNum                    ���������򶥵���������Ϊ6
* @param [IN] pointArry                �����ַ
* @param [IN] ulDir                    �������#IMOS_AREA_DIRECTION_Eö����ȡֵ��
* @param [IN] ulColor                  ������ɫ
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT VOID STDCALL IMOS_PolylineTo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                         IN const CHAR *pcChannelCode,
                                         IN IMOS_HANDLE pDrawHandle,
                                         IN ULONG ulNum,
                                         IN XP_POINT_S *pointArry,
                                         IN ULONG ulDir,
                                         IN ULONG ulColor);

/**
* ��ָ��ͨ����ͼ����Ӱ�ߣ�
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] pDrawHandle              ��ͼ���
* @param [IN] pstStart                 ֱ�����
* @param [IN] pstEnd                   ֱ���յ�
* @param [IN] ulDir                    ��������,���#IMOS_THRU_LINE_DIRECTION_Eö����ȡֵ��
* @param [IN] ulColor                  ������ɫ
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT VOID STDCALL IMOS_DLineTo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                      IN const CHAR *pcChannelCode,
                                      IN IMOS_HANDLE pDrawHandle,
                                      IN const XP_POINT_S *pstStart,
                                      IN const XP_POINT_S *pstEnd,
                                      IN ULONG ulDir,
                                      IN ULONG ulColor);
/**
* ��ָ��ͨ����ͼ����Ӱ�ߣ�
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] pDrawHandle              ��ͼ���
* @param [IN] pstStart                 ֱ�����
* @param [IN] pstEnd                   ֱ���յ�
* @param [IN] ulDir                    ��������,���#IMOS_THRU_LINE_DIRECTION_Eö����ȡֵ��
* @param [IN] ulColor                  ������ɫ
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��IMOS_DLineToָ�굥λΪ1%, IMOS_DLineToEx���굥λΪ0.01%
*/
IMOS_EXPORT VOID STDCALL IMOS_DLineToEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                        IN const CHAR *pcChannelCode,
                                        IN IMOS_HANDLE pDrawHandle,
                                        IN const XP_POINT_S *pstStart,
                                        IN const XP_POINT_S *pstEnd,
                                        IN ULONG ulDir,
                                        IN ULONG ulColor);

/**
* ��ȡ��ͼ���
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @return �������½����
* - �ɹ���IMOS_HANDLE
* - #
* - ʧ�ܣ�IMOS_INVALID_HANDLE
* -
* @note
* - 1����Ҫ����Ƶ����ͼ��������Ҫ����IMOS_GetHVDC��ȡHVDC���ſ�����ͼ����ʾ������IMOS_ReleaseHVDCȡ����ͼ������ʾ
* - 2���ýӿں�����֧��Windows��
*/
IMOS_EXPORT IMOS_HANDLE STDCALL IMOS_GetHVDC(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                             IN const CHAR *pcChannelCode);

/**
* �ͷŻ�ͼ���
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] pDrawHandle              ͨ��IMOS_GetHVDC��ȡ�ľ��
* @return �������½����
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT VOID STDCALL IMOS_ReleaseHVDC(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const CHAR *pcChannelCode,
                                          IN IMOS_HANDLE pDrawHandle);
/**
* ��ʼ��ͼ
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ�����롣
* @param [IN] pDrawHandle              ��ͼ���
* @param [IN] pverBer                  ��������
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER           ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_BEGINHVDC           ��ʼ��ͼʧ��
* @note
* - 1��ͼ���ϻ��ߣ�������Ҫ��IMOS_BeginDraw��IMOS_EndDraw֮�����������������Ч
* - 2���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_BeginDraw(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                         IN const CHAR *pcChannelCode,
                                         IN IMOS_HANDLE pDrawHandle,
                                         IN VOID *pverBer);

/**
* �����ͼ
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] pDrawHandle              ��ͼ���
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT VOID STDCALL IMOS_EndDraw(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                      IN const CHAR *pcChannelCode,
                                      IN IMOS_HANDLE pDrawHandle);

/**
* ���ý�������ܹ�����ʾʹ��
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] ulMethod                 ����������Ϣ���ӷ�����Ϣ
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER           ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERULE   ������ʾ���ܹ���ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShowAnalyseRule(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const BOOL_T bEnable );

/**
* ���ý�������ܽ����ʾʹ��
* @param [IN] pstUserLogIDInfo         �û���¼ID��Ϣ��ʶ
* @param [IN] pstChannelIndex          ����ͨ������
* @param [IN] ulMethod                 ����������Ϣ���ӷ�����Ϣ
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER           ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERESULT ������ʾ���ܽ��ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShowAnalyseResult(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN const BOOL_T bEnable );
/**
* �������ϵͳ��Ӳ������
* @param [IN] �ޡ�
* @return ����#XP_HW_STATUS_Eö�����͵Ĳ���
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_CheckHWValidate();

/**
* ����ͼ��ƴ�Ӳ�����\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͼ��ƴ����Ƶ��ͨ������
* @param [IN] pcCombineResCode          ͼ��ƴ����Դ����
* @param [IN] lCombineStreamNum         ͼ��ƴ�ӵ�ͨ����Ŀ
* @param [IN] ppcNeedCombineChannelCode ͼ��ƴ�ӵ�ͨ����������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      ����ͼ��ƴ�ӳɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_REGISTER                ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_START_COMBINE_IMAGE      ����ͼ��ƴ�Ӳ���ʧ��
* @note
* - 1����ǰֻ֧��3��ͨ������Դƴ��
* - 2��ƴ��ǰ��ҪԤ�ȴ���Ҫƴ�ӵ���Ƶ����ҵ��
* - 3��ppcNeedCombineChannelCodeΪ����
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCombineVideo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN const CHAR *pcCombineResCode,
                                                 IN const LONG lCombineStreamNum,
                                                 IN const CHAR **ppcNeedCombineChannelCode);

/**
* ֹͣͼ��ƴ�Ӳ�����\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ͼ��ƴ����Դ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_IMAGE   ֹͣͼ��ƴ�Ӳ���ʧ��
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCombineVideo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcChannelCode);

/**
* �궨ͼ��ƴ�������㡣\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcCombineResCode          ͼ��ƴ����Դ����
* @param [IN] lCombineStreamNum         ͼ��ƴ����Դͨ����Ŀ
* @param [IN] plCombinPointNum          ����ͼ����������Ŀ������
* @param [IN] plCombinPointVal          �û�����ƴ��ͼ��������ֵ����
* @param [IN] lReserve                  ����ֵ��������ƴ��ģʽʹ�ã���ǰ��д0
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_MARK_COMBINE_IMAGE   �궨������ʧ��
* @note
* - ��ʶΪ�����ֵ��Ҫ�������
*/
IMOS_EXPORT ULONG STDCALL IMOS_MarkCombinePoint(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcCombineResCode,
                                                IN LONG lCombineStreamNum,
                                                IN LONG *plCombinPointNum,
                                                IN LONG *plCombinPointVal,
                                                IN LONG lReserve);

/**
* ������Ƶͼ����ӷŴ��ܡ�\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ���ŵ��ӷŴ���Ƶ��ͨ������
* @param [IN] pcCombineResCode          ͼ��ƴ����Դ����
* @param [IN] pstCombineZoomRect        ���ӷŴ�������Ϣ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_START_COMBINE_ZOOM   �������ӷŴ�ʧ��
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcChannelCode,
                                                IN const CHAR *pcCombineResCode,
                                                IN const XP_COMBINE_ZOOM_RECT_S *pstCombineZoomRect);

/**
* �޸ĵ��ӷŴ�������Ϣ��\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ���ŵ��ӷŴ���Ƶ��ͨ������
* @param [IN] pcCombineResCode          ͼ��ƴ����Դ����
* @param [IN] pstCombineZoomRect        ���ӷŴ�������Ϣ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_MOVE_COMBINE_ZOOM    �޸ĵ��ӷŴ�������Ϣʧ��
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_MoveCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const CHAR *pcCombineResCode,
                                               IN const XP_COMBINE_ZOOM_RECT_S *pstCombineZoomRect);

/**
* �ر���Ƶͼ����ӷŴ��� \n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ���ŵ��ӷŴ���Ƶ��ͨ������
* @param [IN] pcCombineResCode          ͼ��ƴ����Դ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_ZOOM    �رյ��ӷŴ���ʧ��
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const CHAR *pcCombineResCode);



/* begin add by shiyouhua04562 ʡ����Ŀ�����أ� */
/**
* �ж��Ƿ�ΪDB33�����׼���롣\n
* @param [IN] pcDecodetag    decodetag��Ϣ
* @return BOOL_TRUE����DB33��������׼���룻 BOOL_FALSE����DB33��������׼����
* @note ��
*/
IMOS_EXPORT BOOL_T STDCALL IMOS_IsDB33Decodetag(IN const CHAR *pcDecodetag);
/* end add by shiyouhua04562 ʡ����Ŀ�����أ� */

/**
* ���ý���ģʽ \n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ���ŵ�ͨ������
* @param [IN] ulMode                    ����ģʽ 0 �������ģʽ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* @note
* - 1���ýӿں�����֧��Windows��
* - 2���ýӿ�Ϊʵ���Խӿڣ��ᵼ�³����ȶ����½������Ƽ��û�ʹ�á�
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoDecodeMode(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN ULONG ulMode);

/**
* Ӱ�����\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͨ������
* @param [IN] pstAdjust                 Ӱ����ڲ���
* @param [IN] bOpen                     �Ƿ��Ӱ�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER           ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_ADJUST    ����Ӱ�����ʧ��
* @note
* -    1��Ĭ�ϲ�����Ӱ����ڡ�
* -    2���Աȶ�ȡֵ��ΧΪ[0--199]��Ĭ��ֵΪ100��
* -       ����ȡֵ��ΧΪ[0--199]��Ĭ��ֵΪ100��
* -       ɫ��ȡֵ��ΧΪ[0--359]��Ĭ��ֵΪ0��
* -       ���Ͷ�ȡֵ��ΧΪ[0--359]��Ĭ��ֵΪ100��
* -       ٤��ȡֵ��ΧΪ[0--99]��Ĭ��ֵΪ10��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoAdjust(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN const XP_VIDEO_ADJUST_S *pstAdjust,
                                              IN BOOL_T bOpen);

/**
* ģ������\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͨ������
* @param [IN] ulFactor                  ģ���������
* @param [IN] bOpen                     �Ƿ�����ģ������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER               ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_MONITOR_BLUR  ����ģ������ʧ��
* @note
* -    1��Ĭ�ϲ�����ģ������
* -    2��ģ���������ȡֵ��ΧΪ[1--127]��Ĭ��ֵΪ80��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoMotionBlur(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                  IN const CHAR *pcChannelCode,
                                                  IN ULONG ulFactor,
                                                  IN BOOL_T bOpen);

/**
* �񻯴���\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͨ������
* @param [IN] fSigma                    �񻯴������
* @param [IN] bOpen                     �Ƿ������񻯴���
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER               ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_SHARPEN       �����񻯴���ʧ��
* @note
* -    1��Ĭ�ϲ������񻯴���
* -    2���񻯴������ȡֵ��ΧΪ[0.0--2.0]��Ĭ��ֵΪ0.05��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoSharpen(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN FLOAT fSigma,
                                               IN BOOL_T bOpen);

/**
* ȥ����\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͨ������
* @param [IN] ulLowValue                �������ڲ���
* @param [IN] ulHighValue               �������ڲ���
* @param [IN] bOpen                     �Ƿ�����ȥ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER               ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_DE_FOG              ����ȥ����ʧ��
* @note
* -    1��Ĭ�ϲ�����ȥ����
* -    2��ȥ�������1ȡֵ��ΧΪ[0--49]��Ĭ��ֵΪ0��
* -       ȥ�������2ȡֵ��ΧΪ[51--100]��Ĭ��ֵΪ100��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDeFog(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                        IN const CHAR *pcChannelCode,
                                        IN ULONG ulLowValue,
                                        IN ULONG ulHighValue,
                                        IN BOOL_T bOpen);

/**
* �����������������ƴ֡���ԡ�\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] ulDataLostSplitterPolicy �������������ƴ֡���ԣ����#XP_MEDIA_DATA_LOST_SPLITTER_POLICYö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* @note
* - 1���������������Ĭ�ϲ�����ƴ֡
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaDataLostSplitterPolicy(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                            IN ULONG ulDataLostDecodePolicy);

/**
* ����˸����\n
* @param [IN] pstUserLoginIDInfo        �û���¼ID��Ϣ��ʶ
* @param [IN] pcChannelCode             ����ͨ������
* @param [IN] ulWinSize                 �ο�֡��
* @param [IN] ulSoftenVal               ֡���˲�����
* @param [IN] bOpen                     �Ƿ���������˸������
* @return �������´����룺
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_PORT_NOT_REGISTER               ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_ANTI_FLICKER        ���÷���˸����ʧ��
* @note 
* -    1��Ĭ�ϲ�����ȥ����
* -    2��ulWinSizeȡֵ��ΧΪ[0--100]��
* -       ulSoftenValȡֵ��ΧΪ[0--30]��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetAntiFlicker(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN ULONG ulWinSize, 
                                              IN ULONG ulSoftenVal,
                                              IN BOOL_T bOpen);

/**
* ����ͼ��ĶԱȶȲ�����\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode      ����ͨ�����롣
* @param [IN] ulContrastMode     ͼ��Աȶ�ģʽ�����#XP_PICTURE_CONTRAST_MODE_Eö����ȡֵ��
* @param [IN] ulContrastLevelEx  ͼ��Աȶȼ������#XP_PICTURE_CONTRAST_LEVEL_EX_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_PORT_NOT_REGISTER        ����ͨ��û��ע��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_SUPPORT_SET_CONTRAST ����ͨ����֧������ͼ��ԱȶȲ���
* -         #ERR_XP_FAIL_TO_SET_CONTRAST     ����ͼ��ԱȶȲ���ʧ��
* @note
* -    1���ڲ����øýӿڵ�����£���Ƶͼ��Ĭ�ϲ����жԱȶȵ��ڴ���
* -    2�������ڿ�ʼ����IMOS_StartPlay֮ǰ���ã������õĶԱȶȲ��������棬��Ϊ������ͨ������ʱ
* -       �ĳ�ʼֵ���ڿ�ʼ���ź���øýӿ����õĶԱȶȲ�����ʱ�����á�
* -    3�����øýӿڶ�ָ������ͨ�����õĶԱȲ����ڲ����������ڼ�һֱ��Ч��������ulContrastMode
* -       ����XP_PICTURE_CONTRAST_MODE_NONEʱ����ֹͣ�Աȶȵ��ڴ�����ʱulContrastLevel�����
* -       ֵ��Ч��
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetPictureContrastEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                    IN const CHAR *pcChannelCode,
                                                    IN ULONG ulContrastMode,
                                                    IN ULONG ulContrastLevelEx);

/**
* ����ָ��ץ����Ŀ������ʵʱץ�ġ�\n
* @param [IN] pstUserLoginIDInfo �û���¼ID��Ϣ��ʶ��
* @param [IN] pcChannelCode      ����ͨ�����롣
* @param [IN] pcFileName         ͼ���ļ���ŵ�·���������ļ�����������չ������
* @param [IN] ulPicFormat        ͼ���ļ��ĸ�ʽ�����#XP_PICTURE_FORMAT_Eö����ȡֵ��
* @param [IN] ulInterval         ץ�ļ������λΪ���롣
* @param [IN] ulCount            ץ����Ŀ��ȡֵ����0����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_REGISTER            ����ͨ��û��ע��
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH     Ӳ�̿ռ䲻��
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_ALREADY_START_SERIES_SNATCH  ����ͨ���Ѿ�����������ץ�ĵĲ���
* -         #ERR_XP_NOT_SUPPORT_SNATCH_FORMAT    ץ�ĸ�ʽ��֧��
* -         #ERR_XP_FAIL_TO_START_SERIES_SNATCH  ��������ץ�Ĳ���ʧ��
* @note
* - 1�������������ļ������渽�ӡ�ץ��ϵͳʱ��-ϵͳ���뼶ʱ�䡱��ʽ��ʱ����ַ������硰20110228110325-1997060991����
* - 2����λʱ���ڳɹ�ץ�ĵ����ͼƬ���ͻ��������йأ�
* - 3���������������յ�ͼ���ļ����󸽼��Ϻ�׺����
* - 4����������ץ�ĺ���ץ�Ĺ����У�������ͨ��#IMOS_SetRunMsgCB���õĻص������ϱ�ץ����Ϣ#XP_RUN_INFO_SERIES_SNATCH��Я���Ĵ����룺
* -     #ERR_XP_DISK_CAPACITY_WARN          �������ڼ���ץ��
* -     #ERR_XP_DISK_CAPACITY_NOT_ENOUGH    �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -     #ERR_XP_NO_PICTURE_TO_SNATCH        �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -     #ERR_XP_FAIL_TO_SERIES_SNATCH       �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -     #ERR_XP_FINISH_SERIES_SNATCH        �ϱ���Ϣ֮ǰ��������ֹͣץ��
* - 5���ýӿں�����֧��Windows��
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartSnatchSeriesEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                   IN const CHAR *pcChannelCode,
                                                   IN const CHAR *pcFileName,
                                                   IN ULONG ulPicFormat,
                                                   IN ULONG ulInterval,
                                                   IN ULONG ulCount);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_ */



