
#include "app\radio\radio_mft_agent\src\ApiResponseHandler.h"


namespace TA_IRS_App
{

	ApiResponseHandler::ApiResponseHandler (RemoteApiConnection &remoteApiConnection) : 
		m_responseHelper (remoteApiConnection)
	{
		FUNCTION_ENTRY ("ApiResponseHandler");
		start ();
		FUNCTION_EXIT;
	}

	ApiResponseHandler::~ApiResponseHandler ()
	{
		FUNCTION_ENTRY ("~ApiResponseHandler");
		terminateAndWait();
		FUNCTION_EXIT;
	}

	void ApiResponseHandler::sendResult (const ::MCC7500SDK::Parameters & message)
	{
		FUNCTION_ENTRY ("sendResult");

		{
			LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Send response for processing");
			
			if (NULL==(&message))
			{
				LOG_GENERIC (SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							 "Not Valid Message from MCC");
				return;
			}
			
			TA_IRS_App::RemoteAPIRadioEvent * eventMessage = creteEventFromResponse(message);
			
			if (eventMessage)
			{
				boost::shared_ptr <TA_IRS_App::RemoteAPIRadioEvent> sharedEvent (eventMessage);
				insert (sharedEvent);
			}
			

		}

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Returning to apicallback");

		FUNCTION_EXIT;
	}
	
	void ApiResponseHandler::processEvent (boost::shared_ptr<TA_IRS_App::RemoteAPIRadioEvent> eventItem)
	{
		FUNCTION_ENTRY ("processEvent");

		m_responseHelper.processEventRequest(eventItem.get());

		FUNCTION_EXIT;
	}

	TA_IRS_App::RemoteAPIRadioEvent * ApiResponseHandler::creteEventFromResponse (const ::MCC7500SDK::Parameters & message)
	{
		TA_IRS_App::RemoteAPIRadioEvent *event = NULL;

		switch(message[0].V_UINT16())
		{
		case eMsgMcdiAudioProcessingStatusID :
			event=new TA_IRS_App::McdiAudioProcessingStatus(message);
			event->printMessage();
			break;

		case eMsgMcdiAuthenticationErrorID :
			event=new TA_IRS_App::McdiAuthenticationError(message);
			event->printMessage();
			break;

		case eMsgMcdiAuthenticationWarningID :
			event=new TA_IRS_App::McdiAuthenticationWarning(message);
			event->printMessage();
			break;

		case eMsgMcdiAuthenticationStatusID :
			event=new TA_IRS_App::McdiAuthenticationStatus(message);
			event->printMessage();
			break;

		case eMsgMcdiConsoleShutDownID :
			event=new TA_IRS_App::McdiConsoleShutDown(message);
			break;

		case eMsgMcdiActivateClientID :
			event=new TA_IRS_App::McdiActivateClient(message);
			break;

		case eMsgMcdiDeactivateClientID :
			event=new TA_IRS_App::McdiDeactivateClient(message);
			break;

		case eMsgMcdiClientActivationStatusID :
			event=new TA_IRS_App::McdiClientActivationStatus(message);
			break;

		case eMsgMcdiMessageLossID :
			event=new TA_IRS_App::McdiMessageLoss(message);
			break;

		case eMsgMcdiClientRegistrationStatusID :
			event=new TA_IRS_App::McdiClientRegistrationStatus(message);
			break;

		case eMsgMcdiNewDatabaseGrantedID :
			event=new TA_IRS_App::McdiNewDatabaseGranted(message);
			break;

		case eMsgMcdiNewDatabaseNotGrantedID :
			event=new TA_IRS_App::McdiNewDatabaseNotGranted(message);
			break;

		case eMsgMcdiNewDatabaseAvailableID :
			event=new TA_IRS_App::McdiNewDatabaseAvailable(message);
			break;

		case eMsgMcdiDatabaseUpdateNotificationID :
			event=new TA_IRS_App::McdiDatabaseUpdateNotification(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database Update Notification, status = %x", ((McdiDatabaseUpdateNotification*)event)->get_wDataGroup());	
			break;

		case eMsgMcdiDatabaseStatusID :
			event=new TA_IRS_App::McdiDatabaseStatus(message);
			event->printMessage();
			break;

		case eMsgMcaBeginAllMuteErrorID :
			event=new TA_IRS_App::McaBeginAllMuteError(message);
			break;

		case eMsgMcaEndAllMuteErrorID :
			event=new TA_IRS_App::McaEndAllMuteError(message);
			break;

		case eMsgMcaAllMuteStatusID :
			event=new TA_IRS_App::McaAllMuteStatus(message);
			break;

		case eMsgMcaBeginAmbienceListeningCallErrorID :
			event=new TA_IRS_App::McaBeginAmbienceListeningCallError(message);
			break;

		case eMsgMcaEndAmbienceListeningCallErrorID :
			event=new TA_IRS_App::McaEndAmbienceListeningCallError(message);
			break;

		case eMsgMcaAmbienceListeningCallStatusID :
			event=new TA_IRS_App::McaAmbienceListeningCallStatus(message);
			break;

		case eMsgMcaBeginAPBTransmitWarningID :
			event=new TA_IRS_App::McaBeginAPBTransmitWarning(message);
			break;

		case eMsgMcaBeginAPBTransmitErrorID :
			event=new TA_IRS_App::McaBeginAPBTransmitError(message);
			break;

		case eMsgMcaEndAPBTransmitErrorID :
			event=new TA_IRS_App::McaEndAPBTransmitError(message);
			break;

		case eMsgMcaAPBTransmitStatusID :
			event=new TA_IRS_App::McaAPBTransmitStatus(message);
			break;

		case eMsgMcaAssignAudioDestinationWarningID :
			event=new TA_IRS_App::McaAssignAudioDestinationWarning(message);
			break;

		case eMsgMcaAssignAudioDestinationErrorID :
			event=new TA_IRS_App::McaAssignAudioDestinationError(message);
			break;

		case eMsgMcaAudioDestinationStatusID :
			event=new TA_IRS_App::McaAudioDestinationStatus(message);
			break;

		case eMsgMcaSoundBeepsErrorID :
			event=new TA_IRS_App::McaSoundBeepsError(message);
			break;

		case eMsgMcaSelectCriticalResourceSettingErrorID :
			event=new TA_IRS_App::McaSelectCriticalResourceSettingError(message);
			break;

		case eMsgMcaCriticalResourceStatusID :
			event=new TA_IRS_App::McaCriticalResourceStatus(message);
			break;

		case eMsgMcaDatabaseUpdateNotificationStatusID :
			event=new TA_IRS_App::McaDatabaseUpdateNotificationStatus(message);
			break;

		case eMsgMcaAcknowledgeEmergencyErrorID :
			event=new TA_IRS_App::McaAcknowledgeEmergencyError(message);
			event->printMessage();
			break;

		case eMsgMcaEmergencyAlarmStatusID :
			event=new TA_IRS_App::McaEmergencyAlarmStatus(message);
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaEmergencyAlarmStatus status: %u",((McaEmergencyAlarmStatus*)event)->get_wStatus());
			event->printMessage();
			break;

		case eMsgMcaBeginConsoleEmergencyCallErrorID :
			event=new TA_IRS_App::McaBeginConsoleEmergencyCallError(message);
			event->printMessage();
			break;

		case eMsgMcaEndEmergencyErrorID :
			event=new TA_IRS_App::McaEndEmergencyError(message);
			event->printMessage();
			break;

		case eMsgMcaEmergencyCallStatusID :
			event=new TA_IRS_App::McaEmergencyCallStatus(message);
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaEmergencyCallStatus status: %u",((McaEmergencyCallStatus*)event)->get_wStatus());
			event->printMessage();
			break;

		case eMsgMcaSelectFrequencyErrorID :
			event=new TA_IRS_App::McaSelectFrequencyError(message);
			break;

		case eMsgMcaResourceFrequencyStatusID :
			event=new TA_IRS_App::McaResourceFrequencyStatus(message);
			break;

		case eMsgMcaBeginGeneralTransmitWarningID :
			event=new TA_IRS_App::McaBeginGeneralTransmitWarning(message);
			break;

		case eMsgMcaBeginGeneralTransmitErrorID :
			event=new TA_IRS_App::McaBeginGeneralTransmitError(message);
			break;

		case eMsgMcaEndGeneralTransmitErrorID :
			event=new TA_IRS_App::McaEndGeneralTransmitError(message);
			break;

		case eMsgMcaGeneralTransmitStatusID :
			event=new TA_IRS_App::McaGeneralTransmitStatus(message);
			break;

		case eMsgMcaInboundCallStatusID :
			event=new TA_IRS_App::McaInboundCallStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT InboundCallStatus status = %u, channel = %u", ((McaInboundCallStatus*)event)->get_wStatus(), ((McaInboundCallStatus*)event)->get_channel());
			break;

		case eMsgMcaInboundPttIdStatusID :
			event=new TA_IRS_App::McaInboundPttIdStatus(message);
			break;

		case eMsgMcaBeginInstantTransmitErrorID :
			event=new TA_IRS_App::McaBeginInstantTransmitError(message);
			break;

		case eMsgMcaEndInstantTransmitErrorID :
			event=new TA_IRS_App::McaEndInstantTransmitError(message);
			break;

		case eMsgMcaAssignLoggingAudioDestinationErrorID :
			event=new TA_IRS_App::McaAssignLoggingAudioDestinationError(message);
			break;

		case eMsgMcaLoggingAudioDestinationStatusID :
			event=new TA_IRS_App::McaLoggingAudioDestinationStatus(message);
			break;

		case eMsgMcaLoggingGroupCallStatusID :
			event=new TA_IRS_App::McaLoggingGroupCallStatus(message);
			break;

		case eMsgMcaLoggingPrivateCallEventStatusID :
			event=new TA_IRS_App::McaLoggingPrivateCallEventStatus(message);
			break;

		case eMsgMcaLoggingPrivateCallStatusID :
			event=new TA_IRS_App::McaLoggingPrivateCallStatus(message);
			break;

		case eMsgMcaLoggingTelephoneCallEventStatusID :
			event=new TA_IRS_App::McaLoggingTelephoneCallEventStatus(message);
			break;

		case eMsgMcaLoggingTelephoneCallStatusID :
			event=new TA_IRS_App::McaLoggingTelephoneCallStatus(message);
			break;

		case eMsgMcaAssignLoggingResourceWarningID :
			event=new TA_IRS_App::McaAssignLoggingResourceWarning(message);
			break;

		case eMsgMcaAssignLoggingResourceErrorID :
			event=new TA_IRS_App::McaAssignLoggingResourceError(message);
			break;

		case eMsgMcaDeassignLoggingResourceWarningID :
			event=new TA_IRS_App::McaDeassignLoggingResourceWarning(message);
			break;

		case eMsgMcaDeassignLoggingResourceErrorID :
			event=new TA_IRS_App::McaDeassignLoggingResourceError(message);
			break;

		case eMsgMcaLoggingAssignmentStatusID :
			event=new TA_IRS_App::McaLoggingAssignmentStatus(message);
			break;

		case eMsgMcaAssignIndividualLoggingResourceWarningID :
			event=new TA_IRS_App::McaAssignIndividualLoggingResourceWarning(message);
			break;

		case eMsgMcaAssignIndividualLoggingResourceErrorID :
			event=new TA_IRS_App::McaAssignIndividualLoggingResourceError(message);
			break;

		case eMsgMcaDeassignIndividualLoggingResourceWarningID :
			event=new TA_IRS_App::McaDeassignIndividualLoggingResourceWarning(message);
			break;

		case eMsgMcaDeassignIndividualLoggingResourceErrorID :
			event=new TA_IRS_App::McaDeassignIndividualLoggingResourceError(message);
			break;

		case eMsgMcaLoggingIndividualAssignmentStatusID :
			event=new TA_IRS_App::McaLoggingIndividualAssignmentStatus(message);
			break;

		case eMsgMcaBeginMonitorErrorID :
			event=new TA_IRS_App::McaBeginMonitorError(message);
			break;

		case eMsgMcaEndMonitorErrorID :
			event=new TA_IRS_App::McaEndMonitorError(message);
			break;

		case eMsgMcaMonitorStatusID :
			event=new TA_IRS_App::McaMonitorStatus(message);
			break;

		case eMsgMcaOpenMselErrorID :
			event=new TA_IRS_App::McaOpenMselError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaOpenMselError, error number: %u, group number: %u", ((McaOpenMselError*)event)->get_wErrorNum(), ((McaOpenMselError*)event)->get_wMselGroup());
			break;

		case eMsgMcaCloseMselErrorID :
			event=new TA_IRS_App::McaCloseMselError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaCloseMselError, error number: %u, group number: %u", ((McaCloseMselError*)event)->get_wErrorNum(), ((McaCloseMselError*)event)->get_wMselGroup());
			break;

		case eMsgMcaMselGroupStatusID :
			event=new TA_IRS_App::McaMselGroupStatus(message); 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaMselGroupStatus, status: %u, group number: %u", ((McaMselGroupStatus*)event)->get_wStatus(), ((McaMselGroupStatus*)event)->get_wMselGroup());
			break;

		case eMsgMcaAddMselMemberWarningID :
			event=new TA_IRS_App::McaAddMselMemberWarning(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaAddMselMemberWarning warning message: %u, channel: %u, group number: %u",((McaAddMselMemberWarning*)event)->get_wWarningNum(), ((McaAddMselMemberWarning*)event)->get_channel(), ((McaAddMselMemberWarning*)event)->get_wMselGroup());
			break;

		case eMsgMcaAddMselMemberErrorID :
			event=new TA_IRS_App::McaAddMselMemberError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaAddMselMemberError error message: %u, channel: %u, group number: %u",((McaAddMselMemberError*)event)->get_wErrorNum(), ((McaAddMselMemberError*)event)->get_channel(), ((McaAddMselMemberError*)event)->get_wMselGroup());
			break;

		case eMsgMcaDeleteMselMemberErrorID :
			event=new TA_IRS_App::McaDeleteMselMemberError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaDeleteMselMemberError error message: %u, channel:%u, group number: %u", ((McaDeleteMselMemberError*)event)->get_wErrorNum(), ((McaDeleteMselMemberError*)event)->get_channel(), ((McaDeleteMselMemberError*)event)->get_wMselGroup());
			break;

		case eMsgMcaMselMemberStatusID :
			event=new TA_IRS_App::McaMselMemberStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaMselMemberStatus status: %u, channel: %u, group number: %u", ((McaMselMemberStatus*)event)->get_wStatus(), ((McaMselMemberStatus*)event)->get_channel(), ((McaMselMemberStatus*)event)->get_wMselGroup());
			break;

		case eMsgMcaMselMemberRefreshID :
			event=new TA_IRS_App::McaMselMemberRefresh(message);
			break;

		case eMsgMcaGetFreeMselGroupErrorID :
			event=new TA_IRS_App::McaGetFreeMselGroupError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaGetFreeMselGroupError error number: %u", ((McaGetFreeMselGroupError*)event)->get_wErrorNum());
			break;

		case eMsgMcaFreeMselGroupStatusID :
			event=new TA_IRS_App::McaFreeMselGroupStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaFreeMselGroupStatus group number: %u", ((McaFreeMselGroupStatus*)event)->get_wMselGroup());
			break;

		case eMsgMcaBeginMuteSecondReceiverErrorID :
			event=new TA_IRS_App::McaBeginMuteSecondReceiverError(message);
			break;

		case eMsgMcaEndMuteSecondReceiverErrorID :
			event=new TA_IRS_App::McaEndMuteSecondReceiverError(message);
			break;

		case eMsgMcaMuteSecondReceiverStatusID :
			event=new TA_IRS_App::McaMuteSecondReceiverStatus(message);
			break;

		case eMsgMcaAddPatchMemberWarningID :
			event=new TA_IRS_App::McaAddPatchMemberWarning(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaAddPatchMemberWarning warning message: %u, channel: %u, patch group: %u",((McaAddPatchMemberWarning*)event)->get_wWarningNum(), ((McaAddPatchMemberWarning*)event)->get_channel(),((McaAddPatchMemberWarning*)event)->get_wPatchGroup());
			break;

		case eMsgMcaAddPatchMemberErrorID :
			event=new TA_IRS_App::McaAddPatchMemberError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaAddPatchMemberError error message: %u, channel: %u, patch group: %u",((McaAddPatchMemberError*)event)->get_wErrorNum(), ((McaAddPatchMemberError*)event)->get_channel(),((McaAddPatchMemberError*)event)->get_wPatchGroup());
			break;

		case eMsgMcaDeletePatchMemberErrorID :
			event=new TA_IRS_App::McaDeletePatchMemberError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT MsgMcaDeletePatchMemberError error message: %u, channel: %u", ((McaDeletePatchMemberError*)event)->get_wErrorNum(), ((McaDeletePatchMemberError*)event)->get_channel());
			break;

		case eMsgMcaPatchMemberStatusID :
			event=new TA_IRS_App::McaPatchMemberStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaPatchMemberStatus status: %u, channel: %u, patch group: %u",((McaPatchMemberStatus*)event)->get_wStatus(), ((McaPatchMemberStatus*)event)->get_channel(), ((McaPatchMemberStatus*)event)->get_wPatchGroup());
			break;

		case eMsgMcaPatchGroupStatusID :
			event=new TA_IRS_App::McaPatchGroupStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaPatchGroupStatus status:%u, patch group: %u",((McaPatchGroupStatus*)event)->get_wStatus(), ((McaPatchGroupStatus*)event)->get_wPatchGroup());
			break;

		case eMsgMcaPatchMemberRefreshID :
			event=new TA_IRS_App::McaPatchMemberRefresh(message);
			break;

		case eMsgMcaGetFreePatchGroupErrorID :
			event=new TA_IRS_App::McaGetFreePatchGroupError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaGetFreePatchGroupError error message: %u",((McaGetFreePatchGroupError*)event)->get_wErrorNum());
			break;

		case eMsgMcaFreePatchGroupStatusID :
			event=new TA_IRS_App::McaFreePatchGroupStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaFreePatchGroupStatus: free patch group = %u",((McaFreePatchGroupStatus*)event)->get_wPatchGroup());
			break;

		case eMsgMcaBeginPatchTransmitWarningID :
			event=new TA_IRS_App::McaBeginPatchTransmitWarning(message);
			break;

		case eMsgMcaBeginPatchTransmitErrorID :
			event=new TA_IRS_App::McaBeginPatchTransmitError(message);
			break;

		case eMsgMcaEndPatchTransmitErrorID :
			event=new TA_IRS_App::McaEndPatchTransmitError(message);
			break;

		case eMsgMcaPatchTransmitStatusID :
			event=new TA_IRS_App::McaPatchTransmitStatus(message);
			break;

		case eMsgMcaPeripheralsStatusID :
			event=new TA_IRS_App::McaPeripheralsStatus(message);
			event->printMessage();
			break;

		case eMsgMcaAnswerPrivateCallErrorID :
			event=new TA_IRS_App::McaAnswerPrivateCallError(message);
			break;

		case eMsgMcaBeginPrivateCallErrorID :
			event=new TA_IRS_App::McaBeginPrivateCallError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT : message: %d(0x%x)",((McaBeginPrivateCallError*)event)->get_wErrorNum());
			break;

		case eMsgMcaEndPrivateCallErrorID :
			event=new TA_IRS_App::McaEndPrivateCallError(message);
			event->printMessage();
			break;

		case eMsgMcaInboundPrivateCallStatusID :
			event=new TA_IRS_App::McaInboundPrivateCallStatus(message);		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaInboundPrivateCallStatus: channel = %u, alias = %s, status = %u",((McaInboundPrivateCallStatus*)event)->get_channel(),((McaInboundPrivateCallStatus*)event)->get_IndividualAlias().c_str(), ((McaInboundPrivateCallStatus*)event)->get_wStatus());
			break;

		case eMsgMcaPrivateCallStatusID :
			event=new TA_IRS_App::McaPrivateCallStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaPrivateCallStatus: status = %u",((McaPrivateCallStatus*)event)->get_wStatus());
			break;

		case eMsgMcaRejectPrivateCallErrorID :
			event=new TA_IRS_App::McaRejectPrivateCallError(message);
			break;

		case eMsgMcaSelectPrivateLineErrorID :
			event=new TA_IRS_App::McaSelectPrivateLineError(message);
			break;

		case eMsgMcaResourcePrivateLineStatusID :
			event=new TA_IRS_App::McaResourcePrivateLineStatus(message);
			break;

		case eMsgMcaInboundRadioStatusID :
			event=new TA_IRS_App::McaInboundRadioStatus(message);
			break;

		case eMsgMcaRefreshStatusID :
			event=new TA_IRS_App::McaRefreshStatus(message);
			break;

		case eMsgMcaEnableRepeatErrorID :
			event=new TA_IRS_App::McaEnableRepeatError(message);
			break;

		case eMsgMcaDisableRepeatErrorID :
			event=new TA_IRS_App::McaDisableRepeatError(message);
			break;

		case eMsgMcaRepeatStatusID :
			event=new TA_IRS_App::McaRepeatStatus(message);
			break;

		case eMsgMcaAssignResourceWarningID :
			event=new TA_IRS_App::McaAssignResourceWarning(message);
			event->printMessage();
			break;

		case eMsgMcaAssignResourceErrorID :
			event=new TA_IRS_App::McaAssignResourceError(message);
			event->printMessage();
			break;

		case eMsgMcaDeassignResourceWarningID :
			event=new TA_IRS_App::McaDeassignResourceWarning(message);
			event->printMessage();
			break;

		case eMsgMcaDeassignResourceErrorID :
			event=new TA_IRS_App::McaDeassignResourceError(message);
			event->printMessage();
			break;

		case eMsgMcaAssignmentStatusID :
			event=new TA_IRS_App::McaAssignmentStatus(message);
			event->printMessage();
			break;

		case eMsgMcaResourceRejectStatusID :
			event=new TA_IRS_App::McaResourceRejectStatus(message);
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaResourceRejectStatus: status = %u, channel = %u",((McaResourceRejectStatus*)event)->get_wStatus(), ((McaResourceRejectStatus*)event)->get_channel());
			event->printMessage();
			break;

		case eMsgMcaResourceStatusID :
			event=new TA_IRS_App::McaResourceStatus(message);
			event->printMessage();
			break;

		case eMsgMcaResourceTransmitStatusID :
			event=new TA_IRS_App::McaResourceTransmitStatus(message);
			break;

		case eMsgMcaResourceTransmitWarningID :
			event=new TA_IRS_App::McaResourceTransmitWarning(message);
			break;

		case eMsgMcaSelectResourceErrorID :
			event=new TA_IRS_App::McaSelectResourceError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "SERVER -> CLIENT McaSelectResourceError: error number = %u channel = %u",((McaSelectResourceError*)event)->get_wErrorNum(), ((McaSelectResourceError*)event)->get_channel());
			break;

		case eMsgMcaDeselectResourceErrorID :
			event=new TA_IRS_App::McaDeselectResourceError(message);
			break;

		case eMsgMcaResourceSelectStatusID :
			event=new TA_IRS_App::McaResourceSelectStatus(message);
			break;

		case eMsgMcaSelectPriorityErrorID :
			event=new TA_IRS_App::McaSelectPriorityError(message);
			break;

		case eMsgMcaPriorityStatusID :
			event=new TA_IRS_App::McaPriorityStatus(message);
			break;

		case eMsgMcaBeginTakeoverErrorID :
			event=new TA_IRS_App::McaBeginTakeoverError(message);
			break;

		case eMsgMcaEndTakeoverErrorID :
			event=new TA_IRS_App::McaEndTakeoverError(message);
			break;

		case eMsgMcaTakeoverStatusID :
			event=new TA_IRS_App::McaTakeoverStatus(message);
			break;

		case eMsgMcaBeginToneErrorID :
			event=new TA_IRS_App::McaBeginToneError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaBeginToneError: error number = %u",((McaBeginToneError*)event)->get_wErrorNum());
			break;

		case eMsgMcaEndToneErrorID :
			event=new TA_IRS_App::McaEndToneError(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaEndToneError: error number = %u",((McaEndToneError*)event)->get_wErrorNum());
			break;

		case eMsgMcaToneStatusID :
			event=new TA_IRS_App::McaToneStatus(message);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SERVER -> CLIENT McaToneStatus: tone = %u, status = %u, clientid = %u",((McaToneStatus*)event)->get_tone(), ((McaToneStatus*)event)->get_wStatus(), ((McaToneStatus*)event)->get_byClientId());
			break;

		case eMsgMcaSelectTransmitModeErrorID :
			event=new TA_IRS_App::McaSelectTransmitModeError(message);
			break;

		case eMsgMcaSelectTransmitModeStatusID :
			event=new TA_IRS_App::McaSelectTransmitModeStatus(message);
			break;

		case eMsgMcaTrunkingSystemStatusID :
			event=new TA_IRS_App::McaTrunkingSystemStatus(message);
			break;

		case eMsgMcaChangeVolumeWarningID :
			event=new TA_IRS_App::McaChangeVolumeWarning(message);
			break;

		case eMsgMcaChangeVolumeErrorID :
			event=new TA_IRS_App::McaChangeVolumeError(message);
			break;

		case eMsgMcaResourceVolumeStatusID :
			event=new TA_IRS_App::McaResourceVolumeStatus (message);
			break;

		case eMsgMcaActivateWildcardErrorID :
			event=new TA_IRS_App::McaActivateWildcardError(message);
			break;

		case eMsgMcaDeactivateWildcardErrorID :
			event=new TA_IRS_App::McaDeactivateWildcardError(message);
			break;

		case eMsgMcaWildcardStatusID :
			event=new TA_IRS_App::McaWildcardStatus(message);
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unidentified Message");
		}

		return event;
	}


}