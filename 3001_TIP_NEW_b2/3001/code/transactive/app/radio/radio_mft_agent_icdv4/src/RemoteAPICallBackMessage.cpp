#include "RemoteAPICallBackMessage.h"
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
char *RemoteAPICallBackMessage::getResponseName(unsigned short val)
{
for(int i=0;i<CALLBACK_MESSAGES_NUM;i++)
if(responses[i].value==val)
return responses[i].name;
LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Value %u is not found in responses map!",val);
return NULL;
}
unsigned short RemoteAPICallBackMessage::getValueForName(string name)
{
for(int i=0;i<CALLBACK_MESSAGES_NUM;i++)
if(strcmp(name.c_str(),responses[i].name)==0)
return responses[i].value;
LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ERROR : Name %s is not found in responses map!",name);
return NULL;
}