#ifndef __TAPIRESPONSE__
#define __TAPIRESPONSE__
#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPI.h"
#include<iostream>
#include "ace/Get_Opt.h"
#include "core/utilities/src/DebugUtil.h"
#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPICallBackMessage.h"
#include"app/radio/radio_mft_agent_icdv4/src/RemoteAPIRadioEvent.h"

/*
#include<omniORB4/CORBA_Any.h>
*/
using namespace std;
using namespace ICDV4;
namespace MotorolaRemoteAPI
{
class TAPIResponse : public virtual POA_MCC7500SDK::IAPIResponse
{
	RemoteApiConnection &api_connection;
public:
	TAPIResponse         (RemoteApiConnection &con):api_connection(con)
    {

    }
    virtual ~TAPIResponse()
    {

    }
    virtual void  CallBack(const ::MCC7500SDK::Parameters & message)
    {
		if(message.length()==0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received Keep-ALIVE message");
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT : Callback invoked from server. Parameter len: %d Message TYPE: %u(0x%x) | Name : %s",message.length(),message[0].V_UINT16(),message[0].V_UINT16(),RemoteAPICallBackMessage::getResponseName(message[0].V_UINT16()));
			ICDV4::RemoteAPIRadioEvent *event;
			switch(message[0].V_UINT16())
			{
case eMsgMcdiAudioProcessingStatusID :
		event=new McdiAudioProcessingStatus(message);
		break;

case eMsgMcdiAuthenticationErrorID :
		event=new McdiAuthenticationError(message);
		break;

case eMsgMcdiAuthenticationWarningID :
		event=new McdiAuthenticationWarning(message);
		break;

case eMsgMcdiAuthenticationStatusID :
		event=new McdiAuthenticationStatus(message);
		break;

case eMsgMcdiConsoleShutDownID :
		event=new McdiConsoleShutDown(message);
		break;

case eMsgMcdiActivateClientID :
		event=new McdiActivateClient(message);
		break;

case eMsgMcdiDeactivateClientID :
		event=new McdiDeactivateClient(message);
		break;

case eMsgMcdiClientActivationStatusID :
		event=new McdiClientActivationStatus(message);
		break;

case eMsgMcdiMessageLossID :
		event=new McdiMessageLoss(message);
		break;

case eMsgMcdiClientRegistrationStatusID :
		event=new McdiClientRegistrationStatus(message);
		break;

case eMsgMcdiNewDatabaseGrantedID :
		event=new McdiNewDatabaseGranted(message);
		break;

case eMsgMcdiNewDatabaseNotGrantedID :
		event=new McdiNewDatabaseNotGranted(message);
		break;

case eMsgMcdiNewDatabaseAvailableID :
		event=new McdiNewDatabaseAvailable(message);
		break;

case eMsgMcdiDatabaseUpdateNotificationID :
		event=new McdiDatabaseUpdateNotification(message);
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database Update Notification, status = %x", ((McdiDatabaseUpdateNotification*)event)->get_wStatus());	
		break;

case eMsgMcdiDatabaseStatusID :
		event=new McdiDatabaseStatus(message);
		break;

case eMsgMcaBeginAllMuteErrorID :
		event=new McaBeginAllMuteError(message);
		break;

case eMsgMcaEndAllMuteErrorID :
		event=new McaEndAllMuteError(message);
		break;

case eMsgMcaAllMuteStatusID :
		event=new McaAllMuteStatus(message);
		break;

case eMsgMcaBeginAmbienceListeningCallErrorID :
		event=new McaBeginAmbienceListeningCallError(message);
		break;

case eMsgMcaEndAmbienceListeningCallErrorID :
		event=new McaEndAmbienceListeningCallError(message);
		break;

case eMsgMcaAmbienceListeningCallStatusID :
		event=new McaAmbienceListeningCallStatus(message);
		break;

case eMsgMcaBeginAPBTransmitWarningID :
		event=new McaBeginAPBTransmitWarning(message);
		break;

case eMsgMcaBeginAPBTransmitErrorID :
		event=new McaBeginAPBTransmitError(message);
		break;

case eMsgMcaEndAPBTransmitErrorID :
		event=new McaEndAPBTransmitError(message);
		break;

case eMsgMcaAPBTransmitStatusID :
		event=new McaAPBTransmitStatus(message);
		break;

case eMsgMcaAssignAudioDestinationWarningID :
		event=new McaAssignAudioDestinationWarning(message);
		break;

case eMsgMcaAssignAudioDestinationErrorID :
		event=new McaAssignAudioDestinationError(message);
		break;

case eMsgMcaAudioDestinationStatusID :
		event=new McaAudioDestinationStatus(message);
		break;

case eMsgMcaSoundBeepsErrorID :
		event=new McaSoundBeepsError(message);
		break;

case eMsgMcaSelectCriticalResourceSettingErrorID :
		event=new McaSelectCriticalResourceSettingError(message);
		break;

case eMsgMcaCriticalResourceStatusID :
		event=new McaCriticalResourceStatus(message);
		break;

case eMsgMcaDatabaseUpdateNotificationStatusID :
		event=new McaDatabaseUpdateNotificationStatus(message);
		break;

case eMsgMcaAcknowledgeEmergencyErrorID :
		event=new McaAcknowledgeEmergencyError(message);
		break;

case eMsgMcaEmergencyAlarmStatusID :
		event=new McaEmergencyAlarmStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaEmergencyAlarmStatus status: %u",((McaEmergencyAlarmStatus*)event)->get_wStatus());
		break;

case eMsgMcaBeginConsoleEmergencyCallErrorID :
		event=new McaBeginConsoleEmergencyCallError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaBeginConsoleEmergencyCallError, error number = %u, channel = %u",((McaBeginConsoleEmergencyCallError*)event)->get_wErrorNum(), ((McaBeginConsoleEmergencyCallError*)event)->get_channel());
		break;

case eMsgMcaEndEmergencyErrorID :
		event=new McaEndEmergencyError(message);
		break;

case eMsgMcaEmergencyCallStatusID :
		event=new McaEmergencyCallStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaEmergencyCallStatus status: %u",((McaEmergencyCallStatus*)event)->get_wStatus());
		break;

case eMsgMcaSelectFrequencyErrorID :
		event=new McaSelectFrequencyError(message);
		break;

case eMsgMcaResourceFrequencyStatusID :
		event=new McaResourceFrequencyStatus(message);
		break;

case eMsgMcaBeginGeneralTransmitWarningID :
		event=new McaBeginGeneralTransmitWarning(message);
		break;

case eMsgMcaBeginGeneralTransmitErrorID :
		event=new McaBeginGeneralTransmitError(message);
		break;

case eMsgMcaEndGeneralTransmitErrorID :
		event=new McaEndGeneralTransmitError(message);
		break;

case eMsgMcaGeneralTransmitStatusID :
		event=new McaGeneralTransmitStatus(message);
		break;

case eMsgMcaInboundCallStatusID :
		event=new McaInboundCallStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT InboundCallStatus status = %u, channel = %u", ((McaInboundCallStatus*)event)->get_wStatus(), ((McaInboundCallStatus*)event)->get_channel());
		break;

case eMsgMcaInboundPttIdStatusID :
		event=new McaInboundPttIdStatus(message);
		break;

case eMsgMcaBeginInstantTransmitErrorID :
		event=new McaBeginInstantTransmitError(message);
		break;

case eMsgMcaEndInstantTransmitErrorID :
		event=new McaEndInstantTransmitError(message);
		break;

case eMsgMcaAssignLoggingAudioDestinationErrorID :
		event=new McaAssignLoggingAudioDestinationError(message);
		break;

case eMsgMcaLoggingAudioDestinationStatusID :
		event=new McaLoggingAudioDestinationStatus(message);
		break;

case eMsgMcaLoggingGroupCallStatusID :
		event=new McaLoggingGroupCallStatus(message);
		break;

case eMsgMcaLoggingPrivateCallEventStatusID :
		event=new McaLoggingPrivateCallEventStatus(message);
		break;

case eMsgMcaLoggingPrivateCallStatusID :
		event=new McaLoggingPrivateCallStatus(message);
		break;

case eMsgMcaLoggingTelephoneCallEventStatusID :
		event=new McaLoggingTelephoneCallEventStatus(message);
		break;

case eMsgMcaLoggingTelephoneCallStatusID :
		event=new McaLoggingTelephoneCallStatus(message);
		break;

case eMsgMcaAssignLoggingResourceWarningID :
		event=new McaAssignLoggingResourceWarning(message);
		break;

case eMsgMcaAssignLoggingResourceErrorID :
		event=new McaAssignLoggingResourceError(message);
		break;

case eMsgMcaDeassignLoggingResourceWarningID :
		event=new McaDeassignLoggingResourceWarning(message);
		break;

case eMsgMcaDeassignLoggingResourceErrorID :
		event=new McaDeassignLoggingResourceError(message);
		break;

case eMsgMcaLoggingAssignmentStatusID :
		event=new McaLoggingAssignmentStatus(message);
		break;

case eMsgMcaAssignIndividualLoggingResourceWarningID :
		event=new McaAssignIndividualLoggingResourceWarning(message);
		break;

case eMsgMcaAssignIndividualLoggingResourceErrorID :
		event=new McaAssignIndividualLoggingResourceError(message);
		break;

case eMsgMcaDeassignIndividualLoggingResourceWarningID :
		event=new McaDeassignIndividualLoggingResourceWarning(message);
		break;

case eMsgMcaDeassignIndividualLoggingResourceErrorID :
		event=new McaDeassignIndividualLoggingResourceError(message);
		break;

case eMsgMcaLoggingIndividualAssignmentStatusID :
		event=new McaLoggingIndividualAssignmentStatus(message);
		break;

case eMsgMcaBeginMonitorErrorID :
		event=new McaBeginMonitorError(message);
		break;

case eMsgMcaEndMonitorErrorID :
		event=new McaEndMonitorError(message);
		break;

case eMsgMcaMonitorStatusID :
		event=new McaMonitorStatus(message);
		break;

case eMsgMcaOpenMselErrorID :
		event=new McaOpenMselError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaOpenMselError, error number = %u, group number = %u", ((McaOpenMselError*)event)->get_wErrorNum(), ((McaOpenMselError*)event)->get_wMselGroup());
		break;

case eMsgMcaCloseMselErrorID :
		event=new McaCloseMselError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaCloseMselError, error number: %u, group number: %u", ((McaCloseMselError*)event)->get_wErrorNum(), ((McaCloseMselError*)event)->get_wMselGroup());
		break;

case eMsgMcaMselGroupStatusID :
		event=new McaMselGroupStatus(message); 
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaMselGroupStatus, status: %u, group number: %u", ((McaMselGroupStatus*)event)->get_wStatus(), ((McaMselGroupStatus*)event)->get_wMselGroup());
		break;

case eMsgMcaAddMselMemberWarningID :
		event=new McaAddMselMemberWarning(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaAddMselMemberWarning warning message: %u, channel: %u, group number: %u",((McaAddMselMemberWarning*)event)->get_wWarningNum(), ((McaAddMselMemberWarning*)event)->get_channel(), ((McaAddMselMemberWarning*)event)->get_wMselGroup());
		break;

case eMsgMcaAddMselMemberErrorID :
		event=new McaAddMselMemberError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaAddMselMemberError error message: %u, channel: %u, group number: %u",((McaAddMselMemberError*)event)->get_wErrorNum(), ((McaAddMselMemberError*)event)->get_channel(), ((McaAddMselMemberError*)event)->get_wMselGroup());
		break;

case eMsgMcaDeleteMselMemberErrorID :
		event=new McaDeleteMselMemberError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaDeleteMselMemberError error message: %u, channel:%u, group number: %u", ((McaDeleteMselMemberError*)event)->get_wErrorNum(), ((McaDeleteMselMemberError*)event)->get_channel(), ((McaDeleteMselMemberError*)event)->get_wMselGroup());
		break;

case eMsgMcaMselMemberStatusID :
		event=new McaMselMemberStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaMselMemberStatus status: %u, channel: %u, group number: %u", ((McaMselMemberStatus*)event)->get_wStatus(), ((McaMselMemberStatus*)event)->get_channel(), ((McaMselMemberStatus*)event)->get_wMselGroup());
		break;

case eMsgMcaMselMemberRefreshID :
		event=new McaMselMemberRefresh(message);
		break;

case eMsgMcaGetFreeMselGroupErrorID :
		event=new McaGetFreeMselGroupError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaGetFreeMselGroupError error number: %u", ((McaGetFreeMselGroupError*)event)->get_wErrorNum());
		break;

case eMsgMcaFreeMselGroupStatusID :
		event=new McaFreeMselGroupStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaFreeMselGroupStatus group number: %u", ((McaFreeMselGroupStatus*)event)->get_wMselGroup());
		break;

case eMsgMcaBeginMuteSecondReceiverErrorID :
		event=new McaBeginMuteSecondReceiverError(message);
		break;

case eMsgMcaEndMuteSecondReceiverErrorID :
		event=new McaEndMuteSecondReceiverError(message);
		break;

case eMsgMcaMuteSecondReceiverStatusID :
		event=new McaMuteSecondReceiverStatus(message);
		break;

case eMsgMcaAddPatchMemberWarningID :
		event=new McaAddPatchMemberWarning(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaAddPatchMemberWarning warning message: %u, channel: %u, patch group: %u",((McaAddPatchMemberWarning*)event)->get_wWarningNum(), ((McaAddPatchMemberWarning*)event)->get_channel(),((McaAddPatchMemberWarning*)event)->get_wPatchGroup());
		break;

case eMsgMcaAddPatchMemberErrorID :
		event=new McaAddPatchMemberError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaAddPatchMemberError error message = %u, channel = %u, patch group = %u",((McaAddPatchMemberError*)event)->get_wErrorNum(), ((McaAddPatchMemberError*)event)->get_channel(),((McaAddPatchMemberError*)event)->get_wPatchGroup());
		break;

case eMsgMcaDeletePatchMemberErrorID :
		event=new McaDeletePatchMemberError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaDeletePatchMemberError error message: %u, channel: %u", ((McaDeletePatchMemberError*)event)->get_wErrorNum(), ((McaDeletePatchMemberError*)event)->get_channel());
		break;

case eMsgMcaPatchMemberStatusID :
		event=new McaPatchMemberStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaPatchMemberStatus status: %u, channel: %u, patch group: %u",((McaPatchMemberStatus*)event)->get_wStatus(), ((McaPatchMemberStatus*)event)->get_channel(), ((McaPatchMemberStatus*)event)->get_wPatchGroup());
		break;

case eMsgMcaPatchGroupStatusID :
		event=new McaPatchGroupStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaPatchGroupStatus status:%u, patch group: %u",((McaPatchGroupStatus*)event)->get_wStatus(), ((McaPatchGroupStatus*)event)->get_wPatchGroup());
		break;

case eMsgMcaPatchMemberRefreshID :
		event=new McaPatchMemberRefresh(message);
		break;

case eMsgMcaGetFreePatchGroupErrorID :
		event=new McaGetFreePatchGroupError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaGetFreePatchGroupError error message: %u",((McaGetFreePatchGroupError*)event)->get_wErrorNum());
		break;

case eMsgMcaFreePatchGroupStatusID :
		event=new McaFreePatchGroupStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaFreePatchGroupStatus: free patch group = %u",((McaFreePatchGroupStatus*)event)->get_wPatchGroup());
		break;

case eMsgMcaBeginPatchTransmitWarningID :
		event=new McaBeginPatchTransmitWarning(message);
		break;

case eMsgMcaBeginPatchTransmitErrorID :
		event=new McaBeginPatchTransmitError(message);
		break;

case eMsgMcaEndPatchTransmitErrorID :
		event=new McaEndPatchTransmitError(message);
		break;

case eMsgMcaPatchTransmitStatusID :
		event=new McaPatchTransmitStatus(message);
		break;

case eMsgMcaPeripheralsStatusID :
		event=new McaPeripheralsStatus(message);
		break;

case eMsgMcaAnswerPrivateCallErrorID :
		event=new McaAnswerPrivateCallError(message);
		break;

case eMsgMcaBeginPrivateCallErrorID :
		event=new McaBeginPrivateCallError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT MsgMcaBeginPrivateCallErrorID, error Number = %u, channel = %u",((McaBeginPrivateCallError*)event)->get_wErrorNum(),((McaBeginPrivateCallError*)event)->get_channel());
		break;

case eMsgMcaEndPrivateCallErrorID :
		event=new McaEndPrivateCallError(message);
		break;

case eMsgMcaInboundPrivateCallStatusID :
		event=new McaInboundPrivateCallStatus(message);		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaInboundPrivateCallStatus: channel = %u, alias = %s, status = %u",((McaInboundPrivateCallStatus*)event)->get_channel(), ((McaInboundPrivateCallStatus*)event)->get_IndividualAlias(), ((McaInboundPrivateCallStatus*)event)->get_wStatus());
		break;

case eMsgMcaPrivateCallStatusID :
		event=new McaPrivateCallStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaPrivateCallStatus: status = %u",((McaPrivateCallStatus*)event)->get_wStatus());
		break;

case eMsgMcaRejectPrivateCallErrorID :
		event=new McaRejectPrivateCallError(message);
		break;

case eMsgMcaSelectPrivateLineErrorID :
		event=new McaSelectPrivateLineError(message);
		break;

case eMsgMcaResourcePrivateLineStatusID :
		event=new McaResourcePrivateLineStatus(message);
		break;

case eMsgMcaInboundRadioStatusID :
		event=new McaInboundRadioStatus(message);
		break;

case eMsgMcaRefreshStatusID :
		event=new McaRefreshStatus(message);
		break;

case eMsgMcaEnableRepeatErrorID :
		event=new McaEnableRepeatError(message);
		break;

case eMsgMcaDisableRepeatErrorID :
		event=new McaDisableRepeatError(message);
		break;

case eMsgMcaRepeatStatusID :
		event=new McaRepeatStatus(message);
		break;

case eMsgMcaAssignResourceWarningID :
		event=new McaAssignResourceWarning(message);
		break;

case eMsgMcaAssignResourceErrorID :
		event=new McaAssignResourceError(message);
		break;

case eMsgMcaDeassignResourceWarningID :
		event=new McaDeassignResourceWarning(message);
		break;

case eMsgMcaDeassignResourceErrorID :
		event=new McaDeassignResourceError(message);
		break;

case eMsgMcaAssignmentStatusID :
		event=new McaAssignmentStatus(message);
		break;

case eMsgMcaResourceRejectStatusID :
		event=new McaResourceRejectStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaResourceRejectStatus: status = %u, channel = %u",((McaResourceRejectStatus*)event)->get_wStatus(), ((McaResourceRejectStatus*)event)->get_channel());
		break;

case eMsgMcaResourceStatusID :
		event=new McaResourceStatus(message);
		break;

case eMsgMcaResourceTransmitStatusID :
		event=new McaResourceTransmitStatus(message);
		break;

case eMsgMcaResourceTransmitWarningID :
		event=new McaResourceTransmitWarning(message);
		break;

case eMsgMcaSelectResourceErrorID :
		event=new McaSelectResourceError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaSelectResourceError: error number = %u channel = %u",((McaSelectResourceError*)event)->get_wErrorNum(), ((McaSelectResourceError*)event)->get_channel());
		break;

case eMsgMcaDeselectResourceErrorID :
		event=new McaDeselectResourceError(message);
		break;

case eMsgMcaResourceSelectStatusID :
		event=new McaResourceSelectStatus(message);
		break;

case eMsgMcaSelectPriorityErrorID :
		event=new McaSelectPriorityError(message);
		break;

case eMsgMcaPriorityStatusID :
		event=new McaPriorityStatus(message);
		break;

case eMsgMcaBeginTakeoverErrorID :
		event=new McaBeginTakeoverError(message);
		break;

case eMsgMcaEndTakeoverErrorID :
		event=new McaEndTakeoverError(message);
		break;

case eMsgMcaTakeoverStatusID :
		event=new McaTakeoverStatus(message);
		break;

case eMsgMcaBeginToneErrorID :
		event=new McaBeginToneError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaBeginToneError: error number = %u",((McaBeginToneError*)event)->get_wErrorNum());
		break;

case eMsgMcaEndToneErrorID :
		event=new McaEndToneError(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaEndToneError: error number = %u",((McaEndToneError*)event)->get_wErrorNum());
		break;

case eMsgMcaToneStatusID :
		event=new McaToneStatus(message);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaToneStatus: tone = %u, status = %u, clientid = %u",((McaToneStatus*)event)->get_tone(), ((McaToneStatus*)event)->get_wStatus(), ((McaToneStatus*)event)->get_byClientId());
		break;

case eMsgMcaSelectTransmitModeErrorID :
		event=new McaSelectTransmitModeError(message);
		break;

case eMsgMcaSelectTransmitModeStatusID :
		event=new McaSelectTransmitModeStatus(message);
		break;

case eMsgMcaTrunkingSystemStatusID :
		event=new McaTrunkingSystemStatus(message);
		break;

case eMsgMcaChangeVolumeWarningID :
		event=new McaChangeVolumeWarning(message);
		break;

case eMsgMcaChangeVolumeErrorID :
		event=new McaChangeVolumeError(message);
		break;

case eMsgMcaResourceVolumeStatusID :
		event=new McaResourceVolumeStatus (message);
		break;

case eMsgMcaActivateWildcardErrorID :
		event=new McaActivateWildcardError(message);
		break;

case eMsgMcaDeactivateWildcardErrorID :
		event=new McaDeactivateWildcardError(message);
		break;

case eMsgMcaWildcardStatusID :
		event=new McaWildcardStatus(message);
		break;
default:
LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unidentified Message");
			}
			api_connection.processEvent(event);
			delete event;
		}
    }
};
}
#endif