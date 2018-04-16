/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TrainTimeScheduleManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/08/03 14:32:03 $
  * Last modified by:  $Author: builder $
  *
  * This is used to manage the current list of time schedules.
  * Windows will use it to retrieve the list, and to load time schedules.
  *
  * Interested windows will receive a message when the list of time schedules change.
  * WM_UPDATE_TIMESCHEDULE
  * 
  * Interested windows will also receive a message when the current time schedule version
  * changes. WM_UPDATE_CURRENT_TIME_SCHEDULE
  */

#include "stdafx.h"
#include "TrainTimeScheduleManager.h"
#include "TTISMessageManager.h"
#include "TTISCommonConfiguration.h"
#include "resource.h"

#include <algorithm>

#include "boost/shared_ptr.hpp"

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TISComms_MessageTypes.h"


#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ObjectResolutionException;
using TA_Base_Bus::TISAgentAccessFactory;

namespace TA_IRS_App
{
    
    TrainTimeScheduleManager* TrainTimeScheduleManager::m_theClassInstance;
    unsigned int TrainTimeScheduleManager::m_refCounter = 0;
    TA_Base_Core::NonReEntrantThreadLockable TrainTimeScheduleManager::m_singletonLock;


    const std::string TrainTimeScheduleManager::CURRENT_TIME_SCHEDULE_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-CurrentTimeScheduleVersion");


    TrainTimeScheduleManager& TrainTimeScheduleManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
            m_theClassInstance = new TrainTimeScheduleManager();
		}

        // increment reference counter
        ++m_refCounter;
		
		return *m_theClassInstance;
    }


    void TrainTimeScheduleManager::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        TA_ASSERT(m_refCounter > 0, "m_refCounter <= 0");

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            delete m_theClassInstance;
            m_theClassInstance = NULL;
        }
    }


    TrainTimeScheduleManager::TrainTimeScheduleManager()
        : m_currentTimeScheduleKey(0),
          m_currentTimeScheduleVersion(0),
          m_scheduleMap(),
          m_reverseScheduleMap(),
          m_lastScheduleChangedPkey(0),
          m_lastOperation(TA_Base_Core::Added),
          m_tsvWindowsToNotify(),
          m_scheduleChangeWindowsToNotify(),
          m_scadaProxyFactory( TA_Base_Bus::ScadaProxyFactory::getInstance() )
    {
        // register for time schedule version changes (datapoint proxy)
        registerForTimeSceduleVersionChanges();

        // register for time schedule changes (for when someone edits a schedule)
        registerForTimeScheduleChanges();
    }


    TrainTimeScheduleManager::~TrainTimeScheduleManager()
    {
		FUNCTION_ENTRY("~TrainTimeScheduleManager");
        m_currentTimeScheduleVersionDp.reset();

        // stop scada factory
        TA_Base_Bus::ScadaProxyFactory::removeInstance();

        // unsubscribe from messages
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		FUNCTION_EXIT;
    }


    void TrainTimeScheduleManager::registerCurrentTimeScheduleUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_tsvWindowsToNotify.push_back(window);
        }
    }


    void TrainTimeScheduleManager::deregisterCurrentTimeScheduleUser(CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_tsvWindowsToNotify.begin(),
                                                          m_tsvWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_tsvWindowsToNotify.end() )
        {
            m_tsvWindowsToNotify.erase(findIter);
        }
    }


    void TrainTimeScheduleManager::notifyWindowsOfTimeScheduleVersionChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_tsvWindowsToNotify, WM_UPDATE_CURRENT_TIME_SCHEDULE);
    }


    void TrainTimeScheduleManager::registerForTimeScheduleChanges(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_scheduleChangeWindowsToNotify.push_back(window);
        }
    }


    void TrainTimeScheduleManager::deregisterForTimeScheduleChanges(CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_scheduleChangeWindowsToNotify.begin(),
                                                          m_scheduleChangeWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_scheduleChangeWindowsToNotify.end() )
        {
            m_scheduleChangeWindowsToNotify.erase(findIter);
        }
    }


    void TrainTimeScheduleManager::notifyWindowsOfTimeScheduleChange(const TA_Base_Core::TimeScheduleChange& scheduleChange,
                                                                     bool scheduleNameListChanged)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        for (std::vector<CWnd*>::const_iterator iter = m_scheduleChangeWindowsToNotify.begin();
        iter != m_scheduleChangeWindowsToNotify.end(); iter++)
        {
            // if the window is not null and is a window
            if ( *iter != NULL )
            {
                // copy the structure for each message
                TA_Base_Core::TimeScheduleChange* messageData = new TA_Base_Core::TimeScheduleChange(scheduleChange);

                int ret = (*iter)->PostMessage( WM_UPDATE_TIMESCHEDULE,
                                                reinterpret_cast<WPARAM>(messageData),
                                                static_cast<LPARAM>(scheduleNameListChanged));

                if (ret == 0)
                {
                    LOG_GENERIC(
                    SourceInfo, DebugUtil::DebugError,
                    "Error posting time schedule update message");

                    // clean up - it wasnt posted so never will be deleted
                    delete messageData;
                    messageData = NULL;
                }
            }
        }
    }


    void TrainTimeScheduleManager::postMessageToWindows(const std::vector<CWnd*>& windows, int message)
    {
        for (std::vector<CWnd*>::const_iterator iter = windows.begin();
        iter != windows.end(); iter++)
        {
            // if the window is not null and is a window
            if ( *iter != NULL )
            {
                (*iter)->PostMessage(message);
            }
        }
    }


    void TrainTimeScheduleManager::registerForTimeSceduleVersionChanges()
    {
	
        // get initial values
		FUNCTION_ENTRY("registerForTimeSceduleVersionChanges");
        try
        {
            //TA_Base_Core::TimeScheduleVersion currentVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTrainTimeScheduleVersion();
            TA_Base_Core::TimeScheduleVersion currentVersion ;//= TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTrainTimeScheduleVersion();
			//libo 
			try
			{
				CORBA_CALL_RETURN(currentVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentTrainTimeScheduleVersion,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(currentVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentTrainTimeScheduleVersion,());
			}
			//++libo


            m_currentTimeScheduleKey = currentVersion.timeScheduleKey;
            m_currentTimeScheduleVersion = currentVersion.version;
        }
        catch (...)
        {
            // failed to read the value from the agent
        }

        try
        {
			TA_Base_Core::IEntityData* entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(CURRENT_TIME_SCHEDULE_VERSION_DP_NAME);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentTimeScheduleVersionDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(currentTimeScheduleVersionDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

            // create data point proxies
           m_scadaProxyFactory.createDataPointProxy(currentTimeScheduleVersionDp, *this, m_currentTimeScheduleVersionDp);

            m_scadaProxyFactory.setProxiesToControlMode();
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch (...)
        {
            // data point proxy creation error
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxies" );
        }
		FUNCTION_EXIT;
    }


    void TrainTimeScheduleManager::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // this means either the next library version has changed, or the current version has changed

        // check the type of update
        if (updateType == TA_Base_Bus::ValueStateUpdate)
        {

            // determine which datapoint changed
            if (m_currentTimeScheduleVersionDp->getEntityKey() == entityKey)
            {
                // get the new values
                try
                {
                    //TA_Base_Core::TimeScheduleVersion currentVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTrainTimeScheduleVersion();
                    TA_Base_Core::TimeScheduleVersion currentVersion;//= TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTrainTimeScheduleVersion();
				//libo 
				try
				{
					CORBA_CALL_RETURN(currentVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentTrainTimeScheduleVersion,());
				}
				catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
				{                                                                                                               
					LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
					CORBA_CALL_RETURN(currentVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentTrainTimeScheduleVersion,());
				}
				//++libo


                    m_currentTimeScheduleKey = currentVersion.timeScheduleKey;
                    m_currentTimeScheduleVersion = currentVersion.version;

                    // tell the windows the version has changed
                    notifyWindowsOfTimeScheduleVersionChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received current time schedule version change. Unable to get new value from agent.");
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Received unknown data point update for entity %d", entityKey);
            }
        }
    }


    TA_IRS_Core::ITrainTimeSchedule* TrainTimeScheduleManager::getTrainTimeSchedule(std::string name)
    {
        TA_Base_Core::ThreadGuard guard(m_scheduleMapLock);

        // look up in the map for the schedule key
        // find the matching time schedule
        TimeScheduleMap::iterator scheduleInfo = m_scheduleMap.find( name );
        
        // this must be in the map
        if ( scheduleInfo != m_scheduleMap.end() )
        {
            // get the schedule from the factory
            return TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().getTrainTimeSchedule(scheduleInfo->second);
        }

        // not found
        return NULL;
    }


    TA_IRS_Core::ITrainTimeSchedule* TrainTimeScheduleManager::getTrainTimeSchedule(unsigned long key)
    {
        // get the schedule from the factory
        return TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().getTrainTimeSchedule(key);
    }


    TA_IRS_Core::ITrainTimeSchedule* TrainTimeScheduleManager::createNewTrainTimeSchedule()
    {
        return TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().createNewTrainTimeSchedule();
    }


    TA_IRS_Core::ITrainTimeSchedule* TrainTimeScheduleManager::copyTrainTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* toClone)
    {
        return TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().copyTrainTimeSchedule(toClone);
    }


    void TrainTimeScheduleManager::saveTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* scheduleToSave)
    {
        TA_Base_Core::ETimeScheduleChangeType changeType;

        // if it's new
        if (scheduleToSave->isNew())
        {
            // it was an add operation
            changeType = TA_Base_Core::Added;
        }
        else // its existing,
        {
            // it was an update operation
            changeType = TA_Base_Core::Modified;
        }

        // save the changes
        // any invalid data, like a non-unique name will cause an exception
        scheduleToSave->applyChanges();

        // store the details of the update, so when the message does come in,
        // it can be disregarded
        {
            TA_Base_Core::ThreadGuard guard(m_lastOperationLock);
            m_lastScheduleChangedPkey = scheduleToSave->getKey();
            m_lastOperation = changeType;
        }

        // locally update this GUI, in case the message doesnt come back quick enough
        handleTimeScheduleChange(scheduleToSave->getKey(), changeType);

        // tell the agent to send a message
        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            /*TISAgentAccessFactory::getInstance().getTTISAgent()->timeScheduleChanged( 
                    scheduleToSave->getKey(),
                    changeType,
                    sessionId.c_str());
			*/
			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),timeScheduleChanged,
					(scheduleToSave->getKey(),
                    changeType,
                    sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),timeScheduleChanged,
					(scheduleToSave->getKey(),
                    changeType,
                    sessionId.c_str()));
			}
			//++libo
        }
        catch ( const TA_Base_Core::TransactiveException& te )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what() );
	    }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
        }
    }


    void TrainTimeScheduleManager::deleteTimeSchedule(TA_IRS_Core::ITrainTimeSchedule* scheduleToDelete)
    {
        // if it's new - ignore it, theres nothing to delete
        if (scheduleToDelete->isNew())
        {
            return;
        }

        // its existing,

        TA_Base_Core::TimeScheduleChange change;
        change.changeType = TA_Base_Core::Deleted;
        change.timeSchedulePkey = scheduleToDelete->getKey();

        // save the changes
        scheduleToDelete->deleteThisObject();

        // store the details of the update, so when the message does come in,
        // it can be disregarded
        {
            TA_Base_Core::ThreadGuard guard(m_lastOperationLock);
            m_lastScheduleChangedPkey = change.timeSchedulePkey;
            m_lastOperation = change.changeType;
        }


        // locally update this GUI, in case the message doesnt come back quick enough
        handleTimeScheduleChange(change.timeSchedulePkey, change.changeType);

        // tell the agent to send a message
        try
        {
            // get the session Id
            std::string sessionId = TTISCommonConfiguration::getInstance().getSessionId();

            /*TISAgentAccessFactory::getInstance().getTTISAgent()->timeScheduleChanged( 
                    change.timeSchedulePkey,
                    change.changeType,
                    sessionId.c_str());*/
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),timeScheduleChanged,(change.timeSchedulePkey,change.changeType,sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),timeScheduleChanged,(change.timeSchedulePkey,change.changeType,sessionId.c_str()));
			}
			//++libo
        }
        catch ( const TA_Base_Core::TransactiveException& te )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what() );
	    }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
        }
    }


    void TrainTimeScheduleManager::registerForTimeScheduleChanges()
    {
		
        // load the time schedules
        loadTimeScheduleNames();

        // register with the TIS agent for time schedule update messages
        try
        {
            unsigned int locationKey = TTISCommonConfiguration::getInstance().getLocationKey();

            TA_Base_Core::IEntityDataList entityData =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                    TA_Base_Core::TTISEntityData::getStaticType(), locationKey);


            if( entityData.size() != 1)
            {
                const char* msg = "There is no TIS agent at this location.";
                const char* fld = "TIS Agent";
                TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
            }

            unsigned long entityKey    = entityData[0]->getKey();
            unsigned long subsystemKey = entityData[0]->getSubsystem();

            delete entityData[0];

            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TISComms::TisTrainTimeScheduleChange,
                this,
                entityKey,
                subsystemKey,
                locationKey);

        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
	    }
        catch ( const TA_Base_Core::DataException& de )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
	    }
    }


    void TrainTimeScheduleManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        // handle the time schedule changed message
        // ignore it if it originated here, and has been handled locally
        if (std::string(message.messageTypeKey) ==
            TA_Base_Core::TISComms::TisTrainTimeScheduleChange.getTypeKey())
        {
            TA_Base_Core::TimeScheduleChange* scheduleUpdate;

            if( ( message.messageState >>= scheduleUpdate ) != 0 )
            {
                TA_Base_Core::ThreadGuard guard(m_lastOperationLock);

                // check if it is the last change this gui made
                if ( ( scheduleUpdate->timeSchedulePkey == m_lastScheduleChangedPkey) &&
                     ( scheduleUpdate->changeType == m_lastOperation) )
                {
                    // this is the last message this GUI caused. Reset values
                    m_lastScheduleChangedPkey = 0;
                    m_lastOperation = TA_Base_Core::Added;

                    // skip handling the message, its already been done
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Skipping already handled time schedule update.");
                }
                else
                {
                    // message is from somewhere else, it hasnt been handle yet
                    handleTimeScheduleChange( scheduleUpdate->timeSchedulePkey,
                                              scheduleUpdate->changeType);
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Could not extract TimeScheduleChange from CommsMessageCorbaDef");
                return;
            }
        }
    }


    void TrainTimeScheduleManager::handleTimeScheduleChange(unsigned long pkey, TA_Base_Core::ETimeScheduleChangeType changeType)
    {
        TA_Base_Core::TimeScheduleChange change;
        change.timeSchedulePkey = pkey;
        change.changeType = changeType;
        
        // the name list will need reloading if
        // the change is an add or delete
        bool scheduleNameListChanged = (changeType == TA_Base_Core::Added) || (changeType == TA_Base_Core::Deleted);

        // or the change is a modify that changed the items name
        if ( !scheduleNameListChanged )
        {   
            TA_Base_Core::ThreadGuard guard(m_scheduleMapLock);

            // search for the name in the map
            ReverseTimeScheduleMap::iterator scheduleInfo = m_reverseScheduleMap.find( pkey );
        
            // if its not found, or the name is different
            // then the list must be reloaded
            if ( scheduleInfo != m_reverseScheduleMap.end() )
            {
                // is the name different

                try
                {
                    // load the time schedule that changed
                    TA_IRS_Core::ITrainTimeSchedule* changedSchedule = 
                        TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().getTrainTimeSchedule(pkey);

                    // if the load failed, or the name differs
                    if ( (changedSchedule != NULL) ||
                         (changedSchedule->getName() != scheduleInfo->second) )
                    {
                        scheduleNameListChanged = true;
                    }
                }
                catch (...)
                {
                    // error loading, better refresh lists
                    LOG ( SourceInfo, DebugUtil::ExceptionCatch,
                          "...", "Error loading the changed time schedule. Refreshing list." );

                    scheduleNameListChanged = true;
                }
            }
            else
            {
                // not found
                scheduleNameListChanged = true;
            }
        }

        // if it changed, reload the list
        if ( scheduleNameListChanged )
        {
            loadTimeScheduleNames();
        }
        
        notifyWindowsOfTimeScheduleChange(change, scheduleNameListChanged);
    }


    void TrainTimeScheduleManager::loadTimeScheduleNames()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleManager::loadTimeSchedules" );
    
        // temporary, all valid names will be added to m_scheduleMap
        std::vector<TA_IRS_Core::ITrainTimeSchedule*> tempTrainTimeSchedules;
    
        // load the time schedules from the database
        try
        {
            tempTrainTimeSchedules = TA_IRS_Core::TrainTimeScheduleAccessFactory::getInstance().getTrainTimeSchedules();
        }
        catch (DatabaseException& dbe)
        {
            LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", dbe.what() );
        
            TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_TIME_SCHEDULES, TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE);
        }
        // the only difference when handling a database versus a dataexception
        // is that a dataexception will give some sort of error with the data that will
        // need to be fixed.
        // also no data is not really an error.
        catch (DataException& de)
        {
            LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", de.what() );
        
            // dont display the pop up if there is no data
            // thats not really an error
            if (de.getFailType() != DataException::NO_VALUE)
            {
                TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_TIME_SCHEDULES, TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE);
            }
        }
    
    
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
            "Loaded %d time schedules", tempTrainTimeSchedules.size() );
    

        {
            // aquire the lock
            TA_Base_Core::ThreadGuard guard(m_scheduleMapLock);

    
            // make sure the list is empty
            m_scheduleMap.clear();
            m_reverseScheduleMap.clear();
    
            // populate the list of time schedules
            try
            {
                for(std::vector<TA_IRS_Core::ITrainTimeSchedule*>::iterator iter = tempTrainTimeSchedules.begin();
                iter != tempTrainTimeSchedules.end(); iter++)
                {
                    try
                    {
                        m_scheduleMap[(*iter)->getName()] = (*iter)->getKey();
                        m_reverseScheduleMap[(*iter)->getKey()] = (*iter)->getName();
                    }
                    // can be a DatabaseException or a DataException
                    // if it's a dataexception, theres something wrong with
                    // the individual schedule
                    // just ignore the schedule
                    catch (DataException& de)
                    {
                        LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DataException", de.what() );

                        std::stringstream msg;
                        msg << TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE_PKEY
                            << (*iter)->getKey();
                
                
                        TTISMessageManager::getInstance().displayError(msg.str().c_str());
                    }
                }
            }
            // if it's a database exception
            // don't pop up a message for each failed schedule
            catch (DatabaseException& de)
            {
                LOG ( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", de.what() );
        
                TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_TIME_SCHEDULES, TTISMessageManager::ERROR_LOADING_TIME_SCHEDULE);
            }

            // release the lock
        }
    

        // clean up database objects
        for(std::vector<TA_IRS_Core::ITrainTimeSchedule*>::iterator iter = tempTrainTimeSchedules.begin();
        iter != tempTrainTimeSchedules.end(); iter++)
        {
            if ( *iter != NULL )
            {
                delete *iter;
                *iter = NULL;
            }
        }

    
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleManager::loadTimeSchedules" );
    }


} // end namespace TA_IRS_App
