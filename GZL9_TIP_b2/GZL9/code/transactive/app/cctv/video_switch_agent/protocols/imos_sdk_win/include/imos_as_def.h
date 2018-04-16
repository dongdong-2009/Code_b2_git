/*******************************************************************************
 Copyright (c) 2003-2010, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_as_def.h

  Project Code: IMOS
   Module Name: AS��CS��������ͷ�ļ�
  Date Created: 2009-03-19
        Author: Baogang/03821
   Description: ����AS��CS��������ʹ�õĺꡢö��

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


/** @brief XP��󴰸���Ŀ */
#define AS_XP_WIN_MAX_NUM 64

/** @brief ��������������豸�� */
#define UM_MAX_DEVICENUM_INJOB 200

/** @brief Ptzģ���Ԥ��λ����Сֵ */
#define PTZ_PRESET_MINVALUE      1

/** @brief ��̨�ͷ�ʱ����Сֵ���ο�Ĭ��ֵ����̰���ӣ����浥λΪ�� */
#define MIN_PTZ_RELEASE_TIME     30

/** @brief ��̨�ͷ�ʱ�����ֵ���ο�Ĭ��ֵ���60���ӣ����浥λΪ�� */
#define MAX_PTZ_RELEASE_TIME     3600

/** @brief ����ʱ������Сֵ����λ�Ǻ��� */
#define MIN_TIME_SWITCH_INTERVAL 5000

/** @brief ����ʱ�������ֵ����λ�Ǻ��� */
#define MAX_TIME_SWITCH_INTERVAL 3600000

/** @brief �������ȱʡLicense��� */
#define AS_DEFAULT_LICENSE_FOR_CAMERA   8

/** @brief DM��ȱʡLicense��� */
#define AS_DEFAULT_LICENSE_FOR_DM       1

/** @brief MS��ȱʡLicense��� */
#define AS_DEFAULT_LICENSE_FOR_MS       1

/** @brief �洢�豸��ȱʡLicense��� */
#define AS_DEFAULT_LICENSE_FOR_STORAGE_DEV  1

/** @brief �������� */
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
* @brief �ƻ�����
* @attention ��
*/
typedef enum tagPlanType
{
    PLAN_TYPE_DAY       = 0,        /**< �ռƻ� */
    PLAN_TYPE_WEEK,                 /**< �ܼƻ� */

    PLAN_TYPE_MAX,
    PLAN_TYPE_INVALID   = 0xFF
}PLAN_TYPE_E;

/** @brief �洢���� */
typedef enum tagASDEVCMStorageType
{
    STORAGE_TYPE_IPSAN = 0,         /**< IPSAN�洢 */
    STORAGE_TYPE_NAS = 1,           /**< NAS�洢 */
    STORAGE_TYPE_LOCAL = 2,         /**< ���ش洢 */

    STORAGE_TYPE_MAX,
    STORAGE_TYPE_INVALID = 0xFF
}AS_DEVCM_STORAGE_TYPE_E;

/** @brief ���������� */
typedef enum tagASDEVCMDiskFullPolicy
{
    DISK_FULL_POLICY_STOP = 0,      /**< ������ֹͣ�洢 */
    DISK_FULL_POLICY_OVERLOAD= 1,   /**< ���������Ǵ洢 */

    DISK_FULL_POLICY_MAX,
    DISK_FULL_POLICY_INVALID = 0xFF
} AS_DEVCM_DISK_FULL_POLICY_E;


/** @brief ҵ��ǰ״̬ */
typedef enum tagSRVState
{
    SRV_STATE_IDLE = 0,             /**< ����״̬ ʵ�������С�����ʾ������Ѳ����ҵ��״̬ */
    SRV_STATE_ACTIVE,               /**< ����״̬ ʵ�������С�����ʾ������Ѳ����ҵ��״̬ */
    SRV_STATE_PAUSE,                /**< ��ͣ״̬ ���С�����Ѳ����ҵ��״̬ */
    SRV_STATE_UNACTIVE,             /**< ����״̬ ʵ�������С�����Ѳ����ҵ��״̬ */
    SRV_STATE_SUSPEND,              /**< ����״̬ ����ҵ��״̬ */

    SRV_STATE_MAX,                  /**< ���ֵ */
    SRV_STATE_INVALID = 0xFF        /**< ��Чֵ */
}SRV_STATE_E;

#ifdef  __cplusplus
}
#endif

#endif  /** __GBSPT_PUB_H__ */


