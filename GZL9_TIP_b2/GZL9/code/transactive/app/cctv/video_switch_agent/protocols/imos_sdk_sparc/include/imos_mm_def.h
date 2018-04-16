/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_mm_def.h
  Project Code: IMOS MM
   Module Name: MM
  Date Created: 2009-03-31
        Author: yangpengfei/06985
   Description: MM�����꼰ö�ٶ���

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef __IMOS_MM_DEF_H__
#define __IMOS_MM_DEF_H__

/** ����ģʽ�������� */
typedef enum tagMMUpgMode
{
    MM_UPG_MODE_AUTO = 1,        /**< �Զ����� */

    MM_UPG_MODE_MANUAL = 2          /**< �ֶ����� */
} MM_UPGMODE_E;

/** @brief �������ͣ���Чֵ, �����ϰ汾 Added by z07067 for DVR. 2010-12-09 of VVD44847 */
#define MM_DOMAIN_SUBTYPE_INVALID            		0

/** @brief �������ͣ��������� */
#define MM_DOMAIN_SUBTYPE_LOCAL_PHYSICAL            1

/** @brief �������ͣ������������ */
#define MM_DOMAIN_SUBTYPE_LOCAL_VIRTUAL             2

/** @brief �������ͣ������������ */
#define MM_DOMAIN_SUBTYPE_FOREIGN_VIRTUAL           3

/** @brief �������ͣ��ϼ��������� */
#define MM_DOMAIN_SUBTYPE_FOREIGN_PHYSICAL_UPPER    4

/** @brief �������ͣ��¼��������� */
#define MM_DOMAIN_SUBTYPE_FOREIGN_PHYSICAL_LOWER    5

/** @brief �������ͣ�ƽ���������� */
#define MM_DOMAIN_SUBTYPE_FOREIGN_PHYSICAL_PARITY   6

/** @brief �������ͣ���������ECR3308-HF */
#define MM_DOMAIN_SUBTYPE_FOREIGN_DVR3308           7

/** @brief �������ͣ�������ECR3316-HF */
#define MM_DOMAIN_SUBTYPE_FOREIGN_DVR3316           8

/** @brief �������ͣ���������ECR3308-HF-E */
#define MM_DOMAIN_SUBTYPE_FOREIGN_ECR3308E           9

/** @brief �������ͣ�������ECR3316-HF-E */
#define MM_DOMAIN_SUBTYPE_FOREIGN_ECR3316E           10

/** @brief �������ͣ���������DVR���ֵ:Ԥ��58��DVR�� */
#define MM_DOMAIN_SUBTYPE_FOREIGN_DVR_MAX           64

/** @brief �����û� */
#define MM_USER_TYPE_LOCAL_DOMAIN 1UL

/** @brief �����û� */
#define MM_USER_TYPE_FOREIGN_DOMAIN 2UL

/** @brief �����ɫ */
#define MM_ROLE_TYPE_LOCAL_DOMAIN 1UL

/** @brief �����ɫ */
#define MM_ROLE_TYPE_FOREIGN_DOMAIN 2UL

/**
* @struct tagMMDomainType
* @brief ������ö��
* @attention ��
*/
typedef enum tagMMDomainType
{
    MM_DOMAIN_TYPE_PHYSICAL = 1,           /**< ������ */

    MM_DOMAIN_TYPE_VIRTUAL = 2             /**< ������ */

}MM_DOMAINTYPE_E;

/** ����������� */
typedef enum tagMMJobSchdType
{
    MM_JOB_SCHD_UNINITIAL = 0,                  /**< �����������δ��ʼ�� */
    MM_JOB_ONCE = 1,                            /**< һ���� */
    MM_JOB_IMMEDIATELY = 2,                     /**< ����ִ�� */
    MM_JOB_MONTHLY = 3,                         /**< ÿ��(1~28��) */
    MM_JOB_LAST_DAY_OF_MONTH = 4,               /**< ÿ�����һ�� */
    MM_JOB_WEEKLY = 5,                          /**< ÿ�� */
    MM_JOB_DAILY = 6,                           /**< ÿ�� */
    MM_JOB_HOURS_INTERVAL = 7                   /**< ÿ����Сʱ */
} MM_JOBSCHDTYPE_E;

/** �����豸ִ��״̬ */
typedef enum tagMMJobDevStatus
{
    MM_JOBDEV_STATUS_UNINITIAL = 0,             /**< �����豸ִ��״̬δ��ʼ�� */
    MM_JOBDEV_WAITTING = 1,                     /**< �ȴ� */
    MM_JOBDEV_CANCELED = 2,                     /**< ȡ�� */
    MM_JOBDEV_RUNNING = 3,                      /**< ���� */
    MM_JOBDEV_FINISHED = 4                      /**< ���� */
}MM_JOBDEVSTATUS_E;

/** �����豸ִ�н�� */
typedef enum tagMMJobDevResult
{
    MM_JOBDEV_RESULT_UNINITIAL = 0,              /**< �����豸ִ�н��δ��ʼ�� */
    MM_JOBDEV_UNKNOW = 1,                        /**< ���δ֪ */
    MM_JOBDEV_TIMEOUT = 2,                       /**< ��ʱ */
    MM_JOBDEV_FAILED = 3,                        /**< ʧ�� */
    MM_JOBDEV_SUCCESS = 4                        /**< �ɹ� */
}MM_JOBDEVRESULT_E;

/** �豸��������������� */
typedef enum tagMMDevUpgResult
{
    MM_UPG_DEV_RESULT_UNINITIAL = 0,      /**< δ��ʼ�� */

    MM_UPT_DEV_RESULT_UNKNOW = 1,             /**< δ֪ */

    MM_UPG_DEV_RESULT_SUCCESS = 2,            /**< �����ɹ� */

    MM_UPG_DEV_RESULT_VERSION_ERROR = 3,      /**< �汾���� */

    MM_UPG_DEV_RESULT_DOWNLOAD_ERROR = 4,     /**< ���ش��� */

    MM_UPG_DEV_RESULT_UPGCFG_ERROR = 5,       /**< �豸����ʧ�� */

    MM_UPG_DEV_RESULT_TIMEOUT_ERROR = 6,      /**< �豸������ʱ */

    MM_UPG_DEV_RESULT_FAIL = 7                /**< �豸����ʧ�� */
} MM_UPGDEVEXTRESULT_E;

#endif
