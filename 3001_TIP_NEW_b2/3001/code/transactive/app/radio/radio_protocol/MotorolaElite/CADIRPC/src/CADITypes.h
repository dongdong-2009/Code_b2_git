/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/CADIRPC/src/CADITypes.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Decoupled CADI types
  */

#ifndef __CADITypes_H
#define __CADITypes_H

//Haipeng Added the C linkage
#if defined(__cplusplus)
extern "C"
{
#endif
	


	/************************************************************************/
	/* cadi.x constants                                                     */
	/************************************************************************/
//	static const unsigned long CADI_MAX_ALIAS_LEN   = 15;
#define CADI_MAX_ALIAS_LEN 15


	/************************************************************************/
	/* cadcmd.x constants                                                   */
	/************************************************************************/
	static const int CADI_FALSE					= 0;
	static const int CADI_TRUE					= 1;
	static const int CADI_MAX_USERNAME_LEN		= 9;
	static const int CADI_MAX_PASSWORD_LEN		= 31;
	static const long CADI_INVALID_PROGRAM_NUMBER = 0xFFFFFFFF;

	// Define command error/return code
	static const int CADI_SUCCESS				= 0;
	static const int CADI_FAILURE				= -1;
	static const int CADI_LOGIN_REQUIRED			= -2;
	static const int CADI_ZC_CONN_FAILED			= -3;
	static const int CADI_INVALID_RADIO			= -4;
	static const int CADI_INVALID_COMMAND		= -5;
	static const int CADI_INVALID_TARGET			= -6;
	static const int CADI_NO_PERMISSION			= -7;

	typedef int				CADI_CMD_NO;
	typedef CADI_CMD_NO	CADIStatus;


	/************************************************************************/
	/* cadev.x constants                                                    */
	/************************************************************************/
	static const int CADI_INVALID_SUBSCRIBER_ID	= 0;
	static const int CADI_INVALID_MESSAGE_NUMBER	= 0;
	static const int CADI_INVALID_STATUS_NUMBER	= 0;
	static const int CADI_INVALID_ZC_ID			= 0;

	// Qualifiers for "cmd_status_type" field in CADI_CMD_STATUS_DATA
	static const int CADI_ZC_NO_CMD_STATUS		= 0;
	static const int CADI_ZC_SENT_ACK				= 1;
	static const int CADI_ZC_BUSY_ACK				= 2;
	static const int CADI_ZC_REJECTED_NACK		= 3;

	// Qualifiers for the CADI_ZC_STATUS_DATA
	static const int CADI_ZC_STATUS_UNKNOWN		= 0;
	static const int CADI_ZC_STATUS_ACTIVE		= 1;
	static const int CADI_ZC_STATUS_DISABLE		= 2;
	static const int CADI_ZC_STATUS_IDLE			= 3;

	// Qualifiers for the CADI__IZ_TRUNKING_STATUS
	static const int CADI_IZT_CAPABLE				= 1;
	static const int CADI_IZT_INCAPABLE			= 0;


	// Qualifiers for reject_code field (these are from the ZC)
	static const int CADI_NO_REJECT_TYPE					= 0xFF;
	static const int CADI_INVALID_REQ_ID_REJ				= 0x00;
	static const int CADI_INVALID_TARGET_ID_REJ			= 0x01;
	static const int CADI_DISABLED_REQ_ID_REJ				= 0x02;
	static const int CADI_DISABLED_TARGET_ID_REJ			= 0x03;
	static const int CADI_INVALID_TG_REJ					= 0x04;
	static const int CADI_DISABLED_TG_REJ					= 0x05;
	static const int CADI_TARGET_TG_INVALID_REJ			= 0x06;
	static const int CADI_TARGET_TG_DISABLED_REJ			= 0x07;
	static const int CADI_SITEWIDE_INCAPABLE_REJ			= 0x10;
	static const int CADI_MG_INCAPABLE_REJ				= 0x11;
	static const int CADI_DVP_INCAPABLE_REJ				= 0x12;
	static const int CADI_TG_INCAPABLE_REJ				= 0x13;
	static const int CADI_PC_INCAPABLE_REQ_REJ			= 0x14;
	static const int CADI_PC_INCAPABLE_TARGET_REJ			= 0x15;
	static const int CADI_INT_INCAPABLE_REJ				= 0x16;
	static const int CADI_DVP_UPGRADE_REJ					= 0x17;
	static const int CADI_DVP_DNGRADE_REJ					= 0x18;
	static const int CADI_DVP_CODED_ONLY_TARGET_REJ		= 0x19;
	static const int CADI_NO_CAPABLE_RESOURCES_REJ		= 0x20;
	static const int CADI_ASTRO_INCAPABLE_TARGET_REJ		= 0x25;
	static const int CADI_TARGET_NOT_RESPONDED_REJ		= 0x26;
	static const int CADI_ID_INVOLVED_IN_DI_REJ			= 0x27;
	static const int CADI_TG_DENIED_DUE_TO_MG_REJ			= 0x30;
	static const int CADI_INVALID_PATCH_OF_A_MG			= 0x31;
	static const int CADI_INVALID_NORM_TAC_OF_A_MG		= 0x32;
	static const int CADI_INVALID_REPEAT_OF_A_MG			= 0x33;
	static const int CADI_INVALID_CTRL_CNSL_OP_INFO_REJ	= 0x34;
	static const int CADI_NO_AFFILIATED_MEMBERS_REJ		= 0x35;
	static const int CADI_INVALID_MSEL_GROUP_NUMBER		= 0x36;
	static const int CADI_INVALID_MG_IN_MSEL_REQUEST		= 0x37;
	static const int CADI_RQSTR_RECORD_NOT_AVAIL_REJ		= 0x40;
	static const int CADI_TARGET_RECORD_NOT_AVAIL_REJ		= 0x41;
	static const int CADI_TG_REC_NOT_AVAIL_REJ			= 0x42;
	static const int CADI_RQSTR_PROHIBITED_ATIA_SITE_REJ	= 0x43;
	static const int CADI_TARGET_PROHIBITED_ATIA_SITE_REJ	= 0x44;
	static const int CADI_TG_PROHIBITED_ATIA_SITE_REJ		= 0x45;
	static const int CADI_INT_CALL_LIMIT_EXCEEDED_REJ		= 0x46;
	static const int CADI_AUDIO_INTERRUPT_REJ				= 0x47;
	static const int CADI_PC_TARGET_DEAFFILIATED_REJ		= 0x48;
	static const int CADI_PC_TARGET_DEREGISTERED_REJ		= 0x48;
	static const int CADI_NO_BUSY_TO_BUSY_ORIDE_REJ		= 0x49;
	static const int CADI_SITE_ACCESS_DENIAL				= 0x4A;
	static const int CADI_MG_EMRG_INCAPABLE_REJ			= 0x4B;
	static const int CADI_EMRG_NOT_ALLOWED_ON_GRP_REJ		= 0x4C;
	static const int CADI_CA_INCAPABLE_REQ_REJ			= 0x4D;
	static const int CADI_CA_INCAPABLE_TARGET_REJ			= 0x4E;
	static const int CADI_CA_TARGET_DEAFFILIATED_REJ		= 0x4F;
	static const int CADI_CA_TARGET_DEREGISTERED_REJ		= 0x4F;
	static const int CADI_NO_SITEWD_SITES_REJ				= 0x50;
	static const int CADI_SITEWD_OVERLAP_REJ				= 0x51;
	static const int CADI_TARGET_NOT_AFFILIATED			= 0x52;
	static const int CADI_TARGET_NOT_REGISTERED_REJ		= 0x52;
	static const int CADI_CMD_NOT_ALLOWED					= 0x53;
	static const int CADI_UNABLE_TO_GET_PEND_BUFFER_REJ	= 0x54;
	// OmniLink Addition
	static const int CADI_TG_REGRP_INCAPABLE_REJ			= 0x55;
	static const int CADI_INCOMPAT_TG_IND_MOD_TYPE_REJ	= 0x56;
	static const int CADI_INCOMPAT_TG_IND_FRQ_BAND_REJ	= 0x57;
	// OmniLink Modification: used to be 0x2f
	static const int CADI_B_O_NOT_ALLOWED					= 0x58;
	// OmniLink Addition
	static const int CADI_TG_IS_SITE_ONLY_REJ				= 0x59;
	static const int CADI_DATA_PC_TARG_IN_GRP_ACT_REJ		= 0x5A;
	static const int CADI_DATA_PC_TARG_IN_INT_ACT_REJ		= 0x5B;
	static const int CADI_GO_DENIED_DUE_TO_GO_REJ			= 0x5C;
	static const int CADI_GO_DENIED_DUE_TO_LAND_REJ		= 0x5D;
	static const int CADI_INVALID_REQ_AUD_TME_SLT_REJ		= 0x5E;
	static const int CADI_TRESPASS_PROTECTION_REJ			= 0x60;
	static const int CADI_DUPLICATE_ID_REJ				= 0x61;
	static const int CADI_FEATURE_NOT_SUPPORTED_REJ		= 0x62;
	static const int CADI_IND_ATTRIBS_TEMP_NOT_AVAIL_REJ	= 0x63;
	static const int CADI_GRP_ATTRIBS_TEMP_NOT_AVAIL_REJ	= 0x64;
	static const int CADI_SYSTEM_DENIED_IND_ACCESS_REJ	= 0x65;
	static const int CADI_SYSTEM_DENIED_GRP_ACCESS_REJ	= 0x66;
	static const int CADI_RQSTR_AFF_TG_NOT_MEM_OF_MG_REJ	= 0x67;
	static const int CADI_NO_CAPABLE_SITES_REJ			= 0x68;
	static const int CADI_NO_CAPABLE_DIU_REJ				= 0x69;
	static const int CADI_NO_CAPABLE_CIU_REJ				= 0x6A;
	static const int CADI_NO_CAPABLE_MBX_REJ				= 0x6B;
	static const int CADI_NO_CAPABLE_IZAC_REJ				= 0x6C;
	static const int CADI_TARGET_ZONE_UNREACHABLE_REJ		= 0x6D;
	static const int CADI_TARGET_IHLR_UNREACHABLE_REJ		= 0x6E;
	static const int CADI_RQSTR_HLR_UNREACHABLE_TO_TARGET_REJ		= 0x6F;
	static const int CADI_TARGET_NOT_AT_TARGET_ZONE_REJ			= 0x70;
	static const int CADI_TARGET_ATTRIBS_TEMP_NOT_AVAIL_REJ		= 0x71;
	static const int CADI_NON_EMRG_RQST_DENIED_DUE_TO_EMRG_REJ	= 0x72;
	static const int CADI_EXCEEDED_MAX_NUM_CNSL_OPS_REJ			= 0x73;
	static const int CADI_BUSY_SERVICE_TERMINATED_REJ				= 0x74;
	static const int CADI_DEVICE_TYPES_INCOMPAT_FOR_SERVICE_REJ	= 0x75;
	static const int CADI_INCOMPAT_PC_SECURE_MOD_TYPE_REJ			= 0x76;
	static const int CADI_UNCONFIRMED_TG_DENIED_DUE_TO_MG_REJ		= 0x77;
	static const int CADI_UNABLE_TO_BSY_RQSTR_ON_ACT_CALL			= 0x78;
	static const int CADI_LAND_TO_MOBILE_LANDLINE_REJ				= 0x79;
	// NEW SUBSCRIBER REJECTS for OZCAR5
	static const int CADI_STEALTH_REJECT					= 0x90;
	static const int CADI_RQSTR_NOT_REG					= 0x91;
	static const int CADI_RQSTR_NOT_AFF_TO_TG				= 0x92;
	static const int CADI_TG_RQST_ON_REGROUPED_TG			= 0x93;
	static const int CADI_IND_TERMINATED_CALL_REJ			= 0x94;
	static const int CADI_INVALID_SERVICE_STATE_REJ		= 0x95;
	static const int CADI_TARGET_IN_ANOTHER_CALL_REJ		= 0x96;
	static const int CADI_PTT_TEMP_NOT_ALLOWED_REJ		= 0x97;
	static const int CADI_CALL_TIMEOUT_REJ				= 0x98;
	static const int CADI_SEC_RQ_ON_CLR_ONLY_GRP_REC		= 0x99;
	static const int CADI_CLR_RQ_ON_SEC_ONLY_GRP_REC		= 0x9A;
	static const int CADI_PC_TARGET_REC_CLR_ONLY			= 0x9B;
	static const int CADI_PC_TARGET_REC_SEC_ONLY			= 0x9C;
	static const int CADI_SEC_REQ_BY_IND_CLR_ONLY			= 0x9D;
	static const int CADI_CLR_REQ_BY_IND_SEC_ONLY			= 0x9E;
	static const int CADI_KEY_NOT_AVAIL_AT_CON			= 0x9F;
	static const int CADI_INFRASTRUCTURE_ERROR_REJ		= 0xA0;
	static const int CADI_GRP_CALL_SETUP_ABORTED_REJ		= 0xA1;
	static const int CADI_STEALTH_TG_DENIED_DUE_TO_MG_REJ	= 0xA2;
	static const int CADI_ZONE_REMOVED_REJ				= 0xA3;
	static const int CADI_IND_RECORD_DELETED_REJ			= 0xA4;
	static const int CADI_TARGET_REFUSED_CALL_REJ			= 0xA5;
	static const int CADI_IND_IN_ANOTHER_CALL_REJ			= 0xA6;
	static const int CADI_RSRC_PREEMPTION_REJ				= 0xA7;
	static const int CADI_RQSTR_CANT_SRC_AT_SITE_REJ		= 0xA8;
	static const int CADI_IND_NOT_AT_ZONE_REJ				= 0xA9;
	// Radio Rejects
	static const int CADI_RADIO_REJ						= 0x150;





	/************************************************************************/
	/* cadi.x typedefs                                                      */
	/************************************************************************/
	typedef unsigned long	CADI_GROUP_ID;
	typedef unsigned long	CADI_RADIO_ID;
	typedef char			CADI_RADIO_ALIAS[CADI_MAX_ALIAS_LEN];
	typedef char			CADI_GROUP_ALIAS[CADI_MAX_ALIAS_LEN];

	
	/************************************************************************/
	/* cadev.x typedefs                                                     */
	/************************************************************************/
	typedef unsigned short CADI_RADIO_ACK_TYPE;
	typedef unsigned short CADI_RADIO_REJECT_REASON;
	typedef unsigned char  CADI_ZONE_ID;
	typedef unsigned long  CADI_ZC_SEQ_NUM;
	typedef unsigned long	CADI_STATUS_NUMBER;
	typedef unsigned short CADI_CMD_STATUS_TYPE;
	typedef unsigned short CADI_CMD_REJECT_CODE;
	typedef char	CADI_ZONE_ALIAS[CADI_MAX_ALIAS_LEN];
	typedef unsigned short CADI_ZONE_STATUS;
	typedef unsigned short CADI_IZ_TRUNKING_STATUS;
	typedef unsigned long  CADI_RADIO_EVENT_QUALIFIER;


	enum CADI_RADIO_ACK_TYPE_Qualifier
	{
		CADI_ACK_UNKNOWN		= 0,
		CADI_ACK_REGROUP_TG	= 1,
		CADI_ACK_REGROUP_MG	= 2,
		CADI_ACK_INHIBIT		= 3,
		CADI_ACK_XINHIBIT		= 4,
		CADI_ACK_LOCK			= 5,
		CADI_ACK_UNLOCK		= 6,
		CADI_ACK_XREGROUP_TG	= 7,
		CADI_ACK_XREGROUP_MG	= 8,
		CADI_ACK_RADIO_CHECK	= 9,
		CADI_ACK_FAILSOFT		= 10,
		CADI_ACK_XFAILSOFT	= 11
	};
	
	enum CADI_RADIO_EVENT_QUALIFIER_Qualifier
	{
		CADI_EMERGENCY		= 0x80000000,
		CADI_MULTIGROUP		= 0x40000000,
		CADI_GROUP_REGROUP	= 0x20000000,
		CADI_MSEL				= 0x10000000,
		CADI_INTERCONNECT		= 0x08000000,
		CADI_CODED			= 0x04000000,
		CADI_LANDLINE			= 0x02000000,
		CADI_AUDIO_PATCH		= 0x01000000,
		CADI_RING				= 0x00800000,
		CADI_TRANSPOND		= 0x00400000,
		CADI_SITEWIDE			= 0x00200000,
		CADI_ASTRO_CALL		= 0x00100000,
		CADI_SITE_ONLY_CALL	= 0x00080000,
		CADI_REPEAT_DISABLED	= 0x00040000,
		CADI_AFFILIATED		= 0x00020000,
		CADI_GO_CALL			= 0x00010000,
		CADI_DATA				= 0x00008000
	};



	enum CADI_SESSION_STATUS_Qualifier
	{
		CADI_SESSION_UP			= 0,
		CADI_NO_SESSION			= 1,
		CADI_NO_LICENSE			= 2,
		CADI_SESSION_DOWN			= 3,
		// OmniLink Addition
		CADI_SESSION_MAXEDOUT		= 4,
		CADI_ZM_DB_DOWN			= 5,
		CADI_ZM_DB_UP				= 6,
		CADI_ZC_LINK_UP			= 7,
		CADI_ZC_LINK_DOWN			= 8,
		// Additional Session MSG
		CADI_SESSION_FORCED_OUT	= 9
	};



/* Redefinition of struct CADI_TIMESTAMP */
typedef struct CADI_TIMESTAMP
{
	unsigned short year; // 4 digit
	unsigned char month; // 1-12
	unsigned char day;   // 1-31
	unsigned char hour;  // 0-23
	unsigned char minute; // 0-59
	unsigned char second; // 0-59
	unsigned char decisecond; // 0-10
} CADI_TIMESTAMP;

typedef struct CADI_ZC_SEQ 
{
    unsigned long ucn;
    unsigned long csn;
} CADI_ZC_SEQ;

typedef struct CADI_ZONE_INFO 
{
    unsigned char cz;
    unsigned char pz;
    unsigned char rz;
} CADI_ZONE_INFO;

typedef unsigned long CADI_MESSAGE_NUMBER;

/* structures used by events */
typedef struct CADI_RADIO_EVENT_INFO {
	CADI_RADIO_ID radio_id;
	CADI_RADIO_ALIAS radio_alias;
	CADI_GROUP_ID group_id;
	CADI_GROUP_ALIAS group_alias;
	CADI_TIMESTAMP timestamp;
} CADI_RADIO_EVENT_INFO;



typedef struct CADI_CALL_ALERT_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_RADIO_ID target_id;
    CADI_RADIO_ALIAS target_alias;
    CADI_ZONE_INFO zone_info;
} CADI_CALL_ALERT_DATA;


typedef struct CADI_CHANGE_ME_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_ZONE_INFO zone_info;
} CADI_CHANGE_ME_DATA;

typedef struct CADI_CMD_STATUS_DATA {
    CADI_CMD_STATUS_TYPE cmd_status_type;
    CADI_CMD_NO cmd_number;
    CADI_CMD_REJECT_CODE cmd_reject_code;
    CADI_TIMESTAMP timestamp;
} CADI_CMD_STATUS_DATA;



typedef struct CADI_EA_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_ZC_SEQ_NUM zc_ucn;
    CADI_ZONE_INFO zone_info;
} CADI_EA_DATA;



typedef struct CADI_GROUP_CALL_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_RADIO_EVENT_QUALIFIER radio_ev_qualifier;
    CADI_ZC_SEQ zc_seq_numbers;
    CADI_ZONE_INFO zone_info;
} CADI_GROUP_CALL_DATA;




typedef struct CADI_INTERCONNECT_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_RADIO_EVENT_QUALIFIER radio_ev_qualifier;
    CADI_ZC_SEQ zc_seq_numbers;
    CADI_ZONE_INFO zone_info;
} CADI_INTERCONNECT_DATA;



typedef struct CADI_MESSAGE_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_MESSAGE_NUMBER message_num;
    CADI_ZONE_INFO zone_info;
} CADI_MESSAGE_DATA;


typedef struct CADI_PRIVATE_CALL_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_RADIO_ID target_id;
    CADI_RADIO_ALIAS target_alias;
    CADI_ZC_SEQ zc_seq_numbers;
    CADI_ZONE_INFO zone_info;
} CADI_PRIVATE_CALL_DATA;


typedef struct CADI_STATUS_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_STATUS_NUMBER status_num;
    CADI_ZONE_INFO zone_info;
} CADI_STATUS_DATA;



typedef unsigned short CADI_SESSION_STATUS;

typedef struct CADI_ZC_STATUS_DATA {
    CADI_ZONE_ALIAS zone_alias;
    CADI_ZONE_STATUS zone_status;
    CADI_IZ_TRUNKING_STATUS iz_trunking_capable;
    CADI_TIMESTAMP timestamp;
} CADI_ZC_STATUS_DATA;


typedef struct CADI_RADIO_ACK_DATA {
    CADI_RADIO_EVENT_INFO radio_event;
    CADI_RADIO_ACK_TYPE ack_type;
    CADI_RADIO_REJECT_REASON reject_reason;
    CADI_ZONE_INFO zone_info;
} CADI_RADIO_ACK_DATA;

#if defined(__cplusplus)
}
#endif



#endif

