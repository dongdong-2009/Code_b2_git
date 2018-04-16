/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Rob Ashcroft
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * 
  *
  */

#include "stdafx.h"
#include "UserMessages.h"
#include "LibraryVersionMonitor.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"


#include "core/exceptions/src/ScadaProxyException.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"


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
    
    LibraryVersionMonitor* LibraryVersionMonitor::m_theClassInstance;
    TA_Base_Core::NonReEntrantThreadLockable LibraryVersionMonitor::m_singletonLock;

    int LibraryVersionMonitor::m_referenceCount = 0;

    LibraryVersionMonitor* LibraryVersionMonitor::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

		// If guard acquired then make sure the singleton is still NULL.

		if ( m_theClassInstance == NULL )
		{
			// Create the one & only object
            m_theClassInstance = new LibraryVersionMonitor();

            m_referenceCount = 1;
		}
        else
        {
            ++m_referenceCount;
        }
		
		return m_theClassInstance;
    }

    // TD11310 ~ added
    void LibraryVersionMonitor::removeInstance()
    {
        TA_Base_Core::ThreadGuard guard(m_singletonLock);

        if (0 == --m_referenceCount)
        {
            delete m_theClassInstance;
            m_theClassInstance = NULL;
        }

        TA_ASSERT(m_referenceCount >= 0, "Redundant call to removeInstance.");
    }


    LibraryVersionMonitor::LibraryVersionMonitor()
    : m_windowToNotify( NULL ),           
      m_scadaProxyFactory(TA_Base_Bus::ScadaProxyFactory::getInstance())    // TD11310 ~ added
    {
		FUNCTION_ENTRY( "LibraryVersionMonitor" );
        // setup datapoint proxies
        setupVersionInfo();
		FUNCTION_EXIT;
    }


    LibraryVersionMonitor::~LibraryVersionMonitor()
    {
		FUNCTION_ENTRY( "~LibraryVersionMonitor" );
        // stop scada factory
        //m_scadaProxyFactory.removeInstance(); // TD11310 ~ edited

		m_libraryVersionDatapoints.clear();
		
		// 2008-07-18 to solve the scadaproxyfactory singleton usage
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT
    }


    void LibraryVersionMonitor::registerForChanges( CWnd* window )
    {
        if (window != NULL)
        {
 			if( m_windowToNotify == NULL )
			{
				m_windowToNotify = window;

	
				// TEST *********

/*				LibraryVersionInfo *versionInfo = new LibraryVersionInfo();

				versionInfo->entityKey = 40000879;
				versionInfo->value = 2;

				// post it. A reinterpret cast or c-style cast is needed to convert the pointer to an unsigned int.
				int ret = m_windowToNotify->PostMessage( WM_UPDATE_LIBRARY_VERSION,
											   reinterpret_cast<unsigned int>(versionInfo) );
*/

			}
			else
			{
				TA_ASSERT(false,"A second window is attempting to register with the LibraryVersionMonitor");
			}
        }
    }


    void LibraryVersionMonitor::deregisterForChanges( CWnd* window )
    {
		m_windowToNotify = NULL;
    }

    void LibraryVersionMonitor::notifyWindowsOfChange( unsigned long entityKey, unsigned short newValue )
    {
		if( m_windowToNotify != NULL )
		{
			if (::IsWindow(m_windowToNotify->m_hWnd) == false)
			{	//It is not a valid window
				return;
			}
			LibraryVersionInfo *versionInfo = new LibraryVersionInfo();

			versionInfo->entityKey = entityKey;
			versionInfo->value = newValue;

			// post it. A reinterpret cast or c-style cast is needed to convert the pointer to an unsigned int.
			int ret = m_windowToNotify->PostMessage( WM_UPDATE_LIBRARY_VERSION,
										   reinterpret_cast<unsigned int>(versionInfo) );
			if (ret == 0)
			{
				LOG_GENERIC(
				SourceInfo, DebugUtil::DebugError,
				"Error posting library version update message. %d", WM_UPDATE_LIBRARY_VERSION);

				// clean up - it wasnt posted so never will be deleted
				delete versionInfo;
				versionInfo = NULL;
			}

		}
		else
		{
			LOG_GENERIC(
				SourceInfo, DebugUtil::DebugInfo,
				"No windows are registered with the LibraryVersionMonitor to receive library version updates");
		}
        
    }


    void LibraryVersionMonitor::setupVersionInfo()
    {
        try
        {
			// Create appropriate datapoint proxies for each location

 			std::vector<ILocation*> locations = LocationAccessFactory::getInstance().getAllLocations();

			//liqiang++
			std::string currentISCSLibraryDpNames;
			std::string nextISCSLibraryDpNames;
			std::string currentSTISLibraryDpNames;
			std::string nextSTISLibraryDpNames;
			std::string strSingleQuotes("'"); 
			for( int ii=0; ii < locations.size() ; ii++ )
			{
				std::string locationName = locations[ii]->getName();
				if(!currentISCSLibraryDpNames.empty())
				{
					currentISCSLibraryDpNames += ",";
				}
				currentISCSLibraryDpNames += strSingleQuotes + locationName + CURRENT_ISCS_STATION_LIBRARY_VERSION_DP_NAME + strSingleQuotes ;
				
				if(!nextISCSLibraryDpNames.empty())
				{
					nextISCSLibraryDpNames += ",";
				}
				nextISCSLibraryDpNames += strSingleQuotes + locationName + NEXT_ISCS_STATION_LIBRARY_VERSION_DP_NAME + strSingleQuotes ;
				
				if(!currentSTISLibraryDpNames.empty())
				{
					currentSTISLibraryDpNames += ",";
				}
				currentSTISLibraryDpNames += strSingleQuotes + locationName + CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME + strSingleQuotes ;
				
				if(!nextSTISLibraryDpNames.empty())
				{
					nextSTISLibraryDpNames += ",";
				}
				nextSTISLibraryDpNames += strSingleQuotes + locationName + NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME + strSingleQuotes ;
			}
			std::transform(currentISCSLibraryDpNames.begin(), currentISCSLibraryDpNames.end(), currentISCSLibraryDpNames.begin(), toupper);
			std::transform(nextISCSLibraryDpNames.begin(), nextISCSLibraryDpNames.end(), nextISCSLibraryDpNames.begin(), toupper);
			std::transform(currentSTISLibraryDpNames.begin(), currentSTISLibraryDpNames.end(), currentSTISLibraryDpNames.begin(), toupper);
			std::transform(nextSTISLibraryDpNames.begin(), nextSTISLibraryDpNames.end(), nextSTISLibraryDpNames.begin(), toupper);
			TA_Base_Core::IEntityDataList currentISCSLibraryDpNameList = TA_Base_Core::EntityAccessFactory::getInstance().getEntityWithNameInToken(currentISCSLibraryDpNames);
			TA_Base_Core::IEntityDataList nextISCSLibraryDpNameList = TA_Base_Core::EntityAccessFactory::getInstance().getEntityWithNameInToken(nextISCSLibraryDpNames);
			TA_Base_Core::IEntityDataList currentSTISLibraryDpNameList = TA_Base_Core::EntityAccessFactory::getInstance().getEntityWithNameInToken(currentSTISLibraryDpNames);
			TA_Base_Core::IEntityDataList nextSTISLibraryDpNameList = TA_Base_Core::EntityAccessFactory::getInstance().getEntityWithNameInToken(nextSTISLibraryDpNames);
			//++liqiang

			// build the datapoints

			int i=0;
	        for( ; i < locations.size() && i < currentISCSLibraryDpNameList.size(); i++ )
			{
			   // insert it into the map
				//std::string locationName = locations[i]->getName();
				unsigned long locationKey = locations[i]->getKey();
				unsigned long entityKey = 0;
				
				//Bohong++ for CL20818
				LibraryVersionDataPointInfo *ISCSCurrentlibraryVersionInfo = new LibraryVersionDataPointInfo();				
				LibraryVersionDataPointInfo *ISCSNextlibraryVersionInfo = new LibraryVersionDataPointInfo();
				LibraryVersionDataPointInfo *STISCurrentlibraryVersionInfo = new LibraryVersionDataPointInfo();
				LibraryVersionDataPointInfo *STISNextlibraryVersionInfo = new LibraryVersionDataPointInfo();
				//++Bohong

				try
				{
				// ************  Current ISCS library version
					//std::string currentISCSLibraryDpName = locationName + CURRENT_ISCS_STATION_LIBRARY_VERSION_DP_NAME;

					//TA_Base_Core::IEntityData* entityDate = NULL;
					//entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(currentISCSLibraryDpName);

					boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentISCSLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(currentISCSLibraryDpNameList[i]));

					TA_ASSERT(currentISCSLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");
						
					m_scadaProxyFactory.createDataPointProxy(currentISCSLibDp, *this, ISCSCurrentlibraryVersionInfo->dataPoint);

					ISCSCurrentlibraryVersionInfo->type = CURRENT_VERSION;
					ISCSCurrentlibraryVersionInfo->locationKey = locationKey;
					entityKey = ISCSCurrentlibraryVersionInfo->dataPoint->getEntityKey();

					{
						TA_Base_Core::ThreadGuard guard(m_messageLock);

						m_libraryVersionDatapoints.insert
							(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, ISCSCurrentlibraryVersionInfo ));
					}
					//TD 14395
					//zhou yuan ++
					//TA_Base_Core::Thread::sleep(500);

				// ************  Next ISCS library version
					//std::string nextISCSLibraryDpName = locationName + NEXT_ISCS_STATION_LIBRARY_VERSION_DP_NAME;

					//entityDate = NULL;
					//entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(nextISCSLibraryDpName);

					//boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextISCSLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(nextSTISLibraryDpNameList[i]));
					boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextISCSLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(nextISCSLibraryDpNameList[i]));

					TA_ASSERT(nextISCSLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

					m_scadaProxyFactory.createDataPointProxy(nextISCSLibDp, *this, ISCSNextlibraryVersionInfo->dataPoint);

					ISCSNextlibraryVersionInfo->type = NEXT_VERSION;
					ISCSNextlibraryVersionInfo->locationKey = locationKey;
					entityKey = ISCSNextlibraryVersionInfo->dataPoint->getEntityKey();

					{
						TA_Base_Core::ThreadGuard guard(m_messageLock);

						m_libraryVersionDatapoints.insert
							(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, ISCSNextlibraryVersionInfo ));
					}
					//TD 14395
					//zhou yuan++
					//TA_Base_Core::Thread::sleep(500);

				// ************  Current STIS library version
					//std::string currentSTISLibraryDpName = locationName + CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME;

					//entityDate = NULL;
					//entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(currentSTISLibraryDpName);

					boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentSTISLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(currentSTISLibraryDpNameList[i]));

					TA_ASSERT(currentSTISLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

					m_scadaProxyFactory.createDataPointProxy(currentSTISLibDp, *this, STISCurrentlibraryVersionInfo->dataPoint);

					STISCurrentlibraryVersionInfo->type = CURRENT_VERSION;
					STISCurrentlibraryVersionInfo->locationKey = locationKey;
					entityKey = STISCurrentlibraryVersionInfo->dataPoint->getEntityKey();

					{
						TA_Base_Core::ThreadGuard guard(m_messageLock);

						m_libraryVersionDatapoints.insert
							(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, STISCurrentlibraryVersionInfo ));
					}
					//TD 14395
					//zhou yuan++
					//TA_Base_Core::Thread::sleep(500);

				// ************  Next STIS library version
					//std::string nextSTISLibraryDpName = locationName + NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME;

					//entityDate = NULL;
					//entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(nextSTISLibraryDpName);

					boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextSTISLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(nextSTISLibraryDpNameList[i]));

					TA_ASSERT(nextSTISLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

					m_scadaProxyFactory.createDataPointProxy(nextSTISLibDp, *this, STISNextlibraryVersionInfo->dataPoint);

					STISNextlibraryVersionInfo->type = NEXT_VERSION;
					STISNextlibraryVersionInfo->locationKey = locationKey;
					entityKey = STISNextlibraryVersionInfo->dataPoint->getEntityKey();

					{
						TA_Base_Core::ThreadGuard guard(m_messageLock);

						m_libraryVersionDatapoints.insert
							(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, STISNextlibraryVersionInfo ));
					}
					//TD 14395
					//zhou yuan++
					//TA_Base_Core::Thread::sleep(500);
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
					LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxy" );
				}
			}

			//hongran++ TD17500
			// The DPs of central DB should be get separately
			try
			{
				// ************  Current central STIS library version ****************************
				LibraryVersionDataPointInfo *currentLibraryVersionInfo = new LibraryVersionDataPointInfo();
				LibraryVersionDataPointInfo * nextLibraryVersionInfo = new LibraryVersionDataPointInfo();
				TA_Base_Core::IEntityData* entityDate = NULL;
				unsigned long currentDpEntitykey, nextDpEntityKey;
				entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(CENTRAL_CURRENT_STIS_STATION_LIBRARY_VERSION_DP_NAME);

				boost::shared_ptr<TA_Base_Core::DataPointEntityData> currentISCSLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

				TA_ASSERT(currentISCSLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");
					
				m_scadaProxyFactory.createDataPointProxy(currentISCSLibDp, *this, currentLibraryVersionInfo->dataPoint);

				currentLibraryVersionInfo->type = CURRENT_VERSION;
				currentLibraryVersionInfo->locationKey = LocationAccessFactory::getInstance().getOccLocationKey();
				currentDpEntitykey = currentLibraryVersionInfo->dataPoint->getEntityKey();

				{
					TA_Base_Core::ThreadGuard guard(m_messageLock);

					m_libraryVersionDatapoints.insert
						(std::pair<unsigned long,LibraryVersionDataPointInfo*>( currentDpEntitykey, currentLibraryVersionInfo ));
				}

				// ************  Next central STIS library version

				entityDate = NULL;
				entityDate = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(CENTRAL_NEXT_STIS_STATION_LIBRARY_VERSION_DP_NAME);

				boost::shared_ptr<TA_Base_Core::DataPointEntityData> nextSTISLibDp(dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDate));

				TA_ASSERT(nextSTISLibDp.get(), "can't create boost::shared_ptr<TA_Base_Core::DataPointEntityData> befroe create proxy.");

				m_scadaProxyFactory.createDataPointProxy(nextSTISLibDp, *this, nextLibraryVersionInfo->dataPoint);

				nextLibraryVersionInfo->type = NEXT_VERSION;
				nextLibraryVersionInfo->locationKey = LocationAccessFactory::getInstance().getOccLocationKey();
				nextDpEntityKey = nextLibraryVersionInfo->dataPoint->getEntityKey();

				{
					TA_Base_Core::ThreadGuard guard(m_messageLock);

					m_libraryVersionDatapoints.insert
						(std::pair<unsigned long,LibraryVersionDataPointInfo*>( nextDpEntityKey, nextLibraryVersionInfo ));
				}
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
				LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxy" );
			}
			

			// clean up
			for( i=0; i < locations.size() ; i++ )
			{
				delete locations[i];
			}

            m_scadaProxyFactory.setProxiesToControlMode(); // TD11310 ~ edited
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


    void LibraryVersionMonitor::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // check the type of update
		//TD 14395
		//zhou yuan++ 
		//when create the datapoint proxy, it will receive the callback of type TA_Base_Bus::ConfigAvailable
        if ( updateType == TA_Base_Bus::ValueStateUpdate ||
             updateType == TA_Base_Bus::AlarmSummaryUpdate || //limin++, TD20740
             updateType == TA_Base_Bus::AckSummaryUpdate ||   //limin++, TD20740
             updateType == TA_Base_Bus::ConfigAvailable )
        {
			std::map<unsigned long, LibraryVersionDataPointInfo*>::iterator updatedDatapoint;

			{
				TA_Base_Core::ThreadGuard guard(m_messageLock);

				updatedDatapoint = m_libraryVersionDatapoints.find( entityKey );
			}

            // Ensure the datapoint was found
            if ( updatedDatapoint != m_libraryVersionDatapoints.end() )
            {
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Received library version datapoint change");

                // establish the new value of the datapoint
                try
                {
					unsigned short newValue = static_cast<unsigned short>( updatedDatapoint->second->dataPoint->getValueAsDouble() );

                    // tell the windows the version has changed
                    notifyWindowsOfChange( entityKey, newValue );
                }
                catch (...)
                {
                    // failed to read the value from the agent
                    // ignore the change, something is wrong

                    LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 "Received library version datapoint change. Unable to get new value from agent.");
                }
            }
	        else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Received unknown data point update for entity %d", entityKey);
            }
        }
    }

	std::map<unsigned long, unsigned short> LibraryVersionMonitor::getAllLibraryVersions()
	{
		TA_Base_Core::ThreadGuard guard(m_messageLock);

		std::map<unsigned long,LibraryVersionDataPointInfo*>::iterator iter;

		std::map<unsigned long, unsigned short> versionInfoMap;

		iter = m_libraryVersionDatapoints.begin();

		for( ; iter != m_libraryVersionDatapoints.end() ; iter++ )
		{
			unsigned short value = 0;

			try
			{
				value = static_cast<unsigned short>( iter->second->dataPoint->getValueAsDouble() );
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
							 "entityKey=%d, value=%d", iter->second->dataPoint->getEntityKey(), value );
			}
			catch( ... )
			{
				std::stringstream msg;
				msg << "Could not establish value of datapoint at location ";
				msg << iter->second->locationKey;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                                 msg.str().c_str());
			}

			// Add the acquired info for this point to the map
			versionInfoMap.insert( std::pair<unsigned long, unsigned short>( iter->first, value ) );
		}

		return versionInfoMap;

	}

} // end namespace TA_IRS_App
