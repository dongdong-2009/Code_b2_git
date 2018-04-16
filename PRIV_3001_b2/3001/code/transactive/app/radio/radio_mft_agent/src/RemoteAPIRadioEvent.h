#pragma once
#include"app/radio/radio_mft_agent/src/RemoteAPI.h"
#include "app/radio/radio_mft_agent/src/RemoteAPICallBackMessage.h"
#include "core/utilities/src/DebugUtil.h"
#include<string>
#include<exception>

namespace TA_IRS_App {
	class ParameterException : public std::exception
	{
		std::string message;
	public:
		ParameterException(std::string s):message(s)
		{}
		std::string& what(){
			return message;
		}
	};
	class RemoteAPIRadioEvent
	{
		RemoteAPIRadioEvent(){}
	protected:
		unsigned short message_id;
		::MCC7500SDK::Parameters params; 
	public:
		virtual ~RemoteAPIRadioEvent(){};
		unsigned short getMessageType()
		{
			return message_id;
		}
		RemoteAPIRadioEvent(::MCC7500SDK::Parameters p):params(p)
		{
			if(p.length()==0)
			{
				ParameterException ex("Parameters are empty!");
				throw ex;
			}
			message_id=p[0].V_UINT16();
		}
		virtual void printMessage() = 0;
	};


	class McdiAudioProcessingStatus : public RemoteAPIRadioEvent  {
	public:
		McdiAudioProcessingStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		void printMessage() {			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),audioProcessingType), get_wStatus());
		}
	};

	class McdiAuthenticationError : public RemoteAPIRadioEvent  {
	public:
		McdiAuthenticationError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientID(){
			return params[1].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getErrorName(get_wErrorNum(),authErrorType), get_wErrorNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u",
																		 get_byClientID());
		}
	};

	class McdiAuthenticationWarning : public RemoteAPIRadioEvent  {
	public:
		McdiAuthenticationWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientID(){
			return params[1].V_UINT8();
		}
		unsigned short get_wWarning(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Warning Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getWarningName(get_wWarning(),authWarningType), get_wWarning());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u",
																		 get_byClientID());
		} 
	};

	class McdiAuthenticationStatus : public RemoteAPIRadioEvent  {
	public:
		McdiAuthenticationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientID(){
			return params[1].V_UINT8();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),authStatusType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u",
																		 get_byClientID());
		} 
	};

	class McdiConsoleShutDown : public RemoteAPIRadioEvent  {
	public:
		McdiConsoleShutDown(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),consoleShutdownType), get_wStatus());
		} 
	};

	class McdiActivateClient : public RemoteAPIRadioEvent  {
	public:
		McdiActivateClient(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wClient(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		void printMessage() {		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client Id: %u",
				get_wClientId());
		} 
	};

	class McdiDeactivateClient : public RemoteAPIRadioEvent  {
	public:
		McdiDeactivateClient(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wClient(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client Id: %u",
				get_wClientId());
		} 
	};

	class McdiClientActivationStatus : public RemoteAPIRadioEvent  {
	public:
		McdiClientActivationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClient(){
			return params[2].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[3].V_UINT16();
		}
		void printMessage() {		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),consoleShutdownType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u",
				get_wClientId());
		} 
	};

	class McdiMessageLoss : public RemoteAPIRadioEvent  {
	public:
		McdiMessageLoss(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McdiClientRegistrationStatus : public RemoteAPIRadioEvent  {
	public:
		McdiClientRegistrationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_cctClient(){
			return params[2].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[3].V_UINT8();
		}
		std::string get_szClientType(){
			return params[4].V_STRING();
		}
		unsigned short get_wClientId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wEventType(){
			return params[6].V_UINT16();
		}
		void printMessage() {		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),clientRegType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "byClient Id: %u, \
																		 CCT Client: %u, \
																		 wClient Id: %u, \
																		 Event Type: %u ",
				get_byClientId(), get_cctClient(),get_wClientId(), get_wEventType() );} 
	};

	class McdiNewDatabaseGranted : public RemoteAPIRadioEvent  {
	public:
		McdiNewDatabaseGranted(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McdiNewDatabaseNotGranted : public RemoteAPIRadioEvent  {
	public:
		McdiNewDatabaseNotGranted(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McdiNewDatabaseAvailable : public RemoteAPIRadioEvent  {
	public:
		McdiNewDatabaseAvailable(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McdiDatabaseStatus : public RemoteAPIRadioEvent  {
	public:
		McdiDatabaseStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned int get_dwVersionNumber(){
			return params[2].V_UINT32();
		}
		unsigned int get_dwRestoreVersionNumber(){
			return params[3].V_UINT32();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),dbStatusType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "dw Version Number: %u, \
																		 dw Restore Version Number: %u",
				get_dwVersionNumber(), get_dwRestoreVersionNumber());
		} 
	};

	class McaBeginAllMuteError : public RemoteAPIRadioEvent  {
	public:
		McaBeginAllMuteError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McaEndAllMuteError : public RemoteAPIRadioEvent  {
	public:
		McaEndAllMuteError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McaAllMuteStatus : public RemoteAPIRadioEvent  {
	public:
		McaAllMuteStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginAmbienceListeningCallError : public RemoteAPIRadioEvent  {
	public:
		McaBeginAmbienceListeningCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned char get_byClientId(){
			return params[4].V_UINT8();
		}
	void printMessage() {} };

	class McaEndAmbienceListeningCallError : public RemoteAPIRadioEvent  {
	public:
		McaEndAmbienceListeningCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaAmbienceListeningCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaAmbienceListeningCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[7].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginAPBTransmitWarning : public RemoteAPIRadioEvent  {
	public:
		McaBeginAPBTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[2].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginAPBTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaBeginAPBTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[2].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaEndAPBTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaEndAPBTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaAPBTransmitStatus : public RemoteAPIRadioEvent  {
	public:
		McaAPBTransmitStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignAudioDestinationWarning : public RemoteAPIRadioEvent  {
	public:
		McaAssignAudioDestinationWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_selectSpeaker(){
			return params[2].V_UINT8();
		}
		unsigned char get_unselectSpeaker(){
			return params[3].V_UINT8();
		}
		unsigned char get_byHeadsetSelect(){
			return params[4].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[5].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignAudioDestinationError : public RemoteAPIRadioEvent  {
	public:
		McaAssignAudioDestinationError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_selectSpeaker(){
			return params[2].V_UINT8();
		}
		unsigned char get_unselectSpeaker(){
			return params[3].V_UINT8();
		}
		unsigned char get_byHeadsetSelect(){
			return params[4].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[5].V_UINT16();
		}
	void printMessage() {} };

	class McaAudioDestinationStatus : public RemoteAPIRadioEvent  {
	public:
		McaAudioDestinationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_selectSpeaker(){
			return params[2].V_UINT8();
		}
		unsigned char get_unselectSpeaker(){
			return params[3].V_UINT8();
		}
		unsigned char get_byHeadsetSelect(){
			return params[4].V_UINT8();
		}
		unsigned short get_wStatusType(){
			return params[5].V_UINT16();
		}
	void printMessage() {} };

	class McaSoundBeepsError : public RemoteAPIRadioEvent  {
	public:
		McaSoundBeepsError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byNumberOfBeeps(){
			return params[1].V_UINT8();
		}
		unsigned char get_byDuration(){
			return params[2].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaSelectCriticalResourceSettingError : public RemoteAPIRadioEvent  {
	public:
		McaSelectCriticalResourceSettingError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaCriticalResourceStatus : public RemoteAPIRadioEvent  {
	public:
		McaCriticalResourceStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDatabaseUpdateNotificationStatus : public RemoteAPIRadioEvent  {
	public:
		McaDatabaseUpdateNotificationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
	void printMessage() {} };

	class McaAcknowledgeEmergencyError : public RemoteAPIRadioEvent  {
	public:
		McaAcknowledgeEmergencyError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Error Code (Id):%s(%u), \
																		Channel:%u, \
																		Unit Id: %u",
																		RemoteAPICallBackMessage::getErrorName(get_wErrorNum(),errorUndefined), get_wErrorNum(),
																		get_channel(),
																		get_unit_id()
																		);
		} 
	};

	class McaEmergencyAlarmStatus : public RemoteAPIRadioEvent  {
	public:
		McaEmergencyAlarmStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[6].V_UINT16();
		}
		unsigned int get_resource(){
			return params[7].V_UINT32();
		}
		unsigned int get_console_unit_id(){
			return params[8].V_UINT32();
		}
		std::string get_ConsoleIndividualAlias(){
			return params[9].V_STRING();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),emergencyAlarmStatusType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel Id: %u, \
																		 Unit Id: %u, \
																		 Zone Id: %u, \
																		 Site Id: %u, \
																		 Resource Id: %u, \
																		 Console unit Id: %u",
																		 get_channel(), get_unit_id(), 
																		 get_wZoneId(), get_wSiteId(), get_resource(), 
																		 get_console_unit_id());
		} 
	};

	class McaBeginConsoleEmergencyCallError : public RemoteAPIRadioEvent  {
	public:
		McaBeginConsoleEmergencyCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[3].V_UINT8();
		}
		void printMessage() {			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getErrorName(get_wErrorNum()), get_wErrorNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel Id: %u, \
																		 Client Id: %u",
																		 get_channel(), get_byClientId());
		} 
	};

	class McaEndEmergencyError : public RemoteAPIRadioEvent  {
	public:
		McaEndEmergencyError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error Code(Id): %s(%u)",
				(get_wErrorNum() == 0x0400) ? "MCA_EMAC_RESTRICTED_OP" : RemoteAPICallBackMessage::getErrorName(get_wErrorNum()), get_wErrorNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel Id: %u",
																		 get_channel());
		} 
	};

	class McaEmergencyCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaEmergencyCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned short get_wRcvMode(){
			return params[5].V_UINT16();
		}
		unsigned short get_wRcvSecureKey(){
			return params[6].V_UINT16();
		}
		unsigned short get_wZoneId(){
			return params[7].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[8].V_UINT16();
		}
		unsigned int get_resource(){
			return params[9].V_UINT32();
		}
		unsigned short get_wInitiator(){
			return params[10].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),emergencyCallStatusType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel Id: %u, \
																		 Unit Id: %u, \
																		 Received Mode: %u, \
																		 Zone Id: %u, \
																		 Site Id: %u, \
																		 Resource Id: %u",
																		 get_channel(), get_unit_id(), get_wRcvMode(),
																		 get_wZoneId(), get_wSiteId(), get_resource());
		} 
	};

	class McaSelectFrequencyError : public RemoteAPIRadioEvent  {
	public:
		McaSelectFrequencyError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wFrequency(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaResourceFrequencyStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourceFrequencyStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wFrequency(){
			return params[2].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[3].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[4].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginGeneralTransmitWarning : public RemoteAPIRadioEvent  {
	public:
		McaBeginGeneralTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginGeneralTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaBeginGeneralTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaEndGeneralTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaEndGeneralTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaGeneralTransmitStatus : public RemoteAPIRadioEvent  {
	public:
		McaGeneralTransmitStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaInboundCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaInboundCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned short get_wSelectStatus(){
			return params[3].V_UINT16();
		}
		unsigned short get_wRcvMode(){
			return params[4].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSelectStatusEx(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaInboundPttIdStatus : public RemoteAPIRadioEvent  {
	public:
		McaInboundPttIdStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[2].V_UINT32();
		}
		unsigned short get_wRcvMode(){
			return params[3].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[4].V_UINT16();
		}
		unsigned short get_wRcvSecureKey(){
			return params[5].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[6].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[7].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[8].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginInstantTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaBeginInstantTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[2].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaEndInstantTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaEndInstantTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignLoggingAudioDestinationError : public RemoteAPIRadioEvent  {
	public:
		McaAssignLoggingAudioDestinationError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_S_addr(){
			return params[2].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[3].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[4].V_UINT16();
		}
	void printMessage() {} };

	class McaLoggingAudioDestinationStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingAudioDestinationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_S_addr(){
			return params[2].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[3].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[4].V_UINT16();
		}
	void printMessage() {} };

	class McaLoggingGroupCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingGroupCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMessageType(){
			return params[2].V_UINT16();
		}
		unsigned int get_dwCallHndl1(){
			return params[3].V_UINT32();
		}
		unsigned int get_dwCallHndl2(){
			return params[4].V_UINT32();
		}
		unsigned int get_dwCallHndl3(){
			return params[5].V_UINT32();
		}
		unsigned short get_wCallType(){
			return params[6].V_UINT16();
		}
		unsigned short get_wAudioEncoding(){
			return params[7].V_UINT16();
		}
		unsigned short get_wEncryptionState(){
			return params[8].V_UINT16();
		}
		unsigned short get_wSecureKey(){
			return params[9].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[10].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[11].V_STRING();
		}
		unsigned short get_wZoneID(){
			return params[12].V_UINT16();
		}
		unsigned short get_wSiteID(){
			return params[13].V_UINT16();
		}
		unsigned short get_wAudioAvailable(){
			return params[14].V_UINT16();
		}
		unsigned short get_wFrequency(){
			return params[15].V_UINT16();
		}
		unsigned short get_wPlCode(){
			return params[16].V_UINT16();
		}
	void printMessage() {} };

	class McaLoggingPrivateCallEventStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingPrivateCallEventStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channelId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMessageEventType(){
			return params[2].V_UINT16();
		}
		unsigned int get_requestorUnitId(){
			return params[3].V_UINT32();
		}
		std::string get_requestorIndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned int get_targetUnitId(){
			return params[5].V_UINT32();
		}
		std::string get_targetIndividualAlias(){
			return params[6].V_STRING();
		}
		unsigned short get_wEndOfCallReason(){
			return params[7].V_UINT16();
		}
		unsigned short get_wZoneID(){
			return params[8].V_UINT16();
		}
		unsigned short get_wSiteID(){
			return params[9].V_UINT16();
		}
		unsigned short get_wCallType(){
			return params[10].V_UINT16();
		}
		unsigned int get_virtualRequestorUnitId(){
			return params[11].V_UINT32();
		}
		std::string get_virtualRequestorIndividualAlias(){
			return params[12].V_STRING();
		}
		unsigned int get_virtualTargetUnitId(){
			return params[13].V_UINT32();
		}
		std::string get_virtualTargetIndividualAlias(){
			return params[14].V_STRING();
		}
		bool get_bVirtualRequestorIdPresented(){
			return params[15].V_BOOLEAN();
		}
		bool get_bVirtualTargetIdPresented(){
			return params[16].V_BOOLEAN();
		}
	void printMessage() {} };

	class McaLoggingPrivateCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingPrivateCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channelId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMessageType(){
			return params[2].V_UINT16();
		}
		unsigned int get_dwCallHandle1(){
			return params[3].V_UINT32();
		}
		unsigned int get_dwCallHandle2(){
			return params[4].V_UINT32();
		}
		unsigned int get_dwCallHandle3(){
			return params[5].V_UINT32();
		}
		unsigned short get_wAudioEncoding(){
			return params[6].V_UINT16();
		}
		unsigned short get_wEncryptionState(){
			return params[7].V_UINT16();
		}
		unsigned short get_wSecureKey(){
			return params[8].V_UINT16();
		}
		unsigned int get_requestorUnitId(){
			return params[9].V_UINT32();
		}
		std::string get_requestorIndividualAlias(){
			return params[10].V_STRING();
		}
		unsigned int get_targetUnitId(){
			return params[11].V_UINT32();
		}
		std::string get_targetIndividualAlias(){
			return params[12].V_STRING();
		}
		unsigned short get_wZoneID(){
			return params[13].V_UINT16();
		}
		unsigned short get_wSiteID(){
			return params[14].V_UINT16();
		}
		unsigned short get_wAudioAvailable(){
			return params[15].V_UINT16();
		}
		unsigned int get_virtualRequestorUnitId(){
			return params[16].V_UINT32();
		}
		std::string get_virtualRequestorIndividualAlias(){
			return params[17].V_STRING();
		}
		unsigned int get_virtualTargetUnitId(){
			return params[18].V_UINT32();
		}
		std::string get_virtualTargetIndividualAlias(){
			return params[19].V_STRING();
		}
		bool get_bVirtualRequestorIdPresented(){
			return params[20].V_BOOLEAN();
		}
		bool get_bVirtualTargetIdPresented(){
			return params[21].V_BOOLEAN();
		}
	void printMessage() {} };

	class McaLoggingTelephoneCallEventStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingTelephoneCallEventStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channelId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMessageEventType(){
			return params[2].V_UINT16();
		}
		unsigned int get_unitId(){
			return params[3].V_UINT32();
		}
		std::string get_individualAlias(){
			return params[4].V_STRING();
		}
		std::string get_isdnNum(){
			return params[5].V_STRING();
		}
		unsigned short get_wRequestDirection(){
			return params[6].V_UINT16();
		}
		unsigned short get_wEndOfCallReason(){
			return params[7].V_UINT16();
		}
		unsigned short get_wZoneID(){
			return params[8].V_UINT16();
		}
		unsigned short get_wSiteID(){
			return params[9].V_UINT16();
		}
		unsigned int get_virtualUnitId(){
			return params[10].V_INT32();
		}
		std::string get_virtualIndividualAlias(){
			return params[11].V_STRING();
		}
		bool get_bVirtualUnitIdPresented(){
			return params[12].V_BOOLEAN();
		}
	void printMessage() {} };

	class McaLoggingTelephoneCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingTelephoneCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channelId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMessageType(){
			return params[2].V_UINT16();
		}
		unsigned int get_dwCallHandle1(){
			return params[3].V_UINT32();
		}
		unsigned int get_dwCallHandle2(){
			return params[4].V_UINT32();
		}
		unsigned int get_dwCallHandle3(){
			return params[5].V_UINT32();
		}
		unsigned short get_wAudioEncoding(){
			return params[6].V_UINT16();
		}
		unsigned short get_wEncryptionState(){
			return params[7].V_UINT16();
		}
		unsigned short get_wSecureKey(){
			return params[8].V_UINT16();
		}
		unsigned int get_unitId(){
			return params[9].V_UINT32();
		}
		std::string get_individualAlias(){
			return params[10].V_STRING();
		}
		std::string get_isdnNum(){
			return params[11].V_STRING();
		}
		unsigned short get_wRequestDirection(){
			return params[12].V_UINT16();
		}
		unsigned short get_wZoneID(){
			return params[13].V_UINT16();
		}
		unsigned short get_wSiteID(){
			return params[14].V_UINT16();
		}
		unsigned short get_wAudioAvailable(){
			return params[15].V_UINT16();
		}
		unsigned int get_virtualUnitId(){
			return params[16].V_UINT32();
		}
		std::string get_virtualIndividualAlias(){
			return params[17].V_STRING();
		}
		bool get_bVirtualUnitIdPresented(){
			return params[18].V_BOOLEAN();
		}
	void printMessage() {} };

	class McaAssignLoggingResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaAssignLoggingResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned int get_resource(){
			return params[1].V_UINT32();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		unsigned int get_S_addr(){
			return params[3].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[4].V_UINT16();
		}
		unsigned short get_wLogEncryptedAudio(){
			return params[5].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[6].V_UINT16();
		}
		unsigned short get_wWarningNum(){
			return params[7].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignLoggingResourceError : public RemoteAPIRadioEvent  {
	public:
		McaAssignLoggingResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned int get_resource(){
			return params[1].V_UINT32();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		unsigned int get_S_addr(){
			return params[3].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[4].V_UINT16();
		}
		unsigned short get_wLogEncryptedAudio(){
			return params[5].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[6].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[7].V_UINT16();
		}
	void printMessage() {} };

	class McaDeassignLoggingResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaDeassignLoggingResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		unsigned short get_wWarningNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDeassignLoggingResourceError : public RemoteAPIRadioEvent  {
	public:
		McaDeassignLoggingResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaLoggingAssignmentStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingAssignmentStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wClientId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_resource(){
			return params[3].V_UINT32();
		}
		unsigned short get_channel(){
			return params[4].V_UINT16();
		}
		bool get_bLastAssignedLcid(){
			return params[5].V_BOOLEAN();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignIndividualLoggingResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaAssignIndividualLoggingResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned int get_lowUnitId(){
			return params[1].V_UINT32();
		}
		unsigned int get_highUnitId(){
			return params[2].V_UINT32();
		}
		unsigned short get_wClientId(){
			return params[3].V_UINT16();
		}
		unsigned int get_audioDest(){
			return params[4].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[5].V_UINT16();
		}
		unsigned short get_wLogEncryptedAudio(){
			return params[6].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[7].V_UINT16();
		}
		unsigned short get_wWarningNum(){
			return params[8].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignIndividualLoggingResourceError : public RemoteAPIRadioEvent  {
	public:
		McaAssignIndividualLoggingResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned int get_lowUnitId(){
			return params[1].V_UINT32();
		}
		unsigned int get_highUnitId(){
			return params[2].V_UINT32();
		}
		unsigned short get_wClientId(){
			return params[3].V_UINT16();
		}
		unsigned int get_audioDest(){
			return params[4].V_UINT32();
		}
		unsigned short get_wPort(){
			return params[5].V_UINT16();
		}
		unsigned short get_wLogEncryptedAudio(){
			return params[6].V_UINT16();
		}
		unsigned short get_wCriticalResource(){
			return params[7].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[8].V_UINT16();
		}
	void printMessage() {} };

	class McaDeassignIndividualLoggingResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaDeassignIndividualLoggingResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channelId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[2].V_UINT16();
		}
		unsigned short get_wWarningNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDeassignIndividualLoggingResourceError : public RemoteAPIRadioEvent  {
	public:
		McaDeassignIndividualLoggingResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_lowChannel(){
			return params[1].V_UINT16();
		}
		unsigned short get_highChannel(){
			return params[2].V_UINT16();
		}
		unsigned short get_wClientId(){
			return params[3].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[4].V_UINT16();
		}
	void printMessage() {} };

	class McaLoggingIndividualAssignmentStatus : public RemoteAPIRadioEvent  {
	public:
		McaLoggingIndividualAssignmentStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wClientId(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unitId(){
			return params[3].V_UINT32();
		}
		unsigned short get_channel(){
			return params[4].V_UINT16();
		}
		bool get_bLastAssignedLcid(){
			return params[5].V_BOOLEAN();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginMonitorError : public RemoteAPIRadioEvent  {
	public:
		McaBeginMonitorError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaEndMonitorError : public RemoteAPIRadioEvent  {
	public:
		McaEndMonitorError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaMonitorStatus : public RemoteAPIRadioEvent  {
	public:
		McaMonitorStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaOpenMselError : public RemoteAPIRadioEvent  {
	public:
		McaOpenMselError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaCloseMselError : public RemoteAPIRadioEvent  {
	public:
		McaCloseMselError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaMselGroupStatus : public RemoteAPIRadioEvent  {
	public:
		McaMselGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaAddMselMemberWarning : public RemoteAPIRadioEvent  {
	public:
		McaAddMselMemberWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wWarningNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaAddMselMemberError : public RemoteAPIRadioEvent  {
	public:
		McaAddMselMemberError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDeleteMselMemberError : public RemoteAPIRadioEvent  {
	public:
		McaDeleteMselMemberError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaMselMemberStatus : public RemoteAPIRadioEvent  {
	public:
		McaMselMemberStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wMselGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaMselMemberRefresh : public RemoteAPIRadioEvent  {
	public:
		McaMselMemberRefresh(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_startChannel(){
			return params[2].V_UINT16();
		}
		unsigned char get_byMselGroupStatus(){
			return params[3].V_UINT8();
		}
		MCC7500SDK::Parameters get_abyMselBitmap(){
			return params[4].V_SEQUENCE();
		}
	void printMessage() {} };

	class McaGetFreeMselGroupError : public RemoteAPIRadioEvent  {
	public:
		McaGetFreeMselGroupError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
	void printMessage() {} };

	class McaFreeMselGroupStatus : public RemoteAPIRadioEvent  {
	public:
		McaFreeMselGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wMselGroup(){
			return params[1].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginMuteSecondReceiverError : public RemoteAPIRadioEvent  {
	public:
		McaBeginMuteSecondReceiverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaEndMuteSecondReceiverError : public RemoteAPIRadioEvent  {
	public:
		McaEndMuteSecondReceiverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaMuteSecondReceiverStatus : public RemoteAPIRadioEvent  {
	public:
		McaMuteSecondReceiverStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[3].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[4].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaAddPatchMemberWarning : public RemoteAPIRadioEvent  {
	public:
		McaAddPatchMemberWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wWarningNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned short get_wPatchGroup(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaAddPatchMemberError : public RemoteAPIRadioEvent  {
	public:
		McaAddPatchMemberError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned short get_wPatchGroup(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDeletePatchMemberError : public RemoteAPIRadioEvent  {
	public:
		McaDeletePatchMemberError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaPatchMemberStatus : public RemoteAPIRadioEvent  {
	public:
		McaPatchMemberStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned short get_wPatchGroup(){
			return params[3].V_UINT16();
		}
		unsigned int get_urid(){
			return params[4].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[5].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[7].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[8].V_UINT16();
		}
	void printMessage() {} };

	class McaPatchGroupStatus : public RemoteAPIRadioEvent  {
	public:
		McaPatchGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		unsigned short get_wPatchGroup(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaPatchMemberRefresh : public RemoteAPIRadioEvent  {
	public:
		McaPatchMemberRefresh(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_startChannel(){
			return params[2].V_UINT16();
		}
		unsigned char get_byPatchGroupStatus(){
			return params[3].V_UINT8();
		}
		MCC7500SDK::Parameters get_abyPatchBitmap(){
			return params[4].V_SEQUENCE();
		}
	void printMessage() {} };

	class McaGetFreePatchGroupError : public RemoteAPIRadioEvent  {
	public:
		McaGetFreePatchGroupError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
	void printMessage() {} };

	class McaFreePatchGroupStatus : public RemoteAPIRadioEvent  {
	public:
		McaFreePatchGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginPatchTransmitWarning : public RemoteAPIRadioEvent  {
	public:
		McaBeginPatchTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[2].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginPatchTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaBeginPatchTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[2].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaEndPatchTransmitError : public RemoteAPIRadioEvent  {
	public:
		McaEndPatchTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaPatchTransmitStatus : public RemoteAPIRadioEvent  {
	public:
		McaPatchTransmitStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wPatchGroup(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaPeripheralsStatus : public RemoteAPIRadioEvent  {
	public:
		McaPeripheralsStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		void printMessage() {			
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getStatusName(get_wStatus(),peripheralStatusType), get_wStatus());
		}
	};

	class McaAnswerPrivateCallError : public RemoteAPIRadioEvent  {
	public:
		McaAnswerPrivateCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned char get_byClientId(){
			return params[4].V_UINT8();
		}
	void printMessage() {} };

	class McaBeginPrivateCallError : public RemoteAPIRadioEvent  {
	public:
		McaBeginPrivateCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned char get_byClientId(){
			return params[4].V_UINT8();
		}
	void printMessage() {} };

	class McaEndPrivateCallError : public RemoteAPIRadioEvent  {
	public:
		McaEndPrivateCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaInboundPrivateCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaInboundPrivateCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned short get_wRcvMode(){
			return params[4].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[5].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[7].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[8].V_UINT16();
		}
		unsigned int get_HeadNumber(){
			return params[9].V_UINT32();
		}
		std::string get_HeadNumberAlias(){
			return params[10].V_STRING();
		}
	void printMessage() {} };

	class McaPrivateCallStatus : public RemoteAPIRadioEvent  {
	public:
		McaPrivateCallStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[7].V_UINT16();
		}
		unsigned short get_wCallMode(){
			return params[8].V_UINT16();
		}
	void printMessage() {} };

	class McaRejectPrivateCallError : public RemoteAPIRadioEvent  {
	public:
		McaRejectPrivateCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned char get_byClientId(){
			return params[4].V_UINT8();
		}
		unsigned int get_retry_id(){
			return params[5].V_UINT32();
		}
	void printMessage() {} };

	class McaSelectPrivateLineError : public RemoteAPIRadioEvent  {
	public:
		McaSelectPrivateLineError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wPlCode(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaResourcePrivateLineStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourcePrivateLineStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wPlCode(){
			return params[2].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[3].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[4].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
	void printMessage() {} };

	class McaInboundRadioStatus : public RemoteAPIRadioEvent  {
	public:
		McaInboundRadioStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[2].V_UINT32();
		}
		unsigned short get_wStatus(){
			return params[3].V_UINT16();
		}
		unsigned short get_wRadioStatus(){
			return params[4].V_UINT16();
		}
		unsigned short get_wTimeMinutes(){
			return params[5].V_UINT16();
		}
		unsigned short get_wPrimarySetID(){
			return params[6].V_UINT16();
		}
		unsigned short get_wSecondarySetID(){
			return params[7].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[8].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[9].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[10].V_UINT16();
		}
	void printMessage() {} };

	class McaRefreshStatus : public RemoteAPIRadioEvent  {
	public:
		McaRefreshStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wStatus(){
			return params[1].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u)",
																		 RemoteAPICallBackMessage::getStatusName(get_wStatus(),statusUndefined),get_wStatus());
		} 
	};

	class McaEnableRepeatError : public RemoteAPIRadioEvent  {
	public:
		McaEnableRepeatError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaDisableRepeatError : public RemoteAPIRadioEvent  {
	public:
		McaDisableRepeatError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaRepeatStatus : public RemoteAPIRadioEvent  {
	public:
		McaRepeatStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		unsigned char get_IndividualAlias(){
			return params[4].V_UINT8();
		}
		unsigned short get_wZoneId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[7].V_UINT16();
		}
	void printMessage() {} };

	class McaAssignResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaAssignResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned int get_resource(){
			return params[2].V_UINT32();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
		unsigned char get_selectSpeaker(){
			return params[4].V_UINT8();
		}
		unsigned char get_unselectSpeaker(){
			return params[5].V_UINT8();
		}
		unsigned char get_byVolumeLevel(){
			return params[6].V_UINT8();
		}
		unsigned short get_wCriticalResource(){
			return params[7].V_UINT16();
		}
		unsigned char get_byHeadsetSelect(){
			return params[8].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[9].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Warning Code(Id): %s(%u)",
																		 RemoteAPICallBackMessage::getWarningName(get_wWarningNum(),warningUndefined), get_wWarningNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u, \
																		 Resource Id: %u, \
																		 Channel Id: %u, \
																		 Selected Speaker Id: %u, \
																		 Unselected Speaker Id: %u, \
																		 Volume Level: %u, \
																		 Critical Resource: %u, \
																		 Header Select: %u",
																		 get_byClientId(), get_resource(), get_channel(), get_selectSpeaker(), 
																		 get_unselectSpeaker(), get_byVolumeLevel(), get_wCriticalResource(), 
																		 get_byHeadsetSelect());
		} 
	};

	class McaAssignResourceError : public RemoteAPIRadioEvent  {
	public:
		McaAssignResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned int get_resource(){
			return params[2].V_UINT32();
		}
		unsigned short get_channel(){
			return params[3].V_UINT16();
		}
		unsigned char get_selectSpeaker(){
			return params[4].V_UINT8();
		}
		unsigned char get_unselectSpeaker(){
			return params[5].V_UINT8();
		}
		unsigned char get_byVolumeLevel(){
			return params[6].V_UINT8();
		}
		unsigned short get_wCriticalResource(){
			return params[7].V_UINT16();
		}
		unsigned char get_byHeadsetSelect(){
			return params[8].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[9].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getErrorName(get_wErrorNum(),rscAssignmentErrorType), get_wErrorNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u, \
																		 Resource Id: %u, \
																		 Channel Id: %u, \
																		 Selected Speaker Id: %u, \
																		 Unselected Speaker Id: %u, \
																		 Volume Level: %u, \
																		 Critical Resource: %u, \
																		 Header Select: %u",
																		 get_byClientId(), get_resource(), get_channel(), get_selectSpeaker(), 
																		 get_unselectSpeaker(), get_byVolumeLevel(), get_wCriticalResource(), 
																		 get_byHeadsetSelect());
		} 
	};

	class McaDeassignResourceWarning : public RemoteAPIRadioEvent  {
	public:
		McaDeassignResourceWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned char get_byPriority(){
			return params[3].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[4].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Warning Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getWarningName(get_wWarningNum()), get_wWarningNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u, \
																		 Channel Id: %u, \
																		 Priority: %u",
																		 get_byClientId(), get_channel(), get_byPriority());
		} 
	};

	class McaDeassignResourceError : public RemoteAPIRadioEvent  {
	public:
		McaDeassignResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_channel(){
			return params[2].V_UINT16();
		}
		unsigned char get_byPriority(){
			return params[3].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[4].V_UINT16();
		}
		void printMessage() {		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Error Code(Id): %s(%u)",
				RemoteAPICallBackMessage::getErrorName(get_wErrorNum(),rscAssignmentErrorType), get_wErrorNum());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u, \
																		 Channel Id: %u, \
																		 Priority: %u",
																		 get_byClientId(), get_channel(), get_byPriority());
		} 
	};

	class McaAssignmentStatus : public RemoteAPIRadioEvent  {
	public:
		McaAssignmentStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned char get_byClientId(){
			return params[1].V_UINT8();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned int get_resource(){
			return params[3].V_UINT32();
		}
		unsigned short get_channel(){
			return params[4].V_UINT16();
		}
		bool get_bLastAssignedLcid(){
			return params[5].V_BOOLEAN();
		}
		unsigned short get_wStatusType(){
			return params[6].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code(Id): %s(%u), \
																		 Channel: %u, \
																		 Resource Id: %u",
																		 RemoteAPICallBackMessage::getStatusName(get_wStatus(),resourceAssignmentStatusType), get_wStatus(),
																		 get_channel(),
																		 get_resource()
																		 );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client Id: %u, \
																		 Last Assigned LC_ID: %u, \
																		 Status Type: %s",
																		 get_byClientId(),
																		 get_bLastAssignedLcid(),
																		 (get_wStatusType() == 0) ? "MCA_STATUS_UPDATE" : "MCA_STATUS_REFRESH");

		} 
	};

	class McaResourceRejectStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourceRejectStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		void printMessage() {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code: %u, \
																		 Channel: %u",
																		 get_wStatus(),
																		 get_channel()
																		 );
		} 
	};

	class McaResourceStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourceStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_auxio_id(){
			return params[2].V_UINT32();
		}
		unsigned short get_wStatus(){
			return params[3].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[4].V_UINT16();
		}
		void printMessage() {		
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "MCC7500 SERVER -> CLIENT Message Details:");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Status Code: %s(%u)",
																		 RemoteAPICallBackMessage::getStatusName(get_wStatus(),rscSelectStatusType), get_wStatus());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Channel Id: %u, \
																		 Auxio Id: %u, \
																		 Status Type: %s",
																		 get_channel(),
																		 get_auxio_id(),
																		 (get_wStatusType() == 0) ? "MCA_STATUS_UPDATE" : "MCA_STATUS_REFRESH");
		} 
	};

	class McaResourceTransmitStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourceTransmitStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned int get_wStatus(){
			return params[2].V_UINT32();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
		unsigned short get_wTransmitMode(){
			return params[4].V_UINT16();
		}
	void printMessage() {} };

	class McaResourceTransmitWarning : public RemoteAPIRadioEvent  {
	public:
		McaResourceTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wWarningNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaSelectResourceError : public RemoteAPIRadioEvent  {
	public:
		McaSelectResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaDeselectResourceError : public RemoteAPIRadioEvent  {
	public:
		McaDeselectResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaResourceSelectStatus : public RemoteAPIRadioEvent  {
	public:
		McaResourceSelectStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaSelectPriorityError : public RemoteAPIRadioEvent  {
	public:
		McaSelectPriorityError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wPriority(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaPriorityStatus : public RemoteAPIRadioEvent  {
	public:
		McaPriorityStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wPriority(){
			return params[2].V_UINT16();
		}
		unsigned int get_unit_id(){
			return params[3].V_UINT32();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned int get_wZoneId(){
			return params[5].V_UINT32();
		}
		unsigned int get_wSiteId(){
			return params[6].V_UINT32();
		}
		unsigned int get_wStatusType(){
			return params[7].V_UINT32();
		}
	void printMessage() {} };

	class McaBeginTakeoverError : public RemoteAPIRadioEvent  {
	public:
		McaBeginTakeoverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaEndTakeoverError : public RemoteAPIRadioEvent  {
	public:
		McaEndTakeoverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[2].V_UINT16();
		}
	void printMessage() {} };

	class McaTakeoverStatus : public RemoteAPIRadioEvent  {
	public:
		McaTakeoverStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaBeginToneError : public RemoteAPIRadioEvent  {
	public:
		McaBeginToneError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
		unsigned int get_tone(){
			return params[2].V_UINT32();
		}
		unsigned char get_byClientId(){
			return params[3].V_UINT8();
		}
	void printMessage() {} };

	class McaEndToneError : public RemoteAPIRadioEvent  {
	public:
		McaEndToneError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wErrorNum(){
			return params[1].V_UINT16();
		}
	void printMessage() {} };

	class McaToneStatus : public RemoteAPIRadioEvent  {
	public:
		McaToneStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned int get_tone(){
			return params[1].V_UINT32();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned char get_byClientId(){
			return params[3].V_UINT8();
		}
	void printMessage() {} };

	class McaSelectTransmitModeError : public RemoteAPIRadioEvent  {
	public:
		McaSelectTransmitModeError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wTxMode(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaSelectTransmitModeStatus : public RemoteAPIRadioEvent  {
	public:
		McaSelectTransmitModeStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wTxMode(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaTrunkingSystemStatus : public RemoteAPIRadioEvent  {
	public:
		McaTrunkingSystemStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_wSystemID(){
			return params[1].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaChangeVolumeWarning : public RemoteAPIRadioEvent  {
	public:
		McaChangeVolumeWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_byVolumeLevel(){
			return params[2].V_UINT8();
		}
		unsigned short get_wWarningNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaChangeVolumeError : public RemoteAPIRadioEvent  {
	public:
		McaChangeVolumeError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_byVolumeLevel(){
			return params[2].V_UINT8();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaResourceVolumeStatus  : public RemoteAPIRadioEvent  {
	public:
		McaResourceVolumeStatus (const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_Channel(){
			return params[1].V_UINT16();
		}
		unsigned char get_byVolumeLevel(){
			return params[2].V_UINT8();
		}
		unsigned short get_wStatusType(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaActivateWildcardError : public RemoteAPIRadioEvent  {
	public:
		McaActivateWildcardError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wWildcardNumber(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaDeactivateWildcardError : public RemoteAPIRadioEvent  {
	public:
		McaDeactivateWildcardError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wWildcardNumber(){
			return params[2].V_UINT16();
		}
		unsigned short get_wErrorNum(){
			return params[3].V_UINT16();
		}
	void printMessage() {} };

	class McaWildcardStatus : public RemoteAPIRadioEvent  {
	public:
		McaWildcardStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
		unsigned short get_channel(){
			return params[1].V_UINT16();
		}
		unsigned short get_wWildcardNumber(){
			return params[2].V_UINT16();
		}
		unsigned short get_wStatus(){
			return params[3].V_UINT16();
		}
		std::string get_IndividualAlias(){
			return params[4].V_STRING();
		}
		unsigned short get_wZoneId(){
			return params[5].V_UINT16();
		}
		unsigned short get_wSiteId(){
			return params[6].V_UINT16();
		}
		unsigned short get_wStatusType(){
			return params[7].V_UINT16();
		}
	void printMessage() {} };

	class McdiDatabaseUpdateNotification : public RemoteAPIRadioEvent {
	public:
		static const short DB_UPDATE_TALKGROUP = 2;
		static const short DB_UNIT_ALIASES = 11;
		McdiDatabaseUpdateNotification(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
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
				message, get_wDataGroup());
		}
	};

}