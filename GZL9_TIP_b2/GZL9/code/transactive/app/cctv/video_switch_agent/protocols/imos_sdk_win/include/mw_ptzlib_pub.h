/*******************************************************************************
 Copyright (c) 2009, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              mw_ptzlib_pub.h
  Project Code: IMOS
   Module Name: PTZ_LIB
  Date Created: 2009-3-25
        Author: ������/05305
   Description:��̨ͨ�ÿ�ͷ�����ļ�
   Caution:      ��̨Э������ʹ����ȫ�ֱ������������ڶ��̻߳���
--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/

#ifndef _MW_PTZLIB_PUB_H_
#define _MW_PTZLIB_PUB_H_

#ifdef  __cplusplus
extern "C"{
#endif

#define MW_PTZ_CMD_LEN 16  /**< ��̨Э����������󳤶� */
#define MW_PTZLIB_NAME_LEN 20 /**< ��̨Э��������󳤶ȣ������˺�׺���ĳ���4 */
#define MW_PTZ_FACTORY_PATH_LEN 128 /**< ��̨Э��Ŀ¼����*/


/*������̨�������
������̨��ɹ�CC��MW����ʹ�ã�
����ʹ���ڲ������뷽ʽ
���赥�����������*/
#define ERR_PTZLIB_SUCCESS 0UL   /**<  ��̨�ⷵ�سɹ�*/
#define ERR_PTZLIB_GENERAL 1UL /**<  ��̨��ͨ�ô��� ��Ҫ�ǲ������Ϸ���
                                                        �ļ��޷��򿪡������ڴ�ʧ�ܵ�*/
#define ERR_PTZLIB_FACTORY_NOTEXIST 2UL  /**<  ��̨ģ�岻���� */
#define ERR_PTZLIB_CMD_NOTSUPPORT 3UL  /**<  ��̨���֧�� */
#define ERR_PTZLIB_FACTORY_INVALID 4UL  /**<  ��̨ģ�岻�Ϸ�*/
/* Begin Added by l05305, 2009-7-3 of VVD29490 */
#define   MW_PTZ_SPEEDSET 0xffffffff
/* End   Added by l05305, 2009-7-3 of VVD29490 */

/**
* @enum  tagMwPtzCmdEnum
* @brief ��̨��������ö��
* @attention
*/
typedef enum tagMwPtzCmdEnum
{
    MW_PTZ_IRISCLOSESTOP        = 0x0101,       /**< ��Ȧ��ֹͣ */
    MW_PTZ_IRISCLOSE            = 0x0102,       /**< ��Ȧ�� */
    MW_PTZ_IRISOPENSTOP         = 0x0103,       /**< ��Ȧ��ֹͣ */
    MW_PTZ_IRISOPEN             = 0x0104,       /**< ��Ȧ�� */

    MW_PTZ_FOCUSNEARSTOP        = 0x0201,       /**< ���ۼ�ֹͣ */
    MW_PTZ_FOCUSNEAR            = 0x0202,       /**< ���ۼ� */
    MW_PTZ_FOCUSFARSTOP         = 0x0203,       /**< Զ�ۼ� ֹͣ*/
    MW_PTZ_FOCUSFAR             = 0x0204,       /**< Զ�ۼ� */
    
    MW_PTZ_ZOOMTELESTOP         = 0x0301,       /**< �Ŵ�ֹͣ */
    MW_PTZ_ZOOMTELE             = 0x0302,       /**< �Ŵ� */
    MW_PTZ_ZOOMWIDESTOP         = 0x0303,       /**< ��Сֹͣ */
    MW_PTZ_ZOOMWIDE             = 0x0304,       /**< ��С */

    MW_PTZ_TILTUPSTOP           = 0x0401,       /**< ����ֹͣ */
    MW_PTZ_TILTUP               = 0x0402,       /**< ���� */
    MW_PTZ_TILTDOWNSTOP         = 0x0403,       /**< ����ֹͣ */
    MW_PTZ_TILTDOWN             = 0x0404,       /**< ���� */

    MW_PTZ_PANRIGHTSTOP         = 0x0501,       /**< ����ֹͣ */
    MW_PTZ_PANRIGHT             = 0x0502,       /**< ���� */
    MW_PTZ_PANLEFTSTOP          = 0x0503,       /**< ����ֹͣ */
    MW_PTZ_PANLEFT              = 0x0504,       /**< ���� */

    MW_PTZ_PRESAVE              = 0x0601,       /**< Ԥ��λ���� */
    MW_PTZ_PRECALL              = 0x0602,       /**< Ԥ��λ���� */
    MW_PTZ_PREDEL               = 0x0603,       /**< Ԥ��λɾ�� */

    MW_PTZ_LEFTUPSTOP           = 0x0701,       /**< ����ֹͣ */
    MW_PTZ_LEFTUP               = 0x0702,       /**< ���� */
    MW_PTZ_LEFTDOWNSTOP         = 0x0703,       /**< ����ֹͣ */
    MW_PTZ_LEFTDOWN             = 0x0704,       /**< ���� */

    MW_PTZ_RIGHTUPSTOP          = 0x0801,       /**< ����ֹͣ */
    MW_PTZ_RIGHTUP              = 0x0802,       /**< ���� */    
    MW_PTZ_RIGHTDOWNSTOP        = 0x0803,       /**< ����ֹͣ */
    MW_PTZ_RIGHTDOWN            = 0x0804,       /**< ���� */

    MW_PTZ_ALLSTOP              = 0x0901,       /**< ȫͣ������ */

    MW_PTZ_BRUSHON              = 0x0A01,       /**< ��ˢ�� */
    MW_PTZ_BRUSHOFF             = 0x0A02,       /**< ��ˢ�� */
    
    MW_PTZ_LIGHTON              = 0x0B01,       /**< �ƿ� */
    MW_PTZ_LIGHTOFF             = 0x0B02,       /**< �ƹ� */

    MW_PTZ_HEATON               = 0x0C01,       /**< ���ȿ� */
    MW_PTZ_HEATOFF              = 0x0C02,       /**< ���ȹ� */

    MW_PTZ_INFRAREDON           = 0x0D01,       /**< ���⿪ */
    MW_PTZ_INFRAREDOFF          = 0x0D02,       /**< ����� */

    MW_PTZ_SCANCRUISE           = 0x0E01,       /**< ��̨����ɨè */
    MW_PTZ_SCANCRUISESTOP       = 0x0E02,       /**< ��̨����ɨè */
    
    MW_PTZ_TRACKCRUISE          = 0x0F01,       /**<  ��̨�켣Ѳ�� */
    MW_PTZ_TRACKCRUISESTOP      = 0x0F02,       /**<  ��̨�켣Ѳ�� */
    
    MW_PTZ_PRESETCRUISE         = 0x1001,       /**<  ��̨��Ԥ��λѲ�� ���������ֲ�����̨ģ������ */
    MW_PTZ_PRESETCRUISESTOP     = 0x1002,       /**<  ��̨��Ԥ��λѲ�� ֹͣ���������ֲ�����̨ģ������ */

    /* Begin: Added by t07041 20110117, ����Ŵ����� */
    MW_PTZ_AREAZOOMIN           = 0x1101,       /**<  ����Ŵ� */
    MW_PTZ_AREAZOOMOUT          = 0x1102,       /**<  ������С */
    /* End: Added by t07041 20110117, ����Ŵ����� */

    MW_PTZ_CMD_BUTT
}MW_PTZ_CMD_E;


/**
* @struct  tagMwPtzCmdBuffer
* @brief ��̨����ָ��buffer�ͷ��Ͳ���
* @attention
 */
typedef struct tagMwPtzCmdBuffer
{
    ULONG ulCmdLen;  /**<  ����ȣ�ÿ������һ�����ȣ�����ɱ�������ڵ����� */
    ULONG ulCmdIntervalDelay;     /**<   ����ĳЩЭ����Ҫһ��������һ��ʱ���ٴη���  ��λ�Ǻ��뷵��0�Ļ�����Ҫ���*/
    ULONG ulCmdSendTimes;     /**<   ����ĳЩЭ����Ҫ�ظ���η��ͣ�����������ʶ���Ͷ��ٴΣ�Ĭ����1*/
    UCHAR ucCmdBuf[MW_PTZ_CMD_LEN];/**<  ������Buffer*/
    /* Begin Added by l05305, 2009-7-3 of VVD29490 */
    BOOL_T bIsSupportSpeedSet;/* �Ƿ�֧���ٶ����� */
    /* End   Added by l05305, 2009-7-3 of VVD29490 */
}MW_PTZ_CMD_BUF_S;

/**
* @struct  tagMwPtzOperation
* @brief ��̨����ָ�����
* @attention
 */
typedef struct tagMwPtzOperation
{
    MW_PTZ_CMD_E enCmdType;     /**< ��̨�������� */
    ULONG ulCmdParam1;           /**< ����1 ���������д�ο��ĵ� */
    ULONG ulCmdParam2;           /**< ����2 ���������д�ο��ĵ� */
    ULONG ulCmdParam3;           /**< ����3 ���������д�ο��ĵ� */
}MW_PTZ_OPER_S;


/** @defgroup groupPtzInit ��̨��ز����Ľӿ�                                           
*   ��ģ���ṩ��̨��ĳ�ʼ��������Լ���ز����Ľӿڣ���ϸ�ĵ���������ϵ��μ�SDK˵���ĵ���
*   @{
*/
/**
* ��̨���ʼ����������ʼ��ָ��Ŀ¼�µ�������̨Э��  
�κ�ʹ�øÿ��ģ�鶼�������ȳ�ʼ��\n
* @param [IN] CHAR *pcFactoryPath       ��̨Э��·��
* @return �������½��:
* - �ɹ�:
* - #ERR_PTZLIB_SUCCESS
* - ʧ��:
* - #ERR_PTZLIB_GENERAL
* @note ɨ��Ŀ¼ֻ��ɨ���׺��.ini���ļ���
              ����ζ��Э�����Ʊ�����ini��׺
*/
IMOS_EXPORT ULONG MW_PTZLIB_Init(IN CHAR *pcFactoryPath);

/**
* ��̨�����ٺ���\n
* @param ��
* @return �ú����޷���ֵ
* @note 
*/
IMOS_EXPORT VOID MW_PTZLIB_Destroy(VOID);

/**
* ��̨��װĳ����̨Э��ģ��\n
* @param [IN] CHAR *pcFactoryName       ��̨ģ������
* @param [IN] CHAR *pcFilePathName       ��̨Э��·��
* @return �������½��
* - �ɹ�:
* - #ERR_PTZLIB_SUCCESS
* - ʧ��:
* - #ERR_PTZLIB_FACTORY_INVALID
* - #ERR_PTZLIB_GENERAL
* @note 
*/
IMOS_EXPORT ULONG MW_PTZLIB_InstallProtocolFactory(IN CHAR *pcFactoryName, IN CHAR *pcFilePathName);


/**
* ������̨Э�飬��ַ��Ͳ������͵õ���̨buffer\n
* @param [IN] CHAR *pcFactoryName     ��̨Э������       
* @param [IN] MW_PTZ_OPER_S *pstPtzOperation     ��̨��������       
* @param [IN] ULONG ulPtzAddr     ��̨��ַ��       
* @param [OUT] MW_PTZ_CMD_BUF_S *pstCmdBuf     ��̨������buffer       
* @return �������½��
* - �ɹ�:
* - #ERR_PTZLIB_SUCCESS
* - ʧ��:
* - #ERR_PTZLIB_FACTORY_NOTEXIST
* - #ERR_PTZLIB_CMD_NOTSUPPORT
* - #ERR_PTZLIB_GENERAL
* @note 
*/

IMOS_EXPORT ULONG MW_PTZLIB_GetPtzCmdBuffer
(
    IN CHAR *pcFactoryName, 
    IN MW_PTZ_OPER_S *pstPtzOperation, 
    IN ULONG ulPtzAddr, 
    OUT MW_PTZ_CMD_BUF_S *pstCmdBuf
);

/**
* ���Ժ�������ϸ��ʾ��ǰ֧�ֵ�������̨Э��\n
* @param [IN] VOID       
* @return �ú����޷���ֵ
* @note 
*/
IMOS_EXPORT VOID MW_PTZLIB_DumpAllPtzFactory(VOID);


/**
* �����̨Э���Ƿ���� \n
* @param [IN] CHAR *pcFactoryName       ��̨Э������
* @return �������½��:
* - �ɹ�:
* - #BOOL_TRUE
* - ʧ��:
* - #BOOL_FALSE
* @note 
*/
IMOS_EXPORT BOOL_T MW_PTZLIB_CheckPtzTemplateExist(IN CHAR *pcFactoryName);

/** @} */ /* end of ptz Module */

#ifdef  __cplusplus
}
#endif


#endif  /* end of _MW_PTZLIB_PUB_H_  */

