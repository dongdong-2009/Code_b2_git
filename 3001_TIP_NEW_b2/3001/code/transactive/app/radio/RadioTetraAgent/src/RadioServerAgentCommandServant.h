/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioServerAgentCommandServant.h $
 * @author:  Anggiono
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RADIO_SERVER_AGENT_COMMAND_SERVANT_H_INCLUDED
#define RADIO_SERVER_AGENT_COMMAND_SERVANT_H_INCLUDED

#pragma once
#include <string>

//#include "app/radio/RadioTetraAgent/src/RadioServerAgentCommandImpl.h"
#include "app/radio/RadioTetraAgent/src/RadioServerMonitor.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSSummary.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSHandler.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/BaseTcpServer.h"
#include "app/radio/RadioTetraAgent/RadioTcpCommunication/src/ITcpConnectionStatusObserver.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodCallForwardRequest.h"
#include "app/radio/RadioTetraAgent/src/RadioProfileHandler.h"
#include "app/radio/RadioTetraAgent/src/RadioStatusHandler.h"
#include "app/radio/RadioTetraAgent/src/RadioMFTHandler.h"
#include "app/radio/RadioTetraAgent/src/RadioResourceSync.h"

#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSManager.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/CommonTypes.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IRadioPDSStatusCallback.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSLinkStatusThread.h"
namespace TA_Base_Bus
{
	class GenericAgent;
}

namespace TA_IRS_App
{

class RadioSDS;
class RadioServerAgentFramework;
 
class RadioServerAgentCommandServant: public virtual TA_Base_Bus::IEntity,
									  public virtual POA_TA_IRS_Bus::RadioServerAgentIDL,
                                      public virtual TA_Base_Core::ServantBase,
									  public virtual TA_IRS_App::IRadioPDSStatusCallback,
									  public virtual TA_IRS_App::ITcpConnectionStatusObserver,
									  public ISDSRegistationState
		                              
{
public:
	RadioServerAgentCommandServant(TA_Base_Core::IEntityDataPtr EntityData, RadioServerAgentFramework* parentFramework);
	virtual ~RadioServerAgentCommandServant();

    //void setSummary(RadioServerAgentSummary& summary);
    void setRadioSDS(RadioSDSSummary* pSdsSummary);

    // IEntity Interface Functions
    virtual bool isValid(void);
    virtual void start(void);
    virtual void stop(void);
    virtual void update(const class TA_Base_Core::ConfigUpdateDetails &details);
    virtual void remove(void);
    virtual void setToControlMode(void);
    virtual void setToMonitorMode(void);
	virtual void setCommandSender(const std::string &primaryAddress, const std::string &secondaryAddress,unsigned long timeout,TA_Base_Bus::DataPoint* radioDp);
	void switchSDSConnection(bool toPrimary);
    /* Used to send a Train Command (via SDS) to a target train's radio PEI interface */
    virtual void sendSDS(CORBA::ULong recipientSSI, const TA_IRS_Bus::RadioServerAgentIDL::SDSData& rawMessage);

	/* Used to handle API corba call with MCC */
	virtual void setMccHandler(std::string username, std::string password, std::string ior);

    /* Used to send a text message (via SDS) to a target radio mobile unit */
    virtual void sendText(const char* session, const TA_IRS_Bus::Radio::RadioResource& resource, TA_IRS_Bus::Radio::ERadioServerAgentEncodeType encoding, const char* rawMessage);

    /** 
     * Used only by the MFT Agent to notify the Server Agent that the operator's
     * console has received a radio status message 
     */
    virtual void radioStatusMsg(CORBA::ULong recipientSSI, CORBA::ULong statusMsg);

    virtual bool connectDynamicRegroup(const TA_IRS_Bus::Radio::RadioResource& resource,const char* talkgroupAlias);

	virtual bool releaseDynamicRegroup(const TA_IRS_Bus::Radio::RadioResource& resource,const char* talkgroupAlias);

	virtual void requestRadioCheck(const TA_IRS_Bus::Radio::RadioResource& resource, CORBA::String_out baseStation, CORBA::String_out talkgroupAlias);

	virtual void updateOperatorProfile(const char* opName, TA_IRS_Bus::Radio::OpTalkgroups_out talkgroups);

	virtual bool callForwarding(CORBA::Boolean condition, CORBA::ULong  forwardingISSI, CORBA::ULong  forwardedISSI);

	virtual bool cancelCallForwarding(CORBA::ULong ISSI);

	virtual void queryCallForwarding(CORBA::ULong ISSI, CORBA::UShort& condition, CORBA::ULong& forwardingISSI, CORBA::ULong& forwardedISSI);
	
	//Interface for the PDS Data
	void sendData(const char* ITSI, ::CORBA::ULong trainID, const ::TA_IRS_Bus::RadioServerAgentIDL::PDSDataType& pdsData, ::CORBA::Boolean useXON, const char* sessionID);
	/**
	 * Cancel data will allow the transfer to a specified ITSI to be stopped.
	 */
	void cancelSendData(::CORBA::ULong trainID, const char* sessionID);

	/**
	 * Notification status from Train Agent of EndOfTtisDonwload
	 */
	void notifyEndofTtisDownloadStatus(::CORBA::ULong trainID, ::CORBA::UShort notifyStatus);

	void setCommandPDSServer( const std::string& udpServer, const std::string& udpServicePort, const CommonTypes::SRadioPDSTimer& radioPDSTimer );
	// implementation method for IRadioPDSStatusCallback
	virtual  void notifyPDSDownloadStatus (const TA_IRS_App::TtisTypes::SPDSDownloadDataResults & pdsDownloadResult);

	void notifyMftAgent(short status);

    virtual bool getIscsStatus();

    virtual bool getRadioStatus();

	// ITcpConnectionStatusObserver methods
	virtual void onConnectionLost();
	virtual void onConnectionEstablish(bool isPrimary);
	virtual void onSwitchConnection(bool toPrimary);

	//Interface for ISDSRegistationState
	void setSDSRegistrationState(Radio::ERegistrationState regState);

	//Set callback interface for updating SDS Link Status Datapoint
	void setSDSLinkStatusDp(IRadioSDSLinkStatus* sdsLinkStatusDp);


private:
	
	void createRadioSDS();
	void checkOperationMode();
	void sendCmdText(RadioMethod& method);
private:
    //RadioServerAgentCommandImpl m_agentCmd;
    std::string m_objectName;
	unsigned long m_EntityKey;
	unsigned int m_locationKey;
	unsigned long m_entityLocation;

    RadioServerMonitor m_serverMonitor;

    RadioSDSSummary* m_pRadioSDSSummary;

	RadioSDSHandler m_sdsHandler;

	RadioServerAgentFramework* m_parentFramework;

	BaseTcpServer* m_cmdSender;

	RadioProfileHandler* m_mftProfileHandler;

	RadioStatusHandler* m_radioStatusHandler;

	RadioPDSManager* m_pdsManager;

	RadioMFTHandler* m_radioMftHandler;

	RadioResourceSync* m_mftBroadcast;


	unsigned long m_lTrainID;
	std::vector < unsigned char > m_data;
	unsigned short m_sEndofTtisDownloadStatus;
	unsigned long m_lEndofTtisDownloadTrainID;

	CommonTypes::TrainTSIMap m_trainTSIMap;
	static unsigned int OCC_LOCATION_KEY;

	/* 
	 * ISCS Server Status
	 * true - degraded
	 * false - normal
	 */
	bool m_iscsStatus;
	bool m_radioGatewayConnectionStatus;

	//Interface for updating SDS Link Status Datapoint
	IRadioSDSLinkStatus* m_sdsLinkStatusDP;

	bool m_inControlMode;
};
}
#endif

