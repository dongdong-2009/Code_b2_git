/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/RightsManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * A singleton used to check rights.
  * It posts out windows messages to a window when it should re-check rights.
  * It is also used to handle server online and server offline - by denying
  * some actions.
  *
  */

#include "RightsManager.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::RightsException;

using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::DataException;
using TA_IRS_Bus::LocationCache;

namespace TA_IRS_App
{

    RightsManager* RightsManager::m_theClassInstance;
    TA_Base_Core::NonReEntrantThreadLockable RightsManager::m_singletonLock;


    RightsManager& RightsManager::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new RightsManager();
        }

        FUNCTION_EXIT;
        return *m_theClassInstance;
    }


    RightsManager::RightsManager()
        : m_windowsToNotify(),
          m_serverIsOnline(true),
          m_rightsLibrary(NULL)
    {
        FUNCTION_ENTRY( "RightsManager" );

        // build the rights library
        TA_Base_Bus::RightsLibraryFactory factory;
        m_rightsLibrary = factory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

        // register for session id changes
        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        FUNCTION_EXIT;
    }


    RightsManager::~RightsManager()
    {
        FUNCTION_ENTRY( "~RightsManager" );

        RunParams::getInstance().deregisterRunParamUser(this);

        if (m_rightsLibrary != NULL)
        {
            delete m_rightsLibrary;
            m_rightsLibrary = NULL;
        }

        FUNCTION_EXIT;
    }


    void RightsManager::registerForRightsChanges(CWnd* window)
    {
        FUNCTION_ENTRY( "registerForRightsChanges" );

        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_windowsToNotify.push_back(window);
        }

        FUNCTION_EXIT;
    }


    void RightsManager::deregisterForRightsChanges(CWnd* window)
    {
        FUNCTION_ENTRY( "deregisterForRightsChanges" );

        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_windowsToNotify.begin(),
                                                          m_windowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_windowsToNotify.end() )
        {
            m_windowsToNotify.erase(findIter);
        }

        FUNCTION_EXIT;
    }


    void RightsManager::setServerStatus(bool online)
    {
        FUNCTION_ENTRY( "setServerStatus" );

        if (online != m_serverIsOnline)
        {
            m_serverIsOnline = online;

            // update what the user can do
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            postMessageToWindows(m_windowsToNotify, WM_UPDATE_RIGHTS);
        }

        FUNCTION_EXIT;
    }


    void RightsManager::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY( "onRunParamChange" );

        // if the session id changes, rights have changed
        if ( 0 == name.compare(RPARAM_SESSIONID) )
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            postMessageToWindows(m_windowsToNotify, WM_UPDATE_RIGHTS);
        }

        FUNCTION_EXIT;
    }


    void RightsManager::postMessageToWindows(const std::vector<CWnd*>& windows, int message)
    {
        FUNCTION_ENTRY( "postMessageToWindows" );

        for (std::vector<CWnd*>::const_iterator iter = windows.begin();
        iter != windows.end(); iter++)
        {
            // if the window is not null and is a window
            if ( *iter != NULL )
            {
                (*iter)->PostMessage(message);
            }
        }

        FUNCTION_EXIT;
    }


    bool RightsManager::checkPermission(unsigned long actionKey, unsigned long resourceKey)
    {
        FUNCTION_ENTRY( "checkPermission" );

        TA_ASSERT(m_rightsLibrary != NULL, "The rights library is null");

        // if the server is not online, then nothing will work anyway
        // so disallow it
        if (!m_serverIsOnline)
        {
            FUNCTION_EXIT;
            return false;
        }

        bool allowed = false;

        try
        {
            std::string reasonString;
            TA_Base_Bus::EDecisionModule decisionModule;

            std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

            allowed =  m_rightsLibrary->isActionPermittedOnResource(
                        sessionId, resourceKey, actionKey, reasonString, decisionModule );
        }
        catch (RightsException& re)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", re.what());
        }

        FUNCTION_EXIT;
        return allowed;
    }


    unsigned long RightsManager::getTrainAgentResourceKey()
    {
        FUNCTION_ENTRY( "getTrainAgentResourceKey" );

        // if its already been loaded
        if (m_trainAgentResourceKey.hasBeenSet())
        {
            FUNCTION_EXIT;
            return m_trainAgentResourceKey.getValue();
        }

        IEntityDataList trainEntity;
        TA_Base_Core::IResource* resource = NULL;
        try
        {
            trainEntity = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                                        TA_Base_Core::TrainAgentEntityData::getStaticType(),
                                        LocationCache::getInstance().getLocationKey() );

            // no agent
            if ( trainEntity.size() < 1 )
            {
                TA_THROW(DataException("No Train agent at this location", DataException::NO_VALUE, "") );
            }
            // multiple agents
            else if ( trainEntity.size() > 1 )
            {
                TA_THROW(DataException("Multiple Train agents at this location", DataException::NOT_UNIQUE, "") );
            }

            resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(trainEntity[0]->getKey());
            m_trainAgentResourceKey.setValue( resource->getKey() );

            // clean up
            for (IEntityDataList::iterator iter = trainEntity.begin();
                 iter != trainEntity.end(); iter++)
            {
                delete (*iter);
            }
            trainEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            FUNCTION_EXIT;
            return m_trainAgentResourceKey.getValue();
        }
        catch(...)
        {
            // clean up
            for (IEntityDataList::iterator iter = trainEntity.begin();
                 iter != trainEntity.end(); iter++)
            {
                delete (*iter);
            }
            trainEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            FUNCTION_EXIT;
            throw;
        }
    }


    unsigned long RightsManager::getTisAgentResourceKey(RightsManager::TisType tisType)
    {
        FUNCTION_ENTRY( "getTisAgentResourceKey" );

        // if its already been loaded
        if (m_tisAgentResourceKey[tisType].hasBeenSet())
        {
            FUNCTION_EXIT;
            return m_tisAgentResourceKey[tisType].getValue();
        }

        std::string staticType;

        switch (tisType)
        {
        case TTIS:
            staticType = "TTISManager";
            break;
        case STIS:
            staticType = "STISManager";
            break;
        }

        IEntityDataList tisEntity;
        TA_Base_Core::IResource* resource = NULL;
        try
        {
            //Limin, added parameter "true"
            tisEntity = EntityAccessFactory::getInstance().getEntitiesOfType( staticType, true );

            // no TIS agent
            if ( tisEntity.size() < 1 )
            {
                TA_THROW(DataException("No stis/ttis manager", DataException::NO_VALUE, "") );
            }
            // multiple TIS agents
            else if ( tisEntity.size() > 1 )
            {
                TA_THROW(DataException("Multiple stis/ttis manager", DataException::NOT_UNIQUE, "") );
            }

            resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(tisEntity[0]->getKey());
            m_tisAgentResourceKey[tisType].setValue( resource->getKey() );

            // clean up
            for (IEntityDataList::iterator iter = tisEntity.begin();
                 iter != tisEntity.end(); iter++)
            {
                delete (*iter);
            }
            tisEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            FUNCTION_EXIT;
            return m_tisAgentResourceKey[tisType].getValue();
        }
        catch(...)
        {
            // clean up
            for (IEntityDataList::iterator iter = tisEntity.begin();
                 iter != tisEntity.end(); iter++)
            {
                delete (*iter);
            }
            tisEntity.clear();

            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            throw;
        }
    }


    bool RightsManager::canDisplayTTISFreeText()
    {
        FUNCTION_ENTRY( "canDisplayTTISFreeText" );

        unsigned long resourceKey = 0;
        try
        {
            resourceKey = getTisAgentResourceKey(TTIS);
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Error getting resource key. Functionality will be disallowed.");

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_TRAIN_TIS_FREETEXT_MESSAGE, resourceKey);
    }


    bool RightsManager::canDisplayTTISPredefined()
    {
        FUNCTION_ENTRY( "canDisplayTTISPredefined" );

        unsigned long resourceKey = 0;
        try
        {
            resourceKey = getTisAgentResourceKey(TTIS);
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Error getting resource key. Functionality will be disallowed.");

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_TRAIN_TIS_STORED_MESSAGE, resourceKey);
    }


    bool RightsManager::canDisplaySTISFreeText()
    {
        FUNCTION_ENTRY( "canDisplaySTISFreeText" );

        unsigned long resourceKey = 0;
        try
        {
            resourceKey = getTisAgentResourceKey(STIS);
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Error getting resource key. Functionality will be disallowed.");

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_FREE_TEXT, resourceKey);
    }


    bool RightsManager::canDisplaySTISPredefined()
    {
        FUNCTION_ENTRY( "canDisplaySTISPredefined" );

        unsigned long resourceKey = 0;
        try
        {
            resourceKey = getTisAgentResourceKey(STIS);
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Error getting resource key. Functionality will be disallowed.");

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_PRE_DEFINED, resourceKey);
    }


    bool RightsManager::canSubmitRatis()
    {
        FUNCTION_ENTRY( "canSubmitRatis" );

        unsigned long resourceKey = 0;
        try
        {
            resourceKey = getTisAgentResourceKey(STIS);
        }
        catch (...)
        {
            // cant find an agent to use
            // so the function wont be available.
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "Error getting resource key. Functionality will be disallowed.");

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return checkPermission(TA_Base_Bus::aca_STATION_TIS_RATIS_APPROVE, resourceKey);
    }


} // end namespace TA_App
