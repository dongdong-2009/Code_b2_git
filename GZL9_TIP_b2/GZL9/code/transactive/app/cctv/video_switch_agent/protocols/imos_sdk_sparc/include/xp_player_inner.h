/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
xp_player_inner.h

Project Code: IMOSͨ��ý�岥����
Module Name: IMOSͨ��ý�岥����
Date Created: 2012-03-31
Author: jinqifeng 00350
Description: IMOSͨ��ý�岥����XP�ڲ��ӿ�

--------------------------------------------------------------------------------
Modification History
DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_XP_PLAYER_INNER_H_
#define _IMOS_XP_PLAYER_INNER_H_

/* ����IMOS����ƽ̨��ͷ�ļ� */
#include "imos_public.h"
#include "imos_errcode.h"
#include "imos_def.h"

/* ����������SDK���ݽṹͷ�ļ� */
#include "xp_datatype.h"
#include "xp_player.h"

#ifdef  __cplusplus
extern "C"{
#endif

/**
* ����ָ��ץ����Ŀ������ʵʱץ�ġ�\n
* @param [IN] ulPort             ����ͨ���š�
* @param [IN] pcFileName         ͼ���ļ���ŵ�·���������ļ�����������չ������
* @param [IN] ulPicFormat        ͼ���ļ��ĸ�ʽ�����#XP_PICTURE_FORMAT_Eö����ȡֵ��
* @param [IN] ulInterval         ץ�ļ������λΪ���롣
* @param [IN] ulCount            ץ����Ŀ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH     Ӳ�̿ռ䲻��
* -         #ERR_XP_NOT_START_PLAY               ָ���Ĳ���ͨ��δ��ʼ���Ų���
* -         #ERR_XP_ALREADY_START_SERIES_SNATCH  ����ͨ���Ѿ�����������ץ�ĵĲ���
* -         #ERR_XP_NOT_SUPPORT_SNATCH_FORMAT    ץ�ĸ�ʽ��֧��
* -         #ERR_XP_FAIL_TO_START_SERIES_SNATCH  ��������ץ�Ĳ���ʧ��
* @note
* -
* -    1��������XP�����ļ������渽��������"20080928(13-10-50-99)"��ʽ��ץ��ʱ����ַ�����
* -       ��λʱ���ڳɹ�ץ�ĵ����ͼƬ���ͻ��������йأ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartSnatchSeriesEx(IN ULONG ulPort,
                                                                IN const CHAR *pcFileName,
                                                                IN ULONG ulPicFormat,
                                                                IN ULONG ulInterval,
                                                                IN ULONG ulCount);

/**
* ����ָ��ͨ�������ַŴ��ܡ�\n
* @param [IN] ulPort                   ����ͨ����
* @param [IN] hWnd                     ���л����,hWnd = IMOS_INVALID_HANDLE��ʾ�رգ���IMOS_INVALID_HANDLE��ʾ����
* @param [IN] pstRect                  ԭʼͼ���������꣬pstRect = NULLΪȫ����ʾ��������ֵΪ������ͼ���еİٷֱ�ֵ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_SUPPORT                  ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_DIGITALZOOM      ���õ��ӷŴ�ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDigitalZoom(IN ULONG ulPort,
                                                           IN IMOS_HANDLE hWnd,
                                                           IN XP_RECT_S *pstRect);

/**
* ��ȡָ��ͨ����ͼƬ��Ϣ��\n
* @param [IN] ulPort                   ����ͨ����
* @param [OUT] pulWidth                ͼƬ�Ŀ�
* @param [OUT] pulHeight               ͼƬ�ĸ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_GET_PIC_PARAM        ��õ�ǰͨ�������ͼƬ������Ϣʧ��
* -         #ERR_COMMON_INVALID_PARAM            ��������
* @note
* - 1���ýӿں�����֧��Windows��
*/

IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetPicSize(IN ULONG ulPort,
                                                       OUT ULONG *pulWidth,
                                                       OUT ULONG *pulHeight);

/**
* �����Ƿ��������������ܡ�\n
* @param [IN] bAdjust �Ƿ�������������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_SET_PARAM            ���ò����������ò���ʧ��
* @note
* -  1����ǰֻ֧��UDP+TS�е���Ƶ��������
* -  2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_AdjustPktSeq(IN BOOL_T bAdjust);

/**
* ��ָ��ͨ����ͼ������������
* @param [IN] ulPort             ����ͨ����
* @param [IN] pDrawHandle        ��ͼ���
* @param [IN] ulNum              ���������򶥵���������Ϊ6
* @param [IN] pointArry          �����ַ
* @param [IN] ulDir              ����,���#XP_AREA_DIRECTION_Eö����ȡֵ��
* @param [IN] ulColor            ������ɫ
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_PolylineTo(IN ULONG ulPort,
                                                      IN IMOS_HANDLE pDrawHandle,
                                                      IN ULONG ulNum,
                                                      IN XP_POINT_S *pointArry,
                                                      IN ULONG ulDir,
                                                      IN ULONG ulColor);

/**
* ��ָ��ͨ����ͼ����Ӱ�ߣ�
* @param [IN] ulPort             ����ͨ����
* @param [IN] pDrawHandle        ��ͼ���
* @param [IN] pstStart           ֱ�����
* @param [IN] pstEnd             ֱ���յ�
* @param [IN] ulDir��            ��������,���#XP_THRU_LINE_DIRECTION_Eö����ȡֵ
* @param [IN] ulColor            ������ɫ
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_DLineTo(IN ULONG ulPort,
                                                   IN IMOS_HANDLE pDrawHandle,
                                                   IN const XP_POINT_S *pstStart,
                                                   IN const XP_POINT_S *pstEnd,
                                                   IN ULONG ulDir,
                                                   IN ULONG ulColor);

/**
* ��ȡ��ͼ���
* @param [IN] ulPort             ����ͨ����
* @return �������½����
* - �ɹ���IMOS_HANDLE
* - #
* - ʧ�ܣ�IMOS_INVALID_HANDLE
* -
* @note
* - 1����Ҫ����Ƶ����ͼ��������Ҫ����IMOS_GetHVDC��ȡHVDC���ſ�����ͼ����ʾ������IMOS_ReleaseHVDCȡ����ͼ������ʾ
* - 2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT IMOS_HANDLE STDCALL IMOS_XP_GetHVDC(IN ULONG ulPort);

/**
* �ͷŻ�ͼ���
* @param [IN] ulPort             ����ͨ����
* @param [IN] pDrawHandle        ͨ��IMOS_GetHVDC��ȡ�ľ��
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_ReleaseHVDC(IN ULONG ulPort,
                                                       IN IMOS_HANDLE pDrawHandle);
/**
* ��ʼ��ͼ
* @param [IN] ulPort                   ����ͨ����
* @param [IN] pDrawHandle              ��ͼ���
* @param [IN] pverBer                  ��������
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_BEGINHVDC           ��ʼ��ͼʧ��
* @note
* - 1����ͼ���ϻ��ߣ�������Ҫ��#IMOS_BeginDraw��#IMOS_EndDraw ֮�����������������Ч
* - 2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_BeginDraw(IN ULONG ulPort,
                                                      IN IMOS_HANDLE pDrawHandle,
                                                      IN VOID *pverBer);

/**
* �����ͼ
* @param [IN] ulPort             ����ͨ����
* @param [IN] pDrawHandle        ��ͼ���
* @return ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_EndDraw(IN ULONG ulPort,
                                                   IN IMOS_HANDLE pDrawHandle);

/**
* ���ý�������ܹ�����ʾʹ��
* @param [IN] ulPort             ����ͨ����
* @param [IN] bEnable            �Ƿ���ʾ
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERULE   ������ʾ���ܹ���ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ShowAnalyseRule(IN ULONG ulPort,
                                                            IN const BOOL_T bEnable);

/**
* ���ý�������ܽ����ʾʹ��
* @param [IN] ulPort        ����ͨ����
* @param [IN] bEnable       �Ƿ���ʾ
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERESULT ������ʾ���ܽ��ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/

IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ShowAnalyseResult(IN ULONG ulPort,
                                                              IN const BOOL_T bEnable);

/**
* Ӱ�����\n
* @param [IN] ulPort        ����ͨ����
* @param [IN] pstAdjust     Ӱ����ڲ���
* @param [IN] bOpen         �Ƿ��Ӱ�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY              ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                 ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_ADJUST    ����Ӱ�����ʧ��
* @note
*      1���ú�����Ҫ��IMOS_XP_StartPlay֮���IMOS_XP_StopPlay֮ǰ���á�
* -    2��Ĭ�ϲ�����Ӱ����ڡ�
* -    3���Աȶ�ȡֵ��ΧΪ[0--199]��Ĭ��ֵΪ100��
* -       ����ȡֵ��ΧΪ[0--199]��Ĭ��ֵΪ100��
* -       ɫ��ȡֵ��ΧΪ[0--359]��Ĭ��ֵΪ0��
* -       ���Ͷ�ȡֵ��ΧΪ[0--359]��Ĭ��ֵΪ100��
* -       ٤��ȡֵ��ΧΪ[0--99]��Ĭ��ֵΪ10��
* -    4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoAdjust(IN ULONG ulPort,
                                                           IN const XP_VIDEO_ADJUST_S *pstAdjust,
                                                           IN BOOL_T bOpen);

/**
* ģ������\n
* @param [IN] ulPort        ����ͨ����
* @param [IN] ulFactor      ģ���������
* @param [IN] bOpen         �Ƿ�����ģ������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_MONITOR_BLUR  ����ģ������ʧ��
* @note
* -    1���ú�����Ҫ��IMOS_XP_StartPlay֮���IMOS_XP_StopPlay֮ǰ���á�
* -    2��Ĭ�ϲ�����ģ������
* -    3��ģ���������ȡֵ��ΧΪ[1--127]��Ĭ��ֵΪ80��
* -    4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoMotionBlur(IN ULONG ulPort,
                                                               IN ULONG ulFactor,
                                                               IN BOOL_T bOpen);

/**
* �񻯴���\n
* @param [IN] ulPort        ����ͨ����
* @param [IN] fSigma        �񻯴������
* @param [IN] bOpen         �Ƿ������񻯴���
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_VIDEO_SHARPEN       �����񻯴���ʧ��
* @note
* -    1���ú�����Ҫ��IMOS_XP_StartPlay֮���IMOS_XP_StopPlay֮ǰ���á�
* -    2��Ĭ�ϲ������񻯴���
* -    3���񻯴������ȡֵ��ΧΪ[0.0--2.0]��Ĭ��ֵΪ0.05��
* -    4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoSharpen(IN ULONG ulPort,
                                                            IN FLOAT fSigma,
                                                            IN BOOL_T bOpen);

/**
* ȥ����\n
* @param [IN] ulPort            ����ͨ����
* @param [IN] ulLowValue        �������ڲ���
* @param [IN] ulHighValue       �������ڲ���
* @param [IN] bOpen             �Ƿ�����ȥ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_DE_FOG              ����ȥ����ʧ��
* @note
* -    1���ú�����Ҫ��IMOS_XP_StartPlay֮���IMOS_XP_StopPlay֮ǰ���á�
* -    2��Ĭ�ϲ�����ȥ����
* -    3��ȥ�������1ȡֵ��ΧΪ[0--49]��Ĭ��ֵΪ0��
* -       ȥ�������2ȡֵ��ΧΪ[51--100]��Ĭ��ֵΪ100��
* -    4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDeFog(IN ULONG ulPort,
                                                     IN ULONG ulLowValue,
                                                     IN ULONG ulHighValue,
                                                     IN BOOL_T bOpen);

/**
* �����������������ƴ֡���ԡ�\n
* @param [IN] ulDataLostSplitterPolicy  �������������ƴ֡���ԣ����#XP_MEDIA_DATA_LOST_SPLITTER_POLICYö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* @note
* -    1���������������Ĭ�϶���һ��I֡��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetMediaDataLostSplitterPolicy(IN ULONG ulDataLostSplitterPolicy);

/**
* ����˸����\n
* @param [IN] ulPort            ����ͨ����
* @param [IN] ulWinSize         �ο�֡��
* @param [IN] ulSoftenVal       ֡���˲�����
* @param [IN] bOpen             �Ƿ���������˸����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                  ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT                     ��֧�ָù���
* -         #ERR_XP_FAIL_TO_SET_ANTI_FLICKER        ���÷���˸����ʧ��
* @note 
* -    1���ú�����Ҫ��IMOS_XP_StartPlay֮���IMOS_XP_StopPlay֮ǰ���á�
* -    2��Ĭ�ϲ���������˸����
* -    3��ulWinSizeȡֵ��ΧΪ[0--100]��
* -       ulSoftenValȡֵ��ΧΪ[0--30]��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetAntiFlicker(IN ULONG ulPort,
                                                           IN ULONG ulWinSize, 
                                                           IN ULONG ulSoftenVal,
                                                           IN BOOL_T bOpen);

/**
* ����ͼ��ĶԱȶȲ�����\n
* @param [IN] ulPort             ����ͨ���š�
* @param [IN] ulContrastMode     ͼ��Աȶ�ģʽ�����#XP_PICTURE_CONTRAST_MODE_Eö����ȡֵ��
* @param [IN] ulContrastLevelEx  ͼ��Աȶȼ������#XP_PICTURE_CONTRAST_LEVEL_EX_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_SUPPORT_SET_CONTRAST ����ͨ����֧������ͼ��ԱȶȲ���
* -         #ERR_XP_FAIL_TO_SET_CONTRAST     ����ͼ��Աȶ�ʧ��
* @note
* -    1���ڲ����øýӿڵ�����£���Ƶͼ��Ĭ�ϲ����жԱȶȵ��ڴ���
* -    2�������ڿ�ʼ����IMOS_XP_StartPlay֮ǰ���ã������õĶԱȶȲ��������棬��Ϊ������ͨ������ʱ
* -       �ĳ�ʼֵ���ڿ�ʼ���ź���øýӿ����õĶԱȶȲ�����ʱ��Ч��
* -    3�����øýӿڶ�ָ������ͨ�����õĶԱȲ����ڲ����������ڼ�һֱ��Ч��������ulContrastMode
* -       ����XP_PICTURE_CONTRAST_MODE_NONEʱ����ֹͣ�Աȶȵ��ڴ�����ʱulContrastLevel������
* -       ���ֵ��Ч��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPictureContrastEx(IN ULONG ulPort,
                                                                 IN ULONG ulContrastMode,
                                                                 IN ULONG ulContrastLevelEx);

/**
* ��֡���˲��š�\n
* @param [IN] ulPort   ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       ��ǰ���ڱ���¼��
* -         #ERR_XP_FAIL_TO_STEP_PLAY            ��֡����ʧ��
* @note
* -    1�����ڱ���¼��ʱ����֧�ֵ�֡���˲��Ų�����
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OneByOneBack(IN ULONG ulPort);

/**
* �������ϵͳ��Ӳ������
* @param [IN] �ޡ�
* @return ����#XP_HW_STATUS_Eö�����͵Ĳ���
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CheckHWValidate();

/**
* ����ͼ��ƴ�Ӳ���
* @param [IN] ulChannelID                   ����ͼ��ƴ�ӵ�ͨ����
* @param [IN] pcCombineResCode              ͼ��ƴ����Դ����
* @param [IN] ulCombineStreamNum             ͼ��ƴ�ӵ�ͨ����Ŀ
* @param [IN] pulCombineStreamChannelID     ͼ��ƴ��ͨ������
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_START_COMBINE_IMAGE ����ͼ��ƴ�Ӳ���ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartCombineVideo(IN const ULONG ulChannelID,
                                                              IN const CHAR  *pcCombineResCode,
                                                              IN const ULONG ulCombineStreamNum,
                                                              IN const ULONG *pulCombineStreamChannelID);
/**
* ֹͣͼ��ƴ�Ӳ���
* @param [IN] ulChannelID           ����ͼ��ƴ�ӵ�ͨ����
* @return �������½����
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_IMAGE  ֹͣͼ��ƴ�Ӳ���ʧ��
* @note
* - 1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopCombineVideo(IN const ULONG ulChannelID);

/**
* �궨ͼ��ƴ��������
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
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_MarkCombinePoint(IN const CHAR  *pcCombineResCode,
                                                             IN LONG  lCombineStreamNum,
                                                             IN LONG  *plCombinPointNum,
                                                             IN LONG  *plCombinPointVal,
                                                             IN LONG  lReserve);

/**
* ������Ƶͼ����ӷŴ��ܡ�\n
* @param [IN] ulChannelID           ���ŵ��ӷŴ��ͨ����
* @param [IN] pcCombineResCode      ͼ��ƴ����Դ����
* @param [IN] pstComobineZoomRect   ���ӷŴ�������Ϣ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_START_COMBINE_ZOOM   �������ӷŴ�ʧ��
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartCombineZoom(IN const ULONG ulChannelID,
                                                             IN const CHAR  *pcCombineResCode,
                                                             IN const XP_COMBINE_ZOOM_RECT_S *pstComobineZoomRect);

/**
* �޸ĵ��ӷŴ�������Ϣ��\n
* @param [IN] ulChannelID           ���ŵ��ӷŴ��ͨ����
* @param [IN] pcCombineResCode      ͼ��ƴ����Դ����
* @param [IN] pstComobineZoomRect   ���ӷŴ�������Ϣ
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_MOVE_COMBINE_ZOOM    �޸ĵ��ӷŴ�������Ϣʧ��
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_MoveCombineZoom(IN const ULONG ulChannelID,
                                                            IN const CHAR  *pcCombineResCode,
                                                            IN const XP_COMBINE_ZOOM_RECT_S *pstComobineZoomRect);


/**
* �ر���Ƶͼ����ӷŴ��ܡ�\n
* @param [IN] ulChannelID           ���ŵ��ӷŴ��ͨ����
* @param [IN] pcCombineResCode      ͼ��ƴ����Դ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_ZOOM    �رյ��ӷŴ���ʧ��
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopCombineZoom(IN const ULONG ulChannelID,
                                                            IN const CHAR  *pcCombineResCode);

/**
* �ж��Ƿ�ΪDB33�����׼���롣\n
* @param [IN] pcDecodetag    decodetag��Ϣ
* @return BOOL_TRUE����DB33��������׼���룻 BOOL_FALSE����DB33��������׼����
* @note ��
*/
IMOS_XP_PLAYER_EXPORT BOOL_T STDCALL IMOS_XP_IsDB33Decodetag(IN const CHAR *pcDecodetag);

/**
* ���ý���ģʽ \n
* @param [IN] ulChannelID        �û���¼ID��Ϣ��ʶ
* @param [IN] ulMode                    ����ģʽ 0 �������ģʽ
* @return
* @note
* - 1���ýӿں�����֧��Windows��
* - 2���ýӿ�Ϊʵ���Խӿڣ��ᵼ�³����ȶ����½������Ƽ��û�ʹ�á�
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoDecodeMode(IN const ULONG ulChannelID,
                                                               IN ULONG ulMode);

/**
* ��ȡ���ж˿ںš�\n
* @param [INOUT] pulPort    ����ͨ����                       
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ȡ��Դʧ��
* @note ��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetPort(INOUT ULONG *pulPort);

/**
* �ͷŶ˿ںš�\n
* @param [IN] ulPort    ����ͨ����                       
* @return ��
* @note ��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_FreePort(IN ULONG ulPort);


/**
* ����ͨ����ԭʼ������Ϣ��\n
* @param [IN] ulPort        ����ͨ���š�
* @param [IN] pcDecoderTag  �����������ǩ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò��Ŵ��ڵ���Դʧ��
* @note �ýӿڻὫ�����DecoderTag�޸�Ϊ�����̵�ԭʼ������Ӧ��DecoderTag;
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetRawDecoderTag(IN ULONG ulPort,
                                                             IN const CHAR *pcDecoderTag);
#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_PLAYER_INNER_H_ */
