/*******************************************************************************
 Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
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

/* begin add by shiyouhua04562 即时回放项目16秒问题 */
/**
* 开始播放（扩展接口：可以指定起始播放时间和播放速率参数）。
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pcChannelCode      播放通道编码。
* @param [IN] pcTime             设置文件播放位置到指定时间（时间格式为：yyyy-mm-dd hh:mm:ss）。
* @param [IN] ulPlaySpeed        播放速率，需从tagPlayStatus枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER            指定的播放通道没有注册
* -         #ERR_XP_PORT_NOT_OPEN                指定的播放通道未启动媒体业务
* -         #ERR_XP_FAIL_TO_PLAY_MEDIA           媒体播放处理失败
* -         #ERR_XP_FAIL_TO_PLAY_PROTOCOL        流控协商播放失败
* -         #ERR_XP_FAIL_TO_START_PLAY           开始播放失败
* @note
* 1、该接口在用户设置的录像时间点启动指定倍速播放点播回放，用于规避启动即时回放后图片OSD乱的问题。
* 2、SDK用户在调用IMOS_OpenVodStream之后调用该接口，替代IMOS_StartPlay。
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartPlayEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                           IN const CHAR *pcChannelCode,
                                           IN const char *pcTime,
                                           IN ULONG ulPlaySpeed);
/* end add by shiyouhua04562 即时回放项目16秒问题 */

/**
* 设置指定通道的数字放大功能。\n
* @param [IN] pstUserLoginIDInfo       用户登录ID信息标识。
* @param [IN] pcChannelCode            播放通道编码。
* @param [IN] hWnd                     画中画句柄,hWnd = IMOS_INVALID_HANDLE表示关闭，非IMOS_INVALID_HANDLE表示开启
* @param [IN] pstRect                  原始图像区域坐标，pstRect = NULL为全部显示，该坐标值为区域在图像中的百分比值
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_PORT_NOT_REGISTER            播放通道没有注册
* -         #ERR_XP_NOT_SUPPORT                  不支持该功能
* -         #ERR_XP_FAIL_TO_SET_DIGITALZOOM      设置电子放大失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDigitalZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN IMOS_HANDLE hWnd,
                                              IN XP_RECT_S *pstRect);

/**
* 获取图像大小。\n
* @param [IN] pstUserLoginIDInfo       用户登录ID信息标识。
* @param [IN] pcChannelCode            播放通道编码。
* @param [OUT] pulWidth                图像的宽
* @param [OUT] pulHeight               图像的长
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_FAIL_TO_GET_PIC_PARAM        获得当前通道解码后图片长宽信息失败
* -         #ERR_COMMON_INVALID_PARAM            参数错误
* @note
* - 1、该接口函数仅支持Windows。
*/

IMOS_EXPORT ULONG STDCALL IMOS_GetPicSize(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const CHAR *pcChannelCode,
                                          OUT ULONG *pulWidth,
                                          OUT ULONG *pulHeight);

/**
* 启动UDP包乱序整理功能。\n
* @param [IN] bAdjust                  是否启动乱序整理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_SET_PARAM            设置播放器的配置参数失败
* @note
* -  1、当前只支持UDP+TS中的视频包乱序功能
* -  2、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_AdjustPktSeq(IN BOOL_T bAdjust);


/**
* 对指定通道做图（不规则区域）
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pcChannelCode            播放通道编码
* @param [IN] pDrawHandle              画图句柄
* @param [IN] ulNum                    不规则区域顶点个数，最大为6
* @param [IN] pointArry                坐标地址
* @param [IN] ulDir                    方向，需从#IMOS_AREA_DIRECTION_E枚举中取值。
* @param [IN] ulColor                  线条颜色
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT VOID STDCALL IMOS_PolylineTo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                         IN const CHAR *pcChannelCode,
                                         IN IMOS_HANDLE pDrawHandle,
                                         IN ULONG ulNum,
                                         IN XP_POINT_S *pointArry,
                                         IN ULONG ulDir,
                                         IN ULONG ulColor);

/**
* 对指定通道做图（阴影线）
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] pDrawHandle              画图句柄
* @param [IN] pstStart                 直线起点
* @param [IN] pstEnd                   直线终点
* @param [IN] ulDir                    线条方向,需从#IMOS_THRU_LINE_DIRECTION_E枚举中取值。
* @param [IN] ulColor                  线条颜色
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT VOID STDCALL IMOS_DLineTo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                      IN const CHAR *pcChannelCode,
                                      IN IMOS_HANDLE pDrawHandle,
                                      IN const XP_POINT_S *pstStart,
                                      IN const XP_POINT_S *pstEnd,
                                      IN ULONG ulDir,
                                      IN ULONG ulColor);
/**
* 对指定通道做图（阴影线）
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] pDrawHandle              画图句柄
* @param [IN] pstStart                 直线起点
* @param [IN] pstEnd                   直线终点
* @param [IN] ulDir                    线条方向,需从#IMOS_THRU_LINE_DIRECTION_E枚举中取值。
* @param [IN] ulColor                  线条颜色
* @return 无
* @note
* - 1、该接口函数仅支持Windows。IMOS_DLineTo指标单位为1%, IMOS_DLineToEx坐标单位为0.01%
*/
IMOS_EXPORT VOID STDCALL IMOS_DLineToEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                        IN const CHAR *pcChannelCode,
                                        IN IMOS_HANDLE pDrawHandle,
                                        IN const XP_POINT_S *pstStart,
                                        IN const XP_POINT_S *pstEnd,
                                        IN ULONG ulDir,
                                        IN ULONG ulColor);

/**
* 获取画图句柄
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @return 返回如下结果：
* - 成功：IMOS_HANDLE
* - #
* - 失败：IMOS_INVALID_HANDLE
* -
* @note
* - 1、需要在视频上做图，首先需要调用IMOS_GetHVDC获取HVDC，才可以做图并显示；调用IMOS_ReleaseHVDC取消做图数据显示
* - 2、该接口函数仅支持Windows。
*/
IMOS_EXPORT IMOS_HANDLE STDCALL IMOS_GetHVDC(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                             IN const CHAR *pcChannelCode);

/**
* 释放画图句柄
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] pDrawHandle              通过IMOS_GetHVDC获取的句柄
* @return 返回如下结果：
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT VOID STDCALL IMOS_ReleaseHVDC(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                          IN const CHAR *pcChannelCode,
                                          IN IMOS_HANDLE pDrawHandle);
/**
* 开始做图
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码。
* @param [IN] pDrawHandle              画图句柄
* @param [IN] pverBer                  保留参数
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER           播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_BEGINHVDC           开始画图失败
* @note
* - 1、图像上画线，首先需要在IMOS_BeginDraw和IMOS_EndDraw之间操作，其他操作无效
* - 2、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_BeginDraw(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                         IN const CHAR *pcChannelCode,
                                         IN IMOS_HANDLE pDrawHandle,
                                         IN VOID *pverBer);

/**
* 完成做图
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] pDrawHandle              画图句柄
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT VOID STDCALL IMOS_EndDraw(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                      IN const CHAR *pcChannelCode,
                                      IN IMOS_HANDLE pDrawHandle);

/**
* 设置解码端智能规则显示使能
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] ulMethod                 输入解码端信息叠加方法信息
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER           播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERULE   设置显示智能规则失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShowAnalyseRule(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const BOOL_T bEnable );

/**
* 设置解码端智能结果显示使能
* @param [IN] pstUserLogIDInfo         用户登录ID信息标识
* @param [IN] pstChannelIndex          播放通道编码
* @param [IN] ulMethod                 输入解码端信息叠加方法信息
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER           播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERESULT 设置显示智能结果失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShowAnalyseResult(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN const BOOL_T bEnable );
/**
* 检测运行系统的硬件参数
* @param [IN] 无。
* @return 返回#XP_HW_STATUS_E枚举类型的参数
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_CheckHWValidate();

/**
* 启动图像拼接操作。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放图像拼接视频的通道编码
* @param [IN] pcCombineResCode          图像拼接资源编码
* @param [IN] lCombineStreamNum         图像拼接的通道数目
* @param [IN] ppcNeedCombineChannelCode 图像拼接的通道编码数组
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                      启动图像拼接成功
* -         #ERR_COMMON_INVALID_PARAM                输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES             获得通道资源失败
* -         #ERR_XP_PORT_NOT_REGISTER                播放通道没有注册
* -         #ERR_XP_FAIL_TO_START_COMBINE_IMAGE      启动图像拼接操作失败
* @note
* - 1、当前只支持3个通道的资源拼接
* - 2、拼接前需要预先打开需要拼接的视频播放业务
* - 3、ppcNeedCombineChannelCode为数组
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCombineVideo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                 IN const CHAR *pcChannelCode,
                                                 IN const CHAR *pcCombineResCode,
                                                 IN const LONG lCombineStreamNum,
                                                 IN const CHAR **ppcNeedCombineChannelCode);

/**
* 停止图像拼接操作。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             图像拼接资源编码
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_IMAGE   停止图像拼接操作失败
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCombineVideo(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcChannelCode);

/**
* 标定图像拼接特征点。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcCombineResCode          图像拼接资源编码
* @param [IN] lCombineStreamNum         图像拼接资源通道数目
* @param [IN] plCombinPointNum          相邻图像特征点数目的数组
* @param [IN] plCombinPointVal          用户设置拼接图像特征点值数组
* @param [IN] lReserve                  保留值，后续作拼接模式使用，当前填写0
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_MARK_COMBINE_IMAGE   标定特征点失败
* @note
* - 标识为数组各值需要连续存放
*/
IMOS_EXPORT ULONG STDCALL IMOS_MarkCombinePoint(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcCombineResCode,
                                                IN LONG lCombineStreamNum,
                                                IN LONG *plCombinPointNum,
                                                IN LONG *plCombinPointVal,
                                                IN LONG lReserve);

/**
* 启动视频图像电子放大功能。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放电子放大视频的通道编码
* @param [IN] pcCombineResCode          图像拼接资源编码
* @param [IN] pstCombineZoomRect        电子放大区域信息
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_START_COMBINE_ZOOM   启动电子放大失败
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                IN const CHAR *pcChannelCode,
                                                IN const CHAR *pcCombineResCode,
                                                IN const XP_COMBINE_ZOOM_RECT_S *pstCombineZoomRect);

/**
* 修改电子放大区域信息。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放电子放大视频的通道编码
* @param [IN] pcCombineResCode          图像拼接资源编码
* @param [IN] pstCombineZoomRect        电子放大区域信息
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_MOVE_COMBINE_ZOOM    修改电子放大区域信息失败
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_MoveCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const CHAR *pcCombineResCode,
                                               IN const XP_COMBINE_ZOOM_RECT_S *pstCombineZoomRect);

/**
* 关闭视频图像电子放大功能 \n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放电子放大视频的通道编码
* @param [IN] pcCombineResCode          图像拼接资源编码
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_ZOOM    关闭电子放大功能失败
* @note
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCombineZoom(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN const CHAR *pcCombineResCode);



/* begin add by shiyouhua04562 省厅项目（下载） */
/**
* 判断是否为DB33解码标准接入。\n
* @param [IN] pcDecodetag    decodetag信息
* @return BOOL_TRUE：是DB33解码插件标准接入； BOOL_FALSE：非DB33解码插件标准接入
* @note 无
*/
IMOS_EXPORT BOOL_T STDCALL IMOS_IsDB33Decodetag(IN const CHAR *pcDecodetag);
/* end add by shiyouhua04562 省厅项目（下载） */

/**
* 设置解码模式 \n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放的通道编码
* @param [IN] ulMode                    解码模式 0 代表软解模式
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* @note
* - 1、该接口函数仅支持Windows。
* - 2、该接口为实验性接口，会导致程序稳定性下降，不推荐用户使用。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoDecodeMode(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN ULONG ulMode);

/**
* 影像调节\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放通道编码
* @param [IN] pstAdjust                 影像调节参数
* @param [IN] bOpen                     是否打开影像调节
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER           播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_ADJUST    设置影像调节失败
* @note
* -    1、默认不启动影像调节。
* -    2、对比度取值范围为[0--199]，默认值为100；
* -       亮度取值范围为[0--199]，默认值为100；
* -       色调取值范围为[0--359]，默认值为0；
* -       饱和度取值范围为[0--359]，默认值为100；
* -       伽玛取值范围为[0--99]，默认值为10。
* -    3、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoAdjust(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN const XP_VIDEO_ADJUST_S *pstAdjust,
                                              IN BOOL_T bOpen);

/**
* 模糊处理\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放通道编码
* @param [IN] ulFactor                  模糊处理参数
* @param [IN] bOpen                     是否启动模糊处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER               播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_MONITOR_BLUR  设置模糊处理失败
* @note
* -    1、默认不启动模糊处理。
* -    2、模糊处理参数取值范围为[1--127]，默认值为80。
* -    3、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoMotionBlur(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                  IN const CHAR *pcChannelCode,
                                                  IN ULONG ulFactor,
                                                  IN BOOL_T bOpen);

/**
* 锐化处理\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放通道编码
* @param [IN] fSigma                    锐化处理参数
* @param [IN] bOpen                     是否启动锐化处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER               播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_SHARPEN       设置锐化处理失败
* @note
* -    1、默认不启动锐化处理。
* -    2、锐化处理参数取值范围为[0.0--2.0]，默认值为0.05。
* -    3、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetVideoSharpen(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                               IN const CHAR *pcChannelCode,
                                               IN FLOAT fSigma,
                                               IN BOOL_T bOpen);

/**
* 去雾处理\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放通道编码
* @param [IN] ulLowValue                暗区调节参数
* @param [IN] ulHighValue               亮区调节参数
* @param [IN] bOpen                     是否启动去雾处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER               播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_DE_FOG              设置去雾处理失败
* @note
* -    1、默认不启动去雾处理。
* -    2、去雾处理参数1取值范围为[0--49]，默认值为0；
* -       去雾处理参数2取值范围为[51--100]，默认值为100。
* -    3、该接口函数仅支持Windows。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetDeFog(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                        IN const CHAR *pcChannelCode,
                                        IN ULONG ulLowValue,
                                        IN ULONG ulHighValue,
                                        IN BOOL_T bOpen);

/**
* 设置码流丢包情况下拼帧策略。\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] ulDataLostSplitterPolicy 码流丢包情况下拼帧策略，需从#XP_MEDIA_DATA_LOST_SPLITTER_POLICY枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* @note
* - 1、码流丢包情况下默认不继续拼帧
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetMediaDataLostSplitterPolicy(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                            IN ULONG ulDataLostDecodePolicy);

/**
* 防闪烁处理\n
* @param [IN] pstUserLoginIDInfo        用户登录ID信息标识
* @param [IN] pcChannelCode             播放通道编码
* @param [IN] ulWinSize                 参考帧数
* @param [IN] ulSoftenVal               帧间滤波参数
* @param [IN] bOpen                     是否启动防闪烁处理处理
* @return 返回如下错误码：
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_PORT_NOT_REGISTER               播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_ANTI_FLICKER        设置防闪烁处理失败
* @note 
* -    1、默认不启动去雾处理。
* -    2、ulWinSize取值范围为[0--100]；
* -       ulSoftenVal取值范围为[0--30]。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetAntiFlicker(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                              IN const CHAR *pcChannelCode,
                                              IN ULONG ulWinSize, 
                                              IN ULONG ulSoftenVal,
                                              IN BOOL_T bOpen);

/**
* 设置图象的对比度参数。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pcChannelCode      播放通道编码。
* @param [IN] ulContrastMode     图象对比度模式，需从#XP_PICTURE_CONTRAST_MODE_E枚举中取值。
* @param [IN] ulContrastLevelEx  图象对比度级别，需从#XP_PICTURE_CONTRAST_LEVEL_EX_E枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* -         #ERR_XP_PORT_NOT_REGISTER        播放通道没有注册
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     获得播放通道资源失败
* -         #ERR_XP_NOT_SUPPORT_SET_CONTRAST 播放通道不支持设置图象对比度参数
* -         #ERR_XP_FAIL_TO_SET_CONTRAST     设置图象对比度参数失败
* @note
* -    1、在不调用该接口的情况下，视频图像默认不进行对比度调节处理。
* -    2、可以在开始播放IMOS_StartPlay之前设置，但设置的对比度参数被保存，作为启动该通道播放时
* -       的初始值；在开始播放后调用该接口设置的对比度参数即时起作用。
* -    3、调用该接口对指定播放通道设置的对比参数在播放器运行期间一直有效；当参数ulContrastMode
* -       传入XP_PICTURE_CONTRAST_MODE_NONE时，即停止对比度调节处理，此时ulContrastLevel传入的
* -       值无效。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetPictureContrastEx(IN const USER_LOGIN_ID_INFO_S *pstUserLoginIDInfo,
                                                    IN const CHAR *pcChannelCode,
                                                    IN ULONG ulContrastMode,
                                                    IN ULONG ulContrastLevelEx);

/**
* 启动指定抓拍数目的连续实时抓拍。\n
* @param [IN] pstUserLoginIDInfo 用户登录ID信息标识。
* @param [IN] pcChannelCode      播放通道编码。
* @param [IN] pcFileName         图像文件存放的路径，包括文件名（不带扩展名）。
* @param [IN] ulPicFormat        图像文件的格式，需从#XP_PICTURE_FORMAT_E枚举中取值。
* @param [IN] ulInterval         抓拍间隔，单位为毫秒。
* @param [IN] ulCount            抓拍数目（取值大于0）。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_PORT_NOT_REGISTER            播放通道没有注册
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH     硬盘空间不足
* -         #ERR_XP_NOT_START_PLAY               播放通道未开始播放操作
* -         #ERR_XP_ALREADY_START_SERIES_SNATCH  播放通道已经启动了连续抓拍的操作
* -         #ERR_XP_NOT_SUPPORT_SNATCH_FORMAT    抓拍格式不支持
* -         #ERR_XP_FAIL_TO_START_SERIES_SNATCH  启动连续抓拍操作失败
* @note
* - 1、播放器会在文件名后面附加“抓拍系统时间-系统毫秒级时间”格式的时间戳字符串，如“20110228110325-1997060991”；
* - 2、单位时间内成功抓拍的最多图片数和机器性能有关；
* - 3、播放器会在最终的图像文件名后附加上后缀名；
* - 4、启动连续抓拍后，在抓拍过程中，播放器通过#IMOS_SetRunMsgCB设置的回调函数上报抓拍消息#XP_RUN_INFO_SERIES_SNATCH；携带的错误码：
* -     #ERR_XP_DISK_CAPACITY_WARN          播放器内继续抓拍
* -     #ERR_XP_DISK_CAPACITY_NOT_ENOUGH    上报消息之前，播放器停止抓拍
* -     #ERR_XP_NO_PICTURE_TO_SNATCH        上报消息之前，播放器停止抓拍
* -     #ERR_XP_FAIL_TO_SERIES_SNATCH       上报消息之前，播放器停止抓拍
* -     #ERR_XP_FINISH_SERIES_SNATCH        上报消息之前，播放器停止抓拍
* - 5、该接口函数仅支持Windows。
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



