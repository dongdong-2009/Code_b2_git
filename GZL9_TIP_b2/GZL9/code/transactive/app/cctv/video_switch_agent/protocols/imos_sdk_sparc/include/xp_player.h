 /*******************************************************************************
 Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              xp_player.h

  Project Code: IMOSͨ��ý�岥����
   Module Name: IMOSͨ��ý�岥����
  Date Created: 2010-07-12
        Author: shiyouhua/04562
   Description: IMOSͨ��ý�岥����XP�ӿ�ͷ�ļ���XP��x-playerͨ�ò�����

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_XP_PLAYER_H_
#define _IMOS_XP_PLAYER_H_

/* ����IMOS����ƽ̨��ͷ�ļ� */
#include "imos_public.h"
#include "imos_errcode.h"
#include "imos_def.h"

/* ����ͨ��ý�岥����SDK���ݽṹͷ�ļ� */
#include "xp_datatype.h"

#ifdef  __cplusplus
extern "C"{
#endif

/** ����ѡ�� */
#if defined(WIN32)
    #if defined(XP_PLAYER_DLL_EXPORTS)
        #define IMOS_XP_PLAYER_EXPORT  __declspec( dllexport )
    #else
        #define IMOS_XP_PLAYER_EXPORT  __declspec( dllimport )
    #endif
#else
    #define IMOS_XP_PLAYER_EXPORT
#endif

/** ���̱�ʶ */
#define XP_DECODERTAG_H3C_V1       "h3c-v1"          /**< H3C V1�汾 */
#define XP_DECODERTAG_H3C_V3       "h3c-v3"          /**< H3C V3�汾��TS��װ�� */
#define XP_DECODERTAG_H3C_V3_20    "h3c-v3@20"       /**< H3C V3�汾��RTP + TS */
#define XP_DECODERTAG_H3C_V3_31    "h3c-v3@31"       /**< H3C V3�汾��RTP + ES */
#define XP_DECODERTAG_HIK_SELF     "hikvision-self"  /**< ����SELF */
#define XP_DECODERTAG_HIK_V1       "hikvision-v1"    /**< ����V1�汾 */
#define XP_DECODERTAG_HIK_V3       "hikvision-v3"    /**< ����V3�汾 */
#define XP_DECODERTAG_DAHUA_SELF   "dahua-self"      /**< ��SELF */
#define XP_DECODERTAG_DAHUA_V1     "dahua-v1"        /**< ��V1�汾 */
#define XP_DECODERTAG_DAHUA_V3     "dahua-v3"        /**< ��V3�汾 */
#define XP_DECODERTAG_AXIS_V4      "axis-v4"         /**< Axis 4.00�汾(Firmware�汾) */
#define XP_DECODERTAG_AXIS_V5      "axis-v5"         /**< Axis 5.00�汾(Firmware�汾) */
#define XP_DECODERTAG_ALEC         "alec"            /**< ������ */
#define XP_DECODERTAG_MOBOTIX      "mobotix"         /**< mobotix */
#define XP_DECODERTAG_APROUD_ONU   "aproud-onu"      /**< �ǰ���ϵͳ */
#define XP_DECODERTAG_APROUD_OEM   "aproud-sivx"     /**< �ǰ�OEMϵͳ */
#define XP_DECODERTAG_YAAN_V3      "yaan-v3"         /**< �ǰ� */
#define XP_DECODERTAG_SONY         "sony"            /**< ���� */
#define XP_DECODERTAG_SAMSUNG      "samsung"         /**< ���� */
#define XP_DECODERTAG_RUIMING      "ruiming"         /**< ����V3�汾 */
#define XP_DECODERTAG_RUIMING_SELF "ruiming-self"    /**< ����SELF */
#define XP_DECODERTAG_NANZI        "nanzi"           /**< ���� */
#define XP_DECODERTAG_NANZI_SELF   "nanzi-self"      /**< ����SELF */
#define XP_DECODETAG_TDY_SWALLOW   "TDY-SWALLOW"     /**< TDY */
#define XP_DECODETAG_TDY           "tiandy"          /**< ���ΰҵ */
#define XP_DECODETAG_DMA_SWALLOW   "DMA-SWALLOW"     /**< DMA */
#define XP_DECODERTAG_AXIS         "axis"            /**< axis */

/**
* @enum tagXpLogLevel
* @brief ������־�ȼ�ö������
* @attention ��
*/
typedef enum tagXpLogLevel
{
    XP_DEBUG = 1,               /**< debug���� */
    XP_INFO  = 2,               /**< info���� */
    XP_WARN  = 3,               /**< warn���� */
    XP_ERROR = 4,               /**< error���� */
    XP_FATAL = 5                /**< fatal���� */
}XP_LOGLEVEL_E;

/**
* @enum tagMediaPlayerRunInfoType
* @brief �����������ϱ�������Ϣ�����͵�ö�ٶ���
* @attention ��
*/
typedef enum tagMediaPlayerRunInfoType
{
    XP_PLAYER_RUN_INFO_RECORD_VIDEO  = 0,          /**< ����¼��������ϱ�������Ϣ */
    XP_PLAYER_RUN_INFO_MEDIA_PROCESS = 1,          /**< ��Ƶý�崦������е��ϱ�������Ϣ */
    XP_PLAYER_RUN_INFO_SERIES_SNATCH = 2,          /**< ����ץ�Ĺ������ϱ�������Ϣ */
    XP_PLAYER_RUN_INFO_MEDIA_VOICE = 3,            /**< ����ҵ��������ϱ�������Ϣ */
    XP_PLAYER_RUN_INFO_MEDIA_NOT_IDENTIFY = 4,     /**< �����޷�ʶ�� */
    XP_PLAYER_RUN_INFO_RECV_PACKET_NUM = 5,        /**< �����ڽ��յ��İ��� */
    XP_PLAYER_RUN_INFO_RECV_BYTE_NUM = 6,          /**< �����ڽ��յ����ֽ��� */
    XP_PLAYER_RUN_INFO_VIDEO_FRAME_NUM = 7,        /**< �����ڽ�������Ƶ֡�� */
    XP_PLAYER_RUN_INFO_AUDIO_FRAME_NUM = 8,        /**< �����ڽ�������Ƶ֡�� */
    XP_PLAYER_RUN_INFO_LOST_PACKET_RATIO = 9,      /**< �����ڶ�����ͳ����Ϣ����λΪ0.01%�� */
}XP_MEDIA_PLAYER_RUN_INFO_TYPE_E;

/**
* @struct tagRunInfo
* @brief �ϱ���Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerRunInfo
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulErrCode;                            /**< �ϱ��Ĵ����� */
}PLAYER_RUN_INFO_S;

/**
* @struct tagPlayerRecvPktNum
* @brief �ϱ������ڽ��հ�����Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerRecvPktNum
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulRecvPktNum;                         /**< ���յ��İ��� */
    ULONG ulStatPeriod;                         /**< ͳ�Ƶ����ڣ����룩 */
}PLAYER_RECV_PACKET_NUM_S;

/**
* @struct tagPlayerRecvByteNum
* @brief �ϱ������ڽ����ֽ�����Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerRecvByteNum
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulRecvByteNum;                        /**< ���յ����ֽ��� */
    ULONG ulStatPeriod;                         /**< ͳ�Ƶ����ڣ����룩 */
}PLAYER_RECV_BYTE_NUM_S;

/**
* @struct tagPlayerVideoFrameNum
* @brief �ϱ������ڽ�����Ƶ֡����Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerVideoFrameNum
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulVideoFrameNum;                      /**< ��������Ƶ֡�� */
    ULONG ulStatPeriod;                         /**< ͳ�Ƶ����ڣ����룩 */
}PLAYER_VIDEO_FRAME_NUM_S;

/**
* @struct tagPlayerAudioFrameNum
* @brief �ϱ������ڽ�����Ƶ֡����Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerAudioFrameNum
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulAudioFrameNum;                      /**< ��������Ƶ֡�� */
    ULONG ulStatPeriod;                         /**< ͳ�Ƶ����ڣ����룩 */
}PLAYER_AUDIO_FRAME_NUM_S;

/**
* @struct tagPlayerLostPktRatio
* @brief �ϱ������ڶ�������Ϣ�Ľṹ�嶨�塣
* @attention ��
*/
typedef struct tagPlayerLostPktRatio
{
    ULONG ulPort;                               /**< ����ͨ���� */
    ULONG ulLostPktRatio;                       /**< ��������Ϣ����λΪ0.01%�� */
    ULONG ulStatPeriod;                         /**< ͳ�Ƶ����ڣ����룩 */
}PLAYER_LOST_PAKCET_RATIO_S;

/**
* ��¼��־�Ļص��������Ͷ��� \n
* @param [IN] ulLogLevel        ��־����, ��Ӧ#XP_LOGLEVEL_Eö���е�ֵ��
* @param [IN] ulErrCode         ��־��Ϣ�����롣
* @param [IN] ulCmdModuleID     ������ģ��ID��
* @param [IN] pcFileName        �����ļ�����
* @param [IN] ulFileLine        ��������������
* @param [IN] pcLogDescription  ��־�����ַ�����
* @return VOID
* @note
* -    1���������ڲ���¼��־����
* -    2���������ڲ����̵߳��øûص��������û���¼��־��������Ҫ���߳�ͬ������
* -    3���������ڲ��������־�����ַ���ĩβû�д�"\r\n"�����û���д��־�ļ���ʱ����Ҫ�����д���
*/
typedef VOID (STDCALL* XP_WRITE_LOG_CALLBACK_PF)(IN ULONG ulLogLevel,
                                                 IN ULONG ulErrCode,
                                                 IN ULONG ulCmdModuleID,
                                                 IN const CHAR *pcFileName,
                                                 IN ULONG ulFileLine,
                                                 IN const CHAR *pcLogDescription);

/**
* ��Ϣ���쳣��Ϣ�ص�������ָ�����͡�\n
* @param [IN] enRunInfoType ��Ϣ�����쳣��Ϣ���ͣ���Ӧ#XP_MEDIA_PLAYER_RUN_INFO_TYPE_Eö���е�ֵ��
* @param [IN] pParam        �����Ϣ���쳣��Ϣ���ݵĻ�����ָ�루����ŵ���������Ϣ���쳣��Ϣ�����йأ�
* @return VOID
* @note
* -     1���û���Ҫȷ���ûص��������췵�ء�
* -     2���û������ڸûص�������ֱ�ӵ��ò�����SDK���κνӿں�����
* -     3������pParam��ָ�Ļ������д�ŵ����ݵ���������Ϣ�����쳣��Ϣ���Ͷ������û���Ҫ������Ϣ�����쳣��Ϣ���Ͷ���������ת����
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_RECORD_VIDEO
                              #XP_PLAYER_RUN_INFO_MEDIA_PROCESS
                              #XP_PLAYER_RUN_INFO_SERIES_SNATCH
                              #XP_PLAYER_RUN_INFO_MEDIA_VOICEʱ���û��轫pParamת��Ϊ#PLAYER_RUN_INFO_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_MEDIA_NOT_IDENTIFY���û��轫pParamת��Ϊ#PLAYER_RUN_INFO_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_RECV_PACKET_NUM���û��轫pParamת��Ϊ#PLAYER_RECV_PACKET_NUM_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_VIDEO_FRAME_NUM���û��轫pParamת��Ϊ#PLAYER_VIDEO_FRAME_NUM_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_AUDIO_FRAME_NUM���û��轫pParamת��Ϊ#PLAYER_AUDIO_FRAME_NUM_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_RECV_BYTE_NUM���û��轫pParamת��Ϊ#PLAYER_RECV_BYTE_NUM_S�ṹ������ָ�롣
* -        ��ulRunInfoTypeֵΪ#XP_PLAYER_RUN_INFO_LOST_PACKET_RATIO���û��轫pParamת��Ϊ#PLAYER_LOST_PAKCET_RATIO_S�ṹ������ָ�롣
*/
typedef VOID (STDCALL* XP_PLAYER_RUN_INFO_CALLBACK_PF)(IN ULONG ulRunInfoType,
                                                       IN VOID *pParam);

/**
* ƴ֡ǰý�������ݻص�������ָ�����͡�\n
* @param [IN] ulPort           ����ͨ����
* @param [IN] pucBuffer        ���ƴ֡ǰý�������ݻ�����ָ��
* @param [IN] ulBufSize        ��������С
* @param [IN] ulMediaDataType  ý���������ͣ��μ�ý������ö�ٶ���#XP_MEDIA_DATA_FORMAT_E
* @param [IN] lUserParam       �û����ò��������û��ڵ���#IMOS_XP_SetSourceMediaDataCB����ʱָ�����û�����
* @param [IN] lReserved        ���ƴ֡ǰý������չ��Ϣ������ָ�룬�û���Ҫת��Ϊ#XP_SOURCE_DATA_EX_INFO_S
*                              �ṹ��ָ�����ͣ��ں���������ǩ��DecoderTag�������û�ʹ�ò��ſ�������ʾ�ص�
*                              ��ý����ʱ���ɸ��ݽ�������ǩָ����������
* @return �ޡ�
* @note  �û�Ӧ��ʱ���������ý�������ݣ�ȷ���������췵�أ������Ӱ�첥�����ڵ�ý��������
*/
typedef VOID (STDCALL* XP_PLAYER_SOURCE_MEDIA_DATA_CALLBACK_PF)(IN ULONG ulPort,
                                                                IN const UCHAR *pucBuffer,
                                                                IN ULONG ulBufSize,
                                                                IN ULONG ulMediaDataType,
                                                                IN LONG lUserParam,
                                                                IN LONG lReserved);

/**
* ƴ֡����Ƶ�����ݻص�������ָ�����͡�\n
* @param [IN] ulPort                ����ͨ����
* @param [IN] pstParseVideoData     ���ƴ֡����Ƶ��������Ϣ������ָ��
* @param [IN] lUserParam            �û����ò��������û��ڵ���#IMOS_XP_SetParseVideoDataCB����ʱָ�����û�����
* @param [IN] lReserved             ��������
* @return �ޡ�
* @note
* -     �û�Ӧ��ʱ������������ݣ�ȷ���������췵�أ������Ӱ�첥�����ڵ�ý��������
*/
typedef VOID (STDCALL *XP_PLAYER_PARSE_VIDEO_DATA_CALLBACK_PF)(IN ULONG ulPort,
                                                               IN const XP_PARSE_VIDEO_DATA_S *pstParseVideoData,
                                                               IN LONG lUserParam,
                                                               IN LONG lReserved);

/**
* ƴ֡����Ƶ�����ݻص�������ָ�����͡�\n
* @param [IN] ulPort                ����ͨ����
* @param [IN] pstParseAudioData     ���ƴ֡����Ƶ��������Ϣ������ָ��
* @param [IN] lUserParam            �û����ò��������û��ڵ���#IMOS_XP_SetParseAudioDataCB����ʱָ�����û�����
* @param [IN] lReserved             ��������
* @return �ޡ�
* @note
* -     �û�Ӧ��ʱ������������ݣ�ȷ���������췵�أ������Ӱ�첥�����ڵ�ý��������
*/
typedef VOID (STDCALL *XP_PLAYER_PARSE_AUDIO_DATA_CALLBACK_PF)(IN ULONG ulPort,
                                                               IN const XP_PARSE_AUDIO_DATA_S *pstParseAudioData,
                                                               IN LONG lUserParam,
                                                               IN LONG lReserved);

/**
* �������Ƶ�����ݻص�������ָ�����͡�\n
* @param [IN] ulPort         ����ͨ����
* @param [IN] pPictureData   ��Ž������Ƶ��������Ϣ������ָ��
* @param [IN] lUserParam     �û����ò��������û��ڵ���#IMOS_XP_SetDecodeVideoDataCB����ʱָ�����û�����
* @param [IN] lReserved      ��������
* @return �ޡ�
* @note
* -     1���û�Ӧ��ʱ�����������Ƶ�����ݣ�ȷ���������췵�أ������Ӱ�첥�����ڵ�ý��������
* -     2����Ƶ������yv12��ʽ������˳��Y0-Y1-......������U0-U1-......������V0-V1-......��
*/
typedef VOID (STDCALL* XP_PLAYER_DECODE_VIDEO_DATA_CALLBACK_PF)(IN ULONG ulPort,
                                                                IN const XP_PICTURE_DATA_S *pPictureData,
                                                                IN LONG lUserParam,
                                                                IN LONG lReserved);

/**
* �������Ƶ�����ݻص�������ָ�����͡�\n
* @param [IN] ulPort         ����ͨ����
* @param [IN] pWaveData      ��Ž������Ƶ��������Ϣ������ָ��
* @param [IN] lUserParam     �û����ò��������û��ڵ���#IMOS_XP_SetDecodeAudioDataCB����ʱָ�����û�����
* @param [IN] lReserved      ��������
* @return �ޡ�
* @note
* -     �û�Ӧ��ʱ�����������Ƶ�����ݣ�ȷ���������췵�أ������Ӱ�첥�����ڵ�ý��������
*/
typedef VOID (STDCALL* XP_PLAYER_DECODE_AUDIO_DATA_CALLBACK_PF)(IN ULONG ulPort,
                                                                IN const XP_WAVE_DATA_S *pWaveData,
                                                                IN LONG lUserParam,
                                                                IN LONG lReserved);

/* δ�����ӿڰ��� */
#include "xp_player_inner.h"

/**
* ע�Ქ������������־�Ļص�������\n
* @param [IN] pfnWriteLogFunc ��������������־�ص�����ָ�롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note
* -    1��Ϊ�˷�ֹ��־��Ϣ��¼��©���û�Ӧ�ڵ��ò����������ӿں���֮ǰ���øú�����
* -    2������û������øú�������������־�Ļص��������򲥷����ڲ�����¼�κ���־��Ϣ��
* -    3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetWriteLogCB(IN XP_WRITE_LOG_CALLBACK_PF pfnWriteLogFunc);

/**
* ���ò����������ڵĸ�Ŀ¼·����\n
* @param [IN] pcRootPath �����������ڵĸ�Ŀ¼·����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note
* -    1���ýӿ���Ҫ��#IMOS_XP_Init֮ǰ���ã�
* -    2������û������øú������ò����������ڵĸ�Ŀ¼·�����򲥷����ڲ�Ĭ�ϲ��õ�ǰӦ�ó��������·����
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPlayerRootPath(IN const CHAR *pcRootPath);

/**
* ��ʼ��������SDK��Դ��\n
* @param [IN] �ޡ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_XP_FAIL_TO_INIT_XP          ��ʼ����������Դʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_Init();

/**
* �ͷŲ�����SDK��Դ���ڽ���ʹ�ò�����SDK֮ǰ�����á�\n
* @param [IN] �ޡ�
* @return ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_Cleanup();

/**
* ע����Ϣ���쳣��Ϣ�ص�������\n
* @param [IN] pfRunInfoFunc ��Ϣ���쳣��Ϣ�ص�����ָ�롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note
* -    1�������������ø���������Ļص������е��ò�������ӿڣ�
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPlayerRunMsgCB(IN XP_PLAYER_RUN_INFO_CALLBACK_PF pfnRunInfoFunc);

/**
* ������Ƶ��ʾģʽ��\n
* @param [IN] ulRenderMode ��Ƶ��ʾģʽ�����#XP_RENDER_MODE_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note
* -  1���ýӿ���IMOS_XP_Init֮����ã����õ���Ƶ��ʾģʽ�������в���ͨ�������ã�
* -  2������ͻ������øú���������Ƶ��ʾģʽ���򲥷����ڲ�Ĭ��ʹ��D3D����ʾģʽ��
* -  3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetRenderMode(IN ULONG ulRenderMode);

/**
* ������Ƶ���ظ�ʽ��\n
* @param [IN] ulPixelFormat ��Ƶ���ظ�ʽ�����#XP_PIXEL_FORMAT_EX_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* @note
* -  1���ýӿ���IMOS_XP_Init֮����ã����õ���Ƶ���ظ�ʽ�������в���ͨ�������ã�
* -  2������ͻ������øú���������Ƶ���ظ�ʽ���򲥷����ڲ�Ĭ��ʹ��YUV420��Ƶ���ظ�ʽ��
* -  3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPixelFormat(IN ULONG ulPixelFormat);

/**
* �򿪱��ز����ļ���\n
* @param [IN] ulPort        ����ͨ���š�
* @param [IN] pcFileName    �����ļ����ļ����������ļ�·������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_PORT_NOT_AVAILABLE           ����ͨ��������
* -         #ERR_COMMON_NO_MEMORY                ϵͳ�ڴ治��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ������Դʧ��
* -         #ERR_XP_NOT_SUPPORT_FILE_FORMAT      �ļ����Ÿ�ʽ��֧��
* -         #ERR_XP_FAIL_TO_GET_TOTAL_TIME       ����ļ��ܵĲ���ʱ��ʧ��
* -         #ERR_XP_FAIL_TO_OPEN_FILE            ���������ļ�����ʧ��(�ļ������ڻ������ڱ���������ʹ��)
* -         #ERR_XP_FILE_DESTROY                 ��Ҫ�򿪵��ļ�̫С�����Ѿ���
* -         #ERR_XP_FILE_HAVE_OPEN               ָ��ͨ���Ѿ������ļ�
* @note
* -    1�������ļ����ź��ڲ��Ź����У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_MEDIA_PROCESS��Я���Ĵ����룺
* -     #ERR_COMMON_NO_MEMORY               �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FINISH_PLAY_FILE            �ϱ���Ϣ֮ǰ����������ͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FILE_DESTROY                �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_RECV_DATA           �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_PROCESS_MEDIA_DATA  �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OpenFile(IN ULONG ulPort,
                                                     IN const CHAR *pcFileName);

/**
* ����ļ��ܵ�ʱ�䳤�ȡ�\n
* @param [IN] ulPort        ����ͨ���š�
* @param [OUT] pulTotalTime ����ļ��ܵ�ʱ�䳤�ȵ�ָ�루��λΪ�룩��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetFileTime(IN ULONG ulPort,
                                                        OUT ULONG *pulTotalTime);

/**
* �رձ��ز����ļ���\n
* @param [IN] ulPort        ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_CLOSE_FILE           �رձ����ļ��ط�ҵ��ʧ��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CloseFile(IN ULONG ulPort);

/**
* �����������ݲ��š�\n
* @param [IN] ulPort          ����ͨ���š�
* @param [IN] pcLocalIP       ����IP��ַ��
* @param [IN] usLocalPort     ���ؼ����˿ڡ�
* @param [IN] pcRemoteIP      Զ��IP��ַ��
* @param [IN] usRemotePort    Զ�˶˿ڡ�
* @param [IN] ulTransProtl    ����ý�����Ĵ����Э�飬���#XP_PROTOCOL_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     ��ò���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_AVAILABLE       ����ͨ��������
* -         #ERR_COMMON_NO_MEMORY            ϵͳ�ڴ治��
* -         #ERR_XP_FAIL_TO_OPEN_STREAM      ����ý��������ҵ��ʧ��
* @note
* -    1��Ŀǰ��֧��UDPЭ�����ý����������
* -    2���ú���֧�ֵ������鲥���ַ�ʽ������ʱ�����ʹ�ñ���Ĭ�ϵ�IP��ַ���򱾵�IP��ַ������ַ������ɣ�
* -       ��ʱ��Զ�˵�IP��ַ�Ͷ˿ڿ����ڶԽ��յ������ݰ�����У�顢���ˣ���ֹ����������û�����Ҫ��ֹ������
* -       ��ɽ�Զ��IP��ַ�Ͷ˿ڷֱ���дΪ��0.0.0.0����0���鲥ʱ��Զ�˵�IP��ַ�Ͷ˿ڱ�����д�鲥��ַ�Ͷ˿ڣ�
* -       ��ʱ������IP��ַҲ����Ҫ��дһ����ʵ��Ч�ı���IP��ַ�������ض˿ڿ�����д����ֵ(��ʹ��)��
* -    3���������������ź��ڲ��Ź����У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_MEDIA_PROCESS��Я���Ĵ����룺
* -     #ERR_COMMON_NO_MEMORY               �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_RECV_DATA           �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_PROCESS_MEDIA_DATA  �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -    4���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OpenNetStream(IN ULONG ulPort,
                                                          IN const CHAR *pcLocalIP,
                                                          IN USHORT usLocalPort,
                                                          IN const CHAR *pcRemoteIP,
                                                          IN USHORT usRemotePort,
                                                          IN ULONG ulTransProtl);

/**
* �ر����������ݲ��š�\n
* @param [IN] ulPort       ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_CLOSE_STREAM         �ر�ý��������ʧ��
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CloseNetStream(IN ULONG ulPort);

/**
* �����������ݲ��š�\n
* @param [IN] ulPort           ����ͨ���š�
* @param [IN] pucFileHeadBuf   �ļ�ͷ���ݡ�
* @param [IN] ulSize           �ļ�ͷ���ȡ�
* @param [IN] ulBufPoolSize    ���ò������д��ý���������Ļ�������С��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_PORT_NOT_AVAILABLE               ����ͨ��������
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_COMMON_NO_MEMORY                    ϵͳ�ڴ治��
* -         #ERR_XP_FAIL_TO_OPEN_STREAM              ��������������ҵ��ʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
* -    2���������������ź��ڲ��Ź����У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_MEDIA_PROCESS��Я���Ĵ����룺
* -     #ERR_COMMON_NO_MEMORY               �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_RECV_DATA           �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
* -     #ERR_XP_FAIL_TO_PROCESS_MEDIA_DATA  �ϱ���Ϣ֮ǰ��������ֹͣ��Ӧͨ���Ĳ���
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OpenInputStream(IN ULONG ulPort,
                                                            IN const UCHAR *pucFileHeadBuf,
                                                            IN ULONG ulSize,
                                                            IN ULONG ulBufPoolSize);

/**
* ���������ݣ���������������������ݡ�\n
* @param [IN] ulPort          ����ͨ���š�
* @param [IN] pucDataBuf      �����ݻ�������ַ��
* @param [IN] ulDataLen       �����ݻ��������ȡ�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_PORT_NOT_OPEN                    ͨ��δ��ý��ҵ��
* -         #ERR_COMMON_NO_MEMORY                    ϵͳ�ڴ治��
* -         #ERR_XP_FAIL_TO_INPUT_DATA               ��������ʧ��
* -         #ERR_XP_INPUTDATA_BUFFER_FULL            ������������������ʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_InputMediaData(IN ULONG ulPort,
                                                           IN const UCHAR *pucDataBuf,
                                                           IN ULONG ulDataLen);

/**
* �ر����������ݲ��š�\n
* @param [IN] ulPort    ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_CLOSE_STREAM             �ر�ý��������ʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CloseInputStream(IN ULONG ulPort);

/**
* ���ò���ͼ�������Ի�ʵʱ�ԡ�\n
* @param [IN] ulPort        ����ͨ���š�
* @param [IN] ulFluency     ����ͼ����ʵʱ�����Ȼ������������ȣ����#XP_PICTURE_FLUENCY_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                    �ɹ�
* -         #ERR_COMMON_INVALID_PARAM              ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES           ��ò���ͨ����Դʧ��
* @note
* -    1���ú�����������������������������ʱͼ��������ԣ��Ա����ļ��ط�ʱͼ��������ý��������ã�
*         ����Ҫ��#IMOS_XP_StartPlay֮ǰ���ã����ý���ָ��ͨ����Ч��
* -    2������û������øú������ò���ͼ�������ԣ��򲥷����ڲ�Ĭ��ʹ��ͼ��ʵʱ�����ȣ�
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPictureFluency(IN ULONG ulPort,
                                                              IN ULONG ulFluency);

/**
* ����ƴ֡ǰý�������ݻص�������\n
* @param [IN] ulPort                  ����ͨ���š�
* @param [IN] pfnSourceMediaDataCBFun ƴ֡ǰý�������ݻص�������ָ�롣
* @param [IN] bContinue               �Ƿ�������к����ƴ֡���������ʾ������
* @param [IN] lUserParam              �û����ò�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB      ����ý�������ݻص�����ʧ��
* @note
* -     1���ú�����#IMOS_XP_StartPlay֮ǰ��֮����ã���#IMOS_XP_StopPlayʱ�Զ�ʧЧ��
* -     2���ص�����Ҫ���췵�أ����Ҫֹͣ�ص������԰ѻص�����ָ����ΪNULL��
* -     3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetSourceMediaDataCB(IN ULONG ulPort,
                                                                 IN XP_PLAYER_SOURCE_MEDIA_DATA_CALLBACK_PF pfnSourceMediaDataCBFun,
                                                                 IN BOOL_T bContinue,
                                                                 IN LONG lUserParam);

/**
* ����ƴ֡����Ƶ���ݻص�������\n
* @param [IN] ulPort                    ����ͨ���š�
* @param [IN] pfnParseVideoDataCBFun    ƴ֡����Ƶ���ݻص�������ָ�롣
* @param [IN] bContinue                 �Ƿ�������н������ʾ������
* @param [IN] lUserParam                �û����ò�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB      ����ý�������ݻص�����ʧ��
* @note
* -    1���ú�����#IMOS_XP_StartPlay֮ǰ��֮����ã���#IMOS_XP_StopPlayʱ�Զ�ʧЧ��
* -    2���ص�����Ҫ���췵�أ����Ҫֹͣ�ص������԰ѻص�����ָ����ΪNULL��
* -    3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetParseVideoDataCB(IN ULONG ulPort,
                                                                IN XP_PLAYER_PARSE_VIDEO_DATA_CALLBACK_PF pfnParseVideoDataCBFun,
                                                                IN BOOL_T bContinue,
                                                                IN LONG lUserParam);

/**
* ����ƴ֡����Ƶ���ݻص�������\n
* @param [IN] ulPort                    ����ͨ���š�
* @param [IN] pfnParseAudioDataCBFun    ƴ֡����Ƶ���ݻص�������ָ�롣
* @param [IN] bContinue                 �Ƿ�������н���Ͳ��Ų�����
* @param [IN] lUserParam                �û����ò�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB      ����ý�������ݻص�����ʧ��
* @note
* -    1���ú�����#IMOS_XP_StartPlay֮ǰ��֮����ã���#IMOS_XP_StopPlayʱ�Զ�ʧЧ��
* -    2���ص�����Ҫ���췵�أ����Ҫֹͣ�ص������԰ѻص�����ָ����ΪNULL��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetParseAudioDataCB(IN ULONG ulPort,
                                                                IN XP_PLAYER_PARSE_AUDIO_DATA_CALLBACK_PF pfnParseAudioDataCBFun,
                                                                IN BOOL_T bContinue,
                                                                IN LONG lUserParam);

/**
* ���ý������Ƶý�������ݻص�������\n
* @param [IN] ulPort                    ����ͨ���š�
* @param [IN] pfnDecodeVideoDataCBFun   �������Ƶý�������ݻص�������ָ�롣
* @param [IN] bContinue                 �Ƿ�������к������ʾ������
* @param [IN] lUserParam                �û����ò�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB      ����ý�������ݻص�����ʧ��
* @note
* -    1���ú�����#IMOS_XP_StartPlay֮ǰ��֮����ã���#IMOS_XP_StopPlayʱ�Զ�ʧЧ��
* -    2���ص�����Ҫ���췵�أ����Ҫֹͣ�ص������԰ѻص�����ָ����ΪNULL��
* -    3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDecodeVideoMediaDataCB(IN ULONG ulPort,
                                                                      IN XP_PLAYER_DECODE_VIDEO_DATA_CALLBACK_PF pfnDecodeVideoDataCBFun,
                                                                      IN BOOL_T bContinue,
                                                                      IN LONG lUserParam);

/**
* ���ý������Ƶý�������ݻص�������\n
* @param [IN] ulPort                    ����ͨ���š�
* @param [IN] pfnDecodeAudioDataCBFun   �������Ƶý�������ݻص�������ָ�롣
* @param [IN] bContinue                 �Ƿ�������к���Ĳ��Ų�����
* @param [IN] lUserParam                �û����ò�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB      ����ý�������ݻص�����ʧ��
* @note
* -    1���ú�����#IMOS_XP_StartPlay֮ǰ��֮����ã���#IMOS_XP_StopPlayʱ�Զ�ʧЧ��
* -    2���ص�����Ҫ���췵�أ����Ҫֹͣ�ص������԰ѻص�����ָ����ΪNULL��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDecodeAudioMediaDataCB(IN ULONG ulPort,
                                                                      IN XP_PLAYER_DECODE_AUDIO_DATA_CALLBACK_PF pfnDecodeAudioDataCBFun,
                                                                      IN BOOL_T bContinue,
                                                                      IN LONG lUserParam);

/**
* ����ͨ���������ĳ��̱�ʶ��\n
* @param [IN] ulPort        ����ͨ���š�
* @param [IN] pcDecoderTag  ͨ���������ĳ��̱�ʶ�����XP_DECODERTAG_XXXXһ��ĺ궨����ȡֵ����XP_DECODERTAG_H3C_V3
*                           ��H3C V3�汾�ı�ʶ�������˸ñ�ʶ�Ĳ���ͨ����ʹ��H3C V3�汾�Ľ�������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò��Ŵ��ڵ���Դʧ��
* @note
* -    1������û�û�е��øú�������ָ��ͨ���������ĳ��̱�ʶ�����ͨ��Ĭ��ʹ��H3C V3��TS��װ�����Ľ�������
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDecoderTag(IN ULONG ulPort,
                                                          IN const CHAR *pcDecoderTag);

/**
* ���ͨ���������ĳ��̱�ʶ��\n
* @param [IN] ulPort         ����ͨ���š�
* @param [OUT] pcDecoderTag  ͨ���������ĳ��̱�ʶ������Ϊ#IMOS_STRING_LEN_32
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                 �ɹ�
* -         #ERR_COMMON_INVALID_PARAM           ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        ��ò��Ŵ��ڵ���Դʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetDecoderTag(IN ULONG ulPort,
                                                          OUT CHAR *pcDecoderTag);

/**
* ���ò�����Ƶ�Ĵ��ھ����\n
* @param [IN] ulPort    ����ͨ���š�
* @param [IN] hWnd      ���Ŵ��ھ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPlayWnd(IN ULONG ulPort,
                                                       IN IMOS_HANDLE hWnd);

/**
* ��ʼ���š�\n
* @param [IN] ulPort    ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_COMMON_NO_MEMORY                ϵͳ�ڴ治��
* -         #ERR_XP_PORT_NOT_OPEN                ͨ��δ��ý��ҵ��
* -         #ERR_XP_FAIL_TO_START_PLAY           ��ʼ����ʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartPlay(IN ULONG ulPort);

/**
* ��ͣ���š�\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_PAUSE_MEDIA          ��ͣý�岥�Ŵ���ʧ��
* -         #ERR_XP_FAIL_TO_PAUSE                ��ͣ���Ų���ʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_PausePlay(IN ULONG ulPort);

/**
* �ָ����ţ��ָ���������ͣ��ͨ����\n
* @param [IN] ulPort    ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_RESUME_MEDIA         �ָ�ý�岥�Ŵ���ʧ��
* -         #ERR_XP_FAIL_TO_RESUME               �ָ����Ų���ʧ��
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ResumePlay(IN ULONG ulPort);

/**
* ����ʱ�������ļ�����λ�á�\n
* @param [IN] ulPort      ����ͨ���š�
* @param [IN] ulTime      ��תĿ��ʱ�䣨��λΪ�룩��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý�����Ͳ�֧�ָò���
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       ��ǰ���ڱ���¼��
* -         #ERR_XP_FAIL_TO_DRAG_PLAY            ��ת����ʧ��
* @note
* -    1�������ǰ���ڱ���¼�񣬲�֧�ָ���ʱ�������ļ�����λ�ò�����
* -    2���ú���ֻ�����ڱ����ļ��ط�ʱ����ʱ�������䲥��λ�ã�
* -    3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPlayedTime(IN ULONG ulPort,
                                                          IN ULONG ulTime);

/**
* ����ļ���ǰ����ʱ�䡣\n
* @param [IN]  ulPort       ����ͨ���š�
* @param [OUT] pulTime      ��ŵ�ǰ���Ž���ʱ���ָ�루��λΪ�룩��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý�����Ͳ�֧�ָò���
* -         #ERR_XP_FAIL_TO_GET_PLAY_POSITION    ��õ�ǰ���Ž���ʧ��
* @note
* -    1���ú���ֻ�����ڱ����ļ��ط�ʱ����䵱ǰ����ʱ�䣻
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetPlayedTime(IN ULONG ulPort,
                                                          OUT ULONG *pulTime);

/**
* ���ò����ٶȡ�\n
* @param [IN] ulPort        ����ͨ���š�
* @param [IN] ulPlaySpeed   �����ٶȣ����#XP_PLAY_STATUS_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       ��ǰ���ڱ���¼��
* -         #ERR_XP_FAIL_TO_MULTIPL_SPEED_PLAY   ����ǰ��/���˲���ʧ��
* @note
* -    1�����ڱ���¼��ʱ����֧�ֱ���ǰ��/���˲��Ų�����
* -    2�������ļ����Ų�֧�ֺ��˲��ţ�
* -    3���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPlaySpeed(IN ULONG ulPort,
                                                         IN ULONG ulPlaySpeed);

/**
* ��֡ǰ�����š�\n
* @param [IN] ulPort   ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       ��ǰ���ڱ���¼��
* -         #ERR_XP_FAIL_TO_STEP_PLAY            ��֡����ʧ��
* @note
* -    1�����ڱ���¼��ʱ����֧�ֵ�֡���Ų�����
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OneByOne(IN ULONG ulPort);

/**
* ������л�������ʣ�����ݡ�\n
* @param [IN] ulPort        ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_NOT_START_PLAY               ����ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_RESET_BUFFER         ��ջ�����ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ResetBuf(IN ULONG ulPort);

/**
* ֹͣ���š�\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_PLAY            ֹͣ���Ų���ʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopPlay(IN ULONG ulPort);

/**
* ��ռ��ʽ��������\n
* @param [IN] ulPort   ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* -         #ERR_XP_DECODE_AUDIO_NOT_CONTINUE        ��Ƶ�����ݽ���󲻼������в��Ų���
* -         #ERR_XP_FAIL_TO_PLAY_SOUND               ������ʧ��
* @note
* -    1��ͬһʱ��ֻ�ܲ���һ·��������������Ѿ��������򿪣����Զ��ر�ԭ���Ѿ��򿪵�������
* -       Ĭ������������ǹرյġ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_PlaySound(IN ULONG ulPort);

/**
* �ر�������\n
* @param [IN] ulPort    ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* -         #ERR_XP_FAIL_TO_STOP_SOUND               �ر�����ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopSound(IN ULONG ulPort);

/**
* �������ڡ�\n
* @param [IN] ulVolume  ����ֵ����Χ��0��255��255Ϊ�����������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_ADJUST_VOLUME    ������������ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVolume(IN ULONG ulVolume);

/**
* ��õ�ǰ���õ�����ֵ��\n
* @param [OUT] pulVolume    �������ֵ��ָ�루��Χ��0��255��255Ϊ�����������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_VOLUME       �������ֵʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetVolume(OUT ULONG *pulVolume);

/**
* ��������ͨ����Ƶ���ݵ�WAVE���Σ����Ըı�����ͨ�����������Ĵ�С��\n
* @param [IN] ulCoefficient �����Ĳ�������Χ��0��255��255Ϊ����������ֵ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_ADJUST_VOLUME    ������������ʧ��
* @note
* -    1���˺�����#IMOS_XP_SetVolume�����Ĳ�֮ͬ�����ڣ����ǵ�������������ͨ���������ݵ�WAVE���Σ�������ͨ�������ã�
          #IMOS_XP_SetVolume�ǵ�����������������������ϵͳ�����ã�
* -    2������û������øú�����������ͨ����Ƶ���ݵ�WAVE���Σ������Ĭ�ϵĵ�������ֵ146��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_AdjustAllWaveAudio(IN ULONG ulCoefficient);

/**
* ����ָ������ͨ������Ƶͼ����ʾ������\n
* @param [IN] ulPort            ����ͨ���š�
* @param [IN] ulRenderScale     ��Ƶͼ���ڲ���ͨ���ϵ���ʾ���������#XP_RENDER_SCALE_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                     �ɹ�
* -         #ERR_COMMON_INVALID_PARAM               ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            ��ò���ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_SET_RENDER_SCALE        ����ͼ����ʾ��������ʧ��
* @note
* -    1���ڲ����øýӿڵ�����£���Ƶͼ��Ĭ������������Ŵ���
* -    2������������֮ǰ���øýӿ�ʱ�������õ���ʾ���������ں��������ò���ͨ������ʱ��Ч��
* -    3���ڲ��Ź����е��øýӿ�ʱ�������õ���ʾ��������ʵʱ��Ч��
* -    4�����øýӿڶ�ָ������ͨ�����õ���ʾ���������ڲ����������ڼ�һֱ��Ч��
* -    5���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetRenderScale(IN ULONG ulPort,
                                                           IN ULONG ulRenderScale);

/**
* ����ͼ��ĶԱȶȲ�������ʱ�����á�\n
* @param [IN] ulPort             ����ͨ���š�
* @param [IN] ulContrastMode     ͼ��Աȶ�ģʽ�����#XP_PICTURE_CONTRAST_MODE_Eö����ȡֵ��
* @param [IN] ulContrastLevel    ͼ��Աȶȼ������#XP_PICTURE_CONTRAST_LEVEL_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_SUPPORT_SET_CONTRAST ����ͨ����֧������ͼ��ԱȶȲ���
* -         #ERR_XP_FAIL_TO_SET_CONTRAST     ����ͼ��Աȶ�ʧ��
* @note
* -    1���ڲ����øýӿڵ�����£���Ƶͼ��Ĭ�ϲ����жԱȶȵ��ڴ���
* -    2�������ڿ�ʼ����#IMOS_XP_StartPlay֮ǰ���ã������õĶԱȶȲ��������棬��Ϊ������ͨ������ʱ
* -       �ĳ�ʼֵ���ڿ�ʼ���ź���øýӿ����õĶԱȶȲ�����ʱ��Ч��
* -    3�����øýӿڶ�ָ������ͨ�����õĶԱȲ����ڲ����������ڼ�һֱ��Ч��������ulContrastMode
* -       ����#XP_PICTURE_CONTRAST_MODE_NONEʱ����ֹͣ�Աȶȵ��ڴ�����ʱulContrastLevel������
* -       ���ֵ����Ч��
* -    4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPictureContrast(IN ULONG ulPort,
                                                               IN ULONG ulContrastMode,
                                                               IN ULONG ulContrastLevel);

/**
* ���ý��볡ģʽ��\n
* @param [IN] ulPort      ����ͨ���š�
* @param [IN] ulFieldMode ��ģʽ�����#XP_FIELD_MODE_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_SET_FIELD_MODE       ���ý��볡ģʽʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����ã�
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetFieldMode(IN ULONG ulPort,
                                                         IN ULONG ulFieldMode);

/**
* ����ȥ��֯����ģʽ��\n
* @param [IN] ulPort            ����ͨ���š�
* @param [IN] ulDeinterlaceMode ȥ��֯����ģʽ�����#XP_DEINTERLACE_MODE_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ�������Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC            ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_ENABLE_DEINTERLACE       ������ر�ȥ��֯����ʧ��
* @note
* -    1������û������øú�������ȥ��֯����ģʽ���򲥷����ڲ�Ĭ��ʹ�û��ģʽ����ȥ��֯����
* -    2���ú�����Ҫ��#IMOS_XP_StartPlay֮����ã�
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDeinterlaceMode(IN ULONG ulPort,
                                                               IN ULONG ulDeinterlaceMode);

/**
* ����ʵʱץ�ġ�\n
* @param [IN] ulPort            ����ͨ���š�
* @param [IN] pcFileName        ����ͼ����ļ�·���������ļ�����������׺������
* @param [IN] ulPicFormat       ͼ���ʽ�����#XP_PICTURE_FORMAT_Eö����ȡֵ��
* @param [OUT] pcFilePostfix    ���ͼ���ļ���׺��ָ�룬����������Ӧ��С��#IMOS_XP_FILE_POSTFIX_LEN��
*                               ����û�����Ҫ���غ�׺�����ɽ��ò�������ΪNULL��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_SNATCH_FORMAT    ץ�ĸ�ʽ��֧��
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH     Ӳ�̿ռ䲻��
* -         #ERR_XP_NO_PICTURE_TO_SNATCH         û�н������ͼƬ�ɹ�ץ��
* -         #ERR_XP_FAIL_TO_SNATCH_ONCE          ����ץ�Ĳ���ʧ��
* @note
* -    1��������������ͼ���ʽ���ļ�������׷�Ӷ�Ӧ�ĺ�׺������BMPͼ���ļ��ĺ�׺Ϊ.bmp��
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SnatchOnce(IN ULONG ulPort,
                                                       IN const CHAR *pcFileName,
                                                       IN ULONG ulPicFormat,
                                                       OUT CHAR *pcFilePostfix);

/**
* ����¼������������\n
* @param [IN] ulPort                 ����ͨ���š�
* @param [IN] ulRecordQuality        ¼���������������#XP_RECORD_QUALITY_Eö����ȡֵ��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* @note
* -    1��¼��������������Ϊ#XP_RECORD_PRIORITYʱ��¼�������ȣ�����Ϊ#XP_PLAY_PRIORITYʱ�����Ŵ������ȣ���¼����ܻᶪ�����ݣ�
* -    2�������øýӿ�ʱ���������ڲ�Ĭ��Ϊ#XP_PLAY_PRIORITY��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetRecordQuality(IN ULONG ulPort,
                                                             IN ULONG ulRecordQuality);

/**
* ��ʼ����¼��\n
* @param [IN] ulPort            ����ͨ���š�
* @param [IN] pcFileName        ¼���ļ����·���������ļ�����������׺������
* @param [IN] ulMediaFileFormat ¼���ļ��ĸ�ʽ�����#XP_MEDIA_FILE_FORMAT_Eö����ȡֵ��
* @param [OUT] pcFilePostfix    ���¼���ļ���׺��ָ�룬����������Ӧ��С��#IMOS_XP_FILE_POSTFIX_LEN��
*                               ����û�����Ҫ���غ�׺�����ɽ��ò�������ΪNULL��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                          �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                    ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES                 ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                       ͨ��δ��ʼ���Ų���
* -         #ERR_XP_ALREADY_START_RECORD_VIDEO           ָ���Ĳ���ͨ���Ѿ�����¼�����
* -         #ERR_XP_NOT_SUPPORT_RECORD_VIDEO_FORMAT      ¼���ʽ��֧��
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH             Ӳ�̿ռ䲻��
* -         #ERR_XP_IN_PAUSE_PLAY_STATUS                 ��ǰ��������ͣ����״̬
* -         #ERR_XP_IN_MULTIPL_SPEED_PLAY_STATUS         ��ǰ�����ڱ��ٲ���״̬
* -         #ERR_XP_IN_STEP_PLAY_STATUS                  ��ǰ�����ڵ�֡����״̬
* -         #ERR_XP_NOT_SUPPORT                          ��֧�ָù���
* -         #ERR_XP_FAIL_TO_START_RECORD_VIDEO           ��������¼��ʧ��
* @note
* -    1��������������ý������ʽ���ļ�������׷�Ӷ�Ӧ�ĺ�׺������TS��¼���ļ��ĺ�׺Ϊ.ts��
* -    2�������Ŵ�����ͣ���š����ٲ��š����ٲ��Ż�֡����״̬ʱ����֧����������¼��٣�
* -    3������¼�����¼������У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_RECORD_VIDEO��Я���Ĵ����룺
* -     #ERR_XP_DISK_CAPACITY_WARN              �ϱ���Ϣ֮ǰ������������¼��
* -     #ERR_XP_DISK_CAPACITY_NOT_ENOUGH        �ϱ���Ϣ֮ǰ��������ֹͣ¼��
* -     #ERR_XP_FAIL_TO_WRITE_FILE              �ϱ���Ϣ֮ǰ��������ֹͣ¼��
* -     #ERR_XP_FAIL_TO_PROCESS_MEDIA_DATA      �ϱ���Ϣ֮ǰ��������ֹͣ¼��
* -     #ERR_XP_MEDIA_RESOLUTION_CHANGE         �ϱ���Ϣ֮ǰ��������ֹͣ¼��
* -     #ERR_XP_NOT_SUPPORT_MEDIA_ENCODE_TYPE   �ϱ���Ϣ֮ǰ��������ֹͣ¼��
* -    4���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartRecord(IN ULONG ulPort,
                                                        IN const CHAR *pcFileName,
                                                        IN ULONG ulMediaFileFormat,
                                                        OUT CHAR *pcFilePostfix);

/**
* ֹͣ����¼��\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                          �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES                 ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_RECORD_VIDEO               ͨ��δ��ʼ����¼�����
* -         #ERR_XP_FAIL_TO_STOP_RECORD_VIDEO            ֹͣ����¼��ʧ��
* -         #ERR_XP_NOT_START_PLAY                       ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopRecord(IN ULONG ulPort);

/**
* �����Ƶ�����ʽ��\n
* @param [IN]  ulPort             ����ͨ���š�
* @param [OUT] pulVideoEncodeType �����Ƶ�����ʽ��ָ�룬ֵΪ#XP_VIDEO_ENCODE_TYPE_Eö����һ����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                    �ɹ�
* -         #ERR_COMMON_INVALID_PARAM              ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES           ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                 ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC          ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_GET_VIDEO_ENCODE_TYPE  �����Ƶ�����ʽʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����#IMOS_XP_StopPlay֮ǰ���ã�����׼ȷ���ָ������ͨ������Ƶ�����ʽ��
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetVideoEncodeType(IN ULONG ulPort,
                                                               OUT ULONG *pulVideoEncodeType);

/**
* ��ö����ʡ�\n
* @param [IN]  ulPort        ����ͨ���š�
* @param [OUT] pulRecvPktNum ����ѽ��յ�������ָ�롣
* @param [OUT] pulLostPktNum ��Ŷ���������ָ�롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_GET_LOST_PKT_RATE    ��ö�����ͳ������ʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����#IMOS_XP_StopPlay֮ǰ���ã�����׼ȷ���ָ������ͨ���Ķ����ʣ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetLostPacketRate(IN ULONG ulPort,
                                                              OUT ULONG *pulRecvPktNum,
                                                              OUT ULONG *pulLostPktNum);

/**
* ��λ�����ʡ�\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC            ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_RESET_LOST_PKT_RATE      ��λ������ͳ������ʧ��
* @note
* -    1�����øú����󣬽��Ӹ�λʱ�����¿�ʼͳ�ƶ����ʣ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ResetLostPacketRate(IN ULONG ulPort);

/**
* ��ö�֡�ʡ�\n
* @param [IN] ulPort           ����ͨ���š�
* @param [OUT] pulAllFrameNum  ���֡������ָ�롣
* @param [OUT] pulLostFrameNum ��Ŷ�����֡����ָ�롣
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_GET_LOST_FRAME_RATE  ��ö�֡��ͳ������ʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����#IMOS_XP_StopPlay֮ǰ���ã�����׼ȷ���ָ������ͨ���Ķ�֡�ʣ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetLostFrameRate(IN ULONG ulPort,
                                                             OUT ULONG *pulAllFrameNum,
                                                             OUT ULONG *pulLostFrameNum);

/**
* ��λ��֡�ʡ�\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC            ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_RESET_LOST_FRAME_RATE    ��λ��֡��ͳ��ʧ��
* @note
* -    1�����øú����󣬽��Ӹ�λʱ�����¿�ʼͳ�ƶ�֡�ʣ�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ResetLostFrameRate(IN ULONG ulPort);

/**
* ���֡�ʡ�\n
* @param [IN]  ulPort       ����ͨ���š�
* @param [OUT] pulFrameRate ���֡�ʵ�ָ�룬��λΪfps��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_GET_FRAME_RATE       ���֡������ʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����#IMOS_XP_StopPlay֮ǰ���ã�����׼ȷ���ָ������ͨ����֡�ʣ�
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetFrameRate(IN ULONG ulPort,
                                                         OUT ULONG *pulFrameRate);

/**
* ������ʡ�\n
* @param [IN] ulPort        ����ͨ���š�
* @param [OUT] pulBitRate   ������ʵ�ָ�룬��λΪbps��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_COMMON_INVALID_PARAM            ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        ����ͨ����ý��ҵ�����Ͳ�֧�ִ˲���
* -         #ERR_XP_FAIL_TO_GET_BIT_RATE         �����������ʧ��
* @note
* -    1���ú�����Ҫ��#IMOS_XP_StartPlay֮����#IMOS_XP_StopPlay֮ǰ���ã�����׼ȷ���ָ������ͨ�������ʣ����⣬
*         �ú�����֧�ֻ�ñ����ļ��ط�ͨ�������ʣ�
* -    2���ýӿں���֧��Windows��Linux��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetBitRate(IN ULONG ulPort,
                                                       OUT ULONG *pulBitRate);


/**
* ������ҵ��\n
* @param [IN] ulVoiceSvcType  ����ҵ�����ͣ����#XP_VOICE_SERVICE_TYPE_Eö����ȡֵ��
* @param [IN] pcLocalIP       ����IP��ַ��
* @param [IN] usLocalPort     ���˼����˿ڡ�
* @param [IN] pcRemoteIP      Զ��IP��ַ��
* @param [IN] usRemotePort    Զ�˶˿ڡ�
* @param [IN] ulTransProtl    �շ�ý�����Ĵ����Э�飬���#XP_PROTOCOL_Eö����ȡֵ��
* @param [OUT] pulPort        ��������ҵ��ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             �������ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_OPEN_VOICE_SVC           ��������ҵ��ʧ��
* @note
* -    1�����ʹ�ñ���Ĭ�ϵ�IP��ַ���򱾵�IP��ַ����pcLocalIP������ַ�����
* -    2����������ҵ����ڲ��Ź����У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_MEDIA_VOICE��Я���Ĵ����룺
* -     #ERR_XP_FINISH_PLAY_FILE                �ϱ���Ϣ֮ǰ��������ֹͣ����ҵ��
* -     #ERR_XP_VOICE_RUNNING                   �ϱ���Ϣ֮ǰ��������ֹͣ����ҵ��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OpenVoiceSvc(IN ULONG ulVoiceSvcType,
                                                         IN const CHAR *pcLocalIP,
                                                         IN USHORT usLocalPort,
                                                         IN const CHAR *pcRemoteIP,
                                                         IN USHORT usRemotePort,
                                                         IN ULONG ulTransProtl,
                                                         OUT ULONG *pulPort);

/**
* ��ʼ����ҵ��\n
* @param [IN] ulPort ����ҵ��ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             �������ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_START_VOICE_SVC          ��������ҵ��ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartVoiceSvc(IN ULONG ulPort);

/**
* ֹͣ����ҵ��\n
* @param [IN] ulPort ����ҵ��ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             �������ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_STOP_VOICE_SVC           ֹͣ����ҵ��ʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopVoiceSvc(IN ULONG ulPort);

/**
* �ر�����ҵ��\n
* @param [IN] ulPort ����ҵ��ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             �������ͨ����Դʧ��
* -         #ERR_XP_FAIL_TO_CLOSE_VOICE_SVC          �ر�����ҵ��ʧ��
* -         #ERR_XP_NOT_START_PLAY                   ͨ��δ��ʼ���Ų���
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CloseVoiceSvc(IN ULONG ulPort);

/**
* ������ֹͣ�������Զ˷����������ݡ�\n
* @param [IN] bSend     �Ƿ��������Զ˷����������ݣ�BOOL_TRUE���������ͣ�BOOL_FALSE��ֹͣ���ͣ���
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_VOICE_SILENCE            ����/ֹͣ������������ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SendVoiceData(IN BOOL_T bSend);

/**
* ��˷��������ڡ�\n
* @param [IN] ulVolume ����ֵ����Χ��0��255��255Ϊ�����������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_MIC_NOT_EXIST            ϵͳ����˷����䲻����
* -         #ERR_XP_FAIL_TO_ADJUST_VOLUME    ������������ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetMicVolume(IN ULONG ulVolume);

/**
* �����˷�����ֵ��\n
* @param [OUT] pulVolume ����ֵ��ָ�루��Χ��0��255��255Ϊ�����������
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED              �ɹ�
* -         #ERR_COMMON_INVALID_PARAM        ��������Ƿ�
* -         #ERR_XP_MIC_NOT_EXIST            ϵͳ����˷����䲻����
* -         #ERR_XP_FAIL_TO_GET_VOLUME       �������ֵʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetMicVolume(OUT ULONG *pulVolume);

/**
* ��������ʵʱץ�ġ�\n
* @param [IN] ulPort             ����ͨ���š�
* @param [IN] pcFileName         ͼ���ļ���ŵ�·���������ļ�����������չ������
* @param [IN] ulPicFormat        ͼ���ļ��ĸ�ʽ�����#XP_PICTURE_FORMAT_Eö����ȡֵ��
* @param [IN] ulInterval         ץ�ļ������λΪ���롣
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
* -    2����������ץ�ĺ���ץ�Ĺ����У�������ͨ��#IMOS_XP_SetPlayerRunMsgCB���õĻص������ϱ���Ϣ#XP_PLAYER_RUN_INFO_SERIES_SNATCH��Я���Ĵ����룺
* -     #ERR_XP_DISK_CAPACITY_WARN              �ϱ���Ϣ֮ǰ������������ץ��
* -     #ERR_XP_DISK_CAPACITY_NOT_ENOUGH        �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -     #ERR_XP_FAIL_TO_SERIES_SNATCH           �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -     #ERR_XP_NO_PICTURE_TO_SNATCH            �ϱ���Ϣ֮ǰ��������ֹͣץ��
* -    3���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartSnatchSeries(IN ULONG ulPort,
                                                              IN const CHAR *pcFileName,
                                                              IN ULONG ulPicFormat,
                                                              IN ULONG ulInterval);

/**
* ֹͣ����ʵʱץ�ġ�\n
* @param [IN] ulPort ����ͨ���š�
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                  �ɹ�
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         ��ò���ͨ����Դʧ��
* -         #ERR_XP_NOT_START_PLAY               ָ���Ĳ���ͨ��δ��ʼ���Ų���
* -         #ERR_XP_NOT_START_SERIES_SNATCH      ָ���Ĳ���ͨ��δ��������ץ�Ĳ���
* -         #ERR_XP_FAIL_TO_STOP_SERIES_SNATCH   ֹͣ����ץ�Ĳ���ʧ��
* @note
* -    1���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopSnatchSeries(IN ULONG ulPort);

/**
* ����ָ������ͨ����NAT���͡�\n
* @param [IN] ulPort            ����ͨ���š�
* @param [IN] ulNatMode         NAT���ͣ����#XP_NAT_MODE_Eö����ȡֵ��
* @param [IN] pcNatSessionStr   NAT�����ַ�����
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ���������Ч
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             ��ò���ͨ����Դʧ��
* @note
* -    1�������ַ����������ж��壬�����շ�˫�����뱣��һ�£�
* -    2���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetNatParam(IN ULONG ulPort,
                                                        IN ULONG ulNatMode,
                                                        IN const CHAR *pcNatSessionStr);

/**
* ������Ƶ����Դ��\n
* @param [IN] enWaveInSource     ��Ƶ����Դ��������Ƶ�ɼ��豸#XP_WAVEIN_SOURCE_DEVICE����Ƶ�ļ�#XP_WAVEIN_SOURCE_FILE������Ƶ����Դ��
*                                ���#XP_WAVEIN_SOURCE_Eö����ȡֵ��
* @param [IN] pWaveInParameter   ������Ϣ����������Ƶ����ԴΪ#XP_WAVEIN_SOURCE_FILEʱ����Ҫʹ��#XP_WAVEIN_FILE_INFO_S�ṹ�������Ӳ�����
*                                ����Ƶ����ԴΪ#XP_WAVEIN_SOURCE_DEVICEʱ���ò�������NULL��
* @return �������´����룺
* -         #ERR_COMMON_SUCCEED                      �ɹ�
* -         #ERR_COMMON_INVALID_PARAM                ��������Ƿ�
* -         #ERR_XP_FAIL_TO_SET_WAVEIN_MOD           ������Ƶ����Դʧ��
* @note
* - 1���ڲ����øýӿں���������£�Ĭ�ϲ�����Ƶ�ɼ��豸��Ϊ��Ƶ����Դ��
* - 2����������Ƶ�ļ���Ϊ��Ƶ����Դʱ��Ҫ���븽����Ϣ���ο�#XP_WAVEIN_FILE_INFO_S�ṹ�壻
* - 3����Ƶ�ļ�ֻ֧��8000������+16λ����+��������wave��ʽ��Ƶ�ļ���
* - 4���ýӿں�����֧��Windows��
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetWaveInSourceMode(IN XP_WAVEIN_SOURCE_E enWaveInSource, IN VOID *pWaveInParameter);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_PLAYER_H_ */
