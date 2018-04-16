#ifndef H__CADEV
#define H__CADEV
#include "cadi.h"

/*
**-----------------------------------------------------------------------------
**      Common Definitions
**-----------------------------------------------------------------------------
*/

#define CADEV__INVALID_SUBSCRIBER_ID 0
#define CADEV__INVALID_MESSAGE_NUMBER 0
#define CADEV__INVALID_STATUS_NUMBER 0
#define CADEV__INVALID_ZC_ID 0

/*
**-----------------------------------------------------------------------------
**      Customize common fields for Events
**-----------------------------------------------------------------------------
*/


typedef u_long CADEV__MESSAGE_NUMBER;
bool_t xdr_CADEV__MESSAGE_NUMBER();


typedef u_long CADEV__STATUS_NUMBER;
bool_t xdr_CADEV__STATUS_NUMBER();


typedef u_long CADEV__RADIO_EVENT_QUALIFIER;
bool_t xdr_CADEV__RADIO_EVENT_QUALIFIER();


typedef u_short CADEV__RADIO_ACK_TYPE;
bool_t xdr_CADEV__RADIO_ACK_TYPE();


typedef u_short CADEV__CMD_STATUS_TYPE;
bool_t xdr_CADEV__CMD_STATUS_TYPE();


typedef u_short CADEV__CMD_REJECT_CODE;
bool_t xdr_CADEV__CMD_REJECT_CODE();


typedef u_short CADEV__SESSION_STATUS;
bool_t xdr_CADEV__SESSION_STATUS();


typedef char CADEV__ZONE_ALIAS[CADI__MAX_ALIAS_LEN];
bool_t xdr_CADEV__ZONE_ALIAS();


typedef u_short CADEV__ZONE_STATUS;
bool_t xdr_CADEV__ZONE_STATUS();


typedef u_short CADEV__IZ_TRUNKING_STATUS;
bool_t xdr_CADEV__IZ_TRUNKING_STATUS();


typedef u_long CADEV__ZC_SEQ_NUM;
bool_t xdr_CADEV__ZC_SEQ_NUM();


typedef u_char CADEV__ZONE_ID;
bool_t xdr_CADEV__ZONE_ID();


typedef u_short CADEV__RADIO_REJECT_REASON;
bool_t xdr_CADEV__RADIO_REJECT_REASON();


/*
**-----------------------------------------------------------------------------
**     Time structure
**-----------------------------------------------------------------------------
*/


struct CADEV__TIMESTAMP {
	u_short year;
	u_char month;
	u_char day;
	u_char hour;
	u_char minute;
	u_char second;
	u_char decisecond;
};
typedef struct CADEV__TIMESTAMP CADEV__TIMESTAMP;
bool_t xdr_CADEV__TIMESTAMP();


/*
**-----------------------------------------------------------------------------
**     Common structure for the Events
**-----------------------------------------------------------------------------
*/


struct CADEV__RADIO_EVENT_INFO {
	CADI__RADIO_ID radio_id;
	CADI__RADIO_ALIAS radio_alias;
	CADI__GROUP_ID group_id;
	CADI__GROUP_ALIAS group_alias;
	CADEV__TIMESTAMP timestamp;
};
typedef struct CADEV__RADIO_EVENT_INFO CADEV__RADIO_EVENT_INFO;
bool_t xdr_CADEV__RADIO_EVENT_INFO();


struct CADEV__ZONE_INFO {
	CADEV__ZONE_ID cz;
	CADEV__ZONE_ID pz;
	CADEV__ZONE_ID rz;
};
typedef struct CADEV__ZONE_INFO CADEV__ZONE_INFO;
bool_t xdr_CADEV__ZONE_INFO();


struct CADEV__ZC_SEQ_NUMBERS {
	CADEV__ZC_SEQ_NUM ucn;
	CADEV__ZC_SEQ_NUM csn;
};
typedef struct CADEV__ZC_SEQ_NUMBERS CADEV__ZC_SEQ_NUMBERS;
bool_t xdr_CADEV__ZC_SEQ_NUMBERS();


/*
**-----------------------------------------------------------------------------
**  Declare data structure for capturing events.  These structures are  ***
**  published in the API.                                               ***
**-----------------------------------------------------------------------------
**  The following events use the same data structures for
**  their arguments, CADEV__RADIO_EVENT_INFO
**      cadev__affiliation
**      cadev__deaffiliation
**-----------------------------------------------------------------------------
*/


/*
**-----------------------------------------------------------------------------
** cadev__change_me()
**-----------------------------------------------------------------------------
*/


struct CADEV__CHANGE_ME_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__CHANGE_ME_DATA CADEV__CHANGE_ME_DATA;
bool_t xdr_CADEV__CHANGE_ME_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__status()
**-----------------------------------------------------------------------------
*/


struct CADEV__STATUS_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__STATUS_NUMBER status_num;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__STATUS_DATA CADEV__STATUS_DATA;
bool_t xdr_CADEV__STATUS_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__message()
**-----------------------------------------------------------------------------
*/


struct CADEV__MESSAGE_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__MESSAGE_NUMBER message_num;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__MESSAGE_DATA CADEV__MESSAGE_DATA;
bool_t xdr_CADEV__MESSAGE_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__group_call()
**-----------------------------------------------------------------------------
*/


struct CADEV__GROUP_CALL_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__RADIO_EVENT_QUALIFIER radio_ev_qualifier;
	CADEV__ZC_SEQ_NUMBERS zc_seq_numbers;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__GROUP_CALL_DATA CADEV__GROUP_CALL_DATA;
bool_t xdr_CADEV__GROUP_CALL_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__intercon_call()
**-----------------------------------------------------------------------------
*/


struct CADEV__INTERCONNECT_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__RADIO_EVENT_QUALIFIER radio_ev_qualifier;
	CADEV__ZC_SEQ_NUMBERS zc_seq_numbers;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__INTERCONNECT_DATA CADEV__INTERCONNECT_DATA;
bool_t xdr_CADEV__INTERCONNECT_DATA();


/*
**-----------------------------------------------------------------------------
** Radio Event Qualifier (radio_ev_qualifier) Definitions
**-----------------------------------------------------------------------------
*/

#define CADEV__EMERGENCY 0x80000000
#define CADEV__MULTIGROUP 0x40000000
#define CADEV__GROUP_REGROUP 0x20000000
#define CADEV__MSEL 0x10000000
#define CADEV__INTERCONNECT 0x08000000
#define CADEV__CODED 0x04000000
#define CADEV__LANDLINE 0x02000000
#define CADEV__AUDIO_PATCH 0x01000000
#define CADEV__RING 0x00800000
#define CADEV__TRANSPOND 0x00400000
#define CADEV__SITEWIDE 0x00200000
#define CADEV__ASTRO_CALL 0x00100000

/*       OmniLink/APCO Addition        */

#define CADEV__SITE_ONLY_CALL 0x00080000
#define CADEV__REPEAT_DISABLED 0x00040000
#define CADEV__AFFILIATED 0x00020000
#define CADEV__GO_CALL 0x00010000
#define CADEV__DATA 0x00008000

/*
**-----------------------------------------------------------------------------
** cadev__private_call()
**-----------------------------------------------------------------------------
*/


struct CADEV__PRIVATE_CALL_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADI__RADIO_ID target_id;
	CADI__RADIO_ALIAS target_alias;
	CADEV__ZC_SEQ_NUMBERS zc_seq_numbers;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__PRIVATE_CALL_DATA CADEV__PRIVATE_CALL_DATA;
bool_t xdr_CADEV__PRIVATE_CALL_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__call_alert()
**-----------------------------------------------------------------------------
*/


struct CADEV__CALL_ALERT_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADI__RADIO_ID target_id;
	CADI__RADIO_ALIAS target_alias;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__CALL_ALERT_DATA CADEV__CALL_ALERT_DATA;
bool_t xdr_CADEV__CALL_ALERT_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__emergency_alarm()
**-----------------------------------------------------------------------------
*/


struct CADEV__EA_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__ZC_SEQ_NUM zc_ucn;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__EA_DATA CADEV__EA_DATA;
bool_t xdr_CADEV__EA_DATA();


/*
**-----------------------------------------------------------------------------
** cadev__radio_ack()
**-----------------------------------------------------------------------------
*/


struct CADEV__RADIO_ACK_DATA {
	CADEV__RADIO_EVENT_INFO radio_event;
	CADEV__RADIO_ACK_TYPE ack_type;
	CADEV__RADIO_REJECT_REASON reject_reason;
	CADEV__ZONE_INFO zone_info;
};
typedef struct CADEV__RADIO_ACK_DATA CADEV__RADIO_ACK_DATA;
bool_t xdr_CADEV__RADIO_ACK_DATA();


/*
**-----------------------------------------------------------------------------
**      Qualifiers for "ack_type" field 
**-----------------------------------------------------------------------------
*/

#define CADEV__ACK_UNKNOWN 0
#define CADEV__ACK_REGROUP_TG 1
#define CADEV__ACK_REGROUP_MG 2
#define CADEV__ACK_INHIBIT 3
#define CADEV__ACK_XINHIBIT 4
#define CADEV__ACK_LOCK 5
#define CADEV__ACK_UNLOCK 6

/*      Additional Qualifiers for "ack_type" field for APCO system     */

#define CADEV__ACK_XREGROUP_TG 7
#define CADEV__ACK_XREGROUP_MG 8
#define CADEV__ACK_RADIO_CHECK 9

/* Site Lens Specific Qualifiers */

#define CADEV__ACK_FAILSOFT 10
#define CADEV__ACK_XFAILSOFT 11

/*
**-----------------------------------------------------------------------------
** cadev__cmd_status()
**-----------------------------------------------------------------------------
*/


struct CADEV__CMD_STATUS_DATA {
	CADEV__CMD_STATUS_TYPE cmd_status_type;
	CADI__CMD_NO cmd_number;
	CADEV__CMD_REJECT_CODE cmd_reject_code;
	CADEV__TIMESTAMP timestamp;
};
typedef struct CADEV__CMD_STATUS_DATA CADEV__CMD_STATUS_DATA;
bool_t xdr_CADEV__CMD_STATUS_DATA();


/*
**-----------------------------------------------------------------------------
**      Qualifiers for "cmd_status_type" field in CADEV__CMD_STATUS_DATA
**-----------------------------------------------------------------------------
*/

#define CADEV__ZC_NO_CMD_STATUS 0
#define CADEV__ZC_SENT_ACK 1
#define CADEV__ZC_BUSY_ACK 2
#define CADEV__ZC_REJECTED_NACK 3

/*
**-----------------------------------------------------------------------------
**  cadev__session() uses the CADEV__SESSION_STATUS defined above
**-----------------------------------------------------------------------------
*/

#define CADEV__SESSION_UP 0
#define CADEV__NO_SESSION 1
#define CADEV__NO_LICENSE 2
#define CADEV__SESSION_DOWN 3

/*      OmniLink Addition     */

#define CADEV__SESSION_MAXEDOUT 4
#define CADEV__ZM_DB_DOWN 5
#define CADEV__ZM_DB_UP 6
#define CADEV__ZC_LINK_UP 7
#define CADEV__ZC_LINK_DOWN 8

/*      Additional Session MSG  */

#define CADEV__SESSION_FORCED_OUT 9

/*
**-----------------------------------------------------------------------------
**  cadev__zc_status() uses the CADEV__ZC_STATUS_DATA
**  The qualifiers are defined below
**-----------------------------------------------------------------------------
*/


struct CADEV__ZC_STATUS_DATA {
	CADEV__ZONE_ALIAS zone_alias;
	CADEV__ZONE_STATUS zone_status;
	CADEV__IZ_TRUNKING_STATUS iz_trunking_capable;
	CADEV__TIMESTAMP timestamp;
};
typedef struct CADEV__ZC_STATUS_DATA CADEV__ZC_STATUS_DATA;
bool_t xdr_CADEV__ZC_STATUS_DATA();


/*      Qualifiers for the CADEV__ZC_STATUS_DATA        */

#define CADEV__ZC_STATUS_UNKNOWN 0
#define CADEV__ZC_STATUS_ACTIVE 1
#define CADEV__ZC_STATUS_DISABLE 2
#define CADEV__ZC_STATUS_IDLE 3

/*      Qualifiers for the CADI__IZ_TRUNKING_STATUS    */

#define CADEV__IZT_CAPABLE 1
#define CADEV__IZT_INCAPABLE 0

/*
**-----------------------------------------------------------------------------
**      Qualifiers for reject_code field (these are from the ZC)
**-----------------------------------------------------------------------------
*/

#define CADEV__NO_REJECT_TYPE 0xFF
#define CADEV__INVALID_REQ_ID_REJ 0x00
#define CADEV__INVALID_TARGET_ID_REJ 0x01
#define CADEV__DISABLED_REQ_ID_REJ 0x02
#define CADEV__DISABLED_TARGET_ID_REJ 0x03
#define CADEV__INVALID_TG_REJ 0x04
#define CADEV__DISABLED_TG_REJ 0x05
#define CADEV__TARGET_TG_INVALID_REJ 0x06
#define CADEV__TARGET_TG_DISABLED_REJ 0x07
#define CADEV__SITEWIDE_INCAPABLE_REJ 0x10
#define CADEV__MG_INCAPABLE_REJ 0x11
#define CADEV__DVP_INCAPABLE_REJ 0x12
#define CADEV__TG_INCAPABLE_REJ 0x13
#define CADEV__PC_INCAPABLE_REQ_REJ 0x14
#define CADEV__PC_INCAPABLE_TARGET_REJ 0x15
#define CADEV__INT_INCAPABLE_REJ 0x16
#define CADEV__DVP_UPGRADE_REJ 0x17
#define CADEV__DVP_DNGRADE_REJ 0x18
#define CADEV__DVP_CODED_ONLY_TARGET_REJ 0x19
#define CADEV__NO_CAPABLE_RESOURCES_REJ 0x20
#define CADEV__ASTRO_INCAPABLE_TARGET_REJ 0x25
#define CADEV__TARGET_NOT_RESPONDED_REJ 0x26
#define CADEV__ID_INVOLVED_IN_DI_REJ 0x27
#define CADEV__TG_DENIED_DUE_TO_MG_REJ 0x30
#define CADEV__INVALID_PATCH_OF_A_MG 0x31
#define CADEV__INVALID_NORM_TAC_OF_A_MG 0x32
#define CADEV__INVALID_REPEAT_OF_A_MG 0x33
#define CADEV__INVALID_CTRL_CNSL_OP_INFO_REJ 0x34
#define CADEV__NO_AFFILIATED_MEMBERS_REJ 0x35
#define CADEV__INVALID_MSEL_GROUP_NUMBER 0x36
#define CADEV__INVALID_MG_IN_MSEL_REQUEST 0x37
#define CADEV__RQSTR_RECORD_NOT_AVAIL_REJ 0x40
#define CADEV__TARGET_RECORD_NOT_AVAIL_REJ 0x41
#define CADEV__TG_REC_NOT_AVAIL_REJ 0x42
#define CADEV__RQSTR_PROHIBITED_ATIA_SITE_REJ 0x43
#define CADEV__TARGET_PROHIBITED_ATIA_SITE_REJ 0x44
#define CADEV__TG_PROHIBITED_ATIA_SITE_REJ 0x45
#define CADEV__INT_CALL_LIMIT_EXCEEDED_REJ 0x46
#define CADEV__AUDIO_INTERRUPT_REJ 0x47
#define CADEV__PC_TARGET_DEAFFILIATED_REJ 0x48
#define CADEV__PC_TARGET_DEREGISTERED_REJ 0x48
#define CADEV__NO_BUSY_TO_BUSY_ORIDE_REJ 0x49
#define CADEV__SITE_ACCESS_DENIAL 0x4A
#define CADEV__MG_EMRG_INCAPABLE_REJ 0x4B
#define CADEV__EMRG_NOT_ALLOWED_ON_GRP_REJ 0x4C
#define CADEV__CA_INCAPABLE_REQ_REJ 0x4D
#define CADEV__CA_INCAPABLE_TARGET_REJ 0x4E
#define CADEV__CA_TARGET_DEAFFILIATED_REJ 0x4F
#define CADEV__CA_TARGET_DEREGISTERED_REJ 0x4F
#define CADEV__NO_SITEWD_SITES_REJ 0x50
#define CADEV__SITEWD_OVERLAP_REJ 0x51
#define CADEV__TARGET_NOT_AFFILIATED 0x52
#define CADEV__TARGET_NOT_REGISTERED_REJ 0x52
#define CADEV__CMD_NOT_ALLOWED 0x53
#define CADEV__UNABLE_TO_GET_PEND_BUFFER_REJ 0x54

/*      OmniLink Addition     */

#define CADEV__TG_REGRP_INCAPABLE_REJ 0x55
#define CADEV__INCOMPAT_TG_IND_MOD_TYPE_REJ 0x56
#define CADEV__INCOMPAT_TG_IND_FRQ_BAND_REJ 0x57

/*      OmniLink Modification: used to be 0x2f     */

#define CADEV__B_O_NOT_ALLOWED 0x58

/*      OmniLink Addition     */

#define CADEV__TG_IS_SITE_ONLY_REJ 0x59
#define CADEV__DATA_PC_TARG_IN_GRP_ACT_REJ 0x5A
#define CADEV__DATA_PC_TARG_IN_INT_ACT_REJ 0x5B
#define CADEV__GO_DENIED_DUE_TO_GO_REJ 0x5C
#define CADEV__GO_DENIED_DUE_TO_LAND_REJ 0x5D
#define CADEV__INVALID_REQ_AUD_TME_SLT_REJ 0x5E

#define CADEV__TRESPASS_PROTECTION_REJ 0x60
#define CADEV__DUPLICATE_ID_REJ 0x61
#define CADEV__FEATURE_NOT_SUPPORTED_REJ 0x62
#define CADEV__IND_ATTRIBS_TEMP_NOT_AVAIL_REJ 0x63
#define CADEV__GRP_ATTRIBS_TEMP_NOT_AVAIL_REJ 0x64
#define CADEV__SYSTEM_DENIED_IND_ACCESS_REJ 0x65
#define CADEV__SYSTEM_DENIED_GRP_ACCESS_REJ 0x66
#define CADEV__RQSTR_AFF_TG_NOT_MEM_OF_MG_REJ 0x67
#define CADEV__NO_CAPABLE_SITES_REJ 0x68
#define CADEV__NO_CAPABLE_DIU_REJ 0x69
#define CADEV__NO_CAPABLE_CIU_REJ 0x6A
#define CADEV__NO_CAPABLE_MBX_REJ 0x6B
#define CADEV__NO_CAPABLE_IZAC_REJ 0x6C
#define CADEV__TARGET_ZONE_UNREACHABLE_REJ 0x6D
#define CADEV__TARGET_IHLR_UNREACHABLE_REJ 0x6E
#define CADEV__RQSTR_HLR_UNREACHABLE_TO_TARGET_REJ 0x6F
#define CADEV__TARGET_NOT_AT_TARGET_ZONE_REJ 0x70
#define CADEV__TARGET_ATTRIBS_TEMP_NOT_AVAIL_REJ 0x71
#define CADEV__NON_EMRG_RQST_DENIED_DUE_TO_EMRG_REJ 0x72
#define CADEV__EXCEEDED_MAX_NUM_CNSL_OPS_REJ 0x73
#define CADEV__BUSY_SERVICE_TERMINATED_REJ 0x74
#define CADEV__DEVICE_TYPES_INCOMPAT_FOR_SERVICE_REJ 0x75
#define CADEV__INCOMPAT_PC_SECURE_MOD_TYPE_REJ 0x76
#define CADEV__UNCONFIRMED_TG_DENIED_DUE_TO_MG_REJ 0x77
#define CADEV__UNABLE_TO_BSY_RQSTR_ON_ACT_CALL 0x78
#define CADEV__LAND_TO_MOBILE_LANDLINE_REJ 0x79

/*  NEW SUBSCRIBER REJECTS for OZCAR5  */

#define CADEV__STEALTH_REJECT 0x90
#define CADEV__RQSTR_NOT_REG 0x91
#define CADEV__RQSTR_NOT_AFF_TO_TG 0x92
#define CADEV__TG_RQST_ON_REGROUPED_TG 0x93
#define CADEV__IND_TERMINATED_CALL_REJ 0x94
#define CADEV__INVALID_SERVICE_STATE_REJ 0x95
#define CADEV__TARGET_IN_ANOTHER_CALL_REJ 0x96
#define CADEV__PTT_TEMP_NOT_ALLOWED_REJ 0x97
#define CADEV__CALL_TIMEOUT_REJ 0x98
#define CADEV__SEC_RQ_ON_CLR_ONLY_GRP_REC 0x99
#define CADEV__CLR_RQ_ON_SEC_ONLY_GRP_REC 0x9A
#define CADEV__PC_TARGET_REC_CLR_ONLY 0x9B
#define CADEV__PC_TARGET_REC_SEC_ONLY 0x9C
#define CADEV__SEC_REQ_BY_IND_CLR_ONLY 0x9D
#define CADEV__CLR_REQ_BY_IND_SEC_ONLY 0x9E
#define CADEV__KEY_NOT_AVAIL_AT_CON 0x9F
#define CADEV__INFRASTRUCTURE_ERROR_REJ 0xA0
#define CADEV__GRP_CALL_SETUP_ABORTED_REJ 0xA1
#define CADEV__STEALTH_TG_DENIED_DUE_TO_MG_REJ 0xA2
#define CADEV__ZONE_REMOVED_REJ 0xA3
#define CADEV__IND_RECORD_DELETED_REJ 0xA4
#define CADEV__TARGET_REFUSED_CALL_REJ 0xA5
#define CADEV__IND_IN_ANOTHER_CALL_REJ 0xA6
#define CADEV__RSRC_PREEMPTION_REJ 0xA7
#define CADEV__RQSTR_CANT_SRC_AT_SITE_REJ 0xA8
#define CADEV__IND_NOT_AT_ZONE_REJ 0xA9

/* Radio Rejects */

#define CADEV__RADIO_REJ 0x150
/*
**-----------------------------------------------------------------------------
** Application Programming Interface for CADI Events
**-----------------------------------------------------------------------------
*/


#define CADEV__PROG ((u_long)0x20001001)
#define CADEV__VERS ((u_long)1)
#define CADEV__AFFILIATION ((u_long)1)
extern void *cadev__affiliation_1();
#define CADEV__CALL_ALERT ((u_long)2)
extern void *cadev__call_alert_1();
#define CADEV__CHANGE_ME ((u_long)3)
extern void *cadev__change_me_1();
#define CADEV__CMD_STATUS ((u_long)4)
extern void *cadev__cmd_status_1();
#define CADEV__DEAFFILIATION ((u_long)5)
extern void *cadev__deaffiliation_1();
#define CADEV__EMERGENCY_ALARM ((u_long)6)
extern void *cadev__emergency_alarm_1();
#define CADEV__GROUP_CALL ((u_long)7)
extern void *cadev__group_call_1();
#define CADEV__INTERCON_CALL ((u_long)8)
extern void *cadev__intercon_call_1();
#define CADEV__MESSAGE ((u_long)9)
extern void *cadev__message_1();
#define CADEV__PRIVATE_CALL ((u_long)10)
extern void *cadev__private_call_1();
#define CADEV__STATUS ((u_long)11)
extern void *cadev__status_1();
#define CADEV__SESSION ((u_long)12)
extern void *cadev__session_1();
#define CADEV__ZC_STATUS ((u_long)13)
extern void *cadev__zc_status_1();
#define CADEV__RADIO_ACK ((u_long)14)
extern void *cadev__radio_ack_1();

#endif /* H__CADEV */
