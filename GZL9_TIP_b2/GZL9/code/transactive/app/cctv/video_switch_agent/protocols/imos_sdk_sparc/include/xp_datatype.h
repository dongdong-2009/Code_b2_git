/*******************************************************************************
 Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              xp_datatype.h

  Project Code: IMOS通用媒体播放器
   Module Name: IMOS通用媒体播放器
  Date Created: 2008-09-10
        Author: shiyouhua/04562
   Description: 播放器XP的错误码和数据结构类型定义头文件

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_MEDIA_PLAY_SDK_DATA_TYPE_H_
#define _IMOS_MEDIA_PLAY_SDK_DATA_TYPE_H_

/************************************************************************/
/*********************** XP SDK错误码定义********************************/
/**********************[0x000700—0x0008FF] *****************************/
/************************************************************************/

/* 启动和停止播放器XP返回错误码 */
#define ERR_XP_FAIL_TO_REG_BP_CMD                       0x000700        /**< 播放器XP的子模块向基础平台BP注册失败 */
#define ERR_XP_FAIL_TO_INIT_XP                          0x000701        /**< 初始化播放器XP失败 */
#define ERR_XP_FAIL_TO_CLEANUP_XP                       0x000702        /**< 释放播放器XP资源失败 */
#define ERR_XP_FAIL_TO_START_XP                         0x000703        /**< 启动播放器XP失败 */
#define ERR_XP_FAIL_TO_STOP_XP                          0x000704        /**< 停止播放器XP失败 */
#define ERR_XP_FAIL_TO_REGIST_XP                        0x000705        /**< 注册播放器XP失败 */
#define ERR_XP_FAIL_TO_UNREGIST_XP                      0x000706        /**< 注销播放器XP失败 */
#define ERR_XP_FAIL_TO_SET_PARAM                        0x000707        /**< 设置播放器XP的配置参数失败 */
#define ERR_XP_EXCEED_MAX_PLAY_WINDOW_NUM               0x000708        /**< 注册的播放窗口数已超过了最大支持的播放窗口数 */
#define ERR_XP_WINDOW_ALREADY_SET                       0x000709        /**< 通道已被注册 */
#define ERR_XP_EXCEED_MAX_PLAY_PORT_NUM                 0x00070A        /**< 注册的播放通道数已超过了最大支持的播放通道数 */
#define ERR_XP_NOT_SUPPORT                              0x00070B        /**< 不支持该功能*/

/* 加载和释放动态库资源返回错误码 */
#define ERR_XP_DLL_NOT_EXIST                            0x000720        /**< 指定的动态库不存在 */
#define ERR_XP_FAIL_TO_LOAD_DLL                         0x000721        /**< 加载动态库失败 */
#define ERR_XP_FAIL_TO_UNLOAD_DLL                       0x000722        /**< 卸载动态库失败 */
#define ERR_XP_FAIL_TO_WRITE_FILE                       0x000723        /**< 写文件操作失败 */

#define ERR_XP_FUNCTION_CALL_ORDER_ERROR                0x000725        /**< 函数调用顺序错误 */

/* 播放器XP媒体操作返回错误码 */
#define ERR_XP_WINDOW_NOT_REGISTER                      0x000730        /**< 播放窗口没有注册 */
#define ERR_XP_PORT_NOT_AVAILABLE                       0x000731        /**< 通道不可用 */
#define ERR_XP_FAIL_TO_GET_PORT_RES                     0x000732        /**< 获得播放通道资源失败 */
#define ERR_XP_NOT_SUPPORT_MEDIA_SVC                    0x000733        /**< 播放通道的媒体业务类型不支持此操作 */
#define ERR_XP_PORT_NOT_REGISTER                        0x000734        /**< 播放通道没有注册 */
#define ERR_XP_NOT_SUPPORT_MEDIA_ENCODE_TYPE            0x000735        /**< 播放通道的媒体编码格式不支持此操作 */
#define ERR_XP_MEDIA_RESOLUTION_CHANGE                  0x000736        /**< 播放通道的媒体流分辨率发生变化 */

#define ERR_XP_FAIL_TO_OPEN_REALTIME                    0x000741        /**< 启动实时监控业务失败 */
#define ERR_XP_FAIL_TO_OPEN_VOD                         0x000742        /**< 启动点播回放业务失败 */
#define ERR_XP_FAIL_TO_OPEN_FILE                        0x000743        /**< 启动本地文件回放业务失败(用户没有权限，或者该文件正在被其他程序使用) */
#define ERR_XP_FAIL_TO_START_PLAY                       0x000744        /**< 开始播放失败 */
#define ERR_XP_FAIL_TO_PAUSE                            0x000745        /**< 暂停播放失败 */
#define ERR_XP_FAIL_TO_RESUME                           0x000746        /**< 恢复播放失败 */
#define ERR_XP_FAIL_TO_STOP_PLAY                        0x000747        /**< 停止播放失败 */
#define ERR_XP_FAIL_TO_MULTIPL_SPEED_PLAY               0x000748        /**< 倍速前进/后退播放失败 */
#define ERR_XP_FAIL_TO_STEP_PLAY                        0x000749        /**< 单帧播放失败 */
#define ERR_XP_FAIL_TO_DRAG_PLAY                        0x00074A        /**< 跳转播放失败 */
#define ERR_XP_FAIL_TO_GET_PLAY_POSITION                0x00074B        /**< 获得当前播放进度失败 */
#define ERR_XP_FAIL_TO_QUIET                            0x00074C        /**< 启用或停止声音播放失败 */
#define ERR_XP_FAIL_TO_ADJUST_VOLUME                    0x00074D        /**< 调节音量失败 */
#define ERR_XP_FAIL_TO_GET_VOLUME                       0x00074E        /**< 获得音量值失败 */
#define ERR_XP_FAIL_TO_SNATCH_ONCE                      0x00074F        /**< 单次抓拍失败 */
#define ERR_XP_FAIL_TO_START_SERIES_SNATCH              0x000750        /**< 启动连续抓拍失败 */
#define ERR_XP_FAIL_TO_SERIES_SNATCH                    0x000751        /**< 连续抓拍过程中抓拍失败 */
#define ERR_XP_FAIL_TO_STOP_SERIES_SNATCH               0x000752        /**< 停止连续抓拍失败 */
#define ERR_XP_FAIL_TO_START_RECORD_VIDEO               0x000753        /**< 启动本地录像失败 */
#define ERR_XP_FAIL_TO_STOP_RECORD_VIDEO                0x000754        /**< 停止本地录像失败 */
#define ERR_XP_FAIL_TO_RECORD_VIDEO                     0x000755        /**< 本地录像过程中出错 */
#define ERR_XP_FAIL_TO_START_RECORD_AUDIO               0x000756        /**< 启动录音失败 */
#define ERR_XP_FAIL_TO_STOP_RECORD_AUDIO                0x000757        /**< 停止录音失败 */
#define ERR_XP_FAIL_TO_RECORD_AUDIO                     0x000758        /**< 录音过程中出错 */
#define ERR_XP_DOWNLOAD_PORT_NUM_LIMIT                  0x000759        /**< 超过最大下载通道数 */
#define ERR_XP_FAIL_TO_START_DOWNLOAD                   0x00075A        /**< 启动下载录像失败 */
#define ERR_XP_FAIL_TO_GET_DOWNLOAD_POSITION            0x00075B        /**< 获得当前录像下载进度失败 */
#define ERR_XP_MIC_NOT_EXIST                            0x00075C        /**< 系统中麦克风适配不存在 */
#define ERR_XP_NO_PICTURE_TO_SNATCH                     0x00075D        /**< 没有解码过的图片可供抓拍 */
#define ERR_XP_DECODE_AUDIO_NOT_CONTINUE                0x00075E        /**< 音频流数据解码后不继续进行播放操作 */
#define ERR_XP_FAIL_TO_OPEN_DOWNLOAD                    0x00075F        /**< 启动下载业务失败 */
#define ERR_XP_IN_PAUSE_PLAY_STATUS                     0x000760        /**< 当前正处于暂停播放状态 */
#define ERR_XP_IN_MULTIPL_SPEED_PLAY_STATUS             0x000761        /**< 当前正处于倍速播放状态 */
#define ERR_XP_IN_STEP_PLAY_STATUS                      0x000762        /**< 当前正处于单帧播放状态 */
#define ERR_XP_IN_RECORD_VIDEO_STATUS                   0x000763        /**< 当前正处于本地录像状态 */
#define ERR_XP_NOT_SUPPORT_DOWNLOAD_FORMAT              0x000764        /**< 下载格式不支持 */
#define ERR_XP_NOT_SUPPORT_SET_CONTRAST                 0x000765        /**< 播放通道不支持设置图像对比度参数 */
#define ERR_XP_FAIL_TO_SET_CONTRAST                     0x000766        /**< 设置图像对比度参数失败 */
#define ERR_XP_FAIL_TO_OPEN_VOICE_TALKBACK              0x000767        /**< 启动语音对讲失败 */
#define ERR_XP_FAIL_TO_CLOSE_VOICE_TALKBACK             0x000768        /**< 停止语音对讲失败 */
#define ERR_XP_FAIL_TO_OPEN_VOICE_BROADCAST             0x000769        /**< 启动语音广播失败 */
#define ERR_XP_FAIL_TO_CLOSE_VOICE_BROADCAST            0x00076A        /**< 停止语音广播失败 */
#define ERR_XP_ALREADY_START_VOICE_TALKBACK             0x00076B        /**< 语音对讲已经启动 */
#define ERR_XP_ALREADY_START_VOICE_BROADCAST            0x00076C        /**< 语音广播已经启动 */
#define ERR_XP_NOT_START_VOICE_SVC                      0x00076D        /**< 未启动语音业务 */
#define ERR_XP_NOT_START_VOICE_TALKBACK                 0x00076E        /**< 未启动语音对讲业务 */
#define ERR_XP_NOT_START_VOICE_BROADCAST                0x00076F        /**< 未启动语音广播业务 */
#define ERR_XP_FAIL_TO_VOICE_SILENCE                    0x000770        /**< 启动或停止向语音对端发送语音数据失败 */
#define ERR_XP_FAIL_TO_GET_LOST_PKT_RATE                0x000771        /**< 获得当前丢包率数据失败 */
#define ERR_XP_FAIL_TO_RESET_LOST_PKT_RATE              0x000772        /**< 复位丢包率失败 */
#define ERR_XP_FAIL_TO_GET_FRAME_RATE                   0x000773        /**< 获得当前帧率数据失败 */
#define ERR_XP_FAIL_TO_GET_BIT_RATE                     0x000774        /**< 获得当前码率数据失败 */
#define ERR_XP_FAIL_TO_RESET_BUFFER                     0x000775        /**< 清空缓冲区失败 */
#define ERR_XP_FAIL_TO_SET_FIELD_MODE                   0x000776        /**< 设置解码场模式失败 */
#define ERR_XP_FAIL_TO_ENABLE_DEINTERLACE               0x000777        /**< 启动或关闭去交织处理失败 */
#define ERR_XP_FAIL_TO_SET_DEINTERLACE_MODE             0x000778        /**< 设置去交织处理模式失败 */
#define ERR_XP_FAIL_TO_INPUT_DATA                       0x000779        /**< 输入媒体流数据失败 */
#define ERR_XP_NOT_SUPPORT_STREAM_FORMAT                0x00077A        /**< 不支持的码流类型 */
#define ERR_XP_FAIL_TO_SET_PROCESS_DATA_CB              0x00077B        /**< 设置媒体流数据回调函数失败 */
#define ERR_XP_FAIL_TO_ENABLE_OUTPUT_DATA               0x00077C        /**< 启动或关闭解码后媒体流数据输出失败 */
#define ERR_XP_FAIL_TO_SET_PORT_PARAM                   0x00077D        /**< 设置通道的参数失败 */
#define ERR_XP_FAIL_TO_START_VOICE_SVC                  0x00077E        /**< 启动语音业务失败 */
#define ERR_XP_FAIL_TO_STOP_VOICE_SVC                   0x00077F        /**< 停止语音业务失败 */
#define ERR_XP_FAIL_TO_GET_LOST_FRAME_RATE              0x000780        /**< 获得当前丢帧率数据失败 */
#define ERR_XP_FAIL_TO_RESET_LOST_FRAME_RATE            0x000781        /**< 复位丢帧率失败 */
#define ERR_XP_FAIL_TO_SET_WAVEIN_MOD                   0x000782        /**< 设置音频输入源失败 */
#define ERR_XP_FAIL_TO_SET_RENDER_SCALE                 0x000783        /**< 设置图像显示比例参数失败 */
#define ERR_XP_ALREADY_START_SERIES_SNATCH              0x000784        /**< 播放通道已经启动连续抓拍操作 */
#define ERR_XP_ALREADY_START_RECORD_VIDEO               0x000785        /**< 播放通道已经启动本地录像操作 */
#define ERR_XP_ALREADY_START_RECORD_AUDIO               0x000786        /**< 播放通道已经启动录音操作 */
#define ERR_XP_NOT_START_SERIES_SNATCH                  0x000787        /**< 播放通道未启动连续抓拍操作 */
#define ERR_XP_NOT_START_RECORD_VIDEO                   0x000788        /**< 播放通道未启动本地录像操作 */
#define ERR_XP_NOT_START_RECORD_AUDIO                   0x000789        /**< 播放通道未启动录音操作 */
#define ERR_XP_ALREADY_START_OUTPUT_MEDIA               0x00078A        /**< 播放通道已经启动媒体流输出操作 */
#define ERR_XP_ALREADY_STOP_OUTPUT_MEDIA                0x00078B        /**< 播放通道未启动媒体流输出操作 */
#define ERR_XP_FINISH_SERIES_SNATCH                     0x00078C        /**< 完成连续抓拍任务 */
#define ERR_XP_PORT_NOT_EXIST                           0x00078D        /**< 通道不存在 */
#define ERR_XP_PORT_ALREADY_EXIST                       0x00078E        /**< 通道已经存在 */
#define ERR_XP_PORT_ALREADY_OPEN                        0x00078F        /**< 通道已经启动 */
#define ERR_XP_PORT_NOT_OPEN                            0x000790        /**< 通道未打开媒体业务 */
#define ERR_XP_NOT_START_PLAY                           0x000791        /**< 播放通道未开始播放操作 */
#define ERR_XP_FAIL_TO_OPEN_NET_MEDIA                   0x000792        /**< 打开网络媒体失败 */
#define ERR_XP_FAIL_TO_PLAY_MEDIA                       0x000793        /**< 媒体播放处理失败 */
#define ERR_XP_FAIL_TO_PAUSE_MEDIA                      0x000794        /**< 暂停媒体播放处理失败 */
#define ERR_XP_FAIL_TO_RESUME_MEDIA                     0x000795        /**< 恢复媒体播放处理失败 */
#define ERR_XP_FAIL_TO_STOP_MEDIA                       0x000796        /**< 停止媒体播放处理失败 */
#define ERR_XP_NOT_SUPPORT_FILE_FORMAT                  0x000797        /**< 文件播放格式不支持 */
#define ERR_XP_NOT_SUPPORT_SNATCH_FORMAT                0x000798        /**< 抓拍格式不支持 */
#define ERR_XP_NOT_SUPPORT_RECORD_VIDEO_FORMAT          0x000799        /**< 录像格式不支持 */
#define ERR_XP_NOT_SUPPORT_RECORD_AUDIO_FORMAT          0x00079A        /**< 录音格式不支持 */
#define ERR_XP_DISK_CAPACITY_WARN                       0x00079B        /**< 硬盘剩余空间低于阈值 */
#define ERR_XP_DISK_CAPACITY_NOT_ENOUGH                 0x00079C        /**< 硬盘剩余空间不足，无法继续业务 */
#define ERR_XP_FAIL_TO_GET_TOTAL_TIME                   0x00079D        /**< 获得文件总的播放时间失败 */
#define ERR_XP_FINISH_PLAY_FILE                         0x00079E        /**< 文件已经播放完成 */
#define ERR_XP_FILE_DESTROY                             0x00079F        /**< 文件已损坏 */
#define ERR_XP_FILE_HAVE_OPEN                           0x0007A0        /**< 通道已经打开文件 */
#define ERR_XP_FAIL_TO_DECODE_VIDEO                     0x0007A1        /**< 视频解码失败 */
#define ERR_XP_FAIL_TO_DECODE_AUDIO                     0x0007A2        /**< 音频解码失败 */
#define ERR_XP_NOT_SUPPORT_MEDIA_STREAM_TYPE            0x0007A3        /**< 不支持的媒体码流类型 */
#define ERR_XP_NOT_SUPPORT_TRANSPORT_PROTOCOL_TYPE      0x0007A4        /**< 不支持的传输层协议类型 */
#define ERR_XP_FAIL_TO_RECV_DATA                        0x0007A5        /**< 网络故障导致接收数据失败 */
#define ERR_XP_FAIL_TO_SEND_DATA                        0x0007A6        /**< 网络故障导致发送数据失败 */
#define ERR_XP_FAIL_TO_OPERATE_AUDIO_EQUIPMENT          0x0007A7        /**< 音频设备操作失败 */
#define ERR_XP_FAIL_TO_GET_VIDEO_ENCODE_TYPE            0x0007A8        /**< 获得视频编码格式失败 */
#define ERR_XP_FAIL_TO_PROCESS_MEDIA_DATA               0x0007A9        /**< 媒体数据处理失败 */
#define ERR_XP_FAIL_TO_START_REALREPLAY                 0x0007AA        /**< 开始即时回放失败 */
#define ERR_XP_SET_REALREPLAY_TIME_LARGE                0x0007AB        /**< 设置即时回放时间太长 */
#define ERR_XP_VOICE_RUNNING                            0x0007AC        /**< 语音业务运行过程中出错*/
#define ERR_XP_FAIL_TO_OPEN_STREAM                      0x0007AD        /**< 启动媒体流播放失败 */
#define ERR_XP_FAIL_TO_CREATE_SOCKET                    0x0007AE        /**< Socket创建失败 */
#define ERR_XP_PLAY_ALREADY_START                       0x0007AF        /**< 通道已经启动播放 */

/* 流控协商 */
#define ERR_XP_FAIL_TO_SETUP_PROTOCOL                   0x0007B0        /**< 建立流控协商失败 */
#define ERR_XP_FAIL_TO_PLAY_PROTOCOL                    0x0007B1        /**< 流控协商播放失败 */
#define ERR_XP_FAIL_TO_PAUSE_PROTOCOL                   0x0007B2        /**< 流控协商暂停失败 */
#define ERR_XP_FAIL_TO_STOP_PROTOCOL                    0x0007B3        /**< 停止流控协商失败 */
#define ERR_XP_RTSP_COMPLETE                            0x0007B4        /**< RTSP播放或下载完成 */
#define ERR_XP_RTSP_ABNORMAL_TEATDOWN                   0x0007B5        /**< RTSP异常下线，服务器读取文件错误或数据被覆写 */
#define ERR_XP_RTSP_KEEP_LIVE_TIME_OUT                  0x0007B6        /**< RTSP保活失败 */
#define ERR_XP_RTSP_ENCODE_CHANGE                       0x0007B7        /**< RTSP中码流格式切换 */
#define ERR_XP_RTSP_DISCONNECT                          0x0007B8        /**< RTSP连接断开，点播回放或下载已自动终止，请检查网络 */
#define ERR_XP_RTSP_DISCONNECT_ONVIF                    0x0007B9        /**< ONVIF实况直连时，Rtsp连接断开，等待重连 */

/* 会话控制 */
#define ERR_XP_FAIL_TO_SETUP_SESSION                    0x0007C0        /**< 发起会话请求失败 */
#define ERR_XP_FAIL_TO_QUIT_SESSION                     0x0007C1        /**< 退出会话失败 */
#define ERR_XP_AUDIO_DEVICE_UNRIPE                      0x0007C2        /**< 音频设备未准备好 */

/* Rtsp 会话错误 */
#define ERR_XP_RTSP_REQ_TIMEOUT                         0x0007D1        /**< 流控业务处理超时 */
#define ERR_XP_RTSP_NOT_ENOUGH_BANDWIDTH                0x0007D2        /**< 流控业务带宽不足 */
#define ERR_XP_RTSP_SERVINTERERR                        0x0007D3        /**< 流控业务内部错误 */

/* 播放业务*/
#define ERR_XP_FAIL_TO_CLOSE_FILE                       0x0007D4        /**< 关闭文件播放失败 */
#define ERR_XP_FAIL_TO_CLOSE_STREAM                     0x0007D5        /**< 关闭媒体流播放失败 */
#define ERR_XP_INPUTDATA_BUFFER_FULL                    0x0007D6        /**< 输入数据缓存满 */
#define ERR_XP_MEDIA_STREAM_NOT_SUPPORT                 0x0007D7        /**< 无法识别（或不支持）的码流 */
#define ERR_XP_FAIL_TO_PLAY_SOUND                       0x0007D8        /**< 启动声音播放失败 */
#define ERR_XP_FAIL_TO_STOP_SOUND                       0x0007D9        /**< 关闭声音播放失败 */

#define ERR_XP_FAIL_TO_OPEN_VOICE_SVC                   0x0007DA        /**< 启动语音业务失败 */
#define ERR_XP_FAIL_TO_CLOSE_VOICE_SVC                  0x0007DB        /**< 关闭语音业务失败 */
#define ERR_XP_FAIL_TO_SET_DIGITALZOOM                  0x0007DC        /**< 设置数字放大失败 */
#define ERR_XP_FAIL_TO_GET_PIC_PARAM                    0x0007DD        /**< 通道获取图片信息错误 */
#define ERR_XP_FAIL_TO_BEGINHVDC                        0x0007DE        /**< 开始画图失败 */
#define ERR_XP_FAIL_TO_ENABLEANALYSERULE                0x0007DF        /**< 设置显示智能规则失败 */
#define ERR_XP_FAIL_TO_ENABLEANALYSERESULT              0x0007E0        /**< 设置显示智能结果失败 */
#define ERR_XP_FAIL_TO_START_COMBINE_IMAGE              0x0007E1        /**< 启动图像拼接操作失败 */
#define ERR_XP_FAIL_TO_MARK_COMBINE_IMAGE               0x0007E2        /**< 设定图像拼接特征点失败 */
#define ERR_XP_FAIL_TO_STOP_COMBINE_IMAGE               0x0007E3        /**< 停止图像拼接操作失败 */
#define ERR_XP_FAIL_TO_START_COMBINE_ZOOM               0x0007E4        /**< 启动拼接图片数字放大失败 */
#define ERR_XP_FAIL_TO_STOP_COMBINE_ZOOM                0x0007E5        /**< 停止拼接图片数字放大失败 */
#define ERR_XP_FAIL_TO_MOVE_COMBINE_ZOOM                0x0007E6        /**< 修改拼接图片数字放大失败 */
#define ERR_XP_FAIL_TO_SET_DECODER_MODE                 0x0007E7        /**< 设置解码模式失败 */
#define ERR_XP_FAIL_TO_SET_VIDEO_ADJUST                 0x0007E8        /**< 设置影像调节失败 */
#define ERR_XP_FAIL_TO_SET_VIDEO_MOTION_BLUR            0x0007E9        /**< 设置模糊处理失败 */
#define ERR_XP_FAIL_TO_SET_VIDEO_SHARPEN                0x0007EA        /**< 设置锐化处理失败 */
#define ERR_XP_FAIL_TO_SET_DE_FOG                       0x0007EB        /**< 设置去雾处理失败 */
#define ERR_XP_FAIL_TO_SET_ANTI_FLICKER                 0x0007EC        /**< 设置防闪烁处理失败 */

/************************************************************************/
/*************************** 文件后缀名长度******************************/
/************************************************************************/
#define IMOS_XP_FILE_POSTFIX_LEN    32


/************************************************************************/
/*********************** XP SDK数据结构类型定义**************************/
/************************************************************************/

/**
* @enum tagPlayType
* @brief 通道播放业务类型枚举定义
* @attention 无
*/
typedef enum tagPlayType
{
    XP_PLAY_REALTIME = 0,               /**< 实时监控 */
    XP_PLAY_VOD = 1,                    /**< 回放 */
    XP_PLAY_INTELLIGENT = 2             /**< 智能相关业务 */
}XP_PLAY_TYPE_E;

/**
* @enum tagPictureFluency
* @brief 图像播放流畅性枚举类型定义
* @attention 无
*/
typedef enum tagPictureFluency
{
    XP_PICTURE_TIME_PRIORITY = 0,         /**< 实时性优先 */
    XP_PICTURE_FLUENCY_PRIORITY = 1       /**< 流畅性优先 */
}XP_PICTURE_FLUENCY_E;

/**
* @enum tagRenderScale
* @brief 播放窗格显示图像的比例枚举类型定义
* @attention 无
*/
typedef enum tagRenderScale
{
    XP_RENDER_SCALE_FULL = 0,               /**< 图像填充整个播放窗格 */
    XP_RENDER_SCALE_PROPORTION = 1          /**< 图像按比例显示 */
}XP_RENDER_SCALE_E;

/**
* @enum tagPictureFormat
* @brief 图片格式枚举定义
* @attention 无
*/
typedef enum tagPictureFormat
{
    XP_PICTURE_BMP = 0,                 /**< 图片格式为bmp格式 */
    XP_PICTURE_JPG = 1                  /**< 图片格式为jpg格式 */
}XP_PICTURE_FORMAT_E;

/**
* @enum tagVideoEncodeType
* @brief 视频编码格式枚举定义
* @attention 无
*/
typedef enum tagVideoEncodeType
{
    XP_VIDEO_ENCODE_MPEG2 = 0,          /**< 视频符合MPEG-2标准 */
    XP_VIDEO_ENCODE_MPEG4 = 1,          /**< 视频符合MPEG-4标准 */
    XP_VIDEO_ENCODE_H264 = 2,           /**< 视频符合H.264标准 */
    XP_VIDEO_ENCODE_MJPEG = 3           /**< 视频符合MJPEG标准 */
}XP_VIDEO_ENCODE_TYPE_E;

/**
* @enum tagAudioEncodeType
* @brief 音频编码格式枚举定义
* @attention 无
*/
typedef enum tagAudioEncodeType
{
    XP_AUDIO_ENCODE_G711U = 0,          /**< 音频符合G.711U律标准 */
    XP_AUDIO_ENCODE_MP2 = 1             /**< 音频符合MP2编码标准 */
}XP_AUDIO_ENCODE_TYPE_E;

/**
* @enum tagMediaFileFormat
* @brief 媒体文件格式枚举定义
* @attention 无
*/
typedef enum tagMediaFileFormat
{
    XP_MEDIA_FILE_TS  = 0,              /**< TS格式的媒体文件 */
    XP_MEDIA_FILE_FLV = 1               /**< FLV格式的媒体文件 */
}XP_MEDIA_FILE_FORMAT_E;

/**
* @enum tagRenderMode
* @brief 视频显示模式枚举定义
* @attention 无
*/
typedef enum tagRenderMode
{
    XP_RENDER_MODE_D3D = 0,             /**< 使用D3D的方式进行显示 */
    XP_RENDER_MODE_DDRAW_NORMAL = 1     /**< 使用Ddraw的Normal显示模式 */
}XP_RENDER_MODE_E;

/**
* @enum tagPixelFormat
* @brief 视频像素格式枚举定义，主要在老接口函数XP_SetPixelFormat中使用。
* @attention 无
*/
typedef enum tagPixelFormat
{
    XP_PIXEL_FORMAT_YUV = 0,            /**< YUV格式输出 */
    XP_PIXEL_FORMAT_RGB = 1             /**< RGB格式输出 */
}XP_PIXEL_FORMAT_E;

/**
* @enum tagPixelFormatEx
* @brief 视频像素格式枚举增强定义，主要在新接口函数IMOS_SetPixelFormat中使用。
*        和tagPixelFormat枚举区别在于细化了视频像素格式
* @attention 无
*/
typedef enum tagPixelFormatEx
{
    XP_PIXEL_FORMAT_YUV420 = 0,         /**< YUV420格式输出 */
    XP_PIXEL_FORMAT_RGB32 = 1           /**< RGB32格式输出 */
}XP_PIXEL_FORMAT_EX_E;

/**
* @enum tagFieldMode
* @brief 场模式枚举定义
* @attention 无
*/
typedef enum tagFieldMode
{
    XP_FIELD_MODE_SINGLE = 0,           /**< 单场模式 */
    XP_FIELD_MODE_DOUBLE = 1            /**< 双场模式 */
}XP_FIELD_MODE_E;

/**
* @enum tagDeinterlaceMode
* @brief 去交织处理模式的枚举定义
* @attention 无
*/
typedef enum tagDeinterlaceMode
{
    XP_DEINTERLACE_NONE = 0,            /**< 不启用去交织处理 */
    XP_DEINTERLACE_DOUBLETOPFIELG = 1,  /**< 顶场拷贝到底场模式 */
    XP_DEINTERLACE_BLEND = 2,           /**< 混合模式 */
    XP_DEINTERLACE_X  = 3               /**< x模式 */
}XP_DEINTERLACE_MODE_E;

/**
* @enum tagWaveInSource
* @brief 音频输入源设置
* @attention 无
*/
typedef enum tagWaveInSource
{
    XP_WAVEIN_SOURCE_DEVICE = 0,        /**< 麦克风作音频输入源 */
    XP_WAVEIN_SOURCE_FILE = 1           /**< 音频文件作音频输入源 */
}XP_WAVEIN_SOURCE_E;

/**
* @enum tagRecordQuality
* @brief 录像质量枚举定义
* @attention 无
*/
typedef enum tagRecordQuality
{
    XP_RECORD_PRIORITY = 0,             /**< 播放时录像处理质量优先*/
    XP_PLAY_PRIORITY = 1                /**< 播放时播放处理优先*/
}XP_RECORD_QUALITY_E;

/**
* @enum tagMediaDataLostSplitterPolicy
* @brief 码流丢包情况下拼帧策略的枚举定义
* @attention 无
*/
typedef enum tagMediaDataLostSplitterPolicy
{
    XP_MEDIA_LOST_NO_SPLITTER = 0 ,       /**< 码流丢包情况下丢弃处理 */
    XP_MEDIA_LOST_CONTINUE_SPLITTER = 1   /**< 码流丢包情况下继续拼帧 */
}XP_MEDIA_DATA_LOST_SPLITTER_POLICY;

/**
* @enum tagPlayStatus
* @brief 播放状态枚举定义
* @attention 无
*/
typedef enum tagPlayStatus
{
    XP_PLAY_STATUS_16_BACKWARD = 0,     /**< 16倍速后退播放 */
    XP_PLAY_STATUS_8_BACKWARD = 1,      /**< 8倍速后退播放 */
    XP_PLAY_STATUS_4_BACKWARD = 2,      /**< 4倍速后退播放 */
    XP_PLAY_STATUS_2_BACKWARD = 3,      /**< 2倍速后退播放 */
    XP_PLAY_STATUS_1_BACKWARD = 4,      /**< 正常速度后退播放 */
    XP_PLAY_STATUS_HALF_BACKWARD = 5,   /**< 1/2倍速后退播放 */
    XP_PLAY_STATUS_QUARTER_BACKWARD = 6,/**< 1/4倍速后退播放 */
    XP_PLAY_STATUS_QUARTER_FORWARD = 7, /**< 1/4倍速播放 */
    XP_PLAY_STATUS_HALF_FORWARD = 8,    /**< 1/2倍速播放 */
    XP_PLAY_STATUS_1_FORWARD = 9,       /**< 正常速度前进播放 */
    XP_PLAY_STATUS_2_FORWARD = 10,      /**< 2倍速前进播放 */
    XP_PLAY_STATUS_4_FORWARD = 11,      /**< 4倍速前进播放 */
    XP_PLAY_STATUS_8_FORWARD = 12,      /**< 8倍速前进播放 */
    XP_PLAY_STATUS_16_FORWARD = 13      /**< 16倍速前进播放 */
}XP_PLAY_STATUS_E;

/**
* @enum tagRunInfoType
* @brief 上报消息类型的枚举定义
* @attention 无
*/
typedef enum tagRunInfoType
{
    XP_RUN_INFO_SERIES_SNATCH = 0,        /**< 连续抓拍过程中上报运行信息 */
    XP_RUN_INFO_RECORD_VIDEO = 1,         /**< 本地录像过程中上报运行信息 */
    XP_RUN_INFO_MEDIA_PROCESS = 2,        /**< 视频媒体处理过程中的上报运行信息 */
    XP_RUN_INFO_DOWN_MEDIA_PROCESS = 3,   /**< 媒体流下载过程中上报运行信息 */
    XP_RUN_INFO_VOICE_MEDIA_PROCESS = 4,  /**< 语音媒体处理过程中的上报运行信息 */
    XP_RUN_INFO_RTSP_PROTOCOL = 5,        /**< RTSP协议组件运行的错误信息 */
    XP_RUN_INFO_DOWN_RTSP_PROTOCOL = 6,   /**< 下载录像过程中RTSP协议的错误信息 */
    XP_RUN_INFO_SIP_LIVE_TIMEOUT = 7,     /**< SIP注册保活超时 */
    XP_RUN_INFO_PASSIVE_MONITOR = 8,      /**< 被动实况停止操作信息 */
    XP_RUN_INFO_PASSIVE_START_MONITOR = 9,/**< 被动实况启动操作信息 */
    XP_RUN_INFO_MEDIA_NOT_IDENTIFY = 10,  /**< 码流无法识别 */
    XP_RUN_INFO_RECV_PACKET_NUM = 11,     /**< 周期内接收到的包数 */
    XP_RUN_INFO_RECV_BYTE_NUM = 12,       /**< 周期内接收到的字节数 */
    XP_RUN_INFO_VIDEO_FRAME_NUM = 13,     /**< 周期内解析的视频帧数 */
    XP_RUN_INFO_AUDIO_FRAME_NUM = 14,     /**< 周期内解析的音频帧数 */
    XP_RUN_INFO_LOST_PACKET_RATIO = 15    /**< 周期内丢包率统计信息（单位为0.01%） */
}XP_RUN_INFO_TYPE_E;

/**
* @enum tagMediaDataType
* @brief 解码后媒体流数据类型的枚举定义
* @attention 无
*/
typedef enum tagMediaDataType
{
    XP_VIDEO_DATA_DECODE = 0,           /**< 解码后的视频数据 */
    XP_AUDIO_DATA_DECODE = 1            /**< 解码后的音频数据 */
}XP_OUTPUT_MEDIA_DATA_TYPE_E;

/**
* @enum tagWaveFormatInfo
* @brief 解码后音频格式的枚举类型定义
* @attention 无
*/
typedef enum tagWaveFormatInfo
{
    XP_WAVE_FORMAT_PCM8M16 = 0,         /**< PCM，8K采样率，单声道（M），16位采样 */
    XP_WAVE_FORMAT_PCM16M16 = 1,        /**< PCM，16K采样率，单声道（M），16位采样 */
    XP_WAVE_FORMAT_PCM441M16 = 2        /**< PCM，44.1K采样率，单声道（M），16位采样 */
}XP_WAVE_FORMAT_INFO_E;

/**
* @enum tagVoiceSvcType
* @brief 语音业务类型枚举定义
* @attention 无
*/
typedef enum tagVoiceSvcType
{
    XP_VOICE_SERVICE_BROADCAST = 0,        /**< 语音广播业务类型，语音发送 */
    XP_VOICE_SERVICE_TALKBACK = 1,         /**< 语音对讲业务类型，语音收发 */
    XP_VOICE_SERVICE_BE_BROADCASTED = 2    /**< 被语音广播业务类型，语音收 */
}XP_VOICE_SERVICE_TYPE_E;

/**
* @enum tagMediaDataFormat
* @brief 媒体数据流格式
* @attention 无
*/
typedef enum tagMediaDataFormat
{
    XP_MEDIA_DATA_TS = 0,               /**< TS封装码流 */
    XP_MEDIA_DATA_ES12 = 1,             /**< RTP + 音/视频私有头（12字节） + ES码流 */
    XP_MEDIA_DATA_ES12_TIME = 2,        /**< RTP + 时间戳（8字节） + 音/视频私有头（12字节） + ES码流 */
    XP_MEDIA_DATA_RTP_ES = 3,           /**< RTP + ES码流 */
    XP_MEDIA_DATA_HIK = 4,              /**< 海康码流，回调的第一个包是海康40字节系统头 */
    XP_MEDIA_DATA_DH = 5,               /**< 大华码流 */
    XP_MEDIA_DATA_RM = 6,               /**< 锐明码流，回调的第一个包是锐明40字节系统头 */
    XP_MEDIA_DATA_TDY_SWALLOW = 7,      /**< TDY-SWALLOW */
    XP_MEDIA_DATA_DMA_SWALLOW = 8,      /**< DMA-SWALLOW */
    XP_MEDIA_DATA_NANZI = 9             /**< 南自码流 */
}XP_MEDIA_DATA_FORMAT_E;

/**
* @enum tagNetworkTransportProtl
* @brief 网络传输协议，主要在老接口函数XP_SetTransportProtocol中使用。
* @attention 无
*/
typedef enum tagNetworkTransportProtl
{
    XP_NETWORK_TRANSPORT_PROTOCOL_UDP = 0,  /**< UDP协议 */
    XP_NETWORK_TRANSPORT_PROTOCOL_TCP = 1   /**< TCP协议 */
}XP_NETWORK_TRANSPORT_PROTOCOL_E;

/**
* @enum tagProtocol
* @brief 网络协议，主要在新接口函数IMOS_OpenVodStream和IMOS_StartDownMediaStream中使用。
* @attention 无
*/
typedef enum tagProtocol
{
    XP_PROTOCOL_UDP = 0,                   /**< UDP协议 */
    XP_PROTOCOL_TCP = 1,                   /**< TCP协议Client端*/
    XP_PROTOCOL_TCP_SERVER = 2             /**< TCP协议Server端*/
}XP_PROTOCOL_E;

/**
* @enum tagRenderTimeType
* @brief 时间戳枚举类型定义
* @attention 无
*/
typedef enum tagRenderTimeType
{
    XP_RENDER_FRAME_RATE = 0,               /**< 使用帧率数据进行视频渲染 */
    XP_RENDER_ABSOLUTE_TIME = 1             /**< 使用绝对时间数据进行视频渲染 */
}XP_RENDER_TIME_TYPE_E;

/**
* @enum tagDownMediaSpeed
* @brief 媒体数据下载速度的枚举定义
* @attention 无
*/
typedef enum tagDownMediaSpeed
{
    XP_DOWN_MEDIA_SPEED_ONE = 0,            /**< 一倍速下载媒体文件 */
    XP_DOWN_MEDIA_SPEED_TWO = 1,            /**< 二倍速下载媒体文件 */
    XP_DOWN_MEDIA_SPEED_FOUR = 2,           /**< 四倍速下载媒体文件 */
    XP_DOWN_MEDIA_SPEED_EIGHT = 3           /**< 八倍速下载媒体文件 */
}XP_DOWN_MEDIA_SPEED_E;

/**
* @enum tagNatMode
* @brief NAT方式的枚举类型
* @attention 无
*/
typedef enum tagNatMode
{
    XP_NAT_NONE = 0,                     /**< 无NAT的方式 */
    XP_NAT_IN   = 1,                     /**< NAT IN方式 */
    XP_NAT_OUT  = 2                      /**< NAT OUT方式 */
}XP_NAT_MODE_E;

/**
* @enum tagVideoFrameType
* @brief 视频帧类型的枚举定义
* @attention 无
*/
typedef enum tagVideoFrameType
{
    XP_VIDEO_FRAME_I = 0,               /**< 视频帧为I帧 */
    XP_VIDEO_FRAME_P = 1,               /**< 视频帧为P帧 */
    XP_VIDEO_FRAME_B = 2                /**< 视频帧为B帧 */
}XP_VIDEO_FRAME_TYPE_E;

/**
* @enum tagPictureContrastMode
* @brief 图像对比度模式设置
* @attention 无
*/
typedef enum tagPictureContrastMode
{
    XP_PICTURE_CONTRAST_MODE_NONE = 0,       /**< 不进行对比度调节处理 */
    XP_PICTURE_CONTRAST_MODE_BRIGHTNESS = 1, /**< 对比度明亮模式 */
    XP_PICTURE_CONTRAST_MODE_SOFTNESS = 2    /**< 对比度柔和模式 */
}XP_PICTURE_CONTRAST_MODE_E;

/**
* @enum tagPictureContrastLevel
* @brief 图像对比度级别设置
* @attention 无
*/
typedef enum tagPictureContrastLevel
{
    XP_PICTURE_CONTRAST_LEVEL_LOW = 0,       /**< 对比度低级别 */
    XP_PICTURE_CONTRAST_LEVEL_MIDDLE = 1,    /**< 对比度中级别 */
    XP_PICTURE_CONTRAST_LEVEL_HIGH = 2       /**< 对比度高级别 */
}XP_PICTURE_CONTRAST_LEVEL_E;

/**
* @enum tagPictureContrastLevelEx
* @brief 图像对比度级别设置
* @attention 无
*/
typedef enum tagPictureContrastLevelEx
{
    XP_PICTURE_CONTRAST_LEVEL_1 = 1,        /**< 对比度1级别 */
    XP_PICTURE_CONTRAST_LEVEL_2 = 2,        /**< 对比度2级别 */
    XP_PICTURE_CONTRAST_LEVEL_3 = 3,        /**< 对比度3级别 */
    XP_PICTURE_CONTRAST_LEVEL_4 = 4,        /**< 对比度4级别 */
    XP_PICTURE_CONTRAST_LEVEL_5 = 5,        /**< 对比度5级别 */
    XP_PICTURE_CONTRAST_LEVEL_6 = 6,        /**< 对比度6级别 */
    XP_PICTURE_CONTRAST_LEVEL_7 = 7,        /**< 对比度7级别 */
    XP_PICTURE_CONTRAST_LEVEL_8 = 8,        /**< 对比度8级别 */
    XP_PICTURE_CONTRAST_LEVEL_9 = 9,        /**< 对比度9级别 */
    XP_PICTURE_CONTRAST_LEVEL_10 = 10       /**< 对比度10级别 */
}XP_PICTURE_CONTRAST_LEVEL_EX_E;

/**
* @struct tagRunInfo
* @brief 上报消息的结构体定义，主要在回调函数XP_RUN_INFO_CALLBACK_PF中使用。
* @attention 无
*/
typedef struct tagRunInfo
{
    IMOS_HANDLE hWnd;                           /**< 播放窗格句柄 */
    ULONG ulErrCode;                            /**< 上报的错误码 */
}XP_RUN_INFO_S;

/**
* @struct tagRunInfoEx
* @brief 上报消息的增强结构体定义，主要在回调函数XP_RUN_INFO_CALLBACK_EX_PF中使用。
*        和tagRunInfo结构体区别在于使用通道资源编码替换播放窗口句柄。
* @attention 无
*/
typedef struct tagRunInfoEx
{
    CHAR szPortCode[IMOS_RES_CODE_LEN];         /**< 通道资源编码 */
    ULONG ulErrCode;                            /**< 上报的错误码 */
}XP_RUN_INFO_EX_S;

/**
* @struct tagStartMonitorRunInfo
* @brief 上报被动实况启动消息的结构体定义。
* @attention 无
*/
typedef struct tagStartMonitorRunInfo
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    CHAR szCameraID[IMOS_CODE_LEN];             /**< 摄像机编码 */
    ULONG ulErrCode;                            /**< 上报的错误码 */
    CHAR szReserved[64];                        /**< 保留参数 */
}XP_START_MONITOR_RUN_INFO_S;

/**
* @struct tagDownRunInfo
* @brief 媒体流下载过程中上报消息的结构体定义
* @attention 无
*/
typedef struct tagDownRunInfo
{
    CHAR szDownID[IMOS_RES_CODE_LEN];           /**< 媒体流下载ID */
    ULONG ulErrCode;                            /**< 上报的错误码 */
}XP_DOWN_RUN_INFO_S;

/**
* @struct tagVoiceRunInfo
* @brief 语音业务媒体流处理过程中上报消息的结构体定义
* @attention 无
*/
typedef struct tagVoiceRunInfo
{
    CHAR szVoiceID[IMOS_RES_CODE_LEN];           /**< 如果为依赖于协议的语音业务，则该参数为远端资源编码；否则为SDK接口函数的语音通道ID参数 */
    ULONG ulErrCode;                             /**< 上报的错误码 */
}XP_VOICE_RUN_INFO_S;

/**
* @struct tagRecvPktNum
* @brief 上报周期内接收包数消息的结构体定义。
* @attention 无
*/
typedef struct tagRecvPktNum
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    ULONG ulRecvPktNum;                         /**< 接收到的包数 */
    ULONG ulStatPeriod;                         /**< 统计的周期（毫秒）*/
}XP_RECV_PACKET_NUM_S;

/**
* @struct tagRecvByteNum
* @brief 上报周期内接收字节数消息的结构体定义。
* @attention 无
*/
typedef struct tagRecvByteNum
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    ULONG ulRecvByteNum;                        /**< 接收到的字节数 */
    ULONG ulStatPeriod;                         /**< 统计的周期（毫秒）*/
}XP_RECV_BYTE_NUM_S;

/**
* @struct tagVideoFrameNum
* @brief 上报周期内解析视频帧数消息的结构体定义。
* @attention 无
*/
typedef struct tagVideoFrameNum
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    ULONG ulVideoFrameNum;                      /**< 解析的视频帧数 */
    ULONG ulStatPeriod;                         /**< 统计的周期（毫秒） */
}XP_VIDEO_FRAME_NUM_S;

/**
* @struct tagAudioFrameNum
* @brief 上报周期内解析音频帧数消息的结构体定义。
* @attention 无
*/
typedef struct tagAudioFrameNum
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    ULONG ulAudioFrameNum;                      /**< 解析的音频帧数 */
    ULONG ulStatPeriod;                         /**< 统计的周期（毫秒） */
}XP_AUDIO_FRAME_NUM_S;

/**
* @struct tagLostPktRatio
* @brief 上报周期内丢包率消息的结构体定义。
* @attention 无
*/
typedef struct tagLostPktRatio
{
    CHAR szPortCode[IMOS_CODE_LEN];             /**< 通道资源编码 */
    ULONG ulLostPktRatio;                       /**< 丢包率信息（单位为0.01%） */
    ULONG ulStatPeriod;                         /**< 统计的周期（毫秒） */
}XP_LOST_PAKCET_RATIO_S;

/**
* @struct tagSourceDataExInfo
* @brief 拼帧前媒体流扩展信息的结构体定义。
* @attention 无
*/
typedef struct tagSourceDataExInfo
{
    CHAR szDecoderTag[IMOS_CODE_LEN];             /**< 解码器标签*/
    CHAR szReserved[64];                          /**< 保留参数*/
}XP_SOURCE_DATA_EX_INFO_S;

/**
* @struct tagParseVideoData
* @brief 存放拼帧后视频数据的指针和长度等信息的结构体定义
* @attention 无
*/
typedef struct tagParseVideoData
{
    UCHAR *pucData;             /**< 视频数据指针 */
    ULONG ulDataLen;            /**< 视频数据长度 */
    ULONG ulVideoFrameType;     /**< 视频帧类型，从#XP_VIDEO_FRAME_TYPE_E中取值 */
    ULONG ulVideoCodeFormat;    /**< 视频编码格式，从#XP_VIDEO_ENCODE_TYPE_E中取值 */
    ULONG ulHeight;             /**< 视频图像高度 */
    ULONG ulWidth;              /**< 视频图像宽度 */
    DLONG dlTimeStamp;          /**< 时间戳（毫秒）*/
    ULONG ulReserved1;          /**< 保留参数 */
    ULONG ulReserved2;          /**< 保留参数 */
}XP_PARSE_VIDEO_DATA_S;

/**
* @struct tagParseAudioData
* @brief 存放拼帧后音频数据的指针和长度等信息的结构体定义
* @attention 无
*/
typedef struct tagParseAudioData
{
    UCHAR *pucData;             /**< 音频数据指针 */
    ULONG ulDataLen;            /**< 音频数据长度 */
    ULONG ulAudioCodeFormat;    /**< 音频编码格式，从#XP_AUDIO_ENCODE_TYPE_E中取值 */
    ULONG ulWaveFormat;         /**< 音频数据解码后音频格式，对应#XP_WAVE_FORMAT_INFO_E枚举中的值 */
    DLONG dlTimeStamp;          /**< 时间戳（毫秒）*/
    ULONG ulReserved1;          /**< 保留参数 */
    ULONG ulReserved2;          /**< 保留参数 */
}XP_PARSE_AUDIO_DATA_S;

/**
* @struct tagPictureData
* @brief 存放解码后图像数据的指针和长度等信息的结构体定义
* @attention 无
*/
typedef struct tagPictureData
{
    UCHAR *pucData[4];                          /**< pucData[0]:Y 平面指针,pucData[1]:U 平面指针,pucData[2]:V 平面指针 */
    ULONG ulLineSize[4];                        /**< ulLineSize[0]:Y平面每行跨距, ulLineSize[1]:U平面每行跨距, ulLineSize[2]:V平面每行跨距 */
    ULONG ulPicHeight;                          /**< 图片高度 */
    ULONG ulPicWidth;                           /**< 图片宽度 */
    ULONG ulRenderTimeType;                     /**< 用于渲染的时间数据类型，对应tagRenderTimeType枚举中的值 */
    DLONG dlRenderTime;                         /**< 用于渲染的时间数据 */
}XP_PICTURE_DATA_S;

/**
* @struct tagWaveData
* @brief 存放解码后音频数据的指针和长度等信息的结构体定义
* @attention 无
*/
typedef struct tagWaveData
{
    CHAR *pcData;                               /**< 音频数据指针 */
    ULONG ulDataLen;                            /**< 音频数据长度 */
    ULONG ulWaveFormat;                         /**< 解码后音频格式，对应tagWaveFormatInfo枚举中的值 */
}XP_WAVE_DATA_S;

/**
* @struct tagWaveInFileInfo
* @brief 设置音频文件作为输入源时，需要设置的额外参数信息。
* @attention 无
*/
typedef struct tagWaveInFileInfo
{
    CHAR szWaveFileName[IMOS_FILE_PATH_LEN];    /**< 音频文件名称（包括路径信息） */
    BOOL_T bCyclePlay;                          /**< 是否循环播放音频文件 */
}XP_WAVEIN_FILE_INFO_S;

/**
* @struct tagRect
* @brief 矩形信息。
* @attention 无
*/
typedef struct tagRect
{
    LONG lLeft;                               /*x轴左点值百分比[0 ~ 100]*/
    LONG lTop;                                /*y轴顶点值百分比[0 ~ 100]*/
    LONG lRight;                              /*x轴右点值百分比[0 ~ 100]*/
    LONG lBottom;                             /*y轴底点值百分比[0 ~ 100]*/
}XP_RECT_S;

/**
* @struct tagPoint
* @brief 坐标点信息。
* @attention 无
*/
typedef struct tagPoint
{
    LONG lX;                               /*x轴值百分比[0 ~ 100]*/
    LONG lY;                               /*y轴值百分比[0 ~ 100]*/
}XP_POINT_S;

/**
 * @enum tagCombineZoomRect
 * @brief 拼接图片放大区域信息
 * @attention
 */
typedef struct tagCombineZoomRect
{
    LONG  lX;                              /* 拼接图片中需要放大中心点X指标,精确到0.01%,[0 -- 10000] */
    LONG  lY;                              /* 拼接图片中需要放大中心点Y指标,精确到0.01%,[0 -- 10000] */
    ULONG ulZoomHeight;                    /* 放大区域的高度,单位为像素 */
    ULONG ulZoomWidth;                     /* 放大区域的宽度,单位为像素 */
}XP_COMBINE_ZOOM_RECT_S;

/**
* @enum tagVideoAdjust
* @brief 影像调节参数
* @attention
*/
typedef struct tagVideoAdjust
{
    LONG lCont;             /**< 对比度 */
    LONG lLum;              /**< 亮度 */
    LONG lHue;              /**< 色调 */
    LONG lSat;              /**< 饱和度 */
    LONG lGamma;            /**< 伽玛 */
}XP_VIDEO_ADJUST_S;

/**
 * @enum tagXPThruLineDirection
 * @brief "绊线行为"方向
 * @attention
 */
typedef enum tagXPThruLineDirection
{
    XP_THRU_LINE_DIRECTION_CLOCKWISE          = 0,    /**< 顺时针 */
    XP_THRU_LINE_DIRECTION_ANTICLOCKWISE      = 1,    /**< 逆时针 */
    XP_THRU_LINE_DIRECTION_ALL                = 2,    /**< 双向 */

    XP_THRU_LINE_DIRECTION_MAX,
    XP_THRU_LINE_DIRECTION_INVALID            = 0XFFFFFFFF
} XP_THRU_LINE_DIRECTION_E;

/**
 * @enum tagXPAreaDirection
 * @brief "区域行为"方向
 * @attention
 */
typedef enum tagXPAreaDirection
{
    XP_AREA_DIRECTION_IN       = 0,    /**< In */
    XP_AREA_DIRECTION_OUT      = 1,    /**< Out */
    XP_AREA_DIRECTION_ALL      = 2,    /**< 双向 */

    XP_AREA_DIRECTION_MAX,
    XP_AREA_DIRECTION_INVALID  = 0XFFFFFFFF
} XP_AREA_DIRECTION_E;

/**
 * @enum tagXPHWStatus
 * @brief 硬件参数
 * @attention
 */
typedef enum tagXPHWStatus
{
    XP_HW_NORMAL = 0,                    /**< 正常 */
    XP_HW_DDRAW_NOSUPPORT   = 1,         /**< 普通品质硬件加速未开启 */
    XP_HW_D3D_NOSUPPORT     = 2,         /**< 高品质硬件加速未开启 */
    XP_HW_RENDER_NOSUPPORT  = 3          /**< 硬件加速均未开启 */
}XP_HW_STATUS_E;

/**
* 消息或者异常消息回调函数的指针类型，在老接口函数XP_SetRunMsgCB中使用。\n
* @param [IN] enRunInfoType 消息或者异常消息类型。
* @param [IN] pParam        存放消息或者异常消息数据的缓冲区指针（所存放的数据与消息或者异常消息类型有关）。
* @return 无。
* @note
* -     1、客户在实现该回调函数时需要确保函数尽快返回，否则会阻塞播放器XP内部调用该回调函数的线程。
* -     2、参数pParam的实际类型根据具体的消息或者异常消息类型而定，客户需要根据消息或者异常消息类型做相应的类型转换；在某些情况下，该参数有可能为NULL。
* -     具体的类型转换关系如下：
* -         当enRunInfoType值为XP_RUN_INFO_SERIES_SNATCH、XP_RUN_INFO_RECORD_VIDEO、XP_RUN_INFO_MEDIA_PROCESS、
* -     XP_RUN_INFO_RTSP_PROTOCOL或XP_RUN_INFO_PASSIVE_MONITOR时，须将pParam指针转换为#XP_RUN_INFO_S结构体指针类型；
* -         当enRunInfoType值为XP_RUN_INFO_DOWN_MEDIA_PROCESS或XP_RUN_INFO_DOWN_RTSP_PROTOCOL时，
* -     须将pParam指针转换为#XP_DOWN_RUN_INFO_S结构体指针类型；
* -         当enRunInfoType值为XP_RUN_INFO_VOICE_MEDIA_PROCESS时，需将pParam指针转换为#XP_VOICE_RUN_INFO_S结构体指针类型；
* -         当enRunInfoType值为XP_RUN_INFO_SIP_LIVE_TIMEOUT时，pParam指针为NULL。
*/
typedef VOID (STDCALL* XP_RUN_INFO_CALLBACK_PF)(IN XP_RUN_INFO_TYPE_E enRunInfoType,
                                                IN VOID *pParam);

/**
* 解码后媒体流数据回调函数的指针类型，在老接口函数XP_SetOutputMediaDataCB中使用。\n
* @param [IN] hXpWnd          播放窗口句柄。
* @param [IN] enMediaDataType 解码后媒体流数据类型。
* @param [IN] pDataParam      存放解码后媒体流数据信息缓冲区指针（所存放的媒体流数据与媒体流数据类型有关）。
* @return 无。
* @note
* -     1、客户应及时处理输出的媒体数据，确保函数尽快返回，否则会影响播放器XP内的媒体流处理。
* -     2、参数pDataParam的实际类型与参数enMediaDataType有关，客户需要根据运行信息类型做相应的类型转换。
* -     具体的类型转换关系如下：
* -         当enMediaDataType值为XP_VIDEO_DATA_DECODE时，须将pDataParam指针转换为#XP_PICTURE_DATA_S结构体指针类型；
* -         当enMediaDataType值为XP_AUDIO_DATA_DECODE时，须将pDataParam指针转换为#XP_WAVE_DATA_S结构体指针类型。
*/
typedef VOID (STDCALL* XP_PROCESS_MEDIA_DATA_CALLBACK_PF)(IN IMOS_HANDLE hWnd,
                                                          IN XP_OUTPUT_MEDIA_DATA_TYPE_E enMediaDataType,
                                                          IN const VOID *pDataParam);

#endif /* _IMOS_MEDIA_PLAY_SDK_DATA_TYPE_H_ */
