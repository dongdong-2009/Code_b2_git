#include "app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"
#include "core/utilities/src/DebugUtil.h"
CallBackMessage RemoteAPICallBackMessage::responses[CALLBACK_MESSAGES_NUM]=
{
/* MCDI messages */
    {"eMsgMcdiAudioProcessingStatusID",0x5000},

    {"eMsgMcdiAuthenticationErrorID",0x5010},
    {"eMsgMcdiAuthenticationWarningID",0x5011},
    {"eMsgMcdiAuthenticationStatusID",0x5012},

    {"eMsgMcdiConsoleShutDownID",0x5020},

    {"eMsgMcdiActivateClientID",0x5030},
    {"eMsgMcdiDeactivateClientID",0x5031},
    {"eMsgMcdiClientActivationStatusID",0x5032},

    {"eMsgMcdiMessageLossID",0x5040},

    {"eMsgMcdiClientRegistrationStatusID",0x5050},
    {"eMsgMcdiRegistrationStatusRequestID",0x5051},

    {"eMsgMcdiNewDatabaseGrantedID",0x5060},
    {"eMsgMcdiNewDatabaseNotGrantedID",0x5061},
    {"eMsgMcdiNewDatabaseAvailableID",0x5062},
    {"eMsgMcdiDatabaseUpdateNotificationID",0x5063},
    {"eMsgMcdiDatabaseStatusID",0x5064},


    /* MCA messages */
    {"eMsgMcaBeginAllMuteErrorID",0x1000},
    {"eMsgMcaEndAllMuteErrorID",0x1001},
    {"eMsgMcaAllMuteStatusID",0x1002},

    {"eMsgMcaBeginAmbienceListeningCallErrorID",0x1010},
    {"eMsgMcaEndAmbienceListeningCallErrorID",0x1011},
    {"eMsgMcaAmbienceListeningCallStatusID",0x1012},

    {"eMsgMcaBeginAPBTransmitWarningID",0x1020},
    {"eMsgMcaBeginAPBTransmitErrorID",0x1021},
    {"eMsgMcaEndAPBTransmitErrorID",0x1022},
    {"eMsgMcaAPBTransmitStatusID",0x1023},

    {"eMsgMcaAssignAudioDestinationWarningID",0x1030},
    {"eMsgMcaAssignAudioDestinationErrorID",0x1031},
    {"eMsgMcaAudioDestinationStatusID",0x1032},

    {"eMsgMcaAssignAuxioErrorID",0x1040},
    {"eMsgMcaDeassignAuxioErrorID",0x1041},
    {"eMsgMcaAuxioAssignmentStatusID",0x1042},

    {"eMsgMcaActivateAuxioErrorID",0x1050},
    {"eMsgMcaDeactivateAuxioErrorID",0x1051},
    {"eMsgMcaClearAuxioStatusErrorID",0x1052},
    {"eMsgMcaAuxioStatusID",0x1053},

    {"eMsgMcaSoundBeepsErrorID",0x1060},

    {"eMsgMcaSendCallAlertErrorID",0x1070},
    {"eMsgMcaAbortCallAlertErrorID",0x1071},
    {"eMsgMcaCallAlertStatusID",0x1072},
    {"eMsgMcaInboundCallAlertStatusID",0x1073},

    {"eMsgMcaSelectCriticalResourceSettingErrorID",0x1080},
    {"eMsgMcaCriticalResourceStatusID",0x1081},

    {"eMsgMcaDatabaseUpdateNotificationStatusID",0x1090},

    {"eMsgMcaAcknowledgeEmergencyErrorID",0x10A0},
    {"eMsgMcaEmergencyAlarmStatusID",0x10A1},

    {"eMsgMcaBeginConsoleEmergencyCallErrorID",0x10B0},
    {"eMsgMcaEndEmergencyErrorID",0x10B1},
    {"eMsgMcaEmergencyCallStatusID",0x10B2},

    {"eMsgMcaSelectFrequencyErrorID",0x10C0},
    {"eMsgMcaResourceFrequencyStatusID",0x10C1},

    {"eMsgMcaBeginGeneralTransmitWarningID",0x10D0},
    {"eMsgMcaBeginGeneralTransmitErrorID",0x10D1},
    {"eMsgMcaEndGeneralTransmitErrorID",0x10D2},
    {"eMsgMcaGeneralTransmitStatusID",0x10D3},

    {"eMsgMcaInboundCallStatusID",0x10E0},

    {"eMsgMcaInboundPttIdStatusID",0x10F0},

    {"eMsgMcaBeginInstantTransmitErrorID",0x1100},
    {"eMsgMcaEndInstantTransmitErrorID",0x1101},

    {"eMsgMcaSelectKeysetErrorID",0x1110},
    {"eMsgMcaKeysetStatusID",0x1111},

    {"eMsgMcaAssignLoggingAudioDestinationErrorID",0x1120},
    {"eMsgMcaLoggingAudioDestinationStatusID",0x1121},

    {"eMsgMcaLoggingGroupCallStatusID",0x1130},
    {"eMsgMcaLoggingPrivateCallEventStatusID",0x1131},
    {"eMsgMcaLoggingPrivateCallStatusID",0x1132},
    {"eMsgMcaLoggingTelephoneCallEventStatusID",0x1133},
    {"eMsgMcaLoggingTelephoneCallStatusID",0x1134},

    {"eMsgMcaAssignLoggingResourceWarningID",0x1140},
    {"eMsgMcaAssignLoggingResourceErrorID",0x1141},
    {"eMsgMcaDeassignLoggingResourceWarningID",0x1142},
    {"eMsgMcaDeassignLoggingResourceErrorID",0x1143},
    {"eMsgMcaLoggingAssignmentStatusID",0x1144},
    {"eMsgMcaAssignIndividualLoggingResourceWarningID",0x1145},
    {"eMsgMcaAssignIndividualLoggingResourceErrorID",0x1146},
    {"eMsgMcaDeassignIndividualLoggingResourceWarningID",0x1147},
    {"eMsgMcaDeassignIndividualLoggingResourceErrorID",0x1148},
    {"eMsgMcaLoggingIndividualAssignmentStatusID",0x1149},

    {"eMsgMcaBeginMonitorErrorID",0x1150},
    {"eMsgMcaEndMonitorErrorID",0x1151},
    {"eMsgMcaMonitorStatusID",0x1152},

    {"eMsgMcaOpenMselErrorID",0x1160},
    {"eMsgMcaCloseMselErrorID",0x1161},
    {"eMsgMcaMselGroupStatusID",0x1162},
    {"eMsgMcaAddMselMemberWarningID",0x1163},
    {"eMsgMcaAddMselMemberErrorID",0x1164},
    {"eMsgMcaDeleteMselMemberErrorID",0x1165},
    {"eMsgMcaMselMemberStatusID",0x1166},
    {"eMsgMcaMselMemberRefreshID",0x1167},
    {"eMsgMcaGetFreeMselGroupErrorID",0x1168},
    {"eMsgMcaFreeMselGroupStatusID",0x1169},

    {"eMsgMcaBeginMuteSecondReceiverErrorID",0x1170},
    {"eMsgMcaEndMuteSecondReceiverErrorID",0x1171},
    {"eMsgMcaMuteSecondReceiverStatusID",0x1172},

    {"eMsgMcaBeginPagePttWarningID",0x1180},
    {"eMsgMcaBeginPagePttErrorID",0x1181},
    {"eMsgMcaEndPagePttErrorID",0x1182},
    {"eMsgMcaPagePttStatusID",0x1183},

    {"eMsgMcaAddPatchMemberWarningID",0x1190},
    {"eMsgMcaAddPatchMemberErrorID",0x1191},
    {"eMsgMcaDeletePatchMemberErrorID",0x1192},
    {"eMsgMcaPatchMemberStatusID",0x1193},
    {"eMsgMcaPatchGroupStatusID",0x1194},
    {"eMsgMcaPatchMemberRefreshID",0x1195},
    {"eMsgMcaGetFreePatchGroupErrorID",0x1196},
    {"eMsgMcaFreePatchGroupStatusID",0x1197},

    {"eMsgMcaBeginPatchTransmitWarningID",0x11A0},
    {"eMsgMcaBeginPatchTransmitErrorID",0x11A1},
    {"eMsgMcaEndPatchTransmitErrorID",0x11A2},
    {"eMsgMcaPatchTransmitStatusID",0x11A3},

    {"eMsgMcaPeripheralsStatusID",0x11B0},

    {"eMsgMcaAnswerPrivateCallErrorID",0x11C0},
    {"eMsgMcaBeginPrivateCallErrorID",0x11C1},
    {"eMsgMcaEndPrivateCallErrorID",0x11C2},
    {"eMsgMcaInboundPrivateCallStatusID",0x11C3},
    {"eMsgMcaPrivateCallStatusID",0x11C4},
    {"eMsgMcaRejectPrivateCallErrorID",0x11C5},

    {"eMsgMcaSelectPrivateLineErrorID",0x11D0},
    {"eMsgMcaResourcePrivateLineStatusID",0x11D1},

    {"eMsgMcaInboundRadioStatusID",0x11E0},

    {"eMsgMcaRefreshStatusID",0x11F0},

    {"eMsgMcaEnableRepeatErrorID",0x1200},
    {"eMsgMcaDisableRepeatErrorID",0x1201},
    {"eMsgMcaRepeatStatusID",0x1202},

    {"eMsgMcaAssignResourceWarningID",0x1210},
    {"eMsgMcaAssignResourceErrorID",0x1211},
    {"eMsgMcaDeassignResourceWarningID",0x1212},
    {"eMsgMcaDeassignResourceErrorID",0x1213},
    {"eMsgMcaAssignmentStatusID",0x1214},

    {"eMsgMcaResourceRejectStatusID",0x1220},

    {"eMsgMcaResourceStatusID",0x1230},

    {"eMsgMcaResourceTransmitStatusID",0x1240},

    {"eMsgMcaSelectResourceErrorID",0x1250},
    {"eMsgMcaDeselectResourceErrorID",0x1251},
    {"eMsgMcaResourceSelectStatusID",0x1252},

    {"eMsgMcaSelectPriorityErrorID",0x1260},
    {"eMsgMcaPriorityStatusID",0x1261},

    {"eMsgMcaBeginTakeoverErrorID",0x1270},
    {"eMsgMcaEndTakeoverErrorID",0x1271},
    {"eMsgMcaTakeoverStatusID",0x1272},

    {"eMsgMcaBeginToneErrorID",0x1280},
    {"eMsgMcaEndToneErrorID",0x1281},
    {"eMsgMcaToneStatusID",0x1282},

    {"eMsgMcaSelectTransmitModeErrorID",0x1290},
    {"eMsgMcaSelectTransmitModeStatusID",0x1291},

    {"eMsgMcaTrunkingSystemStatusID",0x12A0},

    {"eMsgMcaChangeVolumeWarningID",0x12B0},
    {"eMsgMcaChangeVolumeErrorID",0x12B1},
    {"eMsgMcaResourceVolumeStatusID",0x12B2},

    {"eMsgMcaActivateWildcardErrorID",0x12C0},
    {"eMsgMcaDeactivateWildcardErrorID",0x12C1},
	{"eMsgMcaWildcardStatusID",0x12C2}
};

CallBackMessage RemoteAPICallBackMessage::commands[COMMANDS_NUM] = 
{
	/* MCA methods */
	{"eMcaBeginAllMuteID",                                        1001},
	{"eMcaEndAllMuteID",                                          1002},

	{"eMcaBeginAmbienceListeningCallID",                          1010},
	{"eMcaEndAmbienceListeningCallID",                            1011},

	{"eMcaBeginAPBTransmitID",                                    1020},
	{"eMcaEndAPBTransmitID",                                      1021},

	{"eMcaAssignAudioDestinationExID",                            1030},

	{"eMcaAssignAuxioID",                                         1040},
	{"eMcaDeassignAuxioID",                                       1041},

	{"eMcaActivateAuxioID",                                       1050},
	{"eMcaClearAuxioStatusID",                                    1051},
	{"eMcaDeactivateAuxioID",                                     1052},

	{"eMcaSoundBeepsID",                                          1060},

	{"eMcaAbortCallAlertID",                                      1070},
	{"eMcaSendCallAlertID",                                       1071},

	{"eMcaSelectCriticalResourceSettingID",                       1080},

	{"eMcaEndEmergencyID",                                        1090},
	{"eMcaAcknowledgeEmergencyID",                                1091},
	{"eMcaBeginConsoleEmergencyCallID",                           1092},

	{"eMcaSelectFrequencyID",                                     1100},

	{"eMcaBeginGeneralTransmitID",                                1110},
	{"eMcaEndGeneralTransmitExID",                                1111},
	{"eMcaBeginInstantTransmitID",                                1112},
	{"eMcaEndInstantTransmitID",                                  1113},

	{"eMcaSelectKeysetID",                                        1120},

	{"eMcaAssignLoggingAudioDestinationID",                       1130},
	{"eMcaAssignLoggingResourceID",                               1131},
	{"eMcaDeassignLoggingResourceID",                             1132},

	{"eMcaAssignIndividualLoggingResourceID",                     1135},
	{"eMcaDeassignIndividualLoggingResourceID",                   1136},

	{"eMcaBeginMonitorID",                                        1140},
	{"eMcaEndMonitorExID",                                        1141},

	{"eMcaAddMselMemberID",                                       1150},
	{"eMcaCloseMselID",                                           1151},
	{"eMcaDeleteMselMemberID",                                    1152},
	{"eMcaGetFreeMselGroupID",                                    1153},
	{"eMcaOpenMselID",                                            1154},

	{"eMcaBeginMuteSecondReceiverID",                             1160},
	{"eMcaEndMuteSecondReceiverID",                               1161},

	{"eMcaBeginPagePttID",                                        1170},
	{"eMcaEndPagePttExID",                                        1171},

	{"eMcaAddPatchMemberID",                                      1180},
	{"eMcaDeletePatchMemberID",                                   1181},
	{"eMcaGetFreePatchGroupID",                                   1182},

	{"eMcaBeginPatchTransmitID",                                  1190},
	{"eMcaEndPatchTransmitID",                                    1191},

	{"eMcaAnswerPrivateCallID",                                   1200},
	{"eMcaBeginInterruptPriorityPrivateCallID",                   1201},
	{"eMcaBeginPrivateCallID",                                    1202},
	{"eMcaEndPrivateCallID",                                      1203},
	{"eMcaSelectPrivateLineID",                                   1204},
	{"eMcaRejectPrivateCallID",                                   1205},

	{"eMcaBeginRefreshID",                                        1210},

	{"eMcaDisableRepeatID",                                       1220},
	{"eMcaEnableRepeatID",                                        1221},

	{"eMcaAssignResourceExID",                                    1230},
	{"eMcaDeassignResourceID",                                    1231},
	{"eMcaDeselectResourceID",                                    1232},

	{"eMcaSelectResourceID",                                      1240},
	{"eMcaSelectPriorityID",                                      1241},

	{"eMcaBeginTakeoverID",                                       1250},
	{"eMcaEndTakeoverID",                                         1251},

	{"eMcaBeginToneID",                                           1260},
	{"eMcaEndToneID",                                             1261},

	{"eMcaSelectTransmitModeID",                                  1270},

	{"eMcaChangeVolumeID",                                        1280},

	{"eMcaActivateWildcardID",                                    1290},
	{"eMcaDeactivateWildcardID",                                  1291},

	{"eMcaAssignResourceID",                                      1300},



	/* MCDI methods */
	{"eMcdiGetAudioProcessingStatusID",                           2000},
	{"eMcdiGetAuthenticationStatusID",                            2001},
	{"eMcdiGetMessageTransmitErrorID",                            2002},
	{"eMcdiDatabaseUpdateGrantID",                                2003},
	{"eMcdiDatabaseUpdateNoGrantID",                              2004},
	{"eMcdiGetDatabaseStatusID",                                  2005},


	/* MCDB methods */
	{"eMcdbCreateTalkChannelQueryID",                             3000},
	{"eMcdbCreateTalkChannelQueryByVersionID",                    3001},
	{"eMcdbCreateTalkChannelWithCapProfileMapQueryID",            3002},

	{"eMcdbGetNextTalkChannelID",                                 3010},
	{"eMcdbGetNextTalkChannelByVersionID",                        3011},
	{"eMcdbGetNextTalkChannelWithCapProfileMapID",                3012},
	{"eMcdbCreateTalkChannelWithCapProfileMapQueryByVersionID",   3013},
	{"eMcdbGetNextTalkChannelWithCapProfileMapByVersionID",       3014},
	{"eMcdbGetTalkChannelInfoID",                                 3015},
	{"eMcdbGetNextTalkChannelsID",                                3016},
	{"eMcdbGetNextTalkChannelsByVersionID",                       3017},
	{"eMcdbGetNextTalkChannelsWithCapProfileMapID",               3018},
	{"eMcdbGetNextTalkChannelsWithCapProfileMapByVersionID",      3019},

	{"eMcdbCreateAuxioQueryID",                                   3020},
	{"eMcdbGetAuxioInfoID",                                       3021},
	{"eMcdbGetNextAuxioID",                                       3022},

	{"eMcdbOpenDatabaseExID",                                     3034},
	{"eMcdbCloseDatabaseID",                                      3035},

	{"eMcdbCreateOpCapabilityQueryByVersionID",                   3040},
	{"eMcdbGetNextOpCapabilityByVersionID",                       3041},
	{"eMcdbCreateOpCapabilityIndexQueryID",                       3042},
	{"eMcdbGetNextOpCapabilityIndexRecID",                        3043},
	{"eMcdbCheckOpCapabilityIdID",                                3044},
	{"eMcdbDestroyQueryID",                                       3045},

	{"eMcdbCreateOpPosQueryID",                                   3050},
	{"eMcdbGetMaxMC_LC_IDID",                                        3051},
	{"eMcdbGetNextOpPosID",                                       3052},
	{"eMcdbGetOpIdID",                                            3053},
	{"eMcdbGetLoggingCapacityID",                                 3054},


	{"eMcdbCreateAllResCapabilityByTalkResTypeQueryID",           3060},
	{"eMcdbGetNextAllResCapabilityByTalkResTypeID",               3061},
	{"eMcdbCreateResCapabilityQueryID",                           3062},
	{"eMcdbCreateProfileCapabilityQueryID",                       3063},
	{"eMcdbCreateProfileCapabilityQueryByVersionID",              3064},
	{"eMcdbGetNextResourceCapabilityID",                          3065},
	{"eMcdbCreateResCapabilityQueryByVersionID",                  3066},
	{"eMcdbGetNextResourceCapabilityByVersionID",                 3067},
	{"eMcdbCreateResCapabilityIndexQueryID",                      3068},
	{"eMcdbGetNextResCapabilityIndexRecID",                       3069},
	{"eMcdbGetNextProfileCapabilityID",                           3070},
	{"eMcdbGetNextProfileCapabilityByVersionID",                  3071},
	{"eMcdbCheckChannelCapabilityIdID",                           3072},
	{"eMcdbGetResCapabilityLabelID",                              3073},

	{"eMcdbCreateSiteByZoneQueryByVersionID",                     3080},
	{"eMcdbGetNextSiteByZoneByVersionID",                         3081},
	{"eMcdbCreateZoneQueryID",                                    3082},
	{"eMcdbGetNextZoneID",                                        3083},

	{"eMcdbCreateSpeakerQueryID",                                 3090},
	{"eMcdbGetNextSpeakerID",                                     3091},
	{"eMcdbGetSpeakerLabelID",                                    3092},

	{"eMcdbCreateTrkResQueryID",                                  3100},
	{"eMcdbGetNextTrkResRecID",                                   3101},

	{"eMcdbCreateTrunkSysQueryID",                                3110},
	{"eMcdbGetNextTrunkSysID",                                    3111},   
	{"eMcdbGetTrunkSysInfoID",                                    3112},

	{"eMcdbCreateAuxioQueryByVersionID",                          3120},
	{"eMcdbGetNextAuxioByVersionID",                              3121},
	{"eMcdbCreateSecureKeysetQueryID",                            3122},
	{"eMcdbGetNextSecureKeysetID",                                3123},
	{"eMcdbGetSecureKeysetAliasID",                               3124},
	{"eMcdbCreateUnitAliasListQueryID",                           3125},
	{"eMcdbGetActiveSecureKeysetID",                              3126},
	{"eMcdbGetGivenUnitAliasID",                                  3127},
	{"eMcdbGetNextUnitAliasID",                                   3128},
	{"eMcdbGetUnitIDID",                                          3129},
	{"eMcdbGetFirstUnitAliasID",                                  3130},

	{"eMcdbCreateLoggingUnitAliasListQueryByVersionID",           3131},
	{"eMcdbGetNextUnitAliasByVersionID",                          3132},
	{"eMcdbGetNextUnitAliasesByVersionID",                        3133},

	{"eMcdbGetNextUnitAliasesID",                                 3134}, 

	{"eMcdbCreateOpCapabilityQueryID",                            3140},
	{"eMcdbGetNextOpCapabilityID",                                3141},

	{"eMcdbCreateSiteByZoneQueryID",                              3150},
	{"eMcdbGetNextSiteByZoneID",                                  3151},

	{"eMcdbCreateResCapProfileMapQueryID",                        3160},
	{"eMcdbGetNextResCapProfileMapID",                            3161},
	{"eMcdbCreateSpecificProfileCapabilitiesQueryByVersionID",    3162},
	{"eMcdbGetNextSpecificProfileCapabilitiesByVersionID",        3163},

	{"eMcdbGetStatusAliasID",                                     3170}
};

CallBackMessage RemoteAPICallBackMessage::commonStatus[COMMON_STATUS_NUM]=
{
	{"MCA_BEGUN", 0X00},
	{"MCA_ENDED", 0X01},
	{"MCA_ENABLED", 0X02},
	{"MCA_DISABLED", 0X03},
	{"MCA_ACTIVE", 0X04},
	{"MCA_INACTIVE", 0X05},
	{"MCA_FAILED", 0X06},
	{"MCA_UNKNOWN", 0X07},
	{"MCA_OPENED", 0X08},
	{"MCA_CLOSED", 0X09},
	{"MCA_ADDED", 0X0A},
	{"MCA_DELETED", 0X0B},
	{"MCA_IN_USE", 0X0C},
	{"MCA_DATA_RECEIVED", 0X0D},
	{"MCA_ATTEMPTING_TO_SEND", 0X0E},
	{"MCA_DATA_ABORTED", 0X0F},
	{"MCA_ACK_RECEIVED", 0X10},
	{"MCA_ATTEMPTING_TO_TRANSMIT", 0X11},
	{"MCA_RESOURCE_AVAILABLE", 0X12},
	{"MCA_EMPTY", 0X13},
	{"MCA_RINGING", 0X14},
	{"MCA_TARGET_RINGING", 0X15},
	{"MCA_TALK_EXTEND_BEGUN", 0X16},
	{"MCA_TALK_EXTEND_ENDED", 0X17},
	{"MCA_INTERCOM_RECEIVING_AUDIO", 0X18},
	{"MCA_UNSUCCESSFUL", 0X19},
	{"MCA_SUCCESSFUL", 0X1A},
	{"MCA_ABORTED", 0X1B},
	{"MCA_SERVICE_PARTIALLY_AVAILABLE", 0X1C},
	{"MCA_SERVICE_NOT_AVAILABLE", 0X1D},
	{"MCA_SERVICE_AVAILABLE", 0X1E},
	{"MCA_UPDATE", 0X1F},
	{"MCA_PENDING", 0X20},
	{"MCA_PENDING_AND_IN_USE", 0X21},
	{"MCA_CALL_REQUEST", 0X22},
	{"MCA_REJECTED", 0X23}
};

CallBackMessage RemoteAPICallBackMessage::commonWarnings[WARNINGS_NUM]=
{
	{"MCA_RSC_DEASSIGNED_WARNING",                   0x00},
	{"MCA_OP_DISABLED_WARNING",                      0x01},
	{"MCA_ONE_OR_MORE_RSC_NOT_AVAILABLE_WRN",        0x02},
	{"MCA_ONE_OR_MORE_RSC_INVAL_CONFIG_WRN",         0x03},
	{"MCA_ONE_OR_MORE_RSC_NOT_SECURE_CAPABLE_WRN",   0x04},
	{"MCA_ONE_OR_MORE_INVAL_RSC_WRN",                0x05},
	{"MCA_ONE_OR_MORE_RSC_UNAVAIL_SECURE_KEY_WRN",   0x06},
	{"MCA_ONE_OR_MORE_RSC_WITHOUT_AUTOKEY_WRN",      0x07},
	{"MCA_SERVICE_PARTIALLY_AVAIL_WRN",              0x08},
	{"MCA_RSC_ASGN_NOT_CRITICAL",                  0x09},
	{"MCA_RESOURCE_DELETED_WRN",                     0x0A},
	{"MCA_MAX_MC_LC_ID_CHANGED_WRN",                    0x0B},
	{"MCA_SPEAKER_DELETED_WRN",                      0x0C},
	{"MCA_RSC_CROSS_BUSIED_WRN",                     0x0D},
	{"MCA_NOT_OFF_HOOK_WRN",                         0x0E},
	{"MCA_RESOURCE_NOT_AVAILABLE_WRN",               0x0F},
	{"MCA_PRIVATE_CALL_NOT_BEGUN_WRN",               0x10},
	{"MCA_SCAN_ENABLED_WRN",                         0x11},
	{"MCA_NOT_SECURE_CAPABLE_WRN",                   0x12},
	{"MCA_SECURE_CALL_NO_AUDIO_WRN",                 0x13},
	{"MCA_INSUFFICIENT_AUDIO_RESOURCES_WRN",         0x14},
	{"MCA_OVERSUBSCRIPTION_ERROR_WRN",               0x15},
	{"MCA_FEATURE_DISABLED_WRN",                     0x16},
	{"MCA_OP_CAPABILITY_REMOVED_WRN",                0x17},
	{"MCA_RESOURCE_TAGGING_FAILURE_WRN",		 0x18}
};

CallBackMessage RemoteAPICallBackMessage::commonApiError[COMMON_ERROR_NUM]=
{
   {"MCA_APPLICATION_CONFLICT", 0X00},
   {"MCA_DATA_PATH_NOT_AVAILABLE", 0X01},
   {"MCA_INVALID_MC_LC_ID", 0X02},
   {"MCA_INVALID_RID", 0X03},
   {"MCA_INVALID_SELECT_SPEAKER_ID", 0X04},
   {"MCA_INVALID_UNIT_ID", 0X05},
   {"MCA_INVALID_UNSELECT_SPEAKER_ID", 0X06},
   {"MCA_NOT_ALLOWED_FOR_THIS_OP", 0X07},
   {"MCA_NOT_ALLOWED_FOR_THIS_RSC", 0X08},
   {"MCA_NOT_IN_PROGRESS", 0X09},
   {"MCA_NO_ACK_RECEIVED", 0X0A},
   {"MCA_NO_RSC_SELECTED", 0X0B},
   {"MCA_OP_DISABLED", 0X0C},
   {"MCA_OP_TRANSMITTING", 0X0D},
   {"MCA_RSC_BUSY", 0X0E},
   {"MCA_RSC_IN_PATCH_GROUP", 0X0F},
   {"MCA_RSC_TRANSMITTING", 0X10},
   {"MCA_RSC_CROSS_BUSIED", 0X11},
   {"MCA_RSC_DEASSIGNED", 0X12},
   {"MCA_RSC_REGROUPED", 0X13},
   {"MCA_RSC_CANNOT_BE_KEYED", 0X14},
   {"MCA_TRUNKING_SYSTEM_FAILSOFT", 0X15},
   {"MCA_TRUNKING_SYSTEM_IDLE", 0X16},
   {"MCA_INVALID_VOLUME_LEVEL", 0X17},
   {"MCA_RESOURCE_NOT_AVAILABLE", 0X18},
   {"MCA_RSC_HAS_EMERGENCY", 0X19},
   {"MCA_INVALID_PRIORITY", 0X1A},
   {"MCA_RSC_OFF_HOOK", 0X1B},
   {"MCA_NOT_OFF_HOOK", 0X1C},
   {"MCA_INVALID_AUXID", 0X1D},
   {"MCA_INVALID_SYSTEM_CONFIGURATION", 0X1E},
   {"MCA_INVALID_SECURE_KEY", 0X1F},
   {"MCA_TIMEOUT", 0X20},
   {"MCA_INTERNAL_DB_ERROR", 0X21},
   {"MCA_DID_NOT_INITIATE_THE_INTERCOM_CALL", 0X22},
   {"MCA_NOT_ALLOWED_FOR_GROUP_IDS", 0X23},
   {"MCA_NOT_ALLOWED_DURING_TALK_EXTEND", 0X24},
   {"MCA_PRIVATE_CALL_NOT_BEGUN", 0X25},
   {"MCA_NOT_ALLOWED_ON_SELECTED_RSC", 0X26},
   {"MCA_NOT_ALLOWED_IN_ANALOG_MODE", 0X27},
   {"MCA_INVALID_FREQUENCY_SPECIFIED", 0X28},
   {"MCA_RSC_CONFLICT", 0X29},
   {"MCA_SCAN_ENABLED_ERROR", 0X2A},
   {"MCA_SECURE_KEY_UNAVAIL_ON_SELECTED_RSC", 0X2B},
   {"MCA_SERVICE_NOT_AVAILABLE_ERR", 0X2C},
   {"MCA_MC_LC_ID_NOT_ASSIGNED", 0X2D},
   {"MCA_SERVER_DISABLED", 0X2E},
   {"MCA_INVALID_CRITICAL_RSC_SETTING", 0X2F},
   {"MCA_INVALID_URID", 0X30},
   {"MCA_URID_UNAVAILABLE", 0X31},
   {"MCA_NOT_ALLOWED_FOR_THIS_RSC_TYPE", 0X32},
   {"MCA_INVALID_ENCRYPTED_AUDIO_VALUE", 0X33},
   {"MCA_PLATFORM_MISMATCH", 0X34},
   {"MCA_INVALID_DESTINATION", 0X35},
   {"MCA_INVALID_HEADSET_SELECT", 0X36},
   {"MCA_CAPABILITY_UNAVAILABLE", 0X37},
   {"MCA_AUXIO_NOT_ACCESSIBLE", 0X38},
   {"MCA_INVALID_RANGE", 0X39},
   {"MCA_MC_LC_ID_NOT_ASGND_TO_UNIT_ID", 0X3A},
   {"MCA_MC_LC_ID_NOT_ASGND_TO_GROUP", 0X3B},
   {"MCA_INVALID_RETRY_ID", 0X3C},
   {"MCA_INVALID_OUTPUT_CHANNEL", 0X3D}
};

CallBackMessage RemoteAPICallBackMessage::rscAssignmentError[RemoteAPICallBackMessage::RSC_ASSIGNMENT_ERROR_NUM] = 
{
	{"MCA_NO_AVAILABLE_LC_ID",                  0x0400},
	{"MCA_INTERNAL_DATABASE_ERROR",             0x0401},
	{"MCA_NOT_DEASSIGNED_TRANSMITTING",         0x0402},
	{"MCA_NOT_DEASSIGNED_IN_PATCH_GROUP",       0x0403},
	{"MCA_NOT_DEASSIGNED_EMERGENCY_ACTIVITY",   0x0404},
	{"MCA_NOT_DEASSIGNED_RSC_OFF_HOOK",         0x0405},
	{"MCA_ASSIGNMENT_IN_PROGRESS",              0x0406},
	{"MCA_RSC_ALREADY_ASSIGNED",                0x0407},
	/* This code no longer supported                                              */
	{"MCA_RID_UNAVAILABLE",                     0x0408},
	/* This code no longer supported                                              */
	{"MCA_NOT_ASSIGNED_TNNR_TX_FAIL",           0x0409},
	{"MCA_LCID_ALREADY_ASSIGNED",               0x040A}
};

CallBackMessage RemoteAPICallBackMessage::rscSelectStatus[RSC_SELECT_STATUS_NUM] = 
{
	{"MCA_RESOURCE_NOT_OPERATIONAL",      0x0400},
	{"MCA_DATA_PATH_NOT_OPERATIONAL",     0x0401},
	{"MCA_PENDING_AFFILIATION",           0x0402},
	{"MCA_REJECTED_AFFILIATION",          0x0403},
	{"MCA_LIMITED_CALL_AREA",             0x0404}
};

CallBackMessage RemoteAPICallBackMessage::emergencyAlarmStatus[EMERGENCY_ALARM_STATUS_NUM] = 
{
	{"MCA_RADIO_EMERG_ALARM_RECEIVED",         0x0400},
	{"MCA_EMERG_ACKNOWLEDGED",                 0x0401},
	{"MCA_EMERG_ALARM_ENDED",                  0x0402},
	{"MCA_ACKNOWLEDGED_EMERG_ALARM_REFRESH",   0x0403},
	{"MCA_UNACKNOWLEDGED_EMERG_ALARM_REFRESH", 0x0404},
	{"MCA_EMERG_ALARM_ENDED_REFRESH",          0x0405}
};

CallBackMessage RemoteAPICallBackMessage::emergencyCallStatus[EMERGENCY_CALL_STATUS_NUM] = 
{
	{"MCA_EMERG_CALL_STATE",         0x0400},
	{"MCA_EMERG_ENDED",                  0x0402},
	{"MCA_UNACKNOWLEDGED_EMERG_CALL_REFRESH",   0x0403},
	{"MCA_ACKNOWLEDGED_EMERG_CALL_REFRESH", 0x0404},
	{"MCA_EMERG_CALL_ENDED_REFRESH",          0x0405},
	{"MCA_PATCHED_EMERG_CALL_STATE",          0x0406},
	{"MCA_PATCHED_EMERG_CALL_REFRESH",          0x0407}
};

CallBackMessage RemoteAPICallBackMessage::authError[AUTH_ERROR_NUM] = 
{
	{"MCDI_AUTHENTICATION_FAILURE"                 , 0x0400},
	{"MCDI_USER_ALREADY_AUTHENTICATED"             , 0x0401},
	{"MCDI_NO_USER_NAME"                           , 0x0402},
	{"MCDI_INVALID_AUTHENTICATION_CLIENT_TYPE"     , 0x0403},
	{"MCDI_SERVER_UNAVAILABLE"                     , 0x0404}
};

CallBackMessage RemoteAPICallBackMessage::authWarning[AUTH_WARNING_NUM] = 
{
	{"MCDI_DEFERED_AUTHENTICATION_FAILURE"         , 0x0400}
};

CallBackMessage RemoteAPICallBackMessage::authStatus[AUTH_STATUS_NUM] = 
{
	{"MCDI_AUTHENTICATION_SUCCESSFUL"              , 0x0400},
	{"MCDI_USING_CACHED_CREDENTIALS"               , 0x0401},
	{"MCDI_NO_CURRENT_USER"                        , 0x0402}
};

CallBackMessage RemoteAPICallBackMessage::consoleShutdown[CONSOLE_SHUTDOWN_NUM] = 
{
	{"MCDI_CRITICAL_CLIENT_TERMINATED"     ,0x0400},
	{"MCDI_CRITICAL_CLIENT_NOT_STARTED"    ,0x0401}
};

CallBackMessage RemoteAPICallBackMessage::clientActivate[CLIENT_ACTIVATE_NUM] = 
{
	{"MCDI_NULL_STATUS",                    0x0400},
	{"MCDI_READY",                          0x0401},
	{"MCDI_NOT_READY",                      0x0402}
};

CallBackMessage RemoteAPICallBackMessage::clientReg[CLIENT_REG_NUM] = 
{
	{"MCDI_REGISTERED",                     0x0404},
	{"MCDI_DEREGISTERED",                   0x0405}
};

CallBackMessage RemoteAPICallBackMessage::dbUpdate[DB_UPDATE_NUM] = 
{
	{"MCDI_DATABASE_STATUS",                0x2480},
	{"MCDI_DATABASE_UPDATE_NOTIFICATION",   0x2481},
	{"MCDI_NEW_DATABASE_AVAILABLE",         0x2482},
	{"MCDI_NEW_DATABASE_GRANTED",           0x2483},
	{"MCDI_NEW_DATABASE_NOT_GRANTED",        0x2484},
	{"MCDI_MAX_DATABASE_STATUS_MSG",         0x24bf}
};

CallBackMessage RemoteAPICallBackMessage::audioProcessing[AUDIO_PROCESSING_NUM] = 
{
	{"MCDI_AUDIO_PROCESSING_INITIALIZING",      0x0400},
	{"MCDI_AUDIO_PROCESSING_AVAILABLE",         0x0401},
	{"MCDI_AUDIO_PROCESSING_NOT_AVAILABLE",     0x0402},
	{"MCDI_AUDIO_PROCESSING_INVALID",           0x0403}
};

CallBackMessage RemoteAPICallBackMessage::dbStatus[DB_STATUS_NUM] = 
{
	{"MCDI_DB_READY", 0x0401},
	{"MCDI_DB_NOT_READY", 0x0402}
};

CallBackMessage RemoteAPICallBackMessage::peripheralStatus[PERIPHERAL_STATUS_NUM] = 
{
	{"MCA_GPIOMS_NOT_AVAILABLE",            0x0400},
	{"MCA_GPIOMS_PARTIALLY_AVAILABLE",      0x0401},
	{"MCA_GPIOMS_AVAILABLE",                0x0402}
};

CallBackMessage RemoteAPICallBackMessage::resAsStatus[RESOURCE_AS_STATUS_NUM] = 
{
	{"MCA_RESOURCE_ASSIGNED",              0x400},
	{"MCA_RESOURCE_DEASSIGNED",            0x401},
	{"MCA_ALL_RESOURCES_DEASSIGNED",       0x402}
};

char *RemoteAPICallBackMessage::getResponseName(unsigned short val)
{
	for(int i=0;i<CALLBACK_MESSAGES_NUM;i++)
		if(responses[i].value==val)
			return responses[i].name;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in responses map!",val);
	return "";
}

unsigned short RemoteAPICallBackMessage::getValueForName(string name)
{
	for(int i=0;i<CALLBACK_MESSAGES_NUM;i++)
		if(strcmp(name.c_str(),responses[i].name)==0)
			return responses[i].value;
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Name %s is not found in responses map!",name);
	return NULL;
}

char * RemoteAPICallBackMessage::getErrorName(unsigned short val,ApiErrorType type)
{
	int i = 0;
	switch (type) {
		case rscAssignmentErrorType:
			for (i=0; i<RSC_ASSIGNMENT_ERROR_NUM; i++) {
				if (rscAssignmentError[i].value == val)
					return rscAssignmentError[i].name;
			}
			break;
		case authErrorType:
			for (i=0; i<AUTH_ERROR_NUM; i++) {
				if (authError[i].value == val)
					return authError[i].name;
			}
			break;
		default:
			break;
	};
	i = 0;
	for (i=0; i<COMMON_ERROR_NUM; i++) {
		if (commonApiError[i].value == val)
			return commonApiError[i].name;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in Errors map!",val);
	return "";
}

char * RemoteAPICallBackMessage::getStatusName(unsigned short val, ApiStatusType type)
{
	int i = 0;
	switch (type) {
		case rscSelectStatusType:
			for (i=0; i<RSC_SELECT_STATUS_NUM; i++) {
				if (rscSelectStatus[i].value == val)
					return rscSelectStatus[i].name;
			}
			break;
		case emergencyAlarmStatusType:
			for (i=0; i<EMERGENCY_ALARM_STATUS_NUM; i++) {
				if (emergencyAlarmStatus[i].value == val)
					return emergencyAlarmStatus[i].name;
			}
			break;
		case emergencyCallStatusType:
			for (i=0; i<EMERGENCY_CALL_STATUS_NUM; i++) {
				if (emergencyCallStatus[i].value == val)
					return emergencyCallStatus[i].name;
			}
			break;
		case authStatusType:
			for (i=0; i<AUTH_STATUS_NUM; i++) {
				if (authStatus[i].value == val)
					return authStatus[i].name;
			}
			break;
		case consoleShutdownType:
			for (i=0; i<CONSOLE_SHUTDOWN_NUM; i++) {
				if (consoleShutdown[i].value == val)
					return consoleShutdown[i].name;
			}
			break;
		case clientActivateType:
			for (i=0; i<CLIENT_ACTIVATE_NUM; i++) {
				if (clientActivate[i].value == val)
					return clientActivate[i].name;
			}
			break;
		case clientRegType:
			for (i=0; i<CLIENT_REG_NUM; i++) {
				if (clientReg[i].value == val)
					return clientReg[i].name;
			}
			break;
		case dbUpdateType:
			for (i=0; i<DB_UPDATE_NUM; i++) {
				if (dbUpdate[i].value == val)
					return dbUpdate[i].name;
			}
			break;
		case audioProcessingType:
			for (i=0; i<AUDIO_PROCESSING_NUM; i++) {
				if (audioProcessing[i].value == val)
					return audioProcessing[i].name;
			}
			break;
		case dbStatusType:
			for (i=0; i<DB_STATUS_NUM; i++) {
				if (dbStatus[i].value == val)
					return dbStatus[i].name;
			}
			break;
		case peripheralStatusType:
			for (i=0; i<PERIPHERAL_STATUS_NUM; i++) {
				if (peripheralStatus[i].value == val)
					return peripheralStatus[i].name;
			}
			break;
		case resourceAssignmentStatusType:			
			for (i=0; i<RESOURCE_AS_STATUS_NUM; i++) {
				if (resAsStatus[i].value == val)
					return resAsStatus[i].name;
			}
			break;
		default:
			break;
	};
	i = 0;
	for (i=0; i<COMMON_STATUS_NUM; i++) {
		if (commonStatus[i].value == val)
			return commonStatus[i].name;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in Status map!",val);
	return "";
}

char * RemoteAPICallBackMessage::getCommandName(unsigned short val)
{
	for (int i=0; i<COMMANDS_NUM; i++) {
		if (commands[i].value == val)
			return commands[i].name;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in commands map!",val);
	return "";
}

char * RemoteAPICallBackMessage::getWarningName(unsigned short val, ApiWarningType type)
{
	int i = 0;
	switch (type) {
		case authWarningType:
			for (i=0; i<AUTH_WARNING_NUM; i++) {
				if (authWarning[i].value == val)
					return authWarning[i].name;
			}
			break;
		default:
			break;
	};
	i = 0;
	for (i=0; i<WARNINGS_NUM; i++) {
		if (commonWarnings[i].value == val)
			return commonWarnings[i].name;
	}
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in Warnings map!",val);
	return "";
}
