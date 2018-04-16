#pragma once
#include"RemoteAPI.h"

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

class IRemoteApiRadioCommand
{
	
protected:
	MCC7500SDK::Parameters in;
	MCC7500SDK::Parameters_var out;
	MCC7500SDK::UVariant_var result;
	MCC7500SDK::IAPIRequest_ptr &pAPIRequest;
	MCC7500SDK::METHOD_ID method_id;
	unsigned char clientid;
	IRemoteApiRadioCommand(MCC7500SDK::IAPIRequest_ptr &ptr,MCC7500SDK::METHOD_ID mid);
public:
	~IRemoteApiRadioCommand();
	virtual void execute();
};
class AssignResource : public IRemoteApiRadioCommand
{
public:
AssignResource(MCC7500SDK::IAPIRequest_ptr &ptr,unsigned int resource, unsigned short channel, unsigned char byClientId, unsigned char selectSpeaker, unsigned char unselectSpeaker, unsigned char byVolumeLevel, unsigned short wCriticalResource, unsigned char byHeadsetSelect) : IRemoteApiRadioCommand(ptr,eMcaAssignResourceExID)
{
in.length(8);
in[0].V_UINT32(resource);
in[1].V_UINT16(channel);
in[2].V_UINT8(byClientId);
in[3].V_UINT8(selectSpeaker);
in[4].V_UINT8(unselectSpeaker);
in[5].V_UINT8(byVolumeLevel);
in[6].V_UINT16(wCriticalResource);
in[7].V_UINT8(byHeadsetSelect);
out->release();
}
};