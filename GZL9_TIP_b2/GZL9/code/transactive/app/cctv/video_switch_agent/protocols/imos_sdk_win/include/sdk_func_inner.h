/*******************************************************************************
 Copyright (c) 2008-2009, Hangzhou H3C Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              sdk_func_inner.h

  Project Code: IMOS_SDK
   Module Name: SDK
  Date Created: 2010-04-09
        Author: w05867
   Description: SDK不对外发布的函数接口
                注意: 其中分页查询时，所需列表空间由外部申请，
                      大小根据分页查询时一页的个数进行申请。


--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD

*******************************************************************************/
#ifndef _IMOS_SDK_FUNC_INNER_H_
#define _IMOS_SDK_FUNC_INNER_H_

#include <string>
#include <vector>

#ifdef  __cplusplus
extern "C"{
#endif

/**
* 扫描IPSAN \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szIpsanCode         IPSAN编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_DM_DEV_NOT_ONLINE
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ScanIpsan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szIpsanCode[IMOS_DEVICE_CODE_LEN]
);

/**
* 查询当前登录用户对指定资源的权限列表. \n
* 如果当前登录用户是系统管理员,则只返回"是否是系统管理员"标识,不返回权限列表;
* 如果不是系统管理员,则返回权限列表(包括系统权限和资源权限).
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   ulResType           资源类型(取值#IMOS_TYPE_E)
* @param [IN]   szResCode           资源编码
* @param [OUT]  pbIsSystemMgr       "是否是系统管理员"标识, BOOL_TRUE为系统管理员, BOOL_FALSE为非系统管理员
* @param [OUT]  pstAuthorityInfo    权限列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_QUERY_AUTH_NUM_EXCEED_MAX
* -     其它结果码,参见sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ULONG                   ulResType,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    OUT BOOL_T                  *pbIsSystemMgr,
    OUT ORG_AUTHORITY_S         *pstAuthorityInfo
);

/**
* 划归资源到目标组织 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szDesOrgCode        目标组织编码
* @param [IN]   szResCode           资源编码
* @param [IN]   ulResType           资源类型, 取值自#IMOS_TYPE_E, 目前支持的合法取值为#IMOS_TYPE_EC; #IMOS_TYPE_DC; #IMOS_TYPE_CAMERA; #IMOS_TYPE_MONITOR; #IMOS_TYPE_DM; #IMOS_TYPE_MS; #IMOS_TYPE_VX500
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DOMAIN_NOT_EXIST
* - #ERR_DAO_DEVICE_NOT_EXIST
* - #ERR_MM_DEVICE_ALREADY_IN_DOMAIN
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. 如果该资源没有编码，则把资源名称作为资源编码
* - 2. 一个资源可以划归到多个组织，但不能重复划归到同一个组织
*/
IMOS_EXPORT ULONG STDCALL IMOS_ShareResToDesOrg
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDesOrgCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulResType
);

/**
* 将资源从组织中解除划归 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szDesOrgCode        目标组织编码
* @param [IN]   szResCode           资源编码
* @param [IN]   ulResType           资源类型
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_CAMERA_ALREADY_SHARED
* - #ERR_MM_DEVICE_BELONG_TO_ORIGINAL_DOMAIN
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1、如果该资源没有编码，则把资源名称作为资源编码
*/
IMOS_EXPORT ULONG STDCALL IMOS_ReclaimResFromDesOrg
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDesOrgCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulResType
);

/**
* 查询组织下面的摄像机共享信息列表，只获取一层 \n
* 不填查询条件时,查所有,并默认排序(摄像机名称升序)
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   szOrgCode          组织节点编码
* @param [IN]   szExDomainCode     外域编码
* @param [IN]   pstQueryCondition  通用查询条件(可填NULL;可填条件:NAME_TYPE[摄像机名称])
* @param [IN]   pstQueryPageInfo   请求分页信息
* @param [OUT]  pstRspPageInfo     返回分页信息
* @param [OUT]  pstCamShareInfo    摄像机共享信息列表指针
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCameraShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_CAM_INFO_V2_S             *pstCamShareInfo
);

/**
* 查询某个监视器所对应的解码器信息
* @param [IN] pstUserLogIDInfo              用户登录信息标识
* @param [IN] szScrCode                     监视器编码
* @param [OUT]pstScrRelatedDC               监视器对应的解码器结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryScreenRelatedDC
(
    IN    USER_LOGIN_ID_INFO_S          *pstUserLogIDInfo,
    IN    CHAR                          szScrCode[IMOS_CODE_LEN],
    OUT   SCREEN_RELATED_DC_S           *pstScrRelatedDC
);

/**
* 分页查询建立在设备（摄像机、监视器、播放器窗格）上的实时监控记录 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   ulUAType                设备类型,取值范围见#IMOS_TYPE_E
* @param [IN]   szUACode                摄像机/监视器/窗格编码
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstMonitorUnit          实时监控记录列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_MONITOR_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMonitorRecordUnderUA
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  ULONG                       ulUAType,
    IN  CHAR                        szUACode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT MONITOR_UNIT_S              *pstMonitorUnit
);

/**
* 查询监视器上正在轮切的轮切资源信息 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szMonitorCode           在硬解实况中为监视器编码; 在软解实况中为窗格编码
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSwitchResBase        轮切资源基本信息
* @param [OUT]  pstSwitchResUnit        轮切资源单元信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySwitchResInfoUnderScreen
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLoginIDInfo,
    IN   CHAR                       szMonitorCode[IMOS_RES_CODE_LEN],
    IN   QUERY_PAGE_INFO_S          *pstQueryPageInfo,
    OUT  RSP_PAGE_INFO_S            *pstRspPageInfo,
    OUT  SWITCH_RES_BASE_S          *pstSwitchResBase,
    OUT  SWITCH_RES_UNIT_S          *pstSwitchResUnit
);


/**
* 查询设备上正在进行的业务信息 \n
* @param [IN]   pstUserLoginIDInfo  用户登录ID信息标识
* @param [IN]   szDevCode           设备编码
* @param [IN]   ulDevType           设备类型, 取值为#IMOS_TYPE_E, 合法的取值为#IMOS_TYPE_MONITOR(监视器), #IMOS_TYPE_XP_WIN(XP窗格)
* @param [IN]   szVirDomainCode     虚拟域编码
* @param [OUT]  pstSrvInfo          业务信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前只实现查询实时监控、轮切的业务状态
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServiceUnderDev
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                   ulDevType,
    IN  CHAR                    szVirDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT SRV_STATE_INFO_S        *pstSrvInfo
);

/**
* 查询资源上正在进行的业务信息 \n
* @param [IN]   pstUserLoginIDInfo  用户登录ID信息标识
* @param [IN]   szResCode           资源编码
* @param [IN]   ulResType           资源类型, 取值为#IMOS_TYPE_E, 合法的取值为#IMOS_TYPE_SALVO_RESOURCE(组显示), #IMOS_TYPE_GROUP_SWITCH_RESOURCE(组轮巡)
* @param [IN]   szVirDomainCode     虚拟域编码
* @param [OUT]  pstSrvInfo          业务信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前只实现查询组显示的业务状态
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryServiceUnderRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulResType,
    IN  CHAR                    szVirDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT RES_SRV_STATE_INFO_S    *pstSrvInfo
);

/**
* 分页查询语音广播信息 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szXPVocBrdCode          语音广播编码
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstVocBrdUnit           语音广播单元列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlayerVoiceBroadcastInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szXPVocBrdCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT VOC_BRD_UNIT_S              *pstVocBrdUnit
);

/**
* 查询语音业务信息 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szXPVocTalkCode         语音对讲编码
* @param [IN]   szXPVocBrdCode          语音广播编码
* @param [OUT]  szCameraCode            摄像机编码
* @param [OUT]  szCameraName            摄像机名称
* @param [OUT]  pulVoiceSrvType         语音业务类型, 取值为#SRV_TYPE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_CS_VOCTALK_NOT_EXIST
* - #ERR_CS_VOCBRD_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryXPVoiceServiceInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  CHAR                            szXPVocTalkCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szXPVocBrdCode[IMOS_RES_CODE_LEN],
    OUT CHAR                            szCameraCode[IMOS_RES_CODE_LEN],
    OUT CHAR                            szCameraName[IMOS_RES_CODE_LEN],
    OUT ULONG                           *pulVoiceSrvType
);

/***************************************************************************
配置XP单组播接口
****************************************************************************/
/**
* @name 配置XP单组播接口管理
* @{    配置XP单组播接口管理
*/
/**
* 配置XP单组播信息
* @param [IN]  pstUserLogIDInfo             用户登录ID信息标识
* @param [IN]  ulStreamType                 单组播类型，0为单播，1为组播
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_XP_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.如果找不到xp流信息，调用该接口会返回失败
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigXPStreamType
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulStreamType
);

/**
* 查询XP单组播的信息
* @param [IN]   pstUserLogIDInfo             用户登录ID信息标识
* @param [OUT]  pulStreamType                单组播类型，0为组播，1为单播
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_XP_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryXPStreamType
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulStreamType
);

/**
* 设置本域前端设备最大直连媒体流数量系统参数 \n
* @param [IN]   pstUserLogIDInfo                用户登陆信息
* @param [IN]   ulMaxMediaByPassOfTerminalDev   前端设备最大直连媒体流数量(取值范围1~3)
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. 只有admin用户才有权限
*/
IMOS_EXPORT ULONG STDCALL IMOS_CfgMaxMediaByPassOfTerminalDev
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulMaxMediaByPassOfTerminalDev
);

/**
* 查询本域前端设备最大直连媒体流数量系统参数 \n
* @param [IN]   pstUserLogIDInfo                用户登陆信息
* @param [OUT]  pulMaxMediaByPassOfTerminalDev  前端设备最大直连媒体流数量(取值范围1~3)
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMaxMediaByPassOfTerminalDev
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulMaxMediaByPassOfTerminalDev
);
/** @} */ /* end of XP STREAM MANAGEMENT MODULE */
/** @} */ /* end of INFO MANAGEMENT MODULE */
#if 0
#endif
/*******************************************************************************
 设备-EC,DC串口管理
*******************************************************************************/
/**
* @name 设备(EC,DC)串口管理
* @{    设备(EC,DC)串口管理
*/
/**
* 配置设备的串口信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstChannelIndex         设备通道索引信息
* @param [IN]   pstSerialInfo           串口信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_SERIAL_USED
* - #ERR_DAO_SERIAL_PORT_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfSerial
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S         *pstChannelIndex,
    IN  SERIAL_INFO_S               *pstSerialInfo
);


/**
* 分页查询设备的串口列表 \n
* 条件为空时,查询所有串口,并默认按照"串口索引升序"排序
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szDevCode              设备编码
* @param [IN]   ulDevType              设备类型，取值范围: #IMOS_TYPE_E
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:INDEX_TYPE[串口索引])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstSerialList          串口列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySerialList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                           ulDevType,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SERIAL_QUERY_ITEM_S             *pstSerialList
);

/**
* 查询串口信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstChannelIndex         设备通道索引信息
* @param [OUT]  pstSerialInfo           串口信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_SERIAL_PORT_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySerialInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT SERIAL_INFO_S                   *pstSerialInfo
);

/**
* 分页查询某个EC串口被相关资源占用的信息列表
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstChannelIndex         串口通道索引信息
* @param [IN]   pstQueryPageInfo        请求分页信息(不支持偏移)
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSerialRelationList   串口被相关资源占用的信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1、支持查询的EC串口模式有:云台控制(#IMOS_SE_PTZ)、透明通道(#IMOS_SE_TRANS)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECSerialUsedRelation
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SERIAL_RELATION_ITEM_S          *pstSerialRelationList
);
/** @} */ /* end of SERIAL MANAGEMENT MODULE */

#if 0
#endif
/************************************************************************************************************
 设备-EC,DC OSD图片
************************************************************************************************************/


#if 0
#endif
/**************************************************************************************************************
设备-EC,DC通道，计划以及SNMP模板管理
***************************************************************************************************************/
/** @defgroup groupTempletMg 设备(EC,DC)模板以及计划模板管理
*   设备(EC,DC)模板以及计划模板管理
*   @{
*/
/**
* @name EC模板
* @{    EC模板
*/
/**
* 增加EC通道参数模板 \n
* @param [IN]   pstUserLogIDInfo                用户登录ID信息标识
* @param [IN]   pstSDKECChannelTemplet          EC通道参数模板信息
* @param [OUT]  szECChannelTempletCode          模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  EC_CHANNEL_TEMPLET_S            *pstSDKECChannelTemplet,
    OUT CHAR                            szECChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* 删除EC参数模板 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szECChannelTempletCode      模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szECChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* 修改EC参数模板 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szECChannelTempletCode      模板编码
* @param [IN]   pstSDKECChannelTemplet      EC通道参数模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyECChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szECChannelTempletCode[IMOS_RES_CODE_LEN],
    IN  EC_CHANNEL_TEMPLET_S            *pstSDKECChannelTemplet
);


/**
* 分页查询EC通道参数模板列表 \n
* 条件为空时,查询所有模板,并默认按照"通道号升序排序"排序
* @param [IN]   pstUserLogIDInfo           用户登录ID信息标识
* @param [IN]   pstQueryCondition          通用查询条件(可为NULL;支持的查询条件包括:#DEV_SUB_TYPE[设备子类型]; #NAME_TYPE[模板名称]; #TIME_TYPE[模板更新时间])
* @param [IN]   pstQueryPageInfo           请求分页信息
* @param [OUT]  pstRspPageInfo             响应分页信息
* @param [OUT]  pstSDKECChannelTempList    EC通道参数模板列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECChannelTempletList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S       *pstSDKECChannelTempList
);


/**
* 查询EC参数模板具体信息 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szECChannelTempletCode      模板编码
* @param [OUT]  pstSDKECChannelTemplet      EC通道参数模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VIN_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECChannelTemplet
(
    IN     USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN     CHAR                         szECChannelTempletCode[IMOS_RES_CODE_LEN],
    OUT    EC_CHANNEL_TEMPLET_S         *pstSDKECChannelTemplet
);
/** @} */  /* EC模板结束 */
/**
* @name DC模板
* @{    DC模板
*/
/**
* 增加DC通道参数模板 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstSDKDCChannelTemplet      DC通道参数模板信息
* @param [OUT]  szDCChannelTempletCode      模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  DC_CHANNEL_TEMPLET_S        *pstSDKDCChannelTemplet,
    OUT CHAR                        szDCChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* 删除DC参数模板 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szDCChannelTempletCode      模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szDCChannelTempletCode[IMOS_RES_CODE_LEN]
);

/**
* 修改DC参数模板 \n
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szDCChannelTempletCode      模板编码
* @param [IN]   pstSDKDCChannelTemplet      DC参数模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szDCChannelTempletCode[IMOS_RES_CODE_LEN],
    IN  DC_CHANNEL_TEMPLET_S            *pstSDKDCChannelTemplet
);


/**
* 分页查询DC通道参数模板列表 \n
* 条件为空时,查询所有模板,并默认按照"通道号升序"排序.
* @param [IN]   pstUserLogIDInfo           用户登录ID信息标识
* @param [IN]   pstQueryCondition          通用查询条件(可为NULL;支持的查询条件包括:#DEV_SUB_TYPE[设备子类型]; #NAME_TYPE[模板名称]; #TIME_TYPE[模板更新时间])
* @param [IN]   pstQueryPageInfo           请求分页信息
* @param [OUT]  pstRspPageInfo             响应分页信息
* @param [OUT]  pstSDKDCChannelTempList    DC通道参数模板列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDCChannelTempletList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S       *pstSDKDCChannelTempList
);


/**
* 查询DC参数模板具体信息 \n
* @param [IN]   pstUserLogIDInfo                用户登录ID信息标识
* @param [IN]   szDCChannelTempletCode          模板编码
* @param [OUT]  pstSDKDCChannelTemplet          DC参数模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_VOUT_CHANL_TEMPL_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDCChannelTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szDCChannelTempletCode[IMOS_RES_CODE_LEN],
    OUT DC_CHANNEL_TEMPLET_S        *pstSDKDCChannelTemplet
);
/** @} */  /* DC模板结束 */


/**
* @name 计划模板
* @{    计划模板
*/
/**
* 增加计划模板 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstSDKPlanTemplet       计划模板信息
* @param [OUT]  szPlanTempletCode       模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  PLAN_TEMPLET_S              *pstSDKPlanTemplet,
    OUT CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN]
);


/**
* 删除计划模板 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szPlanTempletCode       模板编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeletePlanTemplet
(
    IN USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN CHAR                             szPlanTempletCode[IMOS_RES_CODE_LEN]
);


/**
* 修改计划模板 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szPlanTempletCode       模板编码
* @param [IN]   pstSDKPlanTemplet       计划模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN],
    IN  PLAN_TEMPLET_S              *pstSDKPlanTemplet
);


/**
* 分页查询计划参数模板列表 \n
* 条件为空时,查询所有模板,并默认按照"模板名称升序"排序
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   pstQueryCondition      通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[模板名称]; #TIME_TYPE[模板更新时间])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstPlanTempList        计划模板信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlanTempletList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CHANNEL_TEMP_QUERY_ITEM_S   *pstPlanTempList
);


/**
* 查询计划参数模板具体信息 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szPlanTempletCode       模板名称
* @param [OUT]  pstSDKPlanTemplet       计划模板信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PLAN_TEMPLET_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryPlanTemplet
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szPlanTempletCode[IMOS_RES_CODE_LEN],
    OUT PLAN_TEMPLET_S              *pstSDKPlanTemplet
);
/** @} */  /* 计划模板结束 */
/** @} */ /* end of TEMPLET MANAGEMENT MODULE */
#if 0
#endif
/** @defgroup groupCruise 巡航业务
*   巡航业务
*   @{
*/
/**
* @name 巡航路线管理
* @{    巡航路线管理
*/
/**
* 创建巡航路线\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstCruisePathInfo   巡航路线信息
* @param [OUT]  szCruisePathCode    巡航路线编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CAMERA_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCruisePath
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CRUISE_PATH_INFO_S      *pstCruisePathInfo,
    OUT CHAR                    szCruisePathCode[IMOS_RES_CODE_LEN]
);


/**
* 删除巡航路线 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePathCode    巡航路线编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);


/**
* 修改巡航路线 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePathCode    巡航路线编码
* @param [IN]   pstCruisePathInfo   巡航路线信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCruisePath
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCruisePathCode[IMOS_RES_CODE_LEN],
    IN  CRUISE_PATH_INFO_S      *pstCruisePathInfo
);


/**
* 分页查询某个摄像机的巡航路线列表 \n
* 条件为空时,查该摄像机的所有巡航路线,并默认"巡航路线名称升序排序"
* @param [IN]   pstUserLogIDInfo       用户信息
* @param [IN]   szCamCode              摄像机编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;可填条件:#NAME_TYPE[路线名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstCruisePathList      巡航路线列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePathList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCamCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CRUISE_PATH_QUERY_ITEM_S    *pstCruisePathList
);


/**
* 查询某个摄像机的巡航路线的详细信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePathCode    巡航路线编码
* @param [OUT]  pstCruisePathInfo   巡航路线信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_PRESET_VALUE_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePathInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN],
    OUT CRUISE_PATH_INFO_S          *pstCruisePathInfo
);

/**
* 手动启动巡航路线
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePathCode    巡航路线编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_DAO_CRUISE_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - 其他返回操作结果码：见结果码文件
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);

/**
* 手动停止巡航路线
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePathCode    巡航路线编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_DAO_CRUISE_NOT_EXIST
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - 其他返回操作结果码：见结果码文件
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCruisePath
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePathCode[IMOS_RES_CODE_LEN]
);

/**
* 停止摄像机下的所有手动巡航路线
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCameraCode        摄像机编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_NO_OUTDOMAIN_OPERATE_CRUISE
* - #ERR_AS_CAMERA_NOT_EXIST
* - #ERR_AS_CAMERA_OFFLINE
* - #ERR_AS_CAMERA_NOT_PTZTYPE
* - #ERR_PTZ_AUTHORITY_LOW
* - 其他返回操作结果码：见结果码文件
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopAllCruisePathByCam
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCameraCode[IMOS_DEVICE_CODE_LEN]
);
/** @} */ /* 巡航路线结束 */

#if 0
#endif
/***************************************************************************
巡航计划
****************************************************************************/
/**
* @name 巡航计划管理
* @{    巡航计划管理
*/
/**
* 创建巡航计划\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstCruisePlanInfo   巡航计划信息
* @param [OUT]  szCruisePlanCode    巡航计划编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CAMERA_NOT_EXIST
* - #ERR_DAO_CRUISE_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CRUISE_PLAN_INFO_S          *pstCruisePlanInfo,
    OUT CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* 修改巡航计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePlanCode    巡航计划编码
* @param [IN]   pstCruisePlanInfo   巡航计划信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN],
    IN  CRUISE_PLAN_INFO_S          *pstCruisePlanInfo
);


/**
* 删除巡航计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePlanCode    巡航计划编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* 查询巡航计划\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePlanCode    巡航计划编码
* @param [OUT]  pstCruisePlanInfo   巡航计划信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISEID_NOT_EXIST
* - #ERR_DAO_CAMERA_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN],
    OUT CRUISE_PLAN_INFO_S          *pstCruisePlanInfo

);

/**
* 分页查询巡航计划列表 \n
* 条件为空时,查所有巡航计划,并默认"巡航计划名称升序"排序
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szOrgCode              组织编码
* @param [IN]   pstQueryCondition      通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[计划名称]、
* -                                    #TIME_TYPE[创建时间]、#CODE_TYPE[摄像机编码])
* @param [IN    pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstCruisePlanList      巡航计划列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note 目前不支持按组织查询巡航计划列表
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCruisePlanList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CRUISE_PLAN_QUERY_ITEM      *pstCruisePlanList
);


/**
* 使能巡航计划 \n
* @param [IN    pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePlanCode    巡航计划编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);


/**
* 去使能巡航计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCruisePlanCode    巡航计划编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_CRUISEID_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopCruisePlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCruisePlanCode[IMOS_RES_CODE_LEN]
);
/** @} */  /* 巡航计划结束 */
/** @} */ // end of groupCruise
#if 0
#endif
/***************************************************************************
告警
****************************************************************************/
/** @defgroup groupAlarm 告警管理
*   告警管理包括告警源增删改查(列表)，告警输出设备增删改查(列表)，告警联动增删改查等
*   @{
*/
/**
* 创建扩展告警源(不包括编码器,解码器,摄像机等)
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstAlarmSrc                 告警源信息
* @param [OUT]  szAlarmSrcCode              告警源编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* - #ERR_DAO_DEVICE_CODE_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.告警源编码不可能为空
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ALARM_SRC_S                         *pstAlarmSrc,
    OUT CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN]
);


/**
* 删除扩展告警源(不包括编码器,解码器,摄像机等)
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szAlarmSrcCode              告警源编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN]
);


/**
* 修改扩展告警源信息(不包括编码器,解码器,摄像机等)
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szAlarmSrcCode              告警源编码
* @param [IN]   pstAlarmSrc                 告警源信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 告警源的编码不可修改, 但在修改告警源信息时需要填写
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ALARM_SRC_S                         *pstAlarmSrc
);


/**
* 查询扩展告警源信息(不包括编码器,解码器,摄像机等)
* @param [IN]   pstUserLoginIDInfo          用户登录ID信息标识
* @param [IN]   szAlarmSrcCode              告警源编码
* @param [OUT]  pstAlarmSrc                 告警源信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmSrcCode[IMOS_RES_CODE_LEN],
    OUT ALARM_SRC_S                         *pstAlarmSrc

);


/**
* 分页查询组织下的告警源(包括编码器,解码器,摄像机等以及扩展告警源)列表  \n
* 条件为空时,查询所有告警源,并默认按照"告警源名称升序"排序.
* @param [IN]   pstUserLogIDInfo       用户信息标识
* @param [IN]   szOrgCode              组织编码
* @param [IN]   pstQueryCondition      通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[告警源名称]、#DOMAIN_TYPE[所属域类型:0 本域,1 外域])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstAlarmSrcList        告警源信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_SRC_QUERY_ITEM_S      *pstAlarmSrcList

);


/**
* 创建告警输出设备
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstAlarmExporter            告警输出设备信息
* @param [OUT]  szAlarmExporterCode         告警输出设备编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.告警输出设备编码不可能为空
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ALARM_EXPORTER_S                    *pstAlarmExporter,
    OUT CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN]
);


/**
* 删除告警输出设备
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szAlarmExporterCode         告警输出设备编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN]
);


/**
* 修改告警输出设备信息
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szAlarmExporterCode         告警输出设备编码
* @param [IN]   pstAlarmExporter            告警输出设备信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN],
    IN  ALARM_EXPORTER_S                    *pstAlarmExporter
);


/**
* 查询告警输出设备信息
* @param [IN]   tUserLoginIDInfo            用户登录ID信息标识
* @param [IN]   szAlarmExporterCode         告警输出设备编码
* @param [OUT]  pstAlarmExporter            告警输出设备信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEVICE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmExporter
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  CHAR                                szAlarmExporterCode[IMOS_RES_CODE_LEN],
    OUT ALARM_EXPORTER_S                    *pstAlarmExporter

);


/**
* 分页查询组织下的告警输出设备列表  \n
* 条件为空时,查所有告警输出设备,并默认"告警输出设备名称升序排序"
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szOrgCode              组织编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:#NAME_TYPE[设备名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstAlarmExporterList   告警输出设备信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmExporterList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_EXPORTER_QUERY_ITEM_S *pstAlarmExporterList

);

/**
* 查询告警信息
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szEventRecordCode   告警编码
* @param [OUT]  pstEventRecord      告警信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_NOT_EXIST
 * -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarm
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szEventRecordCode[IMOS_RES_CODE_LEN],
    OUT EVENT_RECORD_S          *pstEventRecord
);


/**
* 分页查询告警信息列表
* 条件为空时,查询所有角色,并默认按照"告警时间降序"排序.
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstQueryCondition   通用查询条件(可为NULL;支持的查询条件包括:#USER_NAME_TYPE[用户名称];
* -                                 #TIME_TYPE[时间]; #EVENT_TYPE[告警事件]; #EVENT_SECURITY[告警事件级别];
* -                                 #EVENT_COMFIRMED[告警是否已确认]; #EVENT_TIME[告警时间])
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  pstAlarmList        告警信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.告警严重级别排序从危急到提示为升序；
* -     2.告警严重级别排序从提示到危急为降序。
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_RECORD_S                  *pstAlarmList
);


/**
* 确认告警
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szEventRecordCode       告警编码
* @param [IN]   szAckSuggestion         告警确认描述
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_NOT_EXIST
* - #ERR_AS_ALARM_CONFIRMED
* -     其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfirmAlarm
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szEventRecordCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szAckSuggestion[IMOS_DESC_LEN]
);


/**
* 增加告警联动
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szAlarmSrcCode          告警源编码
* @param [IN]   ulAlarmType             告警类型, 取值为#AlARM_TYPE_E
* @param [IN]   pstAlarmAction          告警联动动作信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_LINKAGE_EXIST
* -     其它错误码：见结果码文件sdk_err.h
* @note 可配置完全重复的动作，但注意例子中的情况。例：配置多次"同一个摄像机到同一个监视器", 都会配置成功，但最终只有
*       最先配置的会执行成功.
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_S                  *pstAlarmAction
);


/**
* 删除告警联动
* @param [IN]  pstUserLogIDInfo             用户登录ID信息标识
* @param [IN]  szAlarmSrcCode               告警源编码
* @param [IN]  ulAlarmType                  告警类型, 取值为#AlARM_TYPE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_LINKAGE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType
);


/**
* 修改告警联动信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szAlarmSrcCode          告警源编码
* @param [IN]   ulAlarmType             告警类型, 取值为#AlARM_TYPE_E
* @param [IN]   pstAlarmAction          告警联动动作信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_LINKAGE_NOT_EXIST
* -     其它错误码：见结果码文件sdk_err.h
* @note 可配置完全重复的动作，但注意例子中的情况。例：配置多次"同一个摄像机到同一个监视器", 都会配置成功，但最终只有
*       最先的配置会执行成功.
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ALARM_ACTION_S                  *pstAlarmAction
);


/**
* 查询告警联动信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szAlarmSrcCode          告警源编码
* @param [IN]   ulAlarmSrcType          告警类型, 取值为#AlARM_TYPE_E
* @param [OUT]  pstAlarmAction          告警联动动作信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_LINKAGE_NOT_EXIST
* - #ERR_AS_LINKAGE_NUM
* -     其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmLinkage
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    OUT ALARM_ACTION_S                  *pstAlarmAction
);

/**
* 查询告警事件信息列表 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstQueryCondition       查询条件，支持按照起止时间以及告警联动对象查询录像
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstAlarmEventInfo       告警联动信息数据结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 查询时间跨度不能大于24小时，且在录像检索流程中时间格式为："%Y-%m-%d %H:%M:%S"格式 ，信息长度限定为24字符
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmEventList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  ALARM_EVENT_QUERY_CONDITION_S *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ALARM_EVENT_INFO_S          *pstAlarmEventInfo
);

/** @} */ /* end of ALARM MODULE */

#if 0
#endif
/**************************************************************************************************************
布防以及布防计划 GuardPlan Edited By sunyifan(06966)  Complete
**************************************************************************************************************/
/** @defgroup groupGPMg 布防以及布防计划管理
*   布防以及布防计划管理
*   @{
*/
/**
* 增加布防计划 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szAlarmSrcCode          告警源编码
* @param [IN]   ulAlarmSrcType          告警类型, 取值为#AlARM_TYPE_E
* @param [IN]   pstGuardPlanInfo        布防计划信息
* @param [OUT]  szGuardPlanCode         布防计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_CAM_NOT_LOCAL_DOMAIN
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  GUARD_PLAN_INFO_S               *pstGuardPlanInfo,
    OUT CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN]
);

/**
* 修改布防计划 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGuardPlanCode         布防计划编码
* @param [IN]   pstGuardPlanInfo        布防计划信息结构体
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    IN  GUARD_PLAN_INFO_S               *pstGuardPlanInfo
);

/**
* 删除布防计划 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGuardPlanCode         布防计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN]
);

/**
* 查询布防计划 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGuardPlanCode         布防计划编码
* @param [OUT]  pstGuardPlanInfo        布防计划信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    OUT GUARD_PLAN_INFO_S               *pstGuardPlanInfo
);


/**
* 分页查询布防计划列表 \n
* 条件为空时,查询所有布防计划,并默认按照"计划名称升序"排序.
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szOrgCode              组织编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;可填条件:#NAME_TYPE[计划名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstGuardPlanList       布防计划列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlanList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GUARD_PLAN_QUERY_ITEM_S     *pstGuardPlanList
);

/**
* 布防计划 - 查询列表(按告警源编码) \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szAlarmSrcCode      告警源编码
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  pstGuardPlanList    布防计划信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGuardPlanListBySrc
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szAlarmSrcCode[IMOS_DEVICE_CODE_LEN],
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GUARD_PLAN_QUERY_ITEM_S     *pstGuardPlanList
);


/**
* 控制布防计划, 包括布防计划的启动和停止 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGuardPlanCode         布防计划编码
* @param [IN]   ulCtrlMode              控制操作的类型, 1为启动; 0为停止
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GUARD_POLICY_EXIST
* -     其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CtrlGuardPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGuardPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulCtrlMode
);

/**
* 手动布防处理, 包括手动布防和手动撤防 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szAlarmSrcCode          告警源编码
* @param [IN]   ulAlarmSrcType          告警类型
* @param [IN]   ulCtrlMode              1为手动布防, 0为手动撤防
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 其它错误码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CtrlManualGuard
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szAlarmSrcCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulAlarmType,
    IN  ULONG                           ulCtrlMode
);

/** @} */ /* end of GUARD PLAN MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
GIS
****************************************************************************/
/** @defgroup groupGIS  GIS地图管理
*   GIS地图管理包括上传,下载,删除,设置GIS地图等接口,热点,热区,文本框,各项资源图标的增删改查(列表)等
*   @{
*/
/*--------------------------------------------GIS地图管理--------------------------------------*/

/**
* 上传GIS地图时, 将地图名称以及文件名上传到服务器进行校验, 成功后, 返回FTP服务器用户名、密码以及文件存放路径
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstGISInfo              GIS信息("地图文件名"不允许重复,即不允许重复上传"同文件名"地图)
* @param [OUT]  pstFtpInfo              FTP服务信息
* @param [OUT]  pulSequenceNumber       本次FTP服务器的序列号
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_MAP_NUM_OVERFLOW
* - #ERR_AS_FIND_SAME_MAP_NAME
* - #ERR_AS_FIND_SAME_MAP_FILE_NAME
* -     其它,参见sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisMapStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  FILE_GIS_INFO_S             *pstGISInfo,
    OUT FTP_INFO_S                  *pstFtpInfo,
    OUT ULONG                       *pulSequenceNumber
);

/**
* 将上传GIS地图的结果, 通知服务器
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   bIsSuccess              上传文件的结果, BOOL_TRUE为成功; BOOL_FALSE为失败
* @param [IN]   ulSequenceNumber        本次FTP服务器的序列号
* @param [OUT]  szGISMapCode            GIS地图编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_UPLOAD_MAP_FAIL
* - #ERR_DAO_GIS_MAP_CODE_EXIST
* - #ERR_DAO_GIS_MAP_NAME_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGisMapEnd
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  BOOL_T                        bIsSuccess,
    IN  ULONG                         ulSequenceNumber,
    OUT CHAR                          szGISMapCode[IMOS_RES_CODE_LEN]
);

/**
* 获取GIS地图信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [OUT]  pstGISInfo              GIS地图信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetGisMapInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    OUT FILE_GIS_INFO_S             *pstGISInfo
);



/**
* 下载GIS地图的时候，请求FTP的相关信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [OUT]  pstFtpInfo              FTP服务信息
* @param [OUT]  ulSequenceNumber        本次FTP服务器的序列号
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_FIND_SAME_MAP_FILE_NAME
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisMapStart
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    OUT FTP_INFO_S                  *pstFtpInfo,
    OUT ULONG                       *ulSequenceNumber
);

/**
* 将下载GIS地图的结果通知服务器
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   bIsSuccess              上传文件的结果，BOOL_TRUE为成功; BOOL_FALSE为失败
* @param [IN]   ulSequenceNumber        本次FTP服务器的序列号
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DownloadGisMapEnd
(
    IN  USER_LOGIN_ID_INFO_S         *pstUserLogIDInfo,
    IN  BOOL_T                        bIsSuccess,
    IN  ULONG                         ulSequenceNumber
);


/**
* 删除GIS地图
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MAP_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 注意，如果删除空文件，也返回成功：
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGisMap
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN]
);

/**
* 修改GIS地图名称
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [IN]   szGisMapName            GIS地图名称
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MAP_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGisMapName
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szGisMapName[IMOS_NAME_LEN]
);


/**
* 指定组织下默认地图
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szDomainCode            组织编码
* @param [IN]   szGISMapCode            GIS地图编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetOrgDefGISMap
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szGISMapCode[IMOS_RES_CODE_LEN]
);


/**
* 查询组织下默认地图信息
* @param [IN]  pstUserLogIDInfo         用户登录ID信息标识
* @param [IN]  szDomainCode             组织编码
* @param [OUT] pstGisMapItem            默认地图信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MAP_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetOrgDefGISMap
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    OUT GIS_QUERY_ITEM_S            *pstGisMapItem
);

/*--------------------------------------------热点--------------------------------------*/


/**
* 在GIS地图上创建热区
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热区所在GIS地图的编码
* @param [IN]   pstHotZone              热区信息
* @param [OUT]  szHotZoneCode           热区编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTZONE_CODE_EXIST
* - #ERR_DAO_GIS_HOTZONE_NAME_EXIST
* - #ERR_DAO_GIS_HOTZONE_LINK_MAP_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateHotZone
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTZONE_S                   *pstHotZone,
    OUT CHAR                            szHotZoneCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除热区
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热区所在GIS地图编码
* @param [IN]   szHotZoneCode           热区编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelHotZone
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szHotZoneCode[IMOS_RES_CODE_LEN]
);


/**
* 在GIS地图上修改热区
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热区所在GIS地图编码
* @param [IN]   szHotZoneCode           热区编码
* @param [IN]   pstHotZone              热区信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* - #ERR_DAO_GIS_HOTZONE_NAME_EXIST
* - #ERR_DAO_GIS_HOTZONE_LINK_MAP_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyHotZone
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  CHAR                        szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szHotZoneCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTZONE_S              *pstHotZone
);


/**
* 查询热区信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热区所在GIS地图编码
* @param [IN]   szHotZoneCode           热区编码
* @param [OUT]  pstHotZone              热区信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTZONE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotZone
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szHotZoneCode[IMOS_RES_CODE_LEN],
    OUT GIS_HOTZONE_WITH_CODE_S     *pstHotZone
);


/**
* 分页查询某个GIS地图上的热区列表 \n
* 条件为空时,查所有热区,并默认"热区名称升序排序"
* @param [IN    pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szGISMapCode           热区所在GIS地图编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:#NAME_TYPE[热区名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstHotZoneList         热区列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotZoneList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_HOTZONE_WITH_CODE_S     *pstHotZoneList
);

/*--------------------------------------------热点--------------------------------------*/

/**
* 在GIS地图上创建热点
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热点所在GIS地图编码
* @param [IN]   pstHotSpot              热点信息
* @param [OUT]  szHotSpotCode           热点编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTSPOT_CODE_EXIST
* - #ERR_DAO_GIS_HOTSPOT_LINK_DOMAIN_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTSPOT_S                   *pstHotSpot,
    OUT CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除热点
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热点所在GIS地图编码
* @param [IN]   szHotSpotCode           热点编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN]
);



/**
* 在GIS地图上修改热点
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热点所在GIS地图编码
* @param [IN]   szHotSpotCode           热点编码
* @param [IN]   pstHotSpot              热点信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* - #ERR_DAO_GIS_HOTSPOT_LINK_DOMAIN_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN],
    IN  GIS_HOTSPOT_S                   *pstHotSpot
);


/**
* 查询热点信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            热点所在GIS地图编码
* @param [IN]   szHotSpotCode           热点编码
* @param [OUT]  pstHotSpot              热点信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_HOTSPOT_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotSpot
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szHotSpotCode[IMOS_RES_CODE_LEN],
    OUT GIS_HOTSPOT_WITH_CODE_S         *pstHotSpot
);


/**
* 分页查询某个GIS地图上的热点列表
* 条件为空时,查所有热点,并默认"热点编码升序排序"
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szGISMapCode           热区所在GIS地图编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:#CODE_TYPE[热点编码])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstHotSpotList         热点信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryHotSpotList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_HOTSPOT_WITH_CODE_S     *pstHotSpotList
);

/*--------------------------------------------文本框--------------------------------------*/

/**
* 在GIS地图上创建文本框
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            文本框所在GIS地图编码
* @param [IN]   pstTextBox              文本框信息
* @param [OUT]  szTextBoxCode           文本框编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MSGBOX_CODE_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateTextBox
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_TEXTBOX_S                   *pstTextBox,
    OUT CHAR                             szTextBoxCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除文本框
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            文本框所在GIS地图编码
* @param [IN]   szTextBoxCode           文本框编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTextBox
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szTextBoxCode[IMOS_RES_CODE_LEN]
);


/**
* 在GIS地图上修改文本框
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            文本框所在GIS地图编码
* @param [IN]   szTextBoxCode           文本框编码
* @param [IN]   pstTextBox              文本框信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTextBox
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szTextBoxCode[IMOS_RES_CODE_LEN],
    IN  GIS_TEXTBOX_S               *pstTextBox
);


/**
* 查询某个文本框信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            文本框所在GIS地图编码
* @param [IN]   szTextBoxCode           文本框编码
* @param [OUT]  pstTextBox              文本框信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_MSGBOX_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTextBox
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szTextBoxCode[IMOS_RES_CODE_LEN],
    OUT GIS_TEXTBOX_WITH_CODE_S     *pstTextBox
);


/**
* 分页查询某个GIS地图上的文本框列表 \n
* 条件为空时,查所有文本框,并默认"文本框编码升序"排序
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szGISMapCode           文本框所在GIS地图编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:#CODE_TYPE[文本框编码])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstTextBoxList         文本框信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTextBoxList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_TEXTBOX_WITH_CODE_S     *pstTextBoxList
);


/*-------------------------------资源图标(摄像机、告警源、告警输出)--------------------------------------*/

/**
* 在GIS地图上创建摄像机图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            摄像机图标所在GIS地图编码
* @param [IN]   pstCameraIcon           摄像机图标信息
* @param [OUT]  szCameraIconCode        摄像机图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_CAM_ICON_S              *pstCamIcon,
    OUT CHAR                         szCamIconCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除摄像机图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [IN]   szCamIconCode           摄像机图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN]
);


/**
* 在GIS地图上修改摄像机图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [IN]   szCamIconCode           摄像机图标编码
* @param [IN]   pstCamIcon              摄像机图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_CAM_ICON_S              *pstCamIcon
);


/**
* 查询摄像机图标信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [IN]   szCamIconCode           摄像机图标编码
* @param [OUT]  pstCamIcon              摄像机图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamIcon
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                         szCamIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_CAM_ICON_WITH_CODE_S    *pstCamIcon
);


/**
* 分页查询某个GIS地图上的摄像机图标列表
* 条件为空时,查所有摄像机图标 ,并默认"摄像机图标编码升序排序"
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szGISMapCode           摄像机图标所在GIS地图编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;支持的查询条件包括:#CODE_TYPE[图标编码])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstCameraIconList      摄像机图标信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamIconList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                         szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT GIS_CAM_ICON_WITH_CODE_S    *pstCamIconList
);


/**
* 在GIS地图上增加告警源图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            告警源图标所在GIS地图编码
* @param [IN]   pstAlarmSrcIcon         告警源图标信息
* @param [OUT]  szAlarmSrcIconCode      告警源图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_SRC_ICON_S            *pstAlarmSrcIcon,
    OUT CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除告警源图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            告警源图标所在GIS地图编码
* @param [IN]   szAlarmSrcIconCode      告警源图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN]
);


/**
* 在GIS地图上修改告警源图标
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szGISMapCode            GIS地图编码
* @param [IN]   szAlarmSrcIconCode      告警源图标编码
* @param [IN]   pstAlarmSrcIcon         告警源图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_SRC_ICON_S            *pstAlarmSrcIcon
);


/**
* 查询某个告警源图标信息
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGISMapCode                GIS地图编码
* @param [IN]   szAlarmSrcIconCode          告警源图标编码
* @param [OUT]  pstAlarmSrcIcon             告警源图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmSrcIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_ALARM_SRC_ICON_WITH_CODE_S  *pstAlarmSrcIcon
);


/**
* 分页查询某个GIS地图上的告警源图标列表
* 条件为空时,查所有告警源图标,并默认"告警源图标编码升序排序"
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szGISMapCode           告警源图标所在GIS地图编码
* @param [IN]   pstQueryCondition      通用查询条件(可为NULL;支持的查询条件包括:#CODE_TYPE[图标编码])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstAlarmSrcIconList    告警源图标信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGISAlarmSrcIconList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GIS_ALARM_SRC_ICON_WITH_CODE_S  *pstAlarmSrcIconList
);


/**
* 在GIS地图上创建告警输出图标
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGISMapCode                告警输出图标所在GIS地图编码
* @param [IN]   pstAlarmDstIcon             告警输出图标信息
* @param [OUT]  szAlarmDstIconCode          告警输出图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_DST_ICON_S            *pstAlarmDstIcon,
    OUT CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN]
);

/**
* 在GIS地图上删除告警输出图标
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGISMapCode                GIS地图编码
* @param [IN]   szAlarmDstIconCode          告警输出图标编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN]
);


/**
* 在GIS地图上修改告警输出图标
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGISMapCode                GIS地图编码
* @param [IN]   szAlarmDstIconCode          告警输出图标编码
* @param [IN]   pstAlarmDstIcon             告警输出图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* - #ERR_DAO_GIS_DEV_ICON_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN],
    IN  GIS_ALARM_DST_ICON_S            *pstAlarmDstIcon
);


/**
* 查询某个告警输出图标信息
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGISMapCode                GIS地图编码
* @param [IN]   szAlarmDstIconCode          告警输出图标编码
* @param [OUT]  pstAlarmDstIcon             告警输出图标信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_GIS_ICON_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmDstIcon
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  CHAR                             szAlarmDstIconCode[IMOS_RES_CODE_LEN],
    OUT GIS_ALARM_DST_ICON_WITH_CODE_S  *pstAlarmDstIcon
);


/**
* 分页查询某个GIS地图上的告警输出图标列表 \n
* 条件为空时,查所有告警输出图标,并默认"告警输出图标编码升序排序"
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szGISMapCode        告警输出图标所在GIS地图编码
* @param [IN]   pstQueryCondition   通用查询条件(可填NULL;支持的查询条件包括:#CODE_TYPE[图标编码])
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  pstAlarmDstIconList 告警输出图标信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGISAlarmDstIconList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                             szGISMapCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GIS_ALARM_DST_ICON_WITH_CODE_S  *pstAlarmDstIconList
);
/** @} */ /* end of GIS MODULE */
#if 0
#endif
/*******************************************************************************************************
透明通道 Transparent
********************************************************************************************************/
/** @defgroup groupTP 透明通道管理
*   透明通道管理
*   @{
*/
/**
* 增加透明通道,包含串口到指定地址和串口到串口
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstTransChannelInfo     透明通道的信息
* @param [OUT]  szTransChannelCode      透明通道编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_SERIAL_PORT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.现只支持串口到1个IP的地址的透明通道创建,
* -     2.创建透明通道最大支持2个串口.
* -     3.只支持协议类型为UDP方式的透明通道
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  TRANSPARENT_INFO_S          *pstTransChannelInfo,
    OUT CHAR                        szTransChannelCode[IMOS_RES_CODE_LEN]
);

/**
* 删除透明通道
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szTransChannelCode      透明通道编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szTransChannelCode[IMOS_RES_CODE_LEN]
);


/**
* 修改透明通道
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szTransChannelCode          透明通道编码
* @param [IN]   pstSDKTransparentChannel    透明通道信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_TL_OPENED
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* - #ERR_DAO_SERIAL_PORT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.只能修改透明通道名称与通道端描述字段,
* -     2.只支持协议类型为UDP方式的透明通道
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN],
    IN  TRANSPARENT_INFO_S              *pstSDKTransparentChannel
);



/**
* 分页查询某个组织下的透明通道列表  \n
* 条件为空时,查询所有透明通道,并默认按照"透明通道名称升序"排序.
* @param [IN]   pstUserLogIDInfo         用户信息标识
* @param [IN]   szDomainCode             组织编码
* @param [IN]   pstQueryCondition        通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[透明通道名称]; CODE_TYPE[透明通道编码])
* @param [IN]   pstQueryPageInfo         请求分页信息
* @param [OUT]  pstRspPageInfo           响应分页信息
* @param [OUT]  pstSDKTransparentChannel 透明通道信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTransparentChannelList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRANSPARENT_QUERY_ITEM_S    *pstSDKTransparentChannel
);

/**
* 查询透明通道信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szTransChannelCode      透明通道编码
* @param [OUT]  pstSDKUpgTaskInfo       透明通道信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTransparentChannelInfo
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  CHAR                             szTransChannelCode[IMOS_RES_CODE_LEN],
    OUT TRANSPARENT_INFO_S               *pstSDKTransparentInfo
);

/**
* 启动透明通道
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szTransChannelCode      透明通道编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* - #ERR_AS_TL_DEVICE_OFFLINE
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN]
);
/**
* 停用透明通道
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szTransChannelCode      透明通道编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopTransparentChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN]
);

/**
* 通过透明通道发送数据
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szTransChannelCode      透明通道编码
* @param [IN]   pData                   待发送的数据
* @param [IN]   ulDataLen               待发送的数据长度, 最大长度为#IMOS_STRING_LEN_1024
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_CHANNEL_NOT_ENABLE
* - #ERR_AS_TL_CLOSED
* - #ERR_DAO_TRANSPARENT_CHANNLE_CODE_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_SendDataByChannel
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szTransChannelCode[IMOS_RES_CODE_LEN],
    IN  VOID                            *pData,
    IN  ULONG                           ulDataLen
);
/** @} */ /* end of TRANSPARENT MANAGEMENT MODULE */
#if 0
#endif
/*******************************************************************************
 系统管理
*******************************************************************************/
/** @defgroup groupLogMg 系统管理
*   系统管理模块包括升级管理,日志管理,License管理,信息统计管理等子模块
*   @{
*/
/**
* @name 日志管理
* @{    日志管理
*/
/**
* 分页查询用户操作日志
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   pstQueryCondition      通用查询条件
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstOperateLogInfo      日志信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. 如果无通用查询条件，pstQueryCondition可以为NULL
* - 2. 支持按操作员名称、查询起/止时间、IP地址、操作类型、操作结果、业务类型、操作对象进行排序与内容查询,
* -    对应通用查询条件中的查询项类型分别为:
* -    #USER_NAME_TYPE、#OPER_TIME、#OPER_IP、#OPER_TYPE、#OPER_RESULT、#OPER_SERVICE_TYPE、#OPER_OBJ;
* - 3. 若排序条件不填，将默认按照操作时间降序排序;
* - 4. 操作时间的查询项#OPER_TIME，可以查询某段时间，#QUERY_CONDITION_ITEM_S中#ulLogicFlag中填写请参照#LOGIC_FLAG_E, 如:
* -    查询操作时间为A至B的操作日志，那么查询项为2, 查询项列表中第一(或二)数组元素中查询项类型为操作时间，
* -    查询条件逻辑关系类型为大于等于(或小于等于)，查询内容为A(或B);
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOperLog
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT OPERATE_LOG_S               *pstOperateLogInfo
);

/** @} */ /* end of 日志管理 */
#if 0
#endif
/***************************************************************************
其它
****************************************************************************/
/**
* @name 其它
* @{    其它
*/
/**
* 备份信息（包括配置信息、日志信息和数据库信息）, 并返回备份的执行结果。 \n
* @param [IN]    pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]    ulFlag                  备份类型标记, 取值为#BP_BACKUPTYPE_E
* @param [OUT]   pulStatus               当前备份状态, 取值为#BACKUP_STATUS_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 每次备份时，会删除之前的备份文件。
*/
IMOS_EXPORT ULONG STDCALL IMOS_BackupData
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulFlag,
    OUT ULONG                           *pulStatus
);

/**
* 查询备份信息, 若系统处于非备份状态则给出其所在FTP的信息。 \n
* @param [IN]    pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]    ulFlag                  备份类型标记, 取值为#BP_BACKUPTYPE_E
* @param [OUT]   pulStatus               当前备份状态, 取值为#BACKUP_STATUS_E
* @param [OUT]   pstFtpInfo              备份文件相关的FTP信息, 信息中的文件路径包含了文件名称
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBackupInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulFlag,
    OUT ULONG                           *pulStatus,
    OUT FTP_INFO_S                      *pstFtpInfo
);

/**
* 执行预定义的命令通用接口
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstCmdInfo              命令信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PARAM_NOT_EXIST
* - #ERR_AS_INVALID_NTP_IP_ADDRESS
 * -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ExecutePredefinedCmd
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SDK_CMD_INFO_S                   *pstCmdInfo
);


/**
* 启动/停止服务器NTP时间同步
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulSyncFlag              同步标记，1开始，2停止
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PARAM_NOT_EXIST
* - #ERR_COMMON_FAIL_CALL_SYSTEM
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessNTPSync
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSyncFlag
);

/** @} */ /* end of ADDITIONAL MODULE */
#if 0
#endif
/**
* 校验License序列号的有效性 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szLicenseSerial     License序列号
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_CHECK_LICENSE_SERIAL_FAIL
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CheckLicenseSerial
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szLicenseSerial[IMOS_LICENSE_AUTHORITY_CODE_LENGTH]
);


/**
* 将一系列license整合生成一个license文件 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   apcLicenseSerials       授权码列表
* @param [IN]   ulLicenseSerialCount    授权码个数
* @param [IN]   pstClientInfo           客户信息
* @param [IN]   szContractId            合同编号
* @param [OUT]  pstFtpInfo              FTP服务信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_BP_GENERATE_HOST_ID_FILE_FAIL
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_MakeHostLicenseFile
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        (*apcLicenseSerials)[IMOS_LICENSE_AUTHORITY_CODE_LENGTH],
    IN  ULONG                       ulLicenseSerialCount,
    IN  CLIENTINFO_S                *pstClientInfo,
    IN  CHAR                        szContractId[IMOS_CONTRACT_ID],
    OUT FTP_INFO_S                  *pstFtpInfo
);

/**
* 分页获取当前生效的License授权码列表 \n
* 目前只支持一次查询所有列表，即一页的行数最好为授权码最大规格数#IMOS_LICENSE_SERIALS_MAX_NUM
* 对于分页偏移量不再关注。
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  apcSerials          授权码列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseSerialsList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CHAR                        (*apcSerials)[IMOS_LICENSE_AUTHORITY_CODE_LENGTH]
);


/**
* 分页获取当前生效的License包含的规格信息列表 \n
* 目前只支持一次查询所有列表，即一页的行数最好为license针对的业务类型数:IMOS_LICENSE_SERVICE_TYPE_NUM
* 对于分页偏移量不再关注。
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstServiceInfoList     业务规格列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseInfoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT LICENSE_AUTHORITYINFO_S     *pstServiceInfoList
);


/**
* 获取当前生效的License的超时时间 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [OUT]  pstExpireDate       超时时间
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_BP_LICENSE_FILE_CANT_OPEN
* - #ERR_BP_FAIL_PARSE_LICENSE_FILE
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetLicenseExpireTime
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    OUT TIMELIMIT_S                 *pstExpireDate
);
/** @} */ /* end of LICENCE MANAGEMENT MODULE */

#if 0
#endif
/********************************************************************************************************
升级管理 Edit By sunyifan (06966) Complete
*********************************************************************************************************/
/**
* @name 升级管理
* @{    升级管理
*/
/**
* 设置设备升级模式 \n
* 对于自动升级模式来说，不需要再设置任何升级任务
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   ulUpgMode           升级模式, 取值为#MM_UPGMODE_E
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_UPG_MODE_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetUpgMode
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                            ulUpgMode
);

/**
* 查询设备升级模式 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [OUT]  pulUpgMode         升级模式, 取值为#MM_UPGMODE_E
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUpgMode
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulUpgMode
);

/**
* 创建升级任务 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstUpgTaskInfo      升级任务信息
* @param [IN]   ulDevNum            待升级设备数组中待升级设备的实际数量, 最大取值为#UM_MAX_DEVICENUM_INJOB
* @param [IN]   pstUpgDevInfo       待升级设备数组
* @param [OUT]  szUpgTaskCode       升级任务编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_UPGRADE_DEV_NUM_ERR
* - #ERR_DAO_JOB_DEV_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_CreateUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  UPG_TASK_INFO_S             *pstUpgTaskInfo,
    IN  ULONG                       ulDevNum,
    IN  UPG_DEV_INFO_S              *pstUpgDevInfo,
    OUT CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);

/**
* 修改升级任务信息，如果不需要修改升级设备列表，则ulDevNum和pstUpgDevInfo无效 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @param [IN]   pstUpgTaskInfo      升级任务信息
* @param [IN]   bISModifyDevList    是否需要修改设备列表, BOOL_TRUE为修改; BOOL_FALSE为不修改
* @param [IN]   ulDevNum            待升级设备数组中待升级设备的实际数量, 最大取值为#UM_MAX_DEVICENUM_INJOB
* @param [IN]   pstUpgDevInfo       待升级设备数组
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_AS_UPGRADE_DEV_NUM_ERR
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note 1.仅当任务状态为等待或挂起时, 才允许修改任务。
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyUpgTaskInfo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    IN  UPG_TASK_INFO_S             *pstUpgTaskInfo,
    IN  BOOL_T                      bIsModifyDevList,
    IN  ULONG                       ulDevNum,
    IN  UPG_DEV_INFO_S              *pstUpgDevInfo
);

/**
* 删除升级任务 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note 可以删除已经完成的, 挂起的和处于等待状态的升级任务，在执行中的任务不能删除
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);

/**
* 挂起任务 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note 当任务处于以下几种状态时，挂起任务操作会失败：
* - 当任务状态处于正在挂起，或已经挂起时
* - 当升级任务中设备均已经执行
*/
IMOS_EXPORT ULONG STDCALL IMOS_PauseUpgJob
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);


/**
* 激活任务 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_JOB_NOT_EXIST
* - #ERR_MM_JOB_STATUS_NOT_PERMIT
* @note  当任务处于以下几种状态时，激活任务操作会失败：
* - 升级任务处于非挂起状态（等待，运行，正在挂起）
* - 升级任务过期，任务模块会自动删除过期任务
*/
IMOS_EXPORT ULONG STDCALL IMOS_ActiveUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN]
);


/**
* 分页查询所有升级任务  \n
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   pstQueryCondition      通用查询条件
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstSDKUpgTaskList      升级任务查询信息列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note
* - 1. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 2. 支持按升级任务名称、状态、创建者、升级结果、调度/完成/制定时间排序，对应通用查询条件中的查询项类型分别为:
* -    #NAME_TYPE、#JOB_STATUS、#JOB_CREATOR、#JOB_RESULT、#JOB_EXEC_TIME、#JOB_END_TIME、#JOB_ESTB_TIME;
* - 3. 若排序条件不填，将默认按照升级任务名称升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgTaskList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_TASK_QUERY_ITEM_S       *pstSDKUpgTaskList
);


/**
* 查询升级任务信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @param [OUT]  pstUpgTaskInfo      升级任务信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_JOB_NOT_EXIST
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgTask
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    OUT UPG_TASK_INFO_S             *pstUpgTaskInfo
);

/**
* 查询指定升级任务中的升级设备列表 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szUpgTaskCode       升级任务编码
* @param [IN]   pstQueryCondition   通用查询条件
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  pstUpgDevInfo       待升级设备数组
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note
* - 1. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 2. 支持按升级设备编码、名称排序，对应通用查询条件中的查询项类型为#CODE_TYPE、#NAME_TYPE;
* - 3. 若排序条件不填，将默认按照升级设备名称升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryUpgDev
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szUpgTaskCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_DEV_INFO_S              *pstUpgDevInfo
);


/**
* 分页查询需要升级的设备，以及设备升级版本及升级建议\n
* @param [IN]   pstUserLogIDInfo   用户登录信息
* @param [IN]   pstQueryCondition  通用查询条件
* @param [IN]   pstQueryPageInfo   请求分页信息
* @param [OUT]  pstRspPageInfo     返回分页信息
* @param [OUT]  pstSDKUpgDevInfo   需要升级的设备以及相应版本信息列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* @note
* - 1. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 2. 支持按升级设备编码、名称排序，对应通用查询条件中的查询项类型为#CODE_TYPE、#NAME_TYPE;
* - 3. 若排序条件不填，将默认按照升级设备名称升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetDevVersion
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT UPG_ADVICE_INFO_S           *pstSDKUpgDevInfo
);



/**
* 上传升级包开始,先获取上传时所需要的信息\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szFileName          文件名称
* @param [IN]   szDeviceType        设备类型
* @param [OUT]  pstFtpInfo          FTP服务信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_COMMON_DIR_FAIL_MKDIR
* - #ERR_COMMON_DIR_FAIL_REMOVE
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUploadUpgPackInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szFileName[IMOS_FILE_PATH_LEN],
    IN  CHAR                    szDeviceType[IMOS_DESC_LEN],
    OUT FTP_INFO_S              *pstFtpInfo
);


/**
* 上传升级包完成,通知服务器上传成功\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   bIsSuccess          成功与否的标识, BOOL_TRUE为成功; BOOL_FALSE为失败
* @param [IN]   szFilePath          文件路径
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_COMMON_FILE_CANT_OPEN
* - #ERR_COMMON_DIR_FAIL_OPEN
* - #ERR_COMMON_FAIL_STAT
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_NotifyUploadUpgPackResult
(
    IN USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN BOOL_T                    bIsSuccess,
    IN CHAR                      szFilePath[IMOS_FILE_PATH_LEN]
);


/**
* 上传升级配套表开始, 先获取上传时所需要的信息\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szFileName          文件名
* @param [OUT]  pstFtpInfo          FTP服务信息
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_COMMON_DIR_FAIL_REMOVE
* - #ERR_COMMON_DIR_FAIL_MKDIR
* - #ERR_COMMON_FILE_FAIL_CHMOD
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetUploadUpgMatchListInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szFileName[IMOS_FILE_PATH_LEN],
    OUT FTP_INFO_S              *pstFtpInfo
);

/**
* 上传升级配套表完成, 通知服务器上传成功\n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   bIsSuccess          成功与否的标志, BOOL_TRUE为成功; BOOL_FALSE为失败
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_MM_MATCHTBL_OPEN_FAIL
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_NotifyUploadUpgMatchListResult
(
    IN USER_LOGIN_ID_INFO_S     *pstUserLogIDInfo,
    IN BOOL_T                    bIsSuccess
);
/** @} */ /* end of UPGRADE MANAGEMENT MODULE */

#if 0
#endif
/*******************************************************************************
信息统计
*******************************************************************************/
/**
* @name 信息统计管理
* @{    信息统计管理
*/
/**
* 当前业务信息数据统计
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [OUT]  pstSrvStatInfo          统计业务信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySrvStat
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    OUT SRV_STAT_INFO_S         *pstSrvStatInfo
);


/** @} */  /* end of 信息统计管理模块 */


/**
* 查询设备故障统计
* @param [IN]       pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]       szOrgCode               组织编码
* @param [IN]       ulDevType               设备类型,取值为#IMOS_TYPE_DM, #IMOS_TYPE_MS, #IMOS_TYPE_VX500, #IMOS_TYPE_MONITOR, #IMOS_TYPE_EC, #IMOS_TYPE_DC, #IMOS_TYPE_CAMERA, #IMOS_TYPE_ECR, #IMOS_TYPE_DEV_ALL。若为#IMOS_TYPE_DEV_ALL则为统计所有的设备类型
* @param [IN]       ulDevBelAttr            设备归属属性，取值为#IMOS_DEV_BELONG_ATTR，当取#IMOS_DEV_BELONG_ALL时表示不分本域外域，统计所有的设备
* @param [IN]       pstQueryCondition       通用查询条件，若无查询条件可填为NULL
* @param [IN][OUT]  pulArraySize            设备状态统计数组大小, 若需要统计所有的设备类型，则数组大小应该大于等于#IMOS_DEVICE_TYPE_NUM
* @param [OUT]      pstDevStatistic         设备状态统计数组
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevStatusStatistic
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    CHAR                           szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN    ULONG                          ulDevType,
    IN    ULONG                          ulDevBelAttr,
    IN    COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    INOUT ULONG                          *pulArraySize,
    OUT   DEV_STATUS_STATISTIC_S         *pstDevStatistic
 );


/**
* 根据设备状态查询设备信息列表
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szOrgCode                   组织编码
* @param [IN]   ulDevStatus,                设备状态，取值为#IMOS_DEV_FAULT_STATUS_E, 当取值为#IMOS_DEV_FAULT_STATUS_ALL时表示不分状态，查询所有的设备
* @param [IN]   ulDevType                   设备类型，取值为#IMOS_TYPE_DM, #IMOS_TYPE_MS, #IMOS_TYPE_VX500, #IMOS_TYPE_MONITOR, #IMOS_TYPE_EC, #IMOS_TYPE_DC, #IMOS_TYPE_CAMERA, #IMOS_TYPE_ECR. 不支持查询所有(值为#IMOS_TYPE_DEV_ALL)的设备类型的设备信息列表
* @param [IN]   ulDevBelAttr                设备归属属性，取值范围见#IMOS_DEV_BELONG_ATTR, 值为IMOS_DEV_BELONG_ALL表示不分本域外域，查询所有的设备
* @param [IN]   pstQueryCondition           通用查询条件，若无查询条件, 可以填为NULL
* @param [IN]   pstQueryPageInfo            请求分页信息
* @param [OUT]  pstRspPageInfo              响应分页信息
* @param [OUT]  pstDevInfoArray             设备基本信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevListByFaultStatus
(
    IN USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN CHAR                           szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN ULONG                          ulDevStatus,
    IN ULONG                          ulDevType,
    IN ULONG                          ulDevBelAttr,
    IN COMMON_QUERY_CONDITION_S       *pstQueryCondition,
    IN QUERY_PAGE_INFO_S              *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S               *pstRspPageInfo,
    OUT DEV_BASIC_INFO_S              *pstDevInfoArray
 );
#if 0
#endif
/*******************************************************************************
电视墙
*******************************************************************************/
/** @defgroup groupTvWall 电视墙
*   包含电视墙的增删改查
*   @{
*/
/**
* 增加电视墙 \n
* @param [IN]       pstUserLogIDInfo   用户登录ID信息标识
* @param [IN][OUT]  pstTVWallInfo      电视墙信息
* @return 返回如下结果:
* - 成功:
* - #ERR_COMMON_SUCCEED
* - 失败:
* - #ERR_MM_PARENT_RES_NOT_EXIST
* -      返回操作结果码:见结果码文件sdk_err.h
* @note
* - 1、若电视墙信息pstTVWallInfo中电视墙编码添加时填写为空，则平台处理成功后返回编码，若填写则按照填写的编码添加;
* - 2、一个电视墙包含的最大监视器数目为#IMOS_MONITOR_MAXNUM_PER_WALL,监视器的数量可以为零;
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTVWall
(
    IN      USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    INOUT   TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* 删除电视墙 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szTVWallCode        电视墙编码
* @return 返回如下结果:
* - 成功:
* - #ERR_COMMON_SUCCEED
* - 失败:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelTVWall
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTVWallCode[IMOS_CODE_LEN]
);

/**
* 修改电视墙信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstTVWallInfo      电视墙信息
* @return 返回如下结果:
* - 成功:
* - #ERR_COMMON_SUCCEED
* - 失败:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      返回操作结果码:见结果码文件sdk_err.h
* @note
* - 1、电视墙信息pstTVWallInfo中电视墙编码不允许修改,填错可能导致修改失败或修改错误;
* - 2、一个电视墙包含的最大监视器数目为IMOS_MONITOR_MAXNUM_PER_WALL,监视器的数量可以为零;
* - 3、向电视墙内添加监视器，可以采用该接口实现电视墙信息的更新;
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTVWall
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* 查询电视墙详细信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szTVWallCode        电视墙编码
* @param [OUT]  pstTVWallInfo       电视墙信息
* @return 返回如下结果:
* - 成功:
* - #ERR_COMMON_SUCCEED
* - 失败:
* - #ERR_DAO_RES_AGG_NOT_EXIST
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTVWallInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTVWallCode[IMOS_CODE_LEN],
    OUT TV_WALL_INFO_S          *pstTVWallInfo
);

/**
* 分页查询某个组织下的电视墙列表 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szOrgCode           组织域编码
* @param [IN]   pstQueryCondition   通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[电视墙名称])
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      响应分页信息
* @param [OUT]  pstTVWallList       电视墙信息列表
* @return 返回如下结果:
* - 成功:
* - #ERR_COMMON_SUCCEED
* -      返回操作结果码:见结果码文件sdk_err.h
* @note
* - 1、pstTVWallList分页中每页数组的长度必须大于或等于pstQueryPageInfo内的ulPageRowNum，否则会越界;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTVWallList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TV_WALL_QUERY_ITEM_S        *pstTVWallList
);
/** @} */ /* end of TV Wall */
#if 0
#endif

/**
* SDK接收通知消息接口
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szDevCode                   通用设备ID
* @param [IN]   ulBufferLen                 消息长度，最大长度为:#IMOS_MAX_NOTIFY_LEN
* @param [IN]   pcBuffer                    消息内容
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 此接口目前只开放给文安项目使用
*/
IMOS_EXPORT ULONG STDCALL IMOS_Notify
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szDevCode[IMOS_DEVICE_CODE_LEN],
    IN  ULONG                   ulBufferLen,
    IN  CHAR                    *pcBuffer
);

#if 0
#endif

/**
* 添加通用设备
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstGeneralDevInfo           通用设备信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.通用设备编码不能填空,且不能包含空格
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    GENERAL_DEV_INFO_S        *pstGeneralDevInfo
);

/**
* 修改通用设备
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstGeneralDevInfo           通用设备信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    GENERAL_DEV_INFO_S        *pstGeneralDevInfo
);

/**
* 删除通用设备
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szGeneralDevCode            通用设备ID
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelGeneralDev
(
    IN    USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN    CHAR                      szGeneralDevCode[IMOS_DEVICE_CODE_LEN]
);

/**
* 查询某个组织下的通用设备列表  \n
* 支持按名字模糊查询。pstName允许为空，表示该条件无效，否则按名字进行模糊查询
* @param [IN]   pstUserLogIDInfo            用户信息标识
* @param [IN]   szOrgCode                   域编码
* @param [IN]   pstName                     按名字模糊查询信息条件
* @param [IN]   pstQueryPageInfo            请求分页信息
* @param [OUT]  pstRspPageInfo              返回分页信息
* @param [OUT]  GENERAL_DEV_INFO_S          通用设备信息数组
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGeneralDevList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  QUERY_BY_NAME_S                 *pstName,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT GENERAL_DEV_INFO_S              *pstGeneralDevList
);

/**
* 查询某个通用的信息
* @param [IN]   pstUserLogIDInfo            用户信息标识
* @param [IN]   szGeneralDevCode            通用设备编码
* @param [OUT]  pstGeneralDevInfo           通用设备信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryGeneralInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szGeneralDevCode[IMOS_DEVICE_CODE_LEN],
    OUT GENERAL_DEV_INFO_S              *pstGeneralDevInfo
);

/**
* 查询服务器NTP时间同步信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [OUT]  pbNTPSyncStatus         服务器NTP时间同步信息标记  pbNTPSyncStatus为BOOL_FALSE表示停止同步，为BOOL_TRUE表示开启同步
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PARAM_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryNTPSyncStatus
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT BOOL_T                          *pbNTPSyncStatus
);

#if 0
#endif

/**
* 外域 - 添加V2，包含权签信息
* @param [IN]   pstUserLogIDInfo        用户信息标识
* @param [IN]   pstExDomain            外域信息结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   EX_DOMAIN_V2_S             *pstExDomain
);


/**
* 外域 - 修改V2，编码不可修改，但需要带入
* @param [IN]  pstUserLogIDInfo          用户信息标识
* @param [IN]  pstExDomain               外域信息结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   EX_DOMAIN_V2_S             *pstExDomain
);

/**
* 外域 - 查询（某个）V2
* @param [IN]   pstUserLogIDInfo        用户信息标识
* @param [IN]   szExDomainCode          外域编码
* @param [OUT]  pstExDomain             外域信息结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExDomainV2
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    OUT  EX_DOMAIN_V2_S             *pstExDomain
);

/**
* 外域 - 查询（列表）V2 \n
* @param [IN]   pstUserLogIDInfo       用户信息标识
* @param [IN]   pstQueryCondition      通用查询条件
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         返回分页信息
* @param [OUT]  pstExDomainList        外域查询信息列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 2. 支持按外域编码、名称、类型、IP、端口、干线数量、域间组播策略、引流标志排序，对应通用查询条件中的查询项类型分别为:
* -   #CODE_TYPE、#NAME_TYPE、#EXT_DOMAIN_TPYE、#EXT_DOMAIN_IP、#EXT_DOMAIN_TRUNK_NUM、#EXT_DOMAIN_TRUNK_NUM、
* -   #EXT_DOMAIN_MULTICAST、#EXT_DOMAIN_SESSION;
* - 3. 若排序条件不填，将默认按照外域名称升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryExDomainListV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT EX_DOMAIN_V2_S              *pstExDomainList
);

/**
* 共享摄像机处理V2，可以共享，也可以取消
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulShareFlag             共享标志: 0为取消共享，1为共享
* @param [IN]   ulShareCamNum           摄像机的个数
* @param [IN]   pstShareCamList         共享/取消摄像机列表

* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareCameraV2
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareCamNum,
    IN  SHARE_CAMERA_BASE_V2_S  *pstShareCamList
);

/**
* 查询共享的摄像机列表V2
* @param [IN]   pstUserLogIDInfo    用户信息
* @param [IN]   szExDomainCode      外域编码
* @param [IN]   ulQueryFlag         查询标记
* @param [IN]   pstQueryCondition   通用查询条件
* @param [IN]   pstQueryPageInfo    请求分页信息
* @param [OUT]  pstRspPageInfo      返回分页信息
* @param [OUT]  pstShareCamList     查询出来摄像机列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. ulQueryFlag作为查询标记:0表示查本域共享给外域的摄像机，1表示查外域共享给本域的摄像机
* - 2. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 3. 支持按摄像机编码、名称排序，对应通用查询条件中的查询项类型分别为:#CODE_TYPE、#NAME_TYPE;
* - 4. 若排序条件不填，将默认按照摄像机名称升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareCameraV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulQueryFlag,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SHARE_CAM_INFO_V2_S         *pstShareCamList
);

/**
* 删除外域共享资源
* @param [IN]   pstUserLogIDInfo            用户信息标识
* @param [IN]   ulResType                   资源类型，取值IMOS_TYPE_E
* @param [IN]   szResCode                   待删除的资源编码
* @param [IN]   szExdomainCode              外域编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelResSharedByExdomain
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulResType,
    IN  CHAR                            szResCode[IMOS_RES_CODE_LEN],
    IN  CHAR                            szExdomainCode[IMOS_DOMAIN_CODE_LEN]
);

/***************************************************************************
共享组织管理
****************************************************************************/
/**
* @name 组织共享管理
* @{    组织共享管理
*/
/**
* 组织共享处理，可以共享，也可以取消
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulShareFlag             共享标志: 0为取消共享，1为共享
* @param [IN]   ulShareOrgNum           组织的个数
* @param [IN]   pstShareOrgList         共享/取消组织列表

* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareOrg
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareOrgNum,
    IN  SHARE_ORG_BASE_S        *pstShareOrgList
);

/**
* 查询组织下面的子组织节点共享信息列表，只获取一层 \n
* 不填查询条件时,查所有,并默认排序(组织名称升序)
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   szOrgCode          组织节点编码
* @param [IN]   szExDomainCode     外域编码
* @param [IN]   pstQueryCondition  通用查询条件(可填NULL;可填条件:NAME_TYPE[组织名称])
* @param [IN]   pstQueryPageInfo   请求分页信息
* @param [OUT]  pstRspPageInfo     返回分页信息
* @param [OUT]  pstOrgShareList    组织节点共享信息列表指针
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOrgShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_ORG_INFO_S                *pstOrgShareList
);


/**
* 查询共享给外域/外域共享的组织列表
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulQueryFlag             查询标记，0表示查本域共享给外域的组织，1表示查外域共享给本域的组织
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          返回分页信息
* @param [OUT]  pstShareOrgList         查询出来组织列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareOrgList
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_ORG_INFO_S        *pstShareOrgList
);


/***************************************************************************
共享告警源管理
****************************************************************************/
/**
* @name 告警源共享管理
* @{    告警源共享管理
*/
/**
* 告警源共享处理，可以共享，也可以取消
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulShareFlag             共享标志: 0为取消共享，1为共享
* @param [IN]   ulShareAlarmSrcNum      告警源的个数
* @param [IN]   pstShareAlarmSrcList    共享/取消告警源列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareAlarmSrcNum,
    IN  SHARE_ALARMSRC_BASE_S   *pstShareAlarmSrcList
);

/**
* 查询组织下面的告警源共享信息列表，只获取一层 \n
* 不填查询条件时,查所有,并默认排序(告警源名称升序)
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   szOrgCode          组织节点编码
* @param [IN]   szExDomainCode     外域编码
* @param [IN]   pstQueryCondition  通用查询条件(可填NULL;可填条件:NAME_TYPE[告警源名称])
* @param [IN]   pstQueryPageInfo   请求分页信息
* @param [OUT]  pstRspPageInfo     返回分页信息
* @param [OUT]  pstAlmSrcShareList 告警源共享信息列表指针
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAlarmSrcShareInfo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_ALARMSRC_INFO_S           *pstAlmSrcShareList
);

/**
* 查询共享给外域的告警源列表
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulQueryFlag             查询标记，0表示查本域共享给外域的告警源，1表示查外域共享给本域的告警源
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          返回分页信息
* @param [OUT]  pstShareAlmSrcList      查询出来告警源列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareAlarmSrc
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_ALARMSRC_INFO_S   *pstShareAlmSrcList
);

/***************************************************************************
共享GIS地图管理
****************************************************************************/
/**
* @name GIS地图共享管理
* @{    GIS地图共享管理
*/
/**
* 地图共享处理，可以共享，也可以取消
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulShareFlag             共享标志: 0为取消共享，1为共享
* @param [IN]   ulShareMapNum           地图的个数
* @param [IN]   pstShareMapList         共享/取消地图列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 建议一次共享不要超过8个地图
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessShareMap
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulShareFlag,
    IN  ULONG                   ulShareMapNum,
    IN  SHARE_MAP_BASE_S        *pstShareMapList
);

/**
* 查询组织下面的地图共享信息列表，只获取一层 \n
* 不填查询条件时,查所有,并默认排序(地图名称升序)
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   szOrgCode          组织节点编码
* @param [IN    szExDomainCode     外域编码
* @param [IN]   pstQueryCondition  通用查询条件(可填NULL;可填条件:NAME_TYPE[地图名称])
* @param [IN]   pstQueryPageInfo   请求分页信息
* @param [OUT]  pstRspPageInfo     返回分页信息
* @param [OUT]  pstMapShareList    地图共享信息列表指针
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryMapShareInfoList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  CHAR                            szExDomainCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT SHARE_MAP_INFO_S                *pstMapShareList
);

/**
* 查询共享给外域/外域共享的地图列表
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szExDomainCode          外域编码
* @param [IN]   ulQueryFlag             查询标记，0表示查本域共享给外域的地图，1表示查外域共享给本域的地图
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          返回分页信息
* @param [OUT]  pstShareMapList         查询出来地图列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryShareMapList
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szExDomainCode[IMOS_CODE_LEN],
    IN  ULONG                   ulQueryFlag,
    IN  QUERY_PAGE_INFO_S       *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S         *pstRspPageInfo,
    OUT SHARE_MAP_INFO_S        *pstShareMapList
);

#if 0
#endif

/*************************************************************************************
资源组配置，包含增加，修改，删除，查询，查询列表。
***************************************************************************************/
/**
* 增加资源组
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织编码
* @param [IN]   szGroupName             资源组名称
* @param [IN]   ulGroupResType          资源组类型，枚举为IMOS_TYPE_E，如摄像机组为IMOS_TYPE_CAMERA_GROUP
* @param [IN]   ulResNum                组内资源数目
* @param [IN]   pstResGroupUnit         组内资源单元信息
* @param [OUT]  szResGroupCode          资源组编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前只支持摄像机组/监视器组的配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddResGroup
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szGroupName[IMOS_NAME_LEN],
    IN  ULONG                       ulGroupResType,
    IN  ULONG                       ulResNum,
    IN  RESOURCE_GROUP_UNIT_S       *pstResGroupUnit,
    OUT CHAR                        szResGroupCode[IMOS_RES_CODE_LEN]
);


/**
* 修改资源组
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织编码
* @param [IN]   szResGroupCode          资源组编码
* @param [IN]   szGroupName             资源组名称
* @param [IN]   ulGroupResType          资源组类型，枚举为IMOS_TYPE_E，如摄像机组为IMOS_TYPE_CAMERA_GROUP
* @param [IN]   ulResNum                组内资源数目
* @param [IN]   pstResGroupUnit         组内资源单元信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前只支持摄像机组/监视器组的配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyResGroup
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  CHAR                        szResGroupCode[IMOS_RES_CODE_LEN],
    IN  CHAR                        szGroupName[IMOS_NAME_LEN],
    IN  ULONG                       ulGroupResType,
    IN  ULONG                       ulResNum,
    IN  RESOURCE_GROUP_UNIT_S       *pstResGroupUnit
);

/**
* 删除资源组
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulGroupResType          资源组类型，枚举为IMOS_TYPE_E，如摄像机组为IMOS_TYPE_CAMERA_GROUP
* @param [IN]   szResGroupCode          资源组编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelResGroup
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulGroupResType,
    IN  CHAR                            szResGroupCode[IMOS_RES_CODE_LEN]
);

/**
* 查询资源组信息
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szResGroupCode          资源组编码
* @param [IN]   ulGroupResType          资源组类型
* @param [OUT]  szGroupName             资源组名称
* @param [OUT]  pulResNum               组内资源数目
* @param [OUT]  pstResGroupUnit         组内资源单元信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResGroup
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szResGroupCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulGroupResType,
    OUT CHAR                            szGroupName[IMOS_NAME_LEN],
    OUT ULONG                           *pulResNum,
    OUT RESOURCE_GROUP_UNIT_S           *pstResGroupUnit
);

/**
* 分页查询某个组织下的资源组列表  \n
* 条件为空时,查询组织下所有资源组列表,并默认按照"资源组名称升序"排序.
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   ulGroupResType          资源组类型
* @param [IN]   pstQueryCondition       通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[资源组名称])
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstResGroupList         资源组列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResGroupList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulGroupResType,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT RESOURCE_GROUP_INFO_S       *pstResGroupList
);


#if 0
#endif

/*************************************************************************************
布局配置，包含增加，修改，删除，查询，查询列表。
***************************************************************************************/
#if 0/* 暂时只提供查询功能 */
/**
* 增加布局
* @param [IN]     pstUserLogIDInfo        用户信息
* @param [INOUT]  pstLayoutInfo           布局信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
ULONG STDCALL IMOS_AddLayout
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    INOUT   LAYOUT_INFO_S               *pstLayoutInfo
);

/**
* 修改布局
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   pstLayoutInfo           布局信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
ULONG STDCALL IMOS_ModifyLayout
(
    IN   USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN   LAYOUT_INFO_S               *pstLayoutInfo
);


/**
* 删除布局
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szLayoutCode            布局编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
ULONG STDCALL IMOS_DelLayout
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szLayoutCode[IMOS_RES_CODE_LEN]
);
#endif

/**
* 查询布局信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szLayoutCode            布局编码
* @param [OUT]  pstLayoutInfo           布局信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryLayout
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szLayoutCode[IMOS_RES_CODE_LEN],
    OUT LAYOUT_INFO_S                   *pstLayoutInfo
);

/**
* 分页查询布局列表  \n
* 条件为空时,查询所有布局列表,并默认按照"布局名称升序"排序.
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstQueryCondition       通用查询条件
* - 可填NULL
* - 可填条件:NAME_TYPE(布局名称),LAYOUT_TYPE(布局类型，取值LAYOUT_TYPE_E)
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstLayoutList           布局列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryLayoutList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT LAYOUT_INFO_QUERY_ITEM_S    *pstLayoutList
);

#if 0
#endif

/*************************************************************************************
组显示配置，包含增加，修改，删除，查询，查询列表。
***************************************************************************************/
/**
* 增加组显示
* @param [IN]       pstUserLogIDInfo        用户信息
* @param [IN]       szOrgCode               组织编码
* @param [IN][OUT]  pstSalvoInfo            组显示信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 组显示信息中组显示编码可以为空;若入参的组显示编码为空, 则携带出内部生成的组显示编码)
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddSalvo
(
    IN      USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN      CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    INOUT   SALVO_INFO_S                *pstSalvoInfo
);

/**
* 修改组显示
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstSalvoInfo            组显示信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifySalvo
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN   SALVO_INFO_S               *pstSalvoInfo
);


/**
* 删除组显示
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szSalvoCode             组显示编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelSalvo
(
    IN   USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN   CHAR                       szSalvoCode[IMOS_CODE_LEN]
);

/**
* 查询组显示信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szSalvoCode             组显示编码
* @param [OUT]  pstSalvoInfo            组显示信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySalvo
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSalvoCode[IMOS_CODE_LEN],
    OUT SALVO_INFO_S                    *pstSalvoInfo
);

/**
* 分页查询某个组织下的组显示列表  \n
* 条件为空时,查询组织下所有组显示列表,并默认按照"组显示名称升序"排序.
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstQueryCondition       通用查询条件
* - 可填NULL
* - 可填条件:NAME_TYPE(组显示名称),RES_SUB_TYPE(组显示类型，取值#SALVO_TYPE_E)
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSalvoList            组显示列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySalvoList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SALVO_QUERY_ITEM_S          *pstSalvoList
);

/**
* 启动组显示 \n
* @param [IN]   pstUserLoginIDInfo          用户登录ID信息标识
* @param [IN]   szSalvoCode                 组显示编码
* @param [IN]   ulStreamType                流类型, 取值为#IMOS_FAVORITE_STREAM_E
* @param [IN]   ulOperateCode               操作原因码, 取值为#CS_OPERATE_CODE_E
* @param [IN]   ulSrcHandle                 启动组显示源句柄
* @param [IN]   ulMsgSequence               消息序列号
* @param [OUT]  pulSalvoUnitNum             组显示单元数目
* @param [OUT]  pstSalvoUnitList            组显示单元信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 手动启动组显示不使用ulSrcHandle, 可填无效值IMOS_INVALID_HANDLE;
        手动启动组显示不使用ulMsgSequence, 可填无效值0;
*       组轮巡启动组显示时ulSrcHandle有效, 填写组轮巡上报消息中携带的源句柄;
        组轮巡启动组显示时ulMsgSequence有效, 填写组轮巡上报消息携带的消息序列号
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartSalvo
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szSalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                       ulStreamType,
    IN  ULONG                       ulOperateCode,
    IN  ULONG                       ulSrcHandle,
    IN  ULONG                       ulMsgSequence,
    OUT ULONG                       *pulSalvoUnitNum,
    OUT CS_SALVO_START_SALVO_UNIT_S *pstSalvoUnitList
);

/**
* 停止组显示 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szSalvoCode             组显示编码
* @param [IN]   ulOperateCode           操作原因码, 取值为#CS_OPERATE_CODE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopSalvo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szSalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulOperateCode
);

/**
* 启动/停止/前进/后退/恢复/暂停组轮巡 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szGroupsalvoCode        组轮巡编码
* @param [IN]   ulProcessType           操作类型: 启动/停止/前进/后退/恢复/暂停 #MANUAL_OPER_TYPE_E
* @param [IN]   ulOperateCode           操作原因码: #CS_OPERATE_CODE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessGroupsalvo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szGroupsalvoCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulProcessType,
    IN  ULONG                   ulOperateCode
);

/**
* 启动/停止组轮巡计划 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息标识
* @param [IN]   szGroupsalvoPlanCode    组轮巡计划编码
* @param [IN]   ulProcessType           操作类型: 启动/停止
* @param [IN]   ulOperateCode           操作原因码: #CS_OPERATE_CODE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessGroupsalvoPlan
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLoginIDInfo,
    IN  CHAR                    szGroupsalvoPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulProcessType,
    IN  ULONG                   ulOperateCode
);

#if 0
#endif

/*************************************************************************************
通用轮切资源配置，包含增加，修改，删除，查询，查询列表。
***************************************************************************************/
/**
* 增加通用轮切资源
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstSwitchInfo           通用轮切资源信息
* @param [OUT]  szSwitchResCode         通用轮切资源编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前只支持组轮巡
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_SWITCH_RESOURCE_S    *pstSwitchInfo,
    OUT CHAR                        szSwitchResCode[IMOS_RES_CODE_LEN]
);

/**
* 修改通用轮切资源
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szSwitchResCode         通用轮切资源编码
* @param [IN]   pstSwitchInfo           通用轮切资源信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szSwitchResCode[IMOS_RES_CODE_LEN],
    IN  COMMON_SWITCH_RESOURCE_S    *pstSwitchInfo
);

/**
* 删除通用轮切资源
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulSwitchType            轮切类型，取值为#COMMON_SWITCH_TYPE_E
* @param [IN]   szSwitchResCode         通用轮切资源编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ULONG                   ulSwitchType,
    IN  CHAR                    szSwitchResCode[IMOS_RES_CODE_LEN]
);


/**
* 查询通用轮切资源信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulSwitchType            轮切类型，取值为#COMMON_SWITCH_TYPE_E
* @param [IN]   szSwitchResCode         通用轮切资源编码
* @param [OUT]  pstSwitchInfo           通用轮切资源信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchRes
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchType,
    IN  CHAR                            szSwitchResCode[IMOS_RES_CODE_LEN],
    OUT COMMON_SWITCH_RESOURCE_S        *pstSwitchInfo
);

/**
* 分页查询某个组织下的通用轮切资源列表  \n
* 条件为空时,查询组织下所有通用轮切资源列表,并默认按照"轮切资源名称升序"排序.
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   ulSwitchType            轮切类型，取值为#COMMON_SWITCH_TYPE_E
* @param [IN]   pstQueryCondition       通用查询条件
* - 可填NULL
* - 可填条件:NAME_TYPE(轮切资源名称),RES_SUB_TYPE(轮切子类型，对组轮巡取值#GROUP_SWITCH_TYPE_E)
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSwitchResList        通用轮切资源列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                       ulSwitchType,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SWITCH_RES_QUERY_ITEM_S     *pstSwitchResList
);

#if 0
#endif

/*************************************************************************************
通用轮切计划配置，包含增加，修改，删除，查询，查询列表。
***************************************************************************************/
/**
* 增加通用轮切计划
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstSwitchPlanInfo       通用轮切计划信息
* @param [OUT]  szSwitchPlanCode        通用轮切计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo,
    OUT CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);


/**
* 修改通用轮切计划
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szSwitchPlanCode        轮切计划编码
* @param [IN]   pstSwitchPlanInfo       轮切计划信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    IN  COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo
);


/**
* 删除通用轮切计划
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulSwitchPlanType        通用轮切计划类型，取值为#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   szSwitchPlanCode        通用轮切计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchPlanType,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN]
);

/**
* 查询通用轮切计划信息
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   ulSwitchPlanType        通用轮切计划类型，取值为#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   szSwitchPlanCode        通用轮切计划编码
* @param [OUT]  pstSwitchPlanInfo       通用轮切计划信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulSwitchPlanType,
    IN  CHAR                            szSwitchPlanCode[IMOS_RES_CODE_LEN],
    OUT COMMON_SWITCH_PLAN_INFO_S       *pstSwitchPlanInfo
);

/**
* 分页查询通用轮切计划列表
* 条件为空时,查询所有通用轮切计划,并默认按照"通用轮切计划名称升序"排序.
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   ulSwitchPlanType        通用轮切计划类型，取值为#COMMON_SWITCH_PLANTYPE_E
* @param [IN]   pstQueryCondition       通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[轮切计划名称])
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSwitchPlanList       通用轮切计划列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - 返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCommonSwitchPlanList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  ULONG                           ulSwitchPlanType,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT COMMONSWITCH_PLAN_QUERY_ITEM_S  *pstSwitchPlanList
);



/**
* 添加厂商信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstFactoryInfo      厂商类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddManufacturerType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  MANUFACTURER_TYPE_INFO_S            *pstManufacturerInfo
);

/**
* 删除厂商信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   ulFactoryTypeId     厂商类型信息ID
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelManufacturerType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulManufacturerTypeId
);

/**
* 修改厂商信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstFactoryInfo      厂商类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyManufacturerType
(
    IN    USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN    MANUFACTURER_TYPE_INFO_S            *pstManufacturerInfo
);

/**
* 查询厂商信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   ulTypeId           类型ID
* @param [OUT]  pstFactoryInfo     类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_MANUFACUTURER_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryManufacturerType
(
    IN   USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  ULONG                              ulManufacturerTypeId,
    OUT  MANUFACTURER_TYPE_INFO_S          *pstManufacturerInfo
);

/**
* 查询厂商类型列表 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstQueryCondition  通用查询条件
* @param [IN]   pstQueryPageInfo   查询分页信息
* @param [OUT]  pstRspPageInfo     查询返回分页信息
* @param [OUT]  pstFactoryInfoList 类型信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryManufacturerTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT MANUFACTURER_TYPE_INFO_S        *pstManufacturerInfoList
);

/**
* 添加设备类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstDevTypeInfo     设备类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEV_TYPE_NOT_EXIST 设备类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDevType
(
    IN  USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN  DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* 删除设备类型信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   ulDevTypeId         设备信息类型ID
* @param [IN]   ulDevSubTypeID      设备信息子类型ID
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEV_TYPE_NOT_EXIST 设备类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelDevType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulDevTypeId,
    IN  ULONG                               ulDevSubTypeID
);

/**
* 修改设备类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstDevTypeInfo     设备类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEV_TYPE_NOT_EXIST 设备类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyDevType
(
    IN    USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN    DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* 查询设备类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   ulTypeId           类型ID
* @param [IN]   ulDevSubTypeId     设备信息子类型ID
* @param [OUT]  pstDevTypeInfo     设备类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DEV_TYPE_NOT_EXIST 设备类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevType
(
    IN   USER_LOGIN_ID_INFO_S           *pstUserLogIDInfo,
    IN   ULONG                          ulDevTypeId,
    IN   ULONG                          ulDevSubTypeId,
    OUT  DEV_TYPE_INFO_S                *pstDevTypeInfo
);

/**
* 查询设备类型列表 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstQueryCondition  通用查询条件
* @param [IN]   pstQueryPageInfo   查询分页信息
* @param [OUT]  pstRspPageInfo     查询返回分页信息
* @param [OUT]  pstDevTypeInfoList 类型信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryDevTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT DEV_TYPE_INFO_S                 *pstDevTypeInfoList
);

/**
* 添加事件类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstEventType       事件类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_TYPE_ID_EXIST 事件类型ID已存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddEventType
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  EVENT_TYPE_S                *pstEventType
);

/**
* 删除事件类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   ulDevTypeId        事件信息类型ID
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST 事件类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelEventType
(
    IN  USER_LOGIN_ID_INFO_S                *pstUserLogIDInfo,
    IN  ULONG                               ulEventTypeId
);

/**
* 修改事件类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   pstEventType       事件类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST 事件类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyEventType
(
    IN    USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN    EVENT_TYPE_INFO_S               *pstEventTypeInfo
);

/**
* 查询事件类型信息 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   ulDevTypeId        事件信息类型ID
* @param [OUT]  pstEventTypeInfo   事件类型信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_EVENT_TYPE_NOT_EXIST 事件类型不存在
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventType
(
    IN   USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN   ULONG                            ulEventTypeId,
    OUT  EVENT_TYPE_INFO_S                *pstEventTypeInfo
);

/**
* 查询事件类型列表 \n
* @param [IN]   pstUserLogIDInfo   用户登录ID信息标识
* @param [IN]   ulDevTypeId        设备类型ID
* @param [IN]   ulDevSubTypeId     设备信息子类型ID
* @param [IN]   pstQueryCondition  通用查询条件
* @param [IN]   pstQueryPageInfo   查询分页信息
* @param [OUT]  pstRspPageInfo     查询返回分页信息
* @param [OUT]  pstEventTypeInfo   事件类型信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryEventTypeList
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulDevTypeId,
    IN  ULONG                           ulDevSubTypeId,
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT EVENT_TYPE_INFO_S               *pstEventTypeInfoList
);
/***************************************************************************
 订阅
****************************************************************************/
/**
* 订阅告警 \n
* @param [IN]   pstUserLogIDInfo    用户信息标识
* @param [IN]   pstSubscInfo        订阅信息
* @param [OUT]  szSessionID         返回订阅成功的业务ID
* @return 返回如下结果:
* - 成功:
* - 失败:
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_SubscribeEvent
(
    IN USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN SUBSC_BASE_INFO_S      *pstSubscInfo,
    OUT CHAR  szSessionID[IMOS_SSN_ID_LEN]
);

/**
* 续订告警 \n
* @param [IN]   pstUserLogIDInfo    用户信息标识
* @param [IN]   ulExpires           续订时长
* @param [IN]   szSessionID         订阅成功的业务ID
* @return 返回如下结果:
* - 成功:
* - 失败:
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_RenewSubscribe
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN ULONG  ulExpires,
    IN CHAR   szSessionID[IMOS_SSN_ID_LEN]
);

/**
* 取消订阅 \n
* @param [IN]   pstUserLogIDInfo    用户信息标识
* @param [IN]   szSessionID         订阅成功的业务ID
* @return 返回如下结果:
* - 成功:
* - 失败:
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_CancelSubscribe
(
    IN USER_LOGIN_ID_INFO_S *pstUserLogIDInfo,
    IN CHAR  szSessionID[IMOS_SSN_ID_LEN]
);

/**
* 查询订阅列表 \n
* @param [IN]   pstUserLogIDInfo    用户信息标识
* @param [IN]   pstQueryCondition   查询条件
* @param [IN]   pstQueryPageInfo    分页请求信息
* @param [OUT]  pstRspPageInfo      分页请求返回信息
* @param [OUT]  pstSubscList        查询的告警信息
* @return 返回如下结果:
* - 成功:
* - 失败:
* -      返回操作结果码:见结果码文件sdk_err.h
* @note 在查询到多条记录时pstSubscFullInfo是指向查询列表首记录的地址
*       目前不支持查询条件
*/
IMOS_EXPORT ULONG IMOS_QuerySubscribeList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT SUBSC_FULL_INFO_S           *pstSubscList
);

/**
* 新增干线
* @param [IN]   pstUserLogIDInfo 用户登陆信息
* @param [IN]   pstTrunkInfo     干线信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TRUNK_INFO_S            *pstTrunkInfo
);

/**
* 修改干线信息
* @param [IN]   pstUserLogIDInfo 用户登陆信息
* @param [IN]   pstTrunkInfo     干线信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  TRUNK_INFO_S            *pstTrunkInfo
);

/**
* 删除干线信息
* @param [IN]   pstUserLogIDInfo 用户登陆信息
* @param [IN]   szTrunkCode      干线编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* 查询干线列表信息
* @param [IN]   pstUserLogIDInfo     用户登陆信息
* @param [IN]   pstQueryCondition    通用查询条件
* @param [IN]   pstQueryPageInfo     请求分页信息
* @param [OUT]  pstRspPageInfo       返回分页信息
* @param [OUT]  pstTrunkList         干线列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无查询条件，pstQueryCondition可以为NULL,
*       目前只支持名称查询条件.
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTrunkList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRUNK_INFO_S                *pstTrunkList
);

/**
* 添加干线内容信息
* @param [IN]   pstUserLogIDInfo      用户登陆信息
* @param [IN]   szTrunkCode           干线编码
* @param [IN]   ulContentNum          干线内容数目,最大值为IMOS_TRUNK_CONTENT_MAXNUM
* @param [IN]   pstTrunkContentList   干线内容列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddTrunkContent
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN],
    IN  ULONG                   ulContentNum,
    IN  TRUNK_CONTENT_INFO_S    *pstTrunkContentList
);

/**
* 删除干线内容信息
* @param [IN]   pstUserLogIDInfo    用户登陆信息
* @param [IN]   szTrunkContentCode  干线内容编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DeleteTrunkContent
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkContentCode[IMOS_RES_CODE_LEN]
);

/**
* 查询干线内容信息
* @param [IN]   pstUserLogIDInfo     用户登陆信息
* @param [IN]   szTrunkCode          干线编码
* @param [IN]   pstQueryCondition    通用查询条件
* @param [IN]   pstQueryPageInfo     请求分页信息
* @param [OUT]  pstRspPageInfo       返回分页信息
* @param [OUT]  pstTrunkContentList  干线内容列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 目前暂不支持任何查询条件
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryTrunkContentList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szTrunkCode[IMOS_RES_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT TRUNK_CONTENT_INFO_S        *pstTrunkContentList
);

/**
* 启动干线管理
* @param [IN]   pstUserLogIDInfo  用户登陆信息
* @param [IN]   szTrunkCode       干线编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_EnableTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* 停止干线管理
* @param [IN]   pstUserLogIDInfo  用户登陆信息
* @param [IN]   szTrunkCode       干线编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DisableTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTrunkCode[IMOS_RES_CODE_LEN]
);

/**
* 申请干线
* @param [IN]   pstUserLogIDInfo  用户登陆信息
* @param [IN]   szResCode         资源编码
* @param [OUT]  szSsnID           会话ID
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 该函数针对某种可能产生流传输的业务和资源使用:
* -     目前在回放/下载录像前，需要为摄像机先申请干线。
* -     申请成功后如果业务释放需要调用释放函数释放申请的干线。
*/
IMOS_EXPORT ULONG STDCALL IMOS_ApplyTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szResCode[IMOS_RES_CODE_LEN],
    OUT CHAR                    szSsnID[IMOS_SSN_ID_LEN]
);

/**
* 释放干线
* @param [IN]   pstUserLogIDInfo   用户登陆信息
* @param [IN]   szSsnID            会话ID
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_FreeTrunk
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szSsnID[IMOS_SSN_ID_LEN]
);

/**
* 更新系统流封装格式
* @param [IN] pstUserLogIDInfo      用户登录ID信息标识
* @param [IN] pstSysStreamFormat    系统封装流格式信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PARAM_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* 允许配置 UDP+TS , RTP+TS,RTP_NO_RTCP+ES
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigSysStreamFormat
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  SYS_STREAM_FORMAT_S             *pstSysStreamFormat
);

/**
* 查询系统流封装格式
* @param [IN] pstUserLogIDInfo       用户登录ID信息标识
* @param [OUT] pstSysStreamFormat    系统封装流格式信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_PARAM_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QuerySysStreamFormat
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT SYS_STREAM_FORMAT_S             *pstSysStreamFormat
);

/**
* 将摄像机绑定到EC的某个通道上，需要带齐所有信息，支持场名OSD，遮盖区域，视频流，遮挡检测区域，运动检测区域五种元素变长
* @param [IN]   pstUserLogIDInfo             用户信息
* @param [IN]   pstVINBindCamBaseInfo        摄像机及其绑定通道的基本信息
* @param [IN]   pstVINFullVarietyElement     视频通道全部变长元素结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* 1.摄像机编码不能为空,且不能包含空格
* 2.变长元素个数和变长元素内存长度由调用者保证，变长元素个数不可大于变长元素实际内存长度
*/
IMOS_EXPORT ULONG STDCALL IMOS_BindCameraToVideoInChannelV2
(
    IN  USER_LOGIN_ID_INFO_S                    *pstUserLogIDInfo,
    IN  VINCHNL_BIND_CAMERA_BASE_S              *pstVINBindCamBaseInfo,
    IN  VINCHNL_FULL_VARIETY_ELEMENT_S          *pstVINFullVarietyElement
);

/**
* 查询某个组织下的某台设备的摄像机存储资源分配情况列表V2
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          返回分页信息
* @param [OUT]  pstStoreResList         摄像机存储资源分配信息列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 1.如果无查询条件,指针pstQueryCondition可以为NULL
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamStoreResListV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT STORE_RES_QUERY_ITEM_V2_S   *pstStoreResList
);

#if 0
#endif
/**************************************************************************************************************
设备-BM
***************************************************************************************************************/
/** @defgroup group BM 服务器管理
*   服务器管理包括BM,备份资源，备份任务，备份文件，备份计划管理等
*   @{
*/
/**
* @name BM管理
* @{    BM管理
*/
/**
* 添加BM服务器 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBMInfo           BM信息
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BM_INFO_S                   *pstBMInfo
);

/**
* 修改BM服务器 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBMInfo           BM信息
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BM_INFO_S                   *pstBMInfo
);

/**
* 删除BM服务器 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szBMCode            BM编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBM
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szBMCode[IMOS_DEVICE_CODE_LEN]
);

/**
* 查询BM服务器列表 \n
* szOrgCode值无效，填任何值，都只查询本域BM列表
* 条件为空时,查所有,并默认"名称升序排序"  \n
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szOrgCode              组织编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;可填条件:NAME_TYPE[BM名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstSDKBMList           BM列表
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBMList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT BM_QUERY_ITEM_S             *pstSDKBMList
);
/** @} */ /* end of DEVICE BM MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
备份资源管理
****************************************************************************/
/**
* @name 备份资源管理
* @{    涉及BM备份资源管理,摄像机备份资源管理
*/
/**
* 从存储设备为BM服务器分配资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBMResInfo        BM资源信息
* @param [OUT]  szBMResCode         BM资源编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignBMResFromStoreDev
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  DM_RES_FOR_BM_INFO_S    *pstBMResInfo,
    OUT CHAR                    szBMResCode[IMOS_CODE_LEN]
);

/**
* 从第三方资源为BM服务器分配资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBMResInfo        BM资源信息
* @param [OUT]  szBMResCode         BM资源编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignBMResFromThird
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  THIRD_RES_FOR_BM_INFO_S   *pstBMResInfo,
    OUT CHAR                      szBMResCode[IMOS_CODE_LEN]
);

/**
* 删除BM服务器上分配的备份资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN    szResCode           BM资源编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBMRes
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szResCode[IMOS_CODE_LEN]
);

/**
* 查询BM备份资源列表 \n
* 目前只支持组织编码szOrgCode允许为空串的查询 \n
* 条件为空时,查所有,并默认"名称升序排序"  \n
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   szBMCode               BM编码
* @param [IN]   pstQueryCondition      通用查询条件(可填NULL;可填条件:NAME_TYPE[BM资源名称])
* @param [IN]   pstQueryPageInfo       请求分页信息
* @param [OUT]  pstRspPageInfo         响应分页信息
* @param [OUT]  pstSDKBMResList        BM备份资源列表
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBMResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szBMCode[IMOS_DEVICE_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT BM_RES_QUERY_ITEM_S         *pstSDKBMResList
);
#if 0
#endif
/**
* 分配摄像机备份资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstCamBakResInfo    摄像机备份资源信息
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_AssignCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_BAK_RES_INFO_S      *pstCamBakResInfo
);

/**
* 修改摄像机独享备份资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstCamBakResInfo    摄像机备份资源信息
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CAM_BAK_RES_INFO_S      *pstCamBakResInfo
);

/**
* 删除摄像机备份资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCamCode           摄像机编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szCamCode[IMOS_DEVICE_CODE_LEN]
);

/**
* 查询摄像机备份资源 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szCamCode           摄像机编码
* @param [OUT]  ulItemNum           摄像机备份资源信息条数
* @param [OUT]  pstCamBakResList    摄像机备份资源列表
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机在本域的备份资源查询
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCameraBakRes
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szCamCode[IMOS_DEVICE_CODE_LEN],
    OUT ULONG                     *pulItemNum,
    OUT CAM_BAK_RES_INFO_S        *pstCamBakResList
);


/**
* 分页查询某个组织下摄像机备份资源分配情况列表
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   szOrgCode               组织编码
* @param [IN]   pstQueryCondition       通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[摄像机名称])
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstCamBakResList        摄像机备份资源分配信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机在本域的备份资源查询
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryCamBakResList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_DOMAIN_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CAM_BAK_RES_QUERY_ITEM_S    *pstCamBakResList
);

/** @} */ /* end of BACKUP RESOURCE MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
备份任务管理
****************************************************************************/
/**
* @name 备份任务管理
* @{    备份任务管理
*/
/**
* 设置手动备份 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstManulBakInfo     手动备份任务信息
* @param [OUT]  szTaskCode          备份任务编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行手动备份
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetManualBak
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  MANUL_BAK_INFO_S          *pstManulBakInfo,
    OUT CHAR                      szTaskCode[IMOS_CODE_LEN]
);

/**
* 删除备份任务 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szTaskCode          备份任务编码
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBakTask
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szTaskCode[IMOS_CODE_LEN]
);

/**
* 查询备份任务列表 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   ulTaskType              任务类型#BAK_TYPE_E
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSDKBakTaskList       备份任务列表
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1、通用查询条件支持:任务状态[#TASK_STATUS]、任务提交时间[#TASK_SUB_TIME]、任务结束时间[#TASK_END_TIME]、摄像机名称[#NAME_TYPE]、
* -    录像开始时间[#REC_START_TIME]、录像结束时间[#REC_END_TIME]、用户名称[#USER_NAME_TYPE]、案例描述[#CASE_DESC]的查询和排序;
* - 2、通用查询条件为NULL,或无排序条件时,默认按"任务提交时间升序"排序;
* - 3、非admin用户的任务类型只能是手动任务[#BAK_MANUL];
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBakTaskList
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S   *pstQueryCondition,
    IN  ULONG                      ulTaskType,
    IN  QUERY_PAGE_INFO_S          *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S            *pstRspPageInfo,
    OUT BAK_TASK_QUERY_ITEM_S      *pstSDKBakTaskList
);
/** @} */ /* end of BACKUP TASK MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
备份文件管理
****************************************************************************/
/**
* @name 备份文件管理
* @{    备份文件管理
*/
/**
* 操作备份文件 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBakFileInfo      备份文件操作信息
* @return ULONG 返回如下结果：
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_HandleBakFile
(
    IN  USER_LOGIN_ID_INFO_S       *pstUserLogIDInfo,
    IN  BAK_FILE_HANDLE_INFO_S     *pstBakFileInfo
);

/**
* 备份录像检索 \n
* @param [IN]   pstUserLogIDInfo        用户登录ID信息标识
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstSDKRecordFileInfo    备份文件信息数据结构体
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1、通用查询条件支持:摄像机编码[#CODE_TYPE]、摄像机名称[#NAME_TYPE]、文件生成时间[#FILE_CREATE_TIME]、
* -    备份开始时间[#BAK_START_TIME]、备份结束时间[#BAK_END_TIME]、文件容量[#FILE_CAPACITY]、文件类型[#FILE_TYPE]、
* -    文件锁定标识[#FILE_LOCK_FLAG]、案例描述[#CASE_DESC]的查询和排序;
* - 2、通用查询条件为NULL,或无排序条件时,默认按"备份开始时间升序"排序;
* - 3、通用查询条件中添加的备份开始时间与结束时间之间的时间跨度不能跨天，且时间格式为："%Y-%m-%d %H:%M:%S"格式 ，信息长度限定为24字符
*/
IMOS_EXPORT ULONG STDCALL IMOS_BakRecordRetrieval
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT CAM_BAK_FILE_QUERY_ITEM_S   *pstRecordFileInfo
);

/**
* 获取备份录像文件的URL信息 \n
* @param[IN]    pstUserLogIDInfo        用户登录ID信息标识
* @param[IN]    pstSDKGetBakUrlInfo     获取URL请求消息数据结构
* @param[OUT]    pstSDKURLInfo          URL信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_GetBakRecordFileURL
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  GET_BAK_URL_INFO_S          *pstSDKGetBakUrlInfo,
    OUT URL_INFO_S                  *pstSDKURLInfo
);
/** @} */ /* end of BACKUP FILE MANAGEMENT MODULE */

#if 0
#endif
/***************************************************************************
备份计划
****************************************************************************/
/**
* @name 备份计划管理
* @{    涉及备份计划的删改查,以及查询备份计划列表接口
*/
/**
* 添加备份计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBakPlanInfo      备份计划信息
* @param [OUT]  szBakPlanCode       备份计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  BAK_PLAN_INFO_S             *pstBakPlanInfo,
    OUT CHAR                        szBakPlanCode[IMOS_RES_CODE_LEN]
);

/**
* 修改备份计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   pstBakPlan          备份计划信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 启动备份计划需要调用IMOS_ProcessPlan接口; 而通过修改计划时间的方式不能够自动使计划启动/停止
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  BAK_PLAN_INFO_S                 *pstBakPlanInfo
);

/**
* 删除备份计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szBakPlanCode       备份计划编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szBakPlanCode[IMOS_RES_CODE_LEN]
);

/**
* 查询备份计划信息 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   szBakPlanCode       备份计划编码
* @param [OUT]  pstBakPlanInfo      备份计划信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_BAK_PLAN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryBackupPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szBakPlanCode[IMOS_RES_CODE_LEN],
    OUT BAK_PLAN_INFO_S                 *pstBakPlanInfo
);

/**
* 启动/停止计划 \n
* @param [IN]   pstUserLogIDInfo    用户登录ID信息标识
* @param [IN]   ulPlanType          计划类型#PLAN_TYPE_E
* @param [IN]   szStorePlanCode     计划编码
* @param [IN]   ulPlanMode          计划处理标记, #PLAN_OPER_MODE_E
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_STORAGE_PLAN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
* @attention 本接口支持对共享摄像机进行本域配置
*/
IMOS_EXPORT ULONG STDCALL IMOS_ProcessPlan
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  ULONG                           ulPlanType,
    IN  CHAR                            szPlanCode[IMOS_RES_CODE_LEN],
    IN  ULONG                           ulPlanMode
);

/**
* 对内初始化总入口。 \n
* @param [IN]       szServerAdd     服务器IP
* @param [IN]       ulServerPort    服务器端口
* @param [IN][OUT]  pstInit         SDK初始化参数，其中pParam字段为INOUT型
* @return ULONG  返回如下结果:
* - 成功：ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码，见各个组的错误码文件。
* @note szServerAdd的取值为127.0.0.0,127.0.0.1,VM服务器地址
*       ulServerPort的取值为8800
*/
IMOS_EXPORT ULONG STDCALL IMOS_InterInitiate
(
    IN    CHAR       szServerAdd[IMOS_IPADDR_LEN],
    IN    ULONG      ulServerPort,
    INOUT SDK_INIT_S *pstInit
);

/* added by z06806 for vodwall prj 20101211 begin */
/**
* 开始回放上墙 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息
* @param [IN]   szXPCode                XP编码
* @param [IN]   szMonitorCode           监视器编码
* @param [IN]   ulOperateCode           操作原因码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note ulOperateCode取值参见CS_OPERATE_CODE_E
*       szMonitorCode在回放上墙中为对应监视器编码
*       szXPCode在回放上墙中为对应窗格编码
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartVodToTV
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLoginIDInfo,
    IN  CHAR                        szXPCode[IMOS_CODE_LEN],
    IN  CHAR                        szMonitorCode[IMOS_CODE_LEN],
    IN  ULONG                       ulOperateCode
);

/**
* 停止回放上墙 \n
* @param [IN]   pstUserLoginIDInfo      用户登录ID信息
* @param [IN]   ulCodeType              编码类型，见VODWALL_STOP_CODETYPE_E
* @param [IN]   szXPCode                XP编码
* @param [IN]   szMonitorCode           监视器编码
* @param [IN]   ulOperateCode           操作原因码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note ulOperateCode取值参见CS_OPERATE_CODE_E
*       如果编码类型为监视器，则szMonitorCode填监视器编码，szXPCode可以不填
*       如果编码类型为XP，则szXPCode填XP编码，szMonitorCode可以不填
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopVodToTV
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLoginIDInfo,
    IN  ULONG                           ulCodeType,
    IN  CHAR                            szXPCode[IMOS_CODE_LEN],
    IN  CHAR                            szMonitorCode[IMOS_CODE_LEN],
    IN  ULONG                           ulOperateCode
);
/* added by z06806 for vodwall prj 20101211 end */

/**
* 查询组织下资源信息列表(V3，返回RES_INFO_S结构，包含资源ID，不过滤权限) \n
* @param [IN]   pstUserLogIDInfo           用户登录ID信息标识
* @param [IN]   szOrgCode                  组织编码
* @param [IN]   pstQueryCondition          查询条件
* @param [IN]   pstQueryPageInfo           请求分页信息
* @param [OUT]  pstRspPageInfo             响应分页信息
* @param [OUT]  pstResList                 资源信息列表
* @return ULONG 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DOMAIN_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 过滤
* - 1.条件可以为NULL,表示查指定组织下所有资源(不查子组织)
* - 2.支持按"是否查子组织"条件查询:#IS_QUERY_SUB = 1 是;= 0 否
* - 3.支持按"资源名称"模糊查询:#NAME_TYPE
* - 4.支持按"是否外域资源"查询:#DOMAIN_TYPE = 1 是;= 0 否
* - 5.支持按"资源类型"查询:#RES_TYPE,取值为#IMOS_TYPE_E
* - 6.支持按"资源子类型"查询:#RES_SUB_TYPE(资源类型为摄像机时,取值为#CAMERA_TYPE_E)
* - 排序
* - 1.不指定排序条件情况下,默认按"资源名称"升序排序
* - 2.支持的排序条件,包括:#NAME_TYPE(资源名称),#CODE_TYPE(资源编码),#RES_TYPE(资源类型),#RES_SUB_TYPE(资源子类型),
* -   #DOMAIN_TYPE(是否外域资源)
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryResourceListNoAuth
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT RES_INFO_S                      *pstResList
);

/**
* 根据角色编码，设置角色权限(V2) \n
* @param [IN]   pstUserLogIDInfo      用户登录ID信息标识
* @param [IN]   szRoleCode            角色编码，对应唯一一个角色
* @param [IN]   ulResAuthorityNum     资源授权信息数量
* @param [IN]   pstResAuthorityList   资源授权信息列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_ROLE_NOT_EXIST
* - #ERR_MM_AUTHORITY_NOT_DEFINED
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* -     1.该接口配置的资源范围仅限于角色所属组织和子组织
* -     2.授予的权限可以是系统权限，也可以是针对资源（包括组织、摄像头、监视器等）的权限
* -     3.调用该接口后, 以该次调用的权限为准，角色之前的权限会被覆盖
* -     4.ulResAuthorityNum设置为0，同时pstResAuthorityList设置为NULL，表示清除角色所有权限。
*/
IMOS_EXPORT ULONG STDCALL IMOS_SetRoleAuthorityV2
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szRoleCode[IMOS_CODE_LEN],
    IN  ULONG                       ulResAuthorityNum,
    IN  RES_AUTHORITY_S             *pstResAuthorityList
);

/**
* 根据角色编码，查询某个角色的权限信息(V2) \n
* @param [IN]   pstUserLogIDInfo             用户登录ID信息标识
* @param [IN]   szRoleCode                   角色编码，对应唯一一个角色
* @param [IN]   pstQueryCondition            查询条件(支持的查询条件包括:#RES_ID[资源ID]
*                                                                        #RES_TYPE[资源类型]，
*                                                                        #NAME_TYPE[资源名称])
* @param [IN]   pstQueryPageInfo             请求分页信息
* @param [OUT]  pstRspPageInfo               响应分页信息
* @param [OUT]  pstResAuthorityList          角色权限列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @attention
* - 全局权限和其他权限必须分开查询。
* - 查询全局权限时，查询条件为RES_ID等于0，不能有其他查询条件，查询其他权限时，RES_ID大于0，且可以有其他查询条件。
* - 如果权限记录对应的资源不存在，出参角色权限列表中对应该资源的RES_INFO_S除szResIdCode有效，其他字段都无效。
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAuthListByRoleV2
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szRoleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S        *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S               *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                 *pstRspPageInfo,
    OUT RES_AUTHORITY_QUERY_ITEM_S      *pstResAuthorityList
);

/**
* 删除角色的一条授权记录 \n
* @param [IN]   pstUserLogIDInfo             用户登录ID信息标识
* @param [IN]   szRoleCode                   角色编码，对应唯一一个角色
* @param [IN]   szResIdCode                  资源ID，字符串"0"表示删除角色的系统权限
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelAuthItemByRole
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szRoleCode[IMOS_CODE_LEN],
    IN  CHAR                    szResIdCode[IMOS_CODE_LEN]
);

#if 0
#endif
/***************************************************************************
IPC光学参数配置
****************************************************************************/
/**
* 设置光学参数
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstChannelIndex             设备通道索引信息
* @param [IN]   pstOpticsParam              光学参数信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_OPT_PARAM_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ConfigOpticsParam
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    IN  OPTICS_PARAM_S                  *pstOpticsParam
);

/**
* 查询光学参数
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstChannelIndex             设备通道索引信息
* @param [OUT]  pstOpticsParam              光学参数信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_OPT_PARAM_NOT_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryOpticsParam
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  DEV_CHANNEL_INDEX_S             *pstChannelIndex,
    OUT OPTICS_PARAM_S                  *pstOpticsParam
);

/** @} */ /* end of BACKUP PLAN MANAGEMENT MODULE */
/** @} */

/**
* 通过XML增加设备通用接口
* @param [IN]   pstUserLogIDInfo       用户登录ID信息标识
* @param [IN]   ulXmlLen               设备的XML长度信息
* @param [IN]   pcXml                  设备的XML信息
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* - #ERR_DAO_DOMAIN_NOT_EXIST
* - #ERR_AS_EC_NUM_EXCEEDED
* - #ERR_MM_PARENT_DEIVCE_NOT_EXIST
* - #ERR_DAO_DEVICE_CODE_EXIST
* -     返回操作结果码：见结果码文件sdk_err.h
* @note XML格式请见"批量导入.xml"文件
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddDevByXML
(
    IN    USER_LOGIN_ID_INFO_S          *pstUserLogIDInfo,
    IN    ULONG                         ulXmlLen,
    IN    CHAR                          *pcXml
);

/**
* 启动自动发现
* @param [IN]   pstUserLogIDInfo        用户信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StartAutoFind
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo
);

/**
* 停止自动发现
* @param [IN]   pstUserLogIDInfo        用户信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_StopAutoFind
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo
);

/**
* 查询自动发现状态
* @param [IN]   pstUserLogIDInfo    用户信息
* @param [OUT]  pulstate            自动发现状态，1代表启动，0代表停止，系统每次启动默认停止状态
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryAutoFindState
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    OUT ULONG                           *pulstate
);

#if 0
#endif
/**************************************************************************************************************
ECR33**-HF
***************************************************************************************************************/
/** @defgroup group ECR33**-HF
*   包括VM管理ECR, ECR设备管理两部分
*   @{
*/
/***************************************************************************
PartI VM管理ECR
****************************************************************************/
/**
* @PartI VM管理ECR
* @{    VM管理ECR
*/
/**
* 添加带存储的编码器, 仅支持HF系列. \n
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   pstECRBasicInfo    带存储的编码器基本信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_AddECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* 修改带存储的编码器, 仅支持HF系列. \n
* 目前仅支持修改名称.
* @param [IN]   pstUserLogIDInfo   用户信息
* @param [IN]   pstECRBasicInfo    带存储的编码器基本信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_ModifyECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* 删除带存储的编码器, 仅支持HF系列. \n
* @param [IN]   pstUserLogIDInfo    用户信息
* @param [IN]   szECRCode           带存储的编码器编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelECR
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN]
);

/**
* 查询带存储的编码器, 仅支持HF系列. \n
* @param [IN]   pstUserLogIDInfo    用户信息
* @param [IN]   szECRCode           带存储的编码器编码
* @param [OUT]  pstECRBasicInfo     带存储的编码器基本信息
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRInfo
(
    IN  USER_LOGIN_ID_INFO_S    *pstUserLogIDInfo,
    IN  CHAR                    szECRCode[IMOS_ECR_CODE_LEN],
    OUT ECR_BASIC_INFO_S        *pstECRBasicInfo
);

/**
* 查询某个组织下带存储的编码器列表信息, 仅支持HF系列. \n
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szOrgCode               组织域编码
* @param [IN]   pstQueryCondition       通用查询条件(可为NULL;支持的查询条件包括:#NAME_TYPE[带存储的编码器名称])
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          响应分页信息
* @param [OUT]  pstECRBasicInfoList     带存储的编码器基本信息列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szOrgCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ECR_BASIC_INFO_S            *pstECRBasicInfoList
);

/**
* 查询ECR的摄像机列表, 仅支持HF系列.
* @param [IN]   pstUserLogIDInfo        用户信息
* @param [IN]   szECRCode               ECR编码
* @param [IN]   pstQueryCondition       通用查询条件
* @param [IN]   pstQueryPageInfo        请求分页信息
* @param [OUT]  pstRspPageInfo          返回分页信息
* @param [OUT]  pstECRCamList           查询出来摄像机列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note
* - 1. 如果无查询条件，指针pstQueryCondition可以为NULL;
* - 2. 支持按摄像机编码、名称排序，对应通用查询条件中的查询项类型分别为:#CODE_TYPE、#NAME_TYPE;
* - 3. 若排序条件不填，将默认按照摄像机编码升序排序;
*/
IMOS_EXPORT ULONG STDCALL IMOS_QueryECRCameraList
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szECRCode[IMOS_ECR_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S    *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S           *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S             *pstRspPageInfo,
    OUT ECR_CAM_INFO_S              *pstECRCamList
);

/**
* 删除ECR摄像机, 会将当前业务停止; 用于删除ECR时的操作; 仅支持HF系列.
* @param [IN]   pstUserLogIDInfo        用户信息标识
* @param [IN]   szCamCode               待删除的摄像机编码
* @param [IN]   szECRCode               外域编码
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_DelCameraForECR
(
    IN  USER_LOGIN_ID_INFO_S        *pstUserLogIDInfo,
    IN  CHAR                        szCamCode[IMOS_CODE_LEN],
    IN  CHAR                        szECRCode[IMOS_ECR_CODE_LEN]
);
/** @} */ /* end of PartI：VM管理ECR */

/***************************************************************************
PartII ECR设备管理
****************************************************************************/
/**
* @PartII ECR设备管理
* @{    ECR设备管理
*/
/** @} */ /* end of PartII:ECR设备管理 */


/** @} *//* end of group ECR33**-HF */

#if 0
#endif
/***************************************************************************
告警订阅
****************************************************************************/
/**
* 增加告警订阅规则（非系统预定义规则告警源不能为空，系统预定义规则告警源必须为空）
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstRule                     告警订阅规则
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 不允许增加系统预定义规则
*/
IMOS_EXPORT ULONG IMOS_AddAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN  ALARM_SENDING_RULE_S   *pstRule
);

/**
* 删除告警订阅规则（系统预定义规则不能删除）
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstRule                     告警订阅规则编码
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_DelAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S   *pstUserLogIDInfo,
    IN  CHAR                   szRuleCode[IMOS_CODE_LEN]
);

/**
* 修改告警订阅规则（非系统预定义规则告警源不能为空，系统预定义规则告警源必须为空）
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   pstRule                     告警订阅规则
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_ModifyAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S  *pstUserLogIDInfo,
    IN  ALARM_SENDING_RULE_S  *pstRule
);

/**
* 查询告警订阅规则基本信息
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szRuleCode                  告警订阅规则编码
* @param [OUT]  pstRule                     告警订阅规则
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRule
(
    IN  USER_LOGIN_ID_INFO_S            *pstUserLogIDInfo,
    IN  CHAR                            szRuleCode[IMOS_CODE_LEN],
    OUT ALARM_SENDING_RULE_BASE_INFO_S  *pstRule
);

/**
* 查询告警订阅规则告警源列表（最多每次查询IMOS_ALARM_SEND_SRC_MAXNUM条记录）
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szRuleCode                  告警订阅规则编码
* @param [IN]   pstQueryCondition           通用查询条件
* @param [IN]   pstQueryPageInfo            请求分页信息
* @param [OUT]  pstRspPageInfo              返回分页信息
* @param [OUT]  pstAlarmSrcList             告警源列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 支持的查询条件：NAME_TYPE[告警源名称]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleAlarmSrcList
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szRuleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S  *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S         *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S           *pstRspPageInfo,
    OUT ALARM_SRC_QUERY_ITEM_S    *pstAlarmSrcList
);

/**
* 查询告警订阅规则用户列表（最多每次查询IMOS_ALARM_SEND_USER_MAXNUM条记录）
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szRuleCode                  告警订阅规则编码
* @param [IN]   pstQueryCondition           通用查询条件
* @param [IN]   pstQueryPageInfo            请求分页信息
* @param [OUT]  pstRspPageInfo              返回分页信息
* @param [OUT]  pstAlarmSrcList             告警源列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 支持的查询条件：USER_NAME_TYPE[用户名称]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleUserList
(
    IN  USER_LOGIN_ID_INFO_S      *pstUserLogIDInfo,
    IN  CHAR                      szRuleCode[IMOS_CODE_LEN],
    IN  COMMON_QUERY_CONDITION_S  *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S         *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S           *pstRspPageInfo,
    OUT USER_QUERY_ITEM_S         *pstUserList
);

/**
* 查询告警订阅规则列表
* @param [IN]   pstUserLogIDInfo            用户登录ID信息标识
* @param [IN]   szRuleCode                  告警订阅规则编码
* @param [IN]   pstQueryCondition           通用查询条件
* @param [IN]   pstQueryPageInfo            请求分页信息
* @param [OUT]  pstRspPageInfo              返回分页信息
* @param [OUT]  pstAlarmSrcList             告警订阅规则列表
* @return 返回如下结果：
* - 成功：
* - #ERR_COMMON_SUCCEED
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 支持的查询条件：NAME_TYPE[规则名称]
*/
IMOS_EXPORT ULONG IMOS_QueryAlarmSendingRuleList
(
    IN  USER_LOGIN_ID_INFO_S             *pstUserLogIDInfo,
    IN  COMMON_QUERY_CONDITION_S         *pstQueryCondition,
    IN  QUERY_PAGE_INFO_S                *pstQueryPageInfo,
    OUT RSP_PAGE_INFO_S                  *pstRspPageInfo,
    OUT ALARM_SENDING_RULE_BASE_INFO_S   *pstAlarmSendingRuleList
);

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

/**
* 通用查询接口
* @param [IN]       pstUserLoginID      用户登录ID信息标识
* @param [IN]       ulQryFlag           查询标志位#UNI_QUERY_FLAG_E
* @param [IN]       ulQryPieceNum       查询PIECE数目
* @param [IN]       pulQryPiecelist     查询PIECE列表
* @param [IN]       pstSdkConditon      查询约束信息
* @param [IN][OUT]  pstSdkPageInfo      查询分页信息
* @param [OUT]      oQryInfo            查询到得PIECE内容列表
* @return 返回如下结果：
* - 成功：
* - 失败：
* -     返回操作结果码：见结果码文件sdk_err.h
* @note 无
*/
IMOS_EXPORT ULONG STDCALL IMOS_Query
(
    IN USER_LOGIN_ID_INFO_S        *pstUserLoginID,
    IN ULONG                       ulQryFlag,
    IN ULONG                       ulQryPieceNum,
    IN ULONG                       *pulQryPiecelist,
    IN UNI_CONDITION_S             *pstConditon,
    INOUT UNI_PAGE_S               *pstPageInfo,
    OUT std::vector<std::string>   &oQryInfo
);

#endif


