#ifndef MCC_PROTOCOL_HELPER_H_INCLUDED
#define MCC_PROTOCOL_HELPER_H_INCLUDED

#include "app/radio/RadioTetraAgent/src/RemoteAPI.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/TransactiveException.h"

namespace RadioAgentMcc
{
	struct CallBackMessage
	{
		char name[75];
		unsigned short value;
	};
	static unsigned int MCC_STANDARD_CODE = 0x0400;
	enum E_MethodeID
	{
		/* MCA methods */
		eMcaBeginAllMuteID                                       = 1001,
		eMcaEndAllMuteID                                         = 1002,

		eMcaBeginAmbienceListeningCallID                         = 1010,
		eMcaEndAmbienceListeningCallID                           = 1011,

		eMcaBeginAPBTransmitID                                   = 1020,
		eMcaEndAPBTransmitID                                     = 1021,

		eMcaAssignAudioDestinationExID                           = 1030,

		eMcaAssignAuxioID                                        = 1040,
		eMcaDeassignAuxioID                                      = 1041,

		eMcaActivateAuxioID                                      = 1050,
		eMcaClearAuxioStatusID                                   = 1051,
		eMcaDeactivateAuxioID                                    = 1052,

		eMcaSoundBeepsID                                         = 1060,

		eMcaAbortCallAlertID                                     = 1070,
		eMcaSendCallAlertID                                      = 1071,

		eMcaSelectCriticalResourceSettingID                      = 1080,

		eMcaEndEmergencyID                                       = 1090,
		eMcaAcknowledgeEmergencyID                               = 1091,
		eMcaBeginConsoleEmergencyCallID                          = 1092,

		eMcaSelectFrequencyID                                    = 1100,

		eMcaBeginGeneralTransmitID                               = 1110,
		eMcaEndGeneralTransmitExID                               = 1111,
		eMcaBeginInstantTransmitID                               = 1112,
		eMcaEndInstantTransmitID                                 = 1113,

		eMcaSelectKeysetID                                       = 1120,

		eMcaAssignLoggingAudioDestinationID                      = 1130,
		eMcaAssignLoggingResourceID                              = 1131,
		eMcaDeassignLoggingResourceID                            = 1132,

		eMcaAssignIndividualLoggingResourceID                    = 1135,
		eMcaDeassignIndividualLoggingResourceID                  = 1136,

		eMcaBeginMonitorID                                       = 1140,
		eMcaEndMonitorExID                                       = 1141,

		eMcaAddMselMemberID                                      = 1150,
		eMcaCloseMselID                                          = 1151,
		eMcaDeleteMselMemberID                                   = 1152,
		eMcaGetFreeMselGroupID                                   = 1153,
		eMcaOpenMselID                                           = 1154,

		eMcaBeginMuteSecondReceiverID                            = 1160,
		eMcaEndMuteSecondReceiverID                              = 1161,

		eMcaBeginPagePttID                                       = 1170,
		eMcaEndPagePttExID                                       = 1171,

		eMcaAddPatchMemberID                                     = 1180,
		eMcaDeletePatchMemberID                                  = 1181,
		eMcaGetFreePatchGroupID                                  = 1182,

		eMcaBeginPatchTransmitID                                 = 1190,
		eMcaEndPatchTransmitID                                   = 1191,

		eMcaAnswerPrivateCallID                                  = 1200,
		eMcaBeginInterruptPriorityPrivateCallID                  = 1201,
		eMcaBeginPrivateCallID                                   = 1202,
		eMcaEndPrivateCallID                                     = 1203,
		eMcaSelectPrivateLineID                                  = 1204,
		eMcaRejectPrivateCallID                                  = 1205,

		eMcaBeginRefreshID                                       = 1210,

		eMcaDisableRepeatID                                      = 1220,
		eMcaEnableRepeatID                                       = 1221,

		eMcaAssignResourceExID                                   = 1230,
		eMcaDeassignResourceID                                   = 1231,
		eMcaDeselectResourceID                                   = 1232,

		eMcaSelectResourceID                                     = 1240,
		eMcaSelectPriorityID                                     = 1241,

		eMcaBeginTakeoverID                                      = 1250,
		eMcaEndTakeoverID                                        = 1251,

		eMcaBeginToneID                                          = 1260,
		eMcaEndToneID                                            = 1261,

		eMcaSelectTransmitModeID                                 = 1270,

		eMcaChangeVolumeID                                       = 1280,

		eMcaActivateWildcardID                                   = 1290,
		eMcaDeactivateWildcardID                                 = 1291,

		eMcaAssignResourceID                                     = 1300,



		/* MCDI methods */
		eMcdiGetAudioProcessingStatusID                          = 2000,
		eMcdiGetAuthenticationStatusID                           = 2001,
		eMcdiGetMessageTransmitErrorID                           = 2002,
		eMcdiDatabaseUpdateGrantID                               = 2003,
		eMcdiDatabaseUpdateNoGrantID                             = 2004,
		eMcdiGetDatabaseStatusID                                 = 2005,


		/* MCDB methods */
		eMcdbCreateTalkChannelQueryID                            = 3000,
		eMcdbCreateTalkChannelQueryByVersionID                   = 3001,
		eMcdbCreateTalkChannelWithCapProfileMapQueryID           = 3002,

		eMcdbGetNextTalkChannelID                                = 3010,
		eMcdbGetNextTalkChannelByVersionID                       = 3011,
		eMcdbGetNextTalkChannelWithCapProfileMapID               = 3012,
		eMcdbCreateTalkChannelWithCapProfileMapQueryByVersionID  = 3013,
		eMcdbGetNextTalkChannelWithCapProfileMapByVersionID      = 3014,
		eMcdbGetTalkChannelInfoID                                = 3015,
		eMcdbGetNextTalkChannelsID                               = 3016,
		eMcdbGetNextTalkChannelsByVersionID                      = 3017,
		eMcdbGetNextTalkChannelsWithCapProfileMapID              = 3018,
		eMcdbGetNextTalkChannelsWithCapProfileMapByVersionID     = 3019,

		eMcdbCreateAuxioQueryID                                  = 3020,
		eMcdbGetAuxioInfoID                                      = 3021,
		eMcdbGetNextAuxioID                                      = 3022,

		eMcdbOpenDatabaseExID                                    = 3034,
		eMcdbCloseDatabaseID                                     = 3035,

		eMcdbCreateOpCapabilityQueryByVersionID                  = 3040,
		eMcdbGetNextOpCapabilityByVersionID                      = 3041,
		eMcdbCreateOpCapabilityIndexQueryID                      = 3042,
		eMcdbGetNextOpCapabilityIndexRecID                       = 3043,
		eMcdbCheckOpCapabilityIdID                               = 3044,
		eMcdbDestroyQueryID                                      = 3045,

		eMcdbCreateOpPosQueryID                                  = 3050,
		eMcdbGetMaxMC_LC_IDID                                       = 3051,
		eMcdbGetNextOpPosID                                      = 3052,
		eMcdbGetOpIdID                                           = 3053,
		eMcdbGetLoggingCapacityID                                = 3054,


		eMcdbCreateAllResCapabilityByTalkResTypeQueryID          = 3060,
		eMcdbGetNextAllResCapabilityByTalkResTypeID              = 3061,
		eMcdbCreateResCapabilityQueryID                          = 3062,
		eMcdbCreateProfileCapabilityQueryID                      = 3063,
		eMcdbCreateProfileCapabilityQueryByVersionID             = 3064,
		eMcdbGetNextResourceCapabilityID                         = 3065,
		eMcdbCreateResCapabilityQueryByVersionID                 = 3066,
		eMcdbGetNextResourceCapabilityByVersionID                = 3067,
		eMcdbCreateResCapabilityIndexQueryID                     = 3068,
		eMcdbGetNextResCapabilityIndexRecID                      = 3069,
		eMcdbGetNextProfileCapabilityID                          = 3070,
		eMcdbGetNextProfileCapabilityByVersionID                 = 3071,
		eMcdbCheckChannelCapabilityIdID                          = 3072,
		eMcdbGetResCapabilityLabelID                             = 3073,

		eMcdbCreateSiteByZoneQueryByVersionID                    = 3080,
		eMcdbGetNextSiteByZoneByVersionID                        = 3081,
		eMcdbCreateZoneQueryID                                   = 3082,
		eMcdbGetNextZoneID                                       = 3083,

		eMcdbCreateSpeakerQueryID                                = 3090,
		eMcdbGetNextSpeakerID                                    = 3091,
		eMcdbGetSpeakerLabelID                                   = 3092,

		eMcdbCreateTrkResQueryID                                 = 3100,
		eMcdbGetNextTrkResRecID                                  = 3101,

		eMcdbCreateTrunkSysQueryID                               = 3110,
		eMcdbGetNextTrunkSysID                                   = 3111,   
		eMcdbGetTrunkSysInfoID                                   = 3112,

		eMcdbCreateAuxioQueryByVersionID                         = 3120,
		eMcdbGetNextAuxioByVersionID                             = 3121,
		eMcdbCreateSecureKeysetQueryID                           = 3122,
		eMcdbGetNextSecureKeysetID                               = 3123,
		eMcdbGetSecureKeysetAliasID                              = 3124,
		eMcdbCreateUnitAliasListQueryID                          = 3125,
		eMcdbGetActiveSecureKeysetID                             = 3126,
		eMcdbGetGivenUnitAliasID                                 = 3127,
		eMcdbGetNextUnitAliasID                                  = 3128,
		eMcdbGetUnitIDID                                         = 3129,
		eMcdbGetFirstUnitAliasID                                 = 3130,

		eMcdbCreateLoggingUnitAliasListQueryByVersionID          = 3131,
		eMcdbGetNextUnitAliasByVersionID                         = 3132,
		eMcdbGetNextUnitAliasesByVersionID                       = 3133,

		eMcdbGetNextUnitAliasesID                                = 3134, 

		eMcdbCreateOpCapabilityQueryID                           = 3140,
		eMcdbGetNextOpCapabilityID                               = 3141,

		eMcdbCreateSiteByZoneQueryID                             = 3150,
		eMcdbGetNextSiteByZoneID                                 = 3151,

		eMcdbCreateResCapProfileMapQueryID                       = 3160,
		eMcdbGetNextResCapProfileMapID                           = 3161,
		eMcdbCreateSpecificProfileCapabilitiesQueryByVersionID   = 3162,
		eMcdbGetNextSpecificProfileCapabilitiesByVersionID       = 3163,

		eMcdbGetStatusAliasID                                    = 3170,
	};

	enum ECallBackMessage{
		eMsgMcdiAudioProcessingStatusID = 0x5000,

		eMsgMcdiAuthenticationErrorID = 0x5010,
		eMsgMcdiAuthenticationWarningID = 0x5011,
		eMsgMcdiAuthenticationStatusID = 0x5012,

		eMsgMcdiConsoleShutDownID = 0x5020, 

		eMsgMcdiActivateClientID = 0x5030,
		eMsgMcdiDeactivateClientID = 0x5031,
		eMsgMcdiClientActivationStatusID = 0x5032,

		eMsgMcdiMessageLossID = 0x5040,

		eMsgMcdiClientRegistrationStatusID = 0x5050,
		eMsgMcdiRegistrationStatusRequestID = 0x5051, 

		eMsgMcdiNewDatabaseGrantedID = 0x5060,
		eMsgMcdiNewDatabaseNotGrantedID = 0x5061,
		eMsgMcdiNewDatabaseAvailableID = 0x5062,
		eMsgMcdiDatabaseUpdateNotificationID = 0x5063,
		eMsgMcdiDatabaseStatusID = 0x5064,


		/* MCA messages */
		eMsgMcaBeginAllMuteErrorID = 0x1000,
		eMsgMcaEndAllMuteErrorID = 0x1001,
		eMsgMcaAllMuteStatusID = 0x1002,

		eMsgMcaBeginAmbienceListeningCallErrorID = 0x1010,
		eMsgMcaEndAmbienceListeningCallErrorID = 0x1011,
		eMsgMcaAmbienceListeningCallStatusID = 0x1012,

		eMsgMcaBeginAPBTransmitWarningID = 0x1020,
		eMsgMcaBeginAPBTransmitErrorID = 0x1021,
		eMsgMcaEndAPBTransmitErrorID = 0x1022,
		eMsgMcaAPBTransmitStatusID = 0x1023,

		eMsgMcaAssignAudioDestinationWarningID = 0x1030,
		eMsgMcaAssignAudioDestinationErrorID = 0x1031,
		eMsgMcaAudioDestinationStatusID = 0x1032,

		eMsgMcaAssignAuxioErrorID = 0x1040,
		eMsgMcaDeassignAuxioErrorID = 0x1041,
		eMsgMcaAuxioAssignmentStatusID = 0x1042,

		eMsgMcaActivateAuxioErrorID = 0x1050, 
		eMsgMcaDeactivateAuxioErrorID = 0x1051,
		eMsgMcaClearAuxioStatusErrorID = 0x1052,
		eMsgMcaAuxioStatusID = 0x1053,

		eMsgMcaSoundBeepsErrorID = 0x1060,

		eMsgMcaSendCallAlertErrorID = 0x1070,
		eMsgMcaAbortCallAlertErrorID = 0x1071,
		eMsgMcaCallAlertStatusID = 0x1072,
		eMsgMcaInboundCallAlertStatusID = 0x1073,

		eMsgMcaSelectCriticalResourceSettingErrorID = 0x1080,
		eMsgMcaCriticalResourceStatusID = 0x1081, 

		eMsgMcaDatabaseUpdateNotificationStatusID = 0x1090, 

		eMsgMcaAcknowledgeEmergencyErrorID = 0x10A0,
		eMsgMcaEmergencyAlarmStatusID = 0x10A1,

		eMsgMcaBeginConsoleEmergencyCallErrorID = 0x10B0,
		eMsgMcaEndEmergencyErrorID = 0x10B1,
		eMsgMcaEmergencyCallStatusID = 0x10B2,

		eMsgMcaSelectFrequencyErrorID = 0x10C0,
		eMsgMcaResourceFrequencyStatusID = 0x10C1,

		eMsgMcaBeginGeneralTransmitWarningID = 0x10D0,
		eMsgMcaBeginGeneralTransmitErrorID = 0x10D1,
		eMsgMcaEndGeneralTransmitErrorID = 0x10D2,
		eMsgMcaGeneralTransmitStatusID = 0x10D3,

		eMsgMcaInboundCallStatusID = 0x10E0,

		eMsgMcaInboundPttIdStatusID = 0x10F0,

		eMsgMcaBeginInstantTransmitErrorID = 0x1100, 
		eMsgMcaEndInstantTransmitErrorID = 0x1101,

		eMsgMcaSelectKeysetErrorID = 0x1110,  
		eMsgMcaKeysetStatusID = 0x1111,  

		eMsgMcaAssignLoggingAudioDestinationErrorID = 0x1120, 
		eMsgMcaLoggingAudioDestinationStatusID = 0x1121, 

		eMsgMcaLoggingGroupCallStatusID = 0x1130, 
		eMsgMcaLoggingPrivateCallEventStatusID = 0x1131,
		eMsgMcaLoggingPrivateCallStatusID = 0x1132,
		eMsgMcaLoggingTelephoneCallEventStatusID = 0x1133,
		eMsgMcaLoggingTelephoneCallStatusID = 0x1134,

		eMsgMcaAssignLoggingResourceWarningID = 0x1140, 
		eMsgMcaAssignLoggingResourceErrorID = 0x1141, 
		eMsgMcaDeassignLoggingResourceWarningID = 0x1142, 
		eMsgMcaDeassignLoggingResourceErrorID = 0x1143, 
		eMsgMcaLoggingAssignmentStatusID = 0x1144,
		eMsgMcaAssignIndividualLoggingResourceWarningID = 0x1145,
		eMsgMcaAssignIndividualLoggingResourceErrorID = 0x1146,
		eMsgMcaDeassignIndividualLoggingResourceWarningID = 0x1147,
		eMsgMcaDeassignIndividualLoggingResourceErrorID = 0x1148,
		eMsgMcaLoggingIndividualAssignmentStatusID = 0x1149,

		eMsgMcaBeginMonitorErrorID = 0x1150, 
		eMsgMcaEndMonitorErrorID = 0x1151, 
		eMsgMcaMonitorStatusID = 0x1152, 

		eMsgMcaOpenMselErrorID = 0x1160,  
		eMsgMcaCloseMselErrorID = 0x1161,  
		eMsgMcaMselGroupStatusID = 0x1162,  
		eMsgMcaAddMselMemberWarningID = 0x1163, 
		eMsgMcaAddMselMemberErrorID = 0x1164,  
		eMsgMcaDeleteMselMemberErrorID = 0x1165,  
		eMsgMcaMselMemberStatusID = 0x1166,  
		eMsgMcaMselMemberRefreshID = 0x1167, 
		eMsgMcaGetFreeMselGroupErrorID = 0x1168, 
		eMsgMcaFreeMselGroupStatusID = 0x1169, 

		eMsgMcaBeginMuteSecondReceiverErrorID = 0x1170,  
		eMsgMcaEndMuteSecondReceiverErrorID = 0x1171, 
		eMsgMcaMuteSecondReceiverStatusID = 0x1172, 

		eMsgMcaBeginPagePttWarningID = 0x1180, 
		eMsgMcaBeginPagePttErrorID = 0x1181, 
		eMsgMcaEndPagePttErrorID = 0x1182, 
		eMsgMcaPagePttStatusID = 0x1183, 

		eMsgMcaAddPatchMemberWarningID = 0x1190, 
		eMsgMcaAddPatchMemberErrorID = 0x1191,  
		eMsgMcaDeletePatchMemberErrorID = 0x1192, 
		eMsgMcaPatchMemberStatusID = 0x1193, 
		eMsgMcaPatchGroupStatusID = 0x1194, 
		eMsgMcaPatchMemberRefreshID = 0x1195,  
		eMsgMcaGetFreePatchGroupErrorID = 0x1196, 
		eMsgMcaFreePatchGroupStatusID = 0x1197, 

		eMsgMcaBeginPatchTransmitWarningID = 0x11A0, 
		eMsgMcaBeginPatchTransmitErrorID = 0x11A1, 
		eMsgMcaEndPatchTransmitErrorID = 0x11A2,  
		eMsgMcaPatchTransmitStatusID = 0x11A3, 

		eMsgMcaPeripheralsStatusID = 0x11B0,  

		eMsgMcaAnswerPrivateCallErrorID = 0x11C0,  
		eMsgMcaBeginPrivateCallErrorID = 0x11C1,  
		eMsgMcaEndPrivateCallErrorID = 0x11C2, 
		eMsgMcaInboundPrivateCallStatusID = 0x11C3,  
		eMsgMcaPrivateCallStatusID = 0x11C4,  
		eMsgMcaRejectPrivateCallErrorID = 0x11C5,

		eMsgMcaSelectPrivateLineErrorID = 0x11D0,  
		eMsgMcaResourcePrivateLineStatusID = 0x11D1,  

		eMsgMcaInboundRadioStatusID = 0x11E0,  

		eMsgMcaRefreshStatusID = 0x11F0,  

		eMsgMcaEnableRepeatErrorID = 0x1200,   
		eMsgMcaDisableRepeatErrorID = 0x1201,  
		eMsgMcaRepeatStatusID = 0x1202,  

		eMsgMcaAssignResourceWarningID = 0x1210,   
		eMsgMcaAssignResourceErrorID = 0x1211,   
		eMsgMcaDeassignResourceWarningID = 0x1212,   
		eMsgMcaDeassignResourceErrorID = 0x1213,   
		eMsgMcaAssignmentStatusID = 0x1214,   

		eMsgMcaResourceRejectStatusID = 0x1220,   

		eMsgMcaResourceStatusID = 0x1230,   

		eMsgMcaResourceTransmitStatusID = 0x1240,   
		eMsgMcaResourceTransmitWarningID = 0x1241,
		eMsgMcaSelectResourceErrorID = 0x1250,    
		eMsgMcaDeselectResourceErrorID = 0x1251,   
		eMsgMcaResourceSelectStatusID = 0x1252,   

		eMsgMcaSelectPriorityErrorID = 0x1260,    
		eMsgMcaPriorityStatusID = 0x1261,   

		eMsgMcaBeginTakeoverErrorID = 0x1270,    
		eMsgMcaEndTakeoverErrorID = 0x1271,    
		eMsgMcaTakeoverStatusID = 0x1272,   

		eMsgMcaBeginToneErrorID = 0x1280,   
		eMsgMcaEndToneErrorID = 0x1281,   
		eMsgMcaToneStatusID = 0x1282,   

		eMsgMcaSelectTransmitModeErrorID = 0x1290,    
		eMsgMcaSelectTransmitModeStatusID = 0x1291,   

		eMsgMcaTrunkingSystemStatusID = 0x12A0,   

		eMsgMcaChangeVolumeWarningID = 0x12B0,   
		eMsgMcaChangeVolumeErrorID = 0x12B1,   
		eMsgMcaResourceVolumeStatusID = 0x12B2,   

		eMsgMcaActivateWildcardErrorID = 0x12C0,   
		eMsgMcaDeactivateWildcardErrorID = 0x12C1,   
		eMsgMcaWildcardStatusID = 0x12C2
	};

class MccDbException : public TA_Base_Core::TransactiveException
{
public:
	MccDbException(const std::string msg) : TA_Base_Core::TransactiveException(msg) {};
};


// Handler class to send command
	
class MccRadioCommand
{
	
protected:
	MCC7500SDK::Parameters in;
	MCC7500SDK::Parameters_var out;
	MCC7500SDK::UVariant_var result;
	MCC7500SDK::IAPIRequest_ptr &pAPIRequest;
	MCC7500SDK::METHOD_ID method_id;
	std::string command_name;
	unsigned char clientid;
	MccRadioCommand(MCC7500SDK::IAPIRequest_ptr &ptr,MCC7500SDK::METHOD_ID mid);
public:
	~MccRadioCommand();
	virtual void execute();
	virtual std::string getCommandName();
};

class OpenDatabase : public MccRadioCommand
{
	public:
		OpenDatabase(MCC7500SDK::IAPIRequest_ptr & ptr) : MccRadioCommand(ptr, eMcdbOpenDatabaseExID)
		{
			in.length(0);
			command_name = "eMcdbOpenDatabaseExID";
		}
		unsigned int getHandle()
		{
			return out[0].V_UINT32();
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
				case 0x02: message = "MCDB_DATABASE_UNAVAILABLE"; break;
				case 0x0f: message = "MCDB_OPEN_DATABASE_UNSUCCESSFUL"; break;					
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbOpenDatabaseExID status = %s", message.c_str());
		}
};

class CloseDatabase : public MccRadioCommand
{
	public:
		CloseDatabase(MCC7500SDK::IAPIRequest_ptr & ptr, unsigned int hdb) : MccRadioCommand(ptr, eMcdbCloseDatabaseID)
		{
			in.length(1);
			in[0].V_UINT32(hdb);
			command_name = "eMcdbCloseDatabaseID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x03: message = "MCDB_INVALID_DB_HANDLE"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
				case 0x01: message = "MCDB_CLOSE_DATABASE_UNSUCCESSFUL"; break;			
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbCloseDatabaseID status = %s", message.c_str());			
		}
};

class DestroyQuery : public MccRadioCommand
{
	public:
		DestroyQuery(MCC7500SDK::IAPIRequest_ptr & ptr, unsigned int hq) : MccRadioCommand(ptr, eMcdbDestroyQueryID)
		{
			in.length(1);
			in[0].V_UINT32(hq);
			command_name = "eMcdbDestroyQueryID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
				case 0x09: message = "MCDB_DESTROY_QUERY_UNSUCCESSFUL"; break;			
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbDestroyQueryID status = %s", message.c_str());			
		}
};

class CreateTalkChannelQuery : public MccRadioCommand
{
	public:
		CreateTalkChannelQuery(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hdb) : MccRadioCommand(ptr, eMcdbCreateTalkChannelQueryID)
		{
			in.length(2);
			in[0].V_UINT32(hdb);
			in[1].V_UINT32(0);
			command_name = "eMcdbCreateTalkChannelQueryID";
		}
		unsigned int getNumRecs()
		{
			return out[1].V_UINT32();
		}
		unsigned int getQueryHdb()
		{
			return out[0].V_UINT32();
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x03: message = "MCDB_INVALID_DB_HANDLE"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x07: message = "MCDB_QUERY_UNSUCCESSFUL"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbCreateTalkChannelQueryID status = %s", message.c_str());			
		}
};

class GetNextTalkChannel : public MccRadioCommand
{
	public:
		GetNextTalkChannel(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hq) : MccRadioCommand(ptr, eMcdbGetNextTalkChannelID)
		{
			in.length(1);
			in[0].V_UINT32(hq);
			command_name = "eMcdbGetNextTalkChannelID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		unsigned int getUrid()
		{
			return out[0].V_UINT32();
		}
		std::string getLabel()
		{
			std::wstring ws = out[1].V_WSTRING();
			std::string s(ws.begin(), ws.end());
			return s;
		}
		unsigned int getTalkChannelType()
		{
			return out[2].V_UINT32();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x04: message = "MCDB_INVALID_QUERY_HANDLE"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x05: message = "MCDB_RETRIEVAL_UNSUCCESSFUL"; break;
				case 0x06: message = "MCDB_RECORD_NOT_FOUND"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbGetNextTalkChannelID status = %s", message.c_str());			
		}
};

class CreateResCapabilityIndexQuery : public MccRadioCommand
{
	public:
		CreateResCapabilityIndexQuery(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hdb, unsigned int urid) : MccRadioCommand(ptr, eMcdbCreateResCapabilityIndexQueryID)
		{
			in.length(4);
			in[0].V_UINT32(hdb);
			in[1].V_UINT32(urid);
			in[2].V_UINT16(0);
			in[3].V_UINT32(0);
			command_name = "eMcdbCreateResCapabilityIndexQueryID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		unsigned int getQueryHdb()
		{
			return out[0].V_UINT32();
		}
		unsigned int getNumRecs()
		{
			return out[1].V_UINT32();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x03: message = "MCDB_INVALID_DB_HANDLE"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x07: message = "MCDB_QUERY_UNSUCCESSFUL"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbCreateResCapabilityIndexQueryID status = %s", message.c_str());			
		}
};

class GetNextResCapabilityIndexRec : public MccRadioCommand
{
	public:
		GetNextResCapabilityIndexRec(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hq) : MccRadioCommand(ptr, eMcdbGetNextResCapabilityIndexRecID)
		{
			in.length(1);
			in[0].V_UINT32(hq);
			command_name = "eMcdbGetNextResCapabilityIndexRecID";
		}
		unsigned short getCapabilityIndex()
		{
			return out[0].V_UINT16();
		}
		std::string getLabel()
		{			
			std::wstring ws = out[1].V_WSTRING();
			std::string s(ws.begin(), ws.end());
			return s;
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x04: message = "MCDB_INVALID_QUERY_HANDLE"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x05: message = "MCDB_RETRIEVAL_UNSUCCESSFUL"; break;
				case 0x06: message = "MCDB_RECORD_NOT_FOUND"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbGetNextResCapabilityIndexRecID status = %s", message.c_str());			
		}
};

class CreateUnitAliasListQuery : public MccRadioCommand
{
	public:
		CreateUnitAliasListQuery(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hDb, unsigned int urid) : MccRadioCommand(ptr, eMcdbCreateUnitAliasListQueryID)
		{
			in.length(3);
			in[0].V_UINT32(hDb);
			in[1].V_UINT32(urid);
			in[2].V_UINT32(0);
			command_name = "eMcdbCreateUnitAliasListQueryID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		unsigned int getQueryHdb()
		{
			return out[0].V_UINT32();
		}
		unsigned int getNumRecs()
		{
			return out[1].V_UINT32();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x03: message = "MCDB_INVALID_DB_HANDLE"; break;
				case 0x08: message = "MCDB_COMMUNICATION_FAILURE"; break;
				case 0x07: message = "MCDB_QUERY_UNSUCCESSFUL"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
				case 0x06: message = "MCDB_RECORD_NOT_FOUND"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbCreateUnitAliasListQueryID status = %s", message.c_str());			
		}
};

class GetNextUnitAlias : public MccRadioCommand
{
	public:
		GetNextUnitAlias(MCC7500SDK::IAPIRequest_ptr& ptr, unsigned int hq) : MccRadioCommand(ptr, eMcdbGetNextUnitAliasID)
		{
			in.length(1);
			in[0].V_UINT32(hq);
			command_name = "eMcdbGetNextUnitAliasID";
		}
		unsigned short getStatus()
		{
			return result->V_UINT16();
		}
		std::string getUnitAlias()
		{
			std::wstring ws = out[0].V_WSTRING();
			std::string s(ws.begin(), ws.end());
			return s;
		}
		unsigned int getUnitId()
		{
			return out[1].V_UINT32();
		}
		unsigned int getPosition()
		{
			return out[2].V_UINT32();
		}
		void printStatus()
		{
			std::string message = "";
			switch (getStatus()) {
				case 0x00: message = "MCDB_NO_ERROR"; break;
				case 0x04: message = "MCDB_INVALID_QUERY_HANDLE"; break;
				case 0x06: message = "MCDB_RECORD_NOT_FOUND"; break;
				case 0x0e: message = "MCDB_INVALID_PARAMETER"; break;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "MCC: eMcdbGetNextUnitAliasID status = %s", message.c_str());			
		}
};

class DatabaseUpdateGrant : public MccRadioCommand
{
	public:
		DatabaseUpdateGrant(MCC7500SDK::IAPIRequest_ptr& ptr) : MccRadioCommand(ptr,eMcdiDatabaseUpdateGrantID)
		{
			in.length(0);
			command_name = "eMcdiDatabaseUpdateGrantID";
		}
		bool getStatus()
		{
			return result->V_BOOLEAN();
		}
};

class DatabaseUpdateNoGrant : public MccRadioCommand
{
	public:
		DatabaseUpdateNoGrant(MCC7500SDK::IAPIRequest_ptr& ptr) : MccRadioCommand(ptr,eMcdiDatabaseUpdateNoGrantID)
		{
			in.length(0);
			command_name = "eMcdiDatabaseUpdateNoGrantID";
		}
		bool getStatus()
		{
			return result->V_BOOLEAN();
		}
};

// Handler class for status received from MCC

class MccRadioEvent
{
	public:
		MccRadioEvent(MCC7500SDK::Parameters p);
		virtual ~MccRadioEvent();
		unsigned short getMessageType();
		virtual void printMessage();
		virtual std::string getEventName();
	protected:
		unsigned short message_id;
		::MCC7500SDK::Parameters params;
		std::string event_name;
};

class McdiAudioProcessingStatus : public MccRadioEvent  {
public:
	McdiAudioProcessingStatus(const ::MCC7500SDK::Parameters &param):MccRadioEvent(param) {
		event_name = "McdiAudioProcessingStatus";
	}
	unsigned short get_wStatus(){
		return params[1].V_UINT16();
	}
	bool isReady()
	{
		if (get_wStatus()-MCC_STANDARD_CODE == 01) return true;
		else return false;
	}
	void printMessage() {		
		std::string message = "";
		switch(get_wStatus()-MCC_STANDARD_CODE) 
		{
			case 0: message = "MCDI_AUDIO_PROCESSING_INITIALIZING"; break;
			case 1: message = "MCDI_AUDIO_PROCESSING_AVAILABLE"; break;
			case 2: message = "MCDI_AUDIO_PROCESSING_NOT_AVAILABLE"; break;
			case 3: message = "MCDI_AUDIO_PROCESSING_INVALID"; break;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Audio Processing Status Code(Id): %s(%u)",
			message.c_str(), get_wStatus());
	}
};


	
class McdiDatabaseStatus : public MccRadioEvent  {
public:
	McdiDatabaseStatus(const ::MCC7500SDK::Parameters &param):MccRadioEvent(param) {
		event_name = "McdiDatabaseStatus";
	}
	unsigned short get_wStatus(){
		return params[1].V_UINT16();
	}
	unsigned int get_dwVersionNumber(){
		return params[2].V_UINT32();
	}
	unsigned int get_dwRestoreVersionNumber(){
		return params[3].V_UINT32();
	}
	bool isReady()
	{
		if (get_wStatus()-MCC_STANDARD_CODE == 01) return true;
		else return false;
	}
	void printMessage() {
		std::string message = "";
		switch(get_wStatus()-MCC_STANDARD_CODE) 
		{
			case 1: message = "MCDI_DB_READY"; break;
			case 2: message = "MCDI_DB_NOT_READY"; break;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database Status Code(Id): %s(%u)",
			message.c_str(), get_wStatus());
	} 
};

class McaPeripheralStatus : public MccRadioEvent  {
public:
	McaPeripheralStatus(const ::MCC7500SDK::Parameters &param):MccRadioEvent(param) {
		event_name = "McaPeripheralStatus";
	}
	unsigned short get_wStatus(){
		return params[1].V_UINT16();
	}
	bool isReady()
	{
		if (get_wStatus()-MCC_STANDARD_CODE == 02) return true;
		else return false;
	}
	void printMessage() {			
		std::string message = "";
		switch(get_wStatus()-MCC_STANDARD_CODE) 
		{
			case 0: message = "MCA_GPIOMS_NOT_AVAILABLE"; break;
			case 1: message = "MCA_GPIOMS_PARTIALLY_AVAILABLE"; break;
			case 2: message = "MCA_GPIOMS_AVAILABLE"; break;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Peripheral Status Code(Id): %s(%u)",
			message.c_str(), get_wStatus());
	}
};

class McdiDatabaseUpdateNotification : public MccRadioEvent {
public:
	static const short DB_UPDATE_TALKGROUP = 2;
	static const short DB_UNIT_ALIASES = 11;
	McdiDatabaseUpdateNotification(const ::MCC7500SDK::Parameters &param):MccRadioEvent(param) {
		event_name = "McdiDatabaseUpdateNotification";
	}
	unsigned int get_wDataGroup() {
		return params[1].V_UINT32();
	}
	void printMessage() {
		std::string message = "";
		switch (get_wDataGroup()) {
			case 0: message = "MCDB_DG_NULL"; break;
			case 1: message = "MCDB_DG_ALL"; break;
			case 2: message = "MCDB_DG_TALKGROUPS/MCDB_DG_AUDIO_RESOURCES"; break;
			case 3: message = "MCDB_DG_TALKGROUPS_CAP/MCDB_DG_RESOURCE_CAPABILITIES"; break;
			case 4: message = "MCDB_DG_TRUNKING_SYSTEMS"; break;
			case 5: message = "MCDB_DG_ZONES"; break;
			case 6: message = "MCDB_DG_SITES"; break;
			case 7: message = "MCDB_DG_AUXIOS"; break;
			case 8: message = "MCDB_DG_OPERATOR_POSITIONS"; break;
			case 9: message = "MCDB_DG_OP_CAPABILITIES"; break;
			case 10: message = "MCDB_DG_PERIPHERALS"; break;
			case 11: message = "MCDB_DG_UNIT_ALIASES"; break;
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DatabaseUpdateNotification Data Group(Id): %s(%u)",
			message.c_str(), get_wDataGroup());
	}
};

class MccProtocolHelper
{
public:
	static char * getResponseName(unsigned short val);
};

}

#endif