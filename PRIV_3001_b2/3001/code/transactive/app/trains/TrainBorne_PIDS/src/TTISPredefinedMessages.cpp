/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/TTISPredefinedMessages.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2010/07/20 16:12:34 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This is used to load the current TTIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  */

#include "stdafx.h"
#include "TTISPredefinedMessages.h"
#include "TTISMessageManager.h"
#include "TTISCommonConfiguration.h"
#include "resource.h"

#include <algorithm>

#include "boost/shared_ptr.hpp"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/common_library/src/DpValue.h"
#include "RunThreadedFunc.h"
#include "boost/bind.hpp"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ObjectResolutionException;
using TA_Base_Bus::TISAgentAccessFactory;

namespace TA_IRS_App
{

    TTISPredefinedMessages* TTISPredefinedMessages::m_theClassInstance;
    unsigned int TTISPredefinedMessages::m_refCounter = 0;
    TA_Base_Core::NonReEntrantThreadLockable TTISPredefinedMessages::m_singletonLock;


    const std::string TTISPredefinedMessages::NEXT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-NextTTISLibraryVersion-CDB");
    const std::string TTISPredefinedMessages::CURRENT_LIBRARY_VERSION_DP_NAME( "OCC.TIS.ISCS.ISCS.aiiTISC-CurrentTTISLibraryVersion-CDB" );
    const std::string TTISPredefinedMessages::LIBRARY_SYNCHRONISED_DP_NAME(".TIS.ISCS.ISCS.diiTISC-TrainLibrariesSynchronised");



    TTISPredefinedMessages& TTISPredefinedMessages::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( m_theClassInstance == NULL )
        {
            // Create the one & only object
            m_theClassInstance = new TTISPredefinedMessages();
        }

        // increment reference counter
        ++m_refCounter;

        return *m_theClassInstance;
    }


    void TTISPredefinedMessages::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        TA_ASSERT( m_refCounter > 0, "m_refCounter <= 0" );

        // decrement reference counter
        --m_refCounter;

        // if not referenced anywhere
        if ( 0 == m_refCounter )
        {
            delete m_theClassInstance;
            m_theClassInstance = NULL;
        }
    }


    TTISPredefinedMessages::TTISPredefinedMessages()
        : m_currentTTISLibraryVersion( 0 ),
          m_nextTTISLibraryVersion(0),
          m_librariesAreSynchronised(false),
          m_currentWindowsToNotify(),
          m_nextWindowsToNotify(),
          m_syncWindowsToNotify(),
          m_normalTTISPredefinedMessages(),
          m_emergencyTTISPredefinedMessages(),
          m_normalMessageIndexFromId(),
          m_emergencyMessageIndexFromId(),
          m_defaultAttributes(),
          m_scadaProxyFactory( TA_Base_Bus::ScadaProxyFactory::getInstance() )
    {
        // register for library version changes
		runThreadedFunc(boost::bind(&TTISPredefinedMessages::registerForLibraryChanges, this));
        // the pre-defined messages will be loaded once
        // the data point proxies are initialised
    }


    TTISPredefinedMessages::~TTISPredefinedMessages()
    {
        // clear proxies
        FUNCTION_ENTRY( "~TTISPredefinedMessages" );
        m_currentLibraryDp.reset();
        m_nextLibraryDp.reset();
        m_librarySynchronisedDp.reset();

        // stop scada factory
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
        FUNCTION_EXIT;
    }


    void TTISPredefinedMessages::registerCurrentVersionUser( CWnd* window )
    {
        if ( window != NULL )
        {
            TA_Base_Core::ThreadGuard guard( m_windowListLock );

			postMessageToWindow(window, WM_UPDATE_CURRENT_TTIS_VERSION, m_currentTTISLibraryVersion);

            // add it to the map
            m_currentWindowsToNotify.push_back( window );
        }
    }


    void TTISPredefinedMessages::deregisterCurrentVersionUser( CWnd* window )
    {
        TA_Base_Core::ThreadGuard guard( m_windowListLock );

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find( m_currentWindowsToNotify.begin(),
                                                m_currentWindowsToNotify.end(),
                                                window );

        // if found, delete it
        if ( findIter != m_currentWindowsToNotify.end() )
        {
            m_currentWindowsToNotify.erase( findIter );
        }
    }


    void TTISPredefinedMessages::registerNextVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);
			postMessageToWindow(window, WM_UPDATE_NEXT_TTIS_VERSION, m_nextTTISLibraryVersion);
            // add it to the map
            m_nextWindowsToNotify.push_back(window);
        }
    }


    void TTISPredefinedMessages::deregisterNextVersionUser(CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_nextWindowsToNotify.begin(),
                                                          m_nextWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_nextWindowsToNotify.end() )
        {
            m_nextWindowsToNotify.erase(findIter);
        }
    }


    void TTISPredefinedMessages::registerLibrarySynchronisedUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);
			postMessageToWindow(window, WM_UPDATE_LIBRARY_SYNCHRONISED, m_librariesAreSynchronised);
            // add it to the map
            m_syncWindowsToNotify.push_back(window);
        }
    }


    void TTISPredefinedMessages::deregisterLibrarySynchronisedUser(CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_syncWindowsToNotify.begin(),
                                                          m_syncWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_syncWindowsToNotify.end() )
        {
            m_syncWindowsToNotify.erase(findIter);
        }
    }
    void TTISPredefinedMessages::notifyWindowsOfCurrentVersionChange()
    {
        TA_Base_Core::ThreadGuard guard( m_windowListLock );

        postMessageToWindows( m_currentWindowsToNotify, WM_UPDATE_CURRENT_TTIS_VERSION, m_currentTTISLibraryVersion);
    }


    void TTISPredefinedMessages::notifyWindowsOfNextVersionChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_nextWindowsToNotify, WM_UPDATE_NEXT_TTIS_VERSION, m_nextTTISLibraryVersion);
    }


    void TTISPredefinedMessages::notifyWindowsOfLibrarySynchronisationChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_syncWindowsToNotify, WM_UPDATE_LIBRARY_SYNCHRONISED, m_librariesAreSynchronised);
    }
    void TTISPredefinedMessages::postMessageToWindows( const std::vector<CWnd*>& windows, int message, unsigned long value)
    {
        for ( std::vector<CWnd*>::const_iterator iter = windows.begin();
                iter != windows.end(); iter++ )
        {
            // if the window is not null and is a window
			postMessageToWindow(*iter, message, value);
        }
    }

	bool TTISPredefinedMessages::postMessageToWindow(CWnd* window, UINT message, unsigned long value)
	{
		if((NULL != window) && (::IsWindow(window->m_hWnd)))
		{
			return window->PostMessage(message, value);
		}
		return false;
	}

    const TA_Base_Core::PredefinedMessage* TTISPredefinedMessages::getNormalMessageById( int messageId )
    {
        TA_Base_Core::ThreadGuard guard( m_messageLock );

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_normalMessageIndexFromId.find( messageId );

        // if the message was found
        if ( messageIndexIter != m_normalMessageIndexFromId.end() )
        {
            // return a pointer to the pre-defined message at the index we have found
            return &( m_normalTTISPredefinedMessages[messageIndexIter->second] );
        }
        else
        {
            // not found
            return NULL;
        }
    }


    const TA_Base_Core::PredefinedMessage* TTISPredefinedMessages::getEmergencyMessageById( int messageId )
    {
        TA_Base_Core::ThreadGuard guard( m_messageLock );

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter =
            m_emergencyMessageIndexFromId.find( messageId );

        // if the message was found
        if ( messageIndexIter != m_emergencyMessageIndexFromId.end() )
        {
            // return a pointer to the pre-defined message at the index we have found
            return &( m_emergencyTTISPredefinedMessages[messageIndexIter->second] );
        }
        else
        {
            // not found
            return NULL;
        }
    }

    const TA_Base_Core::PredefinedMessage* TTISPredefinedMessages::getMessageById( int messageId )
    {
        TA_Base_Core::ThreadGuard guard( m_messageLock );

        const TA_Base_Core::PredefinedMessage* message = NULL;
        message = getEmergencyMessageById( messageId );
        if( NULL == message )
        {
            message = getNormalMessageById( messageId );
        }
        return message;
    }

    void TTISPredefinedMessages::registerForLibraryChanges()
    {
	
        try
        {
            // Establish the current and next TTIS predefined message library versions
            // this will provide the initial error if it fails

            /*m_currentTTISLibraryVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTTISMessageLibraryVersion();

            m_nextTTISLibraryVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getNextTTISMessageLibraryVersion();

            m_librariesAreSynchronised = TISAgentAccessFactory::getInstance().getOccSTISAgent()->isTrainLibrarySynchronisationComplete();*/

			try
			{
				CORBA_CALL_RETURN(m_currentTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBTTISMessageLibraryVersion,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_currentTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBTTISMessageLibraryVersion,());
			}
			//++libo

			try
			{
				CORBA_CALL_RETURN(m_nextTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBTTISMessageLibraryVersion,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_nextTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBTTISMessageLibraryVersion,());
			}
			//++libo

			try
			{
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isTrainLibrarySynchronisationComplete,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isTrainLibrarySynchronisationComplete,());
			}
			//++libo

        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );

            //TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::NO_TIS_AGENT_PREDEFINED_UNAVAILABLE);
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database        
            //TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::ERROR_ACCESS_DATABASE);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            //TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::NO_TIS_AGENT_PREDEFINED_UNAVAILABLE);
	    }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            //TTISMessageManager::getInstance().displayErrorOnce(TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::NO_TIS_AGENT_PREDEFINED_UNAVAILABLE);
        }

        try
        {
 			//hongran++ TD17500
            // get the occ location key
            unsigned int OccLocationKey = TTISCommonConfiguration::getInstance().getOccLocationKey();
			//++hongran
            std::string OccLocationName = TTISCommonConfiguration::getInstance().getLocationByKey(OccLocationKey);

 			//hongran++ TD17500
			// All tis agent will just register the central lib version updates
            // build the datapoint names
            std::string currentLibraryDpName =  CURRENT_LIBRARY_VERSION_DP_NAME;
            std::string nextLibraryDpName =  NEXT_LIBRARY_VERSION_DP_NAME;
            std::string librarySyncDpName = OccLocationName + LIBRARY_SYNCHRONISED_DP_NAME;
			//++hongran TD17500

            // create data point proxies
			TA_Base_Core::IEntityData* entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(currentLibraryDpName);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(currentLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

			

           m_scadaProxyFactory.createDataPointProxy(currentLibraryDp, *this, m_currentLibraryDp);
		   

			//zhou yuan++
			//TA_Base_Core::Thread::sleep(500);

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(nextLibraryDpName);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(nextLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

            m_scadaProxyFactory.createDataPointProxy(nextLibraryDp, *this, m_nextLibraryDp);
			//zhou yuan++
			//TA_Base_Core::Thread::sleep(500);

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(librarySyncDpName);
			boost::shared_ptr<TA_Base_Core::DataPointEntityData> librarySyncDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));
			TA_ASSERT(librarySyncDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

           m_scadaProxyFactory.createDataPointProxy(librarySyncDp, *this, m_librarySynchronisedDp);
		   
            
		   m_scadaProxyFactory.setProxiesToControlMode();
		 
		   
        }
        catch ( const DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch ( const DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( ... )
        {
            // data point proxy creation error
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxies" );
        }
    }


    void TTISPredefinedMessages::processEntityUpdateEvent( unsigned long entityKey,
            TA_Base_Bus::ScadaEntityUpdateType updateType )
    {
        // this means either the next library version has changed, or the current version has changed

        // check the type of update
        //when create the datapoint proxy, it will receive the callback of type TA_Base_Bus::ConfigAvailable
        if( updateType == TA_Base_Bus::ValueStateUpdate || updateType == TA_Base_Bus::ConfigAvailable )
        {

            // determine which datapoint changed
            if ( m_currentLibraryDp->getEntityKey() == entityKey )
            {
                // an upgrade has just happened

                // get the new value
                try
                {
                    //m_currentTTISLibraryVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getCurrentTTISMessageLibraryVersion();

					//libo 
					try
					{
						CORBA_CALL_RETURN(m_currentTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBTTISMessageLibraryVersion,());
					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL_RETURN(m_currentTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBTTISMessageLibraryVersion,());
					}
					//++libo


                    // reload the libraries
                    loadTTISPredefinedMessages();

                    // tell the windows the version has changed
                    notifyWindowsOfCurrentVersionChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received current ttis message library version change. Unable to get new value from agent.");
                }
            }
            else if ( m_nextLibraryDp->getEntityKey() == entityKey)
            {
                // the next version has been upgraded

                // get the new value
                try
                {
                    //m_nextTTISLibraryVersion = TISAgentAccessFactory::getInstance().getOccSTISAgent()->getNextTTISMessageLibraryVersion();

					//libo 
					try
					{
						CORBA_CALL_RETURN(m_nextTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBTTISMessageLibraryVersion,());
					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL_RETURN(m_nextTTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBTTISMessageLibraryVersion,());
					}
					//++libo

                    // tell the windows the version has changed
                    notifyWindowsOfNextVersionChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received next ttis message library version change. Unable to get new value from agent.");
                }
            }
            else if ( m_librarySynchronisedDp->getEntityKey() == entityKey)
            {
                // the message libraries are synchronised (or have become not synchronised)

                // get the new value
                try
                {
                    //m_librariesAreSynchronised = TISAgentAccessFactory::getInstance().getOccSTISAgent()->isTrainLibrarySynchronisationComplete();

					//libo 
					try
					{
						CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isTrainLibrarySynchronisationComplete,());
					}
					catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
					{                                                                                                               
						LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
						CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isTrainLibrarySynchronisationComplete,());
					}
					//++libo


                    // tell the windows the status has changed
                    notifyWindowsOfLibrarySynchronisationChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received next ttis message library version change. Unable to get new value from agent.");
                }

            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Received unknown data point update for entity %d", entityKey );
            }
        }
    }


    void TTISPredefinedMessages::loadTTISPredefinedMessages()
    {
        TA_Base_Core::ThreadGuard guard( m_messageLock );

        try
        {
            // build the pre-defined message maps
            // clear them first
            m_normalTTISPredefinedMessages.clear();
            m_normalMessageIndexFromId.clear();
            m_emergencyTTISPredefinedMessages.clear();
            m_emergencyMessageIndexFromId.clear();

            // get the TTIS message libraries
            TA_Base_Core::IPredefinedMessageLibrary* ttisMessageLibrary =
                TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( m_currentTTISLibraryVersion,
                        TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS );


            // a vector of pointers to pre-defined message structures
            TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefinedMessages;
            predefinedMessages = ttisMessageLibrary->getMessages();

            // index count
            unsigned int normalIndex = 0;
            unsigned int emergencyIndex = 0;

            for ( TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::iterator messageIter = predefinedMessages.begin();
                    messageIter != predefinedMessages.end(); messageIter++ )
            {
                // sort by severity
                if( ( *messageIter )->librarySection == TA_Base_Core::NORMAL_SECTION )
                {
                    // add to the normal priority pre-defined messages
                    m_normalTTISPredefinedMessages.push_back( *( *messageIter ) );

                    // index by the tag (id)
                    m_normalMessageIndexFromId[( *messageIter )->messageTag] = normalIndex;

                    // increment the index
                    normalIndex++;
                }
                else if( ( *messageIter )->librarySection == TA_Base_Core::EMERGENCY_SECTION )
                {
                    // add to the normal priority pre-defined messages
                    m_emergencyTTISPredefinedMessages.push_back( *( *messageIter ) );

                    // index by the tag (id)
                    m_emergencyMessageIndexFromId[( *messageIter )->messageTag] = emergencyIndex;

                    // increment the index
                    emergencyIndex++;
                }
                else
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Unknown library section %d. Ignoring message with tag %d",
                                 ( *messageIter )->librarySection, ( *messageIter )->messageTag );
                }
            }

            delete ttisMessageLibrary;
            ttisMessageLibrary = NULL;
        }
        catch ( const DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed load predefined message library %d", m_currentTTISLibraryVersion);

            // this means there is an error with the data

            // if theres no data in the database
			/*
            if ( de.getFailType() == DataException::NO_VALUE )
            {
                // this is only an error if the load attempt was for a valid version
                if ( m_currentTTISLibraryVersion > 0 )
                {
                    TTISMessageManager::getInstance().displayErrorOnce( TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::NO_PREDEFINED_LIBRARY );
                }
            }
            // otherwise its a general error
            else
            {
                TTISMessageManager::getInstance().displayErrorOnce( TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::ERROR_LOADING_PREDEFINED );
            }
			*/
        }
        catch ( const DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database

            //TTISMessageManager::getInstance().displayErrorOnce( TTISMessageManager::UNABLE_TO_LOAD_PREDEFINED, TTISMessageManager::ERROR_LOADING_PREDEFINED );
        }

    }


} // end namespace TA_IRS_App
