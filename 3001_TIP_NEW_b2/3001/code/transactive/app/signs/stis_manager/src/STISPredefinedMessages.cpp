/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/STISPredefinedMessages.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #6 $
  *
  * Last modification: $DateTime: 2016/02/02 09:36:31 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * This is used to load the current STIS pre-defined messages.
  * Messages are organised into normal and emergency priorities, and
  * indexed by name and Id.
  *
  */

#include "stdafx.h"
#include "app/signs/stis_manager/src/Resource.h"
#include "app/signs/stis_manager/src/STISPredefinedMessages.h"
#include "app/signs/stis_manager/src/UserMessages.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
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
#include <boost/shared_ptr.hpp>

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


	// All tis agent will just register the central lib version upates
    const std::string STISPredefinedMessages::NEXT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-NextSTISLibraryVersion-CDB");
    const std::string STISPredefinedMessages::CURRENT_LIBRARY_VERSION_DP_NAME("OCC.TIS.ISCS.ISCS.aiiTISC-CurrentSTISLibraryVersion-CDB");
    const std::string STISPredefinedMessages::LIBRARY_SYNCHRONISED_DP_NAME("OCC.TIS.ISCS.ISCS.diiTISC-StationLibrariesSynchronised");

    int STISPredefinedMessages::m_referenceCount = 0;

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
		  m_defaultStisAttributes(),
          m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance())
    {
    }


    STISPredefinedMessages::~STISPredefinedMessages()
    {
        m_currentLibraryDp.reset();
        m_nextLibraryDp.reset();
        m_librarySynchronisedDp.reset();  
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
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
            TA_Base_Core::EntityAccessFactory& entityAccessFactory = 
                TA_Base_Core::EntityAccessFactory::getInstance();

            // build the datapoint names
            std::string currentLibraryDpName = CURRENT_LIBRARY_VERSION_DP_NAME;
            std::string nextLibraryDpName = NEXT_LIBRARY_VERSION_DP_NAME;
            std::string librarySyncDpName = LIBRARY_SYNCHRONISED_DP_NAME;

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

			m_scadaProxyFactory.createDataPointProxy(nextLibraryDp, *this, m_nextLibraryDp);

			entityDate = NULL;
			entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(librarySyncDpName);

			boost::shared_ptr<TA_Base_Core::DataPointEntityData> librarySyncDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

			TA_ASSERT(librarySyncDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

			m_scadaProxyFactory.createDataPointProxy(librarySyncDp, *this, m_librarySynchronisedDp);

            m_scadaProxyFactory.setProxiesToControlMode();
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
			UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
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
		// when create the datapoint proxy, it will receive the callback of type TA_Base_Bus::ConfigAvailable
		// Only care for data change
        if ( updateType == TA_Base_Bus::ValueStateUpdate || updateType == TA_Base_Bus::ConfigAvailable )
        {

            // determine which datapoint changed

            if ( m_currentLibraryDp->getEntityKey() == entityKey)
            {
                // an upgrade has just happened

                // get the new value
                try
                {					
					/*
                    CORBA_CALL_RETURN( m_currentSTISLibraryVersion, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentCDBSTISMessageLibraryVersion, () );//limin
					*/
					// Only reload when current version truly changed
					unsigned long currentLibraryVersion = static_cast<unsigned long>(m_currentLibraryDp->getValueAsDouble());
					if ( 0 != currentLibraryVersion && currentLibraryVersion != m_currentSTISLibraryVersion)
					{
						m_currentSTISLibraryVersion = currentLibraryVersion;
						// reload the libraries
						loadSTISPredefinedMessages();
						// tell the windows the version has changed
						notifyWindowsOfCurrentVersionChange();
					}					
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
					
					/*
                    CORBA_CALL_RETURN( m_nextSTISLibraryVersion, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), getNextCDBSTISMessageLibraryVersion, () );//limin
					*/
					unsigned long nextLibraryVersion = static_cast<unsigned long>(m_nextLibraryDp->getValueAsDouble());
					if( 0 != nextLibraryVersion && nextLibraryVersion != m_nextSTISLibraryVersion )
					{
						m_nextSTISLibraryVersion = nextLibraryVersion;
                        notifyWindowsOfNextVersionChange();
					}
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
					bool isSynchroised = false;
                    CORBA_CALL_RETURN( isSynchroised, 
                        TISAgentAccessFactory::getInstance().getOccSTISAgent(), isStationLibrarySynchronisationComplete, () );
					if( isSynchroised != m_librariesAreSynchronised )
					{
						m_librariesAreSynchronised = isSynchroised;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_librariesAreSynchronised=%d", m_librariesAreSynchronised);
                        notifyWindowsOfLibrarySynchronisationChange();
					}
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

            // get the STIS message libraries, may throw
			std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary> STISMessageLibrary
				(TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary
				(m_currentSTISLibraryVersion, TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS));
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
			// Get related default display attributes
            m_defaultLedAttributes = STISMessageLibrary->getDefaultSTISLedAttributes();
			m_defaultLcdAttributes = STISMessageLibrary->getDefaultSTISLcdAttributes();
			m_defaultStisAttributes = STISMessageLibrary->getDefaultSTISAttributes();
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Failed load library %d from database", m_currentSTISLibraryVersion);

            // this means there is an error with the data
        
			/*
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
			*/
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
        
            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
        }
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, " catch TA_Base_Core::DatabaseException,STisManger otherException");
			UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
		}
    }

	void STISPredefinedMessages::init()
	{
        try
		{
            // get the location name
            std::stringstream locationKeyStream;
            locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            unsigned long locationKey = 0;
            locationKeyStream >> locationKey;
			std::auto_ptr<ILocation> location(LocationAccessFactory::getInstance().getLocationByKey(locationKey));
            // insert it into the map
			if(location.get() != NULL)
			{
				m_locationName = location->getName();
			}
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "", "Exception when get location name");
		}

        try
        {
            // Establish the current and next STIS predefined message library versions
            // this will provide the initial error if it fails
			try
			{
				CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
				// Build initial library messages
				loadSTISPredefinedMessages();
				// Notify windows version change
				notifyWindowsOfCurrentVersionChange();
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_currentSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getCurrentCDBSTISMessageLibraryVersion,());
			}

			try
			{
				CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
				notifyWindowsOfNextVersionChange();
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_nextSTISLibraryVersion,TISAgentAccessFactory::getInstance().getOccSTISAgent(),getNextCDBSTISMessageLibraryVersion,());
			}

			try
			{
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_librariesAreSynchronised=%d", m_librariesAreSynchronised);
				notifyWindowsOfLibrarySynchronisationChange();
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(m_librariesAreSynchronised,TISAgentAccessFactory::getInstance().getOccSTISAgent(),isStationLibrarySynchronisationComplete,());
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_librariesAreSynchronised=%d", m_librariesAreSynchronised);
			}
			
        }
        catch (const DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            // this means that there is a problem with the database
        
            UserMessages::getInstance().displayErrorOnce(UserMessages::UNABLE_TO_CONNECT_DATABASE, UserMessages::ERROR_CONNECTION_DATABASE);
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
	    {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            UserMessages::getInstance().displayError(UserMessages::ERROR_CONTACT_AGENT);
	    }
        catch( const CORBA::Exception& cex )
        {
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA",
                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            UserMessages::getInstance().displayError(UserMessages::ERROR_CONTACT_AGENT);
        }
		catch( ... )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, " catch TA_Base_Core::DatabaseException,STisManger otherException");
		}

		registerForLibraryChanges();
	}
} // end namespace TA_IRS_App
