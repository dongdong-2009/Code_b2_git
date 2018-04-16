/*
+------------------------------------------------------------------------------+
|                                                                              |
|    R E M O T E   P R O C E D U R E   C A L L   S P E C I F I C A T I O N     |
|                                                                              |
|                      COPYRIGHT 1994 - 2001 MOTOROLA INC.                     |
|                                                                              |
+------------------------------------------------------------------------------+

    PRODUCT NAME:  SmartZone/MultiZone CADI
    HEADER  NAME:  cadev.x
    CODED     BY:  04/15/94   John Kastein

------------------------------- RPC DESCRIPTION --------------------------------

RPC definitions and decalrations for event functions.
RPC procedure ranges for the event processing will have the program range
of 1 to 99.

------------------------------- REVISION HISTORY -------------------------------

<date>    <name>       <problem number> <description>
04/15/94  John Kastein                  Initial version.
09/22/94  Steppen Yi                    General Release Changes
10/04/94  Tam Nguyen                    Modify to include the common header
                                        file, cadi.h
11/22/94  Tam Nguyen                    Revamp according to the API
04/06/95  Tam Nguyen                      Add ZC's ACK defines:
                                            CADEV__ZC_NO_CMD_STATUS
                                            CADEV__ZC_BUSY_ACK 
                                            CADEV__ZC_SENT_ACK 
                                            CADEV__ZC_REJECTED_ACK
                                        and add field "cmd_status_type" to 
                                        structure CADEV__CMD_STATUS_DATA.
05/07/95  Tam Nguyen                    Add CADEV__ZM_LINK_OK and 
                                        CADEV__ZM_LINK_DOWN
04/24/97  Steppen Yi                    OL changes
05 Mar 98 Rob Ekl                       5.0 (OZCAR5) changes
01/25/2000 Daohui Li                    add FS constants for gemini project.
02/18/2000 Daohui Li                    add CADEV__INFRASTRUCTURE_REJ.
21 Mar 00 Daohui Li                     Introduce CADEV__SESSION_FORCED_OUT
                                        constant for bumping out an existing
                                        CADI User case.
07/10/00  Daohui Li                     Introduce CADEV__ZC_INFO structure 
                                        for UCN and CSN; 
                                        Modified the data structure that
                                        contain CADEV__RADIO_EVENT_INFO so that
                                        the structure will have CADEV__ZC_INFO
                                        field.
09/05/00  Daohui Li        Introduce CADEV__ZONE_INFO;
                           change the name of CADEV__ZC_INFO to
                           CADEV__ZC_SEQ_NUMBERS, and add CADEV__ZONE_INFO 
                           in the structure;
                           introduce CADEV__RADIO_RESPONSE_REASON;
                           except for (de-)affiliation, modify the input 
                           structure for those that contains 
                           CADEV__RADIO_EVENT_INFO:
                           CADEV__XXX_CALL  (XXX=GROUP, INTERCON, PRIVATE):
                                             append CADEV__ZC_SEQ_NUMBERS 
                                             and    CADEV__ZONE_INFO;
                           CADEV__EMERGENCY_ALARM:
                                             append CADEV__ZC_SEQ_NUM (ucn) and
                                                    CADEV__ZONE_INFO;
                           CADEV__CHANGE_ME, CADEV__MESSAGE/STATUS:
                                             append CADEV__ZONE_INFO
                           CADEV__RADIO_ACK: append CADEV__RADIO_REJECT_REASON
                                             and    CADEV__ZONE_INFO.
                           remove CADEV__INFRASTRUCTURE_REJ, since Gemini is
                           no longer use this constant.
10/26/00  Daohui Li        zone_info in CADEV__CHANGE_ME_DATA has only RZ info.
05/19/2000 Daohui Li                    Introduce CADEV__SESSION_FORCED_OUT
                                        constant for bumping out an existing
                                        CADI User case (based on ser_szol_3Q00
                                        modification).
19 Feb 01 Jesper Esbensen 		Merged from Dimetra 3.5. Changed 
                                        CADEV__RADIO_ACK to be able
					to pass radio reject reason back.

------------------------------------- CODE -----------------------------------*/
#ifdef RPC_HDR
%#ifndef        H__CADEV
%#define        H__CADEV
%#include "cadi.h"
#endif

%
%/*
%**-----------------------------------------------------------------------------
%**      Common Definitions
%**-----------------------------------------------------------------------------
%*/
%
const   CADEV__INVALID_SUBSCRIBER_ID  = 0;
const   CADEV__INVALID_MESSAGE_NUMBER = 0;
const   CADEV__INVALID_STATUS_NUMBER  = 0;
const   CADEV__INVALID_ZC_ID          = 0;
%
%/*
%**-----------------------------------------------------------------------------
%**      Customize common fields for Events
%**-----------------------------------------------------------------------------
%*/
%
typedef unsigned long                CADEV__MESSAGE_NUMBER;
typedef unsigned long                CADEV__STATUS_NUMBER;
typedef unsigned long                CADEV__RADIO_EVENT_QUALIFIER;
typedef unsigned short               CADEV__RADIO_ACK_TYPE;
typedef unsigned short               CADEV__CMD_STATUS_TYPE;
typedef unsigned short               CADEV__CMD_REJECT_CODE;
typedef unsigned short               CADEV__SESSION_STATUS;
typedef char                         CADEV__ZONE_ALIAS[CADI__MAX_ALIAS_LEN];
typedef unsigned short               CADEV__ZONE_STATUS;
typedef unsigned short               CADEV__IZ_TRUNKING_STATUS;
typedef unsigned long                CADEV__ZC_SEQ_NUM;
typedef unsigned char                CADEV__ZONE_ID;
typedef unsigned short               CADEV__RADIO_REJECT_REASON;
%
%/*
%**-----------------------------------------------------------------------------
%**     Time structure
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__TIMESTAMP
{
    unsigned short                   year;          /* 00-99 e.g. 94 = 1994 */
    unsigned char                    month;         /* 1->12 */
    unsigned char                    day;           /* 1->31 */
    unsigned char                    hour;          /* 0->23 */
    unsigned char                    minute;        /* 0->59 */
    unsigned char                    second;        /* 0->59 */
    unsigned char                    decisecond;    /* 0->10 */
};
%
%/*
%**-----------------------------------------------------------------------------
%**     Common structure for the Events
%**-----------------------------------------------------------------------------
%*/
%
/******************************************************************************
        Basic structure for most events.
This represents the radio information of the event originator.

Descriptions of fields in this structure:

radio_id        The radio id of the event originator. This id number is 
                assigned to a radio by the System Administrator.

radio_alias     The name of the radio user, whose id is the radio_id above. 
                This name is assigned to a radio by the System Administrator.

group_id        The id of a talkgroup that the above radio is currently 
                affiliated with. This id is assigned by the System Administrator. 

group_alias     The name for the above talkgroup, whose id is the group_id 
                above. This name is assigned by the System Administrator.

timestamp       The time marked by Tempest when it receives/generates
                events/commands (see structure CADEV__TIMESTAMP above) 

Note:  The group_id and group_alias fields will be clarified further for each 
       event. See all event APIs for more details.
*******************************************************************************/
struct CADEV__RADIO_EVENT_INFO
{
    CADI__RADIO_ID                  radio_id;
    CADI__RADIO_ALIAS               radio_alias;
    CADI__GROUP_ID                  group_id;
    CADI__GROUP_ALIAS               group_alias;
    CADEV__TIMESTAMP                timestamp;
};
/*******************************************************************************
CADEV__ZONE_INFO contains following information:
    cz     Controlling Zone
    pz     Participating Zone
    rz     Requesting Zone
NOTE: User should ignore those fields that have CADEV__INVALID_ZC_ID (0) values.
*******************************************************************************/
struct CADEV__ZONE_INFO
{
    CADEV__ZONE_ID  cz;
    CADEV__ZONE_ID  pz;
    CADEV__ZONE_ID  rz;
};
/*******************************************************************************
CADEV__ZC_SEQ_NUMBERS structure captures following information:
    ucn        Universal Call Number
    csn        Call Sequence Number
*******************************************************************************/
struct CADEV__ZC_SEQ_NUMBERS
{
    CADEV__ZC_SEQ_NUM    ucn;
    CADEV__ZC_SEQ_NUM    csn;
};
%
%/*
%**-----------------------------------------------------------------------------
%**  Declare data structure for capturing events.  These structures are  ***
%**  published in the API.                                               ***
%**-----------------------------------------------------------------------------
%**  The following events use the same data structures for
%**  their arguments, CADEV__RADIO_EVENT_INFO
%**      cadev__affiliation
%**      cadev__deaffiliation
%**-----------------------------------------------------------------------------
%*/
%
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__change_me()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__CHANGE_ME_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__ZONE_INFO                zone_info;    /* has RZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__status()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__STATUS_DATA 
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__STATUS_NUMBER            status_num;
    CADEV__ZONE_INFO                zone_info;    /* has CZ/RZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__message()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__MESSAGE_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__MESSAGE_NUMBER           message_num;
    CADEV__ZONE_INFO                zone_info;    /* has CZ/RZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__group_call()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__GROUP_CALL_DATA
{
    CADEV__RADIO_EVENT_INFO        radio_event;
    CADEV__RADIO_EVENT_QUALIFIER   radio_ev_qualifier;
    CADEV__ZC_SEQ_NUMBERS          zc_seq_numbers; 
    CADEV__ZONE_INFO               zone_info;          /* has CZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__intercon_call()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__INTERCONNECT_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__RADIO_EVENT_QUALIFIER    radio_ev_qualifier;
    CADEV__ZC_SEQ_NUMBERS           zc_seq_numbers; 
    CADEV__ZONE_INFO                zone_info;          /* has CZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** Radio Event Qualifier (radio_ev_qualifier) Definitions
%**-----------------------------------------------------------------------------
%*/
%
const    CADEV__EMERGENCY           = 0x80000000;
const    CADEV__MULTIGROUP          = 0x40000000;
const    CADEV__GROUP_REGROUP       = 0x20000000;
const    CADEV__MSEL                = 0x10000000;
const    CADEV__INTERCONNECT        = 0x08000000;
const    CADEV__CODED               = 0x04000000;
const    CADEV__LANDLINE            = 0x02000000;
const    CADEV__AUDIO_PATCH         = 0x01000000;
const    CADEV__RING                = 0x00800000;
const    CADEV__TRANSPOND           = 0x00400000;
const    CADEV__SITEWIDE            = 0x00200000;
const    CADEV__ASTRO_CALL          = 0x00100000;
%
%/*       OmniLink/APCO Addition        */
%
const    CADEV__SITE_ONLY_CALL      = 0x00080000;
const    CADEV__REPEAT_DISABLED     = 0x00040000;
const    CADEV__AFFILIATED          = 0x00020000;
const    CADEV__GO_CALL             = 0x00010000;
const    CADEV__DATA                = 0x00008000;
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__private_call()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__PRIVATE_CALL_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADI__RADIO_ID                  target_id;
    CADI__RADIO_ALIAS               target_alias;
    CADEV__ZC_SEQ_NUMBERS           zc_seq_numbers; 
    CADEV__ZONE_INFO                zone_info;          /* has CZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__call_alert()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__CALL_ALERT_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADI__RADIO_ID                  target_id;
    CADI__RADIO_ALIAS               target_alias;
    CADEV__ZONE_INFO                zone_info;      /* has RZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__emergency_alarm()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__EA_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__ZC_SEQ_NUM               zc_ucn;
    CADEV__ZONE_INFO                zone_info;    /* has PZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%** cadev__radio_ack()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__RADIO_ACK_DATA
{
    CADEV__RADIO_EVENT_INFO         radio_event;
    CADEV__RADIO_ACK_TYPE           ack_type;
    CADEV__RADIO_REJECT_REASON      reject_reason;
    CADEV__ZONE_INFO                zone_info;     /* has RZ info */
};
%
%/*
%**-----------------------------------------------------------------------------
%**      Qualifiers for "ack_type" field 
%**-----------------------------------------------------------------------------
%*/
%
const   CADEV__ACK_UNKNOWN          = 0;
const   CADEV__ACK_REGROUP_TG       = 1;
const   CADEV__ACK_REGROUP_MG       = 2;
const   CADEV__ACK_INHIBIT          = 3;
const   CADEV__ACK_XINHIBIT         = 4;
const   CADEV__ACK_LOCK             = 5;
const   CADEV__ACK_UNLOCK           = 6;
%
%/*      Additional Qualifiers for "ack_type" field for APCO system     */
%
const   CADEV__ACK_XREGROUP_TG      = 7;
const   CADEV__ACK_XREGROUP_MG      = 8;
const   CADEV__ACK_RADIO_CHECK      = 9;

%
%/* Site Lens Specific Qualifiers */
%

const   CADEV__ACK_FAILSOFT         = 10;
const   CADEV__ACK_XFAILSOFT        = 11;

%
%/*
%**-----------------------------------------------------------------------------
%** cadev__cmd_status()
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__CMD_STATUS_DATA
{
    CADEV__CMD_STATUS_TYPE          cmd_status_type;
    CADI__CMD_NO                    cmd_number;
    CADEV__CMD_REJECT_CODE          cmd_reject_code;
    CADEV__TIMESTAMP                timestamp;
};
%
%/*
%**-----------------------------------------------------------------------------
%**      Qualifiers for "cmd_status_type" field in CADEV__CMD_STATUS_DATA
%**-----------------------------------------------------------------------------
%*/
%
const   CADEV__ZC_NO_CMD_STATUS     = 0;
const   CADEV__ZC_SENT_ACK          = 1;
const   CADEV__ZC_BUSY_ACK          = 2;
const   CADEV__ZC_REJECTED_NACK     = 3;
%
%/*
%**-----------------------------------------------------------------------------
%**  cadev__session() uses the CADEV__SESSION_STATUS defined above
%**-----------------------------------------------------------------------------
%*/
%
const   CADEV__SESSION_UP           = 0;
const   CADEV__NO_SESSION           = 1;
const   CADEV__NO_LICENSE           = 2;
const   CADEV__SESSION_DOWN         = 3;
%
%/*      OmniLink Addition     */
%
const   CADEV__SESSION_MAXEDOUT     = 4;
const   CADEV__ZM_DB_DOWN           = 5;
const   CADEV__ZM_DB_UP             = 6;
const   CADEV__ZC_LINK_UP           = 7;
const   CADEV__ZC_LINK_DOWN         = 8;
%
%/*      Additional Session MSG  */
%
const   CADEV__SESSION_FORCED_OUT   = 9;

%
%/*
%**-----------------------------------------------------------------------------
%**  cadev__zc_status() uses the CADEV__ZC_STATUS_DATA
%**  The qualifiers are defined below
%**-----------------------------------------------------------------------------
%*/
%
struct CADEV__ZC_STATUS_DATA
{
    CADEV__ZONE_ALIAS               zone_alias;
    CADEV__ZONE_STATUS              zone_status;
    CADEV__IZ_TRUNKING_STATUS       iz_trunking_capable;
    CADEV__TIMESTAMP                timestamp;
};
%
%/*      Qualifiers for the CADEV__ZC_STATUS_DATA        */
%
const   CADEV__ZC_STATUS_UNKNOWN    = 0;
const   CADEV__ZC_STATUS_ACTIVE     = 1;
const   CADEV__ZC_STATUS_DISABLE    = 2;
const   CADEV__ZC_STATUS_IDLE       = 3;
%
%/*      Qualifiers for the CADI__IZ_TRUNKING_STATUS    */
%
const   CADEV__IZT_CAPABLE          = 1;
const   CADEV__IZT_INCAPABLE        = 0;
%
%/*
%**-----------------------------------------------------------------------------
%**      Qualifiers for reject_code field (these are from the ZC)
%**-----------------------------------------------------------------------------
%*/
%
const   CADEV__NO_REJECT_TYPE                   = 0xFF;
const   CADEV__INVALID_REQ_ID_REJ               = 0x00;
const   CADEV__INVALID_TARGET_ID_REJ            = 0x01;
const   CADEV__DISABLED_REQ_ID_REJ              = 0x02;
const   CADEV__DISABLED_TARGET_ID_REJ           = 0x03;
const   CADEV__INVALID_TG_REJ                   = 0x04;
const   CADEV__DISABLED_TG_REJ                  = 0x05;
const   CADEV__TARGET_TG_INVALID_REJ            = 0x06;
const   CADEV__TARGET_TG_DISABLED_REJ           = 0x07;
const   CADEV__SITEWIDE_INCAPABLE_REJ           = 0x10;
const   CADEV__MG_INCAPABLE_REJ                 = 0x11;
const   CADEV__DVP_INCAPABLE_REJ                = 0x12;
const   CADEV__TG_INCAPABLE_REJ                 = 0x13;
const   CADEV__PC_INCAPABLE_REQ_REJ             = 0x14;
const   CADEV__PC_INCAPABLE_TARGET_REJ          = 0x15;
const   CADEV__INT_INCAPABLE_REJ                = 0x16;
const   CADEV__DVP_UPGRADE_REJ                  = 0x17;
const   CADEV__DVP_DNGRADE_REJ                  = 0x18;
const   CADEV__DVP_CODED_ONLY_TARGET_REJ        = 0x19;
const   CADEV__NO_CAPABLE_RESOURCES_REJ         = 0x20;
const   CADEV__ASTRO_INCAPABLE_TARGET_REJ       = 0x25;
const   CADEV__TARGET_NOT_RESPONDED_REJ         = 0x26;
const   CADEV__ID_INVOLVED_IN_DI_REJ            = 0x27;
const   CADEV__TG_DENIED_DUE_TO_MG_REJ          = 0x30;
const   CADEV__INVALID_PATCH_OF_A_MG            = 0x31;
const   CADEV__INVALID_NORM_TAC_OF_A_MG         = 0x32;
const   CADEV__INVALID_REPEAT_OF_A_MG           = 0x33;
const   CADEV__INVALID_CTRL_CNSL_OP_INFO_REJ    = 0x34;
const   CADEV__NO_AFFILIATED_MEMBERS_REJ        = 0x35;
const   CADEV__INVALID_MSEL_GROUP_NUMBER        = 0x36;
const   CADEV__INVALID_MG_IN_MSEL_REQUEST       = 0x37;
const   CADEV__RQSTR_RECORD_NOT_AVAIL_REJ       = 0x40;
const   CADEV__TARGET_RECORD_NOT_AVAIL_REJ      = 0x41;
const   CADEV__TG_REC_NOT_AVAIL_REJ             = 0x42;
const   CADEV__RQSTR_PROHIBITED_ATIA_SITE_REJ   = 0x43;
const   CADEV__TARGET_PROHIBITED_ATIA_SITE_REJ  = 0x44;
const   CADEV__TG_PROHIBITED_ATIA_SITE_REJ      = 0x45;
const   CADEV__INT_CALL_LIMIT_EXCEEDED_REJ      = 0x46;
const   CADEV__AUDIO_INTERRUPT_REJ              = 0x47;
const   CADEV__PC_TARGET_DEAFFILIATED_REJ       = 0x48;
const   CADEV__PC_TARGET_DEREGISTERED_REJ       = 0x48;
const   CADEV__NO_BUSY_TO_BUSY_ORIDE_REJ        = 0x49;
const   CADEV__SITE_ACCESS_DENIAL               = 0x4A;
const   CADEV__MG_EMRG_INCAPABLE_REJ            = 0x4B;
const   CADEV__EMRG_NOT_ALLOWED_ON_GRP_REJ      = 0x4C;
const   CADEV__CA_INCAPABLE_REQ_REJ             = 0x4D;
const   CADEV__CA_INCAPABLE_TARGET_REJ          = 0x4E;
const   CADEV__CA_TARGET_DEAFFILIATED_REJ       = 0x4F;
const   CADEV__CA_TARGET_DEREGISTERED_REJ       = 0x4F;
const   CADEV__NO_SITEWD_SITES_REJ              = 0x50;
const   CADEV__SITEWD_OVERLAP_REJ               = 0x51;
const   CADEV__TARGET_NOT_AFFILIATED            = 0x52;
const   CADEV__TARGET_NOT_REGISTERED_REJ        = 0x52;
const   CADEV__CMD_NOT_ALLOWED                  = 0x53;
const   CADEV__UNABLE_TO_GET_PEND_BUFFER_REJ    = 0x54;
%
%/*      OmniLink Addition     */
%
const   CADEV__TG_REGRP_INCAPABLE_REJ           = 0x55;
const   CADEV__INCOMPAT_TG_IND_MOD_TYPE_REJ     = 0x56;
const   CADEV__INCOMPAT_TG_IND_FRQ_BAND_REJ     = 0x57;
%
%/*      OmniLink Modification: used to be 0x2f     */
%
const   CADEV__B_O_NOT_ALLOWED                  = 0x58;
%
%/*      OmniLink Addition     */
%
const   CADEV__TG_IS_SITE_ONLY_REJ              = 0x59;
const   CADEV__DATA_PC_TARG_IN_GRP_ACT_REJ      = 0x5A;
const   CADEV__DATA_PC_TARG_IN_INT_ACT_REJ      = 0x5B;
const   CADEV__GO_DENIED_DUE_TO_GO_REJ          = 0x5C;
const   CADEV__GO_DENIED_DUE_TO_LAND_REJ        = 0x5D;
const   CADEV__INVALID_REQ_AUD_TME_SLT_REJ      = 0x5E;
%
const   CADEV__TRESPASS_PROTECTION_REJ          = 0x60;
const   CADEV__DUPLICATE_ID_REJ                 = 0x61;
const   CADEV__FEATURE_NOT_SUPPORTED_REJ        = 0x62;
const   CADEV__IND_ATTRIBS_TEMP_NOT_AVAIL_REJ   = 0x63;
const   CADEV__GRP_ATTRIBS_TEMP_NOT_AVAIL_REJ   = 0x64;
const   CADEV__SYSTEM_DENIED_IND_ACCESS_REJ     = 0x65;
const   CADEV__SYSTEM_DENIED_GRP_ACCESS_REJ     = 0x66;
const   CADEV__RQSTR_AFF_TG_NOT_MEM_OF_MG_REJ   = 0x67;
const   CADEV__NO_CAPABLE_SITES_REJ             = 0x68;
const   CADEV__NO_CAPABLE_DIU_REJ               = 0x69;
const   CADEV__NO_CAPABLE_CIU_REJ               = 0x6A;
const   CADEV__NO_CAPABLE_MBX_REJ               = 0x6B;
const   CADEV__NO_CAPABLE_IZAC_REJ              = 0x6C;
const   CADEV__TARGET_ZONE_UNREACHABLE_REJ      = 0x6D;
const   CADEV__TARGET_IHLR_UNREACHABLE_REJ      = 0x6E;
const   CADEV__RQSTR_HLR_UNREACHABLE_TO_TARGET_REJ   = 0x6F;
const   CADEV__TARGET_NOT_AT_TARGET_ZONE_REJ         = 0x70;
const   CADEV__TARGET_ATTRIBS_TEMP_NOT_AVAIL_REJ     = 0x71;
const   CADEV__NON_EMRG_RQST_DENIED_DUE_TO_EMRG_REJ  = 0x72;
const   CADEV__EXCEEDED_MAX_NUM_CNSL_OPS_REJ         = 0x73;
const   CADEV__BUSY_SERVICE_TERMINATED_REJ           = 0x74;
const   CADEV__DEVICE_TYPES_INCOMPAT_FOR_SERVICE_REJ = 0x75;
const   CADEV__INCOMPAT_PC_SECURE_MOD_TYPE_REJ       = 0x76;
const   CADEV__UNCONFIRMED_TG_DENIED_DUE_TO_MG_REJ   = 0x77;
const   CADEV__UNABLE_TO_BSY_RQSTR_ON_ACT_CALL       = 0x78;
const   CADEV__LAND_TO_MOBILE_LANDLINE_REJ           = 0x79;
%
%/*  NEW SUBSCRIBER REJECTS for OZCAR5  */
%
const   CADEV__STEALTH_REJECT                   = 0x90;
const   CADEV__RQSTR_NOT_REG                    = 0x91;
const   CADEV__RQSTR_NOT_AFF_TO_TG              = 0x92;
const   CADEV__TG_RQST_ON_REGROUPED_TG          = 0x93;
const   CADEV__IND_TERMINATED_CALL_REJ          = 0x94;
const   CADEV__INVALID_SERVICE_STATE_REJ        = 0x95;
const   CADEV__TARGET_IN_ANOTHER_CALL_REJ       = 0x96;
const   CADEV__PTT_TEMP_NOT_ALLOWED_REJ         = 0x97;
const   CADEV__CALL_TIMEOUT_REJ                 = 0x98;
const   CADEV__SEC_RQ_ON_CLR_ONLY_GRP_REC       = 0x99;
const   CADEV__CLR_RQ_ON_SEC_ONLY_GRP_REC       = 0x9A;
const   CADEV__PC_TARGET_REC_CLR_ONLY           = 0x9B;
const   CADEV__PC_TARGET_REC_SEC_ONLY           = 0x9C;
const   CADEV__SEC_REQ_BY_IND_CLR_ONLY          = 0x9D;
const   CADEV__CLR_REQ_BY_IND_SEC_ONLY          = 0x9E;
const   CADEV__KEY_NOT_AVAIL_AT_CON             = 0x9F;
const   CADEV__INFRASTRUCTURE_ERROR_REJ         = 0xA0;
const   CADEV__GRP_CALL_SETUP_ABORTED_REJ       = 0xA1;
const   CADEV__STEALTH_TG_DENIED_DUE_TO_MG_REJ  = 0xA2;
const   CADEV__ZONE_REMOVED_REJ                 = 0xA3;
const   CADEV__IND_RECORD_DELETED_REJ           = 0xA4;
const   CADEV__TARGET_REFUSED_CALL_REJ          = 0xA5;
const   CADEV__IND_IN_ANOTHER_CALL_REJ          = 0xA6;
const   CADEV__RSRC_PREEMPTION_REJ              = 0xA7;
const   CADEV__RQSTR_CANT_SRC_AT_SITE_REJ       = 0xA8;
const   CADEV__IND_NOT_AT_ZONE_REJ              = 0xA9;
%
%/* Radio Rejects */
%
const   CADEV__RADIO_REJ                        = 0x150;
%/*
%**-----------------------------------------------------------------------------
%** Application Programming Interface for CADI Events
%**-----------------------------------------------------------------------------
%*/
%
program CADEV__PROG
{
    version CADEV__VERS
    {
        /* 
        **        Event functions from the radios for CAD Host.  These are
        **        resulted from some actions undertaken by the radio or users
        **        of the radios.
        */
        void CADEV__AFFILIATION     ( CADEV__RADIO_EVENT_INFO )  = 1;
        void CADEV__CALL_ALERT      ( CADEV__CALL_ALERT_DATA )   = 2;
        void CADEV__CHANGE_ME       ( CADEV__CHANGE_ME_DATA )    = 3;
        void CADEV__CMD_STATUS      ( CADEV__CMD_STATUS_DATA )   = 4;
        void CADEV__DEAFFILIATION   ( CADEV__RADIO_EVENT_INFO )  = 5;
        void CADEV__EMERGENCY_ALARM ( CADEV__EA_DATA )           = 6;
        void CADEV__GROUP_CALL      ( CADEV__GROUP_CALL_DATA )   = 7;
        void CADEV__INTERCON_CALL   ( CADEV__INTERCONNECT_DATA ) = 8;
        void CADEV__MESSAGE         ( CADEV__MESSAGE_DATA )      = 9;
        void CADEV__PRIVATE_CALL    ( CADEV__PRIVATE_CALL_DATA ) = 10;
        void CADEV__STATUS          ( CADEV__STATUS_DATA )       = 11;
        /*
        **        This event is from the Event Processor to the CAD Host
        **        to inform it the status of the current session
        */
        void CADEV__SESSION         ( CADEV__SESSION_STATUS )    = 12;

        /*
        **        This event is from the ZC.  The ZC may just change the
        **        status (switching between primary and backup system) or
        **        as a result of the testing of the network connections
        */
        void CADEV__ZC_STATUS       ( CADEV__ZC_STATUS_DATA )    = 13;
        /* 
        **        This event is from the radio which has received the command 
        **        from the command processor--ultimately--and now ACK
        */
        void CADEV__RADIO_ACK       ( CADEV__RADIO_ACK_DATA )    = 14;
    } = 1;
} = 0x20001001;

#ifdef RPC_HDR
%#endif                /* H__CADEV */
#endif
