
#ifndef RADIO_MFT_AGENT_REMOTE_API_COMMAND_H_INCLUDED
#define RADIO_MFT_AGENT_REMOTE_API_COMMAND_H_INCLUDED

#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"
#include "app/radio/radio_mft_agent/src/IRadioMFTAgentCommand.h"
#include "app/radio/radio_mft_agent/src/RadioMFTAgentStatusManager.h"
#include "app/radio/radio_mft_agent/src/FutureManager.h"
#include "app/radio/radio_mft_agent/src/RadioApplicationStateManager.h"
#include "app/radio/radio_mft_agent/src/RadioEventDispatcher.h"
#include "bus/radio/Radio_Agent/idl/src/RadioServerAgentIDL.h"

#include "ace/Future.h"
#include "app/radio/radio_mft_agent/src/RemoteApiConnection.h"
#include "app/radio/radio_mft_agent/src/RemoteAPI.h"
#include "app/radio/radio_mft_agent/src/RemoteApiParamHolder.h"
#include "app\radio\radio_mft_agent\src\MCCRestartMngr.h"



#define MCA_INBOUND_RADIO_PVT_CALL 0x0400
#define MCA_INBOUND_CONSOLE_PVT_CALL 0x401

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

namespace RadioMFTRemoteAPI {
class IRemoteApiRadioCommand
{
	
protected:
	MCC7500SDK::Parameters in;
	MCC7500SDK::Parameters_var out;
	MCC7500SDK::UVariant_var result;
	TA_IRS_App::RemoteApiParamHolderPtr m_pRemoteApiParamHolder;
	MCC7500SDK::METHOD_ID method_id;
	unsigned char clientid;
	IRemoteApiRadioCommand(TA_IRS_App::RemoteApiParamHolderPtr ptr,MCC7500SDK::METHOD_ID mid);
public:
	~IRemoteApiRadioCommand();
	virtual void execute();
	virtual bool recurseMccCall ();
};
class AssignResource : public IRemoteApiRadioCommand
{
	public:
		AssignResource(TA_IRS_App::RemoteApiParamHolderPtr ptr,
			unsigned int m_resource, 
			unsigned short m_channel, 
			unsigned char m_client, 
			unsigned char m_select_speaker, 
			unsigned char m_unselect_speaker, 
			unsigned char m_volume, 
			unsigned short m_critical_resource, 
			unsigned char m_header) : IRemoteApiRadioCommand(ptr,eMcaAssignResourceExID)
		{
			in.length(8);
			in[0].V_UINT32(m_resource);
			in[1].V_UINT16(m_channel);
			in[2].V_UINT8(m_client);
			in[3].V_UINT8(m_select_speaker);
			in[4].V_UINT8(m_unselect_speaker);
			in[5].V_UINT8(m_volume);
			in[6].V_UINT16(m_critical_resource);
			in[7].V_UINT8(m_header);
			//out->release();
		}
		unsigned int get_mResource() {return in[0].V_UINT32();}
		unsigned short get_mChannel() {return in[1].V_UINT16();}
		unsigned char get_mClient() {return in[2].V_UINT8();}
		unsigned char get_mSelectSpeaker() {return in[3].V_UINT8();}
		unsigned char get_mUnselectSpeaker() {return in[4].V_UINT8();}
		unsigned char get_mVolume() {return in[5].V_UINT8();}
		unsigned short get_mCriticalResource() {return in[6].V_UINT16();}
		unsigned char get_mHeader() {return in[7].V_UINT8();}
};


class DeassignResource : public IRemoteApiRadioCommand
{
	public:
		DeassignResource(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned char m_client, unsigned char m_priority)  : IRemoteApiRadioCommand(ptr,eMcaDeassignResourceID)
		{
			in.length(3);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT8(m_client);
			in[2].V_UINT8(m_priority);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned char get_mClient() {return in[1].V_UINT8();}
		unsigned char get_mPriority() {return in[2].V_UINT8();}
};

class SelectResource : public IRemoteApiRadioCommand
{
	public:
		SelectResource(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel)  : IRemoteApiRadioCommand(ptr,eMcaSelectResourceID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};

class OpenMSel : public IRemoteApiRadioCommand
{
	public:
		OpenMSel(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_mSelGroup)  : IRemoteApiRadioCommand(ptr,eMcaOpenMselID)
		{
			in.length(1);
			in[0].V_UINT16(m_mSelGroup);
			//out->release();
		}
		unsigned short get_mMselGroup() {return in[0].V_UINT16();}
};

class BeginRefresh : public IRemoteApiRadioCommand
{
	public:
		BeginRefresh(TA_IRS_App::RemoteApiParamHolderPtr ptr)  : IRemoteApiRadioCommand(ptr,eMcaBeginRefreshID)
		{
			in.length(0);
		}
};

class DeselectResource : public IRemoteApiRadioCommand
{
	public:
		DeselectResource(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel)  : IRemoteApiRadioCommand(ptr,eMcaDeselectResourceID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};

class ChangeVolume : public IRemoteApiRadioCommand
{
	public:
		ChangeVolume(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned char m_volume)  : IRemoteApiRadioCommand(ptr,eMcaChangeVolumeID)
		{
			in.length(2);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT8(m_volume);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned char get_mVolume() {return in[1].V_UINT8();}
};


class AddPatchMember : public IRemoteApiRadioCommand
{
	public:
		AddPatchMember(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_group, unsigned short m_channel)  : IRemoteApiRadioCommand(ptr,eMcaAddPatchMemberID)
		{
			in.length(2);
			in[0].V_UINT16(m_group);
			in[1].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mGroup() {return in[0].V_UINT16();}
		unsigned short get_mChannel() {return in[1].V_UINT16();}
};

class DeletePatchMember : public IRemoteApiRadioCommand
{
	public:
		DeletePatchMember(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel) : IRemoteApiRadioCommand(ptr,eMcaDeletePatchMemberID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};

class GetFreeMselGroup : public IRemoteApiRadioCommand
{
	public:
		GetFreeMselGroup(TA_IRS_App::RemoteApiParamHolderPtr ptr)  : IRemoteApiRadioCommand(ptr,eMcaGetFreeMselGroupID)
		{
			in.length(0);
			//out->release();
		}
};

class AddMselMember : public IRemoteApiRadioCommand
{
	public:
		AddMselMember (TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_msel_group, unsigned short m_channel) : IRemoteApiRadioCommand(ptr, eMcaAddMselMemberID)
		{
			in.length(2);
			in[0].V_UINT16(m_msel_group);
			in[1].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mMselGroup() {return in[0].V_UINT16();}
		unsigned short get_mChannel() {return in[1].V_UINT16();}
};

class CloseMselMember : public IRemoteApiRadioCommand
{
	public:
		CloseMselMember (TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_msel_group) : IRemoteApiRadioCommand(ptr, eMcaCloseMselID)
		{
			in.length(1);
			in[0].V_UINT16(m_msel_group);
			//out->release();
		}
		unsigned short get_mMselGroup() {return in[0].V_UINT16();}		
};


class DeleteMselMember : public IRemoteApiRadioCommand
{
	public:
		DeleteMselMember(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_msel_group, unsigned short m_channel)	: IRemoteApiRadioCommand(ptr,eMcaDeleteMselMemberID)
		{
			in.length(2);
			in[0].V_UINT16(m_msel_group);
			in[1].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mMselGroup() {return in[0].V_UINT16();}
		unsigned short get_mChannel() {return in[1].V_UINT16();}
};


class BeginPrivateCall : public IRemoteApiRadioCommand
{
	public:
		BeginPrivateCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned int m_unit_id, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaBeginPrivateCallID)
		{
			in.length(3);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT32(m_unit_id);
			in[2].V_UINT8(m_client);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned int get_mUnitId() {return in[1].V_UINT32();}
		unsigned char get_mClient() {return in[2].V_UINT8();}
};


class AnswerPrivateCall : public IRemoteApiRadioCommand
{
	public:
		AnswerPrivateCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned int m_unit_id, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaAnswerPrivateCallID)
		{
			in.length(3);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT32(m_unit_id);
			in[2].V_UINT8(m_client);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned int get_mUnitId() {return in[1].V_UINT32();}
		unsigned char get_mClient() {return in[2].V_UINT8();}
};

class EndPrivateCall : public IRemoteApiRadioCommand
{
	public:
		EndPrivateCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel)	: IRemoteApiRadioCommand(ptr,eMcaEndPrivateCallID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};


class RejectPrivateCall : public IRemoteApiRadioCommand
{
	public:
		RejectPrivateCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned int m_unit_id, unsigned char m_client, unsigned int m_retry_id)  : IRemoteApiRadioCommand(ptr,eMcaRejectPrivateCallID)
		{
			in.length(4);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT32(m_unit_id);
			in[2].V_UINT8(m_client);
			in[3].V_UINT32(m_retry_id);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned int get_mUnitId() {return in[1].V_UINT32();}
		unsigned char get_mClient() {return in[2].V_UINT8();}
		unsigned int get_mRetryId() {return in[3].V_UINT32();}
};

class BeginConsoleEmergencyCall : public IRemoteApiRadioCommand
{
	public:
		BeginConsoleEmergencyCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaBeginConsoleEmergencyCallID)
		{
			in.length(2);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT8(m_client);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned char get_mClient() {return in[1].V_UINT8();}
};

class EndEmergency : public IRemoteApiRadioCommand
{
	public:
		EndEmergency(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel)  : IRemoteApiRadioCommand(ptr,eMcaEndEmergencyID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};

class AcknowledgeEmergency : public IRemoteApiRadioCommand
{
	public:
		AcknowledgeEmergency(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned int m_unit_id)  : IRemoteApiRadioCommand(ptr,eMcaAcknowledgeEmergencyID)
		{
			in.length(2);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT32(m_unit_id);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned int get_mUnitId() {return in[1].V_UINT32();}
};

class BeginAmbienceListeningCall : public IRemoteApiRadioCommand
{
	public:
		BeginAmbienceListeningCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned int m_unit_id, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaBeginAmbienceListeningCallID)
		{
			in.length(3);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT32(m_unit_id);
			in[2].V_UINT8(m_client);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned int get_mUnitId() {return in[1].V_UINT32();}
		unsigned char get_mClient() {return in[2].V_UINT8();}
};

class EndAmbienceListeningCall : public IRemoteApiRadioCommand
{
	public:
		EndAmbienceListeningCall(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel)  : IRemoteApiRadioCommand(ptr,eMcaEndAmbienceListeningCallID)
		{
			in.length(1);
			in[0].V_UINT16(m_channel);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
};

class BeginGeneralTransmit : public IRemoteApiRadioCommand
{
	public:
		BeginGeneralTransmit(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaBeginGeneralTransmitID)
		{
			in.length(1);
			in[0].V_UINT8(m_client);
			//out->release();
		}
		unsigned char get_mClient() {return in[0].V_UINT8();}
};

class EndGeneralTransmit : public IRemoteApiRadioCommand
{
	public:
		EndGeneralTransmit(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned char m_client)  : IRemoteApiRadioCommand(ptr,eMcaEndGeneralTransmitExID)
		{
			in.length(1);
			in[0].V_UINT8(m_client);
			//out->release();
		}
		unsigned char get_mClient() {return in[0].V_UINT8();}
};

class AssignSelectSpeaker : public IRemoteApiRadioCommand
{
	public:
		AssignSelectSpeaker(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned short m_channel, unsigned char m_select_speaker, unsigned char m_unselect_speaker, unsigned char m_header)  : IRemoteApiRadioCommand(ptr,eMcaAssignAudioDestinationExID)
		{
			in.length(4);
			in[0].V_UINT16(m_channel);
			in[1].V_UINT8(m_select_speaker);
			in[2].V_UINT8(m_unselect_speaker);
			in[3].V_UINT8(m_header);
			//out->release();
		}
		unsigned short get_mChannel() {return in[0].V_UINT16();}
		unsigned char get_mSelectSpeaker() {return in[1].V_UINT8();}
		unsigned char get_mUnselectSpeaker() {return in[2].V_UINT8();}
		unsigned char get_mHeader() {return in[3].V_UINT8();}
};

class GetFreePatchGroup : public IRemoteApiRadioCommand
{
	public:
		GetFreePatchGroup(TA_IRS_App::RemoteApiParamHolderPtr ptr)  : IRemoteApiRadioCommand(ptr,eMcaGetFreePatchGroupID)
		{
			in.length(0);
			//out->release();
		}
};



class BeginTone : public IRemoteApiRadioCommand
{
	public:
		BeginTone(TA_IRS_App::RemoteApiParamHolderPtr ptr, unsigned int tone, unsigned char clientId)  : IRemoteApiRadioCommand(ptr,eMcaBeginToneID)
		{
			in.length(2);
			in[0].V_UINT32(tone);
			in[1].V_UINT8(clientId);
			//out->release();
		}
};

class EndTone : public IRemoteApiRadioCommand
{
	public:
		EndTone(TA_IRS_App::RemoteApiParamHolderPtr ptr)  : IRemoteApiRadioCommand(ptr,eMcaEndToneID)
		{
			in.length(0);
			//out->release();
		}
};

class DatabaseUpdateGrant : public IRemoteApiRadioCommand
{
	public:
		DatabaseUpdateGrant(TA_IRS_App::RemoteApiParamHolderPtr ptr) : IRemoteApiRadioCommand(ptr,eMcdiDatabaseUpdateGrantID)
		{
			in.length(0);
		}
		bool getStatus()
		{
			return result->V_BOOLEAN();
		}
};

class DatabaseUpdateNoGrant : public IRemoteApiRadioCommand
{
	public:
		DatabaseUpdateNoGrant(TA_IRS_App::RemoteApiParamHolderPtr ptr) : IRemoteApiRadioCommand(ptr,eMcdiDatabaseUpdateNoGrantID)
		{
			in.length(0);
		}
		bool getStatus()
		{
			return result->V_BOOLEAN();
		}
};


	class AssignLoggingResource : public IRemoteApiRadioCommand
	{
		public:
			AssignLoggingResource(TA_IRS_App::RemoteApiParamHolderPtr ptr,
								  unsigned int lowUnitId,
								  unsigned int highUnitId,
								  unsigned int lClientId) : IRemoteApiRadioCommand(ptr,eMcaAssignIndividualLoggingResourceID)
			{
				in.length(7);
				in[0].V_UINT32(lowUnitId);
				in[1].V_UINT32(highUnitId);
				in[2].V_UINT16(lClientId);
				in[3].V_UINT32(0); // NULL_IP_ADDR
				in[4].V_UINT16(2);
				in[5].V_UINT16(0);
				in[6].V_UINT16(1);

				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
							"AssignLoggingResource::AssignLoggingResource");
			}

	};

}

class RadioMFTAgentRemoteAPICommandImpl : public IRadioMFTAgentCommand
{
	FutureManager m_pFutureManager;
	RadioEventDispatcher m_pEventDispatcher;

	TA_IRS_App::RemoteApiParamHolder m_paramHolder;
	TA_IRS_App::MCCRestartMngr * m_restartMngr;

	public:

		RadioMFTAgentRemoteAPICommandImpl(string ior,string username,string password);

		virtual ~RadioMFTAgentRemoteAPICommandImpl();


	public:

		/**
		 * Returns a collection of RadioResource's that are currently associated with the
		 * CIE.
		 * 
		 * @param type is optional, if not specified (or set to RR_NILL), then all
		 * RadioResourceType's are returned.
		 * 
		 * @return indicates successful execution of the request
		 * @exception ...
		 */
		virtual std::vector<RadioResource> getResourceCollection(RadioResourceType type);

		/**
		 * Attempts to associate the requested resource with the unselect speaker of the
		 * CIE. This implies that the resource is being passively monitored (ie. the
		 * operator can only listen, they cannot communicate to the resource).
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void assignResource(RadioResource& resource);

		/**
		 * Attempts to break the association between the requested resource and the CIE.
		 * This causes the resource to be removed from the unselect speaker, and no longer
		 * being monitored by the CIE, and it ceases to send events related to the
		 * resource.
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void deassignResource(RadioResource& resource,LogicalChannel channel =0);

		/**
		 * Attempts to associate the requested resource with the select speaker of the CIE.
		 * This implies that the resource is being actively monitored (ie. the operator
		 * can both listen and communicate to the resource).
		 * 
		 * @return indicates successful execution of the request
		 * @exception InvalidParameterException
		 */
		virtual void selectResource(RadioResource& resource);
		virtual void selectAG(RadioResource& resource,CommandState state=START);

		virtual void deselectResource(RadioResource& resource,LogicalChannel channel =0);

		/**
		 * @note speaker is redundant, as the volume change message will only affect the
		 * volume level of the where the resource is currently single-selected.
		 */
		virtual void changeVolume(RadioResource& resource, unsigned int volume);
		
		virtual void connectPatchGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void assignPatchMember(RadioResource& resource);
		virtual void releasePatchGroup(RadioResource& resource);
		virtual void deassignPatchMember(RadioResource& resource);
		virtual void deletePatchMember(RadioResource& resource);

		virtual void connectMselGroup(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void assignMselMember(RadioResource& resource);
		virtual void releaseMselGroup(RadioResource& resource,bool bSolicited);
		virtual void deassignMselMember(RadioResource& resource);

		virtual void connectDynRegroup(RadioResource& resource);
		virtual	void releaseDynRegroup(RadioResource& resource);

		virtual void makePrivateCall(RadioResource& resource,CommandState state=START);
		virtual void answerPrivateCall(RadioResource& resource,CommandState state=START);
		virtual void endPrivateCall(RadioResource& resource);

// ++ Huangxin TD14319 
		virtual void cancelPrivateCall(RadioResource& resource,PrivateCallState ptCallState);
// TD14319++

		virtual void makeEmergencyCall(RadioResource& resource);
		virtual void endEmergencyCall(RadioResource& resource);
		virtual void ackEmergency(RadioResource& resource);

		virtual void makeAmbienceListeningCall(RadioResource& resource);
		virtual void endAmbienceListeningCall(RadioResource& resource);

		virtual void connectSite(RadioResource& resource,CommandState state=START);
		virtual void releaseSite(RadioResource& resource);

		virtual void connectMselSite(RadioResource& resource,CommandState state=START,unsigned int grpNo=0);
		virtual void releaseMselSite(RadioResource& resource);

		virtual void generalTransmit(CommandState state);

		virtual void assignSelectSpeaker(RadioResource& resource); 
		
		virtual void callback_initialisation();
		virtual void callback_termination();

		virtual void beginTone(unsigned int tone);
		virtual void endTone();

		FutureManager* getFutureManager() 
		{ 
			return &m_pFutureManager; 
		};

		virtual void setApStateMgr(RadioApplicationStateManager* p) { m_pApStateMgr = p; };

		virtual RadioApplicationStateManager* getApStateMgr() { return m_pApStateMgr; };
		

		/**
		 * To register the Radio Manager (or other client application) with the Radio MFT
		 * Agent, to enable the client application to receive update notifications.
		 * 
		 * @note Since the MFT Radio Agent is tied to a single MFT and CIE, it is more
		 * efficient to bypass transActive Notification Service, and keep the
		 * communications local to the MFT.
		 * 
		 * @return indicates successful execution of the request
		 * @exception...
		 */
		virtual bool registerRadioMFTAgentCallback(TA_IRS_Bus::RadioMFTAgentCallbackIDL_ptr pCallback);

		/**
		 * Create a temporary Patch Group for patching Radio-DLTS
		 * @return 
		 * @exception...
		 */	
		virtual void createPatchGroup(RadioResource& resource, bool bIsDLTSPatch=true);

		/**
		 * stop and start the connection to the radio system
		 */
		virtual void restartRemoteApiConnection();

		/**
		 * to get Status update from MCC
		 */
		virtual void beginRefresh();

		/**
		 * Dial a phone Resource
		 * @return 
		 * @exception...
		 */	
		/*virtual void makePhoneCall(std::string number,CommandState state=START);

		/**
		 * End call to a phone resource
		 * @return 
		 * @exception...
		 */	
		/*virtual void endPhoneCall(std::string number);*/

		virtual void updateDbGrant(bool grant = true);

		virtual void broadcastTrain(RadioResource& resource, CommandState state=START);

		void addNextPatchMember(RadioResource& resource, ResourceIdentifier iscsGrpId, unsigned int mccGrpId);
		void addNextMselMember(RadioResource& resource, ResourceIdentifier iscsGrpId, unsigned int mccGrpId);
		
	private:

		//Disabled Methods
		RadioMFTAgentRemoteAPICommandImpl(const RadioMFTAgentRemoteAPICommandImpl&);
		RadioMFTAgentRemoteAPICommandImpl& operator=(const RadioMFTAgentRemoteAPICommandImpl&);


		void addPatchMember(RadioResource& resource, unsigned int grpNum);
		
		void addMselMember(RadioResource& resource, unsigned int grpNum);
		void connectTempPG(RadioResource& resource, ResourceCollection& members);

		/*MCC7500SDK::IAPIRequest_ptr& getApiRequest(){
			return IAPIRequest;
		}*/
	private:
		
		TA_Base_Core::NamedObject<TA_IRS_Bus::RadioServerAgentIDL,
							  TA_IRS_Bus::RadioServerAgentIDL_ptr,
					  		  TA_IRS_Bus::RadioServerAgentIDL_var> m_serverAgent;

		RadioApplicationStateManager* m_pApStateMgr;

		//MCC7500SDK::IAPIRequest_ptr IAPIRequest;
		std::string m_username;
		std::string m_password;
		std::string m_ior;

		void checkApState();
		
		static const int CONNECTION_LIMIT; // Fixed TD14320

	private:
		
		void auditAckEmergency (RadioResource& resource);
};
#endif // !defined({8563EF6E-5CB7-4d54-A82C-3D6F4484166F}__INCLUDED_)
