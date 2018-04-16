/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/signs/tis_agent_access/src/LibraryVersionMonitor.cpp $
  * @author:  Rob Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * 
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning ( disable : 4786 )

#include <algorithm>
#include "boost/scoped_ptr.hpp"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/threads/src/Thread.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::DataException;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::ObjectResolutionException;
using TA_Base_Core::ILocation;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Bus::TISAgentAccessFactory;

#include "ILibraryVersionListener.h"

namespace TA_IRS_Bus
{
    std::vector<LibraryVersionMonitor*> LibraryVersionMonitor::monitors;


	unsigned long LibraryVersionMonitor::getEntityKeyFromName( const std::string& entityName )
	{
        TA_Base_Core::IEntityData* entityData = NULL;
        unsigned long entityKey = 0;

        try
        {
            entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
        }
        catch (const TA_Base_Core::DataException& de)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch (const TA_Base_Core::DatabaseException& dbe)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch (...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxy" );
        }


        if( NULL != entityData )
        {
            entityKey = entityData->getKey();
            delete entityData;
        }

        return entityKey;
	}

    void LibraryVersionMonitor::removeFromAllMonitors( ILibraryVersionListener* listener )
    {
        for( std::vector<LibraryVersionMonitor*>::iterator it = monitors.begin();
             it != monitors.end();
             it++ )
        {
            (*it)->deregisterForChanges( listener );
        }
    }

    LibraryVersionMonitor::LibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix )
        : m_libraryVersionDatapoints()
        , m_listeners()
        , m_messageLock()
		, m_dpPostfix(dpPostfix)
        , m_scadaProxyFactory( TA_Base_Bus::ScadaProxyFactory::getInstance() )
    {
        monitors.push_back( this );
		
		//td 14395
		//zhou yuan++
        //this->setupVersionInfo( dpPostfix );
    }


    LibraryVersionMonitor::~LibraryVersionMonitor()
    {
        std::remove( monitors.begin(), monitors.end(), this );

		
		std::map<unsigned long,LibraryVersionDataPointInfo*>::iterator it;

		for(it=m_libraryVersionDatapoints.begin(); it!=m_libraryVersionDatapoints.end(); it++)
		{
			delete it->second;
		}
        m_libraryVersionDatapoints.clear();

        // stop scada factory
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
    }


    void LibraryVersionMonitor::registerForChanges( ILibraryVersionListener* listener )
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);
        this->m_listeners.push_back( listener );
    }


    void LibraryVersionMonitor::deregisterForChanges( ILibraryVersionListener* listener )
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);
        std::remove( this->m_listeners.begin(),
                     this->m_listeners.end(),
                     listener );
    }

    void LibraryVersionMonitor::notifyAllListeners( unsigned long entityKey, unsigned short newValue )
    {
        LibraryVersionInfo versionInfo;

        versionInfo.entityKey = entityKey;
        versionInfo.value = newValue;

        for( std::vector<ILibraryVersionListener*>::iterator it = this->m_listeners.begin();
             it != this->m_listeners.end();
             it++ )
        {
            TA_Base_Core::ThreadGuard guard(m_messageLock);
            
            (*it)->libraryVersionUpdate( versionInfo );
        }
    }


    void LibraryVersionMonitor::setupVersionInfo()
    {
        try
        {
			// Create appropriate datapoint proxies for each location
 			std::vector<ILocation*> locations = LocationAccessFactory::getInstance().getAllLocations();

			// build the datapoints
			for( unsigned int i=0; i < locations.size() ; i++ )
			{
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing location: %s", locations[i]->getName().c_str() );//TD14337 yezh++

			    // insert it into the map
				std::string locationName = locations[i]->getName();
				unsigned long locationKey = locations[i]->getKey();
				unsigned long entityKey = 0;
				

				try
				{
                    for( std::map<std::string,ELibraryVersionPointType>::iterator it = m_dpPostfix.begin();
                         it != m_dpPostfix.end();
                         it++ )
                    {
					    std::string dpName = locationName + it->first;

                        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Processing datapoint: %s", dpName.c_str() );//TD14337 yezh++

						TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dpName);
						TA_Base_Core::DataPointEntityData* dpEntityData = NULL;						
						dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityData);
						TA_ASSERT(NULL != dpEntityData, "Could not cast from IEntityData to DataPointEntityData");

						boost::shared_ptr< TA_Base_Core::DataPointEntityData > sharedDpEntityData(dpEntityData);

						LibraryVersionDataPointInfo *libraryVersionInfo = new LibraryVersionDataPointInfo();

					    m_scadaProxyFactory.createDataPointProxy(sharedDpEntityData, *this, libraryVersionInfo->dataPoint);

					    libraryVersionInfo->type = it->second;
					    libraryVersionInfo->locationKey = locationKey;
					    entityKey = libraryVersionInfo->dataPoint->getEntityKey();

					    {
						    //TA_Base_Core::ThreadGuard guard(m_messageLock);

						    m_libraryVersionDatapoints.insert(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, libraryVersionInfo ));
					    }
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
			}

			// clean up
			for( int i=0; i < locations.size() ; i++ )
			{
				delete locations[i];
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "...", "Error creating datapoint proxies" );
        }
    }


    void LibraryVersionMonitor::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // check the type of update

		//TD 14395
		//zhou yuan ++
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
                    notifyAllListeners( entityKey, newValue );
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
