/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/RadioAccess.cpp $
 * @author:  Adam Radics
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 * 
 * Handles all radio related functions like sending/receiving SDS and
 * making/ending calls.
 * 
 * Refer to the radio system design for how to actually implement this class.
 */


#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/trains/train_agent/src/RadioAccess.h"
#include <algorithm>
#include "ace/OS.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"

#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"

#include "app/trains/train_agent/src/TrainAgentConstants.h"


using namespace TA_Base_Core;
using namespace TA_IRS_Bus;

RadioAccess::~RadioAccess()
{
    FUNCTION_ENTRY("~RadioAccess");
    FUNCTION_EXIT;
}

RadioAccess::RadioAccess(std::map< int, unsigned long> paLiveResources , unsigned long radioInterval)
    : m_paLiveResources(paLiveResources),
      m_radioInterval(radioInterval)
{
    FUNCTION_ENTRY("RadioAccess");

    // Receive SDS messages from OCC Radio Agent (Query Notes Ripple_TrainAgent_QN_001-(3)
    unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());

    try
    {
        CorbaNameList radioAgentCorbaNameList = 
            EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( TrainAgentConstants::RADIO_SERVER_COMMAND_TYPE, locationKey,true);

        TA_ASSERT(radioAgentCorbaNameList.size() == 1,"No or more than one Radio entity found in OCC");

        m_radioServerAgent.setCorbaName(radioAgentCorbaNameList[0]);


        std::vector<IEntityData*> radioAgentEntities = 
            EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TrainAgentConstants::RADIO_SERVER_COMMAND_TYPE, locationKey,true);

        TA_ASSERT(radioAgentEntities.size() == 1,"No or more than one Radio entity found in OCC");

	    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            RadioComms::ReceiveSDS, 
            this, 
	        radioAgentEntities[0]->getKey(), 
	        radioAgentEntities[0]->getSubsystem(), 
	        locationKey);


        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " Key[%d] Subsystem[%d] Location[%d] Name[%s]", 
            radioAgentEntities[0]->getKey(), radioAgentEntities[0]->getSubsystem(), locationKey, radioAgentEntities[0]->getName().c_str());

        delete radioAgentEntities[0];
    }
    catch (const TA_Base_Core::DatabaseException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
    }
    catch (const TA_Base_Core::DataException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
    }
	catch (...)
	{
        LOG_EXCEPTION_CATCH(SourceInfo, "...", 
            "Exception caught while attempting to resolve the RadioAgent.");
    }   

    FUNCTION_EXIT;
}

// ss++
// TD 14871
//virtual void sendData( const std::string& radioId, const TrainMessageData& data )
void RadioAccess::sendData(const std::string& radioId, const std::vector<unsigned char>& data)
{
    FUNCTION_ENTRY("sendData");

    unsigned long ssi = atol(radioId.c_str());

    RadioServerAgentIDL::SDSData rawData;
    rawData.length(data.size());

    // For debugging purposes 
    std::ostringstream dump;
    dump << std::hex;

    for (unsigned int i = 0; i < data.size(); i++)
    {
        // for debugging purposes
        dump << static_cast<unsigned int>(data[i]) << " ";

        rawData[i] = data[i];
    }

    try 
    {
		//try{
		//	m_radioServerAgent->sendSDS(ssi,rawData);
			CORBA_CALL ( m_radioServerAgent , sendSDS ,( ssi,rawData)  );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[SDS sent radio ssi %d][Hex] %s", ssi,dump.str().c_str());
	//	}//TD18706,jianghp, handle the operation mode
	//	catch (TA_Base_Core::OperationModeException &) 
	//	{
		//	m_radioServerAgent.clear();
		//	m_radioServerAgent->sendSDS(ssi,rawData);
	//	}
	}
    catch(const TA_Base_Core::BadParameterException& e)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.reason.in()));
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException("Unknown exception received from Radio Agent"));
    }

    FUNCTION_EXIT;
}
// ++ss

void RadioAccess::receiveSDSData(const std::vector<unsigned char>& data, const std::string& senderRadioId,const std::string& destRadioID)
{
	FUNCTION_ENTRY("ReceiveSDSData");
    TA_ASSERT(m_sdsReceiver != NULL, "SDS receiver has not been set");

		m_sdsReceiver->receiveData(data, senderRadioId,destRadioID);

	FUNCTION_EXIT;
}
    
void RadioAccess::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    /*FUNCTION_ENTRY("receiveSpecialisedMessage");
    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "receiveSpecialisedMessage called");

    std::string messageTypeKey(message.messageTypeKey);
    if (messageTypeKey != RadioComms::ReceiveSDS.getTypeKey() ) 
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Did not receive RadioComms Message");
        FUNCTION_EXIT;
        return;
    }

    RadioServerAgentIDL::recvSDS *pSds;
    if ( ( (message.messageState) >>= pSds ) != 0)
    {
        std::ostringstream radioId;
        radioId << pSds->originatingTrain.ssi;

        // ss++
        // TD 14871 - for debugging purposes
        std::ostringstream dump;
        dump << std::hex;
        // ++ss

        std::vector<unsigned char> rawData;
        for(unsigned int i = 0; i < pSds->message.length(); i++)
        {
            // ss++
            // TD 14871 - for debugging purposes
            dump << static_cast<int>(pSds->message[i]) << " ";
            // ++ss

            rawData.push_back(pSds->message[i]);
        }

        // ss++
        // TD 14871
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
					"[SDS recv from radio %s][Hex] %s", 
					radioId.str().c_str(), 
					dump.str().c_str());
        // ++ss

        TA_ASSERT(m_sdsReceiver != NULL, "SDS receiver has not been set");
        m_sdsReceiver->receiveData(rawData, radioId.str());
    }

    FUNCTION_EXIT;*/
}

void RadioAccess::registerForSDS(TA_IRS_Bus::IResponseReceiver& receiver)
{
    FUNCTION_ENTRY("registerForSDS");
    m_sdsReceiver = &receiver;
    FUNCTION_EXIT;
}

void RadioAccess::makePaLiveAnnouncementCall(const std::string& operatorSessionId,
                                             //const unsigned long resourceId, 
											 const std::string& talkgroupName,
                                             const ERadioBroadcastType broadcastType)
{
    FUNCTION_ENTRY("makePaLiveAnnouncementCall");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "makePaLiveAnnouncementCall triggered on session[%s] and resource Alias [%s]",
        operatorSessionId.c_str(),
		talkgroupName.c_str());


    RadioMFTAgentNamedObject radioMftAgent;

    unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    try
    {
        // ss++
        // TD 14871
        //radioMftAgent.setCorbaName(TrainAgentConstants::RADIO_MFT_MANAGER, getRadioMFTAgentEntityName(operatorSessionId));
        std::string radioMftAgentName = getRadioMFTAgentEntityName(operatorSessionId);
        radioMftAgent.setCorbaName(radioMftAgentName, radioMftAgentName);
        // ++ss
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        std::ostringstream reason;
        reason << "Error resolving radio MFT agent at location " << locationKey << " in makePaLiveAnnouncementCall";
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(reason.str().c_str()));
    }

    Radio::RadioResource resource;
	//resource.id = resourceId;
	resource.alias = talkgroupName.c_str();
    if(broadcastType == SINGLE_TRAIN)
    {
        resource.type = Radio::RR_TALKGROUP;        
    }
    else
    {
        resource.type = Radio::RR_ANNGROUP;
    }

    try
    {
        // ss++
        // TD 14871
		// wong.peter, use assignandselectresource instead of assign and select separately since we dont proceed the channel returned by assignment status.
		//try{
			CORBA_CALL(radioMftAgent,assignAndSelectResource,(operatorSessionId.c_str(), resource, 0));
			//radioMftAgent->assignAndSelectResource(operatorSessionId.c_str(), resource, 0);
		//TD18706,jianghp, handle the operation mode
		//catch (TA_Base_Core::OperationModeException &) 
		//{
		//	radioMftAgent.clear();
		//	radioMftAgent->assignAndSelectResource(operatorSessionId.c_str(), resource, 0);
		//}
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "[makePaLiveAnnouncementCall] assignAndSelectResource to  operator session %s with resource ID = %d",
            operatorSessionId.c_str(), resource.id);

		//// Radio agent requires some time to assign the resources. 
		//// Thus wait for some time for radio before selecting the resources
		//ACE_OS::sleep (m_radioInterval);
		//try{
		//	radioMftAgent->selectResource(operatorSessionId.c_str(),resource);
		//}//TD18706,jianghp, handle the operation mode
		//catch (TA_Base_Core::OperationModeException &) 
		//{
		//	radioMftAgent.clear();
		//	radioMftAgent->selectResource(operatorSessionId.c_str(),resource);
		//}
		//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
		//	"[makePaLiveAnnouncementCall] selectResource for operator session %s with resource ID = %d", 
		//	operatorSessionId.c_str(), resource.id);
        // ++ss
    }
    catch(TA_Base_Core::BadParameterException& e)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.reason.in()));
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException("Unknown exception received from Radio Agent"));
    }
    FUNCTION_EXIT;
}

void RadioAccess::endPaLiveAnnouncementCall( const std::string& operatorSessionId,
                                             //const unsigned long resourceId, 
											 const std::string& talkgroupName,
                                             const ERadioBroadcastType broadcastType )
                                            
{
    FUNCTION_ENTRY("endPaLiveAnnouncementCall");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "endPaLiveAnnouncementCall triggered on session[%s] and resource ID [%s]",
        operatorSessionId.c_str(),
		talkgroupName.c_str());
    

    RadioMFTAgentNamedObject radioMftAgent;
    unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    try
    {
        // ss++
        // TD 14871
        //radioMftAgent.setCorbaName(TrainAgentConstants::RADIO_MFT_MANAGER, getRadioMFTAgentEntityName(operatorSessionId));
        std::string radioMftAgentName = getRadioMFTAgentEntityName(operatorSessionId);
        radioMftAgent.setCorbaName(radioMftAgentName, radioMftAgentName);
        // ++ss
    }
    catch(...)
    {
        std::ostringstream reason;
        reason << "Error resolving radio MFT agent at location " << locationKey << " in endPaLiveAnnouncementCall";
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(reason.str().c_str()));
    }


    Radio::RadioResource resource;
	//resource.id = resourceId;
	resource.alias = talkgroupName.c_str();
    if(broadcastType == SINGLE_TRAIN)
    {
        resource.type = Radio::RR_TALKGROUP;
    }
    else
    {
        resource.type = Radio::RR_ANNGROUP;
    }

    try
    {
        // ss++
        // TD 14871
		// wong.peter, like assign, here deassignAndDeselect is also used instead.
	//	try{
		//	radioMftAgent->deassignAndDeselectResource(operatorSessionId.c_str(),resource);
			CORBA_CALL ( radioMftAgent , deassignAndDeselectResource ,(operatorSessionId.c_str(),resource) ) ;
	//	}//TD18706,jianghp, handle the operation mode
	//	catch (TA_Base_Core::OperationModeException &) 
	//	{
	//		radioMftAgent.clear();
	//		radioMftAgent->deassignAndDeselectResource(operatorSessionId.c_str(),resource);
	//	}
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "[endPaLiveAnnouncementCall] deassignAndDeselectResource for operator session %s with resource ID = %d",
            operatorSessionId.c_str(), resource.id);
        
        // Radio agent requires some time to de-assign the resources. 
        // Thus wait for some time for radio before de-selecting the resources
  //      ACE_OS::sleep (m_radioInterval);

		//try{
		//	radioMftAgent->deselectResource(operatorSessionId.c_str(),resource);
		//}//TD18706,jianghp, handle the operation mode
		//catch (TA_Base_Core::OperationModeException &) 
		//{
		//	radioMftAgent.clear();
		//	radioMftAgent->deselectResource(operatorSessionId.c_str(),resource);
		//}
  //      LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
  //          "[endPaLiveAnnouncementCall] deselectResource for operator session %s with resource ID = %d",
  //          operatorSessionId.c_str(),  resource.id);
        // ++ss
    }
    catch(TA_Base_Core::BadParameterException& e)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.reason.in()));
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException("Exception received from Radio Agent"));
    }

    FUNCTION_EXIT;
}


void RadioAccess::makePrivateCall(const std::string& operatorSessionId,
                                             const unsigned long resourceId)
{
    FUNCTION_ENTRY("makePecCall");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "makePecCall triggered on session[%s] and resource ID [%d]",
        operatorSessionId.c_str(),
        resourceId);


    RadioMFTAgentNamedObject radioMftAgent;

    unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    try
    {
        // ss++
        // TD 14871
        //radioMftAgent.setCorbaName(TrainAgentConstants::RADIO_MFT_MANAGER, getRadioMFTAgentEntityName(operatorSessionId));
        std::string radioMftAgentName = getRadioMFTAgentEntityName(operatorSessionId);
        radioMftAgent.setCorbaName(radioMftAgentName, radioMftAgentName);
        // ++ss
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        std::ostringstream reason;
        reason << "Error resolving radio MFT agent at location " << locationKey << " in makePaLiveAnnouncementCall";
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(reason.str().c_str()));
    }

    Radio::RadioResource resource;
	// peter.wong, DTL-484, radio ssi is used by mft agent
	resource.ssi = resourceId;
	// resource type of pec
    resource.type = Radio::RR_TRAIN; 

    try
    {
 
		CORBA_CALL(radioMftAgent, makePrivateCall, (operatorSessionId.c_str(), resource, 0));
		
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "[makePecCall] makePrivateCall to  operator session %s with radio ID = %d",
            operatorSessionId.c_str(), resource.ssi);
		//}
  //      LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
  //          "[makePecCall] selectResource for operator session %s with resource ID = %d", 
  //          operatorSessionId.c_str(), resource.id);
        // ++ss
    }
    catch(TA_Base_Core::BadParameterException& e)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.reason.in()));
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException("Unknown exception received from Radio Agent"));
    }
    FUNCTION_EXIT;
}

void RadioAccess::endPecCall( const std::string& operatorSessionId,
                                             const unsigned long resourceId)
                                            
{
    FUNCTION_ENTRY("endPecCall");

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
        "endPecCall triggered on session[%s] and resource ID [%d]",
        operatorSessionId.c_str(),
        resourceId);
    

    RadioMFTAgentNamedObject radioMftAgent;
    unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    try
    {
        // ss++
        // TD 14871
        //radioMftAgent.setCorbaName(TrainAgentConstants::RADIO_MFT_MANAGER, getRadioMFTAgentEntityName(operatorSessionId));
        std::string radioMftAgentName = getRadioMFTAgentEntityName(operatorSessionId);
        radioMftAgent.setCorbaName(radioMftAgentName, radioMftAgentName);
        // ++ss
    }
    catch(...)
    {
        std::ostringstream reason;
        reason << "Error resolving radio MFT agent at location " << locationKey << " in endPaLiveAnnouncementCall";
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(reason.str().c_str()));
    }


    Radio::RadioResource resource;
	// peter.wong, DTL-484, radio ssi is used by mft agent
	resource.ssi = resourceId;
	resource.type = Radio::RR_TRAIN;

    try
    {
        // ss++
        // TD 14871
		//try{
		//	radioMftAgent->deassignAndDeselectResource(operatorSessionId.c_str(),resource);

			//CORBA_CALL ( radioMftAgent ,deassignResource , (operatorSessionId.c_str(),resource)   ) ;
	//	}//TD18706,jianghp, handle the operation mode
	//	catch (TA_Base_Core::OperationModeException &) 
	//	{
	//		radioMftAgent.clear();
	//		radioMftAgent->deassignAndDeselectResource(operatorSessionId.c_str(),resource);
	//	}
        //LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
        //    "[endPecCall] deassignResource for operator session %s with resource ID = %d",
        //    operatorSessionId.c_str(), resource.id);
        
        // Radio agent requires some time to de-assign the resources. 
        // Thus wait for some time for radio before de-selecting the resources
        //ACE_OS::sleep (m_radioInterval);

		//try{
		//	radioMftAgent->deselectResource(operatorSessionId.c_str(),resource);
		//}
		CORBA_CALL(radioMftAgent, endCall, (operatorSessionId.c_str(), resource, 0));
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
            "[endPecCall] EndCall for operator session %s with radio ID = %d",
            operatorSessionId.c_str(),  resource.ssi);
  //      // ++ss
    }
    catch(TA_Base_Core::BadParameterException& e)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.reason.in()));
    }
    catch(const CORBA::Exception& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
    }
    catch(const TA_Base_Core::TransactiveException& e)
    {
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException(e.what()));
    }
    catch(...)
    {
        FUNCTION_EXIT;
        TA_THROW(TrainTransmissionFailureException("Exception received from Radio Agent"));
    }

    FUNCTION_EXIT;
}


std::string RadioAccess::getRadioMFTAgentEntityName(const std::string & sessionID)
{
 	FUNCTION_ENTRY("getRadioMFTAgentEntityName");

    TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
    std::stringstream name;
    name << console->getName();
    name << TrainAgentConstants::RADIO_MFT_AGENT_TYPE;
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "RadioMFTAgent Name of session %s is %s", 
        sessionID.c_str(), name.str().c_str());
    delete console;

	FUNCTION_EXIT;
    return name.str();
}

unsigned long RadioAccess::getAllTrainsLivePaResourceId(int line)
{
    FUNCTION_ENTRY("getAllTrainsLivePaResourceId");
    FUNCTION_EXIT;

	unsigned long ret = 0;

	std::map< int, unsigned long>::iterator it = m_paLiveResources.find(line);

	if( it != m_paLiveResources.end() ) 
	{
		ret = it->second;
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Invalid ELiveAnnouncementResource");
	}

    return ret;
}

void RadioAccess::answerPrivateCall(const std::string& operatorSessionId,
								    const unsigned long resourceId)
{
	FUNCTION_ENTRY("answerPrivateCall");

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"answerPrivateCall triggered on session[%s] and resource ID [%d]",
				operatorSessionId.c_str(),
				resourceId);


	RadioMFTAgentNamedObject radioMftAgent;

	unsigned long locationKey = atol(RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
	try
	{
		std::string radioMftAgentName = getRadioMFTAgentEntityName(operatorSessionId);
		radioMftAgent.setCorbaName(radioMftAgentName, radioMftAgentName);
	}
	catch(const CORBA::Exception& e)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(e.what()));
	}
	catch(...)
	{
		std::ostringstream reason;
		reason << "Error resolving radio MFT agent at location " << locationKey << " in makePaLiveAnnouncementCall";
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(reason.str().c_str()));
	}

	Radio::RadioResource resource;
	resource.ssi = resourceId;
	resource.type = Radio::RR_TRAIN; 

	try
	{

		CORBA_CALL(radioMftAgent, answerPrivateCall, (operatorSessionId.c_str(), resource));

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"[answer Driver Call] answerPrivateCall to  operator session %s with radio ID = %d",
			 operatorSessionId.c_str(), resource.ssi);
		
	}
	catch(TA_Base_Core::BadParameterException& e)
	{
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(e.reason.in()));
	}
	catch(const CORBA::Exception& e)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e));
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e)));
	}
	catch(const TA_Base_Core::TransactiveException& e)
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException",  e.what() );
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException(e.what()));
	}
	catch(...)
	{
		FUNCTION_EXIT;
		TA_THROW(TrainTransmissionFailureException("Unknown exception received from Radio Agent"));
	}
	FUNCTION_EXIT;
}
