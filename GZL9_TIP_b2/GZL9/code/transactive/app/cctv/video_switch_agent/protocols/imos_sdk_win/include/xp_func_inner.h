/*******************************************************************************
Copyright (c) 2008, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
            xp_func_inner.h

Project Code: IMOS XP播放器
Module Name: IMOS XP播放器
Date Created: 2010-04-14
Author: shiyouhua/04562
Description: 播放器XP内部接口函数头文件

--------------------------------------------------------------------------------
Modification History
DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_
#define _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_

/* 包含基础平台的头文件*/
#include "imos_public.h"
#include "imos_errcode.h"
#include "imos_def.h"

/* 包含播放器SDK数据结构头文件 */
#include "xp_datatype.h"
#include "xp_func.h"

#ifdef  __cplusplus
extern "C"{
#endif

/**
* 初始化播放器资源。\n
* @param [IN] pcBaseModuleCode 基础平台BP模块的编码。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED          成功
* -         #ERR_COMMON_INVALID_PARAM    输入参数非法
* -         #ERR_XP_FAIL_TO_REG_BP_CMD   注册基础平台BP的命令行模块ID失败
* -         #ERR_XP_FAIL_TO_INIT_XP      初始化播放器XP资源失败
* @note
* - 使用播放器软件包首先需要调用的函数。
*/
IMOS_EXPORT ULONG STDCALL IMOS_InitXP(IN const CHAR *pcBaseModuleCode);

/**
* 释放播放器资源。\n
* @param [IN] 无。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED          成功
* -         #ERR_XP_FAIL_TO_CLEANUP_XP   释放播放器XP资源失败
* @note 退出播放器软件包最后一个需要调用函数。
*/
IMOS_EXPORT ULONG STDCALL IMOS_CleanupXP();

/**
* 注册播放器。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pstXPInfo          播放器信息结构体。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* -         #ERR_XP_FAIL_TO_REGIST_XP        注册播放器XP失败
* @note 无。
*/
IMOS_EXPORT ULONG STDCALL IMOS_RegisterXP(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const XP_INFO_S *pstXPInfo);

/**
* 注销播放器。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* @note 无。
*/
IMOS_EXPORT ULONG STDCALL IMOS_UnregisterXP(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo);

/**
* 添加播放窗格资源编码，一次可添加多个播放窗格资源编码。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pstWndCode         播放窗格编码结构体指针。
* @param [IN] ulPlayWndNum       播放窗格数量。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* -         #ERR_XP_EXCEED_MAX_PLAY_PORT_NUM 注册的播放通道数已超过了最大支持的播放通道数  
* -         #ERR_XP_FAIL_TO_SET_PARAM        设置播放器XP的配置参数失败    
* @note 一个播放窗格资源编码对应一个播放通道。对于同一个用户登陆ID，同时最多支持的播放通道数为128。
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddPlayWndInfo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const PLAY_WND_INFO_S *pstWndCode,
                                              IN ULONG ulPlayWndNum);

/**
* 删除播放窗格资源编码，一次可删除多个播放窗格资源编码。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pstWndCode         播放窗格编码结构体指针。
* @param [IN] ulPlayWndNum       播放窗格数量。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* @note 无。
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeletePlayWndInfo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const PLAY_WND_INFO_S *pstWndCode,
                                                 IN ULONG ulPlayWndNum);

/**
* 单帧后退播放。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pcChannelCode      播放通道编码。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_PORT_NOT_REGISTER            播放通道没有注册
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        播放通道的媒体类型不支持此操作
* -         #ERR_XP_NOT_START_PLAY               播放通道未开始播放操作
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       当前正在本地录像
* -         #ERR_XP_FAIL_TO_STEP_PLAY            单帧后退播放失败
* @note 正在本地录像时，不支持单帧后退播放操作。
*/
IMOS_EXPORT ULONG STDCALL IMOS_OneByOneBack(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                            IN const CHAR *pcChannelCode);

/**
* 设置媒体数据流格式。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pcChannelCode      播放通道编码。
* @param [IN] ulMediaDataformat  媒体数据流格式，需从#XP_MEDIA_DATA_FORMAT_E枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                      成功
* -         #ERR_COMMON_INVALID_PARAM                输入参数非法
* -         #ERR_XP_NOT_SUPPORT_MEDIA_STREAM_TYPE    不支持的码流类型
* -         #ERR_XP_PORT_NOT_REGISTER                播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             获得播放通道资源失败
* @note 无。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaDataFormat(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                  IN const CHAR *pcChannelCode,
                                                  IN ULONG ulMediaDataformat);

/**
* 启动指定通道的即时回放。\n
* @param [IN] pstUserLoginIDInfo       用户登录ID信息标识。
* @param [IN] pcChannelCode            播放通道编码。
* @param [IN] pcReplayChannelCode      播放回放视频编码
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER            通道没有注册
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        播放通道的媒体业务类型不支持此操作
* -         #ERR_XP_NOT_START_PLAY               指定的播放通道未开始播放操作
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放窗口资源失败
* -         #ERR_XP_FAIL_TO_START_REALREPLAY     开始即时回放失败
* @note     pcReplayChannelCode 的资源编码应为有效的通道编码
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartRealReplay(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,   
                                               IN const CHAR *pcReplayChannelCode);

/**
* 设置指定通道的即时回放时长。\n
* @param [IN] pstUserLoginIDInfo       用户登录ID信息标识。
* @param [IN] pcChannelCode            播放通道编码。
* @param [IN] ulTime                   即时回放时长。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER            通道没有注册
* -         #ERR_XP_NOT_SUPPORT_MEDIA_SVC        播放通道的媒体业务类型不支持此操作
* -         #ERR_XP_NOT_START_PLAY               指定的播放通道未开始播放操作
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放窗口资源失败
* -         #ERR_XP_SET_REALREPLAY_TIME_LARGE    即时回放设置时间太长            
* @note     时间取值范围[0 - 60]s
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetRealReplayTime(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN ULONG ulTime);

/* BEGIN: Added by jinqifeng j06715, 2010/12/15   PN:VVD51508 */
/**
* 设置网络媒体流的端口范围。\n
* @param [IN] usMinPort            起始端口。
* @param [IN] usMaxPort            结束端口。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* @note
* -    1、范围包括设置的端口。
* -    2、在调用 IMOS_InitXP之前调用。
* -    3、入参均为零时或不调用时，采用默认值。
* -    4、默认值范围为32768～65535之间的偶数。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaPortRange(IN USHORT usMinPort, 
                                                 IN USHORT usMaxPort);

/**
* 设置信令的端口范围。\n
* @param [IN] usMinPort         起始端口。
* @param [IN] usMaxPort         结束端口。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* @note
* -    1、范围包括设置的端口。
* -    2、在调用 IMOS_InitXP之前调用。
* -    3、入参均为零时或不调用时，采用默认值。
* -    4、默认值范围为6001～10000。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetProtlPortRange(IN USHORT usMinPort, 
                                                 IN USHORT usMaxPort);
/* END:   Added by jinqifeng j06715, 2010/12/15 */

/**
* 启动下载媒体流备份，将接收到的媒体流数据回调给用户。\n
* @param [IN]  pstUserLoginIDInfo       用户登录ID信息标识。
* @param [IN]  pcDownUrl                需下载媒体数据源的URL。
* @param [IN]  pcServerIP               媒体流控服务器的IP。
* @param [IN]  usServerPort             媒体流控服务器的端口号。
* @param [IN]  ulProtl                  接收媒体流的协议类型，需从tagProtocol枚举中取值（保留，目前仅支持TCP协议）。
* @param [IN]  ulDownMediaSpeed         媒体数据下载速度，需从tagDownMediaSpeed枚举中取值。
* @param [IN]  pfnSourceMediaDataCBFun  下载媒体流数据回调函数的指针。
* @param [IN]  lUserParam               用户设置参数。
* @param [OUT] pcDownloadID             下载通道号，缓冲区长度应不小于IMOS_RES_CODE_LEN。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_DOWNLOAD_PORT_NUM_LIMIT      限制下载通道数
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得下载通道资源失败
* -         #ERR_XP_FAIL_TO_SETUP_PROTOCOL       建立流控协商失败
* -         #ERR_XP_FAIL_TO_PLAY_PROTOCOL        流控协商播放（下载）失败
* -         #ERR_XP_FAIL_TO_OPEN_NET_MEDIA       启动媒体处理失败
* -         #ERR_XP_FAIL_TO_START_DOWNLOAD       启动下载失败
* -         #ERR_XP_RTSP_REQ_TIMEOUT             流控业务处理超时
* -         #ERR_XP_RTSP_NOT_ENOUGH_BANDWIDTH    流控业务带宽不足
* -         #ERR_XP_RTSP_SERVINTERERR            流控业务内部错误
* @note
* -         1、该函数调用成功后，返回给用户下载通道号，用于标识一路下载通道。
* -         2、最大同时支持启动32路下载通道。
* -         3、目前仅支持TCP协议下载媒体流。
* -         4、停止媒体流下载备份须调用IMOS_StopDownMediaStreamCallback接口。
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
* 停止下载媒体流备份。\n
* @param [IN]  pstUserLoginIDInfo  用户登录ID信息标识。
* @param [IN]  pcDownloadID        下载通道号。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得下载通道资源失败
* @note 该接口与IMOS_StartDownMediaStreamCallback接口匹配。
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopDownMediaStreamCallback(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                           IN const CHAR *pcDownloadID);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_MEDIA_PLAYER_SDK_INNER_FUNC_H_ */



