/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_def.h

  Project Code: IMOS PLAT
   Module Name: PUBLIC
  Date Created: 2009-03-16
        Author: lilunyi/03464
   Description: ����ƽ̨����ͷ�ļ�

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

/* ��ͬ���ͷ���ʹ����ͬ��ProcID��1-99���������ʹ�� */
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


/* SDK client�͵�����SDK����ʹ��100 */
#define IMOS_PROCID_SDKCLIENT           0x64

/* TP���̱�������ֵ���䣬��101��ʼ */
#define IMOS_PROCID_MW_CTRL             0x65
#define IMOS_PROCID_MW_STOR_MCP         0x66
#define IMOS_PROCID_MW_PTZ              0x67
#define IMOS_PROCID_MW_SNMP             0x68
#define IMOS_PROCID_MW_ALM              0x69
#define IMOS_PROCID_MW_CAPI             0x6A
#define IMOS_PROCID_MW_SDK              0x6B

/* ���Թ����Լ��������߳���ʹ��257 */
#define IMOS_PROCID_DT                  0x101


/** @brief Ȩ����������������� */
#define IMOS_AUTHORITY_MAX_NUM                  128

/** �ڲ�Ȩ����������������� */
#define IMOS_AUTHORITY_MAX_NUM_EX               1024

/** �洢Ȩ���ֶ������� */
#define IMOS_AUTHORITY_MAX_FIELD_COUNT          32

/*@brief IMOSϵͳ��������*/
#define IMOS_SYS_PARAM_LEN                      32             /**< ƽ̨֧�ֵ�ϵͳ��������, ��Ч���λ��Ϊ31λ */

/*@brief ͨ��CODE����*/
#define IMOS_CODE_LEN                           48             /**< ƽ̨֧�ֵ���Ч���λ��Ϊ47λ */

/*@brief ͨ�������ַ������� */
#define IMOS_NAME_LEN                           64             /**< ƽ̨֧�ֵ���Ч���λ��Ϊ63λ */

/*@brief URL��ַ���� */
#define IMOS_URL_LEN                            512
/*@brief ���ͼ���� */
#define MAX_LAYER_NUM                           6              /**< ƽ̨֧�ֵ���Ч���ͼ���� */
/////////////////////

/*@brief ��Դ������Ϣ�ַ�������*/
#define IMOS_RES_CODE_LEN                       IMOS_CODE_LEN

/*@brief �豸������Ϣ�ַ�������*/
#define IMOS_DEVICE_CODE_LEN                    IMOS_CODE_LEN

/*@brief �û�������Ϣ�ַ�������*/
#define IMOS_USER_CODE_LEN                      IMOS_CODE_LEN

/*@brief �������Ϣ�ַ�������*/
#define IMOS_DOMAIN_CODE_LEN                    IMOS_CODE_LEN

/*@brief ��������Ϣ�ַ������� */
#define IMOS_DOMAIN_NAME_LEN                    IMOS_NAME_LEN

/*@brief Ȩ�ޱ�����Ϣ�ַ�������*/
#define IMOS_AUTH_CODE_LEN                      IMOS_CODE_LEN

/*@brief ���й�����UA ID���ַ����鳤��*/
#define IMOS_UAID_LEN                           64             /**< ƽ̨֧�ֵ���Ч���λ��Ϊ63λ */

/*@brief UA������Ϣ�ַ�������*/
#define IMOS_UA_CODE_LEN                        IMOS_UAID_LEN

/*@brief ��Ƶ��ǩ��Ϣ�ַ�������*/
#define IMOS_RECORD_LABEL_LEN                   256

/*@brief ����Э�̻ỰID����*/
#define IMOS_SSN_ID_LEN                         IMOS_STRING_LEN_32

/*@brief MP������������ */
#define IMOS_MP_ABILITY_LEN                     IMOS_STRING_LEN_256

/*@brief DecoderTag�ĳ��� */
#define IMOS_DECODERTAG_LEN                     IMOS_STRING_LEN_32

//////////////


/*@brief MAI/AOI��Ϣ��ͨ��ģ�����Ƶ��ַ������� MODULENAME@DEVCODE */
#define IMOS_MODULE_NAME_LEN                    64 /**< ƽ̨֧�ֵ���Ч���λ��Ϊ63λ */

/*@brief mac��ַ�ַ������� "AA-BB-CC-DD-EE-FF" */
#define IMOS_MAC_ADDR_LEN                       20

/*@brief imos_addr IP��ַ��Ϣ�ַ�������
 * IPV4: "192.168.0.102"
 * IPV6: "A1:0:8:100:200:0:0:321A"
 * ����: "AS_server_hangzhou" */
#define IMOS_IPADDR_LEN                         64

/*@brief ��Ч��IP��ַ���� */
#define IMOS_IPADDR_TYPE_INVALID                (0xFFFFFFFF)

/*@brief IPV4��ַ���� */
#define IMOS_IPADDR_TYPE_IPV4                   1

/*@brief IPV6��ַ���� */
#define IMOS_IPADDR_TYPE_IPV6                   2

/*@brief ������ַ���� */
#define IMOS_IPADDR_TYPE_DOMAINNAME             16

/*@brief ������Ϣ�ַ������� */
#define IMOS_PASSWD_LEN                         16

/*@brief ���Ŀ�����ܺ��ַ������� */
#define IMOS_PASSWD_ENCRYPT_LEN                 64  /**< ƽ̨֧�ֵ���Ч���λ��Ϊ63λ */

/*@brief imos_time ʱ����Ϣ�ַ������� "2008-10-02 09:25:33.001 GMT" */
#define IMOS_TIME_LEN                           32

/*@brief imos_simple_time ʱ����Ϣ�ַ������� "hh:mm:ss" */
#define IMOS_SIMPLE_TIME_LEN                    12

/*@brief imos_simple_date ������Ϣ�ַ������� "YYYY-MM-DD"*/
#define IMOS_SIMPLE_DATE_LEN                    12

/*@brief imos_description ������Ϣ�ַ������� */
#define IMOS_DESC_LEN                           (128 * 3)

/*@brief �̶��绰�����ַ������� */
#define IMOS_PHONE_LEN                          32

/*@brief �ƶ��绰�����ַ������� */
#define IMOS_MOBILE_LEN                         32

/*@brief E-Mail�ַ������� */
#define IMOS_MAIL_LEN                           64

/*@brief ��������������� */
#define IMOS_MAIL_CODE_LEN                      8

/*@brief ͨ�ŵ�ַ�ַ������� */
#define IMOS_CONTACT_ADDRESS_LEN                64

/*@brief ��Ʒ�����ַ������� */
#define IMOS_PRODUCTNAME_LEN                    32

/*@brief �汾��Ϣ�ַ������� */
#define IMOS_VERSION_INFO_LEN                   64

/*@brief �ļ������� */
#define IMOS_FILE_NAME_LEN                      64

/*@brief �ļ�������(V2) */
#define IMOS_FILE_NAME_LEN_V2                   256

/** @brief �������ַ�������(��ULONG���ֵ����,4�ֽ�FFFFFFFF,ʮ����Ϊ4294967295,��1λ'\0',��11λ) */
#define IMOS_ERR_CODE_LEN                       11

/* Begin Added by wuchengping02739, 2011-07-19 of VVD57853 */
/** @brief XML�ֶ� - �ɹ���Ӧ */
#define IMOS_XML_KEY_RSP_OK "Rsp_Ok"

/** @brief XML�ֶ�ֵͨ�ó���(ʹ��ʱ,ע�ⳤ���Ƿ���) */
#define IMOS_XML_KEY_LEN                        64
/* End Added by wuchengping02739, 2011-07-19 of VVD57853 */

/*@brief �ļ�����·��(·��+�ļ���)���� */
#define IMOS_FILE_PATH_LEN                      256

/*@brief �ļ�����·��(�����ļ���)���� */
#define IMOS_FILE_PATH_WITHOUT_FILENAME_LEN     128

/*@brief ��Դ������֯·������ */
#define IMOS_RES_PATH_LEN                       1024

/*@brief ��չ�ֶγ��� */
#define IMOS_RESERVED_LEN                       64

/*@brief ͨ��16λ�ַ��� */
#define IMOS_STRING_LEN_14                      14

/*@brief ͨ��32λ�ַ��� */
#define IMOS_STRING_LEN_32                      32

/*@brief ͨ��64λ�ַ��� */
#define IMOS_STRING_LEN_64                      64

/*@brief ͨ��128λ�ַ��� */
#define IMOS_STRING_LEN_128                     128

/*@brief ͨ��256λ�ַ��� */
#define IMOS_STRING_LEN_256                     256

/*@brief ͨ��512λ�ַ��� */
#define IMOS_STRING_LEN_512                     512

/*@brief ͨ��1024λ�ַ��� */
#define IMOS_STRING_LEN_1024                    1024

/*@brief ��Դ����� */
#define IMOS_RES_TREE_LEVEL                     12

/*@brief �豸����״̬ */
#define IMOS_DEV_STATUS_ONLINE                  1

/*@brief �豸����״̬ */
#define IMOS_DEV_STATUS_OFFLINE         2

/*@brief �豸����״̬ */
#define IMOS_DEV_STATUS_NORMAL          3

/*@brief �豸����״̬ */
#define IMOS_DEV_STATUS_FAULT           4

/*@brief �豸���� */
#define IMOS_DEV_UNABLE                 0

/*@brief �豸���� */
#define IMOS_DEV_ENABLE                 1

/*@brief �����ƣ�IMOS_ltoa������ʹ�� */
#define  IMOS_Radix_2                   2

/*@brief �˽��ƣ�IMOS_ltoa������ʹ�� */
#define  IMOS_Radix_8                   8

/*@brief ʮ���ƣ�IMOS_ltoa������ʹ�� */
#define  IMOS_Radix_10                  10

/*@brief ʮ�����ƣ�IMOS_ltoa������ʹ�� */
#define  IMOS_Radix_16                  16


/*@brief IMOS_ID ��Ӧ�ַ����ĳ��ȣ�64λ����IDת����������Ч�ַ�����Ϊ20�� */
#define  IMOS_ID_STRING_LEN             24

/*@brief ����¼�������� */
#define  IMOS_BAK_CASE_LEN              128


/*@brief ������ */
#define IMOS_SUN            0

/*@brief ����һ */
#define IMOS_MON            1

/*@brief ���ڶ� */
#define IMOS_TUE            2

/*@brief ������ */
#define IMOS_WED            3

/*@brief ������ */
#define IMOS_THU            4

/*@brief ������ */
#define IMOS_FRI            5

/*@brief ������ */
#define IMOS_SAT            6

/*@brief һ�� */
#define IMOS_JAN            0

/*@brief ���� */
#define IMOS_FEB            1

/*@brief ���� */
#define IMOS_MAR            2

/*@brief ���� */
#define IMOS_APR            3

/*@brief ���� */
#define IMOS_MAY            4

/*@brief ���� */
#define IMOS_JUN            5

/*@brief ���� */
#define IMOS_JUL            6

/*@brief ���� */
#define IMOS_AUG            7

/*@brief ���� */
#define IMOS_SEP            8

/*@brief ʮ�� */
#define IMOS_OCT            9

/*@brief ʮһ�� */
#define IMOS_NOV            10

/*@brief ʮ���� */
#define IMOS_DEC            11

/* @brief SG ID��Ϣ�ַ�������, SG ID���ɹ���SG����&5λ����� */
#define IMOS_SG_ID_LEN   (IMOS_DEVICE_CODE_LEN + 6)

/*@brief ÿ����Ƶͨ��֧�ֵ��������Ŀ  */
#define IMOS_STREAM_MAXNUM                  2

/*@brief ÿ����Ƶͨ��֧�ֵ��������Ŀ,Ŀǰ�������͵�EC�Դ˱�׼  */
#define IMOS_STREAM_MAXNUM_II                  3

/*@brief ÿ����Ƶͨ��֧�ֵ����������Ŀ  */
#define IMOS_AREA_MAXNUM                    12

/*@brief ��ͨ���ڸ�����������Ŀ */
#define IMOS_MASK_AREA_MAXNUM               4

/*@brief ��ͨ���������������Ŀ */
#define IMOS_DETECT_AREA_MAXNUM             4

/*@brief ÿ����Ƶͨ��֧�ֵ����OSD��Ŀ ����osd + ͼƬosd  */
#define IMOS_OSD_NAME_MAXNUM                1

/*@brief ÿ����Ƶͨ��֧�ֵ����OSD��Ŀ ����osd + ͼƬosd Ŀǰ����EC2004-HF��EC2016-HC�Դ˱�׼ */
#define IMOS_OSD_NAME_MAXNUM_II             2

/*@brief ÿ����Ƶͨ��֧�ֵ����OSD��Ŀ ����osd + ͼƬosd Ŀǰ�������͵�DC�Դ˱�׼ */
#define IMOS_OSD_NAME_MAXNUM_I              1

/*@brief ÿ����Ƶͨ��֧�ֵ����OSD��Ŀ ����osd + ͼƬosd Ŀǰ��EC2004-HF��EC2016-HC�����EC�豸�Դ˱�׼ */
#define IMOS_OSD_NAME_MAXNUM_IV             4

/*@brief ÿ����Ƶͨ��֧�ֵ����OSD��Ŀ ����osd + ͼƬosd ĿǰEC1001�Դ˱�׼ */
#define IMOS_OSD_NAME_MAXNUM_VIII             8

/* Modified by wuchengping02739, 2011-12-16 of VVD63549 */
/*@brief ����OSD��������  */
#define IMOS_OSD_NAME_MAX                     9

/*@brief ʱ��OSD��������  */
#define IMOS_OSD_TIME_MAX                   1

/** @brief ����OSDȱʡֵ */
#define IMOS_NAME_OSD_DEFAULT_VALUE        "H3C"

/** @brief �л�ʱ����ȱʡֵ */
#define IMOS_INTERVAL_DEFAULT_VALUE         0

/*@brief ÿ��ͨ������ѡ���MS���������  */
#define IMOS_MS_MAX_NUM_PER_CHANNEL         1


/*@brief �ƻ���ÿ���ʱ��Ƭ����  */
#define IMOS_MAX_TIME_SLICE_NUM             4

/*@brief �ƻ���ÿ���ʱ��Ƭ����V2�汾  */
#define IMOS_MAX_TIME_SLICE_NUM_V2          16

/*@brief �ƻ���ÿ�ܵ�����  */
#define IMOS_WEEK_DAYS                      7

/*@brief �ƻ������������  */
#define IMOS_EXCP_DAYS                      16

/*@brief ע��/ע��ҵ����� */
#define  IMOS_SRV_CODE_REGISTER             1001

/*@brief �����·�ҵ����� */
#define  IMOS_SRV_CODE_CONFIG               1002

/*@brief �ļ�����/����ҵ����� */
#define  IMOS_SRV_CODE_FILE                 1003

/*@brief Ѳ��ҵ����� */
#define  IMOS_SRV_CODE_CHECK                1004

/*@brief ʵʱ��Ƶҵ����� */
#define  IMOS_SRV_CODE_MONITOR              2001

/*@brief �����Խ�ҵ����� */
#define  IMOS_SRV_CODE_VOICE_TALKBACK       2011

/*@brief �����㲥ҵ����� */
#define  IMOS_SRV_CODE_VOICE_BROADCAST      2012

/*@brief ����Ƶ�Խ�ҵ����� */
#define  IMOS_SRV_CODE_MON_TALKBACK         2013

/*@brief ����Ƶ�㲥ҵ����� */
#define  IMOS_SRV_CODE_MON_BROADCAST        2014

/*@brief ¼���ѯҵ����� */
#define  IMOS_SRV_CODE_REC_QUERY            3001

/*@brief ¼������ҵ����� */
#define  IMOS_SRV_CODE_REC_DOWNLOAD         3002

/*@brief ¼��ط�ҵ����� */
#define  IMOS_SRV_CODE_REC_VOD              3003

/*@brief ¼�񱸷�ҵ����� */
#define  IMOS_SRV_CODE_REC_BACKUP           3004

/*@brief ¼���ȡURLҵ����� */
#define  IMOS_SRV_CODE_REC_URL              3005

/* added by z06806 for vodwall prj 20101220 begin */
/*@brief �ط���ǽҵ����� */
#define  IMOS_SRV_CODE_VODWALL              3005
/* added by z06806 for vodwall prj 20101220 end */

/*@brief ֱ�Ӵ洢ҵ����� */
#define  IMOS_SRV_CODE_DIR_STROE            4001

/*@brief �����洢ҵ����� */
#define  IMOS_SRV_CODE_STR_STORE            4002

/*@brief ǰ�˴洢ҵ����� */
#define  IMOS_SRV_CODE_TP_STORE             4003

/*@brief ˢ�´洢�������� */
#define  IMOS_SRV_CODE_REFRESH_STORE_IDX    4004

/*@brief ��̨����ҵ����� */
#define  IMOS_SRV_CODE_PTZ                  5001

/*@brief ����������ҵ����� */
#define  IMOS_SRV_CODE_SWITCH               5002

/*@brief ͸��ͨ��ҵ����� */
#define  IMOS_SRV_CODE_TRANS_CHANNEL        6001

/*@brief ͸�����ݴ���ҵ����� */
#define  IMOS_SRV_CODE_TRANS_DATA           6002

/*@brief �澯����ҵ����� */
#define  IMOS_SRV_CODE_ALARM                7001

/*@brief ��/����ҵ����� */
#define  IMOS_SRV_CODE_ALARM_CTRL           7002

/*@brief �澯��ѯҵ����� */
#define  IMOS_SRV_CODE_ALARM_QUERY          7003

/*@brief �豸����ҵ����� */
#define  IMOS_SRV_CODE_DEVICE_SHARE         8001 /* Added by MultiDomain */

/*@brief �����ϱ�ҵ����� */
#define IMOS_SRV_CODE_SUBSCRIBE             9001

/*@brief �������ͬ��ҵ����� */
#define IMOS_SRV_CODE_DOMAIN_DATASYN        9002

/*@brief ��������ҵ����� */
#define IMOS_SRV_CODE_LAYOUT_RES_SHARE      8002

/*@brief ������Ȩ��ĳ��� */
#define IMOS_LICENSE_AUTHORITY_CODE_LENGTH (32 + 4)

/*@brief �������ͣ��ڸ����� */
#define IMOS_VIDEO_AREA_TYPE_MASK           0

/*@brief �������ͣ��ڵ����� */
#define IMOS_VIDEO_AREA_TYPE_COVER_DETECT   1

/*@brief �������ͣ��ƶ�������� */
#define IMOS_VIDEO_AREA_TYPE_MOTION_DETECT  2

/*@brief ��Ƶ�������ͨ��������� */
#define IMOS_VOUT_CHANNEL_MAXNUM            4

/*@brief ��Ƶ�������ͨ��ȱʡ���� */
#define IMOS_VOUT_CHANNEL_DEFAULT_NUM       1

/*@brief BP֧�ֵ�Ԥ����������ÿ�������������󳤶� */
#define IMOS_MAX_CMD_PARAM_LEN              512

/*@brief BP֧�ֵ�Ԥ����������֧�ֵ����������� */
#define IMOS_MAX_CMD_PARAM_NUM              6

/*@brief gb2312�������� */
#define IMOS_CODE_TYPE_GB2312 "gb2312"

/*@brief utf-8�������� */
#define IMOS_CODE_TYPE_UTF8 "utf-8"

/*@brief gbk�������� */
#define IMOS_CODE_TYPE_GBK "gbk"

/*@brief IMOSĬ���豸���� */
#define IMOS_DEVICE_TYPE_DESC_DEFAULT       "defaultType"

/*@brief IMOS֧�ֵ��豸���͸���(EC/DC/MS/DM/VX500/����ͷ/������/ECR/BM/IPSAN) */
#define IMOS_DEVICE_TYPE_NUM                10

/*@brief NTP������ͬ����Ϣ�곤�� */
#define IMOS_NTP_SYNC_STATUS_LEN            32

/*@brief ���������̵�����ID��Сֵ */
#define IMOS_MIN_THIRD_MANUFACTURERID        1

/*@brief ���������̵�����ID���ֵ(�û�������) */
#define IMOS_MAX_THIRD_MANUFACTURER_ID       60000

/*@brief �������豸���͵�����ID��Сֵ */
#define IMOS_MIN_THIRD_DEV_TYPE_ID           20000

/*@brief �������豸���͵�����ID���ֵ(�û�������) */
#define IMOS_MAX_THIRD_DEV_TYPE_ID           40000

/*@brief �������澯���͵�����ID��Сֵ */
#define IMOS_MIN_THIRD_EVT_TYPE_ID           20000

/*@brief �������澯���͵�����ID���ֵ(�û�������) */
#define IMOS_MAX_THIRD_EVT_TYPE_ID           40000

/*@brief �������澯���������ݿ��в����� */
#define IMOS_NO_EXIST_EVENT_TYPE_ID          0

/*@brief �����豸���� */
#define IMOS_DEVICE_TYPE_ALL                 65535

/*@brief ���������̵�����ID���ֵ(�������ó�������) */
#define IMOS_MAX_THIRD_MANUFACTURER_ID_ALL   65535

/*@brief �������豸���͵�����ID���ֵ(���������豸����) */
#define IMOS_MAX_THIRD_DEV_TYPE_ID_ALL       50000

/*@brief �������澯���͵�����ID���ֵ(�������ø澯����) */
#define IMOS_MAX_THIRD_EVT_TYPE_ID_ALL       50000

/*@brief ���Ĺ������������� */
#define IMOS_SUBSC_ITEM_MAX_NUM             8

/*@brief 1���ӵ����� */
#define IMOS_SECOND_IN_MIN                  60

/*@brief 1��Сʱ������ */
#define IMOS_SECOND_IN_HOUR                 3600

/** @brief һ���澯���Ĺ���֧�ֵ��û�������� */
#define IMOS_ALARM_SEND_USER_MAXNUM         256

/** @brief һ���澯���Ĺ���֧�ֵĸ澯Դ������� */
#define IMOS_ALARM_SEND_SRC_MAXNUM          2000

/** @brief ϵͳ֧�ֵĸ澯���Ĺ���������� */
#define IMOS_ALARM_SEND_RULE_MAXNUM         3000

/** @brief ��Ϊ��������������� */
#define IMOS_BEHAVIOR_RULE_MAX  8

/** @brief �����ַ������� 16 */
#define IMOS_COORDINATE_STR_LEN_16  16

/** @brief �����ַ������� 56 */
#define IMOS_COORDINATE_STR_LEN_56  56

/* Added by wuchengping02739, 2011-12-16 of VVD63517 */
/** @brief ��Դ����չ��Ϣ���� 512 */
#define IMOS_RES_GROUP_EXT_LEN 512

/** @brief ���ַ��� */
#define IMOS_NULL_STRING  ""

/* Begin Added by ykf0030, 2012-2-27 of ������Ӧ�� */
/** @brief ECʵ��������Ӧ����Ĭ��ֵ */
#define IMOS_MON_NETWORK_ADAPT_ENABLE_DEFAULT    2

/** @brief EC�洢������Ӧ����Ĭ��ֵ */
#define IMOS_STORE_NETWORK_ADAPT_ENABLE_DEFAULT  2

/** @brief EC������ӦģʽĬ��ֵ */
#define IMOS_NETWORK_ADAPT_MODE_DEFAULT  1

/** @brief ECʵ����������Ӧ�ָ�ʱ��Ĭ��ֵ */
#define IMOS_NETWORK_ADAPT_COUNT_DEFAULT  30
/* End Added by ykf0030, 2012-2-27 of ������Ӧ�� */

/** @brief ����rgb,������ɫֵ */
#define COLOR(r, g, b)  ((ULONG) (((UCHAR) (r) | ((USHORT) ((UCHAR) (g)) << 8)) | (((ULONG) (UCHAR) (b)) << 16)))

/**
 * @enum tagIMOSBehaviorType
 * @brief ��Ϊ����
 * @attention
 */
typedef enum tagIMOSBehaviorType
{
    IMOS_BEHAVIOR_TYPE_THRU_LINE    = 0,    /**< ���� */
    IMOS_BEHAVIOR_TYPE_INOUT_AREA   = 1,    /**< �������� */

    IMOS_BEHAVIOR_TYPE_MAX,
    IMOS_BEHAVIOR_TYPE_INVALID      = 0XFFFFFFFF
} IMOS_BEHAVIOR_TYPE_E;

/** �豸����״̬ö�� */
typedef enum tagIMOSDevExtStatus
{
    IMOS_EXT_STATUS_NORMAL   = 0,                /**< ����״̬ */
    IMOS_EXT_STATUS_VIDEO_LOST_RESUME = 1,       /**< ��Ƶ�ָ� */
    IMOS_EXT_STATUS_VIDEO_LOST = 2,              /**< ��Ƶ��ʧ */
    IMOS_EXT_STATUS_MAX,                         /**< ���ֵ */
    IMOS_EXT_STATUS_INVALID    = 0xFFFFFFFF      /**< ��Чֵ */
}IMOS_DEV_EXT_STATUS_E;

/**
* @enum tagIMOSDevBelongAttr
* @brief  �豸��������ö��
* @attention ��
*/
typedef enum tagIMOSDevBelongAttr
{
    IMOS_DEV_LOCAL = 0,                        /**< ���ڱ�����豸 */
    IMOS_DEV_FOREIGN = 1,                      /**< ����������豸 */
    IMOS_DEV_BELONG_ALL = 2,                   /**< �����豸 */
    IMOS_DEV_BELONG_MAX,                       /**< ���ֵ */
    IMOS_DEV_BELONG_INVALID = 0xFFFFFFFF       /**< ��Чֵ */
}IMOS_DEV_BELONG_ATTR;

/**
* @enum tagIMOSFaultDevStatus
* @brief  �豸����״̬ö��ѡ��
* @attention ��
*/
typedef enum tagIMOSFaultDevStatus
{
    IMOS_DEV_FAULT_STATUS_ONLINE = 0,          /**< �豸�������� */
    IMOS_DEV_FAULT_STATUS_FAULT = 1,           /**< �豸���߹��� */
    IMOS_DEV_FAULT_STATUS_OFFLINE = 2,         /**< �豸���� */
    IMOS_DEV_FAULT_STATUS_ALL = 3,             /**< �豸����״̬ */
    IMOS_DEV_FAULT_STATUS_MAX,                 /**< ���ֵ */
    IMOS_DEV_FAULT_STATUS_INVALID = 0xFFFFFFFF /**< ��Чֵ */
}IMOS_DEV_FAULT_STATUS_E;

/**
* @enum tagIMOSDomainDevType
* @brief ���豸����(��Ʒ����������,���Ӳ�Ʒ�Ƕ����ܿ��������豸����)
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

    IMOS_DOMAIN_DEV_TYPE_MAX,                    /**< ���ֵ */
    IMOS_DOMAIN_DEV_TYPE_INVALID     = 0xFFFF    /**< ��Чֵ */
} IMOS_DOMAIN_DEV_TYPE_E;

/**
* @enum tagIMOSDBConnectionNum
* @brief ���ݿ����Ӻ���
* @attention ��
*/
typedef enum tagIMOSDBConnectionNum
{
    IMOS_DB_CONNECTION_CM = 1,                  /**< ����ģ�����ݿ����Ӻ��� */
    IMOS_DB_CONNECTION_MAX,                     /**< ���ֵ */
    IMOS_DB_CONNECTION_INVALID = 0xFFFFFFFF     /**< ��Чֵ */
}IMOS_DB_CONNECTION_NUM_E;

/**
* @enum tagIMOSDomainDeviceType
* @brief ���豸����(����VM�豸,Ϊ��Ʒ����������)
* @attention
*/
typedef enum tagIMOSDomainDeviceType
{
    /* VM�豸(����"IMOSƽ̨һ�״���"������,��Ʒ���γɵ��豸)������:0��99 */
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

    /* H3C�������豸:100�� */
    IMOS_DOMAIN_DEVICE_TYPE_DA8500      = 100,     /**< DA8500 */

    IMOS_DOMAIN_DEVICE_TYPE_MAX,                   /**< ���ֵ */
    IMOS_DOMAIN_DEVICE_TYPE_INVALID     = 0xFFFF   /**< ��Чֵ */
} IMOS_DOMAIN_DEVICE_TYPE_E;

/**
* @enum tagIMOSBooleanType
* @brief ����������
* @attention ��
*/
typedef enum tagIMOSBooleanType
{
    IMOS_BOOLEAN_TYPE_IN = 0,                   /**< ���뿪���� */
    IMOS_BOOLEAN_TYPE_OUT = 1,                  /**< ��������� */
    IMOS_BOOLEAN_TYPE_NONE = 2,                 /**< ���п��������� */

    IMOS_BOOLEAN_TYPE_MAX,                      /**< ���ֵ */
    IMOS_BOOLEAN_TYPE_INVALID = 0xFFFFFFFF      /**< ��Чֵ */
}IMOS_BOOLEAN_TYPE_E;

/**
* @name ���豸����(�ַ���ֵ)
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

/** @brief ��H3C���� */
#define IMOS_DOMAIN_DEV_TYPE_OTHER_STR          "Other"
/** @} */

/**
* @enum tagIMOSPhyOutChnlWorkingMode
* @brief �������ͨ���Ĺ���ģʽ
* @attention
*/
typedef enum tagIMOSPhyOutChnlWorkingMode
{
    IMOS_PCWM_1_SPLIT   = 1,    /**< 1���� */
    IMOS_PCWM_4_SPLIT   = 4,    /**< 4���� */
    IMOS_PCWM_6_SPLIT   = 6,    /**< 6���� */
    IMOS_PCWM_8_SPLIT   = 8,    /**< 8���� */
    IMOS_PCWM_9_SPLIT   = 9,    /**< 9���� */
    IMOS_PCWM_10_SPLIT  = 10,   /**< 10���� */
    IMOS_PCWM_13_SPLIT  = 13,   /**< 13���� */
    IMOS_PCWM_16_SPLIT  = 16,   /**< 16���� */

    IMOS_PCWM_BUTT
} IMOS_PHY_OUT_CHNL_WORKING_MODE_E;

/** @brief ���¸澯�¶����� - ��С */
#define IMOS_LOW_TEMPERATURE_ALARM_MIN -100

/** @brief ���¸澯�¶����� - ��� */
#define IMOS_LOW_TEMPERATURE_ALARM_MAX 49

/** @brief ���¸澯�¶����� - ��С */
#define IMOS_HIGH_TEMPERATURE_ALARM_MIN 50

/** @brief ���¸澯�¶����� - ��� */
#define IMOS_HIGH_TEMPERATURE_ALARM_MAX 104

/**
* @enum tagIMOSNatLevel
* @brief Nat����
* @attention ��
*/
typedef enum tagIMOSNatLevel
{
    IMOS_NAT_LEVEL_LOWER = 1,                   /**< nat����ϵͣ�����ȱ��򣬱��ȽϷ���nat�� */
    IMOS_NAT_LEVEL_EQUAL = 2,                   /**< ͬ���𣬼���ȱ���û�о���nat */
    IMOS_NAT_LEVEL_UPPER = 3,                   /**< nat����ϸߣ�����ȱ��򣬱�����nat�ڣ����ȽϷ��ڹ��� */

    IMOS_NAT_LEVEL_INVALID = 0xFFFFFFFF         /**< ��Чֵ */
}IMOS_NAT_LEVEL_E;


#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_DEF_H_ */

