/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/system_control/banner/pages/call_page/src/RightsChecker.cpp $
  * @author:  Dirk McCormick - Copied from Configuration Editor rights checker by Karen G
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class is used to determine if a user has access rights for a
  * particular action that the call banner can perform.
  */


#include "app/system_control/banner/pages/call_page/src/RightsChecker.h"

#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/RightsException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <sstream>

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;


namespace TA_IRS_App
{
    RightsChecker::RightsChecker() 
        :
        m_rightsLibrary(NULL),
        m_bannerResourceId(0)
    {
        FUNCTION_ENTRY("Constructor");

        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;

        TA_Base_Core::IEntityData* entity = NULL;
        TA_Base_Core::IResource* resource = NULL;

        try
        {
            TA_ASSERT( !RunParams::getInstance().get(RPARAM_SESSIONID).empty(),
                       "Session ID run param must be set" );
            TA_ASSERT( !RunParams::getInstance().get(RPARAM_ENTITYNAME).empty(),
                       "Entity name run param must be set" );
            
            m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

            //
            // Get the entity key from the entity name, and the resource id
            // from the entity key.
            //
            entity =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntity(
                    RunParams::getInstance().get(RPARAM_ENTITYNAME) );
            resource =
                TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(
                    entity->getKey() );

            m_bannerResourceId = resource->getKey();

            delete entity;
            entity = NULL;
            delete resource;
            resource = NULL;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(
                SourceInfo, "Database Error",
                "Could not initialise rights checker.");

            if(m_rightsLibrary != NULL)
            {
                delete m_rightsLibrary;
                m_rightsLibrary = NULL;
            }
            if(entity != NULL)
            {
                delete entity;
                entity = NULL;
            }
            if(resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            throw;
        }
    }


    RightsChecker::~RightsChecker()
    {
        try
        {
            if(m_rightsLibrary != NULL)
            {
                delete m_rightsLibrary;
                m_rightsLibrary = NULL;
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(
                SourceInfo, "Unknown", "Exception caught in destructor");
        }
    }


    bool RightsChecker::canPerformAction(unsigned long action)
    {
        FUNCTION_ENTRY("canPerformAction");

        TA_ASSERT(
            m_rightsLibrary != NULL,
            "The rights library was not constructed correctly so we "
            "cannot determine rights");

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        try
        {
            bool canPerformAction =
                m_rightsLibrary->isActionPermittedOnResource( 
                    RunParams::getInstance().get(RPARAM_SESSIONID),
                    m_bannerResourceId,
                    action,
                    reason,
                    decisionModule );

            //
            // If the action couldn't be performed, log a message saying why
            //
            if( !canPerformAction )
            {
                LOG_GENERIC(
                    SourceInfo, DebugUtil::DebugNorm,
                    "Access denied for action with id '%d' because '%s'",
                    action, reason.c_str()); //TD14337 yezh++
            }

            FUNCTION_EXIT;
            return canPerformAction;
        }
        catch(const TA_Base_Core::RightsException& ex)
        {
            std::ostringstream message;
            message 
                << "Could not determine rights for action with id '" << action
                << "'. Exception: '" << ex.what() << "'";

            LOG_EXCEPTION_CATCH(
                SourceInfo, "RightsException", message.str().c_str());
        }
        catch(...)
        {
            std::ostringstream message;
            message 
                << "Could not determine rights for action with id '"
                << action
                << "'";

            LOG_EXCEPTION_CATCH(
                SourceInfo, "Unknown Exception", message.str().c_str());
        }


        //
        // If rights could not be determined (because an exception was caught)
        // access should be denied.
        //
        FUNCTION_EXIT;
        return false;
    }

}


