/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/call_page/src/VettingManager.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Class to handle vetting requirements. It informs the TIS
  * Agent if vetting should be turned on or off, and listens
  * for messages from the TIS Agent in case the vetting status
  * changes.
  *
  */

#include "VettingManager.h"

#include "app\system_control\banner\pages\call_page\src\IVettingCallback.h"
//#include "bus\signs\tis_agent_access\src\TISAgentAccessFactory.h"
#include "core/message/src/MessageSubscriptionManager.h"
//#include "core/message/types/TisComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
//#include "core/data_access_interface/entity_access/src/TISAgentEntityData.h"
//#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    VettingManager::VettingManager(IVettingCallback& callback, unsigned long locationKey)
        : m_callback(callback)
    {
/*
        try
        {
            //
            //
            // Get the tis agent's entity data from its type and location.
            //
            std::vector<TA_Base_Core::IEntityData*> entities =
		        TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
		        TA_Base_Core::TISAgentEntityData::getStaticType(), locationKey);
            //
            // connect straight to the TIS Agent at OCC even if this is a station
            int occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
            std::vector<TA_Base_Core::IEntityData*> entities =
			    TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
			    TA_Base_Core::STISEntityData::getStaticType(), occLocationKey);
    
            if(entities.size() != 1)
            {
                const char* msg = "There is no TIS agent at this location.";
                const char* fld = "TIS Agent";
                TA_THROW(TA_Base_Core::DataException(msg, TA_Base_Core::DataException::MISSING_MANDATORY, fld));
            }

            unsigned long entityKey    = 0;
            unsigned long subsystemKey = 0;
            std::string entityName     = "";

            try
            {
                entityKey    = entities[0]->getKey();
                subsystemKey = entities[0]->getSubsystem();
                entityName   = entities[0]->getName();
            }
            catch(...)
            {
                delete entities[0];
                entities[0] = NULL;
                throw;
            }
            delete entities[0];
            entities[0] = NULL;

	        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TISComms::RATISVetting,
                (TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>*)(this),
                entityKey,
                subsystemKey,
                occLocationKey); // subscribing directly with the OCC agent

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Subscribing to RATISVetting on %s = %d, subsystem = %d, location = %d", entityName.c_str(), entityKey, subsystemKey, occLocationKey);

            // Retrieve the current vetting state.
            CORBA::Boolean vetting = TA_IRS_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent()->isRATISVettingOn();
            m_isVetting = (vetting == 1 ? true : false);
        }
        catch( const CORBA::Exception& ce )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ce).c_str());
            m_isVetting = true;
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error retrieving initial vetting state");
            // TODO: Display error
            m_isVetting = true;
        }
*/
    }


    VettingManager::~VettingManager()
    {
        // Unsubscribe to messages
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
    }


    void VettingManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");

        // Extract to the message info
        CORBA::Boolean vetting;
        if( ( message.messageState >>= vetting ) == 0 )
        {
            FUNCTION_EXIT;
            return;
        }

        // Notify the observer.
        m_isVetting = (vetting == 1 ? true : false);
        m_callback.vettingStateChanged(m_isVetting);

        FUNCTION_EXIT;
    }


    void VettingManager::setVettingState(bool vettingState)
    {
        FUNCTION_ENTRY("setVettingState");
/*
        try
        {
            CORBA::Boolean isVetting = (vettingState ? 1 : 0);
            // m_tisAgent->setRATISVetting(isVetting);
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_IRS_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent()->setRATISVetting(isVetting, sessionId.c_str());
        }
        catch( const CORBA::SystemException& se )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(se).c_str());
            m_isVetting = true;
        }
        catch( TA_Base_Core::TransactiveException& ex )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Error setting vetting state");
            // TODO: Display error
            m_isVetting = true;
        }
*/
        FUNCTION_EXIT;
    }

} // namespace TA_IRS_App
