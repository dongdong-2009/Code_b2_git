/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/signs/tis_agent_access/src/LibraryVersionMonitor.cpp $
  * @author:  Rob Ashcroft
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2013/04/22 17:18:57 $
  * Last modified by:  $Author: huang.wenbo $
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

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"


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

    LibraryVersionMonitor::LibraryVersionMonitor( std::map<std::string,ELibraryVersionPointType> dpPostfix )
        : m_libraryVersionDatapoints()
        , m_listeners()
        , m_messageLock()
		, m_dpPostfix(dpPostfix)
        , m_scadaProxyFactory( TA_Base_Bus::ScadaProxyFactory::getInstance() )
    {
    }


    LibraryVersionMonitor::~LibraryVersionMonitor()
    {
		
		std::map<unsigned long,LibraryVersionDataPointInfo*>::iterator it;

		for(it=m_libraryVersionDatapoints.begin(); it!=m_libraryVersionDatapoints.end(); it++)
		{
			delete it->second;
		}
        m_libraryVersionDatapoints.clear();
        TA_Base_Bus::ScadaProxyFactory::removeInstance();
    }


    void LibraryVersionMonitor::registerForChanges( ILibraryVersionListener* listener )
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);
        m_listeners.push_back( listener );
    }


    void LibraryVersionMonitor::deregisterForChanges( ILibraryVersionListener* listener )
    {
        TA_Base_Core::ThreadGuard guard(m_messageLock);
        std::remove( this->m_listeners.begin(),
                     this->m_listeners.end(),
                     listener );
    }

	bool LibraryVersionMonitor::isNoListener()
	{
        TA_Base_Core::ThreadGuard guard(m_messageLock);
		return m_listeners.empty();
	}

    void LibraryVersionMonitor::notifyAllListeners( unsigned long entityKey, const LibraryVersionUpdateInfo& updateInfo)
	{
        TA_Base_Core::ThreadGuard guard(m_messageLock);
        for( std::vector<ILibraryVersionListener*>::iterator it = this->m_listeners.begin();
             it != this->m_listeners.end();
             it++ )
        {
            (*it)->libraryVersionUpdate( updateInfo );
        }
	}

    void LibraryVersionMonitor::setupVersionInfo()
    {
		TA_Base_Core::EntityAccessFactory& entityAccessFactory = TA_Base_Core::EntityAccessFactory::getInstance();
        for( std::map<std::string,ELibraryVersionPointType>::iterator itVersionType = m_dpPostfix.begin();
             itVersionType != m_dpPostfix.end();
             ++itVersionType )
		{
			std::string nameLikeToken = std::string("%") + (*itVersionType).first;
			try
			{
				TA_Base_Core::IEntityDataList entityDatas =
					entityAccessFactory.getEntitiesOfTypeWithNameStrictLikeToken(TA_Base_Core::DataPointEntityData::getStaticType(), nameLikeToken);
				for(TA_Base_Core::IEntityDataList::iterator itEntityData = entityDatas.begin(); itEntityData != entityDatas.end(); ++itEntityData)
				{
					TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(*itEntityData);
					TA_ASSERT(NULL != dpEntityData, "Could not cast from IEntityData to DataPointEntityData");
					boost::shared_ptr< TA_Base_Core::DataPointEntityData > sharedDpEntityData(dpEntityData);
					LibraryVersionDataPointInfo *libraryVersionInfo = new LibraryVersionDataPointInfo();
				    m_scadaProxyFactory.createDataPointProxy(sharedDpEntityData, *this, libraryVersionInfo->dataPoint);
				    libraryVersionInfo->type = (*itVersionType).second;
				    libraryVersionInfo->locationKey = (*itEntityData)->getLocation();
				    unsigned long entityKey = libraryVersionInfo->dataPoint->getEntityKey();
				    m_libraryVersionDatapoints.insert(std::pair<unsigned long,LibraryVersionDataPointInfo*>( entityKey, libraryVersionInfo ));
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
    }


    void LibraryVersionMonitor::processEntityUpdateEvent(unsigned long entityKey,
                                                          TA_Base_Bus::ScadaEntityUpdateType updateType)
    {
        // check the type of update

        if ( updateType == TA_Base_Bus::ValueStateUpdate ||
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
					LibraryVersionUpdateInfo newValue;
					newValue.entityKey = entityKey;
					newValue.locationKey = updatedDatapoint->second->locationKey;
					newValue.newValue = static_cast<unsigned long>( updatedDatapoint->second->dataPoint->getValueAsDouble() );
					newValue.type = updatedDatapoint->second->type;
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

	std::map<unsigned long, unsigned long> LibraryVersionMonitor::getAllLibraryVersions()
	{
		TA_Base_Core::ThreadGuard guard(m_messageLock);
		std::map<unsigned long, unsigned long> versionInfoMap;
		for(std::map<unsigned long,LibraryVersionDataPointInfo*>::iterator it = m_libraryVersionDatapoints.begin();
			it != m_libraryVersionDatapoints.end(); ++it)
		{
			unsigned long value = 0;
			try
			{
				value = static_cast<unsigned long>( it->second->dataPoint->getValueAsDouble() );
			}
			catch( ... )
			{
				std::stringstream msg;
				msg << "Could not establish value of datapoint at location ";
				msg << it->second->locationKey;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError, msg.str().c_str());
			}

			// Add the acquired info for this point to the map
			versionInfoMap.insert( std::pair<unsigned long, unsigned long>( it->first, value ) );
		}

		return versionInfoMap;

	}

} // end namespace TA_IRS_App
