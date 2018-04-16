/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/RadioTetraAgent/src/RadioSDS.h $
 * @author:  albertus
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef __RADIOSDS230424039923__
#define __RADIOSDS230424039923__

#include <string>
#include <vector>
#include "ace/Time_Value.h"

#include "core/naming/src/NamedObject.h"

#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSOutputQueue.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSQueueManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSSendText.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSTransactionManager.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDSEventObserver.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.h"
#include "app/radio/RadioTetraAgent/src/RadioSDSSummary.h"

#include "core/message/src/CommsMessageSender.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "bus/trains/train_agent/idl/src/IRecvSDSTrainAgentCorbaDef.h"
//#include "E:\TIPBuild\4677_Build\4677\build\win32_n\transactive\bus\trains\train_agent\IDL\src\IRecvSDSTrainAgentCorbaDef.h"
namespace TA_IRS_App
{

class RadioSDSHandler;
namespace Radio
{
	enum ERegistrationState
    {
        NOT_REGISTERED,
        REGISTRATION_STARTED,
#ifdef REGISTERED
#define TEMP__REGISTER REGISTERED
#undef REGISTERED
		REGISTERED
#define REGISTERED TEMP__REGISTER
#else
		REGISTERED
#endif

	};
}

//Callback interface that would notify that 
//the SDS Registration is successful
class ISDSRegistationState
{
public:
	/**
	 * setSDSRegistrationState
	 *
	 * Callback interface, used to set the SDS Registration state
	 * 
	 * @param : ERegistrationState regState - Registration State
	 */
	virtual void setSDSRegistrationState(Radio::ERegistrationState regState) = 0;
};


typedef TA_Base_Core::NamedObject<TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef,
	                              TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef_ptr,
	                              TA_IRS_Bus_4677::IRecvSDSTrainAgentCorbaDef_var> 
                                  RecvSDSNamedObject;

class RadioSDS : virtual public TA_IRS_App::SDSEventObserver
{
public:
    RadioSDS( RadioSDSSummary* pSDSSummary,
			  TA_IRS_App::SDSOutputQueue* pOutputq,
			  TA_IRS_App::SDSQueueManager* pQMgr,
			  TA_IRS_App::SDTSTransactionManager* ptransactMgr,
			  ISDSRegistationState& sdsRegState,
			  bool bIsActiveSDS = false);
    virtual ~RadioSDS();


    

    /**
     * Return true to indicate registration was done
     * i.e., state changed from not registered to registered
     */
    bool initializeSDS();

    void cleanupSDS();

	bool isReady() { return m_registrationState == Radio::REGISTERED;};

    /**
     * Used to send a Train Command (via SDS) to a target train's radio PEI interface.
     * 
     * @return indicates successful execution of the request
     */
    void sendSDS(unsigned long recipientSSI, std::vector<unsigned char> rawMessage);

    /**
     * Used to send a text message (via SDS) to a target radio mobile unit.
     * 
     * @return indicates successful execution of the request
     */
    void sendText(unsigned long recipientSSI, TextCodingScheme encoding, std::string rawMessage, bool isBroadcast=false);

    /**
	  * This function is inherited from SDSEventObserver::notify();
      * This method should be called when SDSEvent happened;
	  * It is called from within SDSInputQueue::notifyObservers();
      */
	virtual void notify( TA_IRS_App::SDSEvent* sdsEvt);
	virtual void setUnregistrationState();

	
	void setClientSSI(unsigned long clientSSI);
	unsigned long getClientSSI();
	
	void setConsoleName(const std::string& name);
	std::string getConsoleName();

	void setSDSHost(const std::string& host);
	void setSDSPort(const std::string& port);
	std::string RadioSDS::getSDSHost();
	std::string RadioSDS::getSDSPort();
	void setSDSPassNumber(unsigned long passnumber);


	bool isTrainborne();
	void setTrainborne(bool isTrain);
	
	void testSDS();

	// set the SDS active connection state
	void setActiveSDSConnection (const bool & bStateValue);
	bool getActiveSDSConnection ();

	bool needReconnection();
	void resetRetryCount();
	
private:


    /**
      * This method handles registration to SDS
      */
	void connectToSDS();

    /**
      * TD16359 - return a bool t indicate whether a delivery receipt is required
      */
	bool processTLDataEvent(TA_IRS_App::TL_DataEvent* dataEvent); 

	void processCMResponse(TA_IRS_App::CM_RegisterResponse* pResponse);
    void submitAlarm(const TA_Base_Core::MessageType& pAlarm);
    void closeAlarm(const TA_Base_Core::MessageType& pAlarm);

	/**
	  * initialiseSDSNamedObject
	  * 
	  * Resolves the train agent object and initializes a pointer to it.
	  * 
	  * @return void 
	  */
    void initialiseSDSNamedObject();

private:
    	// To keep all input/output events & requests;
    TA_IRS_App::SDSOutputQueue *m_pSDSOutputQueue;

    //	To manage all output/input events & requests;
	TA_IRS_App::SDSQueueManager *m_pSDSQueueManager;

    //	To manage all transactions of events & requests;
	TA_IRS_App::SDTSTransactionManager *m_pSDTSTransactionManager;


	Radio::ERegistrationState m_registrationState;

    ACE_Time_Value m_timeout;
    ACE_Time_Value m_registrationTime;

    unsigned long m_entityKey;
    unsigned long m_entityLocation;
    unsigned long m_entitySubsystem;
    unsigned long m_entityTypeKey;
	unsigned long m_serverAgentKey;
    std::string m_entityName;

    std::string m_session;

    //	To send notifications of incoming messages to Manager 
    TA_Base_Core::CommsMessageSender* m_commsSender;
    
	/**
	*	Used for safe acess;
	*/
	TA_Base_Core::ReEntrantThreadLockable m_lock;

	std::string m_host;
	std::string m_port;
	std::string m_consoleName;
	unsigned long m_clientSSI;
	unsigned long m_passNumber;

	bool m_bTrainborne;
	
	/**
	 * This is the train agent object
	 */
	RecvSDSNamedObject m_trainAgent;
	bool m_bIsActiveSDS;
	unsigned int cntNumRetires;
	unsigned int m_MaxNumRetry;
	ISDSRegistationState& m_sdsRegistrationState;
};
}

#endif
