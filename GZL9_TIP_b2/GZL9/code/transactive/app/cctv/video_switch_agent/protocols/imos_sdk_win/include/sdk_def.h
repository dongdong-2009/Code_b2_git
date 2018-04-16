/*******************************************************************************
 Copyright (c) 2008-2009, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              sdk_def.h

  Project Code: SDK
   Module Name: SDK
  Date Created: 2009-02-27
        Author: yangbin (07022)
                sunyifan (s06966)
                zhuyi (KF1397)
   Description: �꣬ö�ٶ����ĵ�

--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_SDK_DEFINE_H_
#define _IMOS_SDK_DEFINE_H_

#ifdef  __cplusplus
extern "C"{
#endif

/** @brief ѡ��ֵ�ڴ�鳤����Сֵ */
#define IMOS_OPTION_BUFFER_LEN_MIN              1

/** @brief ѡ��ֵ�ڴ�鳤�����ֵ */
#define IMOS_OPTION_BUFFER_LEN_MAX              64


/** @brief һ������ǽ�����������������Ŀ */
#define IMOS_MONITOR_MAXNUM_PER_WALL            256

/*@brief �澯�����洢�������Ŀ */
#define IMOS_ALARM_STORE_ACTION_MAXNUM          16

/*@brief �澯����Ԥ��λ�������Ŀ */
#define IMOS_ALARM_PRESET_ACTION_MAXNUM         16

/*@brief �澯����ʵ��������������Ŀ */
#define IMOS_ALARM_SCREENLIVE_ACTION_MAXNUM     16

/*@brief �澯����ʵ�����������������Ŀ */
#define IMOS_ALARM_MONITORLIVE_ACTION_MAXNUM    16

/*@brief �澯������ͼ��˸�������Ŀ */
#define IMOS_ALARM_GIS_ACTION_MAXNUM            16

/*@brief �澯��������������������Ŀ */
#define IMOS_ALARM_SWITCHOUT_ACTION_MAXNUM      16

/*@brief �澯�������ݵ������Ŀ */
#define IMOS_ALARM_BACKUP_ACTION_MAXNUM         16

/*@brief ͸��ͨ��3PCCģʽ������Ĵ����� */
#define IMOS_TRANSPARENT_SERIAL_NUM             2

/*@brief �������������������Ŀ */
#define IMOS_SWITCH_CAM_MAXNUM                  32

/*@brief Ѳ��·��Ԥ��λ�������ֵ */
#define IMOS_CRUISE_PATH_PRESET_MAX_COUNT       32

/*@brief �������������λ��Ŀ */
#define IMOS_ARRAY_SLOT_MAXNUM                  10

/*@brief ��ͨ���ڸ�����������Ŀ(3) */
#define IMOS_MASK_AREA_MAXNUM_III               3

/*@brief ��ͨ���ڸ�����������Ŀ(1) */
#define IMOS_MASK_AREA_MAXNUM_I                 1

/*@brief ��ͨ���ڵ�����������Ŀ */
#define IMOS_COVER_AREA_MAXNUM                   4

/*@brief NTP������������ */
#define IMOS_NTP_SERVER_MAXNUM                  3

/*@brief һ���澯Դ�豸�����ĵ�ͼ�б�������� */
#define IMOS_GIS_FOR_ALARM_SRC_MAX_NUM          5

/** ��˾��ͬ��ų��� */
#define IMOS_CONTRACT_ID                        32

/** license���кŹ���� */
#define IMOS_LICENSE_SERIALS_NUM                256

/** license��Ե�ҵ�������� */
#define IMOS_LICENSE_SERVICE_TYPE_NUM           20

/** DC��ÿ������ͨ������֧�ֵ�����߼�ͨ���� */
#define IMOS_DC_LOGIC_CHANNEL_MAXNUM            4

/*@brief ��ҳ��ѯ�������ֵ */
#define IMOS_PAGE_QUERY_ROW_MAX_NUM             200

/** @brief һ�����͵������Դ��Ŀ */
#define IMOS_ONCE_SHARE_RES_MAXNUM              1    /**< MAX_DEVSHARE_ONCE_PUSH */

/** @brief EC�µ�CAM�����Ŀ*/
#define IMOS_MAX_CAMERA_NUM_PER_EC              16   /**< MAX_CAMERA_NUM_PER_EC */

/** @brief DC�µ�SCREEN�����Ŀ*/
#define IMOS_MAX_SCREEN_NUM_PER_DC              16   /**< MAX_SCREEN_NUM_PER_DC */

/** @brief URL�ַ�������.��Ҫ��MCͬ������ */
#define IMOS_IE_URL_LEN                         IMOS_STRING_LEN_512

/*@brief ��Чͨ���� */
#define IMOS_INVALID_CHANNEL_NO                 0xFF

/*@brief ÿ��XP��Ӧ��������� */
#define IMOS_XP_WIN_MAX_NUM                     64

/*@brief ��ѯ�����ݵ����̶� */
#define IMOS_QUERY_DATA_MAX_LEN                 64

/*@brief ��ѯ�������� */
#define IMOS_QUERY_ITEM_MAX_NUM                 16

/*@brief ����ӵ����������Ŀ */
#define IMOS_MAX_EXTERNAL_DOMAIN_NUM            1024

/*@brief ��Ϊ�����û����������ɫ���� */
#define IMOS_MAX_USER_ROLES_NUM                 16

/*@brief Notify��󳤶� */
#define IMOS_MAX_NOTIFY_LEN                     4096

/*@brief �豸״̬��������ֵ */
#define IMOS_DEV_STATUS_MAX_NUM                4


/*@brief ����������������ʾ��Ŀ */
#define IMOS_DISPLAY_MAXNUM_PER_LAYOUT          IMOS_MONITOR_MAXNUM_PER_WALL

/*@brief ����ʾ����������ʾ��Ŀ */
#define IMOS_DISPLAY_MAXNUM_PER_SALVO           IMOS_MONITOR_MAXNUM_PER_WALL

/*@brief ͨ����������������Դ��Ŀ */
#define IMOS_COMMON_SWITCH_RES_MAXNUM           256

/** @brief һ����ͼ���������õ�������������� */
#define IMOS_GIS_MAX_HOTZONE_NUM_PER_MAP        32
/** @brief һ����ͼ���������õ�ͼ��(�����ȵ㡢�ı��������ͼ�ꡢ�澯Դͼ�ꡢ�澯���ͼ��)��������� */
#define IMOS_GIS_MAX_ICON_NUM_PER_MAP           256

/** @brief ��ͼ�����Ĳ���������*/
#define IMOS_MAX_GIS_MAP_PARAM_NUM             (IMOS_GIS_MAX_HOTZONE_NUM_PER_MAP + IMOS_GIS_MAX_ICON_NUM_PER_MAP)

/*
* @name ��������
* @{
*/
/** @brief H3C VM/ISC */
#define IMOS_MANUFACTURER_TYPE_VMISC        "H3C VM/ISC"

/** @brief H3C DA */
#define IMOS_MANUFACTURER_TYPE_DA           "H3C DA"

/** @brief Other */
#define IMOS_MANUFACTURER_TYPE_OTHER        "Other"

/** @brief Aproud PL */
#define IMOS_MANUFACTURER_TYPE_PL           "Aproud PL"

/** @brief Aproud PL-S */
#define IMOS_MANUFACTURER_TYPE_PLS          "Aproud PL-S"

/** @brief H3C ECR HF */
#define IMOS_MANUFACTURER_TYPE_ECRHF        "H3C ECR-HF"
/** @} */

/*
* @name ��̨Э������
* @{
*/
/** @brief INTERNAL-PTZ */
#define IMOS_PTZ_PROTOCOL_TYPE_INTERNAL     "INTERNAL-PTZ"
/** @} */


/*******************************************************************************
SS��������
����Ѷ�����Ķ���ǰ������IMOS_�� �Ա�����
*******************************************************************************/
/*@brief Email��ǰ֪ͨ���� */
#define IMOS_EMAIL_NOTIFICATION_NUMBER                  3

/*@brief URL��ַ���� */
#define IMOS_URL_LEN                                    256

/*@brief �᳡���Ƶ���󳤶� */
#define IMOS_HD_MAX_SITE_NAME_LEN                       31
#define IMOS_SITE_NAME_LEN                              48 /*< �ο� #IMOS_HD_MAX_SITE_NAME_LEN */

/*@brief �������Ƶ���󳤶� */
#define IMOS_HD_MAX_CONF_NAME_LEN                       31
#define IMOS_CONF_NAME_LEN                              48  /*< �ο� #IMOS_HD_MAX_CONF_NAME_LEN */

/*@brief �᳡���������֧�ֵ���ƵЭ���� */
#define IMOS_HD_MAX_AUDIO_PROTOCOL_NUM                  20

/*@brief ��������� */
#define IMOS_HD_MAX_BANNER_LEN                          43
#define IMOS_BANNER_LEN                                 66 /*< �ο� #IMOS_HD_MAX_BANNER_LEN */


/*@brief �����������󳤶� */
#define IMOS_HD_MAX_CONF_ALIAS_LEN                      31
#define IMOS_CONF_ALIAS_LEN                             (IMOS_HD_MAX_CONF_ALIAS_LEN + 1)

/*@brief Э�鶨���ÿ����������᳡�� */
#define IMOS_HD_MAX_SITE_PER_CONF_NUM                   256

/*@brief �ӻ��������� */
#define IMOS_HD_MAX_SUBPIC_NUM                          64

/*@brief �᳡�����������͸������֧�ֵ���Ƶ��ʽ�� */
#define IMOS_HD_MAX_VIDEO_FORMAT_NUM                    32

/*@brief �᳡���������֧�ֵ���Ƶ֡���� */
#define IMOS_HD_MAX_VIDEO_FRAMERATE_NUM                 7

/*@brief �᳡���������֧�ֵ���ƵЭ���� */
#define IMOS_HD_MAX_VIDEO_PROTOCOL_NUM                  4

/*@brief �᳡���������֧�ֵ���Ƶ������ */
#define IMOS_HD_MAX_VIDEO_RATE_NUM                      8

/*@brief ��������ͬʱ������������  */
#define IMOS_HD_MAX_SIM_CAP_NUM                         8



/*@brief ������������Ѳ���� */
#define IMOS_MC_MAX_CONF_CYCLE_TABLE_COUNT              10


/*@brief �����������󳤶� */
#define IMOS_MC_MAX_CONF_PWD_LEN                        11
#define IMOS_CONF_PWD_LEN                               (IMOS_MC_MAX_CONF_PWD_LEN + 1)

/*@brief ������ڸ��� */
#define IMOS_MC_MAX_LAN_NET_NUM                         4

/*@brief ���ڱ�ʶ��󳤶� */
#define IMOS_HD_MAX_NETCARD_NAME_LEN                    31
#define IMOS_NETCARD_NAME_LEN                           48 /*< �ο� #IMOS_HD_MAX_NETCARD_NAME_LEN */

/*@brief MCU���Ƶ���󳤶� */
#define IMOS_MC_MAX_MCU_NAME_LEN                        31
#define IMOS_MCU_NAME_LEN                               (IMOS_MC_MAX_MCU_NAME_LEN + 1)

/*@brief MCU�ⲿIP��ַ�������� */
#define IMOS_MC_MAX_MCU_IP_NUM                          4

/*@brief ÿ�����ڿ�����·�ɸ��� */
#define IMOS_MC_MAX_LAN_ROUTE_NUM                       20

/*@brief ÿ������ARP������ */
#define IMOS_MC_MAX_LAN_ARP_NUM                         20

/*@brief MAC��ַ��󳤶� */
#define IMOS_MC_MAX_MAC_ADDR_LEN                        20


/*@brief ����汾�ŵ���󳤶� */
#define IMOS_HD_MAX_SOFT_VERSION_LEN                    255
#define IMOS_SOFT_VERSION_LEN                           (IMOS_HD_MAX_SOFT_VERSION_LEN + 1)

/*@brief ÿ�������DSP�� */
#define IMOS_HD_MAX_DSP_PER_PLANK                       15

/*@brief ÿ��DSP����������� */
#define IMOS_HD_MAX_DSP_NUM_PER_CONF                    4

/*@brief �����ʼ�����󳤶� */
#define IMOS_HD_MAX_EMAIL_NAME_LEN                      63

/*@brief ���ͼ���ػ᳡���� */
#define IMOS_MC_MAX_WATCH_SITE_NUM                      9

/*@brief �ն���Ч������� */
#define IMOS_HD_VIDEO_OUT_NUM                          (4)

/*@brief �ն���/Ӳ���汾/���кų��� */
#define IMOS_HD_VERSION_LEN                            (32)

/*@brief UI��WEB��PC����û��� */
#define IMOS_HD_MAX_USER_NUM                           (32)

/*@brief Ԥ��λ�� */
#define IMOS_HD_MAX_CAM_PRE_NUM                        (20)

/*@brief �ն���������ַ������� */
#define IMOS_HD_MAX_NAME_LENGTH                        (32)

/*@brief �ն����IP�ַ������� */
#define IMOS_HD_MAX_IP_LENGTH                          (32)

/*@brief �ն����·���ַ������� */
#define IMOS_HD_MAX_PATH_LENGTH                        (100)

/*@brief �ն���������ַ������� */
#define IMOS_HD_MAX_PASSWORD_LENGTH                    (32)

/*@brief �ն����˽������ */
#define IMOS_HD_MAX_PRIVATE_NET_COUNT                  (4)

/*@brief ���MAC�ַ�����ʾ���� */
#define IMOS_HD_MAX_MAC_DISPLAYLENGTH                  (17)

/*@brief �����Ļ��Ŀ */
#define IMOS_TITLE_MAXNUM                              (2)

/*@brief �����Ļ���� */
#define IMOS_TITLE_LEN                                 (2404)

/*@brief һ����������ϵ�������Ŀ*/
#define IMOS_MAX_CONTACTS_IN_CONF                      (512)

/*@brief IMOS֧�ֵ���������Ŀ*/
#define IMOS_COMPONENT_MAX_NUM                         (50)

/*@brief MCU�����Ļ�᳡��Ŀ */
#define IMOS_MAX_SITE_LIVE_NUM                         (240)

/*@brief MCU�������鲥��Ŀ*/
#define IMOS_MAX_MULTI_ADDR_NUM                        (8)

/*@brief �����������·�� */
#define IMOS_MC_MAX_BITRATE_ADAPT_COUNT                (3)

/*@brief һ��������MCU�����Ŀ */
#define IMOS_MAX_MCU_IN_CONF                           (16)

/*@brief һ���������ն������Ŀ */
#define IMOS_MAX_TERM_IN_CONF                          (1024)


/*@brief ͸��ͨ��DIRECTģʽ�������IP�� */
#define IMOS_TRANSPARENT_IP_NUM                 1

/** @brief ���������㲥����֧�ֵ�������������  */
#define MAX_VOCBRD_CAM_NUM       128

/** @brief ��̨����ٶ�ֵ  */
#define MAX_PTZ_SPEED            9

/** @brief ����������󳤶�  */
#define IMOS_PROC_NAME_MAX_LEN                          32
 
/** @brief ������ֱ��ý��������ȱʡֵ */
#define IMOS_DEFAULT_NUM_OF_STREAM_BYPASS            1024

/**
* @enum tagOptionName
* @brief ѡ����ö��
* @attention
*/
typedef enum tagOptionName
{
    OPTION_NAME_CC_SERVER_ADDR      = 0,            /**< CC�ķ�������ַ, ��Ӧ�ṹ : (CHAR *) */
    OPTION_IMOS_ROOT_PATH           = 1,            /**< IMOSROOT��������, ��Ӧ�ṹ : (CHAR *) */
    OPTION_PROTOCOL_PORT_RANGE      = 2,            /**< Э��Ķ˿ڷ�Χ, ��Ӧ�ṹ : (PORT_RANGE_S *) */
    OPTION_MEDIA_PORT_RANGE         = 3,            /**< ý�����Ķ˿ڷ�Χ,��Ӧ�ṹ : (PORT_RANGE_S *) */

    OPTION_NAME_MAX,
    OPTION_NAME_INVALID             = 0XFFFFFFFF
} OPTION_NAME_E;

/** @brief ECR���볤�� */
#define IMOS_ECR_CODE_LEN               (IMOS_CODE_LEN - 8)

/*@brief ϵͳ֧�ֵĸ��ߵ������ */
#define IMOS_TRUNK_MAXNUM                       32

/*@brief ������������������Ŀ */
#define IMOS_TRUNK_CONTENT_MAXNUM               256

/** �ص�����������Ϣ���� */
typedef enum tagCallBackProcType
{
    PROC_TYPE_DEV_STATUS            = 0,            /**< �豸״̬����Ӧ�ṹ : AS_STAPUSH_UI_S */
    PROC_TYPE_ALARM                 = 1,            /**< �澯����Ӧ�ṹ : AS_ALARMPUSH_UI_S */
    PROC_TYPE_DEV_CAM_SHEAR         = 2,            /**< �������������Ӧ�ṹ : AS_DEVPUSH_UI_S */
    PROC_TYPE_MONITOR_BE_REAVED     = 3,            /**< ʵ������ռ����Ӧ�ṹ : CS_MONITOR_REAVE_NOTIFY_S */
    PROC_TYPE_SWITCH_BE_REAVED      = 4,            /**< ���б���ռ����Ӧ�ṹ : CS_SWITCH_REAVE_NOTIFY_S */
    PROC_TYPE_MONITOR_BE_STOPPED    = 5,            /**< ʵ����ֹͣ����Ӧ�ṹ : CS_MONITOR_REAVE_NOTIFY_S */
    PROC_TYPE_SWITCH_BE_STOPPED     = 6,            /**< ���б�ֹͣ����Ӧ�ṹ : CS_SWITCH_REAVE_NOTIFY_S */
    PROC_TYPE_VOCSRV_BE_REAVED      = 7,            /**< ��������ռ����Ӧ�ṹ : CS_VOCSRV_REAVE_NOTIFY_S */
    PROC_TYPE_PTZ_EVENT             = 8,            /**< ��̨�¼�֪ͨ����Ӧ�ṹ : CS_PTZ_REAVE_NOTIFY_S */
    PROC_TYPE_TRB_PROC              = 9,            /**< ���ϴ���֪ͨ����Ӧ�ṹ : CS_NOTIFY_UI_TRB_EVENT_PROC_S */
    PROC_TYPE_SRV_SETUP             = 10,           /**< ���ϻָ�ҵ����֪ͨ����Ӧ�ṹ : CS_NOTIFY_UI_SRV_SETUP_S */
    PROC_TYPE_XP_ALARM_SETUP        = 11,           /**< �澯������XP����֪ͨ����Ӧ�ṹ : CS_NOTIFY_UI_SRV_SETUP_S */

    PROC_TYPE_LOGOUT                = 12,           /**< �˳���½����Ӧ�ṹ :�� */

    PROC_TYPE_MEDIA_PARAM_CHANGE    = 13,           /**< ý������ı䣬��Ӧ�ṹ : CS_NOTIFY_UI_MEDIA_PARAM_CHANGE_PROC_S */

    PROC_TYPE_EXDOMAIN_STATUS       = 14,           /**< ����״̬����Ӧ�ṹ : AS_EXDOMAIN_STAPUSH_UI_S */

    PROC_TYPE_BACKUP_DATA_FINISH    = 15,           /**< ��Ϣ�������֪ͨ, ��Ӧ�ṹ : CS_BACKUP_FINISH_INFO_S */

    PROC_TYPE_TL_EVENT              = 16,           /**< ͸��ͨ���¼�֪ͨ����Ӧ�ṹ : CS_TL_REAVE_NOTIFY_S */

    PROC_TYPE_SALVO_UNIT_EVENT      = 17,           /**< ����ʾ��Ԫ�¼�֪ͨ, ��Ӧ�ṹ : CS_NOTIFY_SALVO_UNIT_EVENT_S */
    PROC_TYPE_SALVO_EVENT           = 18,           /**< ����ҵ���¼�֪ͨ, ��Ӧ�ṹ : CS_NOTIFY_UI_SALVO_EVENT_S */
    PROC_TYPE_START_XP_SALVO        = 19,           /**< ֪ͨUI��������Ѳ������ʾ, ��Ӧ�ṹ: CS_NOTIFY_START_XP_SALVO_S */

    PROC_TYPE_VODWALL_BE_REAVED     = 20,           /**< ֪ͨ�ط���ǽ����ռ����Ӧ�ṹ��CS_VODWALL_REAVE_NOTIFY_S */
    PROC_TYPE_VODWALL_BE_STOPPED    = 21,           /**< ֪ͨ�ط���ǽ��ֹͣ����Ӧ�ṹ��CS_VODWALL_REAVE_NOTIFY_S */


    /*******************************************************************************
    SS�������� �����ص�����
    *******************************************************************************/
    PROC_TYPE_ACCEPT_SPEAK_YESORNO  = 100,           /**< �������룬 ��Ӧ�ṹ ��CONF_SITE_INFO_EX_S */
    PROC_TYPE_CONF_STATUS_CHANGE    = 101,           /**< ����״̬�ı䣬 ��Ӧ�ṹ ��CONF_STATUS_INFO_EX_S ��������ڻ����ҷ����һ�����ڣ��ϱ�����δ��ʼ/�����ϱ������Ѿ����� */
    PROC_TYPE_DEVICE_CODE_CHANGE    = 102,           /**< �豸����ı䣬 ��Ӧ�ṹ ��DEVICE_CODE_CHANGE_INFO_EX_S */
    PROC_TYPE_DEVICE_CHANGE         = 103,           /**< �ն��豸������Ϣ�� �������豸������ɾ��ʱ�� �ϱ�������Ϣ�� ��Ӧ�Ľṹ ��DEVICE_CHANGE_INFO_EX_S */
    PROC_TYPE_MODIFY_TERM           = 104,           /**< �޸��ն���Ϣ�� ��Ӧ�Ľṹ ��MODIFY_TERM_REP_EX_S */
    PROC_TYPE_CHAIR_CHANGE          = 105,           /**< ��ǰ��ϯ�����ı䣬 ��ϯ�᳡�ͷ���᳡����Ϊ�ա���Ӧ�Ľṹ ��CONF_SITE_INFO_EX_S */
    PROC_TYPE_SPEAKER_CHANGE        = 106,           /**< ��ǰ�����˷����ı䣬 ��Ӧ�Ľṹ ��CONF_SITE_INFO_EX_S */
    PROC_TYPE_TERM_STATUS_CHANGE    = 107,           /**< �᳡�ն�״̬�ı䣬 ��Ӧ�Ľṹ ��TERM_STATUS_CHANGE_EX_S */
    PROC_TYPE_DELAY_CONF            = 108,           /**< �ӳٻ��飬 ��Ӧ�ṹ ��DELAY_CONF_INFO_EX_S */
    PROC_TYPE_SYNCHRONIZE_WITH_WEB  = 109,           /**< �ϱ��㲥�᳡�� ��ϯ�������ۿ��᳡ ��Ӧ�Ľṹ �� MC_SYNCHRONIZE_WITH_WEB_EX_S  */
    PROC_TYPE_MCU_BACKUP_CHANGE_TO_MASTER  = 110,    /**< MCU����֪ͨ�� ��Ӧ�ṹ ��BACKUP_MCU_REPORT_S  */
    PROC_TYPE_SEND_ROLE_SITE_CHANGE = 111,           /**< ��ǰ���������߱仯֪ͨ�� ��Ӧ�ṹ ��CONF_SEND_ROLE_SITE_CHANGE_S  */
    PROC_TYPE_AUTOMULTIPIC_CHANGE   = 112,           /**< �໭���Զ��л�ֵ�ı�֪ͨ�� ��Ӧ�ṹ ��CONF_AUTOMULTIPIC_CHANGE_S  */
    PROC_TYPE_SET_TURN_BROADCAST_CHANGE = 113,       /**< ���û�����ѯģʽ�ı�֪ͨ�� ��Ӧ�ṹ ��CONF_SET_TURN_BROADCAST_CHANGE_S */
    PROC_TYPE_SET_PIC_MODE_CHANGE   = 114,           /**< ���û���ģʽ�ı�֪ͨ�� ��Ӧ�ṹ ��CONF_SET_PIC_MODE_CHANGE_S */
    PROC_TYPE_MCU_SYNC_STATUS_CHANGE = 115,          /**< MCUͬ��״̬�ı�֪ͨ�� ��Ӧ�ṹ ��MCU_SYNC_STATUS_CHANGE_S */
    PROC_TYPE_CUR_BROADCAST_CHANGE   = 116,          /**< ��ǰʵ�ʹ㲥�᳡�ı�֪ͨ����Ӧ�ṹ��CUR_BROADCAST_INFO_EX_S */
    PROC_TYPE_CUR_CHAIR_BROWSE_CHANGE     = 117,     /**< ��ǰ��ϯ������ʵ�ʹۿ��Ļ᳡�ı�֪ͨ����Ӧ�ṹ��CUR_CHAIR_BROWSE_INFO_EX_S */
    PROC_TYPE_CONF_FECC_CHANGE       = 118,          /**< ��ǰFECC�����߻򱻿��߱仯֪ͨ����Ӧ�ṹ��CONF_FECC_CHANGE_S */
    PROC_TYPE_CONF_MCU_BACKUP_CHANGE      = 119,     /**< ��ǰ������MCU���ݱ仯֪ͨ����Ӧ�ṹ��CONF_MCU_BACKUP_CHANGE_S */
    PROC_TYPE_CALL_SITE_RESULT       = 120,          /**< ���л᳡���֪ͨ����Ӧ�ṹ��CALL_SITE_INFO_EX_S */
    PROC_TYPE_GK_REG_STATE           = 121,          /**< GKע����֪ͨ����Ӧ�ṹ��GK_REG_STATE_INFO_EX_S */
    PROC_TYPE_MG_SESSION_STATUS_CHANGE    = 122,     /**< �ն˻Ự״̬����Ӧ�ṹ��MG_SESSION_STATUS_EX_S */
    PROC_TYPE_MAX,                                   /**< �ص�����������Ϣ�������ֵ */
    PROC_TYPE_INVALID                = 0xFFFFFFFF    /**< ��Чֵ */
}CALL_BACK_PROC_TYPE_E;

/** ��̨����UI���¼�ö�����Ͷ��� */
typedef enum tagPtzNotifyUIEventType
{
    PTZ_NOTIFY_TYPE_RELEASE_TIME_OUT                = 0,    /** ��̨���ӿ��г�ʱ ���ӱ��ͷ� */
    PTZ_NOTIFY_TYPE_REAVED                          = 1,    /** ��̨��ǿռ�������ͷ� */
    PTZ_NOTIFY_TYPE_TROUBLE                         = 2,    /** ��̨���ϣ������ͷ� */
    PTZ_NOTIFY_TYPE_USER_HAS_NO_AUTHORITY           = 3,    /** �û�����̨����Ȩ�� */
    PTZ_NOTIFY_TYPE_PTZ_USER_HAS_LOWER_AUTHORITY    = 4,    /** �û�Ȩ�޵� */
    PTZ_NOTIFY_TYPE_PTZ_HAS_LOCKED_BY_OTHER         = 5,    /** ��̨�������û����� */
    /* Added by wangyaqi,KF1963,2010-12-07 of VVD50927 */
    PTZ_NOTIFY_TYPE_PTZ_CCB_FULL                    = 6,    /** ���ƿ����� */
    PTZ_NOTIFY_TYPE_MAX,
    PTZ_NOTIFY_TYPE_INVALID = 0xFFFFFFFF

}PTZ_NOTIFY_UI_EVENT_TYPE;

/** ����ʾ��Ԫ�����¼�ö�����Ͷ��� */
typedef enum tagSalvoUnitNotifyUIEventType
{
    SALVO_UNIT_NOTIFY_TYPE_START_FAIL       = 0,    /** ����ʾ��Ԫ����ʵ��ʧ�� */
    SALVO_UNIT_NOTIFY_TYPE_BE_REAVED        = 1,    /** ����ʾ��Ԫ��ʵ������ռ */
    SALVO_UNIT_NOTIFY_TYPE_SAME_EXIST       = 2,    /** ��ͬ������ʾ��Ԫ��ʵ���Ѵ��� */
    SALVO_UNIT_NOTIFY_TYPE_SAME_CREATING    = 3,    /** ��ͬ������ʾ��Ԫ��ʵ�����ڽ��� */
    SALVO_UNIT_NOTIFY_TYPE_TROUBLE          = 4,    /** ������������ʾ��Ԫʵ���ͷ� */
    SALVO_UNIT_NOTIFY_TYPE_RESUME_SUCC      = 5,    /** ����ʾ��Ԫ�ָ�ʵ�� */
    SALVO_UNIT_NOTIFY_TYPE_RES_MODIFY       = 6,    /** ����ʾ�޸�����Ԫʵ���ͷ� */
    SALVO_UNIT_NOTIFY_TYPE_USER_STOP        = 7,    /** �û�ֹͣ����ʾ��Ԫ */

    SALVO_UNIT_NOTIFY_TYPE_MAX,
    SALVO_UNIT_NOTIFY_TYPE_INVALID = 0xFFFFFFFF

}SALVO_UNIT_NOTIFY_UI_EVENT_TYPE_E;

/** ����ҵ�������¼�ö�����Ͷ��� */
typedef enum tagSalvoNotifyEventType
{
    NOTIFY_TYPE_SALVO_BE_STOPPED                = 0,    /** ����ʾҵ��ֹͣ */
    NOTIFY_TYPE_SALVO_BE_REAVED                 = 1,    /** ����ʾҵ����ռ */
    NOTIFY_TYPE_GROUPSALVO_BE_STOPPED           = 2,    /** ����Ѳҵ��ֹͣ */
    NOTIFY_TYPE_GROUPSALVO_BE_REAVED            = 3,    /** ����Ѳҵ����ռ */
    NOTIFY_TYPE_GROUPSALVO_STOP_SALVO_PER_TIME  = 4,    /** ����Ѳ�л�ֹͣ����ʾ */
    NOTIFY_TYPE_SALVO_ALL_MON_UNACTIVE          = 5,    /** ����ʾҵ��������ʵ�����ָ� */

    NOTIFY_TYPE_MAX,
    NOTIFY_TYPE_INVALID = 0xFFFFFFFF

}SALVO_NOTIFY_EVENT_TYPE_E;

/*@brief ������Ϣ����ص��������ͣ�ulProcTypeΪ�������ͣ�ö��ֵΪCALL_BACK_PROC_TYPE_E */
typedef VOID (STDCALL *CALL_BACK_PROC_PF)(IN ULONG ulProcType, IN VOID *pParam);

/** �澯���� */
typedef enum tagAlarmType
{
    /** MIB�澯 1~200 */
    AlARM_TYPE_HIGH_TEMPERATURE         = 1,    /**< ���¸澯 */
    AlARM_TYPE_LOW_TEMPERATURE          = 2,    /**< ���¸澯 */
    AlARM_TYPE_TEMPERATURE_RESUME       = 3,    /**< �¶ȸ澯�ָ� */
    AlARM_TYPE_FAN_FAULT                = 4,    /**< ���ȹ��ϸ澯 */
    AlARM_TYPE_FAN_FAULT_RESUME         = 5,    /**< ���ȹ��ϸ澯�ָ� */
    ALARM_TYPE_FLASH_OPERATE            = 6,    /**< Flash�����澯 */
    ALARM_TYPE_CPU_EXCEED               = 7,    /**< CPU�����ʹ��߸澯 */
    ALARM_TYPE_MEMOY_EXCEED             = 8,    /**< �ڴ������ʹ��߸澯 */
    ALARM_TYPE_REBOOT                   = 9,    /**< �豸���� */
    ALARM_TYPE_DEVICE_CONFIG_CHANGE     = 10,   /**< �豸���ñ�� */
    AlARM_TYPE_DISK_ERR                 = 11,   /**< ���̹��� */
    AlARM_TYPE_DISK_ERR_RECOVER         = 12,   /**< ���̹��ϻָ� */
    AlARM_TYPE_DISK_POWER_ON            = 13,   /**< �������� */
    AlARM_TYPE_DISK_POWER_OFF           = 14,   /**< �������� */
    AlARM_TYPE_RAID_EXCEPTION           = 15,   /**< �����쳣�澯 */
    AlARM_TYPE_RAID_REBUILD_START       = 16,   /**< �����ؽ�״̬ */
    AlARM_TYPE_RAID_REBUILD_FINISH      = 17,   /**< �˳��ؽ� */
    AlARM_TYPE_STP_EXEC_EXCEPTION       = 18,   /**< δ���ƻ�¼��澯 */
    AlARM_TYPE_STP_EXEC_RECOVER         = 19,   /**< δ���ƻ�¼��ָ��澯 */
    AlARM_TYPE_IPSAN_NO_ACCESS          = 20,   /**< �޷�����IPSAN�洢�豸�澯 */
    AlARM_TYPE_IPSAN_NO_ACCESS_RECOVER  = 21,   /**< �޷�����IPSAN�洢�豸�ָ��澯 */
    AlARM_TYPE_STORED_DATA_READ_ERR     = 22,   /**< ��ȡ�洢����ʧ�� */
    AlARM_TYPE_STORED_DATA_SEEK_ERR     = 23,   /**< ��λ�洢�豸ʧ�� */
    AlARM_TYPE_VOD_OVER_THRESHOLD       = 24,   /**< �㲥·��������ֵ */
    AlARM_TYPE_VOD_UNDER_THRESHOLD      = 25,   /**< �㲥�������ָ� */
    ALARM_TYPE_TEMPERATURE              = 26,   /**< �¶ȸ澯 */
    AlARM_TYPE_CAM_BAK_OWN_UNDER_THR    = 27,   /**< �������������Դ����ʹ�ôﵽ��ֵ�ָ� */
    AlARM_TYPE_CAM_BAK_SHARD_OVER_THR   = 28,   /**< �������������Դ����ʹ�ôﵽ��ֵ */
    AlARM_TYPE_CAM_BAK_SHARD_UNDER_THR  = 29,   /**< �������������Դ����ʹ�ôﵽ��ֵ�ָ� */
    AlARM_TYPE_CAM_BAK_OWN_CAP_SCANT    = 30,   /**< �������������Դ������ֹͣ����ʱ����Դʣ���������� */
    AlARM_TYPE_CAM_BAK_OWN_CAP_ENOUGH   = 31,   /**< �������������Դ������ֹͣ����ʱ����Դʣ����������ָ� */
    AlARM_TYPE_BAK_RES_CAP_SCANT        = 32,   /**< ȫ�ֱ���������ֹͣ����ʱ��ȫ����Դʣ���������� */
    AlARM_TYPE_BAK_RES_CAP_ENOUGH       = 33,   /**< ȫ�ֱ���������ֹͣ����ʱ��ȫ����Դʣ����������ָ� */
    AlARM_TYPE_BAK_RES_ABNORMAL         = 34,   /**< ������Դ�쳣 */
    AlARM_TYPE_BAK_RES_NORMAL           = 35,   /**< ������Դ�쳣�ָ� */
    AlARM_TYPE_BAK_FAILED               = 36,   /**< ��������ִ��ʧ�� */
    AlARM_TYPE_CAM_BAK_OWN_OVER_THR     = 37,   /**< �������������Դ����ʹ�ôﵽ��ֵ */

    /** SIP�澯 201~ */
    AlARM_TYPE_VIDEO_LOST               = 201,  /**< ��Ƶ��ʧ�澯 */
    AlARM_TYPE_VIDEO_LOST_RESUME        = 202,  /**< ��Ƶ��ʧ�澯�ָ� */
    AlARM_TYPE_MOVE_DETECT              = 203,  /**< �˶����澯 */
    AlARM_TYPE_MOVE_DETECT_RESUME       = 204,  /**< �˶����澯�ָ� */
    AlARM_TYPE_MASK_DETECT              = 205,  /**< �ڵ����澯 */
    AlARM_TYPE_MASK_DETECT_RESUME       = 206,  /**< �ڵ����澯�ָ� */
    AlARM_TYPE_INPUT_SWITCH             = 207,  /**< ���뿪�����澯 */
    AlARM_TYPE_INPUT_SWITCH_RESUME      = 208,  /**< ���뿪�����澯�ָ� */
    AlARM_TYPE_SHORT_CIRCUIT            = 209,  /**< ��������·��·�澯 */
    AlARM_TYPE_BREAKER_CIRCUIT          = 210,  /**< ��������·��·�澯 */
    AlARM_TYPE_SHORT_CIRCUIT_RESUME     = 211,  /**< ��������·��·�澯�ָ� */
    AlARM_TYPE_STOR_FULL_PRE            = 212,  /**< �洢�������澯 */
    AlARM_TYPE_STOR_FULL                = 213,  /**< ���洢�澯 */
    AlARM_TYPE_STOR_FAILED              = 214,  /**< �洢��дʧ�ܸ澯 */
    AlARM_TYPE_STOR_FAILED_RESUME       = 215,  /**< �洢��дʧ�ܸ澯�ָ� */
    AlARM_TYPE_DEVICE_ONLINE            = 216,  /**< �豸���߸澯 */
    AlARM_TYPE_DEVICE_OFFLINE           = 217,  /**< �豸���߸澯 */
    AlARM_TYPE_BREAKER_CIRCUIT_RESUME   = 219,  /**< ��������·��·�澯�ָ� */

    AlARM_TYPE_MAX,                         /**< ���ֵ */

    AlARM_TYPE_ALL      = 0xFFFE,           /**< ���и澯�������� */
    AlARM_TYPE_INVALID  = 0xFFFF            /**< ��Чֵ */
}AlARM_TYPE_E;

/**
* @enum tagDevStatusBit
* @brief �豸״̬λö��
* @attention
*/
typedef enum tagDevStatusBit
{
    DEV_STATUS_BIT_DEVICE_OFFLINE       = 0,    /**< �豸���� */
    DEV_STATUS_BIT_HIGH_TEMPERATURE     = 1,    /**< �豸���� */
    DEV_STATUS_BIT_LOW_TEMPERATURE      = 2,    /**< �豸���� */
    DEV_STATUS_BIT_FAN_FAULT            = 3,    /**< �豸���ȹ��� */
    DEV_STATUS_BIT_VIDEO_LOST           = 4,    /**< ��Ƶ��ʧ */
    DEV_STATUS_BIT_MOVE_DETECT          = 5,    /**< �˶���� */
    DEV_STATUS_BIT_MASK_DETECT          = 6,    /**< �ڵ���� */
    DEV_STATUS_BIT_STOR_FAILED          = 7,    /**< �洢��дʧ�� */
    DEV_STATUS_BIT_INPUT_SWITCH         = 8,    /**< ���뿪���� */
    DEV_STATUS_BIT_SHORT_CIRCUIT        = 9,    /**< ��������·��· */
    DEV_STATUS_BIT_BREAKER_CIRCUIT      = 10,   /**< ��������·��· */

    DEV_STATUS_BIT_MAX_NUM              = 128   /**< ���ֵ */
}DEV_STATUS_BIT_E;

/**
* @enum tagManualStoreStatus
* @brief �洢״̬ö��
* @attention
*/
typedef enum tagManualStoreStatus
{
    MANUAL_STORE_STATUS_STOP  =  0,                   /**< �洢״̬Ϊֹͣ */
    MANUAL_STORE_STATUS_START   = 1,                  /**< �洢״̬Ϊ��ʼ */

    MANUAL_STORE_STATUS_MAX,                          /**< �洢״̬���ֵ */
    MANUAL_STORE_STATUS_INVALID = 0XFFFFFFFF
}MANUAL_STORE_STATUS_E;

/**
* @enum LANG_SETTING
* @brief ��Ʒ������Ӣ������ö��ֵ(�������־����������)
* @attention
*/
typedef enum LANG_SETTING
{
    LANG_CN = 0,                        /* < �������� */
    LANG_EN = 1,                        /* < Ӣ�� */

    LANG_MAX,                           /* < ���ֵ */
    LANG_INVALID                        /* < �������� */
} MW_LANGUAGE_E;

/**
* @enum tagTerminalDevLang
* @brief �ն��豸(EC/DC)����������ö��ֵ
* @attention
*/
typedef enum tagTerminalDevLang
{
    TERMINAL_DEV_LANG_CN = 1,                /* < ��������(�ն�����) */
    TERMINAL_DEV_LANG_EN = 2,                /* < Ӣ��(�ն�����) */

    TERMINAL_DEV_LANG_MAX,                   /* < ���ֵ */
    TERMINAL_DEV_LANG_INVALID                /* < �������� */
} TD_LANGUAGE_E;

/** �������� */
typedef enum tagActionType
{
    ACTION_TYPE_PRESET          = 0,    /**< Ԥ��λ */
    ACTION_TYPE_BOOLEAN_OUTPUT  = 1,    /**< ��������� */
    ACTION_TYPE_STORE           = 2,    /**< �洢 */
    ACTION_TYPE_BUZZER          = 4,    /**< ������ */
    ACTION_TYPE_GIS             = 10,   /**< GIS */
    ACTION_TYPE_MON_LIVE        = 11,   /**< ʵ���������� */
    ACTION_TYPE_PANE_LIVE       = 12,   /**< ʵ�����û����� */
    ACTION_TYPE_BACKUP          = 13,   /**< ����*/

    ACTION_TYPE_MAX,                    /**< ���ֵ */
    ACTION_TYPE_INVALID = 0xFFFFFFFF    /**< ��Чֵ */
}ACTION_TYPE_E;

/** �û�״̬ö�� */
typedef enum tagUserStatus
{
    USER_STATUS_LOCK    = 1,    /**< ���� */
    USER_STATUS_UNLOCK  = 2,    /**< ���� */
    USER_STATUS_ONLINE  = 3,    /**< ���� */
    USER_STATUS_OFFLINE = 4,    /**< ���� */

    USER_STATUS_MAX,                       /**< ���ֵ */
    USER_STATUS_INVALID = 0xFFFFFFFF       /**< ��Чֵ */
}USER_STATUS_E;

/**
* @enum tagASCameraStatus
* @brief �����״̬ö��
* @attention
*/
typedef enum tagASCameraStatus
{
    AS_CAMERA_STATUS_ONLINE         = 1,    /**< ��������� ,��IMOS_DEV_STATUS_ONLINEȡֵһ��*/
    AS_CAMERA_STATUS_OFFLINE        = 2,    /**< ��������� */
    AS_CAMERA_STATUS_VIDEO_LOST     = 3,    /**< �������Ƶ��ʧ */
    AS_CAMERA_STATUS_VIDEO_RESUME   = 4,    /**< �������Ƶ�ָ� */

    AS_CAMERA_STATUS_MAX,
    AS_CAMERA_STATUS_INVALID = 0xFFFFFFFF
}AS_CAMERA_STATUS_E;

/** ��������ö�� */
typedef enum tagVideoAreaType
{
    VIDEO_AREA_TYPE_MASK          = IMOS_VIDEO_AREA_TYPE_MASK,           /**< �ڸ����� */
    VIDEO_AREA_TYPE_COVER_DETECT  = IMOS_VIDEO_AREA_TYPE_COVER_DETECT,   /**< �ڵ����� */
    VIDEO_AREA_TYPE_MOTION_DETECT = IMOS_VIDEO_AREA_TYPE_MOTION_DETECT,  /**< �˶�������� */

    VIDEO_AREA_TYPE_MAX,                   /**< ���ֵ */
    VIDEO_AREA_TYPE_INVALID = 0xFFFFFFFF   /**< ��Чֵ */
}VIDEO_AREA_TYPE_E;

/** �澯���ؼ���ö�� */
typedef enum tagAlarmSeverityLevel
{
    ALARM_SEVERITY_CRITICAL = 0,    /**< Σ�� */
    ALARM_SEVERITY_MAJOR    = 1,    /**< ��Ҫ */
    ALARM_SEVERITY_MINOR    = 2,    /**< ��Ҫ */
    ALARM_SEVERITY_WARN     = 3,    /**< ���� */
    ALARM_SEVERITY_INFO     = 4,    /**< ��ʾ */

    ALARM_SEVERITY_MAX,                     /**< ���ֵ */
    ALARM_SEVERITY_ALL      = 0xFFFFFFFE,   /**< ���и澯����������� */
    ALARM_SEVERITY_INVALID  = 0xFFFFFFFF    /**< ��Чֵ */
}ALARM_SEVERITY_LEVEL_E;

/**
* @enum tagSrvType
* @brief ҵ������,������¼�����ã��û�����ɫ��Ȩ�ޡ��豸����Դ����ʵ���ࡢ�����ࡢ��̨���ơ�Ԥ��λ��Ѳ����·���洢�ࡢ�ط���
* @attention ��
*/
typedef enum tagSrvType
{
    SRV_TYPE_LOGIN          = 0,                        /**< ��¼ */
    SRV_TYPE_CONFIG_USER    = 1,                        /**< �û�����:���û������ɫ */
    SRV_TYPE_CONFIG_ROLE    = 2,                        /**< ��ɫ���� */
    SRV_TYPE_CONFIG_DEV     = 4,                        /**< �豸���� */
    SRV_TYPE_CONFIG_RES     = 5,                        /**< ��Դ����(�����洢��Դ,������Դ��) */

    SRV_TYPE_CONFIG_STORAGE_PLAN    = 6,                /**< �洢�ƻ����� */
    SRV_TYPE_CONFIG_SWITCH_PLAN     = 7,                /**< ���мƻ����� */
    SRV_TYPE_CONFIG_CRUISE_PLAN     = 8,                /**< Ѳ���ƻ����� */

    SRV_TYPE_CONFIG_PTZ     = 9,                        /**< ��̨���� */
    SRV_TYPE_CONFIG_TL      = 10,                       /**< ͸��ͨ�������� */
    SRV_TYPE_VOD            = 11,                       /**< �ط�:��ǩ��ɾ */

    SRV_TYPE_MONITOR            = 12,                   /**< ʵʱ��� */
    SRV_TYPE_SWITCH             = 13,                   /**< ���� */
    SRV_TYPE_PLAN_SWITCH        = 14,                   /**< �ƻ����� */
    SRV_TYPE_VOICE_TALK         = 15,                   /**< �����Խ� */
    SRV_TYPE_VOICE_BROADCAST    = 16,                   /**< �����㲥 */
    SRV_TYPE_PTZ                = 17,                   /**< ��̨���� */
    SRV_TYPE_TL                 = 18,                   /**< ͸��ͨ�� */

    SRV_TYPE_BOOLEAN    = 19,                           /**< ������ */

    SRV_TYPE_MAP        = 20,                           /**< ��ͼ����(�ϴ�,ɾ��,�޸�,����,����ȱʡ��ͼ) */

    SRV_TYPE_STORAGE    = 22,                           /**< �洢 */

    SRV_TYPE_SYS_BACKUP = 23,                           /**< ϵͳ��Ϣ���� */

    SRV_TYPE_SUBSCRIBE  = 24,                           /**< �����ϱ�ҵ�� */

    SRV_TYPE_TRUNK      = 25,                           /**< ���߲���ҵ�� */

    SRV_TYPE_CONFIG_BACKUP_PLAN = 26,                   /**< ���ݼƻ����� */

    SRV_TYPE_BACKUP     = 27,                           /**< ���� */

    SRV_TYPE_SALVO      = 28,                           /**< ����ʾ */
    SRV_TYPE_GROUPSALVO         = 29,                   /**< ����Ѳ */
    SRV_TYPE_GROUPSALVO_PLAN    = 30,                   /**< ����Ѳ�ƻ� */

    /* added by z06806 for vodwall prj 20101202 begin */
    SRV_TYPE_VODWALL            = 31,                   /**< �ط���ǽ */
    /* added by z06806 for vodwall prj 20101202 end */

    SRV_TYPE_CONFIG_TIME = 32,                          /**< ʱ������ */

    SRV_TYPE_REBOOT = 33,                               /**< ���� */

    SRV_TYPE_SHUTDOWN = 34,                             /**< �ػ� */

    SRV_TYPE_RESTORE_DEFAULT = 35,                      /**< �ָ�Ĭ�� */

    SRV_TYPE_MAX,
    SRV_TYPE_INVALID = 0xFFFFFFFF
}SRV_TYPE_E;

/** �������ö�� */
typedef enum tagOperateResult
{
    OPERATE_RESULT_SUCCESS  = 0,    /**< �ɹ� */
    OPERATE_RESULT_FAIL     = 1,    /**< ʧ�� */

    OPERATE_RESULT_MAX,                     /**< ���ֵ */
    OPERATE_RESULT_INVALID = 0xFFFFFFFF     /**< ��Чֵ */
}OPERATE_RESULT_E;

typedef enum tagOperateType
{
    OPERATE_USER_LOGIN  = 0,    /**< �û���½ */
    OPERATE_USER_LOGOUT = 1,    /**< �û��ǳ� */

    OPERATE_USER_START  = 2,    /**< �û�����ҵ�� */
    OPERATE_USER_STOP   = 3,    /**< �û�ֹͣҵ�� */

    OPERATE_CONFIG_ADD  = 4,    /**< �������� */
    OPERATE_CONFIG_MOD  = 5,    /**< �޸����� */
    OPERATE_CONFIG_DEL  = 6,    /**< ɾ������ */

    OPERATE_FILE_DOWNLOAD   = 7,    /**< �ļ����� */

/*****************��������������Ķ�������(��ʼ)*******************/

    OPERATE_DEV_OFFLINE_STOP    = 8,    /**< �豸����ֹͣ */
    OPERATE_DEV_DELETE_STOP     = 9,    /**< �豸ɾ��ֹͣ */
    OPERATE_DEV_ONLINE_STOP     = 10,   /**< �豸�������ҵ�� */

    OPERATE_USER_QUIT_STOP      = 11,   /**< �û��˳�ֹͣ */
    OPERATE_USER_DELETE_STOP    = 12,   /**< �û�ɾ��ֹͣ */
    OPERATE_USER_KEEPOUT_STOP   = 13,   /**< �û�����ʧ�����ҵ�� */

    OPERATE_EXTDOMAIN_OFFLINE_STOP  = 14,   /**< ��������ֹͣ */
    OPERATE_EXTDOMAIN_DELETE_STOP   = 15,   /**< ����ɾ��ֹͣ */
    OPERATE_EXTDOMAIN_ONLINE_STOP   = 16,   /**< �����������ҵ�� */

    OPERATE_SRV_TRANSFER_STOP   = 17,   /**< ҵ��ת��ֹͣ */
    OPERATE_SRV_TRANSFER_START  = 18,   /**< ҵ��ת������ */
    OPERATE_SRV_REAVE_STOP      = 19,   /**< ҵ����ռֹͣ */
    OPERATE_SRV_RESTORE_START   = 20,   /**< ���ϻָ����� */

    OPERATE_SRV_TIMEOUT_STOP    = 21,   /**< ��ʱֹͣ */

    OPERATE_CONFIG_MOD_STOP = 22,   /**< ���ñ仯ֹͣ */

    OPERATE_ALARM_START = 23,       /**< �澯�������� */
    OPERATE_ALARM_STOP  = 24,       /**< �澯����ֹͣ */

    OPERATE_PLAN_START  = 25,       /**< �ƻ����������� */
    OPERATE_PLAN_STOP   = 26,       /**< �ƻ�������ֹͣ */

/*****************��������������Ķ�������(����)*******************/

    OPERATE_ADJUST_FORWARD = 27,    /**< ������һ������ */
    OPERATE_ADJUST_BACKWARD = 28,   /**< ������һ������ */
    OPERATE_ADJUST_PAUSE = 29,      /**< ������ͣ���� */
    OPERATE_ADJUST_CONTINUE = 30,   /**< ���м������� */

    OPERATE_TRUNK_START = 31,       /**< �������߲��� */
    OPERATE_TRUNK_STOP = 32,        /**< ֹͣ���߲��� */
    OPERATE_TRUNK_APPLY = 33,       /**< ������߲��� */
    OPERATE_TRUNK_FREE = 34,        /**< �ͷŸ��߲��� */

    OPERATE_FILE_LOCK     = 35,     /**< �ļ����� */
    OPERATE_FILE_UNLOCK   = 36,     /**< �ļ����� */
    OPERATE_FILE_DELETE   = 37,     /**< �ļ�ɾ�� */

    OPERATE_USER_RESTART  = 38,     /**< �û�����ҵ�� */
    OPERATE_TYPE_MAX,
    OPERATE_TYPE_INVALID = 0xFF
}OPERATE_TYPE_E;

/** �������� */
typedef enum tagExDomainType
{
    EX_DOMAIN_SUPERIOR  = 1,    /**< �ϼ��� */
    EX_DOMAIN_JUNIOR    = 2,    /**< �¼��� */
    EX_DOMAIN_PARITY    = 3,    /**< ͬ���� */

    EX_DOMAIN_MAX,                      /**< ���ֵ */
    EX_DOMAIN_INVALID = 0xFFFFFFFF      /**< ��Чֵ */
}EX_DOMAIN_TYPE_E;

/** �洢ģʽ */
typedef enum tagStorageMode
{
    STORAGE_MODE_IPSAN  = 1,    /**< ����IPSAN�洢ģʽ*/
    STORAGE_MODE_NAS    = 2,    /**< NAS �洢ģʽ*/
    STORAGE_MODE_LOCAL  = 3,    /**< ����Ӳ�̴洢ģʽ*/

    STORAGE_MODE_MAX ,                     /**< ���ֵ*/
    STORAGE_MODE_INVALID = 0xFFFFFFFF,     /**< ��Чֵ*/

}STORAGE_MODE_E;

/**
* @enum tagASDEVShareOperationType
* @brief �豸�����������ö��
* @attention
*/
typedef enum tagASDEVShareOperationType
{
    AS_DEVSHARE_OPERATION_DEL   = 0,    /**< �����豸ȡ�� */
    AS_DEVSHARE_OPERATION_ADD   = 1,    /**< �����豸���� */

    AS_DEVSHARE_OPERATION_MAX,
    AS_DEVSHARE_OPERATION_INVALID = 0xFFFFFFFF
}AS_DEVSHARE_OPERATION_TYPE_E;

/**
* @enum tagIMOSType
* @brief ���Ͷ���
* @attention IMOS������Դ����ö��(��15��1003�Ǹ澯Դ,�澯Դ�豸��Ҫ�ڴ˷�Χ��,�Ǹ澯Դ�豸�����ڴ˷�Χ��)
*            4001-5000Ϊ��Ѷ����
*            201-300Ϊ��Ѷ�豸
*            �豸���͵�ȡֵ��ΧΪ11-2000
*/
typedef enum tagIMOSType
{
    IMOS_TYPE_ORG = 1,                     /**< ��֯�� */
    IMOS_TYPE_OUTER_DOMAIN = 2,            /**< ���� */
    IMOS_TYPE_LOCAL_DOMAIN = 3,            /**< ���� */

    IMOS_TYPE_DM = 11,                     /**< DM */
    IMOS_TYPE_MS = 12,                     /**< MS */
    IMOS_TYPE_VX500 = 13,                  /**< VX500 */
    IMOS_TYPE_MONITOR = 14,                /**< ������ */

    IMOS_TYPE_EC = 15,                     /**< EC */
    IMOS_TYPE_DC = 16,                     /**< DC */

    IMOS_TYPE_GENERAL = 17,                /**< ͨ���豸 */
    IMOS_TYPE_ECR = 18,                    /**< ECR */

    IMOS_TYPE_MCU = 201,                   /**< MCU */
    IMOS_TYPE_MG = 202,                    /**< MG */

    IMOS_TYPE_CAMERA = 1001,               /**< ����� */
    IMOS_TYPE_ALARM_SOURCE = 1003,         /**< �澯Դ */

    IMOS_TYPE_STORAGE_DEV = 1004,          /**< �洢�豸 */
    IMOS_TYPE_TRANS_CHANNEL = 1005,        /**< ͸��ͨ�� */

    IMOS_TYPE_ALARM_OUTPUT = 1200,         /**< �澯��� */

    IMOS_TYPE_BM = 1300,                   /**< BM */

    IMOS_TYPE_GUARD_TOUR_RESOURCE = 2001,  /**< ������Դ */
    IMOS_TYPE_GUARD_TOUR_PLAN = 2002,      /**< ���мƻ� */
    IMOS_TYPE_MAP = 2003,                  /**< ��ͼ */

    IMOS_TYPE_XP = 2005,                   /**< XP */
    IMOS_TYPE_XP_WIN = 2006,               /**< XP���� */
    IMOS_TYPE_GUARD_PLAN = 2007,           /**< �����ƻ� */

    IMOS_TYPE_DEV_ALL = 2008,              /**< ���е��豸����(EC/DC/MS/DM/VX500/����ͷ/������) */

    IMOS_TYPE_TV_WALL = 3001,              /**< ����ǽ */

    IMOS_TYPE_CAMERA_GROUP = 3002,                   /**< ������� */
    IMOS_TYPE_MONITOR_GROUP = 3003,                  /**< �������� */
    IMOS_TYPE_SALVO_RESOURCE = 3004,                 /**< ����ʾ��Դ */

    IMOS_TYPE_GROUP_SWITCH_RESOURCE = 3010,          /**< ����Ѳ��Դ */
    IMOS_TYPE_GROUP_SWITCH_PLAN = 3011,              /**< ����Ѳ�ƻ���Դ */


    IMOS_TYPE_TRUNK = 3050,                /**< ���� */

    IMOS_TYPE_CONFERENCE = 4001,           /**< ������Դ */

    IMOS_TYPE_MAX,
    IMOS_TYPE_INVALID = 0xFFFFFFFF
}IMOS_TYPE_E;

/**
 * @enum tagAlarmSrcType
 * @brief �澯�豸����
 * @attention
 */
typedef enum tagAlarmDevType
{
    ALARM_DEV_TYPE_EC  = 0,            /**< �澯�豸ΪEC */
    ALARM_DEV_TYPE_DC  = 1,            /**< �澯�豸ΪDC */
    ALARM_DEV_TYPE_IPSAN  = 2,            /**< �澯�豸Ϊ�洢�豸 */
    ALARM_DEV_TYPE_CAM  = 3,            /**< �澯�豸Ϊ����� */
    ALARM_DEV_TYPE_BOOLEAN  = 4,            /**< �澯�豸Ϊ������ */
    ALARM_DEV_TYPE_SERIAL   = 5,            /**< �澯�豸Ϊ���� */
    ALARM_DEV_TYPE_GENERAL  = 6,            /**< �澯�豸Ϊͨ���豸 */

    ALARM_DEV_TYPE_MAX,                     /**< �澯�豸ö�����ֵ */
    ALARM_DEV_TYPE_INVALID = 0xFFFFFFFF     /**< ��Чֵ */

}ALARM_DEV_TYPE_E;

/**
* @enum tagNameCodeFlag
* @brief ���Ʊ��:0 ������Ч 1 ����ѯ�豸���� 2 ����ѯ��Դ����
* @attention
*/
typedef enum tagNameCodeFlag
{
    NAME_FLAG_NULL    = 0,                /**< ������Ч */
    NAME_FLAG_DEV     = 1,                /**< ����ѯ�豸���� */
    NAME_FLAG_RES     = 2,                /**< ����ѯ��Դ���� */
    NAME_FLAG_RESCODE = 3,                /**< ����ѯ��Դ���� */

    NAME_FLAG_MAX,                      /**< ���Ʊ��ö�����ֵ */
    NAME_FLAG_INVALID   = 0xFFFFFFFF    /**< ��Чֵ */

}NAME_CODE_FLAG_E;

/**
* @enum tagSearchMode
* @brief ��Ѱģʽ���:0 ������Ч 1 ��ȷƥ�� 2 ģ��ƥ��
* @attention
*/
typedef enum tagSearchMode
{
    SEARCH_MODE_NULL    = 0,                /**< ��Ч */
    SEARCH_MODE_EQU     = 1,                /**< ��ȷƥ��*/
    SEARCH_MODE_LIKE    = 2,                /**< ģ��ƥ�� */

    SEARCH_MODE_MAX,                        /**< ���ֵ */
    SEARCH_MODE_INVALID   = 0xFFFFFFFF      /**< ��Чֵ */

}SEARCH_MODE_E;

/**
* @enum tagTimeFlag
* @brief ʱ����:0 ʱ����Ч 1 ��ʱ����ѯ 2 ��ʱ��β�ѯ
* @attention
*/
typedef enum tagTimeFlag
{
    TIME_FLAG_NULL  = 0,                /**< ʱ����Ч */
    TIME_FLAG_ONE   = 1,                /**< ��ʱ����ѯ */
    TIME_FLAG_TWO   = 2,                /**< ��ʱ��β�ѯ */

    TIME_FLAG_MAX,                      /**< ʱ����ö�����ֵ */
    TIME_FLAG_INVALID   = 0xFFFFFFFF    /**< ��Чֵ */
}TIME_FLAG_E;

/**
* @enum tagStoreAssignType
* @brief �洢�����������
* @attention
*/
typedef enum tagStoreAssignType
{
    ASSIGN_TYPE_SIZE         = 0,       /**< ֱ�ӷ���洢���������д洢 */
    ASSIGN_TYPE_DAY          = 1,       /**< ����洢��ͨ���������������Ҫ��������� */

    ASSIGN_TYPE_MAX,                    /**< �洢�������ö�����ֵ */
    ASSIGN_TYPE_INVALID = 0xFFFFFFFF    /**< ��Чֵ */

}STORE_ASSIGN_TYPE_E;

/**
* @enum tagStoreDevType
* @brief �洢�豸����ö��
* @attention
*/
typedef enum tagStoreDevType
{
    STORE_DEV_TYPE_IPSAN    = 0,            /**< IPSAN�豸 */
    STORE_DEV_TYPE_VX500    = 1,            /**< VX500���� */
    STORE_DEV_TYPE_ECR      = 2,            /**< ECR�豸 */

    STORE_DEV_TYPE_MAX,                     /**< �洢�豸����ö�����ֵ */
    STORE_DEV_TYPE_INVALID  = 0xFFFFFFFF    /**< ��Чֵ */
}STORE_DEV_TYPE_E;

/** ͨ������ */
typedef enum tagIMOSChannelType
{
    IMOS_CHANNEL_TYPE_BOOLEAN   = 0,    /**< ������ */
    IMOS_CHANNEL_TYPE_SERIAL    = 1,    /**< ���� */

    IMOS_CHANNEL_TYPE_MAX,
    IMOS_CHANNEL_TYPE_INVALID = 0xFFFFFFFF
}IMOS_CHANNEL_TYPE_E;

/** �����㲥״̬*/
typedef enum tagVocBrdStatus
{
    VOCBRD_STATUS_ON    = 0,    /**< ���ڹ㲥*/
    VOCBRD_STATUS_OFF   = 1,    /**< û�й㲥*/

    VOCBRD_STATUS_MAX,
    VOCBRD_STATUS_INVALID = 0xFFFFFFFF
}VOCBRD_STATUS_E;

/** ͸��ͨ��ʹ��״̬ö�� */
typedef enum tagTlTransChannelStatus
{
    TL_TRANS_CHANNEL_DISABLED   = 0,    /**< ��ʹ�� */
    TL_TRANS_CHANNEL_ENABLED    = 1,    /**< ʹ�� */

    TL_TRANS_CHANNEL_STATUS_MAX,                     /**< ���ֵ */
    TL_TRANS_CHANNEL_STATUS_INVALID = 0xFFFFFFFF     /**< ��Чֵ */
}TL_TRANS_CHANNEL_STATUS_E;

/** ͸��ͨ����ǰ����״̬ö�� */
typedef enum tagTlTransChannelWorkStatus
{
    TL_TRANS_CHANNEL_CLOSED = 0,                        /**< �ر� */
    TL_TRANS_CHANNEL_OPENED = 1,                        /**< �� */

    TL_TRANS_CHANNEL_WORK_STATUS_MAX,                   /**< ���ֵ */
    TL_TRANS_CHANNEL_WORK_STATUS_INVALID = 0xFFFFFFFF   /**< ��Чֵ */
}TL_TRANS_CHANNEL_WORK_STATUS_E;

/**
* @enum tagManufacturerType
* @brief ��������ö��
* @attention
*/
typedef enum tagManufacturerType
{
    MANUFACTURER_TYPE_H3C      = 0,             /**< H3C_8500 */
    MANUFACTURER_TYPE_H3C_3000E,                /**< H3C_3000E */
    MANUFACTURER_TYPE_DB33,                     /**< ��H3C��������DB33��׼����ĳ��� */
    MANUFACTURER_TYPE_OTHER,                    /**< ���϶������ڵĳ��� */

    MANUFACTURER_TYPE_MAX,
    MANUFACTURER_TYPE_INVALID = 0xFFFFFFFF
}MANUFACTURER_TYPE_E;

/**
* @enum tagCameraType
* @brief ���������ö��
* @attention
*/
typedef enum tagCameraType
{
    CAMERA_TYPE_FIX         = 1,            /**< �̶������ */
    CAMERA_TYPE_PTZ         = 2,            /**< ��̨����� */

    CAMERA_TYPE_MAX,                        /**< ���������ö�����ֵ */
    CAMERA_TYPE_INVALID     = 0xFFFFFFFF    /**< ��Чֵ */
}CAMERA_TYPE_E;

/**
* @enum tagLicenseType
* @brief License�������
* @attention ��
*/
typedef enum tagLicenseType
{
    LICENSE_TYPE_BASE_FUNCTION_FOR_LINUX = 0x00,    /**< ��ع������������ȨFor Linux */

    LICENSE_TYPE_MCU_LO_DOMAIN = 0x0A,              /**< ����MCU������� */
    LICENSE_TYPE_CAMERA_LO_DOMAIN = 0x10,           /**< ����CAMERA������� */
    LICENSE_TYPE_CAMERA_EX_H3C_DOMAIN = 0x11,       /**< H3C����CAMERA������� */
    LICENSE_TYPE_CAMERA_EX_NOT_H3C_DOMAIN = 0x12,   /**< ��H3C����CAMERA������� */

    LICENSE_TYPE_STORAGE = 0x30,                    /**< �洢������� */

    LICENSE_TYPE_DM = 0x35,                         /**< DM������� */
    LICENSE_TYPE_MS = 0x36,                         /**< MS������� */

    /* �����Ȩ */
    LICENSE_TYPE_CONFERENCE_BASE = 0x97,            /**< ��Ѷ�����Ȩ */

    LICENSE_TYPE_MAX,                               /**< ����������ֵ */
    LICENSE_TYPE_INVALID        = 0xFF
}LICENSE_TYPE_E;

/* Begin: Added by MultiDomain */
/**
* @enum tagInterDomainProtocolType
* @brief ���ͨѶЭ������ö��
* @attention
*/
typedef enum tagInterDomainProtocolType
{
    INTER_DOMAIN_PROTOCOL_DB33      = 0,    /**< DB33 */
    INTER_DOMAIN_PROTOCOL_GLOBALEYE = 1,    /**< ȫ���� */
    INTER_DOMAIN_PROTOCOL_WIDEEYE   = 2,    /**< ���ӽ� */
    INTER_DOMAIN_PROTOCOL_ELECTRIC  = 3,    /**< �Ĵ����� */
    INTER_DOMAIN_PROTOCOL_IMOS      = 4,    /**< IMOS */

    INTER_DOMAIN_PROTOCOL_MAX,                    /**< ö�����ֵ */
    INTER_DOMAIN_PROTOCOL_INVALID = 0xFFFFFFFF    /**< ��Чֵ */
}INTERDOMAIN_PROTOCOL_TYPE_E;
/* End: Added by MultiDomain */


/**
* @enum tagSwitchCircuitAlarmEnabledType
* @brief ��������·���澯����
* @attention ��
*/
typedef enum tagSwitchCircuitAlarmEnabledType
{
    SWITCH_CIRCUIT_ALARM_ENABLED        = 1,    /**< ֧�ֿ�������·���澯(��֧�ֿ�������·��·�澯Ҳ֧�ֿ�������·��·�澯) */
    SWITCH_CIRCUIT_ALARM_DISABLED       = 2,    /**< ��֧�ֿ�������·���澯(�Ȳ�֧�ֿ�������·��·�澯Ҳ��֧�ֿ�������·��·�澯) */
    SWITCH_CIRCUIT_SHORT_ENABLED_ONLY   = 3,    /**< ֧�ֿ�������·��·�澯����֧�ֿ�������·��·�澯 */
    SWITCH_CIRCUIT_BREAK_ENABLED_ONLY   = 4,    /**< ֧�ֿ�������·��·�澯����֧�ֿ�������·��·�澯 */

    SWITCH_CIRCUIT_ALARM_MAX,
    SWITCH_CIRCUIT_ALARM_INVALID = 0xFF
}SWITCH_CIRCUIT_ALARM_ENABLED_TYPE_E;

/**
* @enum tagBackupStatus
* @brief ����״̬ö��
* @attention
*/
typedef enum tagBackupStatus
{
    BACKUP_STATUS_START         = 0,    /**< ��ʼ�������� */
    BACKUP_STATUS_PROCESSING    = 1,    /**< ���ݽ����� */
    BACKUP_STATUS_IDLE          = 2,    /**< �Ǳ���״̬ */

    BACKUP_STATUS_MAX,                    /**< ����״̬ö�����ֵ */
    BACKUP_STATUS_INVALID    = 0xFFFFFFFF /**< ��Чֵ */
}BACKUP_STATUS_E;

/**
 * @enum tagLogicFlag
 * @brief  ���Ӳ�ѯ����Լ���е��߼���ϵ����
 * @attention ��
 */
typedef enum tagLogicFlag
{
    EQUAL_FLAG      = 0,    /**< ���� */
    GREAT_FLAG      = 1,    /**< ���� */
    LITTLE_FLAG     = 2,    /**< С�� */
    GEQUAL_FLAG     = 3,    /**< ���ڵ��� */
    LEQUAL_FLAG     = 4,    /**< С�ڵ��� */
    LIKE_FLAG       = 5,    /**< ģ����ѯ */
    ASCEND_FLAG     = 6,    /**< ���� */
    DESCEND_FLAG    = 7,    /**< ���� */
    LOGIC_FLAG_MAX          /**< ��Чֵ */
}LOGIC_FLAG_E;


/**
 * @enum tagQueryType
 * @brief  ��ѯ������
 * @attention 300~500 ��Ѷʹ��
 */
typedef enum tagQueryType
{
    CODE_TYPE       = 0,    /**< ��������(���û�����֮��) */
    NAME_TYPE       = 1,    /**< ��������(���û�����֮��) */
    USER_CODE_TYPE  = 2,    /**< �û��������� */
    USER_NAME_TYPE  = 3,    /**< �û��������� */
    TIME_TYPE       = 4,    /**< ʱ������ */

    EVENT_TYPE      = 5,    /**< �澯�¼�����,ȡֵΪ#AlARM_TYPE_E */
    EVENT_SECURITY  = 6,    /**< �澯�¼�����,ȡֵΪ#ALARM_SEVERITY_LEVEL_E */
    EVENT_COMFIRMED = 7,    /**< �澯�Ƿ���ȷ�� */
    EVENT_TIME      = 8,    /**< �澯ʱ�� */

    DEV_SUB_TYPE    = 9,    /**< �豸������ */

    INDEX_TYPE      = 10,   /**< ��������(����Ƶ����ͨ������,��Ƶ���ͨ������,��������,����������) */

    RES_SUB_TYPE    = 11,   /**< ��Դ������ */

    /* Begin: Add by x06948 for VVD38087, 20100115 */
    SRV_TYPE            = 12,       /**< ҵ������ */
    MONITOR_TYPE        = 13,       /**< ���������� */
    MONITOR_NAME        = 14,       /**< ���������� */
    MONITOR_DOMAIN      = 15,       /**< ������������ */
    CAMERA_NAME         = 16,       /**< ��������� */
    USER_LOGIN_IP       = 17,       /**< �û���¼IP */
    MON_SRV_STATUS      = 18,       /**< ʵ��״̬ */
    VOCBRD_SRV_STATUS   = 19,       /**< �㲥״̬ */
    /* End: Add by x06948 for VVD38087, 20100115 */
    //RES_TYPE            = 20,       /**< ��Դ���� */

    CASE_DESC               = 20,       /**< �������� */
    TASK_STATUS             = 21,       /**< ����״̬ */
    TASK_SUB_TIME           = 22,       /**< �����ύʱ�� */
    TASK_END_TIME           = 23,       /**< �������ʱ�� */
    BAK_START_TIME          = 24,       /**< ���ݿ�ʼʱ�� */
    BAK_END_TIME            = 25,       /**< ���ݽ���ʱ�� */
    FILE_CREATE_TIME        = 26,       /**< �ļ�����ʱ�� */
    FILE_CAPACITY           = 27,       /**< �ļ����� */
    FILE_TYPE               = 28,       /**< �ļ����� */
    FILE_LOCK_FLAG          = 29,       /**< �ļ�������ʶ */
    LAYOUT_TYPE             = 30,       /**< �������ͣ�ȡֵΪ#LAYOUT_TYPE_E */

    DOMAIN_TYPE = 100,       /**< ������,��ֻ֧�ֱ����� */

    EXT_DOMAIN_TPYE         = 200,      /**< ��������:��/��/ƽ���� */
    EXT_DOMAIN_IP           = 201,      /**< ����IP */
    EXT_DOMAIN_PORT         = 202,      /**< ����PORT */
    EXT_DOMAIN_TRUNK_NUM    = 203,      /**< ����������� */
    EXT_DOMAIN_MULTICAST    = 204,      /**< ����鲥���� */
    EXT_DOMAIN_SESSION      = 205,      /**< ����������־ */

    JOB_STATUS          = 210,          /**< ��������״̬ */
    JOB_CREATOR         = 211,          /**< �������񴴽��� */
    JOB_EXEC_TIME       = 212,          /**< �����������ʱ�� */
    JOB_ESTB_TIME       = 213,          /**< ���������ƶ�ʱ�� */
    JOB_END_TIME        = 214,          /**< �����������ʱ�� */
    JOB_RESULT          = 215,          /**< ������� */

    OPER_TIME               = 220,      /**< ����ʱ�� */
    OPER_IP                 = 221,      /**< ������IP��ַ */
    OPER_TYPE               = 222,      /**< �������� */
    OPER_RESULT             = 223,      /**< ������� */
    OPER_SERVICE_TYPE       = 224,      /**< ������ҵ������ */
    OPER_OBJ                = 225,      /**< �������� */

    LABEL_TIME          = 230,          /**< ��ǩʱ��� */
    REC_START_TIME      = 231,          /**< ��ǩ¼��ʼʱ�䣬��ʽΪ"hh:mm:ss" */
    REC_END_TIME        = 232,          /**< ��ǩ¼�����ʱ�䣬��ʽΪ"hh:mm:ss" */

    USER_FULL_NAME          = 240,      /**< �û�ȫ�� */
    USER_TEL_PHONE          = 241,      /**< �û������绰 */
    USER_MOBILE_PHONE       = 242,      /**< �û��ƶ��绰 */
    USER_EMAIL              = 243,      /**< �û������ʼ� */
    USER_IS_LOCKED          = 244,      /**< �û��Ƿ����� */

    ROLE_PRIORITY       =250,           /**< ��ɫ���ȼ� */

    DEV_TYPE            = 255,          /**< �豸���� */
    RES_TYPE            = 256,          /**< ��Դ���� */
    IS_QUERY_SUB        = 257,          /**< �Ƿ���¼�(��Ҫ��AS_CON_Parse���н���) */
    RES_ID              = 258,          /**< ��ԴID */

    CONF_START_TIME         = 300,      /**< ���鿪ʼʱ�䣬1970������������ */
    CONF_END_TIME           = 301,      /**< �������ʱ�䣬1970������������ */

    QUERY_TYPE_MAX          /**< ��Чֵ */
}QUERY_TYPE_E;

/**
 * @enum tagIMOSFavoriteStream
 * @brief  �����ò�������
 * @attention ��
 */
typedef enum tagIMOSFavoriteStream
{
    IMOS_FAVORITE_STREAM_ANY        = 0,        /**< ��ָ�� */
    IMOS_FAVORITE_STREAM_PRIMARY    = 1,        /**< ָ������ */
    IMOS_FAVORITE_STREAM_SECONDERY  = 2,        /**< ָ������ */
    IMOS_FAVORITE_STREAM_THIRD      = 3,        /**< ָ������ */
    IMOS_FAVORITE_STREAM_FOURTH     = 4,        /**< ָ������ */
    IMOS_FAVORITE_STREAM_FIFTH      = 5,        /**< ָ������ */
    IMOS_FAVORITE_STREAM_BI_AUDIO   = 6,        /**< ָ�������Խ� */
    IMOS_FAVORITE_STREAM_VOICE_BR   = 7,        /**< ָ�������㲥 */
    IMOS_FAVORITE_STREAM_BUTT
}IMOS_FAVORITE_STREAM_E;


/**
 * @enum  tagSubscribePushType
 * @brief �������͵�����
 * @attention ��
 */
typedef enum tagSubscribePushType
{
    SUBSCRIBE_PUSH_TYPE_ALL          = 0,   /**< ���ܸ澯���ͺ��豸״̬���� */
    SUBSCRIBE_PUSH_TYPE_ALARM        = 1,   /**< ֻ���ո澯���� */
    SUBSCRIBE_PUSH_TYPE_ALARM_STATUS = 2,   /**< ֻ�����豸״̬���� */

    SUBSCRIBE_PUSH_TYPE_MAX,
    SUBSCRIBE_PUSH_TYPE_INVALID = 0xFFFFFFFF
}SUBSCRIBE_PUSH_TYPE_E;

/**
 * @enum  tagCSOperateCode
 * @brief ���з������ԭ����
 * @attention ��
 */
typedef enum tagCSOperateCode
{
    USER_OPERATE_SERVICE = 0,                               /**< �û������������ */

/*****************��������������Ķ�������(��ʼ)*******************/

    SWITCH_OPERATE              = 1,    /**< ���в��� */
    PLAN_SWITCH_OPERATE         = 2,    /**< �ƻ����в��� */
    SERVICE_REAVE               = 3,    /**< ҵ����ռ */
    ALARM_LINKAGE_START_SERVICE = 4,    /**< �澯���� */
    EXT_DOMAIN_OPER_SERVICE     = 5,    /**< ��������� */

    EC_ONLINE                   = 6,    /**< EC���� */
    EC_OFFLINE                  = 7,    /**< EC���� */
    EC_DELETE                   = 8,    /**< ECɾ�� */
    CAMERA_ONLINE               = 9,    /**< ��������� */
    CAMERA_OFFLINE              = 10,   /**< ��������� */

    DC_ONLINE                   = 11,   /**< DC���� */
    DC_OFFLINE                  = 12,   /**< DC���� */
    DC_DELETE                   = 13,   /**< DCɾ�� */

    MS_ONLINE                   = 14,   /**< MS���� */
    MS_OFFLINE                  = 15,   /**< MS���� */
    MS_OFFLINE_TRANSFER         = 16,   /**< MS���������ת�� */
    MS_DELETE_TRANSFER          = 17,   /**< MSɾ�������ת�� */
    MS_DELETE                   = 18,   /**< MSɾ�� */

    VX500_ONLINE                = 19,   /**< VX500���� */
    VX500_OFFLINE               = 20,   /**< VX500���� */
    VX500_DELETE                = 21,   /**< VX500ɾ�� */

    ISC_ONLINE                  = 22,   /**< ISC3000-E���� */
    ISC_OFFLINE                 = 23,   /**< ISC3000-E���� */

    EX_DOMAIN_JUNIOR_ONLINE     = 24,   /**< �¼������� */
    EX_DOMAIN_SUPERIOR_ONLINE   = 25,   /**< �ϼ������� */
    EX_DOMAIN_JUNIOR_OFFLINE    = 26,   /**< �¼������� */
    EX_DOMAIN_SUPERIOR_OFFLINE  = 27,   /**< �ϼ������� */
    EXT_DOMAIN_DELETE           = 28,   /**< ����ɾ�� */

    EXT_DOM_CANCEL_SHR_CAM      = 29,   /**< ����ȡ����������������� */
    LOC_DOM_CANCEL_SHR_CAM      = 30,   /**< ����ȡ����������������� */

    CAMERA_DELETE               = 31,   /**< CAMERAɾ�� */
    MONITOR_DELETE              = 32,   /**< MONITORɾ�� */

    SWITCH_RES_MDF              = 33,   /**< ������Դ�޸� */
    SWITCH_RES_DEL              = 34,   /**< ������Դɾ�� */
    SWITCH_PLAN_DEL             = 35,   /**< ���мƻ�ɾ�� */
    SWITCH_PLAN_MODIFY          = 36,   /**< ���мƻ��޸� */

    XP_ONLINE                   = 37,   /**< XP���� */
    XP_OFFLINE                  = 38,   /**< XP���� */
    USER_KEEPALIVE_FAIL         = 39,   /**< �û�����ʧ�� */
    USER_DELETE                 = 40,   /**< �û���ɾ�� */
    USER_QUIT                   = 41,   /**< �û��˳� */
    USER_KICKED_OUT             = 42,   /**< �û���ǿ������ */

    SYSMANGER_OPERATE_SERVICE   = 43,   /**< ����Ա���� */
    PTZ_LINK_TIMER_OUT          = 44,   /**< ��̨���ӳ�ʱ */
    PTZ_FIRST_LINK              = 45,   /**< ��̨�״����� */
    PTZ_LOW_AUTHORITY           = 46,   /**< ��̨����Ȩ�޵� */
    PTZ_NO_AUTHORITY            = 47,   /**< ����̨����Ȩ�� */
    PTZ_HAS_LOCKED              = 48,   /**< ��̨������ */

    DEV_MEDIA_PARAM_CHANGE      = 49,   /**< �豸ý������޸� */

    SALVO_OPERATE               = 50,   /**< ����ʾ���� */

    SALVO_RES_DEL               = 51,   /**< ����ʾɾ�� */
    SALVO_RES_MDF               = 52,   /**< ����ʾ�޸� */

    GROUPSWITCH_RES_DEL         = 53,   /**< ����Ѳ��Դɾ�� */
    GROUPSWITCH_RES_MDF         = 54,   /**< ����Ѳ��Դ�޸� */

    GROUPSWITCH_PLAN_DEL        = 55,   /**< ����Ѳ�ƻ�ɾ�� */
    GROUPSWITCH_PLAN_MODIFY     = 56,   /**< ����Ѳ�ƻ��޸� */

    GROUPSALVO_OPERATE          = 57,   /**< ����Ѳ���� */
    GROUPSALVO_STOP             = 58,   /**< ����Ѳֹͣ */
    GROUPSALVO_PLAN_OPERATE     = 59,   /**< ����Ѳ�ƻ����� */

    INTERNAL_ERR_OPERATE        = 60,   /**< �ڲ������������ */

    /* Added by wangyaqi,KF1963,2010-12-07 of VVD50927 */
    PTZ_CCB_FULL                = 61,    /**< ���ƿ����� */

/*****************��������������Ķ�������(����)*******************/

    CS_OPERATE_CODE_MAX,
    CS_OPERATE_CODE_INVALID = 0xFF
}CS_OPERATE_CODE_E;

/**
 * @enum  tagCSSwitchAdjustAction
 * @brief ���з����ֶ���������
 * @attention ��
 */
typedef enum tagCSSwitchAdjustAction
{
    SWTICH_PAUSE    = 0,    /**< ������ͣ */
    SWITCH_CONTINUE = 1,    /**< ���лָ� */
    SWITCH_FORWARD  = 2,    /**< ������ǰ���� */
    SWITCH_BACKWARD = 3,    /**< ���������� */

    SWITCH_ADJUST_ACTION_MAX,                   /**< ���ֵ */
    SWITCH_ADJUST_ACTION_INVALID = 0xFF         /**< ��Чֵ */
}CS_SWITCH_ADJUST_ACTION_E;



/**
 * @enum tagTCProtocolType
 * @brief  ͸��ͨ�����ݴ���Э������ö��
 * @attention 1.ֻ֧��UDPЭ������,�ݲ�֧��TCP��SIPЭ������
 */
typedef enum tagTCProtocolType
{
    UDP_PROTOCOL    = 1,    /**< Э������ΪUDP */

    TCP_PROTOCOL    = 2,    /**< Э������ΪTCP(�ݲ�֧��) */
    SIP_PROTOCOL    = 3,    /**< Э������ΪSIP(�ݲ�֧��) */

    PROTOCOL_TYPE_INVALID = 0xFFFFFFFF /**< ��Чֵ */
}TC_PROTOCOL_TYPE_E;

/**
* @enum tagGisMapParamType
* @brief ��ͼ��������ö��
* @attention
*/
typedef enum tagGisMapParamType
{
    GIS_MAP_PAPAM_HOTZONE   = 0,    /**< ���� */
    GIS_MAP_PAPAM_HOTPOT   = 1,     /**< �ȵ� */
    GIS_MAP_PAPAM_CAMICON = 2,      /**< �����ͼ�� */
    GIS_MAP_PAPAM_ALMSRCICON = 3,   /**< �澯Դͼ�� */

    GIS_MAP_PAPAM_MAX,
    GIS_MAP_PAPAM_INVALID = 0xFF
}GIS_MAP_PAPAM_TYPE_E;


/*******************************************************************************
SS��������
*******************************************************************************/

/**
 * @enum tagDeviceOnlineStatus
 * @brief �豸����״̬
 * @attention
 */
typedef enum tagDeviceOnlineStatus
{
    IMOS_EN_DEVICE_STATUS_INVALID   = 0,    /**< �豸��Ч״̬*/
    IMOS_EN_DEVICE_STATUS_ONLINE    = 1,    /**< �豸����, ��IMOS_DEV_STATUS_ONLINEȡֵһ��*/
    IMOS_EN_DEVICE_STATUS_OFFLINE   = 2,    /**< �豸����, ��IMOS_DEV_STATUS_OFFLINEȡֵһ��*/

    IMOS_EN_DEVICE_STATUS_MAX               /**< �豸״̬, ���ֵ*/
}DEV_ONLINE_STATUS_E;

/**
 * @enum tagDeviceOperateType
 * @brief �豸��������
 * @attention
 */
typedef enum tagDeviceOperateType
{
    OPERATE_DEVICE_ADD  = 0,    /**< �豸���� */
    OPERATE_DEVICE_MOD  = 1,    /**< �豸�޸� */
    OPERATE_DEVICE_DEL  = 2     /**< �豸ɾ�� */
}DEVICE_OPERATE_TYPE_E;

/**
* @enum tagHD_NaaTypeEx
* @brief NAA����
* @attention ��
*/
typedef  enum  tagHD_NaaTypeEx
{
    IMOS_EN_NAA_TYPE_AUTO = 0,                                     /**< �Զ� */
    IMOS_EN_NAA_TYPE_100MS = 100,                                  /**< 100ms */
    IMOS_EN_NAA_TYPE_200MS = 200,                                  /**< 200ms */
    IMOS_EN_NAA_TYPE_300MS = 300,                                  /**< 300ms */
    IMOS_EN_NAA_TYPE_400MS = 400,                                  /**< 400ms */
    IMOS_EN_NAA_TYPE_500MS = 500,                                  /**< 500ms */
    IMOS_EN_NAA_TYPE_600MS = 600,                                  /**< 600ms */
    IMOS_EN_NAA_TYPE_700MS = 700,                                  /**< 700ms */
    IMOS_EN_NAA_TYPE_800MS = 800,                                  /**< 800ms */
    IMOS_EN_NAA_TYPE_900MS = 900,                                  /**< 900ms */
    IMOS_EN_NAA_TYPE_1000MS = 1000,                                /**< 1000ms */
    IMOS_EN_NAA_TYPE_1100MS = 1100,                                /**< 1100ms */
    IMOS_EN_NAA_TYPE_1200MS = 1200,                                /**< 1200ms */
    IMOS_EN_NAA_TYPE_1300MS = 1300,                                /**< 1300ms */
    IMOS_EN_NAA_TYPE_1400MS = 1400,                                /**< 1400ms */
    IMOS_EN_NAA_TYPE_1500MS = 1500,                                /**< 1500ms */
    IMOS_EN_NAA_TYPE_1600MS = 1600,                                /**< 1600ms */
    IMOS_EN_NAA_TYPE_1700MS = 1700,                                /**< 1700ms */
    IMOS_EN_NAA_TYPE_1800MS = 1800,                                /**< 1800ms */
    IMOS_EN_NAA_TYPE_1900MS = 1900,                                /**< 1900ms */
    IMOS_EN_NAA_TYPE_2000MS = 2000,                                /**< 2000ms */
    IMOS_EN_NAA_TYPE_COUNT
} HD_NAA_TYPE_EX_E;

/**
 * @enum tagMC_MonitorEx
 * @brief ���ͼ����������
 * @attention
 */
typedef  enum tagMC_MonitorEx
{
    IMOS_EN_MONITOR1_VIDEO  = 0,    /**< ��ص����� */
    IMOS_EN_MONITOR4_VIDEO  = 1,    /**< ���4���� */
    IMOS_EN_MONITOR9_VIDEO  = 2,    /**< ���9���� */
    IMOS_EN_MONITOR_VIDEO_COUNT
} MC_MONITOR_EX_E;

/**
 * @enum tagMC_ConfStatusEx
 * @brief ����״̬
 */
typedef  enum tagMC_ConfStatusEx
{
    IMOS_EN_CONF_STATUS_NOT_BEGIN   = 0,    /**< ����ԤԼ�ɹ���û�п�ʼ */
    IMOS_EN_CONF_STATUS_CALLING     = 1,    /**< �����ټ��� */
    IMOS_EN_CONF_STATUS_LIVING      = 2,    /**< �ٿ��� */
    IMOS_EN_CONF_STATUS_DROPPING    = 3,    /**< �ڽ�ɢ�� */
    IMOS_EN_CONF_STATUS_ENDED       = 4,    /**< �����Ѿ����� */
    IMOS_EN_CONF_UNREGED            = 5,    /**< ������ע�� */
    IMOS_EN_CONF_REGED              = 6,    /**< ������ע�� */
    IMOS_EN_CONF_STATUS_CREATE_FAIL = 1000, /**< ���鴴��ʧ�� */
    IMOS_EN_CONF_STATUS_CALL_IN     = 1001, /**< �����ٿ����һ���ͨ���� */
    IMOS_EN_CONF_STATUS_DELETED     = 1002, /**< ����ɾ�� */

    IMOS_EN_CONF_STATUS_INVALID = 0xFFFF
} MC_CONF_STATUS_EX_E;

/* Begin: added by o06299 for �������� of 2010.04.13 */
/**
* @enum tagConfMcuCascTypeEx
* @brief ����MCU��������
* @attention
*/
typedef  enum tagConfMcuCascTypeEx
{
    IMOS_EN_CONF_MCU_TYPE_MASTER     = 0,  /**< �򵥼�����MCU */
    IMOS_EN_CONF_MCU_TYPE_SECONDARY  = 1,  /**< �򵥴�MCU */
    IMOS_EN_CONF_MCU_TYPE_NO_CASCADE = 2,  /**< �Ǽ���MCU */
    IMOS_EN_CONF_MCU_TYPE_COUNT
}CONF_MCU_CASCADE_TYPE_E;
/* END: added by o06299 for �������� of 2010.04.13 */

/**
 * @enum tagMC_BroadcastedObserveEx
 * @brief ���㲥�᳡�Ĺۿ��᳡����
 * @attention
 */
typedef  enum tagMC_BroadcastedObserveEx
{
    IMOS_EN_OBSERVE_MYSELF  = 0,    /**< �Լ� */
    IMOS_EN_OBSERVE_LASTER  = 1,    /**< �ϴι㲥�᳡ */
    IMOS_EN_OBSERVE_MAIN    = 2,    /**< ���᳡ */
    IMOS_EN_OBSERVE_COUNT
} MC_BROADCASTED_OBSERVE_EX_E;

/**
 * @enum tagMC_SingleShowModeEx
 * @brief ��������ʾģʽ
 * @attention
 */
typedef  enum tagMC_SingleShowModeEx
{
    IMOS_EN_SHOW_SET_SITE        = 0,   /**< ��ʾָ���᳡ */
    IMOS_EN_SHOW_CHAIR           = 1,   /**< ��ʾ������ϯ */
    IMOS_EN_SHOW_SPEAKER         = 2,   /**< ��ʾ������ */
    IMOS_EN_SHOW_VOICE_ACTIVATED = 3,   /**< �������� */
    IMOS_EN_SHOW_COUNT
} MC_SINGLE_SHOW_MODE_EX_E;

/**
 * @enum tagHD_ColorGroupTypeEx
 * @brief 16����ɫ��
 * @attention
 */
typedef  enum tagHD_ColorGroupTypeEx
{
     IMOS_EN_COLOR_GROUP0   = 0,
     IMOS_EN_COLOR_GROUP1   = 1,
     IMOS_EN_COLOR_GROUP2   = 2,
     IMOS_EN_COLOR_GROUP3   = 3,
     IMOS_EN_COLOR_GROUP4   = 4,
     IMOS_EN_COLOR_GROUP5   = 5,
     IMOS_EN_COLOR_GROUP6   = 6,
     IMOS_EN_COLOR_GROUP7   = 7,
     IMOS_EN_COLOR_GROUP8   = 8,
     IMOS_EN_COLOR_GROUP9   = 9,
     IMOS_EN_COLOR_GROUP10  = 10,
     IMOS_EN_COLOR_GROUP11  = 11,
     IMOS_EN_COLOR_GROUP12  = 12,
     IMOS_EN_COLOR_GROUP13  = 13,
     IMOS_EN_COLOR_GROUP14  = 14,
     IMOS_EN_COLOR_GROUP15  = 15,
     IMOS_EN_COLOR_GROUP_COUNT
} HD_COLOR_GROUP_TYPE_EX_E;


/**
 * @enum tagHD_DiffServTypeEx
 * @brief Diffserv���
 * @attention
 */
typedef  enum  tagHD_DiffServTypeEx
{
    IMOS_EN_EXPEDITED_FORWARDING = 0xB8,              /**< ����ת������ */
    IMOS_EN_ASSURED_FORWARDING_4_1 = 0x88,            /**< ȷ��ת������41 */
    IMOS_EN_ASSURED_FORWARDING_4_2 = 0x90,            /**< ȷ��ת������42 */
    IMOS_EN_ASSURED_FORWARDING_4_3 = 0x98,            /**< ȷ��ת������43 */
    IMOS_EN_CLASS_BEST_EFFORD = 0                     /**< �������� */
} HD_DIFFSERV_TYPE_EX_E;

/**
 * @enum tagMC_ChairLookModeEx
 * @brief ��������Ҫ���ӵ��л��㲥����ʱ��ϯ�ۿ�ģʽ
 * @attention
 */
typedef enum tagMC_ChairLookModeEx
{
    IMOS_EN_CHAIR_LOOK_NOCHG    = 0,    /**< [����] Ĭ�� */
    IMOS_EN_CHAIR_LOOK_BROAD    = 1,    /**< [����㲥�л�] */
    IMOS_EN_CHAIR_LOOK_CLOSE    = 2,    /**< �ر� */
    IMOS_EN_CHAIR_LOOK_COUNT
} MC_CHAIR_LOOK_MODE_EX_E;

/**
 * @enum tagMC_MultiPicCycModeEx
 * @brief �໭����Ѳ��Ҫ���õ�����ģʽ
 * @attention
 */
typedef  enum  tagMC_MultiPicCycModeEx
{
    IMOS_EN_MULTI_CYC_SAME_LIST = 0,    /**< �໭����Ѳʹ��ͬһ����Ѳ�б� */
    IMOS_EN_MULTI_CYC_SEP_LIST  = 1,    /**< �໭����Ѳʹ�ø��Ե���Ѳ�б� */
    IMOS_EN_MULTI_CYC_COUNT
} MC_MULTIPIC_CYC_MODE_EX_E;

/**
 * @enum tagMC_SelectLookEx
 * @brief �᳡ѡ��ۿ�ö��
 * @attention
 */
typedef  enum  tagMC_SelectLookEx
{
    IMOS_EN_SEL_LOOK_NO     = 0,    /**< ������ѡ��ۿ� */
    IMOS_EN_SEL_LOOK_YES    = 1,    /**< ����ѡ��ۿ� */
    IMOS_EN_SEL_LOOK_COUNT
} MC_SELECT_LOOK_EX_E;

/**
 * @enum tagMC_AheadEndConfEx
 * @brief ��ǰ��������֪ͨ��ö��
 * @attention
 */
typedef  enum  tagMC_AheadEndConfEx
{
    IMOS_EN_AHEAD_END_CONF_NO     = 0,                /**< ����ǰ�������� */
    IMOS_EN_AHEAD_END_CONF_THREE  = 180,              /**< ��ǰ�����ӽ������� */
    IMOS_EN_AHEAD_END_CONF_FIVE   = 300,              /**< ��ǰ����ӽ������� */
    IMOS_EN_AHEAD_END_CONF_TEN    = 600,              /**< ��ǰʮ���ӽ������� */
    IMOS_EN_AHEAD_END_CONF_TWENTY = 1200,             /**< ��ǰ��ʮ���ӽ������� */
    IMOS_EN_AHEAD_END_CONF_THIRTY = 1800,             /**< ��ǰ��ʮ���ӽ������� */
    IMOS_EN_AHEAD_END_CONF_COUNT,
} MC_AHEAD_END_CONF_EX_E;

/**
 * @enum tagMC_ConfCycleTypeEx
 * @brief �����ظ���ʽ
 * @attention
 */
typedef  enum tagMC_ConfCycleTypeEx
{
    IMOS_EN_CONF_NOT_CYCLE              = 0,    /**< �����ڻ���[��һ��] */
    IMOS_EN_CONF_CYCLE_TYPE_EVERY_MONTH = 1,    /**< ���� */
    IMOS_EN_CONF_CYCLE_TYPE_EVERY_WEEK  = 2,    /**< ���� */
    IMOS_EN_CONF_CYCLE_TYPE_EVERY_DAY   = 3,    /**< ���� */
    IMOS_EN_CONF_CYCLE_TYPE_COUNT
} MC_CONF_CYCLE_TYPE_EX_E;

/**
 * @enum tagMC_ConfCycleEndTypeEx
 * @brief ���ڻ����������
 * @attention
 */
typedef  enum tagMC_ConfCycleEndTypeEx
{
    IMOS_EN_CONF_CYCLE_END_COUNT = 0,   /**< �ظ����� */
    IMOS_EN_CONF_CYCLE_END_DATE  = 1,   /**< ��ֹ���� */
    IMOS_EN_CONF_CYCLE_END_TYPE_COUNT
} MC_CONF_CYCLE_END_TYPE_EX_E;

/**
 * @enum tagConfReportSampleType
 * @brief ���������������
 * @attention
 */
typedef  enum tagConfReportSampleType
{
    IMOS_EN_CONF_REPORT_SAMPLE_TYPE_HOUR    = 0,    /**< ��Сʱ */
    IMOS_EN_CONF_REPORT_SAMPLE_TYPE_DAY     = 1,    /**< ���� */
    IMOS_EN_CONF_REPORT_SAMPLE_TYPE_MONTH   = 2,    /**< ���� */
    IMOS_EN_CONF_REPORT_SAMPLE_TYPE_COUNT
} CONF_REPORT_SAMPLE_TYPE_E;

/**
 * @enum tagMC_ConfCascadeTypeEx
 * @brief ���鼶��ģʽ
 * @attention
 */
typedef  enum tagMC_ConfCascadeTypeEx
{
    IMOS_EN_CONF_CASCADE_TYPE_NO    = 0,    /**< �Ǽ��� */
    IMOS_EN_CONF_CASCADE_TYPE_YES   = 1,    /**< ���� */
    IMOS_EN_CONF_CASCADE_TYPE_COUNT
} MC_CONF_CASCADE_TYPE_EX_E;

/**
 * @enum tagMC_ConfCascadeRelationEx
 * @brief ���鼶����ϵ,����Ӽ�������ʱָ��
 * @attention
 */
typedef  enum tagMC_ConfCascadeRelationEx
{
    IMOS_EN_CONF_CASCADE_RELATION_SUP    = 0,   /**< �ϼ����� */
    IMOS_EN_CONF_CASCADE_RELATION_SUB    = 1,   /**< �¼����� */
    IMOS_EN_CONF_CASCADE_RELATION_SIMPLE = 2,   /**< �򵥼��� */
    IMOS_EN_CONF_CASCADE_RELATION_COUNT
} MC_CONF_CASCADE_RELATION_EX_E;



/**
 * @enum tagHD_ColorTable16TypeEx
 * @brief 16ɫ��ɫ������ɫ����
 * @attention
 */
typedef  enum tagHD_ColorTable16TypeEx
{
     IMOS_EN_COLOR_TABLE16_TYPE0    = 0,
     IMOS_EN_COLOR_TABLE16_TYPE1    = 1,
     IMOS_EN_COLOR_TABLE16_TYPE2    = 2,
     IMOS_EN_COLOR_TABLE16_TYPE3    = 3,
     IMOS_EN_COLOR_TABLE16_TYPE4    = 4,
     IMOS_EN_COLOR_TABLE16_TYPE5    = 5,
     IMOS_EN_COLOR_TABLE16_TYPE6    = 6,
     IMOS_EN_COLOR_TABLE16_TYPE7    = 7,
     IMOS_EN_COLOR_TABLE16_TYPE8    = 8,
     IMOS_EN_COLOR_TABLE16_TYPE9    = 9,
     IMOS_EN_COLOR_TABLE16_TYPE10   = 10,
     IMOS_EN_COLOR_TABLE16_TYPE11   = 11,
     IMOS_EN_COLOR_TABLE16_TYPE12   = 12,
     IMOS_EN_COLOR_TABLE16_TYPE13   = 13,
     IMOS_EN_COLOR_TABLE16_TYPE14   = 14,
     IMOS_EN_COLOR_TABLE16_TYPE15   = 15,
     IMOS_EN_COLOR_TABLE16_TYPE_COUNT
} HD_COLOR_TABLE16_TYPE_EX_E;

/**
 * @enum tagHD_BannerPosEx
 * @brief ���λ��
 * @attention
 */
typedef  enum  tagHD_BannerPosEx
{
    IMOS_EN_BANNER_POS_TOPQTR   = 0,    /**< ���λ��ռ��Ļ�Ϸ�1/4 */
    IMOS_EN_BANNER_POS_TOP3RD   = 1,    /**< ���λ��ռ��Ļ�Ϸ�1/3 */
    IMOS_EN_BANNER_POS_TOP5     = 2,    /**< ���λ��ռ��Ļ�Ϸ�1/5 */
    IMOS_EN_BANNER_POS_TOP6TH   = 3,    /**< ���λ��ռ��Ļ�Ϸ�1/6 */
    IMOS_EN_BANNER_POS_TOP7     = 4,    /**< ���λ��ռ��Ļ�Ϸ�1/7 */
    IMOS_EN_BANNER_POS_TOP8TH   = 5,    /**< ���λ��ռ��Ļ�Ϸ�1/8 */
} HD_BANNER_POS_EX_E;

/**
 * @enum tagHD_ConfShowTimerTypeEx
 * @brief ��ʾ����ʱ�������,�����Ѿ���ʼʱ�䡢��������ʣ��ʱ��
 * @attention
 */
typedef  enum tagHD_ConfShowTimerTypeEx
{
    IMOS_EN_SHOW_TIMER_TYPE_NO      = 0,    /**< ����ʾ����ʱ�� */
    IMOS_EN_SHOW_TIMER_TYPE_BEGIN   = 1,    /**< ����ʣ��ʱ�� */
    IMOS_EN_SHOW_TIMER_TYPE_END     = 2,    /**< �������ʱ�� */
    IMOS_EN_SHOW_TIMER_TYPE_COUNT
} HD_CONF_SHOW_TIMER_TYPE_EX_E;



/**
 * @enum tagMC_ProtocolTypeEx
 * @brief MCU����֧�ֵ�Э������
 * @attention
 */
typedef  enum tagMC_ProtocolTypeEx
{
    IMOS_EN_PROTOCOL_H323   = 0,    /**< H323Э�� */
    IMOS_EN_PROTOCOL_SIP    = 1,    /**< SIPЭ�� */
    IMOS_EN_PROTOCOL_MIX    = 2,    /**< ��Э��[˫Э��] */
    IMOS_EN_PROTOCOL_COUNT
} MC_PROTOCOL_TYPE_EX_E;

/**
 * @enum tagMC_ConfModeEx
 * @brief ����ģʽ
 * @attention
 */
typedef  enum tagMC_ConfModeEx
{
    IMOS_EN_CONF_MODE_DIRECTOR       = 0,   /**< ����ģʽ */
    IMOS_EN_CONF_MODE_CHAIR          = 1,   /**< ��ϯģʽ */
    IMOS_EN_CONF_MODE_VOICE_ACTUATOR = 2,   /**< ��������ģʽ */
    IMOS_EN_CONF_MODE_COUNT
} MC_CONF_MODE_EX_E;

/**
 * @enum tagHD_MultiPicNumEx
 * @brief �����������
 * @attention
 */
typedef  enum  tagHD_MultiPicNumEx
{
    IMOS_EN_MULTI_PIC_0 = 0,                 /**< ��ת�������� */
    IMOS_EN_MULTI_PIC_1 = 1,                 /**< ������ */
    IMOS_EN_MULTI_PIC_2 = 2,                 /**< 2���� */
    IMOS_EN_MULTI_PIC_3 = 3,                 /**< 3���� */
    IMOS_EN_MULTI_PIC_4 = 4,                 /**< 4���� */
    IMOS_EN_MULTI_PIC_5 = 5,                 /**< 5���� */
    IMOS_EN_MULTI_PIC_6 = 6,                 /**< 6���� */
    IMOS_EN_MULTI_PIC_7 = 7,                 /**< 7���� */
    IMOS_EN_MULTI_PIC_8 = 8,                 /**< 8���� */
    IMOS_EN_MULTI_PIC_9 = 9,                 /**< 9���� */
    IMOS_EN_MULTI_PIC_10 = 10,               /**< 10���� */
    IMOS_EN_MULTI_PIC_11 = 11,               /**< 11���� */
    IMOS_EN_MULTI_PIC_12 = 12,               /**< 12���� */
    IMOS_EN_MULTI_PIC_13 = 13,               /**< 13���� */
    IMOS_EN_MULTI_PIC_14 = 14,               /**< 14���� */
    IMOS_EN_MULTI_PIC_15 = 15,               /**< 15���� */
    IMOS_EN_MULTI_PIC_16 = 16,               /**< 16���� */
    IMOS_EN_MULTI_PIC_17 = 17,               /**< 17���� */
    IMOS_EN_MULTI_PIC_18 = 18,               /**< 18���� */
    IMOS_EN_MULTI_PIC_19 = 19,               /**< 19���� */
    IMOS_EN_MULTI_PIC_20 = 20,               /**< 20���� */
    IMOS_EN_MULTI_PIC_21 = 21,               /**< 21���� */
    IMOS_EN_MULTI_PIC_22 = 22,               /**< 22���� */
    IMOS_EN_MULTI_PIC_23 = 23,               /**< 23���� */
    IMOS_EN_MULTI_PIC_24 = 24,               /**< 24���� */
    IMOS_EN_MULTI_PIC_25 = 25,               /**< 25���� */
    IMOS_EN_MULTI_PIC_26 = 26,               /**< 26���� */
    IMOS_EN_MULTI_PIC_27 = 27,               /**< 27���� */
    IMOS_EN_MULTI_PIC_28 = 28,               /**< 28���� */
    IMOS_EN_MULTI_PIC_29 = 29,               /**< 29���� */
    IMOS_EN_MULTI_PIC_30 = 30,               /**< 30���� */
    IMOS_EN_MULTI_PIC_31 = 31,               /**< 31���� */
    IMOS_EN_MULTI_PIC_32 = 32,               /**< 32���� */
    IMOS_EN_MULTI_PIC_33 = 33,               /**< 33���� */
    IMOS_EN_MULTI_PIC_34 = 34,               /**< 34���� */
    IMOS_EN_MULTI_PIC_35 = 35,               /**< 35���� */
    IMOS_EN_MULTI_PIC_36 = 36,               /**< 36���� */
    IMOS_EN_MULTI_PIC_NUM_COUNT
} HD_MULTI_PIC_NUM_EX_E;

/**
 * @enum tagMC_MultiPicObserveEx
 * @brief �໭��ʱ�ۿ�������
 * @attention
 */
typedef  enum tagMC_MultiPicObserveEx
{
    IMOS_EN_MULTI_PIC_OBSERVE_BROAD_CAST = 0,   /**< �㲥�໭�� */
    IMOS_EN_MULTI_PIC_OBSERVE_CHAIR      = 1,   /**< ��ϯ�ۿ��໭�� */
    IMOS_EN_MULTI_PIC_OBSERVE_COUNT
} MC_MULTI_PIC_OBSERVE_EX_E;

/**
 * @enum tagMC_DropReinviteModeEx
 * @brief ����������ʽ
 * @attention
 */
typedef  enum tagMC_DropReinviteModeEx
{
    IMOS_DROP_REINVITE_MODE_ALWAYS  = 0,    /**< ������������ [����] */
    IMOS_DROP_REINVITE_MODE_AUTO    = 1,    /**< �����Զ����� [�Զ���20��] */
    IMOS_DROP_REINVITE_MODE_CLOSE   = 2,    /**< �ر��������� [�ر�] */
    IMOS_DROP_REINVITE_MODE_COUNT
} MC_DROP_REINVITE_MODE_EX_E;

/**
 * @enum tagHD_FrameThickTypeEx
 * @brief �߿��ϸ����
 * @attention
 */
typedef  enum  tagHD_FrameThickTypeEx
{
    IMOS_EN_FRAME_THICK     = 0,    /**< �ֱ߿� */
    IMOS_EN_FRAME_MIDDLE    = 1,    /**< �����߿� */
    IMOS_EN_FRAME_THIN      = 2,    /**< ϸ�߿� */
    IMOS_EN_FRAME_COUNT
} HD_FRAME_THICK_TYPE_EX_E;

/**
 * @enum tagMC_CycleTableIndexEx
 * @brief ��Ѳ������
 * @attention
 */
typedef enum tagMC_CycleTableIndexEx
{
    IMOS_EN_CYCLE_TABLE_INDEX_0 = 0,        /**< ȫ���᳡�� */
    IMOS_EN_CYCLE_TABLE_INDEX_1 = 1,        /**< ��Ѳ��1 */
    IMOS_EN_CYCLE_TABLE_INDEX_2 = 2,        /**< ��Ѳ��2 */
    IMOS_EN_CYCLE_TABLE_INDEX_3 = 3,        /**< ��Ѳ��3 */
    IMOS_EN_CYCLE_TABLE_INDEX_4 = 4,        /**< ��Ѳ��4 */
    IMOS_EN_CYCLE_TABLE_INDEX_5 = 5,        /**< ��Ѳ��5 */
    IMOS_EN_CYCLE_TABLE_INDEX_6 = 6,        /**< ��Ѳ��6 */
    IMOS_EN_CYCLE_TABLE_INDEX_7 = 7,        /**< ��Ѳ��7 */
    IMOS_EN_CYCLE_TABLE_INDEX_8 = 8,        /**< ��Ѳ��8 */
    IMOS_EN_CYCLE_TABLE_INDEX_9 = 9,        /**< ��Ѳ��9 */
    IMOS_EN_CYCLE_TABLE_INDEX_COUNT
} MC_CYCLE_TABLE_INDEX_EX_E;

/**
 * @enum tagHD_ColorTable4TypeEx
 * @brief 4ɫ��ɫ������ɫ����
 * @attention
 */
typedef  enum tagHD_ColorTable4TypeEx
{
     IMOS_EN_COLOR_TABLE4_TYPE0 = 0,
     IMOS_EN_COLOR_TABLE4_TYPE1 = 1,
     IMOS_EN_COLOR_TABLE4_TYPE2 = 2,
     IMOS_EN_COLOR_TABLE4_TYPE3 = 3,
     IMOS_EN_COLOR_TABLE4_TYPE_COUNT
} HD_COLOR_TABLE4_TYPE_EX_E;

/**
 * @enum tagHD_TitlePosEx
 * @brief ������Ļλ��
 * @attention
 */
typedef  enum tagHD_TitlePosEx
{
    IMOS_EN_TITLE_POS_BOTTOM      = 0,      /**< ��Ļ����Ļ�·�1 */
    IMOS_EN_TITLE_POS_TOP         = 1,      /**< ��Ļ����Ļ�Ϸ�1 */
    IMOS_EN_TITLE_POS_LEFT        = 2,      /**< ��Ļ����Ļ���1 */
    IMOS_EN_TITLE_POS_RIGHT       = 3,      /**< ��Ļ����Ļ�Ҳ�1 */
    IMOS_EN_TITLE_POS_FULL_SCREEN = 4,      /**< ��Ļ��ȫ����ʾ[ȫ��] */
    IMOS_EN_TITLE_POS_COUNT
} HD_TITLE_POS_EX_E;

/**
 * @enum tagHD_TitleRollSpeedEx
 * @brief ������Ļ�����ٶ�
 * @attention
 */
typedef  enum tagHD_TitleRollSpeedEx
{
    IMOS_EN_TITLE_ROLL_MID          = 0,    /**< ��Ļ��������[����] */
    IMOS_EN_TITLE_ROLL_LOW          = 1,    /**< ��Ļ��������[����] */
    IMOS_EN_TITLE_ROLL_FAST         = 2,    /**< ��Ļ��������[����] */
    IMOS_EN_TITLE_ROLL_QUIESCENCE   = 3,    /**< ��Ļ��ֹ[��ֹ] */
    IMOS_EN_TITLE_ROLL_SPEED_COUNT
} HD_TITLE_ROLL_SPEED_EX_E;

/**
 * @enum tagHD_TitleRollEndTypeEx
 * @brief ��Ļ����������ʽ
 * @attention
 */
typedef  enum tagHD_TitleRollEndTypeEx
{
    IMOS_EN_TITLE_ROLL_END_COUNT    = 0,    /**< ��Ļ�������� */
    IMOS_EN_TITLE_ROLL_END_TIME     = 1,    /**< ��Ļ����ʱ�� */
    IMOS_EN_TITLE_ROLL_END_TYPE_COUNT
} HD_TITLE_ROLL_END_TYPE_EX_E;

/**
 * @enum tagHD_TitleRollDirectionEx
 * @brief ������Ļ��������
 * @attention
 */
typedef  enum tagHD_TitleRollDirectionEx
{
    IMOS_EN_ROLL_FROM_RIGHT_TO_LEFT = 0,    /**< ��Ļ���ҵ������[���ҵ���] */
    IMOS_EN_ROLL_FROM_BUTTOM_TO_TOP = 1,    /**< ��Ļ���µ��Ϲ���[���µ���] */
    IMOS_EN_TITLE_ROLL_DIRECTION_COUNT
} HD_TITLE_ROLL_DIRECTION_EX_E;

/**
 * @enum tagHD_TitleLastTimeEx
 * @brief ������Ļ����ʱ��
 * @attention
 */
typedef  enum tagHD_TitleLastTimeEx
{
    IMOS_EN_TITLE_LAST_TIME_FOREVER = 0,       /**< ��Ļ����ʱ������[����] */
    IMOS_EN_TITLE_LAST_TIME_5MIN  = 5,         /**< ��Ļ����5����[5����] */
    IMOS_EN_TITLE_LAST_TIME_10MIN = 10,        /**< ��Ļ����10����[10����] */
    IMOS_EN_TITLE_LAST_TIME_15MIN = 15,        /**< ��Ļ����15����[15����] */
    IMOS_EN_TITLE_LAST_TIME_30MIN = 30,        /**< ��Ļ����30����[30����] */
    IMOS_EN_TITLE_LAST_TIME_45MIN = 45,        /**< ��Ļ����45����[45����] */
    IMOS_EN_TITLE_LAST_TIME_60MIN = 60,        /**< ��Ļ����60����[60����] */
    IMOS_EN_TITLE_LAST_TIME_120MIN = 120,      /**< ��Ļ����120����[120����] */
    IMOS_EN_TITLE_LAST_TIME_COUNT
} HD_TITLE_LAST_TIME_EX_E;

/**
 * @enum tagHD_MultiPicModeEx
 * @brief ����ģʽ
 * @attention
 */
typedef  enum  tagHD_MultiPicModeEx
{
    IMOS_EN_SINGLE_MODE =0,     /**< ������ */

    IMOS_EN_MULTI_PIC2_MODE     = 1,    /**< 2���� */
    IMOS_EN_MULTI_PIC3_MODE1    = 2,    /**< 3����1  */
    IMOS_EN_MULTI_PIC3_MODE2    = 3,    /**< 3����2  */
    IMOS_EN_MULTI_PIC4_MODE     = 4,    /**< 4����  */

    IMOS_EN_MULTI_PIC5P1_MODE   = 5,    /**< 5+1����  */
    IMOS_EN_MULTI_PIC9_MODE     = 6,    /**< 9����  */

    IMOS_EN_MULTI_PIC3P1_MODE   = 7,    /**< 3+1���� */
    IMOS_EN_MULTI_PIC4P2_MODE1  = 8,    /**< 4+2����1 */
    IMOS_EN_MULTI_PIC12_MODE    = 9,    /**< 12���� */
    IMOS_EN_MULTI_PIC4P2_MODE2  = 10,   /**< 4+2����2 */
    IMOS_EN_MULTI_PIC7P1_MODE   = 11,   /**< 7+1���� */
    IMOS_EN_MULTI_PIC12P1_MODE1 = 12,   /**< 12+1����1 */
    IMOS_EN_MULTI_PIC8P2_MODE1  = 13,   /**< 8+2����1 */
    IMOS_EN_MULTI_PIC16_MODE    = 14,   /**< 16���� */
    IMOS_EN_MULTI_PIC8P2_MODE2  = 15,   /**< 8+2����2 */
    IMOS_EN_MULTI_PIC8P2_MODE3  = 16,   /**< 8+2����3 */
    IMOS_EN_MULTI_PIC8P2_MODE4  = 17,   /**< 8+2����4 */
    IMOS_EN_MULTI_PIC8P2_MODE5  = 18,   /**< 8+2����8 */
    IMOS_EN_MULTI_PIC12P1_MODE2 = 19,   /**< 12+1����2 */
    IMOS_EN_MULTI_PIC12P1_MODE3 = 20,   /**< 12+1����3 */
    IMOS_EN_MULTI_PIC12P1_MODE4 = 21,   /**< 12+1����4 */
    IMOS_EN_MULTI_PIC12P1_MODE5 = 22,   /**< 12+1����5 */
    IMOS_EN_MULTI_PIC8P2_MODE6  = 23,   /**< 8+2����6 */
    IMOS_EN_MULTI_PIC3P4_MODE1  = 24,   /**< 3+4����1 */
    IMOS_EN_MULTI_PIC3P4_MODE2  = 25,   /**< 3+4����2 */
    IMOS_EN_MULTI_PIC3P4_MODE3  = 26,   /**< 3+4����3 */
    IMOS_EN_MULTI_PIC3P4_MODE4  = 27,   /**< 3+4����4 */

    IMOS_EN_MULTI_PIC11P1_MODE  = 28,   /**< 11+1���� */
    IMOS_EN_MULTI_PIC20P1_MODE1 = 29,   /**< 20+1����1 */
    IMOS_EN_MULTI_PIC20P1_MODE2 = 30,   /**< 20+1����2 */
    IMOS_EN_MULTI_PIC24P3_MODE1 = 31,   /**< 24+3����1 */
    IMOS_EN_MULTI_PIC24P3_MODE3 = 32,   /**< 24+3����3 */
    IMOS_EN_MULTI_PIC6P12_MODE3 = 33,   /**< 6+12����3 */
    IMOS_EN_MULTI_PIC6P12_MODE5 = 34,   /**< 6+12����5 */
    IMOS_EN_MULTI_PIC27P1_MODE1 = 35,   /**< 27+1����1 */
    IMOS_EN_MULTI_PIC9P3_MODE4  = 36,   /**< 9+3����4 */
    IMOS_EN_MULTI_PIC18P2_MODE1 = 37,   /**< 18+2����1 */
    IMOS_EN_MULTI_PIC18P2_MODE4 = 38,   /**< 18+2����4 */
    IMOS_EN_MULTI_PIC18P2_MODE6 = 39,   /**< 18+2����6 */
    IMOS_EN_MULTI_PIC36_MODE    = 40,   /**< 36���� */

    IMOS_EN_MULTI_PICE_COUNT
} HD_MULTIPIC_MODE_EX_E;



/**
 * @enum tagHD_VideoProtFmtEx
 * @brief ��Ƶ��Э���ʽ����
 * @attention
 */
typedef  enum  tagHD_VideoProtFmtEx
{
    IMOS_EN_VID_H264_1080P  = 0,   /**<  H264Э�� 1080P��ʽ */
    IMOS_EN_VID_H264_720P   = 1,   /**<  H264Э�� 720P��ʽ */
    IMOS_EN_VID_H264_4CIF   = 2,   /**<  H264Э�� 4CIF��ʽ */
    IMOS_EN_VID_H264_CIF    = 3,   /**<  H264Э�� CIF��ʽ */
    IMOS_EN_VID_H263_1080P  = 4,   /**<  H263Э�� 1080P��ʽ */
    IMOS_EN_VID_H263_720P   = 5,   /**<  H263Э�� 720P��ʽ */
    IMOS_EN_VID_H263_4CIF   = 6,   /**<  H263Э�� 4CIF��ʽ */
    IMOS_EN_VID_H263_CIF    = 7,   /**<  H263Э�� CIF��ʽ */
    IMOS_EN_VID_PROT_FMT_COUNT
} HD_VIDEO_PROT_FMT_EX_E;


/**
 * @enum tagMC_CycleTimeIntervalEx
 * @brief ��Ѳʱ����
 * @attention
 */
typedef  enum tagMC_CycleTimeIntervalEx
{
    IMOS_EN_CYCLE_TIME_INTERVAL_15SEC = 15,                   /**< 15�� */
    IMOS_EN_CYCLE_TIME_INTERVAL_30SEC = 30,                   /**< 30�� */
    IMOS_EN_CYCLE_TIME_INTERVAL_45SEC = 45,                   /**< 45�� */
    IMOS_EN_CYCLE_TIME_INTERVAL_1MIN  = 60,                   /**< 1���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_2MIN  = 120,                  /**< 2���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_5MIN  = 300,                  /**< 5���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_10MIN = 600,                  /**< 10���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_15MIN = 900,                  /**< 15���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_30MIN = 1800,                 /**< 30���� */
    IMOS_EN_CYCLE_TIME_INTERVAL_COUNT
} MC_CYCLE_TIME_INTERVAL_EX_E;


/**
 * @enum tagMC_MultiPicOnePicEx
 * @brief �໭����С��������
 * @attention
 */
typedef  enum tagMC_MultiPicOnePicEx
{
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_AUTO_FILL        = 0,    /**< �Զ���� */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_CHAIR            = 1,    /**< ������ϯ */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_SPEAKER          = 2,    /**< ������ */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_SET_SITE         = 3,    /**< ָ���᳡ */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_CYCLE            = 4,    /**< �᳡��Ѳ */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_VOICE_ACTUATOR   = 5,    /**< �������� */
    IMOS_EN_CONF_MULTI_PIC_ONE_PIC_COUNT
} MC_MULTI_PIC_ONE_PIC_EX_E;

/**
 * @enum tagHD_VideoProtEx
 * @brief ��Ƶ��Э������
 * @attention
 */
typedef  enum  tagHD_VideoProtEx
{
    IMOS_EN_VID_PROT_H261   = 0,    /**< H261Э�� */
    IMOS_EN_VID_PROT_H263   = 1,    /**< H263Э�� */
    IMOS_EN_VID_PROT_H263P  = 2,    /**< H263+Э�� */
    IMOS_EN_VID_PROT_H264   = 3,    /**< H264Э�� */
    IMOS_EN_VID_PROT_MPEG4  = 4,    /**< MPEG4Э�� */
    IMOS_EN_VID_PROT_AUTO   = 5,    /**< �Զ� */
    IMOS_EN_VID_PROT_COUNT
} HD_VIDEO_PROT_EX_E;

/**
 * @enum tagHD_AudioProtEx
 * @brief ��ƵЭ������
 * @attention
 */
typedef  enum  tagHD_AudioProtEx
{
    IMOS_EN_AUD_PROT_G711A                  = 0,    /**< G711 A */
    IMOS_EN_AUD_PROT_G711U                  = 1,    /**< G711 U */
    IMOS_EN_AUD_PROT_G722_48K               = 2,    /**< G.722 48k */
    IMOS_EN_AUD_PROT_G722_56K               = 3,    /**< G.722 56k */
    IMOS_EN_AUD_PROT_G722_64K               = 4,    /**< G.722 64k */
    IMOS_EN_AUD_PROT_G7221_24K              = 5,    /**< G.722.1 24k */
    IMOS_EN_AUD_PROT_G7221_32K              = 6,    /**< G.722.1 32k */
    IMOS_EN_AUD_PROT_G7221AC_24K            = 7,    /**< G.722.1 AnnexC 24k */
    IMOS_EN_AUD_PROT_G7221AC_32K            = 8,    /**< G.722.1 AnnexC 32k */
    IMOS_EN_AUD_PROT_G7221AC_48K            = 9,    /**< G.722.1 AnnexC 48k */
    IMOS_EN_AUD_PROT_G7231_5_3K             = 10,   /**< G.723.1 5.3k */
    IMOS_EN_AUD_PROT_G7231_6_3K             = 11,   /**< G.723.1 6.3k */
    IMOS_EN_AUD_PROT_G7231SC_5_3K           = 12,   /**< G.723.1 5.3k Silence Compression */
    IMOS_EN_AUD_PROT_G7231SC_6_3K           = 13,   /**< G.723.1 6.3k Silence Compression */
    IMOS_EN_AUD_PROT_G728                   = 14,   /**< G728 */
    IMOS_EN_AUD_PROT_G7221AC_STEREO_48K     = 15,   /**< G.722.1 AnnexC 48k stereo */
    IMOS_EN_AUD_PROT_G7221AC_STEREO_64K     = 16,   /**< G.722.1 AnnexC 64k stereo */
    IMOS_EN_AUD_PROT_G7221AC_STEREO_96K     = 17,   /**< G.722.1 AnnexC 96k stereo */
    IMOS_EN_AUD_PROT_AUTO                   = 18,   /**< �Զ� */
    IMOS_EN_AUD_PROT_COUNT
} HD_AUDIO_PROT_EX_E;

/**
 * @enum tagHD_VideoFormatEx
 * @brief �����С
 * @attention
 */
typedef  enum  tagHD_VideoFormatEx
{
    IMOS_EN_VIDEO_FMT_1080P     = 0,    /**< 1080P(1920 x 1080) */
    IMOS_EN_VIDEO_FMT_720P      = 1,    /**< 720P(1280 x 720) */
    IMOS_EN_VIDEO_FMT_4CIF      = 2,    /**< 4CIF(704 x 576) */
    IMOS_EN_VIDEO_FMT_CIF       = 3,    /**< CIF(352 x 288) */
    IMOS_EN_VIDEO_FMT_QCIF      = 4,    /**< QCIF(176 x 144) */
    IMOS_EN_VIDEO_FMT_WUXGA     = 5,    /**< WUXGA(1920 x 1200) */
    IMOS_EN_VIDEO_FMT_UXGA      = 6,    /**< UXGA(1600 x 1200) */
    IMOS_EN_VIDEO_FMT_SXGA      = 7,    /**< SXGA(1280 x 1024) */
    IMOS_EN_VIDEO_FMT_XGA       = 8,    /**< XGA(1024 x 768) */
    IMOS_EN_VIDEO_FMT_SVGA      = 9,    /**< SVGA(800 x 600) */
    IMOS_EN_VIDEO_FMT_VGA       = 10,   /**< VGA(640 x 480) */
    IMOS_EN_VIDEO_FMT_AUTO      = 11,   /**< �Զ� */
    IMOS_EN_VIDEO_FMT_NONSTD    = 12,   /**< �Ǳ��ʽ*/
    IMOS_EN_VIDEO_FMT_COUNT

} HD_VIDEO_FORMAT_EX_E;

/**
 * @enum tagHD_FrameRateEx
 * @brief ֡��
 * @attention
 */
typedef  enum  tagHD_FrameRateEx
{
    IMOS_EN_FRAME_RATE_AUTO = 0,                                   /**< �Զ� */
    IMOS_EN_FRAME_RATE_3F = 3,                                     /**< 3֡ */
    IMOS_EN_FRAME_RATE_5F = 5,                                     /**< 5֡ */
    IMOS_EN_FRAME_RATE_10F = 10,                                   /**< 10֡ */
    IMOS_EN_FRAME_RATE_15F = 15,                                   /**< 15֡ */
    IMOS_EN_FRAME_RATE_25F = 25,                                   /**< 25֡ */
    IMOS_EN_FRAME_RATE_30F = 30,                                   /**< 30֡ */
    IMOS_EN_FRAME_RATE_COUNT
} HD_FRAME_RATE_EX_E;

/**
 * @enum tagMC_ConfMixRateEx
 * @brief �����������
 * @attention
 */
typedef  enum tagMC_ConfMixRateEx
{
    IMOS_EN_MIX_RATE_NO     = 0,    /**< ��֧�ֻ��� */
    IMOS_EN_MIX_RATE_PART   = 1,    /**< ���ֻ��� */
    IMOS_EN_MIX_RATE_ALL    = 2,    /**< ȫ���� */
    IMOS_EN_MIX_RATE_CHOICE = 3,    /**< ѡ����� */
    IMOS_EN_MIX_RATE_COUNT
} MC_CONF_MIXRATE_EX_E;

/**
 * @enum tagHD_RateAndFrameAdaptModeEx
 * @brief ���ʣ�֡���������ȵ�ö��
 * @attention
 */
typedef  enum  tagHD_RateAndFrameAdaptModeEx
{
    IMOS_EN_RATE_FRAME_NO           = 0,    /**< ���������� */
    IMOS_EN_RATE_FRAME_FALL         = 1,    /**< ���� */
    IMOS_EN_RATE_FRAME_RATE_FIRST   = 2,    /**< ������������ */
    IMOS_EN_RATE_FRAME_FRAME_FIRST  = 3,    /**< ֡���������� */
    IMOS_EN_RATE_FRAME_COUNT
} HD_RATE_AND_FRAME_ADAPT_MODE_EX_E;

/**
 * @enum tagMC_AdaptCountEx
 * @brief ����·��ö��
 * @attention
 */
typedef  enum  tagMC_AdaptCountEx
{
    IMOS_EN_ADAPT_COUNT_1 = 1,            /**< ����1· */
    IMOS_EN_ADAPT_COUNT_2 = 2,            /**< ����2· */
    IMOS_EN_ADAPT_COUNT_3 = 3,            /**< ����3· */
    IMOS_EN_ADAPT_COUNT_COUNT
} MC_ADAPT_COUNT_EX_E;


/**
 * @enum tagMC_ConfTypeEx
 * @brief ���鴴��ʱ��������
 * @attention
 */
typedef  enum tagMC_ConfTypeEx
{
    IMOS_EN_CONF_TYPE_ONLY_AUDIO    = 0,    /**< ����Ƶ���� */
    IMOS_EN_CONF_TYPE_AUDIVISION    = 1,    /**< ����Ƶ���� */
    IMOS_EN_CONF_TYPE_ANNEX_VIDEO   = 2,    /**< ˫����Ƶ���� */
    IMOS_EN_CONF_TYPE_COUNT
} MC_CONF_TYPE_EX_E;


/**
 * @enum tagHD_RoleVideoTypeEx
 * @brief ��������
 * @attention
 */
typedef  enum    tagHD_RoleVideoTypeEx
{
     IMOS_EN_HD_ROLE_LIVE           = 1,    /**< ����ģʽ */
     IMOS_EN_HD_ROLE_TOKEN          = 2,    /**< ����ģʽ */
     IMOS_EN_HD_ROLE_LIVE_TOKEN     = 3,    /**< ͬʱ֧�����ɺ�����ģʽ */
     IMOS_EN_HD_ROLE_NO_STANDARD    = 4,    /**< �Ǳ�ģʽ */
     IMOS_EN_HD_ROLE_COUNT                                        /**< ���� */
} HD_ROLE_VIDEO_TYPE_EX_E;

/**
 * @enum tagMC_MediaEncryptEx
 * @brief ý���������
 * @attention
 */
typedef enum tagMC_MediaEncryptEx
{
    IMOS_EN_MEDIA_ENCRYPT_NO    = 0,    /**< ��֧��ý����� */
    IMOS_EN_MEDIA_ENCRYPT_AES   = 1,    /**< AES���� */
    IMOS_EN_MEDIA_ENCRYPT_DES   = 2,    /**< DES���� */
    IMOS_EN_MEDIA_ENCRYPT_COUNT
} MC_MEDIA_ENCRYPT_EX_E;


/**
 * @enum tagMC_SignalTypeEx
 * @brief ý���������
 * @attention
 */
typedef  enum tagMC_SignalTypeEx
{
    IMOS_EN_SIGNAL_TYPE_NO      = 0,    /**< ��֧��������� */
    IMOS_EN_SIGNAL_TYPE_H235    = 1,    /**< H235���� */
    IMOS_EN_SIGNAL_TYPE_COUNT
} MC_SIGNAL_TYPE_EX_E;

/**
 * @enum tagHD_IFrameNumEx
 * @brief ÿ�ν���/����I֡����ķ���I֡֡��
 * @attention
 */
typedef  enum  tagHD_IFrameNumEx
{
    IMOS_EN_IFRAME_NUM_1F = 1,                               /**< 1֡ */
    IMOS_EN_IFRAME_NUM_2F = 2,                               /**< 2֡ */
    IMOS_EN_IFRAME_NUM_3F = 3,                               /**< 3֡ */
    IMOS_EN_IFRAME_NUM_4F = 4,                               /**< 4֡ */
    IMOS_EN_IFRAME_NUM_5F = 5,                               /**< 5֡ */
    IMOS_EN_IFRAME_NUM_6F = 6,                               /**< 6֡ */
    IMOS_EN_IFRAME_NUM_7F = 7,                               /**< 7֡ */
    IMOS_EN_IFRAME_NUM_8F = 8,                               /**< 8֡ */
    IMOS_EN_IFRAME_NUM_9F = 9,                               /**< 9֡ */
    IMOS_EN_IFRAME_NUM_10F = 10,                             /**< 10֡ */
    IMOS_EN_IFRAME_NUM_20F = 20,                             /**< 20֡ */
    IMOS_EN_IFRAME_NUM_30F = 30,                             /**< 30֡ */
    IMOS_EN_IFRAME_NUM_40F = 40,                             /**< 40֡ */
    IMOS_EN_IFRAME_NUM_50F = 50,                             /**< 50֡ */
    IMOS_EN_IFRAME_NUM_60F = 60,                             /**< 60֡ */
    IMOS_EN_IFRAME_NUM_70F = 70,                             /**< 70֡ */
    IMOS_EN_IFRAME_NUM_80F = 80,                             /**< 80֡ */
    IMOS_EN_IFRAME_NUM_90F = 90,                             /**< 90֡ */
    IMOS_EN_IFRAME_NUM_120F = 120,                           /**< 120֡ */
    IMOS_EN_IFRAME_NUM_FRAME_COUNT
} HD_IFRAME_NUM_FRAME_EX_E;

/**
 * @enum tagHD_IFrameAutoEncIntervalEx
 * @brief �Զ�����I֡��ʱ����
 * @attention
 */
typedef  enum  tagHD_IFrameAutoEncIntervalEx
{
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_20MS = 20,                    /**< 20ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_40MS = 40,                    /**< 40ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_60MS = 60,                    /**< 60ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_80MS = 80,                    /**< 80ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_120MS = 120,                  /**< 120ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_160MS = 160,                  /**< 160ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_200MS = 200,                  /**< 200ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_240MS = 240,                  /**< 240ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_300MS = 300,                  /**< 300ms */
    IMOS_HD_IFRAME_ATUO_ENC_INTERVAL_COUNT
} HD_IFRAME_ATUO_ENC_INTERVAL_EX_E;


/**
 * @enum tagHD_IFrameManualEncIntervalEx
 * @brief �Զ�����I֡��ʱ����
 * @attention
 */
typedef  enum  tagHD_IFrameManualEncIntervalEx
{
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_1S = 1,                      /**< 1s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_2S = 2,                      /**< 2s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_3S = 3,                      /**< 3s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_4S = 4,                      /**< 4s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_5S = 5,                      /**< 5s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_6S = 6,                      /**< 6s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_7S = 7,                      /**< 7s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_8S = 8,                      /**< 8s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_9S = 9,                      /**< 9s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_10S = 10,                    /**< 10s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_20S = 20,                    /**< 20s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_30S = 30,                    /**< 30s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_40S = 40,                    /**< 40s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_50S = 50,                    /**< 50s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_60S = 60,                    /**< 60s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_70S = 70,                    /**< 70s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_80S = 80,                    /**< 80s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_90S = 90,                    /**< 90s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_120S = 120,                  /**< 120s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_180S = 180,                  /**< 180s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_300S = 300,                  /**< 300s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_600S = 600,                  /**< 600s */
    IMOS_EN_IFRAME_MANUAL_ENC_INTERVAL_COUNT
} HD_IFRAME_MANUAL_ENC_INTERVAL_EX_E;


/**
 * @enum tagHD_CameraPositionEx
 * @brief �������ת/��б����
 * @attention
 */
typedef  enum  tagHD_CameraPositionEx
{
    IMOS_EN_POS_UP      = 0,    /**< ����  */
    IMOS_EN_POS_DOWN    = 1,    /**< ����  */
    IMOS_EN_POS_LEFT    = 2,    /**< ����  */
    IMOS_EN_POS_RIGHT   = 3,    /**< ����  */
    IMOS_EN_POS_RESET   = 4,    /**< ����  */
    IMOS_EN_POS_STOP    = 5,    /**< ֹͣ */
    IMOS_EN_POS_COUNT
} HD_CAMERA_POSITION_EX_E;

/**
 * @enum tagHD_CameraZoomEx
 * @brief ͼ������ģʽ
 * @attention
 */
typedef  enum  tagHD_CameraZoomEx
{
    IMOS_EN_CAMERA_ZOOM_OUT     = 0,    /**< ��С  */
    IMOS_EN_CAMERA_ZOOM_IN      = 1,    /**< �Ŵ�  */
    IMOS_EN_CAMERA_ZOOM_STOP    = 2,    /**< ֹͣ */
    IMOS_EN_CAMERA_ZOOM_COUNT
} HD_CAMERA_ZOOM_EX_E;

/**
 * @enum tagHD_CameraFocusEx
 * @brief �۽�ģʽ
 * @attention
 */
typedef enum tagHD_CameraFocusEx
{
    IMOS_EN_CAMERA_FOCUS_AUTO   = 0,    /**< �Զ� */
    IMOS_EN_CAMERA_FOCUS_MANUAL = 1,    /**< �ֶ� */
    IMOS_EN_CAMERA_FOCUS_COUNT
} HD_CAMERA_FOCUS_EX_E;


/**
 * @enum tagMC_SiteStatusEx
 * @brief �᳡״̬
 * @attention
 */
typedef  enum tagMC_SiteStatusEx
{
    IMOS_EN_SITE_OFF_LINE       = 0,    /**< �᳡������ */
    IMOS_EN_SITE_CALLING_IN     = 1,    /**< ���ڱ�MCU���� */
    IMOS_EN_SITE_CALLING_OUT    = 2,    /**< ���ں���MCU */
    IMOS_EN_SITE_IN_CONF        = 3,    /**< �Ѿ��ڻ����� */
    IMOS_EN_SITE_DROPING        = 4,    /**< ���ڹҶϻ᳡ */
    IMOS_EN_SITE_DROPED         = 5,    /**< �᳡�Ѿ��˳����� */
    IMOS_EN_SITE_UNKNOW         = 6,    /**< �᳡״̬δ֪ */
    IMOS_EN_SITE_DELETING       = 7,    /**< ���ڱ�ɾ�� */
    IMOS_EN_SITE_DISABLE        = 8,    /**< �᳡�����ã����������ʹ�� */
    IMOS_EN_SITE_STATUS_COUNT
} MC_SITE_STATUS_EX_E;

/**
 * @enum tagHD_CapTypeEx
 * @brief �������е���������
 * @attention
 */
typedef  enum    tagHD_CapTypeEx
{
     IMOS_EN_HD_CAP_AUDIO   = 0,    /**< ��Ƶ���� */
     IMOS_EN_HD_CAP_VIDEO   = 1,    /**< ��Ƶ���� */
     IMOS_EN_HD_CAP_ANNEX   = 2,    /**< �������� */
     IMOS_EN_HD_CAP_DATA    = 3,    /**< �������� */
     IMOS_EN_HD_CAP_CTRL    = 4,    /**< �������� */
     IMOS_EN_HD_CAP_COUNT           /**< ���� */
} HD_CAP_TYPE_EX_E;

/**
 * @enum tagHD_DataProtEx
 * @brief ����Э������
 * @attention
 */
typedef  enum  tagHD_DataProtEx
{
    IMOS_EN_DATA_PROT_H224 = 0,                                    /**< H224Э�� */
    IMOS_EN_DATA_PROT_COUNT
} HD_DATA_PROT_EX_E;

/**
 * @enum tagHD_ControlProtEx
 * @brief ����Э������
 * @attention
 */
typedef  enum  tagHD_ControlProtEx
{
    IMOS_EN_CTRL_PROT_H239 = 0,                                   /**< H239Э�� */
    IMOS_EN_CTRL_PROT_LPR  = 1,                                   /**< �Ǳ�LPR */
    IMOS_EN_CTRL_PROT_COUNT
} HD_CTRL_PROT_EX_E;

/**
 * @enum tagHD_SysTimeZoneEx
 * @brief ʱ��
 * @attention
 */
typedef enum tagHD_SysTimeZoneEx
{
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_12 = 0,    /**< (GMT-12:00)�ս�����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_11 = 1,    /**< (GMT-11:00)��;��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_10 = 2,    /**< (GMT-10:00)������*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_9  = 3,    /**< (GMT-09:00)����˹��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_8  = 4,    /**< (GMT-08:00)̫ƽ��ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_7  = 5,    /**< (GMT-07:00)ɽ��ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_6  = 6,    /**< (GMT-06:00)�в�ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_5  = 7,    /**< (GMT-05:00)����ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_4  = 8,    /**< (GMT-04:00)������ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_3  = 9,    /**< (GMT-03:00)������*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_2  = 10,   /**< (GMT-02:00)�д�����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT_1  = 11,   /**< (GMT-01:00)��ý�Ⱥ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT    = 12,   /**< (GMT)�������α�׼ʱ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT1   = 13,   /**< (GMT+01:00)���֣�����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT2   = 14,   /**< (GMT+02:00)���ޣ��ŵ�*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT3   = 15,   /**< (GMT+03:00)Ī˹�ƣ��͸��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT4   = 16,   /**< (GMT+04:00)�Ϳ⣬������*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT5   = 17,   /**< (GMT+05:00)��˹����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT6   = 18,   /**< (GMT+06:00)����ľͼ*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT7   = 19,   /**< (GMT+07:00)���ȣ�����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT8   = 20,   /**< (GMT+08:00)���������� */
    IMOS_EN_HD_SYS_TIME_ZONE_GMT9   = 21,   /**< (GMT+09:00)�������׶�*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT10  = 22,   /**< (GMT+10:00)��������ī����*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT11  = 23,   /**< (GMT+11:00)������Ⱥ��*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT12  = 24,   /**< (GMT+12:00)�¿����������*/
    IMOS_EN_HD_SYS_TIME_ZONE_GMT13  = 25,   /**< (GMT+13:00)Ŭ�Ⱒ�巨*/
    IMOS_EN_HD_SYS_TIME_ZONE_COUNT
} HD_SYS_TIMEZONE_EX_E;

/**
 * @enum tagHD_SysLanguageEx
 * @brief ϵͳ��������
 * @attention
 */
typedef enum tagHD_SysLanguageEx
{
    IMOS_EN_LANGUAGE_SIMPLE_CHINESE         = 0,    /**< �������� */
    IMOS_EN_LANGUAGE_TRADITIONAL_CHINESE    = 1,    /**< �������� */
    IMOS_EN_LANGUAGE_ENGLISH                = 2,    /**< Ӣ�� */
    IMOS_EN_LANGUAGE_COUNT
} HD_SYS_LANGUAGE_EX_E;

/**
 * @enum tagHD_NetCardDuplexTypeEx
 * @brief ����˫��ģʽ
 * @attention
 */
typedef  enum  tagHD_NetCardDuplexTypeEx
{
    IMOS_EN_FULLDUPLEX          = 0,    /**< ȫ˫�� */
    IMOS_EN_SEMIDUPLEX          = 1,    /**< ��˫�� */
    IMOS_EN_DUPLEX_NEGOTIATE    = 2,    /**< ��Э�� */
    IMOS_EN_KEEP_DUPLEX_CFG     = 255   /**< ����ԭ������ */
} HD_NETCARD_DUPLEX_TYPE_EX_E;

/**
 * @enum tagHD_NetCardRateEx
 * @brief ����֧������
 * @attention
 */
typedef  enum  tagHD_NetCardRateEx
{
    IMOS_EN_CARD_RATE10M        = 0,    /**< 10M */
    IMOS_EN_CARD_RATE100M       = 1,    /**< 100M */
    IMOS_EN_CARD_RATE1000M      = 2,    /**< 1000M */
    IMOS_EN_CARD_RATE_NEGOTIATE = 3,    /**< ��Э�� */
    IMOS_EN_KEEP_CARD_RATE_CFG  = 255   /**< ����ԭ������ */
} HD_NETCARD_RATE_EX_E;

/**
 * @enum tagHD_BoardTypeEx
 * @brief �豸����
 * @attention
 */
typedef  enum tagHD_BoardTypeEx
{
    IMOS_EN_BOARD_MMC   = 0,    /**< ����MC����� */
    IMOS_EN_BOARD_BMC   = 1,    /**< ����MC����� */
    IMOS_EN_BOARD_LAN   = 2,    /**< �ӿڴ���� */
    IMOS_EN_BOARD_AUX   = 3,    /**< ��Ƶ����� */
    IMOS_EN_BOARD_VP    = 4,    /**< ��Ƶ����� */
    IMOS_EN_BOARD_TYPE_COUNT
} HD_BOARD_TYPE_EX_E;

/**
 * @enum tagHD_FanStateEx
 * @brief ����״̬
 * @attention
 */
typedef enum tagHD_FanStateEx
{
    IMOS_EN_FAN_STATE_ABSENT            = -1,   /**< ����λ */
    IMOS_EN_FAN_STATE_NORMAL            = 0,    /**< ���� */
    IMOS_EN_FAN_STATE_SPEED_ABNORMAL    = 1,    /**< ת���쳣 */
    IMOS_EN_FAN_STATE_BLOCKED           = 2,    /**< �������� */
    IMOS_EN_FAN_STATE_OUT_OF_CONTROL    = 3     /**< ʧȥ����--���� */
} HD_FAN_STATE_EX_E;

/**
 * @enum tagHD_DspStatusEnEx
 * @brief DSP״̬����ʶDSP�Ƿ����
 * @attention
 */
typedef  enum  tagHD_DspStatusEnEx
{
    IMOS_EN_DSP_STAT_IDLE   = 0,    /**< ����״̬  */
    IMOS_EN_DSP_STAT_USED   = 1,    /**< DSP��ʹ����  */
    IMOS_EN_DSP_STAT_ERROR  = 2,    /**< DSP����  */
    IMOS_EN_DSP_STAT_COUNT
} HD_DSP_STATUS_EX_E;

/**
 * @enum tagHD_ComBitRateEx
 * @brief ���ڴ��䲨����
 * @attention
 */
typedef  enum tagHD_ComBitRateEx
{
    IMOS_EN_COM_RATE_100BPS = 100,                                /**< 100kbps */
    IMOS_EN_COM_RATE_300BPS = 300,                                /**< 300kbps */
    IMOS_EN_COM_RATE_1200BPS = 1200,                              /**< 1200kbps */
    IMOS_EN_COM_RATE_2400BPS = 2400,                              /**< 2400kbps */
    IMOS_EN_COM_RATE_4800BPS = 4800,                              /**< 4800kbps */
    IMOS_EN_COM_RATE_9600BPS = 9600,                              /**< 960kbps */
    IMOS_EN_COM_RATE_19200BPS = 19200,                            /**< 19200kbps */
    IMOS_EN_COM_RATE_38400BPS = 38400,                            /**< 39400kbps */
    IMOS_EN_COM_RATE_57600BPS = 57600,                            /**< 57600kbps */
    IMOS_EN_COM_RATE_115200BPS = 115200,                          /**< 115200kbps */
    IMOS_EN_COM_RATE_230400BPS = 230400,                          /**< 230400kbps */
    IMOS_EN_COM_RATE_460800BPS = 460800,                          /**< 460800kbps */
    IMOS_EN_COM_RATE_921600BPS = 921600,                          /**< 921600kbps */
    IMOS_EN_COM_RATE_COUNT
} HD_COM_BIT_RATE_EX_E;

/**
 * @enum tagMC_TermTypeEx
 * @brief �ն�����Ƶʹ������
 * @attention
 */
typedef  enum tagMC_TermTypeEx
{
    IMOS_EN_TERM_AUDIO                      = 0,    /**< ����Ƶ�ն� */
    IMOS_EN_TERM_AUDIO_VIDEO                = 1,    /**< ����Ƶ�ն� */
    IMOS_EN_TERM_SECOND_AUDIO_VIDEO         = 2,    /**< ������Ƶ�ն� */
    IMOS_EN_TERN_VIRTUAL_SEND_VIDEO_ONLY    = 3,    /**< ֻ����Ƶ�������ն� */
    IMOS_EN_TERN_VIRTUAL_RECV_VIDEO_ONLY    = 4,    /**< ֻ������Ƶ�������ն� */
    IMOS_EN_TERM_TYPE_COUNT
} MC_TERM_TYPE_EX_E;

/**
 * @enum tagMC_CallTermTypeEx
 * @brief ��������ʱ��ǰ�ն˺�������
 * @attention
 */
typedef enum tagMC_CallTermTypeEx
{
    IMOS_EN_CALL_TERM_TYPE_TERM = 0,    /**< ���е��ն�����Ϊ�˵� */
    IMOS_EN_CALL_TERM_TYPE_MCU  = 1,    /**< ���е��ն�����ΪMCU */
    IMOS_EN_CALL_TERM_TYPE_COUNT
}MC_CALL_TERM_TYPE_EX_E;

/**
 * @enum tagHD_ChnDirectionEx
 * @brief ͨ������
 * @attention
 */
typedef  enum  tagHD_ChnDirectionEx
{
    IMOS_EN_CHN_IN  = 0,    /**< ����ͨ�� */
    IMOS_EN_CHN_OUT = 1,    /**< ���ͨ�� */
    IMOS_EN_DIRECT_COUNT
} HD_CHN_DIRECTION_EX_E;

/**
 * @enum tagMC_ConfTurnBroadcastModeEx
 * @brief ������Ѳ���㲥��Ѳģʽ
 * @attention
 */
typedef  enum tagMC_ConfTurnBroadcastModeEx
{
    IMOS_EN_TURN_MAIN_SITE = 0,     /**< ������Ѳ */
    IMOS_EN_TURN_BROADCAST = 1,     /**< �㲥��Ѳ */
    IMOS_EN_TURN_COUNT
} MC_CONF_TURN_BROADCAST_MODE_EX_E;

/**
 * @enum tagMC_GKTypeEx
 * @brief GK������
 * @attention
 */
typedef    enum    tagMC_GKTypeEx
{
    IMOS_EN_INNER_GK    = 0,    /**< ����GK */
    IMOS_EN_EXTER_GK    = 1,    /**< ����GK */
    IMOS_EN_NO_GK       = 2,    /**< ��ʹ��GK */
    IMOS_EN_GK_TYPE_COUNT,
} MC_GK_TYPE_EX_E;

/**
 * @enum tagMC_NtpTimeSynchIntervalEx
 * @brief NTPʱ��ͬ�����
 * @attention
 */
typedef  enum tagMC_NtpTimeSynchIntervalEx
{
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_10MIN = 10 * 60,                  /**< 10���� */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_15MIN = 15 * 60,                  /**< 15���� */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_30MIN = 30 * 60,                  /**< 30���� */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_1HOUR = 60 * 60,                  /**< 1Сʱ */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_2HOUR = 2 * 60 * 60,              /**< 2Сʱ */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_12HOUR = 12 * 60 * 60,            /**< 12Сʱ */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_24HOUR = 24 * 60 * 60,            /**< 24Сʱ */
    IMOS_EN_NTP_TIME_SYNCH_INTERVAL_COUNT
} MC_NTP_TIME_SYNCH_INTERVAL_EX_E;

#if 0
#endif

/*******************************************************************************
SS �ն��豸ʹ�õ�ö��
*******************************************************************************/

/**
 * @enum tagHD_AudioVolumeEx
 * @brief ����
 * @attention
 */
typedef  enum    tagHD_AudioVolumeEx
{
    IMOS_EN_AUDIO_VOLUME1   = 1,    /**< 1 */
    IMOS_EN_AUDIO_VOLUME2   = 2,    /**< 2 */
    IMOS_EN_AUDIO_VOLUME3   = 3,    /**< 3 */
    IMOS_EN_AUDIO_VOLUME4   = 4,    /**< 4 */
    IMOS_EN_AUDIO_VOLUME5   = 5,    /**< 5 */
    IMOS_EN_AUDIO_VOLUME6   = 6,    /**< 6 */
    IMOS_EN_AUDIO_VOLUME7   = 7,    /**< 7 */
    IMOS_EN_AUDIO_VOLUME_COUNT
} HD_AUDIO_VOLUME_EX_E;

/**
 * @enum tagHD_RingTypeEx
 * @brief �������
 * @attention
 */
typedef enum tagHD_RingTypeEx
{
    IMOS_EN_RING_TYPE_BOOT_CALL1            = 0,    /**< ��������1 */
    IMOS_EN_RING_TYPE_BOOT_CALL2            = 1,    /**< ��������2 */
    IMOS_EN_RING_TYPE_BOOT_CALL3            = 2,    /**< ��������3 */
    IMOS_EN_RING_TYPE_BOOT_CALL4            = 3,    /**< ��������4 */
    IMOS_EN_RING_TYPE_BOOT_CALL5            = 4,    /**< ��������5 */
    IMOS_EN_RING_TYPE_BOOT_DEFAULT          = 5,    /**< Ĭ�ϵĿ������� */
    IMOS_EN_RING_TYPE_BOOT_FASHION          = 6,    /**< ʱ�еĿ������� */
    IMOS_EN_RING_TYPE_BOOT_GRACE            = 7,    /**< ������ŵĿ������� */
    IMOS_EN_RING_TYPE_INCOMING_CALL1        = 8,    /**< ��������1 */
    IMOS_EN_RING_TYPE_INCOMING_CALL2        = 9,    /**< ��������2 */
    IMOS_EN_RING_TYPE_INCOMING_CALL3        = 10,   /**< ��������3 */
    IMOS_EN_RING_TYPE_INCOMING_CALL4        = 11,   /**< ��������4 */
    IMOS_EN_RING_TYPE_INCOMING_CALL5        = 12,   /**< ��������5 */
    IMOS_EN_RING_TYPE_INCOMING_CALL_DEFAULT = 13,   /**< Ĭ�ϵ��������� */
    IMOS_EN_RING_TYPE_INCOMING_CALL_FASHION = 14,   /**< ʱ�е��������� */
    IMOS_EN_RING_TYPE_INCOMING_CALL_GRACE   = 15,   /**< ������ŵ��������� */
    IMOS_EN_RING_TYPE_CUSTOM1               = 16,   /**< �Զ�������1��������������������ѡ�� */
    IMOS_EN_RING_TYPE_CUSTOM2               = 17,   /**< �Զ�������2��������������������ѡ�� */
    IMOS_EN_RING_TYPE_CUSTOM3               = 18,   /**< �Զ�������3��������������������ѡ�� */
    IMOS_EN_RING_TYPE_CUSTOM4               = 19,   /**< �Զ�������4��������������������ѡ�� */
    IMOS_EN_RING_TYPE_CUSTOM5               = 20,   /**< �Զ�������5��������������������ѡ�� */
    IMOS_EN_RING_TYPE_COUNT                         /**< ���� */
}HD_RING_TYPE_EX_E;


/**
 * @enum tagHD_NIC_NumberEx
 * @brief ���ں�
 * @attention
 */
typedef  enum  tagHD_NIC_NumberEx
{
    IMOS_EN_HD_NIC_INVALID = -1,
    IMOS_EN_HD_NIC_0 = 0,                                          /**< ����0 ֵ���ɸı�(���������±�, ������������ֵ����ת��ֱ���ô�) */
    IMOS_EN_HD_NIC_1 = 1,                                          /**< ����1 ֵ���ɸı�(���������±�) */
    IMOS_EN_HD_NIC_COUNT
}HD_NIC_NUMBER_EX_E;

/**
 * @enum tagHD_Framerate_TypeEx
 * @brief ��Ƶ֡������
 * @attention
 */
typedef enum tagHD_Framerate_TypeEx
{
    IMOS_EN_AUTO_FRAMERATE      = 0,    /**< �Զ� */
    IMOS_EN_CLEAR_FRAMERATE     = 1,    /**< ���� */
    IMOS_EN_FLUENT_FRAMERATE    = 2,    /**< ���� */
    IMOS_EN_FRAMERATE_TYPE_COUNT,
}HD_FRAMERATE_TYPE_EX_E;


/**
 * @enum tagHD_VideoInPortEx
 * @brief ��Ƶ����˿�
 * @attention
 */
typedef enum tagHD_VideoInPortEx
{
    IMOS_EN_VIDEO_IN_PORT_DVI_I1    = 0,    /**< DVI1  */
    IMOS_EN_VIDEO_IN_PORT_S1        = 1,    /**< DVI1-S1 */
    IMOS_EN_VIDEO_IN_PORT_DVI_I2    = 2,    /**< DVI2  */
    IMOS_EN_VIDEO_IN_PORT_S2        = 3,    /**< DVI2-S2  */
    IMOS_EN_VIDEO_IN_PORT_DVI_I3    = 4,    /**< DVI3  */
    IMOS_EN_VIDEO_IN_PORT_SDI1      = 5,    /**< SDI1 */
    IMOS_EN_VIDEO_IN_PORT_SDI2      = 6,    /**< SDI2  */
    IMOS_EN_VIDEO_IN_PORT_COUNT             /**< ���� */
} HD_VIDEO_IN_PORT_EX_E;


/**
 * @enum tagHD_AECTypeEx
 * @brief MIC �� RCA ������������
 * @attention
 */
typedef enum tagHD_AECTypeEx
{
    IMOS_EN_AEC_TYPE_STRONG = 0,    /**< ǿ */
    IMOS_EN_AEC_TYPE_WEAK   = 1,    /**< �� */
    IMOS_EN_AEC_TYPE_NONE   = 2,    /**< �� */
    IMOS_EN_AEC_TYPE_COUNT          /**< ���� */
}HD_AEC_TYPE_EX_E;


/**
 * @enum tagHD_TNAAType
 * @brief �����������
 * @attention
 */
typedef enum tagHD_TNAAType
{
    IMOS_NAA_TYPE_SHORT_DELAY   = 0,    /** ��������ʱ*/
    IMOS_NAA_TYPE_MID_DELAY     = 1,    /** �е�������ʱ*/
    IMOS_NAA_TYPE_LONG_DELAY    = 2,    /** ��������ʱ*/
    IMOS_NAA_TYPE_DELAY_COUNT
}HD_TNAA_TYPE_EX_E;


/**
 * @enum tagHD_DebugLevelEx
 * @brief ���Լ���
 * @attention
 */
typedef  enum  tagHD_DebugLevelEx
{
    IMOS_EN_HD_DEBUG    = 0,    /**< Debug��������Ϣ,�������е�����Ϣ */
    IMOS_EN_HD_INFO     = 1,    /**< Info��������Ϣ,�������漶������Ϣ */
    IMOS_EN_HD_WARNING  = 2,    /**< ���漶������Ϣ,�������󼶵�����Ϣ */
    IMOS_EN_HD_ERROR    = 3,    /**< ���󼶵�����Ϣ,�����������������Ϣ */
    IMOS_EN_HD_FATAL    = 4,    /**< �������󼶵�����Ϣ */
    IMOS_EN_HD_NO       = 5,    /**< �����������Ϣ */
    IMOS_EN_HD_DEBUG_LEVEL_COUNT    /**< ��ö�����͵ĸ��� */
} HD_DEBUG_LEVEL_EX_E;


/**
 * @enum tagHD_LogoTypeEx
 * @brief ʹ��Logo ����
 * @attention
 */
typedef enum tagHD_LogoTypeEx
{
    IMOS_EN_LOGO_DEFAULT    = 0,    /**< Ĭ��LOGO */
    IMOS_EN_LOGO_TYPE1      = 1,    /**< ���������1 */
    IMOS_EN_LOGO_TYPE2      = 2,    /**< ���������2 */
    IMOS_EN_LOGO_TYPE3      = 3,    /**< ���������3 */
    IMOS_EN_LOGO_TYPE4      = 4,    /**< ���������4 */
    IMOS_EN_LOGO_TYPE5      = 5,    /**< ���������5 */
    IMOS_EN_LOGO_TYPE6      = 6,    /**< ���������6 */
    IMOS_EN_LOGO_TYPE7      = 7,    /**< ���������7 */
    IMOS_EN_LOGO_TYPE8      = 8,    /**< ���������8 */
    IMOS_EN_LOGO_TYPE9      = 9,    /**< ���������9 */
    IMOS_EN_LOGO_TYPE10     = 10,   /**< ���������10 */
    IMOS_EN_LOGO_COUNT              /**< ���� */
}HD_LOGO_TYPE_EX_E;


/**
 * @enum tagHD_QosTypeEx
 * @brief QOS�������IP Precedence��Diffserv
 * @attention
 */
typedef  enum  tagHD_QosTypeEx
{
    IMOS_EN_IP_PRECEDENCE = 0,                                   /**< IP ���ȼ��� */
    IMOS_EN_DIFF_SERV     = 1,                                   /**< Diffserv */
    IMOS_EN_SERVER_COUNT
} HD_QOS_TYPE_EX_E;


/**
 * @enum tagHD_UserTypeEx
 * @brief �û�����
 * @attention
 */
typedef enum tagHD_UserTypeEx
{
    IMOS_EN_USER_TYPE_TERMINAL_ADMIN    = 0,    /**< �ն˹���Ա */
    IMOS_EN_USER_TYPE_WEB_ADMIN         = 1,    /**< WEB����Ա  */
    IMOS_EN_USER_TYPE_PC_ADMIN          = 2,    /**< ����̨����Ա */
    IMOS_EN_USER_TYPE_COUNT                     /**< ���� */
}HD_USER_TYPE_EX_E;


/**
 * @enum tagHD_PwdEncTypeEx
 * @brief ������ܷ���
 * @attention
 */
typedef enum tagHD_PwdEncTypeEx
{
    IMOS_EN_PWD_ENC_NONE    = 0,    /**< �����ܣ����� */
    IMOS_EN_PWD_ENC_MD5     = 1,    /**< MD5����  */
    IMOS_EN_PWD_ENC_DES     = 2,    /**< DES���� */
    IMOS_EN_PWD_ENC_TYPE_COUNT
}HD_PWD_ENC_TYPE_EX_E;


/**
 * @enum tagHD_ConnectTypeEx
 * @brief �������ӷ�ʽ
 * @attention
 */
typedef enum tagHD_ConnectTypeEx
{
    IMOS_EN_CONNECT_TYPE_FIXEDIP    = 0,    /**< �̶�IP */
    IMOS_EN_CONNECT_TYPE_PPPOE      = 1,    /**< PPPoE��ʽ */
    IMOS_EN_CONNECT_TYPE_DHCP       = 2,    /**< �Զ����IP */
    IMOS_EN_CONNECT_TYPE_COUNT              /**< ���� */
}HD_CONNECT_TYPE_EX_E;


/**
 * @enum tagHD_BitRateEx
 * @brief ����
 * @attention
 */
typedef  enum tagHD_BitRateEx
{
    IMOS_EN_BIT_RATE_AUTO = 0,                      /**< �Զ� */
    IMOS_EN_BIT_RATE_64K  = 64 * 1000,              /**< 64kbps[64K] */
    IMOS_EN_BIT_RATE_128K = 128 * 1000,             /**< 128kbps[128K] */
    IMOS_EN_BIT_RATE_256K = 256 * 1000,             /**< 256kbps[256K] */
    IMOS_EN_BIT_RATE_320K = 320 * 1000,             /**< 320kbps[320K] */
    IMOS_EN_BIT_RATE_384K = 384 * 1000,             /**< 384kbps[384K] */
    IMOS_EN_BIT_RATE_512K = 512 * 1000,             /**< 512kbps[512K] */
    IMOS_EN_BIT_RATE_640K = 640 * 1000,             /**< 640kbps[640K] */
    IMOS_EN_BIT_RATE_768K = 768 * 1000,             /**< 768kbps[768K] */
    IMOS_EN_BIT_RATE_1M = 1024 * 1000,              /**< 1Mbps[1024K] */
    IMOS_EN_BIT_RATE_1280K = 1280 * 1000,           /**< 1280Kbps[1280K] */
    IMOS_EN_BIT_RATE_1536K = 1536 * 1000,           /**< 1536Kbps[1536K] */
    IMOS_EN_BIT_RATE_1920K = 1920 * 1000,           /**< 1920Kbps[1920K] */
    IMOS_EN_BIT_RATE_2M = 2 * 1024 * 1000,          /**< 2Mbps[2048K] */
    IMOS_EN_BIT_RATE_2560K = 2560 * 1000,           /**< 2560Kbps[2560K] */
    IMOS_EN_BIT_RATE_3M = 3 * 1024 * 1000,          /**< 3Mbps[3072K] */
    IMOS_EN_BIT_RATE_3584K = 3584 * 1000,           /**< 3584Kbps[3584K] */
    IMOS_EN_BIT_RATE_4M = 4 * 1024 * 1000,          /**< 4Mbps[4096K] */
    IMOS_EN_BIT_RATE_4608K = 4608 * 1000,           /**< 4608Kbps[4608K] */
    IMOS_EN_BIT_RATE_5M = 5 * 1024 * 1000,          /**< 5Mbps[5120K] */
    IMOS_EN_BIT_RATE_5632K = 5632 * 1000,           /**< 5632Kbps[5632K] */
    IMOS_EN_BIT_RATE_6M = 6 * 1024 * 1000,          /**< 6Mbps[6144K] */
    IMOS_EN_BIT_RATE_6656K = 6656 * 1000,           /**< 6656Kbps[6656K] */
    IMOS_EN_BIT_RATE_7M = 7 * 1024 * 1000,          /**< 7Mbps[7168K] */
    IMOS_EN_BIT_RATE_7680K = 7680 * 1000,           /**< 7680Kbps[7680K] */
    IMOS_EN_BIT_RATE_8M = 8 * 1024 * 1000,          /**< 8Mbps[8192K] */
    IMOS_EN_BIT_RATE_12M = 12 * 1024 * 1000,        /**< 12Mbps[12288K] */
    IMOS_EN_BIT_RATE_16M = 16 * 1024 * 1000,        /**< 16Mbps[16384K] */
    IMOS_EN_BIT_RATE_32M = 32 * 1024 * 1000,        /**< 32Mbps[32768K] */
    IMOS_EN_BIT_RATE_COUNT
} HD_BITRATE_EX_E;



/**
 * @enum tagHD_NetCardSetTypeEx
 * @brief �������÷�ʽ
 * @attention
 */
typedef  enum  tagHD_NetCardSetTypeEx
{
    IMOS_EN_CARD_SET_BY_MANUAL  = 0,    /**< �ֶ��������� */
    IMOS_EN_CARD_SET_BY_AUTO    = 1,    /**< ��Э�� */
    IMOS_EN_CARD_SET_COUNT,
} HD_NETCARD_SET_TYPE_EX_E;


/**
 * @enum tagHD_VideoOutPortEx
 * @brief ��Ƶ����˿�
 * @attention
 */
typedef enum tagHD_VideoOutPortEx
{
    IMOS_EN_VIDEO_OUT_PORT_DVI_I1   = 0,        /**< DVI1  */
    IMOS_EN_VIDEO_OUT_PORT_S1       = 1,        /**< DVI1-S1  */
    IMOS_EN_VIDEO_OUT_PORT_DVI_I2   = 2,        /**< DVI2  */
    IMOS_EN_VIDEO_OUT_PORT_S2       = 3,        /**< DVI2-S2  */
    IMOS_EN_VIDEO_OUT_PORT_DVI_I3   = 4,        /**< DVI3  */
    IMOS_EN_VIDEO_OUT_PORT_S3       = 5,        /**< DVI3-S3  */
    IMOS_EN_VIDEO_OUT_PORT_COUNT                /**< ���� */
} HD_VIDEO_OUT_PORT_EX_E;

/**
 * @enum tagHD_VideoOutContentTypeEx
 * @brief �������
 * @attention
 */
typedef enum tagHD_VideoOutContentTypeEx
{
    IMOS_EN_VIDEO_OUT_CONTENT_WITH_GUI      = 0,    /**< ��GUIͼ������   */
    IMOS_EN_VIDEO_OUT_CONTENT_LOCAL_MAIN    = 1,    /**< �������� */
    IMOS_EN_VIDEO_OUT_CONTENT_LOCAL_ROLE    = 2,    /**< ���ظ��� */
    IMOS_EN_VIDEO_OUT_CONTENT_REMOTE_MAIN   = 3,    /**< Զ������ */
    IMOS_EN_VIDEO_OUT_CONTENT_REMOTE_ROLE   = 4,    /**< Զ�˸���  */
    IMOS_EN_VIDEO_OUT_CONTENT_WITH_FIRST    = 5,    /**< ���һ·��ͬ  */
    IMOS_EN_VIDEO_OUT_CONTENT_COUNT                 /**< ���� */
} HD_VIDEO_OUT_CONTENT_TYPE_EX_E;


/**
 * @enum tagHD_Audio_In_Port_Sub_TypeEx
 * @brief ��Ƶ����˿ڵľ�������
 * @attention
 */
typedef enum tagHD_Audio_In_Port_Sub_TypeEx
{
    IMOS_EN_AUDIO_IN_PORT_SUB_TYPE_CLEARONEMIC  = 0,    /**< clearone MIC */
    IMOS_EN_AUDIO_IN_PORT_SUB_TYPE_OTHERS       = 1,    /**< ���� */
    IMOS_EN_AUDIO_IN_PORT_SUB_TYPE_COUNT                /**< ���� */
} HD_AUDIO_IN_PORT_SUB_TYPE_EX_E;


/**
 * @enum tagHD_Record_Storage_TypeEx
 * @brief ¼���洢����
 * @attention
 */
typedef enum tagHD_Record_Storage_TypeEx
{
   IMOS_EN_HD_RECORD_2PORT  = 0,    /**< ¼�����ӿ���� */
   IMOS_EN_HD_RECORD_2USB   = 1,    /**< ¼���洢��U�� */
   IMOS_EN_HD_RECORD_2DISK  = 2,    /**< ¼���洢��Ӳ��*/
   IMOS_EN_HD_RECORD_STORAGE_COUNT
} HD_RECORD_STORAGE_TYPE_EX_E;


/**
 * @enum tagHD_AudioOutPortEx
 * @brief ��Ƶ�����
 * @attention
 */
typedef enum tagHD_AudioOutPortEx
{
    IMOS_EN_AUDIO_OUT_PORT_PHOENIX1 = 0,    /**< ��˿� */
    IMOS_EN_AUDIO_OUT_PORT_RCA1     = 1,    /**< RCA1 */
    IMOS_EN_AUDIO_OUT_PORT_DVI_I1   = 2,    /**< DVI1 */
    IMOS_EN_AUDIO_OUT_PORT_DVI_I2   = 3,    /**< DVI2 */
    IMOS_EN_AUDIO_OUT_PORT_DVI_I3   = 4,    /**< DVI3 */
    IMOS_EN_AUDIO_OUT_PORT_COUNT             /**< ��Ƶ����˿���Ŀ */
} HD_AUDIO_OUT_PORT_EX_E;


/**
 * @enum tagHD_AudioInPortEx
 * @brief ��Ƶ����ӿ�
 * @attention
 */
typedef  enum    tagHD_AudioInPortEx
{
    IMOS_EN_AUDIO_IN_PORT_MIC1      = 0,
    IMOS_EN_AUDIO_IN_PORT_MIC2      = 1,
    IMOS_EN_AUDIO_IN_PORT_PHOENIX1  = 2,
    IMOS_EN_AUDIO_IN_PORT_RCA1      = 3,
    IMOS_EN_AUDIO_IN_PORT_RCA2      = 4,
    IMOS_EN_AUDIO_IN_PORT_DVI_I1    = 5,
    IMOS_EN_AUDIO_IN_PORT_DVI_I2    = 6,
    IMOS_EN_AUDIO_IN_PORT_DVI_I3    = 7,
    IMOS_EN_AUDIO_IN_PORT_COUNT
} HD_AUDIO_IN_PORT_EX_E;


/**
 * @enum tagHD_ShowSiteNameEx
 * @brief ��ʾ�᳡��ģʽ,������Ϊ��ʾ����Ԥ��Ļ᳡��ȴ
 *        û��Ԥ��û᳡�Ļ᳡��ʱ��ʾ�᳡�Դ��Ļ᳡����
 * @attention
 */
typedef  enum tagHD_ShowSiteNameEx
{
    IMOS_EN_SHOW_SITE_OWNER = 0,                                   /**< ��ʾ�᳡�Դ��Ļ᳡�� */
    IMOS_EN_SHOW_SET_NAME   = 1,                                   /**< ��ʾԤ��Ļ᳡�� */
    IMOS_EN_SHOW_NO         = 2,                                   /**< ����ʾ�᳡�� */
    IMOS_EN_SHOW_SITE_COUNT
} HD_CONF_SHOW_SITE_NAME_EX_E;

/**
 * @enum tagHD_FontEx
 * @brief �᳡�������塢��Ļ���塢�������
 * @attention
 */
typedef  enum tagHD_FontEx
{
     IMOS_EN_FONT_HEITI = 0,                                       /**< ���� */
     IMOS_EN_FONT_YOUYUAN = 1,                                     /**< ��Բ */
     IMOS_EN_FONT_COUNT
} HD_FONT_EX_E;


/**
 * @enum tagHD_FontSizeEx
 * @brief �᳡���������С����Ļ�����С����������С
 * @attention
 */
typedef  enum tagHD_FontSizeEx
{
     IMOS_EN_FONT_SMALL = 0,                                       /**< С���� */
     IMOS_EN_FONT_MIDDLE = 1,                                      /**< ������ [��������] */
     IMOS_EN_FONT_LARGE = 2,                                       /**< ������ [������] */
     IMOS_EN_FONT_SIZE_COUNT
} HD_FONT_SIZE_EX_E;


/**
 * @enum tagHD_SiteNameTimerPosEx
 * @brief �᳡���ơ�����ʱ����ʾ��λ��
 * @attention
 */
typedef  enum tagHD_SiteNameTimerPosEx
{
    IMOS_EN_SITE_NAME_TIMER_POS_RIGHT_TOP       = 0,    /**< �᳡���ơ�ʱ�������ϲ�[����] */
    IMOS_EN_SITE_NAME_TIMER_POS_RIGHT_BOTTOM    = 1,    /**< �᳡���ơ�ʱ�������²�[����] */
    IMOS_EN_SITE_NAME_TIMER_POS_LEFT_TOP        = 2,    /**< �᳡���ơ�ʱ�������ϲ�[����] */
    IMOS_EN_SITE_NAME_TIMER_POS_LEFT_BOTTOM     = 3,    /**< �᳡���ơ�ʱ�������²�[����] */
    IMOS_EN_SITE_NAME_TIMER_POS_COUNT
} HD_SITE_NAME_TIMER_POS_EX_E;


/**
 * @enum tagHD_CameraTypeEx
 * @brief ���������
 * @attention
 */
typedef enum tagHD_CameraTypeEx
{
    IMOS_EN_CAM_SONY_D70    = 0,    /**< SONY_D70 */
    IMOS_EN_CAM_SONY_D100   = 1,    /**< SONY_D100 */
    IMOS_EN_CAM_SONY_EVIHD1 = 2,    /**< EVIHD1 */
    IMOS_EN_CAM_TYPE_COUNT          /**< ���� */
}HD_CAMERA_TYPE_EX_E;

/**
 * @enum tagHD_CameraSpeedEx
 * @brief �����ת���ٶ�
 * @attention
 */
typedef enum tagHD_CameraSpeedEx
{
    IMOS_EN_CAM_SPEED_LOW       = 0,        /**< ���� */
    IMOS_EN_CAM_SPEED_MIDDLE    = 1,        /**< ���� */
    IMOS_EN_CAM_SPEED_HIGH      = 2,        /**< ���� */
    IMOS_EN_CAM_SPEED_COUNT                 /**< ���� */
}HD_CAMERA_SPEED_EX_E;


/**
 * @enum tagHD_CameraPortEx
 * @brief ������˿�
 * @attention
 */
typedef enum tagHD_CameraPortEx
{
    IMOS_EN_CAM_PORT_NONE       = 0,    /**< �޶˿� */
    IMOS_EN_CAM_PORT_RS232_1    = 1,    /**< ����һ */
    IMOS_EN_CAM_PORT_RS232_2    = 2,    /**< ���ڶ� */
    IMOS_EN_CAM_PORT_COUNT
}HD_CAMERA_PORT_EX_E;

/**
 * @enum tagHD_VideoPortTypeEx
 * @brief �˿�����
 * @attention
 */
typedef enum tagHD_VideoPortTypeEx
{
    IMOS_EN_HD_VIDEO_PORT_DVI_I = 0,    /**< DVI  */
    IMOS_EN_HD_VIDEO_PORT_S     = 1,    /**< S���� */
    IMOS_EN_HD_VIDEO_PORT_RCA   = 2,    /**< RCA  */
    IMOS_EN_HD_VIDEO_PORT_SDI   = 3,    /**< SDI  */
    IMOS_EN_HD_VIDEO_PORT_COUNT         /**< ���� */
} HD_VIDEO_PORT_TYPE_EX_E;

/**
 * @enum tagHD_IPPrecedenceTypeEx
 * @brief IP���������Сʱ�ӡ��������������߿ɿ��ԡ���С����
 * @attention
 */
typedef  enum  tagHD_IPPrecedenceTypeEx
{
    IMOS_EN_PRECEDENCE_MIN_DELAY       = 0x10,                  /**< ��Сʱ�� */
    IMOS_EN_PRECEDENCE_MAX_THROUGHPUT  = 0x08,                  /**< ��������� */
    IMOS_EN_PRECEDENCE_MAX_RELIABILITY = 0x04,                  /**< ��߿ɿ��� */
    IMOS_EN_PRECEDENCE_MIN_MONETARY    = 0x02,                  /**< ��С���� */
    IMOS_EN_PRECEDENCE_NORMAL          = 0                      /**< ��ͨ */
} HD_IP_PRECEDENCE_TYPE_EX_E;



/**
 * @enum tagHD_Process_TypeEx
 * @brief ��������
 * @attention
 */
typedef enum tagHD_Process_TypeEx
{
    IMOS_EN_HD_PROCESS_TYPE_GUI = 0,
    IMOS_EN_HD_PROCESS_TYPE_UM  = 1,
    IMOS_EN_HD_PROCESS_TYPE_MCM = 2,
    IMOS_EN_HD_PROCESS_TYPE_DBM = 3,
    IMOS_EN_HD_PROCESS_TYPE_H323 = 4,
    IMOS_EN_HD_PROCESS_TYPE_VP  = 5,
    IMOS_EN_HD_PROCESS_TYPE_AP  = 6,
    IMOS_EN_HD_PROCESS_TYPE_COUNT                          /**< ���� */
}HD_PROCESS_TYPE_EX_E;

/**
 * @enum tagHD_VideoPortDVITypeEx
 * @brief DVI����Ƶ����
 * @attention
 */
typedef enum tagHD_VideoPortDVITypeEx
{
    IMOS_EN_HD_VIDEO_PORT_DVI_RGBHV     = 0,    /**< RGBHV������D-SUB,XGA */
    IMOS_EN_HD_VIDEO_PORT_DVI_HDMIDVI   = 1,    /**< HDMI DVI ��������  */
    /**< IMOS_EN_HD_VIDEO_PORT_DVI_D,                                    D  */
    IMOS_EN_HD_VIDEO_PORT_DVI_YPrPb     = 2,    /**< YPrPb  */
    IMOS_EN_HD_VIDEO_PORT_DVI_RGB       = 3,    /**< RGB  */
    IMOS_EN_HD_VIDEO_PORT_DVI_COUNT             /**< ���� */
} HD_VIDEO_PORT_DVI_TYPE_EX_E;


/**
 * @enum tagHD_VideoFrequenceEx
 * @brief ��ƵƵ������
 * @attention
 */
typedef enum tagHD_VideoFrequenceEx
{
    IMOS_EN_HD_VIDEO_FREQUENCE_25HZ = 0,    /**< 25HZ */
    IMOS_EN_HD_VIDEO_FREQUENCE_30HZ = 1,    /**< 30HZ */
    IMOS_EN_HD_VIDEO_FREQUENCE_60HZ = 2,    /**< 60HZ */
    IMOS_EN_HD_VIDEO_FREQUENCE_75HZ = 3,    /**< 75HZ */
    IMOS_EN_HD_VIDEO_FREQUENCE_85HZ = 4,    /**< 85HZ */
    IMOS_EN_HD_VIDEO_FREQUENCE_COUNT        /**< ���� */
} HD_VIDEO_FREQUENCE_TYPE_EX_E;


/**
 * @enum tagHD_Restrict_TypeEx
 * @brief PAL��NTSC��ʽ
 * @attention
 */
typedef enum tagHD_Restrict_TypeEx
{
    IMOS_EN_PAL_TYPE    = 0,    /**< PAL��ʽ */
    IMOS_EN_NTSC_TYPE   = 1,    /**< NTSC��ʽ */
    IMOS_EN_RESTRICT_COUNT,
}HD_RESTRICT_TYPE_EX_E;

/**
 * @enum tagHD_ProtoTypeEx
 * @brief Э������
 * @attention
 */
typedef enum tagHD_ProtoTypeEx
{
    IMOS_EN_PROTO_TYPE_H323 = 0,    /**< H323Э�� */
    IMOS_EN_PROTO_TYPE_SIP  = 1,    /**< sipЭ��  */
    IMOS_EN_PROTO_TYPE_COUNT        /**< ���� */
}HD_PROTO_TYPE_EX_E;


/**
 * @enum tagHD_VideoTypeEx
 * @brief ����˿ڶ�Ӧ������������
 * @attention
 */
typedef  enum  tagHD_VideoTypeEx
{
    IMOS_EN_MV_PORT_1   = 0,    /**< ������ƵԴ1 */
    IMOS_EN_MV_PORT_2   = 1,    /**< ������ƵԴ2 */
    IMOS_EN_RV_PORT     = 2,    /**< ������ƵԴ */
    IMOS_EN_VIDEO_TYPE_COUNT
} HD_VIDEO_TYPE_EX_E;



/**
 * @enum tagModule_TypeEx
 * @brief �ն�ģ������
 * @attention
 */
typedef enum tagModule_TypeEx
{
    IMOS_EN_HD_MODULE_GUI   = 0,    /**< GUI (��ֵ���ɸı䣬���������±�)*/
    IMOS_EN_HD_MODULE_UM    = 1,    /**< UM */
    IMOS_EN_HD_MODULE_MCM   = 2,    /**< MCM */
    IMOS_EN_HD_MODULE_DBM   = 3,    /**< DBM */
    IMOS_EN_HD_MODULE_STK   = 4,    /**< STK */
    IMOS_EN_HD_MODULE_VP    = 5,    /**< VP */
    IMOS_EN_HD_MODULE_AP    = 6,    /**< AP */
    IMOS_EN_HD_MODULE_OSD   = 7,    /**< OSD */
    IMOS_EN_HD_MODULE_TYPE_COUNT
}HD_MODULE_TYPE_EX_E;



/**
 * @enum tagHD_LogTypeEx
 * @brief ��־����
 * @attention
 */
typedef  enum  tagHD_LogTypeEx
{
    IMOS_EN_HD_SYS_TYPE     = 0,    /**< ϵͳ��־ */
    IMOS_EN_HD_USER_TYPE    = 1,    /**< �û���־ */
    IMOS_EN_HD_CONF_TYPE    = 2,    /**< ������־ */
    IMOS_EN_HD_DEBUG_TYPE   = 3,    /**< ������־(�ն˽��治��ʾ) */
    IMOS_EN_HD_LOG_TYPE_COUNT       /**< ��ö�����͵ĸ��� */
} HD_LOG_TYPE_EX_E;


/**
 * @enum tagHD_VERSION_INDEXEx
 * @brief �汾�������
 * @attention
 */
typedef enum tagHD_VERSION_INDEXEx
{
    IMOS_EN_SOFT_OLD_VER    = 0,
    IMOS_EN_SOFT_NEW_VER    = 1,
    IMOS_EN_MG_SOFT_VER     = 2,
    IMOS_EN_JRE_OLD_VER     = 3,
    IMOS_EN_JRE_NEW_VER     = 4,
    IMOS_EN_MAX_VERSION_COUNT
} HD_VERSION_INDEX_EX_E;


/**
 * @enum tagBAK_HostStateEx
 * @brief ��ǰBAK���̵�״̬��������������������δȷ��
 * @attention
 */
typedef enum tagBAK_HostStateEx
{
    IMOS_EN_BAK_STATE_ALONE     = 0,    /**< ���� */
    IMOS_EN_BAK_STATE_MAIN      = 1,    /**< ���� */
    IMOS_EN_BAK_STATE_BACK      = 2,    /**< ���� */
    IMOS_EN_BAK_STATE_UNSURE    = 3,    /**< δȷ�� */

    IMOS_EN_BAK_SWITCHING_ALONE = 4,    /**< �����е��� */
    IMOS_EN_BAK_SWITCHING_MAIN  = 5,    /**< ���������� */
    IMOS_EN_BAK_SWITCHING_BACK  = 6,    /**< �����б��� */

    IMOS_EN_BAK_STATE_COUNT
}BAK_STATE_EX_E;


/**
 * @enum tagMC_LinkStatusEx
 * @brief ��·״̬ [����״̬���л�ʹ��]
 * @attention
 */
typedef enum tagMC_LinkStatusEx
{
    /** ������·�ߴ����ȶ� ������·UP */
    IMOS_EN_LINK_STATUS_MAIN_HB_BAK_UP      = 0,

    /** ������·�ʹ����ȶ� ������·UP */
    IMOS_EN_LINK_STATUS_MAIN_LB_BAK_UP      = 1,

    /** ������·�ʹ����� ������·UP */
    IMOS_EN_LINK_STATUS_MAIN_LP_BAK_UP      = 2,

    /** ������·DOWN ������·UP */
    IMOS_EN_LINK_STATUS_MAIN_DOWN_BAK_UP    = 3,

    /** ������·�ߴ����ȶ� ������·DOWN */
    IMOS_EN_LINK_STATUS_MAIN_HB_BAK_DOWN    = 4,

    /** ������·�ʹ����ȶ� ������·DOWN */
    IMOS_EN_LINK_STATUS_MAIN_LB_BAK_DOWN    = 5,

    /** ������·�ʹ����� ������·DOWN */
    IMOS_EN_LINK_STATUS_MAIN_LP_BAK_DOWN    = 6,

    /** ������·DOWN ������·DOWN */
    IMOS_EN_LINK_STATUS_MAIN_DOWN_BAK_DOWN  = 7,
    IMOS_EN_LINK_STATUS_COUNT
} MC_LINK_STATUS_EX_E;

/**
 * @enum tagMC_LinkStatusChgStepEx
 * @brief ��·״̬�л����� [����״̬���л�ʹ��]
 * @attention
 */
typedef enum tagMC_LinkStatusChgStepEx
{
    /** ����BFD */
    IMOS_EN_CHG_STEP_UPDATE_BFD_STATUS      = 0,

    /** ɾ��������·BFD */
    IMOS_EN_CHG_STEP_DELETE_MAIN_BFD        = 1,

    /** ɾ��������·BFD */
    IMOS_EN_CHG_STEP_DELETE_BAK_BFD         = 2,

    /** ����������·BFD */
    IMOS_EN_CHG_STEP_CREATE_MAIN_BFD        = 3,

    /** ����������·BFD */
    IMOS_EN_CHG_STEP_CREATE_BAK_BFD         = 4,

    /** �л�·����������· */
    IMOS_EN_CHG_STEP_CHG_ROUTE2MAIN         = 5,

    /** �л�·����������· */
    IMOS_EN_CHG_STEP_CHG_ROUTE2BAK          = 6,

    /** ����request mode */
    IMOS_EN_CHG_STEP_SEND_REQMODE           = 7,

    /** �޸�VP�������� */
    IMOS_EN_CHG_STEP_MODIFY_CHN_IN          = 8,

    /** �޸�VP�������� */
    IMOS_EN_CHG_STEP_MODIFY_CHN_OUT         = 9,

    /** ��ʼ������ */
    IMOS_EN_CHG_STEP_START_DETECT_BITRATE   = 10,

    /** ֹͣ������ */
    IMOS_EN_CHG_STEP_STOP_DETECT_BITRATE    = 11,
    IMOS_EN_CHG_STEP_COUNT
} MC_LINK_STATUS_CHG_STEP_EX_E;

/**
 * @enum tagMC_LinkChgActionEx
 * @brief ��·״̬�л����� [����״̬���л�ʹ��]
 * @attention
 */
typedef enum tagMC_LinkChgActionEx
{
    /** ·���л���������· */
    IMOS_EN_CHG_ACTION_ROUTE2BAK                = 0,

    /** ·���л���������· */
    IMOS_EN_CHG_ACTION_ROUTE2MAIN               = 1,

    /** ����ʽ��������·�ɲ��� */
    IMOS_EN_CHG_ACTION_ROUTE2BAK_BITRATE2BAK    = 2,

    /** ����ʽ��������·�ɲ��� */
    IMOS_EN_CHG_ACTION_ROUTE2MAIN_BITRATE2MAIN  = 3,

    /** [����ʽ]�����ʲ��� */
    IMOS_EN_CHG_ACTION_D_BITRATE                = 4,

    /** [����ʽ]�����ʲ��� */
    IMOS_EN_CHG_ACTION_U_BITRATE                = 5,

    /** ·���л���������·��ͨ��RESET������·BFDʵ�� */
    IMOS_EN_CHG_ACTION_RESET2MAIN               = 6,
    IMOS_EN_CHG_ACTION_COUNT
} MC_LINK_CHG_ACTION_EX_E;

/**
 * @enum tagMC_Link_Layer_TypeEx
 * @brief ��·����
 * @attention
 */
typedef  enum  tagMC_Link_Layer_TypeEx
{
    /** ������· */
    IMOS_EN_LINK_LAYER_MAIN = 0,

    /** ������· */
    IMOS_EN_LINK_LAYER_BAK  = 1,
    IMOS_EN_LINK_LAYER_TYPE_COUNT
} MC_LINK_LAYER_TYPE_EX_E;

/**
 * @enum tagMC_Link_Layer_StateEx
 * @brief ��·����״̬
 * @attention
 */
typedef  enum  tagMC_Link_Layer_StateEx
{
    /** ��·�Ͽ� */
    IMOS_EN_LINK_LAYER_DISCONNECT = 0,

    /** �ߴ����ȶ� */
    IMOS_EN_LINK_LAYER_HIGH_BANDWIDTH = 1,

    /** �ʹ����ȶ� */
    IMOS_EN_LINK_LAYER_LOW_BANDWIDTH = 2,

    /** �ʹ����� */
    IMOS_EN_LINK_LAYER_LOW_BANDWIDTH_WITH_LOST = 3,

    IMOS_EN_LINK_LAYER_STATE_COUNT
} MC_LINK_LAYER_STATE_EX_E;

/**
 * @enum tagMC_Link_Layer_NIC_TypeEx
 * @brief ��·��
 * @attention
 *      1������ MCU :������·�� --> eth0;   ������·�� --> eth1
 *      2������ �ն�:������·�� --> eth0/1; ������·�� --> E1
 *      3��LO �� IP ��ַ��Ϊҵ���ַ
 */
typedef  enum  tagMC_Link_Layer_NIC_TypeEx
{
    /** ������·��[������·��ַ] */
    IMOS_EN_LINK_LAYER_NIC_TYPE_MAIN = 0,

    /** ������·��[������·��ַ] */
    IMOS_EN_LINK_LAYER_NIC_TYPE_BAK = 1,

    /** LO ��[ҵ���ַ] */
    IMOS_EN_LINK_LAYER_NIC_TYPE_LO = 2,
    IMOS_EN_LINK_LAYER_NIC_TYPE_COUNT,
} MC_LINK_LAYER_NIC_TYPE_EX_E;

/**
 * @enum tagMC_Link_Layer_Switch_ModeEx
 * @brief ��·�л�ģʽ
 * @attention
 */
typedef  enum  tagMC_Link_Layer_Switch_ModeEx
{
    /** �Զ� */
    IMOS_EN_LINK_LAYER_SWITCH_BY_AUTO = 0,

    /** �ֶ� */
    IMOS_EN_LINK_LAYER_SWITCH_BY_MANUAL = 1,
    IMOS_EN_LINK_LAYER_SWITCH_MODE_COUNT,
} MC_LINK_LAYER_SWITCH_MODE_EX_E;

 /**
 * @enum tagMC_TermAlarmStatusEx
 * @brief �ն�����澯״̬
 * @attention
 */
typedef enum tagMC_TermAlarmStatusEx
{
    /**< ������ */
    IMOS_EN_TERM_ALARM_DISABLE  = 0,

    /**< �澯 */
    IMOS_EN_TERM_ALARM_YES = 1,

    /**< ���� */
    IMOS_EN_TERM_ALARM_NO = 2,
    IMOS_EN_TERM_ALARM_COUNT
} MC_TERM_ALARM_STATUS_EX_E;

 /**
 * @enum tagMC_NodeTypeEx
 * @brief �ڵ�����
 * @attention
 */
typedef enum tagMC_NodeTypeEx
{
    /**< MG */
    IMOS_EN_NODE_TYPE_MG  = 0,

    /**< MCU */
    IMOS_EN_NODE_TYPE_MCU = 1,

    IMOS_EN_NODE_TYPE_COUNT
} MC_NODE_TYPE_EX_E;

/**
 * @enum tagMC_VirtualResStatusEx
 * @attention ��������ն�״̬
 */
typedef enum tagMC_VirtualResStatusEx
{
    /**< ���� */
    IMOS_EN_VIRTUAL_RES_STATUS_IDLE  = 0,

    /**< ���������� */
    IMOS_EN_VIRTUAL_RES_STATUS_CAMERA = 1,

    /**< ����ɼ����� */
    IMOS_EN_VIRTUAL_RES_STATUS_SCREEN = 2,

    IMOS_EN_VIRTUAL_RES_STATUS_COUNT

} MC_VIRTUAL_RES_STATUS_EX_E;


/******************************************************************************
��־��MC_DEVICE_TYPE_E
���ͣ�ö��
Ŀ�ģ��ϱ���iMSC���豸�ͺ�
���壺
******************************************************************************/

/**
 * @enum tagMC_VirtualResStatusEx
 * @attention �ϱ���iMSC���豸�ͺ�
 */
typedef  enum tagMcDeviceTypeEx
{
    IMOS_EN_DEVICE_TYPE_SD_TERM_BEGIN   = 0,
    IMOS_EN_DEVICE_TYPE_MG6030          = 1,
    IMOS_EN_DEVICE_TYPE_MG6050          = 2,
    IMOS_EN_DEVICE_TYPE_MG6050S         = 3,
    IMOS_EN_DEVICE_TYPE_MG6060          = 4,
    IMOS_EN_DEVICE_TYPE_TOPVIEW         = 5,

    /** �����ն� */
    IMOS_EN_DEVICE_TYPE_VIRTUAL_TERM    = 6,

    /** �������ն� */
    IMOS_EN_DEVICE_TYPE_3RD_TERM    = 7,
    /** ������MCU */
    IMOS_EN_DEVICE_TYPE_3RD_MCU     = 8,
    IMOS_EN_DEVICE_TYPE_SD_TERM_END,

    IMOS_EN_DEVICE_TYPE_HD_TERM_BEGIN   = 100,
    IMOS_EN_DEVICE_TYPE_MG9010          = 101,
    IMOS_EN_DEVICE_TYPE_MG9030          = 102,
    IMOS_EN_DEVICE_TYPE_MG9060          = 103,
    IMOS_EN_DEVICE_TYPE_MG9090          = 104,
    IMOS_EN_DEVICE_TYPE_HD_TERM_END,

    IMOS_EN_DEVICE_TYPE_MCU_BEGIN   = 500,
    IMOS_EN_DEVICE_TYPE_ME5000      = 501,
    IMOS_EN_DEVICE_TYPE_ME8000      = 502,
    IMOS_EN_DEVICE_TYPE_ME8600      = 503,
    IMOS_EN_DEVICE_TYPE_MCU_END,

    IMOS_EN_DEVICE_TYPE_GK_BEGIN    = 800,
    IMOS_EN_DEVICE_TYPE_H3C_GK      = 801,
    IMOS_EN_DEVICE_TYPE_GK_END,
}MC_DEVICE_TYPE_EX_E;


/**
 * @enum MC_CHAIR_BROWSE_MODE_EX_E
 * @attention ����ѡ��ۿ�ģʽ
*/
typedef  enum tagMC_ChairBrowseModeEx
{
    /**< ����ѡ��ۿ������� */
    IMOS_EN_CHAIR_BROWSE_MODE_SINGLE_PIC = 0,

    /**< ����ѡ��ۿ��໭�� */
    IMOS_EN_CHAIR_BROWSE_MODE_MULTI_PIC = 1,

    IMOS_EN_CHAIR_BROWSE_MODE_COUNT
} MC_CHAIR_BROWSE_MODE_EX_E;

/**
 * @enum MC_BROADCAST_MODE_EX_E
 * @attention �㲥ģʽ
*/
typedef  enum tagMC_BroadcastModeEX
{
    /**< �㲥������ */
    IMOS_EN_BROADCAST_MODE_SINGLE_PIC = 0,

    /**< �㲥�໭�� */
    IMOS_EN_BROADCAST_MODE_MULTI_PIC = 1,

    IMOS_EN_BROADCAST_MODE_COUNT
} MC_BROADCAST_MODE_EX_E;

/**
 * @enum tagTermQueryType
 * @brief �ն˲�ѯ����
 * @attention
 */
typedef enum tagTermQueryType
{
    /**< �������ն� */
    IMOS_EN_TERM_QUERY_TYPE_3RD  = 0,

    /**< H3C�ն� */
    IMOS_EN_TERM_QUERY_TYPE_H3C = 1,

    /**< �����ն�(�����������ն�) */
    IMOS_EN_TERM_QUERY_TYPE_ALL = 2,

    /**< �����ն� */
    IMOS_EN_TERM_QUERY_TYPE_ANONY,

    IMOS_EN_TERM_QUERY_TYPE_COUNT
} TERM_QUERY_TYPE_E;



/**
 * @enum tagMC_ConfigRouteTypeEx
 * @brief ·�ɲ�������
 * @attention
 */
typedef  enum  tagMC_ConfigRouteTypeEx
{
    /**< ���·�� */
    IMOS_EN_CONFIG_ROUTE_ADD = 0,

    /**< �޸�·�� */
    IMOS_EN_CONFIG_ROUTE_MDF = 1,

    /**< ɾ��·�� */
    IMOS_EN_CONFIG_ROUTE_DEL = 2,

    /**< ���·�� */
    IMOS_EN_CONFIG_ROUTE_CLR = 3,

    IMOS_EN_CONFIG_ROUTE_COUNT
} MC_CONFIG_ROUTE_TYPE_EX_E;


/**
 * @enum tagMC_BrowseSiteTypeEx
 * @brief �ն˱��ۿ�ģʽ
 * @attention
 */
typedef  enum  tagMC_BrowseSiteTypeEx
{
    /**< �᳡��Ч */
    IMOS_EN_BROWSE_SITE_USE = 0,

    /**< �᳡��Ч */
    IMOS_EN_BROWSE_SITE_INVALID = 1,

    /**< �໭�� */
    IMOS_EN_BROWSE_SITE_MULTIPIC = 2,

    /**< MCU */
    IMOS_EN_BROWSE_SITE_MCU,

    IMOS_EN_BROWSE_SITE_COUNT
} MC_BROWSE_SITE_TYPE_EX_E;



/**
 * @enum tagEN_SCALER_TYPE_EX
 * @brief ����ģʽ: �ȱȺڱߴ����ȱȲü��������죬���ܵ��±��Ρ�͸������1:1
 * @attention
 */
typedef enum tagEN_SCALER_TYPE_EX
{
    /**< �ȱ�����ڱ� */
    IMOS_EN_HD_SCALER_TYPE_STRETCH_BLACK = 0,

    /**< �ȱ����ü� */
    IMOS_EN_HD_SCALER_TYPE_STRETCH_CUT = 1,

    /**< ȫ�� */
    IMOS_EN_HD_SCALER_TYPE_STRETCH = 2,

    /**< ԭʼ��С */
    IMOS_EN_HD_SCALER_TYPE_TRANSP = 3
} HD_SCALER_TYPE_EX_E;


/**
 * @enum tagHD_LPR_Type_EX
 * @brief LPR����
 * @attention
 */
typedef enum tagHD_LPR_Type_EX
{
    /**< ������LPR */
    IMOS_EN_HD_LPR_TYPE_NO = 0,

    /**< �������᳡���� */
    IMOS_EN_HD_LPR_TYPE_LOST_PATCKET_SITE = 1,

    /**< ȫ���᳡���� */
    IMOS_EN_HD_LPR_TYPE_ALL_SITE = 2
} HD_LPR_TYPE_EX_E;


/**
* @enum tagInterconnectInfoType
* @brief ������Ϣ����ö��
* @attention
*/
typedef enum tagInterconnectInfoType
{
    INTERCONNECT_INFO_TYPE_LODOMAIN      = 0,             /**< ��������Ϣ  */

    INTERCONNECT_INFO_TYPE_MAX,
    INTERCONNECT_INFO_TYPE_INVALID = 0xFF
}INTERCONNECT_INFO_TYPE_E;


/**
* @enum tagPlanOperateMode
* @brief �ƻ�����ģʽ
* @attention
*/
typedef enum tagPlanOperateMode
{
    PLAN_OPER_STOP = 0,                                 /**< ֹͣ�ƻ� */
    PLAN_OPER_START = 1,                                /**< �����ƻ� */
    PLAN_OPER_INVALID = 0xFFFFFFFF                      /**< ��Чֵ */
}PLAN_OPER_MODE_E;

/**
* @enum tagManualOperType
* @brief �ֶ���������
* @attention
*/
typedef enum tagManualOperType
{
    MANUAL_OPER_STOP = 0,               /**< ֹͣ */
    MANUAL_OPER_START = 1,              /**< ���� */
    MANUAL_OPER_FORWARD = 2,            /**< ��ǰ���� */
    MANUAL_OPER_BACKWARD = 3,           /**< ������ */
    MANUAL_OPER_PAUSE = 4,              /**< ��ͣ */
    MANUAL_OPER_RESUME = 5,             /**< �ָ� */

    MANUAL_OPER_MAX,                    /**< ö�����ֵ */
    MANUAL_OPER_INVALID = 0xFFFFFFFF    /**< ��Чֵ */
}MANUAL_OPER_TYPE_E;

/**
* @enum tagLayoutType
* @brief ��������ö��
* @attention
*/
typedef enum tagLayoutType
{
    LAYOUT_TYPE_SCREEN       = 1,       /**< ��Ļ���� */
    LAYOUT_TYPE_MONITOR      = 2,       /**< ���������� */

    LAYOUT_TYPE_MAX,                    /**< ö�����ֵ */
    LAYOUT_TYPE_INVALID = 0xFFFFFFFF    /**< ��Чֵ */
}LAYOUT_TYPE_E;


/**
* @enum tagSalvoType
* @brief ����ʾ����ö��
* @attention
*/
typedef enum tagSalvoType
{
    SALVO_TYPE_MONITOR      = 0,       /**< ����������ʾ */
    SALVO_TYPE_TVWALL       = 1,       /**< ����ǽ����ʾ */
    SALVO_TYPE_SCREEN       = 2,       /**< ��Ļ����ʾ */

    SALVO_TYPE_MAX,                    /**< ö�����ֵ */
    SALVO_TYPE_INVALID = 0xFFFFFFFF    /**< ��Чֵ */
}SALVO_TYPE_E;


/**
* @enum tagCommonSwitchType
* @brief ͨ����������ö��
* @attention
*/
typedef enum tagCommonSwitchType
{
    COMMON_SWITCH_GUARDTOUR     =  0,           /**< ��ͨ���� */
    COMMON_SWITCH_GROUPSWITCH   =  1,           /**< ����Ѳ */

    COMMON_SWITCH_MAX,                          /**< ͨ�������������ֵ */
    COMMON_SWITCH_INVALID = 0XFFFFFFFF
}COMMON_SWITCH_TYPE_E;

/**
* @enum tagCommonSwitchPlanType
* @brief ͨ�����мƻ�����ö��
* @attention
*/
typedef enum tagCommonSwitchPlanType
{
    COMMON_SWITCHPLAN_TYPE_MONITOR = 0,                 /**< Ӳ�����мƻ� */
    COMMON_SWITCHPLAN_TYPE_SCREEN = 1,                  /**< ������мƻ� */
    COMMON_SWITCHPLAN_TYPE_GROUPSWITCH = 2,             /**< ����Ѳ�ƻ� */

    COMMON_SWITCHPLAN_TYPE_MAX,                         /**< ö�����ֵ */
    COMMON_SWITCHPLAN_TYPE_INVALID = 0xFFFFFFFF         /**< ��Чֵ */
}COMMON_SWITCHPLAN_TYPE_E;


/**
* @enum tagGroupSwitchType
* @brief ����Ѳ����ö��
* @attention
*/
typedef enum tagGroupSwitchType
{
    GROUP_SWITCH_TYPE_MONITOR     =  0,           /**< ����������Ѳ */
    GROUP_SWITCH_TYPE_TVWALL      =  1,           /**< ����ǽ����Ѳ */
    GROUP_SWITCH_TYPE_SCREEN      =  2,           /**< ��Ļ����Ѳ */

    GROUP_SWITCH_TYPE_MAX,                        /**< ����Ѳ�������ֵ */
    GROUP_SWITCH_TYPE_INVALID = 0XFFFFFFFF
}GROUP_SWITCH_TYPE_E;

/** ͸��ͨ������UI���¼�ö�����Ͷ��� */
typedef enum tagTLNotifyUIEventType
{
    TL_NOTIFY_TYPE_REAVE   = 1,    /** ͸��ͨ����ͣ�� */

    TL_NOTIFY_TYPE_INVALID = 0xFFFFFFFF

}TL_NOTIFY_UI_EVENT_TYPE;

/**
* @enum tagConfCheckAuthMode
* @brief �����Ȩ��ʽ
* @attention
*/
typedef enum tagConfCheckAuthMode
{
    IMOS_EN_CONF_CHECK_AUTH_CONFCTRL   = 0,             /**< ������Ƽ�Ȩ */
    IMOS_EN_CONF_CHECK_AUTH_CONFMANAGE = 1,                 /**< ��������Ȩ */
    IMOS_EN_CONF_CHECK_AUTH_COUNT
} CONF_CHECK_AUTH_MODE_E;


/**
* @enum tagMcuSynchronStatus
* @brief MCUͬ��״̬
* @attention
*/
typedef enum tagMcuSynchronStatus
{
    IMOS_EN_MCU_SYNC_STATUS_OPEN  = 0,             /**< ����ͬ�� */
    IMOS_EN_MCU_SYNC_STATUS_CLOSE = 1,             /**< û����ͬ�� */
    IMOS_EN_MCU_SYNC_STATUS_COUNT
} MCU_SYNCHRON_STATUS_E;


/**
* @enum tagIMOSFtpFileType
* @brief FTP�ļ�����
* @attention
*/
typedef enum tagIMOSFtpFileType
{
    IMOS_FTP_FILE_TYPE_DEFAULT           = 0,      /** ȱʡ�ļ����ͣ���ʾֱ����ftp��Ŀ¼ */
    IMOS_FTP_FILE_TYPE_CONFERENCE_CONFIG = 1,      /** ��Ѷ�豸���������ļ�����Ӧ�����Ѷ���������ļ���Ŀ¼ */

    IMOS_FTP_FILE_TYPE_MAX,
    IMOS_FTP_FILE_TYPE_INVALID = 0xFFFFFFFF
}IMOS_FTP_FILE_TYPE_E;

/**
* @enum tagMCUBackupType
* @brief MCU��������
* @attention
*/
typedef enum tagMCUBackupType
{
    IMOS_MCU_BACKUP_TYPE_SPECIFIC       = 0,    /**< ר���ڱ��ݵ�MCU */
    IMOS_MCU_BACKUP_TYPE_AVAILABLE      = 1,    /**< �����ڱ��ݵ�MCU */
    IMOS_MCU_BACKUP_TYPE_UNAVAILABLE    = 2,    /**< �������ڱ��ݵ�MCU */
    IMOS_MCU_BACKUP_TYPE_COUNT
}MCU_BACKUP_TYPE_E;

/**
* @enum tagMCUBackupScope
* @brief MCU��������
* @attention
*/
typedef enum tagMCUBackupScope
{
    IMOS_MCU_BACKUP_SCOPE_LOCAL              = 0,  /**< ֻ���ڱ��ݵ�ǰ���MCU */
    IMOS_MCU_BACKUP_SCOPE_LOCAL_AND_CHILDREN = 1,  /**< ���ڱ��ݵ�ǰ����������MCU */
    IMOS_MCU_BACKUP_SCOPE_COUNT
}MCU_BACKUP_SCOPE_E;

/**
* @enum tagMgSessionStatus
* @brief MG�Ự״̬
* @attention
*/
typedef enum tagMgSessionStatus
{
    IMOS_MG_SESSION_STATUS_IDLE              = 0,  /**< ���� */
    IMOS_MG_SESSION_STATUS_CALLING           = 1,  /**< ͨ���� */
    IMOS_MG_SESSION_STATUS_UNKNOWN           = 2,  /**< δ֪ */
    IMOS_MG_SESSION_STATUS_COUNT
}MG_SESSION_STATUS_E;

/**
* @enum tagMCMCUCallTypeEX
* @brief MCU�����ն˵ĺ�����������
* @attention
*/
typedef enum tagMCMCUCallTypeEX
{
    IMOS_EN_MCU_CALL_TYPE_AUTO               = 0,  /* �Զ� */
    IMOS_EN_MCU_CALL_TYPE_CALL               = 1,  /* �������� */
    IMOS_EN_MCU_CALL_TYPE_WAIT               = 2,  /* �ȴ����� */
    IMOS_EN_MCU_CALL_TYPE_COUNT
} MC_MCU_CALL_TYPE_EX_E;

/**
* @enum tagTIP_TimeLenTypeEx
* @brief ��ʾ��ǰ���õ������ֽ�����ʾ��
* @attention
*/
typedef enum tagTIP_TimeLenTypeEx
{
    IMOS_EN_AUD_TIP_CONF_END_TIME0           = 0,  /* �����û��������ʾ�� */
    IMOS_EN_AUD_TIP_CONF_END_TIME3           = 3,  /* ���û������3���� */
    IMOS_EN_AUD_TIP_CONF_END_TIME5           = 5,  /* ���û������5���� */
    IMOS_EN_AUD_TIP_CONF_END_TIME10          = 10  /* ���û������10���� */
} HD_TIP_TIME_LEN_TYPE_EX_E;

/**
* @enum tagSubscType
* @brief ��������ö��
* @attention
*/
typedef enum tagSubscType
{
    SUBSC_ALARM_TYPE              = 0,                /**< �澯���ģ���Ӧ����ö��Ϊ:#SUBSC_ALARM_CON_E  */

    SUBSC_MAX_TYPE,
    SUBSC_INVALID_TYPE            = 0xFF
}SUBSC_TYPE_E;

/**
* @struct SUBSC_ALARM_CON_S
* @brief ���ĸ澯��������ö��
*/
typedef enum tagAlarmConType
{
    ALARM_CON_EVENT_TYPE            = 0,            /** ���ĵĸ澯���� */
    ALARM_CON_LEVEL_TYPE,                           /** ���ĵĸ澯���𣬴�������ALARM_CON_EVENT_TYPE���� */

    ALARM_CON_TYPE_MAX,
    ALARM_CON_TYPE_INVALID          = 0xFF
}SUBSC_ALARM_CON_E;

/**
* @enum tagTrunkType
* @brief ���߹�������ö��
* @attention
*/
typedef enum tagTrunkType
{
    TRUNK_TYPE_EXDOMAIN     = 0,             /**< �������  */
    TRUNK_TYPE_DEV          = 1,             /**< �豸����  */

    TRUNK_TYPE_MAX,
    TRUNK_TYPE_INVALID          = 0xFF
}TRUNK_TYPE_E;

/**
* @enum tagTrunkContentType
* @brief ������������ö��
* @attention
*/
typedef enum tagTrunkContentType
{
    TRUNK_CONTENT_TYPE_ALL_EXDOMAIN     = 0,             /**< ��������  */
    TRUNK_CONTENT_TYPE_EXDOMAIN         = 1,             /**< ����  */
    TRUNK_CONTENT_TYPE_ORG              = 2,             /**< ��֯  */
    TRUNK_CONTENT_TYPE_DEV              = 3,             /**< �豸  */

    TRUNK_CONTENT_TYPE_MAX,
    TRUNK_CONTENT_TYPE_INVALID          = 0xFF
}TRUNK_CONTENT_TYPE_E;


/**
* @enum tagTrunkContentRangeType
* @brief �������ݹ���Χ����ö��
* @attention
*/
typedef enum tagTrunkContentRangeType
{
    TRUNK_CONTENT_RANGE_TYPE_LOC            = 0,        /**< ���� */
    TRUNK_CONTENT_RANGE_TYPE_LOC_AND_SUB    = 1,        /**< �����Լ��¼�  */

    TRUNK_CONTENT_RANG_TYPE_MAX,
    TRUNK_CONTENT_RANG_TYPE_INVALID         = 0xFF
}TRUNK_CONTENT_RANGE_TYPE_E;

/**
* @enum tagPlanTypeDef
* @brief �ƻ����Ͷ���
* @attention
*/
typedef enum tagPlanTypeDef
{
    PLAN_BAK              = 0,                          /**< ���ݼƻ� */
    PLAN_TYPE_DEF_MAX,                                  /**< ���ֵ */
    PLAN_TYPE_DEF_INVALID = 0xFFFFFFFF                  /**< ��Чֵ */
}PLAN_TYPE_DEF_E;

/**
* @enum tagBakMode
* @brief ����ģʽ
* @attention
*/
typedef enum tagBakMode
{
    BAK_SHARE        = 0,                               /**< ���� */
    BAK_OWN          = 1,                               /**< ���� */
    BAK_MODE_MAX,                                       /**< ���ֵ */
    BAK_MODE_INVALID = 0xFFFFFFFF                       /**< ��Чֵ */
}BAK_MODE_E;

/**
* @enum tagBakType
* @brief ��������
* @attention
*/
typedef enum tagBakType
{
    BAK_PLAN         = 0,                               /**< �ƻ����� */
    BAK_MANUL        = 1,                               /**< �ֶ����� */
    BAK_ALARM        = 2,                               /**< �澯���� */
    BAK_ALL          = 3,                               /**< �����������ͣ���ѯ����ʱʹ�� */
    BAK_TYPE_MAX,                                       /**< ���ֵ */
    BAK_TYPE_INVALID = 0xFFFFFFFF                       /**< ��Чֵ */
}BAK_TYPE_E;

/**
* @enum tagBakPolicy
* @brief ���ݲ���
* @attention
*/
typedef enum tagBakPolicy
{
    BAK_POLICY_FULLFRAME = 0,                           /**< ȫ֡���� */
    BAK_POLICY_SOMEFRAME = 1,                           /**< ��֡���� */
    BAK_POLICY_MAX,                                     /**< ���ֵ */
    BAK_POLICY_INVALID   = 0xFFFFFFFF                   /**< ��Чֵ */
}BAK_POLICY_E;

/**
* @enum tagBakTaskStatus
* @brief ��������״̬
* @attention
*/
typedef enum tagBakTaskStatus
{
    BAK_TASK_WAIT           = 0,                        /**< δ���� */
    BAK_TASK_RUN            = 1,                        /**< ���ڱ��� */
    BAK_TASK_DONE           = 2,                        /**< �Ѿ���� */
    BAK_TASK_HANG           = 3,                        /**< �һ� */
    BAK_TASK_STATUS_MAX,                                /**< ���ֵ */
    BAK_TASK_STATUS_INVALID = 0xFFFFFFFF                /**< ��Чֵ */
}BAK_TASK_STATUS_E;

/**
* @enum tagBakFileOper
* @brief �����ļ�����
* @attention
*/
typedef enum tagBakFileOper
{
    BAK_FILE_LOCK         = 1,                          /**< ���� */
    BAK_FILE_UNLOCK       = 2,                          /**< ���� */
    BAK_FILE_DELETE       = 3,                          /**< ɾ�� */
    BAK_FILE_OPER_MAX,                                  /**< ���ֵ */
    BAK_FILE_OPER_INVALID = 0xFFFFFFFF                  /**< ��Чֵ */
}BAK_FILE_OPER_E;

/**
* @enum tagBakResStatus
* @brief ������Դ״̬
* @attention
*/
typedef enum tagBakResStatus
{
    BAK_RES_UNFORMATTED    = 0,                         /**< δ��ʽ�� */
    BAK_RES_FORMATTING     = 1,                         /**< ���ڸ�ʽ�� */
    BAK_RES_ONLINE         = 2,                         /**< ���� */
    BAK_RES_OFFLINE        = 3,                         /**< ���� */
    BAK_RES_STATUS_MAX,                                 /**< ���ֵ */
    BAK_RES_STATUS_INVALID = 0xFFFFFFFF                 /**< ��Чֵ */
}BAK_RES_STATUS_E;

/**
* @enum tagBakFileQueryType
* @brief ����¼���ѯ����
* @attention
*/
typedef enum tagBakFileQueryType
{
    BAK_FILE_QUERY_TYPE_TIME    = 0,                    /**< ������ʱ���ѯ */
    BAK_FILE_QUERY_TYPE_CASE    = 1,                    /**< ������������ѯ */
    BAK_FILE_QUERY_TYPE_MAX,                            /**< ���ֵ */
    BAK_FILE_QUERY_TYPE_INVALID = 0xFFFFFFFF            /**< ��Чֵ */
}BAK_FILE_QUERY_TYPE_E;

/**
* @enum tagBakResType
* @brief ������Դ����
* @attention
*/
typedef enum tagBakResType
{
    BAK_RES_TYPE_H3C     = 0,                           /**< ��˾�豸 */
    BAK_RES_TYPE_THIRD   = 1,                           /**< �������豸 */
    BAK_RES_TYPE_MAX,                                   /**< ���ֵ */
    BAK_RES_TYPE_INVALID = 0xFFFFFFFF                   /**< ��Чֵ */
}BAK_RES_TYPE_E;

/* added by z06806 for vodwall prj 20101216 begin */
/**
* @enum tagVODWALLStopCodeType
* @brief ��ʾʹ�����ֱ������رջط���ǽ
* @attention
*/
typedef enum tagVODWALLStopCodeType
{
    IMOS_VODWALL_STOP_CODETYPE_XP           = 0,  /* ����XP����رջط���ǽ */
    IMOS_VODWALL_STOP_CODETYPE_MONITOR      = 1,  /* ���ݼ���������رջط���ǽ */
} VODWALL_STOP_CODETYPE_E;
/* added by z06806 for vodwall prj 20101216 end */

/*@brief �ص��������� */
typedef VOID (STDCALL *SDK_CALL_BACK_PROC_PF)(INOUT VOID *pParam);

/**************************************************************************/
/**
* @enum tagUniversalQueryFlag
* @brief ͨ�ò�ѯ��־λ
* @attention
*/
typedef enum tagUniversalQueryFlag
{
    UNI_QRY_CAM_STORAGE_INFO, /**< ������洢��Ϣ��ѯ,��Ӧ������#QRY_CAM_STORAGE_INFO_E */
    UNI_QRY_MAX
}UNI_QUERY_FLAG_E;

/**
* @enum tagQryCamStorageInfo
* @brief ������洢��Ϣ��ѯ����
* @attention
*/
typedef enum tagQryCamStorageInfo
{
    CAMST_CAMERA_NAME = 0,    /**< ���������#IMOS_NAME_LEN */
    CAMST_CAMERA_CODE,        /**< ���������#IMOS_CODE_LEN */
    CAMST_RECODRD_DATE,       /**< �洢����#IMOS_TIME_LEN */
    CAMST_STORAGE_DEVNAME,    /**< �洢�豸����#IMOS_NAME_LEN */
    CAMST_STORAGE_DEVCODE,    /**< �洢�豸����#IMOS_CODE_LEN */
    CAMST_STORAGE_DEVTYPE,    /**< �洢�豸����#ULONG */
    CAMST_RECORD_HOUR0002,    /**< 00:00-02:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR0204,    /**< 02:00-04:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR0406,    /**< 04:00-06:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR0608,    /**< 06:00-08:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR0810,    /**< 08:00-10:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR1012,    /**< 10:00-12:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR1214,    /**< 12:00-14:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR1416,    /**< 14:00-16:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR1618,    /**< 16:00-18:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR1820,    /**< 18:00-20:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR2022,    /**< 20:00-22:00��Ĵ洢��Ϣ#ULONG */
    CAMST_RECORD_HOUR2224,    /**< 22:00-24:00��Ĵ洢��Ϣ#ULONG */
    CAMST_IS_CREATEPLAN,      /**< �Ƿ��ƶ��˴洢�ƻ�#ULONG */
    CAMST_IS_ENABELPLAN,      /**< �Ƿ������˴洢�ƻ�#ULONG */
    CAM_ST_MAX
}QRY_CAM_STORAGE_INFO_E;

/**
* @enum tagTVWallResolution
* @brief ����ǽ�ֱ�������
* @attention
*/
typedef enum tagTVWallResolution
{
    TW_RESOLUTION_1440_900                     = 0,                 /**< ����ǽ1440*900�ֱ��� */
    TW_RESOLUTION_1280_1024                    = 1,                 /**< ����ǽ1280*1024�ֱ��� */
    TW_RESOLUTION_1280_800                     = 2,                 /**< ����ǽ1280*800�ֱ��� */
    TW_RESOLUTION_1024_768                     = 3,                 /**< ����ǽ1024*768�ֱ��� */
    TW_RESOLUTION_MAX,                                              /**< ���ֵ */
    TW_RESOLUTION_INVALID                      = 0xFFFFFFFF         /**< ��Чֵ */
}TW_RESOLUTION_E;

/**
* @enum tagUniversalConditionFlag
* @brief ͨ��Լ����ѯ������־λ
* @attention
*/
typedef enum tagUniversalConditionFlag
{
    /** ȫ�� */
    CONDITION_OR_ALL,
    /** ȫ�� */
    CONDITION_AND_ALL,
    /** �Ȼ���� ����:<a and b and (c or d)> */
    CONDITION_AND_OR,
    /** ������ ����:<a or b or (c and d)> */
    CONDITION_OR_AND
}UNI_CONDITION_FLAG_E;

/**
* @enum tagIMOSStreamServerMode
* @brief �����䷽ʽ
* @attention
*/
typedef enum tagIMOSStreamServerMode
{
    IMOS_STREAM_MODE_MS                 = 0,   /**< ָ��MS*/
    IMOS_STREAM_MODE_MS_AUTO            = 1,   /**< MS����Ӧ*/
    IMOS_STREAM_MODE_MS_BYPASS          = 2,   /**< ������MS */
    IMOS_STREAM_MODE_MS_BYPASS_FIRST    = 3,   /**< ֱ������ */

    IMOS_STREAM_MODE_BUTT
}IMOS_STREAM_SERVER_MODE_E;

/**
 * @enum tagOutChannelType
 * @brief ���ͨ������
 * @attention ��
 */
typedef enum tagOutChannelType
{
    OUT_CHANNEL_TYPE_VGA        = 0,            /**< VGA */
        
    OUT_CHANNEL_TYPE_MAX,                       /**< ���ֵ */
    OUT_CHANNEL_TYPE_INVALID    = 0xFFFFFFFF,   /**< ��Чֵ */
} OUT_CHANNEL_TYPE_E;

/**
 * @enum tagOutChannelMode
 * @brief ���ͨ��ģʽ
 * @attention ��
 */
typedef enum tagOutChannelMode
{
    OUT_CHANNEL_MODE_EXIT_FULL_SCREEN   = 0,            /**< �˳�ȫ�� */
    OUT_CHANNEL_MODE_FULL_SCREEN,                       /**< �Ŵ�ȫ�� */
        
    OUT_CHANNEL_MODE_MAX,                               /**< ���ֵ */
    OUT_CHANNEL_MODE_INVALID            = 0xFFFFFFFF,   /**< ��Чֵ */
} OUT_CHANNEL_MODE_E;


/**
 * @struct tagVinChnlBindCameraElement
 * @brief ����������ͨ���䳤Ԫ�ؽṹ��
 * @attention
 */
typedef enum tagVinChnlBindCameraVarietyElementID
{
    /** ����OSD */
    OSD_NAME_ELEMENT,

    /** �ڸ����� */
    MASK_AREA_ELEMENT ,
    
    /** ��Ƶ��  */
    VIDEO_STREAM_ELEMENT,
        
    /** �ڵ�������� */
    COVER_DETEC_AREA_ELEMENT,
  
    /** �˶�������� */
    MOTION_DETEC_AREA_ELEMENT,

    VARIETY_ELEMENT_ID_MAX
}VINCHNL_BIND_CAMERA_VARIETY_ELEMENT_ID_E;

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif

