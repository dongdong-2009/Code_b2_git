/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_def.h

  Project Code: IMOS PLAT
   Module Name: PUBLIC
  Date Created: 2009-03-16
        Author: lilunyi/03464
   Description: 基础平台公共头文件

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_DEF_H_
#define _IMOS_DEF_H_

#ifdef  __cplusplus
extern "C"{
#endif

/* 相同类型服务使用相同的ProcID。1-99给服务进程使用 */
#define IMOS_PROCID_VMSERVER            0x1
#define IMOS_PROCID_ISCSERVER           0x1
#define IMOS_PROCID_SDKSERVER           0x2
#define IMOS_PROCID_MCSERVER            0x3
#define IMOS_PROCID_MPSERVER            0x4
#define IMOS_PROCID_MSSERVER            0x4
#define IMOS_PROCID_DMSERVER            0x4
#define IMOS_PROCID_VXSERVER            0x4
#define IMOS_PROCID_SGSERVER            0x5
#define IMOS_PROCID_BMSERVER            0x6
#define IMOS_PROCID_BWSERVER            0x7
#define IMOS_PROCID_NDSERVER            0x8


/* SDK client和第三方SDK程序使用100 */
#define IMOS_PROCID_SDKCLIENT           0x64

/* TP进程保持现有值不变，从101开始 */
#define IMOS_PROCID_MW_CTRL             0x65
#define IMOS_PROCID_MW_STOR_MCP         0x66
#define IMOS_PROCID_MW_PTZ              0x67
#define IMOS_PROCID_MW_SNMP             0x68
#define IMOS_PROCID_MW_ALM              0x69
#define IMOS_PROCID_MW_CAPI             0x6A
#define IMOS_PROCID_MW_SDK              0x6B

/* 调试工具以及其它工具程序使用257 */
#define IMOS_PROCID_DT                  0x101


/** @brief 权限最大配置粒度数量 */
#define IMOS_AUTHORITY_MAX_NUM                  128

/** 内部权限最大配置粒度数量 */
#define IMOS_AUTHORITY_MAX_NUM_EX               1024

/** 存储权限字段最大个数 */
#define IMOS_AUTHORITY_MAX_FIELD_COUNT          32

/*@brief IMOS系统参数长度*/
#define IMOS_SYS_PARAM_LEN                      32             /**< 平台支持的系统参数长度, 有效最大位数为31位 */

/*@brief 通用CODE长度*/
#define IMOS_CODE_LEN                           48             /**< 平台支持的有效最大位数为47位 */

/*@brief 通用名称字符串长度 */
#define IMOS_NAME_LEN                           64             /**< 平台支持的有效最大位数为63位 */

/*@brief URL地址长度 */
#define IMOS_URL_LEN                            512
/*@brief 最大图层数 */
#define MAX_LAYER_NUM                           6              /**< 平台支持的有效最大图层数 */
/////////////////////

/*@brief 资源编码信息字符串长度*/
#define IMOS_RES_CODE_LEN                       IMOS_CODE_LEN

/*@brief 设备编码信息字符串长度*/
#define IMOS_DEVICE_CODE_LEN                    IMOS_CODE_LEN

/*@brief 用户编码信息字符串长度*/
#define IMOS_USER_CODE_LEN                      IMOS_CODE_LEN

/*@brief 域编码信息字符串长度*/
#define IMOS_DOMAIN_CODE_LEN                    IMOS_CODE_LEN

/*@brief 域名称信息字符串长度 */
#define IMOS_DOMAIN_NAME_LEN                    IMOS_NAME_LEN

/*@brief 权限编码信息字符串长度*/
#define IMOS_AUTH_CODE_LEN                      IMOS_CODE_LEN

/*@brief 呼叫过程中UA ID的字符数组长度*/
#define IMOS_UAID_LEN                           64             /**< 平台支持的有效最大位数为63位 */

/*@brief UA编码信息字符串长度*/
#define IMOS_UA_CODE_LEN                        IMOS_UAID_LEN

/*@brief 视频标签信息字符串长度*/
#define IMOS_RECORD_LABEL_LEN                   256

/*@brief 服务协商会话ID长度*/
#define IMOS_SSN_ID_LEN                         IMOS_STRING_LEN_32

/*@brief MP能力描述长度 */
#define IMOS_MP_ABILITY_LEN                     IMOS_STRING_LEN_256

/*@brief DecoderTag的长度 */
#define IMOS_DECODERTAG_LEN                     IMOS_STRING_LEN_32

//////////////


/*@brief MAI/AOI消息中通信模块名称的字符串长度 MODULENAME@DEVCODE */
#define IMOS_MODULE_NAME_LEN                    64 /**< 平台支持的有效最大位数为63位 */

/*@brief mac地址字符串长度 "AA-BB-CC-DD-EE-FF" */
#define IMOS_MAC_ADDR_LEN                       20

/*@brief imos_addr IP地址信息字符串长度
 * IPV4: "192.168.0.102"
 * IPV6: "A1:0:8:100:200:0:0:321A"
 * 域名: "AS_server_hangzhou" */
#define IMOS_IPADDR_LEN                         64

/*@brief 无效的IP地址类型 */
#define IMOS_IPADDR_TYPE_INVALID                (0xFFFFFFFF)

/*@brief IPV4地址类型 */
#define IMOS_IPADDR_TYPE_IPV4                   1

/*@brief IPV6地址类型 */
#define IMOS_IPADDR_TYPE_IPV6                   2

/*@brief 域名地址类型 */
#define IMOS_IPADDR_TYPE_DOMAINNAME             16

/*@brief 密码信息字符串长度 */
#define IMOS_PASSWD_LEN                         16

/*@brief 密文口令（加密后）字符串长度 */
#define IMOS_PASSWD_ENCRYPT_LEN                 64  /**< 平台支持的有效最大位数为63位 */

/*@brief imos_time 时间信息字符串长度 "2008-10-02 09:25:33.001 GMT" */
#define IMOS_TIME_LEN                           32

/*@brief imos_simple_time 时间信息字符串长度 "hh:mm:ss" */
#define IMOS_SIMPLE_TIME_LEN                    12

/*@brief imos_simple_date 日期信息字符串长度 "YYYY-MM-DD"*/
#define IMOS_SIMPLE_DATE_LEN                    12

/*@brief imos_description 描述信息字符串长度 */
#define IMOS_DESC_LEN                           (128 * 3)

/*@brief 固定电话号码字符串长度 */
#define IMOS_PHONE_LEN                          32

/*@brief 移动电话号码字符串长度 */
#define IMOS_MOBILE_LEN                         32

/*@brief E-Mail字符串长度 */
#define IMOS_MAIL_LEN                           64

/*@brief 邮政编码符串长度 */
#define IMOS_MAIL_CODE_LEN                      8

/*@brief 通信地址字符串长度 */
#define IMOS_CONTACT_ADDRESS_LEN                64

/*@brief 产品名称字符串长度 */
#define IMOS_PRODUCTNAME_LEN                    32

/*@brief 版本信息字符串长度 */
#define IMOS_VERSION_INFO_LEN                   64

/*@brief 文件名长度 */
#define IMOS_FILE_NAME_LEN                      64

/*@brief 文件名长度(V2) */
#define IMOS_FILE_NAME_LEN_V2                   256

/** @brief 错误码字符串长度(按ULONG最大值计算,4字节FFFFFFFF,十进制为4294967295,补1位'\0',共11位) */
#define IMOS_ERR_CODE_LEN                       11

/* Begin Added by wuchengping02739, 2011-07-19 of VVD57853 */
/** @brief XML字段 - 成功响应 */
#define IMOS_XML_KEY_RSP_OK "Rsp_Ok"

/** @brief XML字段值通用长度(使用时,注意长度是否够用) */
#define IMOS_XML_KEY_LEN                        64
/* End Added by wuchengping02739, 2011-07-19 of VVD57853 */

/*@brief 文件绝对路径(路径+文件名)长度 */
#define IMOS_FILE_PATH_LEN                      256

/*@brief 文件绝对路径(不带文件名)长度 */
#define IMOS_FILE_PATH_WITHOUT_FILENAME_LEN     128

/*@brief 资源所在组织路径长度 */
#define IMOS_RES_PATH_LEN                       1024

/*@brief 扩展字段长度 */
#define IMOS_RESERVED_LEN                       64

/*@brief 通用16位字符串 */
#define IMOS_STRING_LEN_14                      14

/*@brief 通用32位字符串 */
#define IMOS_STRING_LEN_32                      32

/*@brief 通用64位字符串 */
#define IMOS_STRING_LEN_64                      64

/*@brief 通用128位字符串 */
#define IMOS_STRING_LEN_128                     128

/*@brief 通用256位字符串 */
#define IMOS_STRING_LEN_256                     256

/*@brief 通用512位字符串 */
#define IMOS_STRING_LEN_512                     512

/*@brief 通用1024位字符串 */
#define IMOS_STRING_LEN_1024                    1024

/*@brief 资源树深度 */
#define IMOS_RES_TREE_LEVEL                     12

/*@brief 设备在线状态 */
#define IMOS_DEV_STATUS_ONLINE                  1

/*@brief 设备离线状态 */
#define IMOS_DEV_STATUS_OFFLINE         2

/*@brief 设备正常状态 */
#define IMOS_DEV_STATUS_NORMAL          3

/*@brief 设备故障状态 */
#define IMOS_DEV_STATUS_FAULT           4

/*@brief 设备禁用 */
#define IMOS_DEV_UNABLE                 0

/*@brief 设备启用 */
#define IMOS_DEV_ENABLE                 1

/*@brief 二进制，IMOS_ltoa函数中使用 */
#define  IMOS_Radix_2                   2

/*@brief 八进制，IMOS_ltoa函数中使用 */
#define  IMOS_Radix_8                   8

/*@brief 十进制，IMOS_ltoa函数中使用 */
#define  IMOS_Radix_10                  10

/*@brief 十六进制，IMOS_ltoa函数中使用 */
#define  IMOS_Radix_16                  16


/*@brief IMOS_ID 对应字符串的长度（64位整型ID转换后的最大有效字符长度为20） */
#define  IMOS_ID_STRING_LEN             24

/*@brief 备份录像案例长度 */
#define  IMOS_BAK_CASE_LEN              128


/*@brief 星期日 */
#define IMOS_SUN            0

/*@brief 星期一 */
#define IMOS_MON            1

/*@brief 星期二 */
#define IMOS_TUE            2

/*@brief 星期三 */
#define IMOS_WED            3

/*@brief 星期四 */
#define IMOS_THU            4

/*@brief 星期五 */
#define IMOS_FRI            5

/*@brief 星期六 */
#define IMOS_SAT            6

/*@brief 一月 */
#define IMOS_JAN            0

/*@brief 二月 */
#define IMOS_FEB            1

/*@brief 三月 */
#define IMOS_MAR            2

/*@brief 四月 */
#define IMOS_APR            3

/*@brief 五月 */
#define IMOS_MAY            4

/*@brief 六月 */
#define IMOS_JUN            5

/*@brief 七月 */
#define IMOS_JUL            6

/*@brief 八月 */
#define IMOS_AUG            7

/*@brief 九月 */
#define IMOS_SEP            8

/*@brief 十月 */
#define IMOS_OCT            9

/*@brief 十一月 */
#define IMOS_NOV            10

/*@brief 十二月 */
#define IMOS_DEC            11

/* @brief SG ID信息字符串长度, SG ID生成规则：SG编码&5位随机数 */
#define IMOS_SG_ID_LEN   (IMOS_DEVICE_CODE_LEN + 6)

/*@brief 每个视频通道支持的最大流数目  */
#define IMOS_STREAM_MAXNUM                  2

/*@brief 每个视频通道支持的最大流数目,目前各种类型的EC以此标准  */
#define IMOS_STREAM_MAXNUM_II                  3

/*@brief 每个视频通道支持的最大区域数目  */
#define IMOS_AREA_MAXNUM                    12

/*@brief 单通道遮盖区域的最大数目 */
#define IMOS_MASK_AREA_MAXNUM               4

/*@brief 单通道检测区域的最大数目 */
#define IMOS_DETECT_AREA_MAXNUM             4

/*@brief 每个视频通道支持的最大OSD数目 场名osd + 图片osd  */
#define IMOS_OSD_NAME_MAXNUM                1

/*@brief 每个视频通道支持的最大OSD数目 场名osd + 图片osd 目前仅有EC2004-HF和EC2016-HC以此标准 */
#define IMOS_OSD_NAME_MAXNUM_II             2

/*@brief 每个视频通道支持的最大OSD数目 场名osd + 图片osd 目前各种类型的DC以此标准 */
#define IMOS_OSD_NAME_MAXNUM_I              1

/*@brief 每个视频通道支持的最大OSD数目 场名osd + 图片osd 目前除EC2004-HF和EC2016-HC以外的EC设备以此标准 */
#define IMOS_OSD_NAME_MAXNUM_IV             4

/*@brief 每个视频通道支持的最大OSD数目 场名osd + 图片osd 目前EC1001以此标准 */
#define IMOS_OSD_NAME_MAXNUM_VIII             8

/* Modified by wuchengping02739, 2011-12-16 of VVD63549 */
/*@brief 场名OSD的最大个数  */
#define IMOS_OSD_NAME_MAX                     9

/*@brief 时间OSD的最大个数  */
#define IMOS_OSD_TIME_MAX                   1

/** @brief 场名OSD缺省值 */
#define IMOS_NAME_OSD_DEFAULT_VALUE        "H3C"

/** @brief 切换时间间隔缺省值 */
#define IMOS_INTERVAL_DEFAULT_VALUE         0

/*@brief 每个通道中能选择的MS的最大数量  */
#define IMOS_MS_MAX_NUM_PER_CHANNEL         1


/*@brief 计划中每天的时间片数量  */
#define IMOS_MAX_TIME_SLICE_NUM             4

/*@brief 计划中每天的时间片数量V2版本  */
#define IMOS_MAX_TIME_SLICE_NUM_V2          16

/*@brief 计划中每周的天数  */
#define IMOS_WEEK_DAYS                      7

/*@brief 计划中例外的天数  */
#define IMOS_EXCP_DAYS                      16

/*@brief 注册/注销业务编码 */
#define  IMOS_SRV_CODE_REGISTER             1001

/*@brief 配置下发业务编码 */
#define  IMOS_SRV_CODE_CONFIG               1002

/*@brief 文件下载/升级业务编码 */
#define  IMOS_SRV_CODE_FILE                 1003

/*@brief 巡检业务编码 */
#define  IMOS_SRV_CODE_CHECK                1004

/*@brief 实时视频业务编码 */
#define  IMOS_SRV_CODE_MONITOR              2001

/*@brief 语音对讲业务编码 */
#define  IMOS_SRV_CODE_VOICE_TALKBACK       2011

/*@brief 语音广播业务编码 */
#define  IMOS_SRV_CODE_VOICE_BROADCAST      2012

/*@brief 音视频对讲业务编码 */
#define  IMOS_SRV_CODE_MON_TALKBACK         2013

/*@brief 音视频广播业务编码 */
#define  IMOS_SRV_CODE_MON_BROADCAST        2014

/*@brief 录像查询业务编码 */
#define  IMOS_SRV_CODE_REC_QUERY            3001

/*@brief 录像下载业务编码 */
#define  IMOS_SRV_CODE_REC_DOWNLOAD         3002

/*@brief 录像回放业务编码 */
#define  IMOS_SRV_CODE_REC_VOD              3003

/*@brief 录像备份业务编码 */
#define  IMOS_SRV_CODE_REC_BACKUP           3004

/*@brief 录像获取URL业务编码 */
#define  IMOS_SRV_CODE_REC_URL              3005

/* added by z06806 for vodwall prj 20101220 begin */
/*@brief 回放上墙业务编码 */
#define  IMOS_SRV_CODE_VODWALL              3005
/* added by z06806 for vodwall prj 20101220 end */

/*@brief 直接存储业务编码 */
#define  IMOS_SRV_CODE_DIR_STROE            4001

/*@brief 收流存储业务编码 */
#define  IMOS_SRV_CODE_STR_STORE            4002

/*@brief 前端存储业务编码 */
#define  IMOS_SRV_CODE_TP_STORE             4003

/*@brief 刷新存储索引编码 */
#define  IMOS_SRV_CODE_REFRESH_STORE_IDX    4004

/*@brief 云台控制业务编码 */
#define  IMOS_SRV_CODE_PTZ                  5001

/*@brief 开关量控制业务编码 */
#define  IMOS_SRV_CODE_SWITCH               5002

/*@brief 透明通道业务编码 */
#define  IMOS_SRV_CODE_TRANS_CHANNEL        6001

/*@brief 透明数据传输业务编码 */
#define  IMOS_SRV_CODE_TRANS_DATA           6002

/*@brief 告警发送业务编码 */
#define  IMOS_SRV_CODE_ALARM                7001

/*@brief 布/撤防业务编码 */
#define  IMOS_SRV_CODE_ALARM_CTRL           7002

/*@brief 告警查询业务编码 */
#define  IMOS_SRV_CODE_ALARM_QUERY          7003

/*@brief 设备共享业务编码 */
#define  IMOS_SRV_CODE_DEVICE_SHARE         8001 /* Added by MultiDomain */

/*@brief 订阅上报业务编码 */
#define IMOS_SRV_CODE_SUBSCRIBE             9001

/*@brief 域间数据同步业务编码 */
#define IMOS_SRV_CODE_DOMAIN_DATASYN        9002

/*@brief 场景推送业务编码 */
#define IMOS_SRV_CODE_LAYOUT_RES_SHARE      8002

/*@brief 定义授权码的长度 */
#define IMOS_LICENSE_AUTHORITY_CODE_LENGTH (32 + 4)

/*@brief 区域类型：遮盖区域 */
#define IMOS_VIDEO_AREA_TYPE_MASK           0

/*@brief 区域类型：遮挡区域 */
#define IMOS_VIDEO_AREA_TYPE_COVER_DETECT   1

/*@brief 区域类型：移动侦测区域 */
#define IMOS_VIDEO_AREA_TYPE_MOTION_DETECT  2

/*@brief 视频输出虚拟通道最大数量 */
#define IMOS_VOUT_CHANNEL_MAXNUM            4

/*@brief 视频输出虚拟通道缺省数量 */
#define IMOS_VOUT_CHANNEL_DEFAULT_NUM       1

/*@brief BP支持的预定义命令中每个命令参数的最大长度 */
#define IMOS_MAX_CMD_PARAM_LEN              512

/*@brief BP支持的预定义命令中支持的最大参数个数 */
#define IMOS_MAX_CMD_PARAM_NUM              6

/*@brief gb2312编码名称 */
#define IMOS_CODE_TYPE_GB2312 "gb2312"

/*@brief utf-8编码名称 */
#define IMOS_CODE_TYPE_UTF8 "utf-8"

/*@brief gbk编码名称 */
#define IMOS_CODE_TYPE_GBK "gbk"

/*@brief IMOS默认设备类型 */
#define IMOS_DEVICE_TYPE_DESC_DEFAULT       "defaultType"

/*@brief IMOS支持的设备类型个数(EC/DC/MS/DM/VX500/摄像头/监视器/ECR/BM/IPSAN) */
#define IMOS_DEVICE_TYPE_NUM                10

/*@brief NTP服务器同步信息宏长度 */
#define IMOS_NTP_SYNC_STATUS_LEN            32

/*@brief 第三方厂商的类型ID最小值 */
#define IMOS_MIN_THIRD_MANUFACTURERID        1

/*@brief 第三方厂商的类型ID最大值(用户可配置) */
#define IMOS_MAX_THIRD_MANUFACTURER_ID       60000

/*@brief 第三方设备类型的类型ID最小值 */
#define IMOS_MIN_THIRD_DEV_TYPE_ID           20000

/*@brief 第三方设备类型的类型ID最大值(用户可配置) */
#define IMOS_MAX_THIRD_DEV_TYPE_ID           40000

/*@brief 第三方告警类型的类型ID最小值 */
#define IMOS_MIN_THIRD_EVT_TYPE_ID           20000

/*@brief 第三方告警类型的类型ID最大值(用户可配置) */
#define IMOS_MAX_THIRD_EVT_TYPE_ID           40000

/*@brief 第三方告警类型在数据库中不存在 */
#define IMOS_NO_EXIST_EVENT_TYPE_ID          0

/*@brief 任意设备类型 */
#define IMOS_DEVICE_TYPE_ALL                 65535

/*@brief 第三方厂商的类型ID最大值(包括内置厂商类型) */
#define IMOS_MAX_THIRD_MANUFACTURER_ID_ALL   65535

/*@brief 第三方设备类型的类型ID最大值(包括内置设备类型) */
#define IMOS_MAX_THIRD_DEV_TYPE_ID_ALL       50000

/*@brief 第三方告警类型的类型ID最大值(包括内置告警类型) */
#define IMOS_MAX_THIRD_EVT_TYPE_ID_ALL       50000

/*@brief 订阅过滤条件最大个数 */
#define IMOS_SUBSC_ITEM_MAX_NUM             8

/*@brief 1分钟的秒数 */
#define IMOS_SECOND_IN_MIN                  60

/*@brief 1个小时的秒数 */
#define IMOS_SECOND_IN_HOUR                 3600

/** @brief 一条告警订阅规则支持的用户最大数量 */
#define IMOS_ALARM_SEND_USER_MAXNUM         256

/** @brief 一条告警订阅规则支持的告警源最大数量 */
#define IMOS_ALARM_SEND_SRC_MAXNUM          2000

/** @brief 系统支持的告警订阅规则最大数量 */
#define IMOS_ALARM_SEND_RULE_MAXNUM         3000

/** @brief 行为分析规则最大数量 */
#define IMOS_BEHAVIOR_RULE_MAX  8

/** @brief 坐标字符串长度 16 */
#define IMOS_COORDINATE_STR_LEN_16  16

/** @brief 坐标字符串长度 56 */
#define IMOS_COORDINATE_STR_LEN_56  56

/* Added by wuchengping02739, 2011-12-16 of VVD63517 */
/** @brief 资源组扩展信息长度 512 */
#define IMOS_RES_GROUP_EXT_LEN 512

/** @brief 空字符串 */
#define IMOS_NULL_STRING  ""

/* Begin Added by ykf0030, 2012-2-27 of 网络适应性 */
/** @brief EC实况网络适应开关默认值 */
#define IMOS_MON_NETWORK_ADAPT_ENABLE_DEFAULT    2

/** @brief EC存储网络适应开关默认值 */
#define IMOS_STORE_NETWORK_ADAPT_ENABLE_DEFAULT  2

/** @brief EC网络适应模式默认值 */
#define IMOS_NETWORK_ADAPT_MODE_DEFAULT  1

/** @brief EC实况网络自适应恢复时间默认值 */
#define IMOS_NETWORK_ADAPT_COUNT_DEFAULT  30
/* End Added by ykf0030, 2012-2-27 of 网络适应性 */

/** @brief 根据rgb,计算颜色值 */
#define COLOR(r, g, b)  ((ULONG) (((UCHAR) (r) | ((USHORT) ((UCHAR) (g)) << 8)) | (((ULONG) (UCHAR) (b)) << 16)))

/**
 * @enum tagIMOSBehaviorType
 * @brief 行为类型
 * @attention
 */
typedef enum tagIMOSBehaviorType
{
    IMOS_BEHAVIOR_TYPE_THRU_LINE    = 0,    /**< 绊线 */
    IMOS_BEHAVIOR_TYPE_INOUT_AREA   = 1,    /**< 进出区域 */

    IMOS_BEHAVIOR_TYPE_MAX,
    IMOS_BEHAVIOR_TYPE_INVALID      = 0XFFFFFFFF
} IMOS_BEHAVIOR_TYPE_E;

/** 设备额外状态枚举 */
typedef enum tagIMOSDevExtStatus
{
    IMOS_EXT_STATUS_NORMAL   = 0,                /**< 正常状态 */
    IMOS_EXT_STATUS_VIDEO_LOST_RESUME = 1,       /**< 视频恢复 */
    IMOS_EXT_STATUS_VIDEO_LOST = 2,              /**< 视频丢失 */
    IMOS_EXT_STATUS_MAX,                         /**< 最大值 */
    IMOS_EXT_STATUS_INVALID    = 0xFFFFFFFF      /**< 无效值 */
}IMOS_DEV_EXT_STATUS_E;

/**
* @enum tagIMOSDevBelongAttr
* @brief  设备归属属性枚举
* @attention 无
*/
typedef enum tagIMOSDevBelongAttr
{
    IMOS_DEV_LOCAL = 0,                        /**< 属于本域的设备 */
    IMOS_DEV_FOREIGN = 1,                      /**< 属于外域的设备 */
    IMOS_DEV_BELONG_ALL = 2,                   /**< 所有设备 */
    IMOS_DEV_BELONG_MAX,                       /**< 最大值 */
    IMOS_DEV_BELONG_INVALID = 0xFFFFFFFF       /**< 无效值 */
}IMOS_DEV_BELONG_ATTR;

/**
* @enum tagIMOSFaultDevStatus
* @brief  设备故障状态枚举选项
* @attention 无
*/
typedef enum tagIMOSFaultDevStatus
{
    IMOS_DEV_FAULT_STATUS_ONLINE = 0,          /**< 设备正常在线 */
    IMOS_DEV_FAULT_STATUS_FAULT = 1,           /**< 设备在线故障 */
    IMOS_DEV_FAULT_STATUS_OFFLINE = 2,         /**< 设备离线 */
    IMOS_DEV_FAULT_STATUS_ALL = 3,             /**< 设备所有状态 */
    IMOS_DEV_FAULT_STATUS_MAX,                 /**< 最大值 */
    IMOS_DEV_FAULT_STATUS_INVALID = 0xFFFFFFFF /**< 无效值 */
}IMOS_DEV_FAULT_STATUS_E;

/**
* @enum tagIMOSDomainDevType
* @brief 域设备类型(产品化配置数据,即从产品角度所能看到的域设备类型)
* @attention
*/
typedef enum tagIMOSDomainDevType
{
    IMOS_DOMAIN_DEV_TYPE_DA          = 0,        /**< DA */
    IMOS_DOMAIN_DEV_TYPE_DVR         = 1,        /**< DVR */
    IMOS_DOMAIN_DEV_TYPE_NVR         = 2,        /**< NVR */
    IMOS_DOMAIN_DEV_TYPE_PLS         = 3,        /**< PL-S */
    IMOS_DOMAIN_DEV_TYPE_V3VM        = 4,        /**< V3VM */
    IMOS_DOMAIN_DEV_TYPE_ISC3000E    = 5,        /**< ISC3000-E */

    IMOS_DOMAIN_DEV_TYPE_MAX,                    /**< 最大值 */
    IMOS_DOMAIN_DEV_TYPE_INVALID     = 0xFFFF    /**< 无效值 */
} IMOS_DOMAIN_DEV_TYPE_E;

/**
* @enum tagIMOSDBConnectionNum
* @brief 数据库连接号码
* @attention 无
*/
typedef enum tagIMOSDBConnectionNum
{
    IMOS_DB_CONNECTION_CM = 1,                  /**< 配置模块数据库连接号码 */
    IMOS_DB_CONNECTION_MAX,                     /**< 最大值 */
    IMOS_DB_CONNECTION_INVALID = 0xFFFFFFFF     /**< 无效值 */
}IMOS_DB_CONNECTION_NUM_E;

/**
* @enum tagIMOSDomainDeviceType
* @brief 域设备类型(对于VM设备,为产品化配置数据)
* @attention
*/
typedef enum tagIMOSDomainDeviceType
{
    /* VM设备(即在"IMOS平台一套代码"基础上,产品化形成的设备)的类型:0～99 */
    IMOS_DOMAIN_DEVICE_TYPE_VM8500      = 0,       /**< VM8500 */
    IMOS_DOMAIN_DEVICE_TYPE_VM5500      = 1,       /**< VM5500 */
    IMOS_DOMAIN_DEVICE_TYPE_ECR3308HF   = 2,       /**< ECR3308-HF */
    IMOS_DOMAIN_DEVICE_TYPE_ECR3316HF   = 3,       /**< ECR3316-HF */
    IMOS_DOMAIN_DEVICE_TYPE_ISC3000E    = 4,       /**< ISC3000-E */
    IMOS_DOMAIN_DEVICE_TYPE_ISC3000S    = 5,       /**< ISC3000-S */
    IMOS_DOMAIN_DEVICE_TYPE_PL          = 6,       /**< PL */
    IMOS_DOMAIN_DEVICE_TYPE_PLS         = 7,       /**< PL-S */
    IMOS_DOMAIN_DEVICE_TYPE_ISC6000     = 8,       /**< ISC6000 */

    IMOS_DOMAIN_DEVICE_TYPE_ISC3500E    = 9,       /**< ISC3500-E */
    IMOS_DOMAIN_DEVICE_TYPE_ISC3500S    = 10,       /**< ISC3500-S */
    IMOS_DOMAIN_DEVICE_TYPE_ECR3308HFE   = 11,       /**< ECR3308-HF-E */
    IMOS_DOMAIN_DEVICE_TYPE_ECR3316HFE   = 12,       /**< ECR3316-HF-E */

    /* H3C其它域设备:100～ */
    IMOS_DOMAIN_DEVICE_TYPE_DA8500      = 100,     /**< DA8500 */

    IMOS_DOMAIN_DEVICE_TYPE_MAX,                   /**< 最大值 */
    IMOS_DOMAIN_DEVICE_TYPE_INVALID     = 0xFFFF   /**< 无效值 */
} IMOS_DOMAIN_DEVICE_TYPE_E;

/**
* @enum tagIMOSBooleanType
* @brief 开关量类型
* @attention 无
*/
typedef enum tagIMOSBooleanType
{
    IMOS_BOOLEAN_TYPE_IN = 0,                   /**< 输入开关量 */
    IMOS_BOOLEAN_TYPE_OUT = 1,                  /**< 输出开关量 */
    IMOS_BOOLEAN_TYPE_NONE = 2,                 /**< 所有开关量类型 */

    IMOS_BOOLEAN_TYPE_MAX,                      /**< 最大值 */
    IMOS_BOOLEAN_TYPE_INVALID = 0xFFFFFFFF      /**< 无效值 */
}IMOS_BOOLEAN_TYPE_E;

/**
* @name 域设备类型(字符串值)
* @{
*/

/** @brief H3C ISC6000 */
#define IMOS_DOMAIN_DEV_TYPE_ISC6000_STR        "H3C-ISC6000"

/** @brief H3C VM8500 */
#define IMOS_DOMAIN_DEV_TYPE_VM8500_STR         "H3C-VM8500"

/** @brief H3C VM5500 */
#define IMOS_DOMAIN_DEV_TYPE_VM5500_STR         "H3C-VM5500"

/** @brief H3C ECR3308HF */
#define IMOS_DOMAIN_DEV_TYPE_ECR3308HF_STR      "H3C-ECR3308HF"

/** @brief H3C ECR3316HF */
#define IMOS_DOMAIN_DEV_TYPE_ECR3316HF_STR      "H3C-ECR3316HF"

/** @brief H3C ECR3308HF_E */
#define IMOS_DOMAIN_DEV_TYPE_ECR3308HF_E_STR      "H3C-ECR3308HF-E"

/** @brief H3C ECR3316HF_E */
#define IMOS_DOMAIN_DEV_TYPE_ECR3316HF_E_STR      "H3C-ECR3316HF-E"

/** @brief H3C VM8000 */
#define IMOS_DOMAIN_DEV_TYPE_VM8000_STR         "H3C-VM8000"

/** @brief H3C VM5000 */
#define IMOS_DOMAIN_DEV_TYPE_VM5000_STR         "H3C-VM5000"

/** @brief H3C ISC3000 */
#define IMOS_DOMAIN_DEV_TYPE_ISC3000_STR        "H3C-ISC3000"

/** @brief H3C ISC3000-E */
#define IMOS_DOMAIN_DEV_TYPE_ISC3000E_STR       "H3C-ISC3000-E"

/** @brief H3C ISC3000-S */
#define IMOS_DOMAIN_DEV_TYPE_ISC3000S_STR       "H3C-ISC3000-S"

/** @brief H3C DA8500 */
#define IMOS_DOMAIN_DEV_TYPE_DA8500_STR         "H3C-DA8500"

/** @brief Aproud PL */
#define IMOS_DOMAIN_DEV_TYPE_PL_STR             "Aproud-PL"

/** @brief Aproud PL-S */
#define IMOS_DOMAIN_DEV_TYPE_PLS_STR            "Aproud-PL-S"

/** @brief Aproud ONU */
#define IMOS_DOMAIN_DEV_TYPE_ONU_STR            "Aproud-ONU"

/** @brief 非H3C外域 */
#define IMOS_DOMAIN_DEV_TYPE_OTHER_STR          "Other"
/** @} */

/**
* @enum tagIMOSPhyOutChnlWorkingMode
* @brief 物理输出通道的工作模式
* @attention
*/
typedef enum tagIMOSPhyOutChnlWorkingMode
{
    IMOS_PCWM_1_SPLIT   = 1,    /**< 1分屏 */
    IMOS_PCWM_4_SPLIT   = 4,    /**< 4分屏 */
    IMOS_PCWM_6_SPLIT   = 6,    /**< 6分屏 */
    IMOS_PCWM_8_SPLIT   = 8,    /**< 8分屏 */
    IMOS_PCWM_9_SPLIT   = 9,    /**< 9分屏 */
    IMOS_PCWM_10_SPLIT  = 10,   /**< 10分屏 */
    IMOS_PCWM_13_SPLIT  = 13,   /**< 13分屏 */
    IMOS_PCWM_16_SPLIT  = 16,   /**< 16分屏 */

    IMOS_PCWM_BUTT
} IMOS_PHY_OUT_CHNL_WORKING_MODE_E;

/** @brief 低温告警温度上限 - 最小 */
#define IMOS_LOW_TEMPERATURE_ALARM_MIN -100

/** @brief 低温告警温度上限 - 最大 */
#define IMOS_LOW_TEMPERATURE_ALARM_MAX 49

/** @brief 高温告警温度下限 - 最小 */
#define IMOS_HIGH_TEMPERATURE_ALARM_MIN 50

/** @brief 高温告警温度下限 - 最大 */
#define IMOS_HIGH_TEMPERATURE_ALARM_MAX 104

/**
* @enum tagIMOSNatLevel
* @brief Nat级别
* @attention 无
*/
typedef enum tagIMOSNatLevel
{
    IMOS_NAT_LEVEL_LOWER = 1,                   /**< nat级别较低，即相比本域，被比较方在nat内 */
    IMOS_NAT_LEVEL_EQUAL = 2,                   /**< 同级别，即相比本域没有经过nat */
    IMOS_NAT_LEVEL_UPPER = 3,                   /**< nat级别较高，即相比本域，本域在nat内，被比较方在公网 */

    IMOS_NAT_LEVEL_INVALID = 0xFFFFFFFF         /**< 无效值 */
}IMOS_NAT_LEVEL_E;


#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_DEF_H_ */

