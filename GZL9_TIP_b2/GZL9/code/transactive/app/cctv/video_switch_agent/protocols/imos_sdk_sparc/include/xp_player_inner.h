/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
xp_player_inner.h

Project Code: IMOS通用媒体播放器
Module Name: IMOS通用媒体播放器
Date Created: 2012-03-31
Author: jinqifeng 00350
Description: IMOS通用媒体播放器XP内部接口

--------------------------------------------------------------------------------
Modification History
DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_XP_PLAYER_INNER_H_
#define _IMOS_XP_PLAYER_INNER_H_

/* 包含IMOS基础平台的头文件 */
#include "imos_public.h"
#include "imos_errcode.h"
#include "imos_def.h"

/* 包含播放器SDK数据结构头文件 */
#include "xp_datatype.h"
#include "xp_player.h"

#ifdef  __cplusplus
extern "C"{
#endif

/**
* 启动指定抓拍数目的连续实时抓拍。\n
* @param [IN] ulPort             播放通道号。
* @param [IN] pcFileName         图像文件存放的路径，包括文件名（不带扩展名）。
* @param [IN] ulPicFormat        图像文件的格式，需从#XP_PICTURE_FORMAT_E枚举中取值。
* @param [IN] ulInterval         抓拍间隔，单位为毫秒。
* @param [IN] ulCount            抓拍数目。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_DISK_CAPACITY_NOT_ENOUGH     硬盘空间不足
* -         #ERR_XP_NOT_START_PLAY               指定的播放通道未开始播放操作
* -         #ERR_XP_ALREADY_START_SERIES_SNATCH  播放通道已经启动了连续抓拍的操作
* -         #ERR_XP_NOT_SUPPORT_SNATCH_FORMAT    抓拍格式不支持
* -         #ERR_XP_FAIL_TO_START_SERIES_SNATCH  启动连续抓拍操作失败
* @note
* -
* -    1、播放器XP会在文件名后面附加类似于"20080928(13-10-50-99)"格式的抓拍时间戳字符串，
* -       单位时间内成功抓拍的最多图片数和机器性能有关；
* -    2、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartSnatchSeriesEx(IN ULONG ulPort,
                                                                IN const CHAR *pcFileName,
                                                                IN ULONG ulPicFormat,
                                                                IN ULONG ulInterval,
                                                                IN ULONG ulCount);

/**
* 设置指定通道的数字放大功能。\n
* @param [IN] ulPort                   播放通道号
* @param [IN] hWnd                     画中画句柄,hWnd = IMOS_INVALID_HANDLE表示关闭，非IMOS_INVALID_HANDLE表示开启
* @param [IN] pstRect                  原始图像区域坐标，pstRect = NULL为全部显示，该坐标值为区域在图像中的百分比值
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_NOT_SUPPORT                  不支持该功能
* -         #ERR_XP_FAIL_TO_SET_DIGITALZOOM      设置电子放大失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDigitalZoom(IN ULONG ulPort,
                                                           IN IMOS_HANDLE hWnd,
                                                           IN XP_RECT_S *pstRect);

/**
* 获取指定通道的图片信息。\n
* @param [IN] ulPort                   播放通道号
* @param [OUT] pulWidth                图片的宽
* @param [OUT] pulHeight               图片的高
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得播放通道资源失败
* -         #ERR_XP_FAIL_TO_GET_PIC_PARAM        获得当前通道解码后图片长宽信息失败
* -         #ERR_COMMON_INVALID_PARAM            参数错误
* @note
* - 1、该接口函数仅支持Windows。
*/

IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetPicSize(IN ULONG ulPort,
                                                       OUT ULONG *pulWidth,
                                                       OUT ULONG *pulHeight);

/**
* 设置是否启动乱序整理功能。\n
* @param [IN] bAdjust 是否开启乱序整理功能
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_SET_PARAM            设置播放器的配置参数失败
* @note
* -  1、当前只支持UDP+TS中的视频包乱序功能
* -  2、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_AdjustPktSeq(IN BOOL_T bAdjust);

/**
* 对指定通道做图（不规则区域）
* @param [IN] ulPort             播放通道号
* @param [IN] pDrawHandle        画图句柄
* @param [IN] ulNum              不规则区域顶点个数，最大为6
* @param [IN] pointArry          坐标地址
* @param [IN] ulDir              方向,需从#XP_AREA_DIRECTION_E枚举中取值。
* @param [IN] ulColor            线条颜色
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_PolylineTo(IN ULONG ulPort,
                                                      IN IMOS_HANDLE pDrawHandle,
                                                      IN ULONG ulNum,
                                                      IN XP_POINT_S *pointArry,
                                                      IN ULONG ulDir,
                                                      IN ULONG ulColor);

/**
* 对指定通道做图（阴影线）
* @param [IN] ulPort             播放通道号
* @param [IN] pDrawHandle        画图句柄
* @param [IN] pstStart           直线起点
* @param [IN] pstEnd             直线终点
* @param [IN] ulDir，            线条方向,需从#XP_THRU_LINE_DIRECTION_E枚举中取值
* @param [IN] ulColor            线条颜色
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_DLineTo(IN ULONG ulPort,
                                                   IN IMOS_HANDLE pDrawHandle,
                                                   IN const XP_POINT_S *pstStart,
                                                   IN const XP_POINT_S *pstEnd,
                                                   IN ULONG ulDir,
                                                   IN ULONG ulColor);

/**
* 获取画图句柄
* @param [IN] ulPort             播放通道号
* @return 返回如下结果：
* - 成功：IMOS_HANDLE
* - #
* - 失败：IMOS_INVALID_HANDLE
* -
* @note
* - 1、需要在视频上做图，首先需要调用IMOS_GetHVDC获取HVDC，才可以做图并显示；调用IMOS_ReleaseHVDC取消做图数据显示
* - 2、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT IMOS_HANDLE STDCALL IMOS_XP_GetHVDC(IN ULONG ulPort);

/**
* 释放画图句柄
* @param [IN] ulPort             播放通道号
* @param [IN] pDrawHandle        通过IMOS_GetHVDC获取的句柄
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_ReleaseHVDC(IN ULONG ulPort,
                                                       IN IMOS_HANDLE pDrawHandle);
/**
* 开始做图
* @param [IN] ulPort                   播放通道号
* @param [IN] pDrawHandle              画图句柄
* @param [IN] pverBer                  保留参数
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_BEGINHVDC           开始画图失败
* @note
* - 1、在图像上画线，首先需要在#IMOS_BeginDraw和#IMOS_EndDraw 之间操作，其他操作无效
* - 2、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_BeginDraw(IN ULONG ulPort,
                                                      IN IMOS_HANDLE pDrawHandle,
                                                      IN VOID *pverBer);

/**
* 完成做图
* @param [IN] ulPort             播放通道号
* @param [IN] pDrawHandle        画图句柄
* @return 无
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_EndDraw(IN ULONG ulPort,
                                                   IN IMOS_HANDLE pDrawHandle);

/**
* 设置解码端智能规则显示使能
* @param [IN] ulPort             播放通道号
* @param [IN] bEnable            是否显示
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERULE   设置显示智能规则失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ShowAnalyseRule(IN ULONG ulPort,
                                                            IN const BOOL_T bEnable);

/**
* 设置解码端智能结果显示使能
* @param [IN] ulPort        播放通道号
* @param [IN] bEnable       是否显示
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_ENABLEANALYSERESULT 设置显示智能结果失败
* @note
* - 1、该接口函数仅支持Windows。
*/

IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_ShowAnalyseResult(IN ULONG ulPort,
                                                              IN const BOOL_T bEnable);

/**
* 影像调节\n
* @param [IN] ulPort        播放通道号
* @param [IN] pstAdjust     影像调节参数
* @param [IN] bOpen         是否打开影像调节
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY              播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                 不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_ADJUST    设置影像调节失败
* @note
*      1、该函数需要在IMOS_XP_StartPlay之后和IMOS_XP_StopPlay之前调用。
* -    2、默认不启动影像调节。
* -    3、对比度取值范围为[0--199]，默认值为100；
* -       亮度取值范围为[0--199]，默认值为100；
* -       色调取值范围为[0--359]，默认值为0；
* -       饱和度取值范围为[0--359]，默认值为100；
* -       伽玛取值范围为[0--99]，默认值为10。
* -    4、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoAdjust(IN ULONG ulPort,
                                                           IN const XP_VIDEO_ADJUST_S *pstAdjust,
                                                           IN BOOL_T bOpen);

/**
* 模糊处理\n
* @param [IN] ulPort        播放通道号
* @param [IN] ulFactor      模糊处理参数
* @param [IN] bOpen         是否启动模糊处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_MONITOR_BLUR  设置模糊处理失败
* @note
* -    1、该函数需要在IMOS_XP_StartPlay之后和IMOS_XP_StopPlay之前调用。
* -    2、默认不启动模糊处理。
* -    3、模糊处理参数取值范围为[1--127]，默认值为80。
* -    4、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoMotionBlur(IN ULONG ulPort,
                                                               IN ULONG ulFactor,
                                                               IN BOOL_T bOpen);

/**
* 锐化处理\n
* @param [IN] ulPort        播放通道号
* @param [IN] fSigma        锐化处理参数
* @param [IN] bOpen         是否启动锐化处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_VIDEO_SHARPEN       设置锐化处理失败
* @note
* -    1、该函数需要在IMOS_XP_StartPlay之后和IMOS_XP_StopPlay之前调用。
* -    2、默认不启动锐化处理。
* -    3、锐化处理参数取值范围为[0.0--2.0]，默认值为0.05。
* -    4、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoSharpen(IN ULONG ulPort,
                                                            IN FLOAT fSigma,
                                                            IN BOOL_T bOpen);

/**
* 去雾处理\n
* @param [IN] ulPort            播放通道号
* @param [IN] ulLowValue        暗区调节参数
* @param [IN] ulHighValue       亮区调节参数
* @param [IN] bOpen             是否启动去雾处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_DE_FOG              设置去雾处理失败
* @note
* -    1、该函数需要在IMOS_XP_StartPlay之后和IMOS_XP_StopPlay之前调用。
* -    2、默认不启动去雾处理。
* -    3、去雾处理参数1取值范围为[0--49]，默认值为0；
* -       去雾处理参数2取值范围为[51--100]，默认值为100。
* -    4、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetDeFog(IN ULONG ulPort,
                                                     IN ULONG ulLowValue,
                                                     IN ULONG ulHighValue,
                                                     IN BOOL_T bOpen);

/**
* 设置码流丢包情况下拼帧策略。\n
* @param [IN] ulDataLostSplitterPolicy  码流丢包情况下拼帧策略，需从#XP_MEDIA_DATA_LOST_SPLITTER_POLICY枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* @note
* -    1、码流丢包情况下默认丢弃一个I帧组
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetMediaDataLostSplitterPolicy(IN ULONG ulDataLostSplitterPolicy);

/**
* 防闪烁处理\n
* @param [IN] ulPort            播放通道号
* @param [IN] ulWinSize         参考帧数
* @param [IN] ulSoftenVal       帧间滤波参数
* @param [IN] bOpen             是否启动防闪烁处理
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                     成功
* -         #ERR_COMMON_INVALID_PARAM               输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES            获得播放通道资源失败
* -         #ERR_XP_NOT_START_PLAY                  播放通道未开始播放操作
* -         #ERR_XP_NOT_SUPPORT                     不支持该功能
* -         #ERR_XP_FAIL_TO_SET_ANTI_FLICKER        设置防闪烁处理失败
* @note 
* -    1、该函数需要在IMOS_XP_StartPlay之后和IMOS_XP_StopPlay之前调用。
* -    2、默认不启动防闪烁处理。
* -    3、ulWinSize取值范围为[0--100]；
* -       ulSoftenVal取值范围为[0--30]。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetAntiFlicker(IN ULONG ulPort,
                                                           IN ULONG ulWinSize, 
                                                           IN ULONG ulSoftenVal,
                                                           IN BOOL_T bOpen);

/**
* 设置图象的对比度参数。\n
* @param [IN] ulPort             播放通道号。
* @param [IN] ulContrastMode     图象对比度模式，需从#XP_PICTURE_CONTRAST_MODE_E枚举中取值。
* @param [IN] ulContrastLevelEx  图象对比度级别，需从#XP_PICTURE_CONTRAST_LEVEL_EX_E枚举中取值。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED              成功
* -         #ERR_COMMON_INVALID_PARAM        输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES     获得播放通道资源失败
* -         #ERR_XP_NOT_SUPPORT_SET_CONTRAST 播放通道不支持设置图象对比度参数
* -         #ERR_XP_FAIL_TO_SET_CONTRAST     设置图象对比度失败
* @note
* -    1、在不调用该接口的情况下，视频图像默认不进行对比度调节处理。
* -    2、可以在开始播放IMOS_XP_StartPlay之前设置，但设置的对比度参数被保存，作为启动该通道播放时
* -       的初始值；在开始播放后调用该接口设置的对比度参数即时生效。
* -    3、调用该接口对指定播放通道设置的对比参数在播放器运行期间一直有效；当参数ulContrastMode
* -       传入XP_PICTURE_CONTRAST_MODE_NONE时，即停止对比度调节处理，此时ulContrastLevel参数传
* -       入的值无效。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetPictureContrastEx(IN ULONG ulPort,
                                                                 IN ULONG ulContrastMode,
                                                                 IN ULONG ulContrastLevelEx);

/**
* 单帧后退播放。\n
* @param [IN] ulPort   播放通道号。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_NOT_START_PLAY               播放通道未开始播放操作
* -         #ERR_XP_IN_RECORD_VIDEO_STATUS       当前正在本地录像
* -         #ERR_XP_FAIL_TO_STEP_PLAY            单帧播放失败
* @note
* -    1、正在本地录像时，不支持单帧后退播放操作；
* -    2、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_OneByOneBack(IN ULONG ulPort);

/**
* 检测运行系统的硬件参数
* @param [IN] 无。
* @return 返回#XP_HW_STATUS_E枚举类型的参数
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_CheckHWValidate();

/**
* 启动图像拼接操作
* @param [IN] ulChannelID                   播放图像拼接的通道号
* @param [IN] pcCombineResCode              图像拼接资源编码
* @param [IN] ulCombineStreamNum             图像拼接的通道数目
* @param [IN] pulCombineStreamChannelID     图像拼接通道数组
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_FAIL_TO_START_COMBINE_IMAGE 启动图像拼接操作失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartCombineVideo(IN const ULONG ulChannelID,
                                                              IN const CHAR  *pcCombineResCode,
                                                              IN const ULONG ulCombineStreamNum,
                                                              IN const ULONG *pulCombineStreamChannelID);
/**
* 停止图像拼接操作
* @param [IN] ulChannelID           播放图像拼接的通道号
* @return 返回如下结果：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放通道资源失败
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_IMAGE  停止图像拼接操作失败
* @note
* - 1、该接口函数仅支持Windows。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopCombineVideo(IN const ULONG ulChannelID);

/**
* 标定图像拼接特征点
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
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_MarkCombinePoint(IN const CHAR  *pcCombineResCode,
                                                             IN LONG  lCombineStreamNum,
                                                             IN LONG  *plCombinPointNum,
                                                             IN LONG  *plCombinPointVal,
                                                             IN LONG  lReserve);

/**
* 启动视频图像电子放大功能。\n
* @param [IN] ulChannelID           播放电子放大的通道号
* @param [IN] pcCombineResCode      图像拼接资源编码
* @param [IN] pstComobineZoomRect   电子放大区域信息
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_START_COMBINE_ZOOM   启动电子放大失败
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StartCombineZoom(IN const ULONG ulChannelID,
                                                             IN const CHAR  *pcCombineResCode,
                                                             IN const XP_COMBINE_ZOOM_RECT_S *pstComobineZoomRect);

/**
* 修改电子放大区域信息。\n
* @param [IN] ulChannelID           播放电子放大的通道号
* @param [IN] pcCombineResCode      图像拼接资源编码
* @param [IN] pstComobineZoomRect   电子放大区域信息
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_MOVE_COMBINE_ZOOM    修改电子放大区域信息失败
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_MoveCombineZoom(IN const ULONG ulChannelID,
                                                            IN const CHAR  *pcCombineResCode,
                                                            IN const XP_COMBINE_ZOOM_RECT_S *pstComobineZoomRect);


/**
* 关闭视频图像电子放大功能。\n
* @param [IN] ulChannelID           播放电子放大的通道号
* @param [IN] pcCombineResCode      图像拼接资源编码
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_COMMON_INVALID_PARAM            输入参数无效
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获得通道资源失败
* -         #ERR_XP_FAIL_TO_STOP_COMBINE_ZOOM    关闭电子放大功能失败
* @note
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_StopCombineZoom(IN const ULONG ulChannelID,
                                                            IN const CHAR  *pcCombineResCode);

/**
* 判断是否为DB33解码标准接入。\n
* @param [IN] pcDecodetag    decodetag信息
* @return BOOL_TRUE：是DB33解码插件标准接入； BOOL_FALSE：非DB33解码插件标准接入
* @note 无
*/
IMOS_XP_PLAYER_EXPORT BOOL_T STDCALL IMOS_XP_IsDB33Decodetag(IN const CHAR *pcDecodetag);

/**
* 设置解码模式 \n
* @param [IN] ulChannelID        用户登录ID信息标识
* @param [IN] ulMode                    解码模式 0 代表软解模式
* @return
* @note
* - 1、该接口函数仅支持Windows。
* - 2、该接口为实验性接口，会导致程序稳定性下降，不推荐用户使用。
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetVideoDecodeMode(IN const ULONG ulChannelID,
                                                               IN ULONG ulMode);

/**
* 获取空闲端口号。\n
* @param [INOUT] pulPort    播放通道号                       
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                  成功
* -         #ERR_XP_FAIL_TO_GET_PORT_RES         获取资源失败
* @note 无
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_GetPort(INOUT ULONG *pulPort);

/**
* 释放端口号。\n
* @param [IN] ulPort    播放通道号                       
* @return 无
* @note 无
*/
IMOS_XP_PLAYER_EXPORT VOID STDCALL IMOS_XP_FreePort(IN ULONG ulPort);


/**
* 设置通道的原始厂商信息。\n
* @param [IN] ulPort        播放通道号。
* @param [IN] pcDecoderTag  任意解码器标签。
* @return 返回如下错误码：
* -         #ERR_COMMON_SUCCEED                 成功
* -         #ERR_COMMON_INVALID_PARAM           输入参数非法
* -         #ERR_XP_FAIL_TO_GET_PORT_RES        获得播放窗口的资源失败
* @note 该接口会将传入的DecoderTag修改为各厂商的原始码流对应的DecoderTag;
*/
IMOS_XP_PLAYER_EXPORT ULONG STDCALL IMOS_XP_SetRawDecoderTag(IN ULONG ulPort,
                                                             IN const CHAR *pcDecoderTag);
#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_XP_PLAYER_INNER_H_ */
