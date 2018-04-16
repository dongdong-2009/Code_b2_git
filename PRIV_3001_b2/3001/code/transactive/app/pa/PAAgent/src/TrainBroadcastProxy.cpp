/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/PRIV_3001/3001/transactive/app/pa/PAAgent/src/TrainBroadcastProxy.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification: $Date: 2018/03/14 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  */

#include <iostream>
#include <sstream>
#include <time.h>
#include <ace/OS.h>

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#pragma warning ( disable : 4250)
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#pragma warning ( default : 4250)

#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/GlobalConstants.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/TrainDVABroadcast.h"
#include "app/pa/PAAgent/src/TrainLiveBroadcast.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/TrainBroadcastProxy.h"

namespace TA_IRS_App
{

const std::string TrainBroadcastProxy::ATS_AGENT_SESSION_ID = "AtsAgentSessionId";
const std::string TrainBroadcastProxy::MANAGER_LAUNCHING_BROADCAST_ID = "HardwareConnectionTest";

TrainBroadcastProxy::TrainBroadcastProxy( BroadcastManager& refManager ) :
m_refManager( refManager )
{
    FUNCTION_ENTRY( "TrainBroadcastProxy" );

    std::string strStationPaName = CachedConfig::getInstance()->getAgentName();
    strStationPaName += TA_Base_Bus::IPAAgentCorbaDef::TRAIN_PA_CORBA_NAME;
    activateServantWithName( strStationPaName );

    FUNCTION_EXIT;
}


TrainBroadcastProxy::~TrainBroadcastProxy()
{
    FUNCTION_ENTRY( "~TrainBroadcastProxy" );

    try
    {
        deactivateServant();
    }
    catch ( ... )
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
    }

    FUNCTION_EXIT;
}


// IDL Interface functions
// exception checked
std::string TrainBroadcastProxy::getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType,
                                                const std::string& extraInformation /* = "" */)
    // TA_THROW CorruptInternalStateDataException
{
    FUNCTION_ENTRY( "getNewBroadcastId" );

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastTypeString;

    switch (broadcastType)
    {
    case TA_Base_Bus::IPAAgentCorbaDef::TrainLive :
        broadcastTypeString = "TrainLive at ";
        break;
    case TA_Base_Bus::IPAAgentCorbaDef::TrainDva :
		//broadcastTypeString = "TrainDva at ";
		broadcastTypeString = extraInformation;
		break;
    default :
        TA_THROW(TA_Base_Core::CorruptInternalStateDataException("Unexpected EBroadcastType"));
    }

    // Build time stamp
    time_t now = time( NULL );
    tm nowTm;
    ACE_OS::localtime_r(&now, &nowTm);

    char nowString[80] = {0};
    sprintf(nowString,
            " %02d/%02d/%04d %02d:%02d:%02d",
			(nowTm.tm_mday),
			(nowTm.tm_mon + 1),
			(nowTm.tm_year + 1900),
            (nowTm.tm_hour),
            (nowTm.tm_min),
            (nowTm.tm_sec));

    // Since we only have a resolution of 1 sec, we need to check if the request
    // was done within 1 second of the previous broadcast by comparing the new nowString
    // with the previous.  We just need to make sure that the broadcastIds are all
    // unique.

    unsigned long extraInfoForUniqueness = 0;
    std::string broadcastId = broadcastTypeString + nowString;

    while ( m_refManager.isBroadcastIdExisted( broadcastId ))
    {
        // Ensure broadcastIds are different by updating the suffix
        ++extraInfoForUniqueness;

        char suffix[10] = {0};
        sprintf(suffix, " (%d)", extraInfoForUniqueness);

        broadcastId = broadcastTypeString + nowString + suffix;
    }

    FUNCTION_EXIT;
    return broadcastId;
}


char* TrainBroadcastProxy::broadcastTrainDva
    ( const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
      CORBA::ULong messageKey,
      CORBA::ULong libraryVersion,
      const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams,
      CORBA::Boolean hasOverrideOption,
      const char* sessionId ,
      CORBA::Boolean isEmergency)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastTrainDva" );

    LOGOUT("broadcastTrainDva");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

	std::ostringstream extraInformation;

	try
	{
		extraInformation << TA_IRS_Bus::CachedMaps::getInstance()->getTrainDvaMessageRecordFromKey(messageKey).m_label;	
	}
	catch(TA_Base_Core::CachedMappingNotFoundException& e)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified message does not exist in the database" );
	}

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::TrainDva, extraInformation.str());

    PasHelpers::getInstance()->submitTrainPAPrerecordedAuditMessage(
        broadcastId,
        trains, 
        messageKey, 
        libraryVersion, 
        isEmergency,
        sessionId );

    // Check the broadcast limit after sending the audit messages for tracability
    if (m_refManager.getTrainBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
    {
        std::ostringstream msg;
        msg << "Request failed due to broadcast limit ("
            << CachedConfig::getInstance()->getMaxBroadcasts() << ")";

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             msg.str().c_str() );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            msg.str(),
            sessionId);

        PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::TrainDvaConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::TrainDva;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = false;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.hasOverrideOption          = hasOverrideOption;
    theSpecifics.libraryVersion             = libraryVersion;
    theSpecifics.messageKey                 = messageKey;
    theSpecifics.stCyclicParams         = stCyclicParams;
    theSpecifics.trains                     = trains;
	theSpecifics.isEmergency			= isEmergency;

    newBroadcastConfigP->theBroadcastConfigSpecifics.theTrainDvaConfig(theSpecifics);

    AbstractBroadcast* newBroadcast = NULL;

    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        newBroadcast = new TrainDVABroadcast(newBroadcastConfigP, sessionId);

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& bpe)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            std::string(bpe.strWhat.in()),
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            "",
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addTrainBroadcast(newBroadcast);

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
    updateData.broadcastType   = newBroadcastConfigP->broadcastType;
    updateData.sessionId       = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


char* TrainBroadcastProxy::broadcastTrainLive(const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains, CORBA::Boolean hasOverrideOption, const char* staticGroup, const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastTrainLive" );

    LOGOUT("broadcastTrainLive");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::TrainLive);

    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter,
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled.");
    }

    PasHelpers::getInstance()->submitTrainPASetupLiveAnnouncementAuditMessage(
        broadcastId,
        trains,
        sessionId );

    // Check the broadcast limit after sending the audit messages for tracability
    if (m_refManager.getTrainBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
    {
        std::ostringstream msg;
        msg << "Request failed due to broadcast limit ("
            << CachedConfig::getInstance()->getMaxBroadcasts() << ")";

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             msg.str().c_str() );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            msg.str(),
            sessionId);

        PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::TrainLiveConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::TrainLive;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = false;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.hasOverrideOption          = hasOverrideOption;
    theSpecifics.trains                     = trains;
    theSpecifics.staticGroupName            = staticGroup;

    newBroadcastConfigP->theBroadcastConfigSpecifics.theTrainLiveConfig(theSpecifics);


    AbstractBroadcast* newBroadcast = NULL;

    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        newBroadcast = new TrainLiveBroadcast(newBroadcastConfigP, sessionId);

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& bpe)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            std::string(bpe.strWhat.in()),
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            "",
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addTrainBroadcast(newBroadcast);

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
    updateData.broadcastType   = newBroadcastConfigP->broadcastType;
    updateData.sessionId       = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
    return CORBA::string_dup(newBroadcastConfigP->broadcastId);
}


void TrainBroadcastProxy::beginTrainLiveAnnouncement ( const char* broadcastId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "beginTrainLiveAnnouncement" );

    LOGOUT("beginTrainLiveAnnouncement");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled." );
    }

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to begin a train live announcement on a non-existent broadcast",
            broadcastId );
    }
    TrainLiveBroadcast* trainLiveBroadcast = dynamic_cast<TrainLiveBroadcast*>(broadcast);
    if (trainLiveBroadcast == NULL)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified broadcastId is not a train live broadcast" );
    }

    trainLiveBroadcast->beginTrainLiveAnnouncement();

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::continueTrainLiveAnnouncement(const char* broadcastId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "continueTrainLiveAnnouncement" );

    LOGOUT("continueTrainLiveAnnouncement");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to continue a train live announcement on a non-existent broadcast",
            broadcastId );
    }
    TrainLiveBroadcast* trainLiveBroadcast = dynamic_cast<TrainLiveBroadcast*>(broadcast);
    if (trainLiveBroadcast == NULL)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified broadcastId is not a train live broadcast" );
    }

//	    PasHelpers::getInstance()->submitTrainPAContinueLiveAnnouncementAuditMessage(
//	        broadcastId,
//	        trainId );

    trainLiveBroadcast->continueTrainLiveAnnouncement();

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::endTrainLiveAnnouncement(const char* broadcastId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "endTrainLiveAnnouncement" );

    LOGOUT("endTrainLiveAnnouncement");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to end a train live announcement on a non-existent broadcast",
            broadcastId );
    }
    TrainLiveBroadcast* trainLiveBroadcast = dynamic_cast<TrainLiveBroadcast*>(broadcast);
    if (trainLiveBroadcast == NULL)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified broadcastId is not a train live broadcast" );
    }

    // Extensive audit logging
    PasHelpers::getInstance()->submitTrainPAEndLiveAnnouncementAuditMessage(
        broadcastId,
        trainLiveBroadcast->getTrainCoverage(),
		trainLiveBroadcast->getSessionId());

    trainLiveBroadcast->endTrainLiveAnnouncement();

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::retryTrainBroadcast
    ( const char* broadcastId,
      const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains,
      CORBA::Boolean hasOverrideOption,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "retryTrainBroadcast" );

    LOGOUT("retryTrainBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to retry a non-existent broadcast",
            broadcastId );
    }

    AbstractTrainBroadcast* trainBroadcast = dynamic_cast<AbstractTrainBroadcast*>(broadcast);
    if (trainBroadcast == NULL)
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "The specified broadcastId is not a train broadcast" );
    }

    // log as a new broadcast - or change later to have a retry message
    switch (trainBroadcast->getBroadcastType())
    {
    case TA_Base_Bus::IPAAgentCorbaDef::TrainLive:
        {
            PasHelpers::getInstance()->submitTrainPASetupLiveAnnouncementAuditMessage(
                broadcastId,
                trains,
                sessionId );
            break;
        }
    case TA_Base_Bus::IPAAgentCorbaDef::TrainDva:
        {
            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfig =
                trainBroadcast->getBroadcastConfigCopy();
            PasHelpers::getInstance()->submitTrainPAPrerecordedAuditMessage(
                broadcastId,
                trains,
                broadcastConfig->theBroadcastConfigSpecifics.theTrainDvaConfig().messageKey,
                broadcastConfig->theBroadcastConfigSpecifics.theTrainDvaConfig().libraryVersion,
                broadcastConfig->theBroadcastConfigSpecifics.theTrainDvaConfig().isEmergency,
                sessionId );
            delete broadcastConfig;
            break;
        }
    }

    // ToDo - use new session id?
    // Queue actual execution in BroadcastWorker thread
    trainBroadcast->retry(trains, hasOverrideOption, sessionId);

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::terminateBroadcast ( const char* broadcastId, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "terminateBroadcast" );

    LOGOUT("terminateBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );

    if ( NULL == broadcast)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to terminate a non-existent broadcast ",
            broadcastId );
    }

    if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::StationDva )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Broadcast %s is StationDva while being terminated", broadcastId );
    }
    else if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::TrainDva )
    {
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* config = broadcast->getBroadcastConfigCopy();

        if ( NULL != config )
        {
            if ( config->theBroadcastConfigSpecifics.theTrainDvaConfig().isEmergency )
            {
                PasHelpers::getInstance()->submitTrainPAPrerecordedAbortAuditMessage( 
                    broadcastId, config->theBroadcastConfigSpecifics.theTrainDvaConfig().trains, sessionId );
            }

            delete config;
        }
    }

    if (broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::Termination &&
        broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::Aborting &&
        broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
    {
        // Queue actual termination in BroadcastWorker thread
        broadcast->operatorAbort();
    }

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::removeBroadcast ( const char* broadcastId, const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "removeBroadcast" );

    LOGOUT("removeBroadcast");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );

    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException( "Failed to remove a non-existent broadcast", broadcastId );
    }

    // Flag the table for deletion when the BroadcastWorker is ready
    broadcast->setIsTaggedForRemoval(true);

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::restoreBroadcast(const char* broadcastId, const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY("restoreBroadcast");

    LOGOUT("restoreBroadcast");

    TA_THREADGUARD(m_lock);

    TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById(broadcastId);

    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to remove a non-existent broadcast", broadcastId);
    }

    // Flag the table for deletion when the BroadcastWorker is ready
    broadcast->setIsTaggedForRemoval(false);

    FUNCTION_EXIT;
}


TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* TrainBroadcastProxy::getBroadcastIdTypes()
{
    FUNCTION_ENTRY( "getBroadcastIdTypes" );

    LOGOUT("getBroadcastIdTypes");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    FUNCTION_EXIT;
    return m_refManager.getTrainBroadcastIdTypes();
}


void TrainBroadcastProxy::changeBroadcastId
    ( const char* fromBroadcastId,
      const char* toBroadcastId,
      const char* sessionId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "changeBroadcastId" );

    LOGOUT("changeBroadcastId");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( fromBroadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to change a non-existent broadcast",
            fromBroadcastId );
    }

    if ( !m_refManager.updateBroadcastId( fromBroadcastId, toBroadcastId ))
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, "NewBroadcastId is used by another broadcast" );
    }

    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    updateData.fromBroadcastId = fromBroadcastId;
    updateData.toBroadcastId   = toBroadcastId;
    updateData.broadcastType   = broadcast->getBroadcastType();
    updateData.sessionId       = broadcast->getSessionId().c_str();

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentRenamedBroadcastUpdate(updateData);

    PasHelpers::getInstance()->submitPaChangeBroadcastIdAuditMessage(
        fromBroadcastId,
        toBroadcastId,
        sessionId );

    FUNCTION_EXIT;
}


TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* TrainBroadcastProxy::getBroadcastConfig( const char* broadcastId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getBroadcastConfig" );

    LOGOUT("getBroadcastConfig");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to return the broadcast config for a non-existent broadcast", broadcastId);
    }

    // Return the deep copy
    FUNCTION_EXIT;
    return broadcast->getBroadcastConfigCopy();
}


TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* TrainBroadcastProxy::getProgressReport( const char* broadcastId )
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "getProgressReport" );

    LOGOUT("getProgressReport");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    AbstractBroadcast* broadcast = m_refManager.getBroadcastById( broadcastId );
    if (broadcast == NULL)
    {
        PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
            "Failed to return the broadcast config for a non-existent broadcast", broadcastId);
    }

    // Return the deep copy
    FUNCTION_EXIT;
    return broadcast->getProgressReportCopy();
}


TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* TrainBroadcastProxy::getTrainDvaMessageRecords()
{
    FUNCTION_ENTRY( "getTrainDvaMessageRecords" );

    LOGOUT("getTrainDvaMessageRecords");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* newData = NULL;

    TA_IRS_Bus::PaTrainDvaMessageCachedMap& theMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaTrainDvaMessageCachedMap();
    std::list<TA_IRS_Bus::TrainDvaMessageRecord> allRecords = theMap.getAllRecords();
    std::list<TA_IRS_Bus::TrainDvaMessageRecord>::iterator it = allRecords.begin();

    newData = new TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords();
    newData->length(allRecords.size());
    for (int index=0;    it!=allRecords.end();    ++it,++index)
    {
        (*newData)[index].m_pKeyOrId    = (*it).m_pKeyOrId;
        (*newData)[index].m_label       = (*it).m_label.c_str();
    }

    // There should be no exceptions thrown above
    FUNCTION_EXIT;
    return newData;
}


void TrainBroadcastProxy::broadcastEventTriggeredTrainDva( CORBA::ULong messageKey,
                                                        CORBA::Short libraryVersion,
                                                        const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams,
                                                        const char* sessionId,
                                                        const TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage& trains)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredTrainDva" );

    LOGOUT("broadcastEventTriggeredTrainDva");

    TA_THREADGUARD( m_lock );

	TA_Base_Bus::GenericAgent::ensureControlMode();

    std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::TrainDva);

    // Check if Train Agent is available
    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled." );
    }

    PasHelpers::getInstance()->submitTrainPAPrerecordedAuditMessage(
        broadcastId,
        trains,
        messageKey,
        libraryVersion,
        false,
        sessionId );

    // Check the broadcast limit after sending the audit messages for tracability
    if ( m_refManager.getTrainBroadcastCount() >= CachedConfig::getInstance()->getMaxBroadcasts())
    {
        std::ostringstream msg;
        msg << "Request failed due to broadcast limit ("
            << CachedConfig::getInstance()->getMaxBroadcasts() << ")";

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
             msg.str().c_str() );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            msg.str(),
            sessionId);

        PasHelpers::getInstance()->throwBroadcastLimitReachedException();
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
    TA_Base_Bus::IPAAgentCorbaDef::TrainDvaConfig theSpecifics;

    // Note: string members will make a deep copy from a const char*

    newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::TrainDva;
    newBroadcastConfigP->broadcastId        = broadcastId.c_str();
    newBroadcastConfigP->isEventTriggered   = true;
    newBroadcastConfigP->submitTime         = time( NULL );

    theSpecifics.hasOverrideOption          = false;
    theSpecifics.libraryVersion             = libraryVersion;
    theSpecifics.messageKey                 = messageKey;
    theSpecifics.stCyclicParams         = stCyclicParams;
    theSpecifics.trains                     = trains;

    newBroadcastConfigP->theBroadcastConfigSpecifics.theTrainDvaConfig(theSpecifics);


    AbstractBroadcast* newBroadcast = NULL;

    try
    {
        // Can throw TA_Base_Bus::IPAAgentCorbaDef::PaErrorException

        newBroadcast = new TrainDVABroadcast(newBroadcastConfigP, sessionId);

        newBroadcast->validateBroadcastConfig();
    }
    catch(TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& bpe)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            std::string(bpe.strWhat.in()),
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }
    catch(...)
    {
        delete newBroadcast;
        newBroadcast = NULL;

        LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

        // submit audit message
        PasHelpers::getInstance()->submitPaTrainBroadcastFailureAuditMessages(
            trains,
            CachedConfig::getInstance()->getAgentLocationKey(),
            "",
            sessionId);

        // Pass on to the client

        FUNCTION_EXIT;
        throw;
    }

    m_refManager.addTrainBroadcast( newBroadcast );

    // Notify all PA Managers
    TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

    // Note: string members will make a deep copy from a const char*
    updateData.fromBroadcastId = EMPTY_STRING;
    updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
    updateData.broadcastType   = newBroadcastConfigP->broadcastType;
    updateData.sessionId       = sessionId;

    PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
    PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::broadcastEventTriggeredTrainDvaForStation( CORBA::ULong messageKey,
                                                                  CORBA::Short libraryVersion,
                                                                  const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& stCyclicParams,
                                                                  const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredTrainDvaForStation" );

    LOGOUT("broadcastEventTriggeredTrainDvaForStation");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    // Locked in broadcastEventTriggeredTrainDva()

    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled." );
    }

    TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgentInfo =
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();

    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainListV;
    try
    {
        trainListV = trainAgentInfo->getTrainList();
    }
    catch (...)
    {
        PasHelpers::getInstance()->throwTrainAgentCommsFailureException();
    }

    // Initialise the trains to the maximum potential size
    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trains;
    trains.length( trainListV.size() );

    unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();
    int count = 0;
    for (unsigned int index=0; index< trainListV.size(); ++index)
    {
        // Only filter according to location if we are not Occ
        if (!CachedConfig::getInstance()->getIsMaster())
        {
            if (trainListV[index].currentLocation != locationKey)
            {
                continue;
            }
        }

        // Only want trains in this location

        trains[count++] = trainListV[index].trainId;
    }

    // Resize to the actual number of trains after filtering
    trains.length(count);

    broadcastEventTriggeredTrainDva( messageKey,
                                     libraryVersion,
                                     stCyclicParams,
                                     sessionId,
                                     trains);

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::broadcastEventTriggeredTrainDvaForInbound( CORBA::ULong messageKey,
                                                                  CORBA::Short libraryVersion,
                                                                  const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod,
                                                                  const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredTrainDvaForInbound" );

    LOGOUT("broadcastEventTriggeredTrainDvaForInbound");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    // Locked in broadcastEventTriggeredTrainDva()

    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled." );
    }

    TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgentInfo =
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();


    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainListV;
    try
    {
        trainListV = trainAgentInfo->getTrainList();//limin
    }
    catch (...)
    {
        PasHelpers::getInstance()->throwTrainAgentCommsFailureException();
    }

    // Initialise the trains to the maximum potential size
    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trains;
    trains.length(trainListV.size());

    unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();
    int count = 0;
    for (unsigned int index=0; index<trainListV.size(); ++index)
    {
        // Only filter according to location if we are not Occ
        if (!CachedConfig::getInstance()->getIsMaster())
        {
            if (trainListV[index].currentLocation != locationKey)
            {
                continue;
            }
        }

        if (TA_IRS_Bus::TrainInformationTypes::AtsTrackInner != trainListV[index].track)
        {
            continue;
        }

        // Only want inbound trains in this location

        trains[count++] = trainListV[index].trainId;
    }

    // Resize to the actual number of trains after filtering
    trains.length(count);


    broadcastEventTriggeredTrainDva( messageKey,
                                     libraryVersion,
                                     announcementPeriod,
                                     sessionId,
                                     trains);

    FUNCTION_EXIT;
}


void TrainBroadcastProxy::broadcastEventTriggeredTrainDvaForOutbound( CORBA::ULong messageKey,
                                                                   CORBA::Short libraryVersion,
                                                                   const TA_Base_Bus::IPAAgentCorbaDef::TrainCyclicParams& announcementPeriod,
                                                                   const char* sessionId)
// raises (TA_Base_Core::OperationModeException, IPAAgentCorbaDef::PaErrorException);
{
    FUNCTION_ENTRY( "broadcastEventTriggeredTrainDvaForOutbound" );

    LOGOUT("broadcastEventTriggeredTrainDvaForOutbound");

	TA_Base_Bus::GenericAgent::ensureControlMode();
    // Locked in broadcastEventTriggeredTrainDva()

    if (CachedConfig::getInstance()->getLocalTrainAgentEntityName().empty())
    {
        FUNCTION_EXIT;
        PasHelpers::getInstance()->throwPaErrorException( 
            TA_Base_Bus::IPAAgentCorbaDef::eBadParameter, 
            "The local Train Agent cannot be resolved uniquely in the database.  Train PA broadcasts are disabled." );
    }

    TA_IRS_Bus::TrainInformationCorbaProxyPtr trainAgentInfo =
        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainInformationCorbaProxy();


    TA_IRS_Bus::TrainInformationTypes::TrainDetailsList trainListV;

    try
    {
        trainListV = trainAgentInfo->getTrainList();//limin
    }
    catch (...)
    {
        PasHelpers::getInstance()->throwTrainAgentCommsFailureException();
    }

    // Initialise the trains to the maximum potential size
    TA_Base_Bus::IPAAgentCorbaDef::TrainCoverage trains;
    trains.length(trainListV.size());

    unsigned long locationKey = CachedConfig::getInstance()->getAgentLocationKey();
    int count = 0;
    for (unsigned int index=0; index<trainListV.size(); ++index)
    {
        // Only filter according to location if we are not Occ
        if (!CachedConfig::getInstance()->getIsMaster())
        {
            if (trainListV[index].currentLocation != locationKey)
            {
                continue;
            }
        }

        if (TA_IRS_Bus::TrainInformationTypes::AtsTrackInner != trainListV[index].track)
        {
            continue;
        }

        // Only want outbound trains in this location

        trains[count++] = trainListV[index].trainId;
    }

    // Resize to the actual number of trains after filtering
    trains.length(count);


    broadcastEventTriggeredTrainDva( messageKey,
                                     libraryVersion,
                                     announcementPeriod,
                                     sessionId,
                                     trains);

    FUNCTION_EXIT;
}

} // namespace TA_IRS_App

