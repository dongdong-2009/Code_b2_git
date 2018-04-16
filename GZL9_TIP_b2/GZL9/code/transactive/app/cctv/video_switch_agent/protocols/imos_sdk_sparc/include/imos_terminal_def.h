
/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_terminal_def.h

  Project Code: IMOS PLAT
   Module Name: PUBLIC
  Date Created: 2009-06-09
        Author: lipengfei/07070
   Description: 基础平台公共头文件

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_TERMINAL_DEF_H_
#define _IMOS_TERMINAL_DEF_H_

#ifdef  __cplusplus
extern "C"{
#endif

/* 波特率 */
#define IMOS_BUADRATE_1200                      1200
#define IMOS_BUADRATE_2400                      2400
#define IMOS_BUADRATE_4800                      4800
#define IMOS_BUADRATE_9600                      9600
#define IMOS_BUADRATE_19200                     19200
#define IMOS_BUADRATE_38400                     38400
#define IMOS_BUADRATE_57600                     57600
#define IMOS_BUADRATE_115200                    115200

/* 串口流控 */
#define IMOS_FLOW_CTRL_NULL                     0
#define IMOS_FLOW_CTRL_SOFTWARE                 1
#define IMOS_FLOW_CTRL_HARDWARE                 2

/* 校验位 */
#define IMOS_PARITY_NULL                        0
#define IMOS_PARITY_ODD                         1
#define IMOS_PARITY_EVEN                        2

#define IMOS_SIC_DESC                           "SIC"
#define IMOS_HIC_DESC                           "HIC"
#define IMOS_HIC5401_DESC                           "HIC5401"
#define IMOS_HIC5421_DESC                           "HIC5421"
#define STANDARD_DEF_IP_CAMERA                "STANDARD_DEF_IP_CAMERA"
#define HIGH_DEF_IP_CAMERA                    "HIGH_DEF_IP_CAMERA"
/* 前端设备类型 */
typedef enum tagIMOSDeviceType
{
    IMOS_DT_EC1001_HF = 0,
    IMOS_DT_EC1002_HD,                          /* 软型号 由 EC1004_HC 配置而成，对应 EC1004-HC[2CH] */
    IMOS_DT_EC1004_HC,
    IMOS_DT_EC2004_HF,
    IMOS_DT_ER3304_HF,
    IMOS_DT_ER3308_HD = 5,
    IMOS_DT_ER3316_HC,
    IMOS_DT_DC1001_FF = 7,
    IMOS_DT_EC3016_HF,
    IMOS_DT_ISC3100_ER,
    IMOS_DT_EC1001_EF = 10,
    IMOS_DT_EC3004_HF_ER,
    IMOS_DT_EC3008_HD_ER,
    IMOS_DT_EC3016_HC = 13,
    IMOS_DT_EC3016_HC_8CH = 14,
    IMOS_DT_EC3016_HC_4CH = 15,

    /* 枚举值 200 -- 500 */
    IMOS_DT_EC2016_HC = 200,
    IMOS_DT_EC2016_HC_8CH,
    IMOS_DT_EC2016_HC_4CH,
    IMOS_DT_EC1501_HF,                          /* 6437真双流 */
    IMOS_DT_EC1004_MF,                          /* 多路EC */
    IMOS_DT_ECR2216_HC,                         /* ECR预研，1U */
    IMOS_DT_EC1304_HC,                          /* 带PCMCIA */
    IMOS_DT_EC1501_IHF = 207,

    IMOS_DT_ECR3316_HC = 300,
    IMOS_DT_ECR3316_HC_8CH,
    IMOS_DT_ECR3316_HC_4CH,
    IMOS_DT_ECR3308_HD,
    IMOS_DT_ECR3308_HD_4CH,

    /* 枚举值 500 -- 800 */
    IMOS_DT_DC3016_FC = 500,                    /* 目前V1R5，2U，和ISC3000-M共用设计 */
    IMOS_DT_DC2016_FC,                          /* DC3016-FC降成本 */
    IMOS_DT_DC1016_MF,                          /* 多路DC */
    IMOS_DT_DC2004_FF,                          /* 带数字矩阵的多路DC */

    IMOS_DT_EC1001 = 1000,                      /* R1 编码板，为了方便扩展，从1000开始计数 */
    IMOS_DT_DC1001 = 1001,                      /* R1 解码板 */
    IMOS_DT_EC1101_HF = 1002,
    IMOS_DT_EC1102_HF = 1003,
    IMOS_DT_EC1801_HH = 1004,
    IMOS_DT_DC1801_FH = 1005,  
    IMOS_DT_ECR3308_HF = 1006,                  /* ECR3308-HF */
    IMOS_DT_ECR3316_HF = 1007,                  /* ECR3316-HF */
    IMOS_DT_EC1504_HF = 1008,                   /* EC1504-HF */
    IMOS_DT_EC2516_HF = 1009,                   /* EC2516-HF */
    IMOS_DT_EC2508_HF = 1010,                   /* EC2508-HF */
    IMOS_DT_EC1004_HC_1CH   = 1011,
    IMOS_DT_EC1004_HC_2CH   = 1012,
    IMOS_DT_EC2508_HF_4CH = 1013,
    IMOS_DT_EC2516_HF_8CH = 1014,
    IMOS_DT_EC_PAG = 1015,

    IMOS_DT_ECR3308_HF_E = 1016,                  /* ECR3308-HF-E */
    IMOS_DT_ECR3316_HF_E = 1017,                  /* ECR3316-HF-E */

    /* V3R5产品 */
    IMOS_DT_DC2808_FH = 1100,                     /* DC2808-FH */
    IMOS_DT_DC2804_FH = 1101,                     /* DC2804-FH */
    /* OEM产品型号 */
    IMOS_DT_VR2004 = 10003,
    IMOS_DT_R1000  = 10203,
    IMOS_DT_VL2004 = 10503,
    IMOS_DT_VR1102 = 11003,
    /* IPC产品型号 */
    IMOS_DT_HIC5201 = 12001,
    IMOS_DT_HIC5221 = 12002,
    IMOS_DT_HIC5421 = 12003,
    IMOS_DT_HIC5401 = 12004,
    IMOS_DT_SIC = 12005,    /**< 标清网络摄像机(standard definition IP camera) */
    IMOS_DT_HIC = 12006,    /**< 高清网络摄像机(high definition IP camera) */

    /* SDC设备 */
    IMOS_DT_SDC_HJSX = 20000,                   /* 黄金视讯的SDC */
    IMOS_DT_BUTT
}IMOS_DEVICE_TYPE_E;

/* 编码格式搭配套餐 */
typedef enum tagIMOSStreamRelationSet
{
    IMOS_SR_MPEG4_MPEG4 = 0,                    /* MPEG4[主码流] + MPEG4[辅码流] */
    IMOS_SR_H264_SHARE = 1,                     /* H.264[主码流] */
    IMOS_SR_MPEG2_MPEG4 = 2,                    /* MPEG2[主码流] + MPEG4[辅码流] */
    IMOS_SR_H264_MJPEG = 3,                     /* H.264[主码流] + MJPEG[辅码流] */
    IMOS_SR_MPEG4_SHARE = 4,                    /* MPEG4[主码流] */
    IMOS_SR_MPEG2_SHARE = 5,                    /* MPEG2[主码流] */
    IMOS_SR_STREAM_MPEG4_8D1 = 8,               /* MPEG4[主码流_D1] 8D1 套餐*/
    IMOS_SR_MPEG2_MPEG2 = 9,                    /* MPEG2[主码流] + MPEG2[辅码流] */
    IMOS_SR_H264_H264 = 11,                     /* H.264[主码流] + H.264[辅码流] */
    IMOS_SR_MPEG2_H264 = 13,                    /* MPEG2[主码流] + H.264[辅码流] */

    IMOS_SR_H264_H264_H264 = 14,                /* H.264[主码流] + H.264[辅码流] + H.264[第三码流] */
    IMOS_SR_BUTT,
    IMOS_SR_INVALID        =0xFFFF              /* 65535非法值 */
}IMOS_STREAM_RELATION_SET_E;

/* 码流类型 */
typedef enum tagIMOSStreamType
{
    IMOS_ST_TS  = 0,
    IMOS_ST_H3C_ES = 1,                         /* H3C ES RTP */
    IMOS_ST_PS  = 2,
    IMOS_ST_MEGA_EYE = 3,                       /* 中国电信全球眼 */
    IMOS_ST_CNC = 4,                            /* 中国网通宽世界 */
    IMOS_ST_LPR = 5,                            /* LPR流封装格式 */
    IMOS_ST_BUTT
}IMOS_STREAM_TYPE_E;

 /* 图像制式 */
typedef enum tagIMOSPictureFormat
{
    IMOS_PF_PAL = 0,                            /* PAL 制式 */
    IMOS_PF_NTSC,                               /* NTSC 制式 */
    IMOS_PF_720P24HZ,
    IMOS_PF_720P25HZ,
    IMOS_PF_720P30HZ,
    IMOS_PF_720P50HZ,
    IMOS_PF_720P60HZ,
    IMOS_PF_1080I48HZ,
    IMOS_PF_1080I50HZ,
    IMOS_PF_1080I60HZ,
    IMOS_PF_1080P24HZ,
    IMOS_PF_1080P25HZ,
    IMOS_PF_1080P30HZ,
    IMOS_PF_1080P50HZ,
    IMOS_PF_1080P60HZ, 
    IMOS_PF_XGA60HZ,
    IMOS_PF_SXGA60HZ,
    IMOS_PF_AUTO,                               
    IMOS_PF_INVALID    
}IMOS_PICTURE_FORMAT_E;

/* 视频编码格式 */
typedef enum tagIMOSVideoFormat
{
    IMOS_VF_MPEG1 = 0,
    IMOS_VF_MPEG2,
    IMOS_VF_MPEG4,
    IMOS_VF_MJPEG,
    IMOS_VF_H263,
    IMOS_VF_H263PLUS,
    IMOS_VF_H264,
    IMOS_VF_BUTT,
    IMOS_VF_ENCFORMAT_INVALID = 0xFFFF         /* OTHER */
}IMOS_VIDEO_FORMAT_E;

/* 流传输方式 */
typedef enum tagIMOSTransType
{
    IMOS_TT_UDP = 0,
    IMOS_TT_TCP = 1,
    IMOS_TT_RTP = 2,
    IMOS_TT_RTP_NO_RTCP = 3,
    IMOS_TT_AUTO = 4,
    IMOS_TT_BUTT
}IMOS_TRANS_TYPE_E;

/* 编码方式 */
typedef enum tagIMOSEncMode
{
    IMOS_EM_CBR = 0,                           /* CBR方式 */
    IMOS_EM_VBR,                               /* VBR方式 */
    IMOS_EM_BUTT
}IMOS_ENC_MODE_E;

/* CBR 编码模式设置 */
typedef enum tagIMOSCbrEncMode
{
    IMOS_CE_MODE_FLUENT = 0,                   /* 连贯性方式 */
    IMOS_CE_MODE_CLEARLY,                      /* 清晰性方式 */
    IMOS_CE_MODE_INVALID
}IMOS_CBR_ENC_MODE_E;

/* 平滑等级设置 */
typedef enum tagIMOSStreamSmooth
{
    IMOS_SS_NO_SMOOTH = 0,
    IMOS_SS_VALUE1,
    IMOS_SS_VALUE2,
    IMOS_SS_VALUE3,
    IMOS_SS_VALUE4,
    IMOS_SS_VALUE5,
    IMOS_SS_VALUE6,
    IMOS_SS_VALUE7,
    IMOS_SS_VALUE8,
    IMOS_SS_VALUE9,
    IMOS_SS_INVALID_VALUE
}IMOS_STREAM_SMOOTH_E;

/* GOP 结构参数 */
typedef enum tagIMOSGopType
{
    IMOS_GT_IP = 0,                           /* IP */
    IMOS_GT_IBP,                              /* IBP */
    IMOS_GT_IBBP,                             /* IBBP */
    IMOS_GT_I,                                /* I */
    IMOS_GT_INVALID                           /* OTHER */
}IMOS_GOP_TYPE_E;

/* 图像大小 */
typedef enum tagIMOSPictureSize
{
    IMOS_PS_D1 = 0,
    IMOS_PS_4CIF,
    IMOS_PS_2CIF,
    IMOS_PS_CIF,
    IMOS_PS_QCIF,
    IMOS_PS_HALFD1,
    IMOS_PS_720,
    IMOS_PS_1080,
    IMOS_PS_1024,
    IMOS_PS_1280,
    IMOS_PS_INVALID
}IMOS_PICTURE_SIZE_E;

/* 图像质量，在VBR 模式下有效 */
typedef enum tagIMOSVideoQuality
{
    IMOS_VQ_L0 = 0,                          /* Level 0*/
    IMOS_VQ_L1 = 1,                          /* Level 1*/
    IMOS_VQ_L2 = 4,                          /* Level 2*/
    IMOS_VQ_L3 = 8,                          /* Level 3*/
    IMOS_VQ_L4 = 12,                         /* Level 4*/
    IMOS_VQ_L5 = 16,                         /* Level 5*/
    IMOS_VQ_L6 = 20,                         /* Level 6*/
    IMOS_VQ_L7 = 24,                         /* Level 7*/
    IMOS_VQ_L8 = 28,                         /* Level 8*/
    IMOS_VQ_L9 = 31                          /* Level 9*/
}IMOS_VIDEO_QUALITY_E;

/* 声道设置：L/R/STERO(MIC为单声道) */
typedef enum tagIMOSAudioChannelType
{
    IMOS_AC_STEREO = 0,                      /* 立体声 */
    IMOS_AC_L,                               /* 左声道 */
    IMOS_AC_R,                               /* 右声道 */
    IMOS_AC_MONO,                            /* 单声道 */
    IMOS_AC_INVALID
}IMOS_AUDIO_CHANNEL_TYPE_E;

/* 音频输入源：凤凰头/BNC和MIC二者选一 */
typedef enum tagIMOSAudioInputSource
{
    IMOS_AI_LR = 0,                          /* 凤凰头/BNC输入方式 */
    IMOS_AI_MIC,                             /* MIC输入方式 */
    IMOS_AI_INVALID                          /* OTHER */
}IMOS_AUDIO_INPUT_SOURCE_E;

/* 音频编码格式定义 */
typedef enum tagIMOSAudioFormat
{
    IMOS_AF_MPEG1_LAYER2 = 0,
    IMOS_AF_G711A,
    IMOS_AF_G711U,
    IMOS_AF_ADPCM,
    IMOS_AF_AUTO,
    IMOS_AF_G722,
    IMOS_AF_BUTT
}IMOS_AUDIO_FORMAT_E;

/* 音频编码采样率定义 */
typedef enum tagIMOSAudioSampling
{
    IMOS_AS_8000 = 0,
    IMOS_AS_16000,
    IMOS_AS_22050,
    IMOS_AS_24000,
    IMOS_AS_32000,
    IMOS_AS_44100,
    IMOS_AS_48000,
    IMOS_AS_96000,
    IMOS_AS_INVALID
}IMOS_AUDIO_SAMPLING_E;

/* OSD 颜色 */
typedef enum tagIMOSOsdColor
{
     IMOS_OC_WHITE  = 0,
     IMOS_OC_RED,
     IMOS_OC_YELLOW,
     IMOS_OC_BLUE,
     IMOS_OC_BLACK,
     IMOS_OC_GREEN,
     IMOS_OC_PURPLE,
     IMOS_OC_BUTT
}IMOS_OSD_COLOR_E;

/* OSD 透明度 */
typedef enum tagIMOSOsdAlpha
{
    IMOS_OA_HIDE = 0,                        /* 不透明 */
    IMOS_OA_HALF,                            /* 半透明 */
    IMOS_OA_SHOW,                            /* 全透明 */
    IMOS_OA_BUTT
}IMOS_OSD_ALPHA_E;

/* OSD日期显示格式 */
typedef enum tagIMOSDateFormat
{
    IMOS_DF_Y_M_D1 = 0,                      /* 0: XXXX-XX-XX 年月日 */
    IMOS_DF_M_D_Y1,                          /* 1: XX-XX-XXXX 月日年 */
    IMOS_DF_Y_M_D2,                          /* 2: XXXX年XX月XX日 */
    IMOS_DF_M_D_Y2,                          /* 3: XX月XX日XXXX年 */
    IMOS_DF_BUTT                             /* 无效格式 */
}IMOS_DATA_FORMAT_E;

/* OSD时间显示格式 */
typedef enum tagIMOSTimeFormat
{
    IMOS_TF_1 = 0,                           /* XX:XX:XX ，常见于中国，美国，英国等 */
    IMOS_TF_2,                               /* XX:XX:XX p.m. ，常见于除西班牙以外的西班牙语国家 */
    IMOS_TF_3,                               /* XX:XX:XX PM ,常见于除英国以外的英语国家 */
    IMOS_TF_4,                               /* PM XX:XX:XX ，见于中文(新加坡) */
    IMOS_TF_5,                               /* 下午 XX:XX:XX ，见于中文(台湾) */
    IMOS_TF_BUTT                             /* 无效格式 */
}IMOS_TIME_FORMAT_E;

/* 场名OSD 类型 */
typedef enum tagIMOSInfoOsdType
{
    IMOS_IOT_WORD = 0,                       /* 文字 */
    IMOS_IOT_PICTURE,                        /* 图片 */
    IMOS_IOT_BUTT
}IMOS_INFO_OSD_TYPE_E;

/* 告警类型 */
typedef enum tagIMOSAlarmType
{
    IMOS_AT_HIGH_TEMPERATURE = 0,            /* 高温告警 */
    IMOS_AT_HIGH_TEMPERATURE_RESUME,         /* 高温告警恢复 */
    IMOS_AT_LOW_TEMPERATURE = 2,             /* 低温告警 */
    IMOS_AT_LOW_TEMPERATURE_RESUME,          /* 低温告警恢复 */
    IMOS_AT_VIDEO_LOST = 4,                  /* 视频丢失告警 */
    IMOS_AT_VIDEO_LOST_RESUME,               /* 视频丢失告警恢复 */
    IMOS_AT_MOVE_DETECT = 6,                 /* 运动检测告警 */
    IMOS_AT_MOVE_DETECT_RESUME,              /* 运动检测告警恢复 */
    IMOS_AT_MASK_DETECT = 8,                 /* 遮挡侦测告警 */
    IMOS_AT_MASK_DETECT_RESUME,              /* 遮挡侦测告警恢复 */
    IMOS_AT_INPUT_SWITCH = 10,               /* 输入开关量告警 */
    IMOS_AT_INPUT_SWITCH_RESUME,             /* 输入开关量告警恢复 */
    IMOS_AT_CIRCUIT_DETECT = 12,             /* 开关量线路检测告警 */
    IMOS_AT_CIRCUIT_DETECT_RESUME,           /* 开关量线路检测告警恢复 */
    IMOS_AT_FAN_DETECT = 14,                 /* 风扇检测告警 */
    IMOS_AT_FAN_DETECT_RESUME,               /* 风扇检测告警恢复 */
    IMOS_AT_STOR_FAILED = 16,                /* 存储失败告警 */
    IMOS_AT_STOR_FAILED_RESUME,              /* 存储失败告警恢复 */
    IMOS_AT_BUTT
}IMOS_ALARM_TYPE_E;

/* 串口模式 */
typedef enum tagIMOSSerialMode
{
    IMOS_SE_PTZ = 1,                         /* 云台控制 */
    IMOS_SE_TRANS,                           /* 透明通道 */
    IMOS_SE_CONSOLE,                         /* 控制台 */
    IMOS_SE_PTZ_AND_TRANS,                   /* 云台控制和透明通道 */
    IMOS_SE_BUTT
}IMOS_SERIAL_MODE_E;

/* 开关量运行模式 */
typedef enum tagIMOSSwitchRunMode
{
    IMOS_SM_OPEN = 1,                        /* 常开 */
    IMOS_SM_CLOSE,                           /* 常闭 */
    IMOS_SM_BUTT
}IMOS_SIWTCH_RUNMODE_E;

/* 黑白模式 */
typedef enum tagBlackWhiteMode
{
    IMOS_BLACKWHITE_AUTO = 0,          /* 自动模式 */
    IMOS_BLACKWHITE_COLOR,             /* 永远彩色 */
    IMOS_BLACKWHITE_GRAY,              /* 永远黑白 */
    IMOS_BLACKWHITE_BUTT,
}IMOS_BLACKWHIT_MODE_E;

/* 快门时间 */
typedef enum tagShuuter
{
    IMOS_ESHUTTER_AUTO      = 0,       /* 快门时间为自动 */  
    IMOS_ESHUTTER_4_SEC     = 4,       /* 1/4 快门时间 */
    IMOS_ESHUTTER_8_SEC     = 8,       /* 1/8 快门时间 */
    IMOS_ESHUTTER_15_SEC    = 15,      /* 1/15 快门时间 */
    IMOS_ESHUTTER_25_SEC    = 25,      /* 1/25 快门时间 */
    IMOS_ESHUTTER_30_SEC    = 30,      /* 1/30 快门时间 */
    IMOS_ESHUTTER_50_SEC    = 50,      /* 1/50 快门时间 */
    IMOS_ESHUTTER_60_SEC    = 60,      /* 1/60 快门时间 */
    IMOS_ESHUTTER_100_SEC   = 100,     /* 1/100 快门时间 */
    IMOS_ESHUTTER_120_SEC   = 120,     /* 1/120 快门时间 */
    IMOS_ESHUTTER_180_SEC   = 180,     /* 1/180 快门时间 */
    IMOS_ESHUTTER_200_SEC   = 200,     /* 1/200 快门时间 */
    IMOS_ESHUTTER_250_SEC   = 250,     /* 1/250 快门时间 */
    IMOS_ESHUTTER_500_SEC   = 500,     /* 1/500 快门时间 */
    IMOS_ESHUTTER_1000_SEC  = 1000,    /* 1/1000 快门时间 */
    IMOS_ESHUTTER_2000_SEC  = 2000,    /* 1/2000 快门时间 */
    IMOS_ESHUTTER_4000_SEC  = 4000,    /* 1/4000 快门时间 */
    IMOS_ESHUTTER_8000_SEC  = 8000,    /* 1/8000 快门时间 */
    IMOS_ESHUTTER_10000_SEC = 10000,   /* 1/10000 快门时间 */
    IMOS_ESHUTTER_BUTT,
}IMOS_SHUTTER_E;

/* 测光控制 */
typedef enum tagAEMetering
{
    IMOS_AEMETERING_CENTER = 0,        /* 中央权重 */
    IMOS_AEMETERING_SPOT,              /* 点测光权重 */
    IMOS_AEMETERING_BACKLIGHT,         /* 背光补偿 */
    IMOS_AEMETERING_BUTT,
}IMOS_AEMETERING_E;

/* 白平衡 */
typedef enum tagAWB
{
    IMOS_AWB_AUTO = 0, 
    IMOS_AWB_TUNGSTEN,                 /* 日落模式 */
    IMOS_AWB_DAYLIGHT,                 /* 日光模式 */
    IMOS_AWB_CLOUDSHADY,               /* 阴天多云 */
    IMOS_AWB_FLUORESC1,                /* 荧光灯1，偏蓝 */
    IMOS_AWB_FLUORESC2,                /* 荧光灯2，偏不蓝 */
    IMOS_AWB_INDOOR = 6,               /* 室内 */
    IMOS_AWB_OUTDOOR = 7,              /* 室外 */
    IMOS_AWB_BUTT,
}IMOS_AWB_E;

/* 图像镜像 */
typedef enum MirrorType
{
    IMOS_MIRRORTYPE_NONE = 0,          /* 正常模式 */
    IMOS_MIRRORTYPE_COL,               /* 垂直翻转 */
    IMOS_MIRRORTYPE_ROW,               /* 水平翻转 */
    IMOS_MIRRORTYPE_ROW_COL,           /* 垂直水平翻转 */
    IMOS_MIRRORTYPE_BUTT,
}IMOS_MIRRORTYPE_E;

/* 背景频率 */
typedef enum tagFlickerMode
{
    IMOS_FLICKERMODE_OFF = 0,           /* 关闭 */
    IMOS_FLICKERMODE_50HZ,              /* 50HZ */
    IMOS_FLICKERMODE_60HZ,              /* 60HZ */
    IMOS_FLICKERMODE_BUTT,
}IMOS_FLICKER_MODE_E;

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_PRODUCT_DEF_H_ */

