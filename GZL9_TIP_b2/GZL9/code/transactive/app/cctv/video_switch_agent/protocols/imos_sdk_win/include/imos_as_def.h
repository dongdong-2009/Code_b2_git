/*******************************************************************************
 Copyright (c) 2003-2010, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_as_def.h

  Project Code: IMOS
   Module Name: AS、CS公共对外头文件
  Date Created: 2009-03-19
        Author: Baogang/03821
   Description: 定义AS、CS公共对外使用的宏、枚举

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------

*******************************************************************************/

#ifndef __IMOS_AS_DEF_H__
#define __IMOS_AS_DEF_H__

#ifdef  __cplusplus
extern "C"{
#endif


/** @brief XP最大窗格数目 */
#define AS_XP_WIN_MAX_NUM 64

/** @brief 单个任务中最大设备数 */
#define UM_MAX_DEVICENUM_INJOB 200

/** @brief Ptz模块的预置位的最小值 */
#define PTZ_PRESET_MINVALUE      1

/** @brief 云台释放时间最小值，参考默认值，最短半分钟，下面单位为秒 */
#define MIN_PTZ_RELEASE_TIME     30

/** @brief 云台释放时间最大值，参考默认值，最长60分钟，下面单位为秒 */
#define MAX_PTZ_RELEASE_TIME     3600

/** @brief 轮切时间间隔最小值，单位是毫秒 */
#define MIN_TIME_SWITCH_INTERVAL 5000

/** @brief 轮切时间间隔最大值，单位是毫秒 */
#define MAX_TIME_SWITCH_INTERVAL 3600000

/** @brief 摄像机的缺省License规格 */
#define AS_DEFAULT_LICENSE_FOR_CAMERA   8

/** @brief DM的缺省License规格 */
#define AS_DEFAULT_LICENSE_FOR_DM       1

/** @brief MS的缺省License规格 */
#define AS_DEFAULT_LICENSE_FOR_MS       1

/** @brief 存储设备的缺省License规格 */
#define AS_DEFAULT_LICENSE_FOR_STORAGE_DEV  1

/** @brief 串口类型 */
typedef enum tagSerialType
{
    SERIAL_TYPE_RS232 = 1,          /**< RS232 */
    SERIAL_TYPE_RS422 = 2,          /**< RS422 */
    SERIAL_TYPE_RS485 = 3,          /**< RS485 */

    SERIAL_TYPE_MAX,
    SERIAL_TYPE_INVALID = 0xFF
}SERIAL_TYPE_E;

/**
* @enum tagPlanType
* @brief 计划类型
* @attention 无
*/
typedef enum tagPlanType
{
    PLAN_TYPE_DAY       = 0,        /**< 日计划 */
    PLAN_TYPE_WEEK,                 /**< 周计划 */

    PLAN_TYPE_MAX,
    PLAN_TYPE_INVALID   = 0xFF
}PLAN_TYPE_E;

/** @brief 存储类型 */
typedef enum tagASDEVCMStorageType
{
    STORAGE_TYPE_IPSAN = 0,         /**< IPSAN存储 */
    STORAGE_TYPE_NAS = 1,           /**< NAS存储 */
    STORAGE_TYPE_LOCAL = 2,         /**< 本地存储 */

    STORAGE_TYPE_MAX,
    STORAGE_TYPE_INVALID = 0xFF
}AS_DEVCM_STORAGE_TYPE_E;

/** @brief 磁盘满策略 */
typedef enum tagASDEVCMDiskFullPolicy
{
    DISK_FULL_POLICY_STOP = 0,      /**< 磁盘满停止存储 */
    DISK_FULL_POLICY_OVERLOAD= 1,   /**< 磁盘满覆盖存储 */

    DISK_FULL_POLICY_MAX,
    DISK_FULL_POLICY_INVALID = 0xFF
} AS_DEVCM_DISK_FULL_POLICY_E;


/** @brief 业务当前状态 */
typedef enum tagSRVState
{
    SRV_STATE_IDLE = 0,             /**< 空闲状态 实况、轮切、组显示、组轮巡共用业务状态 */
    SRV_STATE_ACTIVE,               /**< 运行状态 实况、轮切、组显示、组轮巡共用业务状态 */
    SRV_STATE_PAUSE,                /**< 暂停状态 轮切、组轮巡共用业务状态 */
    SRV_STATE_UNACTIVE,             /**< 休眠状态 实况、轮切、组轮巡共用业务状态 */
    SRV_STATE_SUSPEND,              /**< 挂起状态 轮切业务状态 */

    SRV_STATE_MAX,                  /**< 最大值 */
    SRV_STATE_INVALID = 0xFF        /**< 无效值 */
}SRV_STATE_E;

#ifdef  __cplusplus
}
#endif

#endif  /** __GBSPT_PUB_H__ */


