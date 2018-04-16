#pragma once
#include"RemoteAPI.h"
#include<string>
#include<exception>
namespace ICDV4{
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
};


class McdiAudioProcessingStatus : public RemoteAPIRadioEvent  {
public:
McdiAudioProcessingStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
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
};

class McdiConsoleShutDown : public RemoteAPIRadioEvent  {
public:
McdiConsoleShutDown(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
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
};

class McdiMessageLoss : public RemoteAPIRadioEvent  {
public:
McdiMessageLoss(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

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
};

class McdiNewDatabaseGranted : public RemoteAPIRadioEvent  {
public:
McdiNewDatabaseGranted(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

class McdiNewDatabaseNotGranted : public RemoteAPIRadioEvent  {
public:
McdiNewDatabaseNotGranted(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

class McdiNewDatabaseAvailable : public RemoteAPIRadioEvent  {
public:
McdiNewDatabaseAvailable(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

class McdiDatabaseUpdateNotification : public RemoteAPIRadioEvent  {
public:
McdiDatabaseUpdateNotification(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned int get_wStatus() {
	return params[1].V_UINT32();
}
};

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
};

class McaBeginAllMuteError : public RemoteAPIRadioEvent  {
public:
McaBeginAllMuteError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

class McaEndAllMuteError : public RemoteAPIRadioEvent  {
public:
McaEndAllMuteError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

class McaAllMuteStatus : public RemoteAPIRadioEvent  {
public:
McaAllMuteStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wStatus(){
return params[1].V_UINT16();
}
unsigned short get_wStatusType(){
return params[2].V_UINT16();
}
};

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
};

class McaEndAmbienceListeningCallError : public RemoteAPIRadioEvent  {
public:
McaEndAmbienceListeningCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

class McaEndAPBTransmitError : public RemoteAPIRadioEvent  {
public:
McaEndAPBTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wMselGroup(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

class McaDatabaseUpdateNotificationStatus : public RemoteAPIRadioEvent  {
public:
McaDatabaseUpdateNotificationStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
};

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
};

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
};

class McaBeginGeneralTransmitWarning : public RemoteAPIRadioEvent  {
public:
McaBeginGeneralTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned char get_byClientId(){
return params[1].V_UINT8();
}
unsigned short get_wWarningNum(){
return params[2].V_UINT16();
}
};

class McaBeginGeneralTransmitError : public RemoteAPIRadioEvent  {
public:
McaBeginGeneralTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned char get_byClientId(){
return params[1].V_UINT8();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaEndGeneralTransmitError : public RemoteAPIRadioEvent  {
public:
McaEndGeneralTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned char get_byClientId(){
return params[1].V_UINT8();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaGeneralTransmitStatus : public RemoteAPIRadioEvent  {
public:
McaGeneralTransmitStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wStatus(){
return params[1].V_UINT16();
}
unsigned short get_wStatusType(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

class McaEndInstantTransmitError : public RemoteAPIRadioEvent  {
public:
McaEndInstantTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

class McaBeginMonitorError : public RemoteAPIRadioEvent  {
public:
McaBeginMonitorError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned char get_byClientId(){
return params[1].V_UINT8();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaEndMonitorError : public RemoteAPIRadioEvent  {
public:
McaEndMonitorError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned char get_byClientId(){
return params[1].V_UINT8();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaMonitorStatus : public RemoteAPIRadioEvent  {
public:
McaMonitorStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wStatus(){
return params[1].V_UINT16();
}
unsigned short get_wStatusType(){
return params[2].V_UINT16();
}
};

class McaOpenMselError : public RemoteAPIRadioEvent  {
public:
McaOpenMselError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
unsigned short get_wMselGroup(){
return params[2].V_UINT16();
}
};

class McaCloseMselError : public RemoteAPIRadioEvent  {
public:
McaCloseMselError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
unsigned short get_wMselGroup(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

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
};

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
};

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
};

class McaGetFreeMselGroupError : public RemoteAPIRadioEvent  {
public:
McaGetFreeMselGroupError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
};

class McaFreeMselGroupStatus : public RemoteAPIRadioEvent  {
public:
McaFreeMselGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wMselGroup(){
return params[1].V_UINT16();
}
};

class McaBeginMuteSecondReceiverError : public RemoteAPIRadioEvent  {
public:
McaBeginMuteSecondReceiverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaEndMuteSecondReceiverError : public RemoteAPIRadioEvent  {
public:
McaEndMuteSecondReceiverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

class McaDeletePatchMemberError : public RemoteAPIRadioEvent  {
public:
McaDeletePatchMemberError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
unsigned short get_channel(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

class McaGetFreePatchGroupError : public RemoteAPIRadioEvent  {
public:
McaGetFreePatchGroupError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
};

class McaFreePatchGroupStatus : public RemoteAPIRadioEvent  {
public:
McaFreePatchGroupStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wPatchGroup(){
return params[1].V_UINT16();
}
};

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
};

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
};

class McaEndPatchTransmitError : public RemoteAPIRadioEvent  {
public:
McaEndPatchTransmitError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wPatchGroup(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

class McaPeripheralsStatus : public RemoteAPIRadioEvent  {
public:
McaPeripheralsStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
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
};

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
};

class McaEndPrivateCallError : public RemoteAPIRadioEvent  {
public:
McaEndPrivateCallError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

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
};

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
};

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
};

class McaRefreshStatus : public RemoteAPIRadioEvent  {
public:
McaRefreshStatus(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wStatus(){
return params[1].V_UINT16();
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
};

class McaDisableRepeatError : public RemoteAPIRadioEvent  {
public:
McaDisableRepeatError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

class McaResourceTransmitWarning : public RemoteAPIRadioEvent  {
public:
McaResourceTransmitWarning(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wWarningNum(){
return params[2].V_UINT16();
}
};

class McaSelectResourceError : public RemoteAPIRadioEvent  {
public:
McaSelectResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaDeselectResourceError : public RemoteAPIRadioEvent  {
public:
McaDeselectResourceError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

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
};

class McaBeginTakeoverError : public RemoteAPIRadioEvent  {
public:
McaBeginTakeoverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

class McaEndTakeoverError : public RemoteAPIRadioEvent  {
public:
McaEndTakeoverError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_channel(){
return params[1].V_UINT16();
}
unsigned short get_wErrorNum(){
return params[2].V_UINT16();
}
};

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
};

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
};

class McaEndToneError : public RemoteAPIRadioEvent  {
public:
McaEndToneError(const ::MCC7500SDK::Parameters &param):RemoteAPIRadioEvent(param) {}
unsigned short get_wErrorNum(){
return params[1].V_UINT16();
}
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

}