/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/STISPredefinedMessages.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/04/08 13:44:49 $
  * Last modified by:  $Author: builder $
  *
  * This is used to load the current STIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  */

#include "stdafx.h"
#include "boost/shared_ptr.hpp"
#include "STISPredefinedMessages.h"
#include "UserMessages.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "core/exceptions/src/ScadaProxyException.h"


#include <algorithm>

using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ObjectResolutionException;
using TA_Base_Core::ILocation;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Bus::TISAgentAccessFactory;

namespace TA_IRS_App
{
    
    STISPredefinedMessages* STISPredefinedMessages::m_theClassInstance;
    TA_Base_Core::NonReEntrantThreadLockable STISPredefinedMessages::m_singletonLock;


	//hongran++ TD17500
	// All tis agent will just register the central lib version upates
    const std::string STISPredefinedMessages::NEXT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion-CDB");
    const std::string STISPredefinedMessages::CURRENT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion-CDB");
    const std::string STISPredefinedMessages::LIBRARY_SYNCHRONISED_DP_NAME(".TIS.ISCS.ISCS.diiTISC-StationLibrariesSynchronised");
	//++hongran TD17500

    int STISPredefinedMessages::m_referenceCount = 0;  // TD11310

    STISPredefinedMessages* STISPredefinedMessages::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
            m_theClassInstance = new STISPredefinedMessages();

            m_referenceCount = 1;
		}
        else
        {
            ++m_referenceCount;
        }
		
		return m_theClassInstance;
    }

    // TD11310 ~ added
    void STISPredefinedMessages::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if (0 == --m_referenceCount)
        {        
		    delete m_theClassInstance;
            m_theClassInstance = NULL;
        }
        
        TA_ASSERT(m_referenceCount >= 0, "Redundant call to removeInstance.");
    }


    STISPredefinedMessages::STISPredefinedMessages()
        : m_currentSTISLibraryVersion(0),
          m_nextSTISLibraryVersion(0),
          m_librariesAreSynchronised(false),
          m_currentWindowsToNotify(),
          m_nextWindowsToNotify(),
          m_syncWindowsToNotify(),
          m_normalSTISPredefinedMessages(),
          m_emergencySTISPredefinedMessages(),
          m_normalMessageIndexFromId(),
          m_emergencyMessageIndexFromId(),
          m_defaultLedAttributes(),
          m_defaultPlasmaAttributes(),
          m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance())  // TD11310
    {
		FUNCTION_ENTRY( "STISPredefinedMessages" );
        // register for library version changes
        registerForLibraryChanges();

        // the pre-defined messages will be loaded once
        // the data point proxies are initialised
		FUNCTION_EXIT;
    }


    STISPredefinedMessages::~STISPredefinedMessages()
    {
		FUNCTION_ENTRY( "STISPredefinedMessages" );
        // stop scada factory
		// 2008-07-18 cleart the datapointproxysmartptr
		// the name of the current and next library datapoints at this location
        m_currentLibraryDp.reset();
        m_nextLibraryDp.reset();

		// OCC library version datapoints
        m_currentOCCLibraryDp.reset();
        m_nextOCCLibraryDp.reset();
        m_librarySynchronisedDp.reset();  
		
		// 2008-07-18 to solve the scadaproxyfactory singleton usage
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT
    }


    void STISPredefinedMessages::registerCurrentVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_currentWindowsToNotify.push_back(window);
        }
    }


    void STISPredefinedMessages::deregisterCurrentVersionUser(CWnd* window)
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        // find the map entry
        std::vector<CWnd*>::iterator findIter = std::find(m_currentWindowsToNotify.begin(),
                                                          m_currentWindowsToNotify.end(),
                                                          window);

        // if found, delete it
        if ( findIter != m_currentWindowsToNotify.end() )
        {
            m_currentWindowsToNotify.erase(findIter);
        }
    }



    void STISPredefinedMessages::registerNextVersionUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_nextWindowsToNotify.push_back(window);
        }
    }


    void STISPredefinedMessages::deregisterNextVersionUser(CWnd* window)
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

    void STISPredefinedMessages::registerLibrarySynchronisedUser(CWnd* window)
    {
        if (window != NULL)
        {
            TA_Base_Core::ThreadGuard guard(m_windowListLock);

            // add it to the map
            m_syncWindowsToNotify.push_back(window);
        }
    }


    void STISPredefinedMessages::deregisterLibrarySynchronisedUser(CWnd* window)
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


    void STISPredefinedMessages::notifyWindowsOfCurrentVersionChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_currentWindowsToNotify, WM_UPDATE_CURRENT_STIS_VERSION);
    }


    void STISPredefinedMessages::notifyWindowsOfNextVersionChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_nextWindowsToNotify, WM_UPDATE_NEXT_STIS_VERSION);
    }

    void STISPredefinedMessages::notifyWindowsOfLibrarySynchronisationChange()
    {
        TA_Base_Core::ThreadGuard guard(m_windowListLock);

        postMessageToWindows(m_syncWindowsToNotify, WM_UPDATE_LIBRARY_SYNCHRONISED);
    }

    
    void STISPredefinedMessages::postMessageToWindows(const std::vector<CWnd*>& windows, int message)
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


    const TA_Base_Core::PredefinedMessage* STISPredefinedMessages::getNormalMessageById(int messageId)
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter = 
            m_normalMessageIndexFromId.find(messageId);
         
        // if the message was found
        if ( messageIndexIter != m_normalMessageIndexFromId.end() )
        {
            // return a pointer to the pre-defined message at the index we have found
            return &( m_normalSTISPredefinedMessages[messageIndexIter->second] );
        }
        else
        {
            // not found
            return NULL;
        }
    }
    
    
    const TA_Base_Core::PredefinedMessage* STISPredefinedMessages::getEmergencyMessageById(int messageId)
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);

        // find the message by id

        std::map<int, unsigned int>::const_iterator messageIndexIter = 
            m_emergencyMessageIndexFromId.find(messageId);
         
        // if the message was found
        if ( messageIndexIter != m_emergencyMessageIndexFromId.end() )
        {
            // return a pointer to the pre-defined message at the index we have found
            return &( m_emergencySTISPredefinedMessages[messageIndexIter->second] );
        }
        else
        {
            // not found
            return NULL;
        }
    }
    
    
    void STISPredefinedMessages::registerForLibraryChanges()
    {
		try
		{
            // get the location name
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;

            ILocation* location = LocationAccessFactory::getInstance().getLocationByKey(locationKey);

            // insert it into the map
            m_locationName = location->getName();

            // clean up
            delete location;
		}
		catch( ... )
		{

		}

        try
        {
            // Establish the current and next STIS predefined message library versions
            // this will provide the initial error if it fails
			//libo++
			try
			{
				CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
			}

			try
			{
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
			}
			try
			{
				CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
			}
			//++libo

        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );

            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
        
            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
	    }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_AGENT_NO_PREDEFINED);
        }
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, " catch TA_Base_Core::DatabaseException,STisManger otherException");
		}

        try
        {
            // TD11310 ~ added calls to access factories
            TA_Base_Core::EntityAccessFactory& entityAccessFactory = 
                TA_Base_Core::EntityAccessFactory::getInstance();

            // build the datapoint names
			//hongran++ TD17500
            std::string currentLibraryDpName = CURRENT_LIBRARY_VERSION_DP_NAME;
            std::string nextLibraryDpName = NEXT_LIBRARY_VERSION_DP_NAME;
            std::string librarySyncDpName = "OCC" + LIBRARY_SYNCHRONISED_DP_NAME;
			//++hongran TD17500

			// create data point proxies
			TA_Base_Core::IEntityData* entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(currentLibraryDpName);

			boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

			TA_ASSERT(currentLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

			m_scadaProxyFactory.createDataPointProxy(currentLibraryDp, *this, m_currentLibraryDp); // TD11310 ~ edited

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(nextLibraryDpName);

			boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextLibraryDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

			TA_ASSERT(nextLibraryDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

			m_scadaProxyFactory.createDataPointProxy(nextLibraryDp, *this, m_nextLibraryDp); // TD11310 ~ edited

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(librarySyncDpName);

			boost::shared_ptr<TA_Base_Core::DataPointEntityData> librarySyncDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

			TA_ASSERT(librarySyncDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

			m_scadaProxyFactory.createDataPointProxy(librarySyncDp, *this, m_librarySynchronisedDp); // TD11310 ~ edited

            m_scadaProxyFactory.setProxiesToControlMode();  // TD11310 ~ edited
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
    }


    void STISPredefinedMessages::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // this means either the next library version has changed, or the current version has changed

        // check the type of update
		//TD 14395
		//zhou yuan++ 
		//when create the datapoint proxy, it will receive the callback of type TA_Base_Bus::ConfigAvailable
        if ( updateType == TA_Base_Bus::ValueStateUpdate ||
             updateType == TA_Base_Bus::AlarmSummaryUpdate || //limin++, TD20740
             updateType == TA_Base_Bus::AckSummaryUpdate ||   //limin++, TD20740
             updateType == TA_Base_Bus::ConfigAvailable )
        {

            // determine which datapoint changed

            if ( m_currentLibraryDp->getEntityKey() == entityKey)
            {
                // an upgrade has just happened

                // get the new value
                try
                {
                    CORBA_CALL_RETURN( m_currentSTISLibraryVersion, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBSTISMessageLibraryVersion, () );//limin

                    // reload the libraries
                    loadSTISPredefinedMessages();

                    // tell the windows the version has changed
                    notifyWindowsOfCurrentVersionChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received current STIS message library version change. Unable to get new value from agent.");
                }
            }
            else if ( m_nextLibraryDp->getEntityKey() == entityKey)
            {
                // the next version has been upgraded

                // get the new value
                try
                {
                    CORBA_CALL_RETURN( m_nextSTISLibraryVersion, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), getNextCDBSTISMessageLibraryVersion, () );//limin

                    // tell the windows the version has changed
                    notifyWindowsOfNextVersionChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received next STIS message library version change. Unable to get new value from agent.");
                }
            }
           else if (m_librarySynchronisedDp->getEntityKey() == entityKey)
            {
                // the message libraries are synchronised (or have become not synchronised)

                // get the new value
                try
                {
                    CORBA_CALL_RETURN( m_librariesAreSynchronised, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), isStationLibrarySynchronisationComplete, () );//limin

                    // tell the windows the status has changed
                    notifyWindowsOfLibrarySynchronisationChange();
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received next STIS message library version change. Unable to get new value from agent.");
                }

            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Received unknown data point update for entity %d", entityKey);
            }
        }
    }

    
    void STISPredefinedMessages::loadSTISPredefinedMessages()
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);

        try
        {
            // build the pre-defined message maps
            // clear them first
            m_normalSTISPredefinedMessages.clear();
            m_normalMessageIndexFromId.clear();
            m_emergencySTISPredefinedMessages.clear();
            m_emergencyMessageIndexFromId.clear();

            // get the STIS message libraries
            TA_Base_Core::IPredefinedMessageLibrary* STISMessageLibrary =  
                TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(m_currentSTISLibraryVersion,
                                                                                                          TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
        

            // a vector of pointers to pre-defined message structures
            TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefinedMessages;
            predefinedMessages = STISMessageLibrary->getMessages();

            // index count
            unsigned int normalIndex = 0;
            unsigned int emergencyIndex = 0;

            for (TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList::iterator messageIter = predefinedMessages.begin();
            messageIter != predefinedMessages.end(); messageIter++)
            {
                // sort by severity
                if( (*messageIter)->librarySection == TA_Base_Core::NORMAL_SECTION )
                {
                    // add to the normal priority pre-defined messages
                    m_normalSTISPredefinedMessages.push_back( *(*messageIter) );

                    // index by the tag (id)
                    m_normalMessageIndexFromId[(*messageIter)->messageTag] = normalIndex;

                    // increment the index
                    normalIndex++;
                }
                else if( (*messageIter)->librarySection == TA_Base_Core::EMERGENCY_SECTION )
                {
                    // add to the normal priority pre-defined messages
                    m_emergencySTISPredefinedMessages.push_back( *(*messageIter) );
                
                    // index by the tag (id)
                    m_emergencyMessageIndexFromId[(*messageIter)->messageTag] = emergencyIndex;

                    // increment the index
                    emergencyIndex++;
                }
                else
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                        "Unknown library section %d. Ignoring message with tag %d",
                        (*messageIter)->librarySection, (*messageIter)->messageTag );
                }
            }
          

            // get the default adhoc attributes
            m_defaultLedAttributes = STISMessageLibrary->getDefaultSTISLedAttributes();
            m_defaultPlasmaAttributes = STISMessageLibrary->getDefaultSTISPlasmaAttributes();
        

            delete STISMessageLibrary;
            STISMessageLibrary = NULL;
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );

            // this means there is an error with the data
        
            // if theres no data in the database
            if (de.getFailType() == DataException::NO_VALUE)
            {
                // this is only an error if the load attempt was for a valid version
                if (m_currentSTISLibraryVersion > 0)
                {
                    UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_NO_PREDEFINED_IN_DB);
                }
            }
            // otherwise its a general error
            else
            {
                UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
            }
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
        
            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_LOAD_PREDEFINED, UserMessages::ERROR_LOADING_PREDEFINED);
        }
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, " catch TA_Base_Core::DatabaseException,STisManger otherException");
		}

    }


} // end namespace TA_IRS_App
