/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/CDI_Event.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $ Haipeng Jiang 
  * 1. hide the header file including declaration for <windows.h>
  */


#ifndef __CDI_Event_H
#define __CDI_Event_H

#include <vector>

//#include <windows.h> Haipeng hided this line! do not need.
#include <wtypes.h>

//#include "app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchEvent.h"

#define MCA_FEATURE_STATUS_CODE 1024
// Emergency Call Status
#define MCA_EMERG_CALL_STATE                  (MCA_FEATURE_STATUS_CODE + 0x0)
#define MCA_EMERG_ENDED                       (MCA_FEATURE_STATUS_CODE + 0x2) 
#define MCA_UNACKNOWLEDGED_EMERG_CALL_REFRESH (MCA_FEATURE_STATUS_CODE + 0x3)
#define MCA_ACKNOWLEDGED_EMERG_CALL_REFRESH   (MCA_FEATURE_STATUS_CODE + 0x4)
#define MCA_EMERG_CALL_ENDED_REFRESH          (MCA_FEATURE_STATUS_CODE + 0x5)
#define MCA_PATCHED_EMERG_CALL_STATE          (MCA_FEATURE_STATUS_CODE + 0x6)
#define MCA_PATCHED_EMERG_CALL_REFRESH        (MCA_FEATURE_STATUS_CODE + 0x7)

// Emergency Alarm Status
#define MCA_RADIO_EMERG_ALARM_RECEIVED         (MCA_FEATURE_STATUS_CODE + 0x0)
#define MCA_EMERG_ACKNOWLEDGED                 (MCA_FEATURE_STATUS_CODE + 0x1)
#define MCA_EMERG_ALARM_ENDED                  (MCA_FEATURE_STATUS_CODE + 0x2)
#define MCA_ACKNOWLEDGED_EMERG_ALARM_REFRESH   (MCA_FEATURE_STATUS_CODE + 0x3)
#define MCA_UNACKNOWLEDGED_EMERG_ALARM_REFRESH (MCA_FEATURE_STATUS_CODE + 0x4)
#define MCA_EMERG_ALARM_ENDED_REFRESH          (MCA_FEATURE_STATUS_CODE + 0x5)

namespace TA_IRS_App {
        namespace Mcdi
        {
		    //*** Registration ***/

		    class ClientRegistrationStatusEvent// //: public ConsoleDispatchEvent
		    {
		    public:
			    typedef enum { Registered, Deregistered } RegT;
		    };  

            class DispatchNotReadyEvent //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {  Serial_Link_Pc_Cie_Down, 
                                Serial_Link_Cie_Ceb_Down,
                                No_Local_View,
                                Retreiveing_New_View,
                                Pds_Synchronization,
                                Op_Id_Mismatch,
                                Internal_Error,
                                Pds_Sync_Timeout_Wrn,
                                Rc_Sync_Timeout_Wrn,
                                Rc_Sync_Err,
                                Rc_Syn_Comm_Err,
                                Rc_Local_Acess_Err,
                                Rc_Record_Not_Found_Err
                                } RegT;
            };

            class DispatchStatusEvent //: public ConsoleDispatchEvent
            {
            public:
			    typedef enum { Dispatch_Ready=1025, Dispatch_Not_Ready=1026 } RegT;
				typedef enum { GPIOM_NOT_VAILABLE=1024, GPIOM_AVAILABLE=1026 } PheripheralStatus;
            };
            
            class ConsoleShutdownEvent //:  public ConsoleDispatchEvent
            {
            public:
                typedef enum { Critical_Client_Terminated, Critical_Client_Not_Started } RegT;
            };
        };//end namespace Mcdi

        namespace Mca
        {
            typedef DWORD McaRadioSSI;
            typedef DWORD McaRID;
            typedef WORD McaChannel;
            typedef WORD McaErrorNumber;
    		typedef unsigned char CDClientId;
            typedef McaErrorNumber McaWarningNumber;

		    //*** Ambience Listening ***/
            class AmbienceListeningCallStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Attempted_To_Send, Target_Running, 
                              Ambience_Listening_Call_Begun, Ambience_Listening_Call_Ended} RegT;
            };

            class EmergencyCallStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Emergency_Call_State=1024,Emergency_Call_Ended=1026,
                              Unreg_Emergency_Call_Refresh=1025,Reg_Emergency_Call_Refresh=1028, 
							  Emergency_Call_Ended_Refresh = 1029 } RegT;              
            };

            class GeneralTransmitStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {General_Transmit_Begun,General_Transmit_Ended,
                              General_Transmit_Talk_Idle,General_Transmit_Talk_Waiting,
                              General_Transmit_Paging_Talk_Permitted,General_Transmit_Paging_Talk_Begun} RegT;
            };
            
            //**   Inbound Call**/
            class InboundCallStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Inbound_Call_Begun=0,  Inbound_Call_Without_Audio=1027,
                              Inbound_Call_Ended=1, Inbound_Call_Rsc_Selected_Here=1024,
                              Inbound_Call_Rsc_Selected_Elsewhere=1028, Inbound_Call_Rsc_Selected_Nowhere} RegT;
            };

            //**   Inbound Pttid status**/
            class InboundPttIdStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum { Inbound_Ptt_Radio_Init, Inbound_Ptt_Console_Init} RegT;           
            };

            class MselGroupStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Multi_Select_Group_Opened = 0x08, Multi_Select_Group_Closed=0x09 } RegT;
            };

            class PatchMemberStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Patch_Member_Added=0x0A, Patch_Member_Deleted=0x0B, Patch_Member_In_Used=0x0C} RegT;
            };
            
            class PatchGroupStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Patch_Group_Empty=0x13, Patch_Group_Active=0x04, Patch_Group_Inactive=0x05} RegT;
            };
			
			class PrivateCallStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum{Private_Call_Attempt_To_Send=0x0E, Private_Call_Target_Ringing=0x15,
                            Private_Call_Begun=0x00, Private_Call_Ended=0x01} RegT;
			};

            ///****** RefreshStatus *******//
            class RefreshStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum{Refresh_Begun, Refresh_Ended} RegT;
            };

            class AssignmentStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum{Resource_Assigned=1024, Resource_Deassigned, All_Resource_Deassigned} RegT;
            };

            ///****** Resource Reject Status  **//
            class ResourceRejectStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum{
                            INV_IND_ID_RQSTR,
                            INV_TGT_ID,
                            DSB_IND_ID_RQSTR,
                            DSB_IND_ID_TGT,
                            INV_TLK_GRP,
                            DSB_TLK_GRP_ID,
                            DSB_PC_II_TGT,
                            DSB_SYS_WIDE,
                            DSB_ANNOUNC,
                            DSB_TLK_GRP_CALL,
                            DSB_PC_II_RQSTR,
                            CLEAR_ONLY_RQSTR,
                            CLEAR_ONLY_TGT,
                            CODED_ONLY_RQSTR,
                            CODED_ONLY_TGT,
                            TGT_ID_NOT_ASTRO,
                            DI_NO_RSP_FRM_TGT_ID,
                            DI_TGT_NOT_INTERRUPTIBLE,
                            TLK_GRP_DENIED_MTG_ACT,
                            PTCH_RES_MTG,
                            CHG_TAC_STS_MTG,
                            CHG_RPT_STS_MTG,
                            INVALID_CNTRL_CNSL,
                            NO_AFFIL_MEMBERS,
                            INV_MSEL_GRP_NUM,
                            INV_MTG_IN_MSEL_REQ,
                            RQSTR_SUBS_REC_NOT_AVL,
                            PC_TGT_SUBS_REC_NOT_AVL,
                            TG_SUBS_REC_NOT_AVL,
                            RQSTR_SITE_PRHB,
                            PC_TGT_SITE_PRHB,
                            PC_TGT_DEAFFL,
                            INDV_EMRG_MTG_CAL_PRHB,
                            GRP_EMERG_REQ_PRHB,
                            RQSTR_CA_PRHB,
                            TGT_CA_PRHB,
                            TGT_CA_DEAFFL,
                            NO_SITEWD_SITES,
                            SITEWD_OVERLAP,
                            TGT_NOT_AFFILIATED,
                            CMD_NOT_ALLOWED_ON_REGRPED_TG,
                            NOT_ALLOWED_ON_REGRPED_TG,
                            TG_IS_SITE_ONLY,
                            UNKNOWN_REJECT,
                            FNE_DENIES_REQ,
                            RADIO_TIMEOUT,
                            RADIO_DENIES_REQ,
                            CHAN_FAILED,
                            SYS_BUSY,
                            ACCESS_FAILED,
                            TARGET_NOT_RESP,
                            KEY_2ND_TG_DENIED,
                            INV_AREA_ID_REQ,
                            SVC_INACTIVE,
                            NO_SVC,
                            ISO,
                            RADIO_LOCKED,
                            HIGH_BATT,
                            HIGH_TEMP,
                            OP_ID_NOT_IN_DB,
                            CAL_WAIT_IN_APC_BSY,
                            NO_RESRC_AVAIL,
                            GRP_REQ_SITE_PRHB,
                            DUPLICATE_ID,
                            FEATURE_NOT_SUPPORTED,
                            IND_TEMP_ATTRIBS_NOT_AVAIL,
                            TG_TEMP_ATTRIBS_NOT_AVAIL,
                            SYS_DENIED_IND_ACCESS,
                            SYS_DENIED_TG_ACCESS,
                            NO_CAPABLE_SITES,
                            NO_CAPABLE_DIU,
                            NO_CAPABLE_CIU,
                            NO_CAPABLE_MBX,
                            NO_CAPABLE_IZAC,
                            TARGET_ZONE_UNREACHABLE,
                            TARGET_IHLR_UNREACHABLE,
                            RQSTR_HLR_UNREACH_TO_TRGT,
                            TARGET_NOT_AT_TARGET_ZONE,
                            TARGET_ATT_TEMP_NOT_AVAIL,
                            NON_EMRG_RQST_DENIED_DUE_TO_EMRG,
                            EXCEEDED_MAX_NUM_OPS,
                            DEV_TYPE_INCOMPAT_FOR_SERVICE,
                            INCOMPAT_PC_SECURE_MOD_TYPE,
                            UNCONFIRM_TG_DENIED_MTG_ACTV,
                            UNABLE_TO_BUSY_RQSTR_ON_ACT_CALL,
                            INFRASTRUCTURE_REJ,
                            TALK_PROHIBIT_RCV,
                            AFFL_REJ_MSG,
                            RQSTR_NOT_REG,
                            RQSTR_NOT_AFF_TO_TG,
                            IND_TERMINATED_CALL,
                            INVALID_SERVICE_STATE,
                            TARGET_IN_ANOTHER_CALL,
                            PTT_TEMP_NOT_ALLOWED,
                            CALL_TIMEOUT,
                            SEC_RQ_ON_CLR_ONLY_GRP_REC,
                            CLR_RQ_ON_SEC_ONLY_GRP_REC,
                            PC_TARGET_REC_CLR_ONLY,
                            PC_TARGET_REC_SEC_ONLY,
                            SEC_REQ_BY_IND_CLR_ONLY,
                            CLR_REQ_BY_IND_SEC_ONLY,
                            KEY_NOT_AVAIL_AT_CONSOLE,
                            INFRASTRUCTURE_ERROR,
                            TARGET_REFUSED_CALL,
                            BUSY_SERVICE_TERMINATED,
                            NON_TACT_NOT_ALLOWED,
                            UNDEFINED_CAUSE,
                            USER_REQUESTED_DISCONNECT,
                            TGT_BUSY_REJECT,
                            TGT_NOT_REACHABLE,
                            ENCRYPTION_NOT_SUPPORTED,
                            INFRASTRUCTURE_CONGESTION,
                            TRAFFIC_CASE_NOT_ALLOWED,
                            TRAFFIC_CASE_INCOMPATIBLE,
                            SERVICE_NOT_AVAILABLE_REJECT,
                            RSC_PREEMPTION,
                            INVALID_CALL_IDENTIFIER,
                            CALL_REJECTED,
                            NO_IDLE_CALL_CONTROL_ENTITY,
                            CALL_TIMER_EXPIRED,
                            INFRASTRUCTURE_DISCONNECT,
                            NO_ACK_RECEIVED_REJECT,
                            INVALID_UNIT_ID_REJECT,
                            SUPPLEMENTARY_SERVICE_DISCONNECT
                            } RegT;
            };

            ///****** Resource Transmit Status  **//
            class ResourceTransmitStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum{Resource_Transmit_Begun=0x0400,Resource_Transmit_Busy,
                            Resource_Transmit_Callback,Resource_Transmit_Idle,
                            Resource_Transmit_Data_Busy, Resource_Transmit_Pending,
							Resource_Transmit_Without_Radio
                            } RegT;
            };

			class ResourceSelectStatus //: public ConsoleDispatchEvent
            {
            public:
                typedef enum {Resource_Selected=0x0400, Resource_Deselected} RegT;
			};
        };//end namespace Mca
};//end namespace TA_IRS_App


#endif
