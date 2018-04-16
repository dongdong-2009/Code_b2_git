/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/scada/common_library/src/MmsScadaUtility.cpp $
  * @author:  Andrew Del Carlo
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2010/01/25 18:07:02 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides utility functions for logging Audit Events.
  *
  */

//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786)
#endif // defined _MSC_VER

#include "bus/scada/common_library/src/MmsScadaUtility.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeAccessFactory.h"
#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/naming/src/NamedObject.h"
#include "core/exceptions/src/ScadaCommonException.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"

#include "bus/scada/common_library/src/CobraCppConvertUtilities.h"
#include "bus/scada/datanode_library/src/DataNodeTypes.h"
#include "bus/maintenance_management/mmsCorbaDef/idl/src/IMmsGlobalInhibitCorbaDef.h"
#include "bus/generic_agent/src/IEntity.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"


#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/Thread.h"

#include "core/data_access_interface/entity_access/src/EntityData.h"	// zn++ reduce DB access

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Bus
{
	MmsScadaUtility::MmsScadaUtility()
    :
    m_localAgentName (""),
    m_localEntityMap (NULL),
    m_useProxies (true),
    m_globalInhibitDpExist(true),
    m_globalInhibitProxyCreated(false),
    m_proxyFactory(NULL)
	{
        // listen for entity updates so we can refresh the cache maps
        // TD11885 - Online update only allows a single registration to all entity updates
        // TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ENTITY, *this);
        // Mintao++(TD13650)
        initScadaProxyFactory();
		// Mintao++(TD13650)
	}

	MmsScadaUtility::MmsScadaUtility(std::string agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap, bool useProxies)
    :
    m_localAgentName (""),
    m_localEntityMap (NULL),
    m_useProxies (useProxies),
    m_globalInhibitDpExist(true),
    m_globalInhibitProxyCreated(false),
    m_proxyFactory(NULL)
	{
        // listen for entity updates so we can refresh the cache maps

        // TD11885 - Online update only allows a single registration to all entity updates
        // TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::ENTITY, *this);
        // Mintao++(TD13650)
        initScadaProxyFactory();
		registerLocalEntities(agentName, entityMap);
	}

    void MmsScadaUtility::initScadaProxyFactory()
    {
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());
		TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");
		m_proxyFactory->setProxiesToControlMode();
    }

	MmsScadaUtility::~MmsScadaUtility()
	{
		{
			TA_THREADGUARD(m_subsystemNodesLock);
			freeSubSystemNodes();
		}

		// remove handle to ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_proxyFactory = NULL;
		// Mintao++ (TD13650)
	}

	void MmsScadaUtility::freeSubSystemNodes()
	{
		std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it;
		for(it=m_subsystemNodes.begin(); it!=m_subsystemNodes.end(); it++)
		{
			DataNodeProxySmartPtr * dn = it->second;
			if(dn!=NULL)
			{
				delete dn;
			}
		}
			
		m_subsystemNodes.clear();

	}
	
    void MmsScadaUtility::processOnlineUpdateOfMaintenanceData()
    {
        // synchronize access to the map
        TA_THREADGUARD(m_mapLock);

        // clear all the maps, so next time the functions are called, we will refresh
        // data from the database
        //m_reportTypeMap.clear();		// CL-21212 this buffer is useless
        //m_reportPeriodMap.clear();	// CL-21212 this buffer is useless
        m_physicalSubsystemEntitiesMap.clear();
    }

    void MmsScadaUtility::registerLocalEntities(const std::string& agentName, const std::map<unsigned long, TA_Base_Bus::IEntity*>* entityMap)
	{
		m_localAgentName = agentName;
		m_localEntityMap = entityMap;

        //this caused a fatal error for local proxy creation
		//ScadaProxyFactory::getInstance().registerLocalEntities(agentName, entityMap);
	}

	bool MmsScadaUtility::isDataPointMmsInhibited(unsigned long dataPointKey)
	{	
        // First check Global Inhibit
		if (isMmsGlobalInhibited())
		{
			return true;
		}

        std::string agentname;
        unsigned long dataNodeKey = 0;
        try
        {
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(dataPointKey);
            if (entityData.get() != NULL)
            {
                agentname = entityData->getAgentName();
                dataNodeKey = entityData->getParent();
            }
            else
            {
                // get entity data from database
                std::auto_ptr<TA_Base_Core::IEntityData> entitydata(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointKey, true));
                agentname = entitydata->getAgentName();
                dataNodeKey = entitydata->getParent();
            }
        }
        catch (...) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get the datapoint agent name[%d] from the database",dataPointKey );            
            return false;
        }

		// Then DataPoint Inhibit
		if (isMmsDpInhibited(dataPointKey, agentname))
		{
			return true;
		}
        // Do we need to check subsystem inhibition for each dp?
        // How to cater for the case that parent DN was MMS inhibited while child DP is not.
        // Do we consider that this child DP is also MMS inhibited?
		// Then Subsystem Inhibit
		return isMmsSubsystemInhibited(dataNodeKey);
	}


    bool MmsScadaUtility::isDataPointMmsInhibited(unsigned long dataPointKey, TA_Base_Core::DataPointEntityDataPtr dataPointEntityAccess)
    {
        // First check Global Inhibit
		if (isMmsGlobalInhibited())
		{
			return true;
		}

		// Then DataPoint Inhibit
		if (isMmsDpInhibited(dataPointKey, dataPointEntityAccess->getAgentName()))
		{
			return true;
		}
		
		// Then Subsystem Inhibit
		return isMmsSubsystemInhibited(dataPointEntityAccess->getParent());
	}

	bool MmsScadaUtility::isDataPointMmsInhibited(const std::string& dataPointName)
	{
        unsigned long dataPointKey = 0;
        try
        {
            // first try to get the entity from DataNodeAccessFactory cache
            TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(dataPointName);
            if (entityData.get() != NULL)
            {
                dataPointKey = entityData->getKey();
            }
            else
            {
                // get entity data from database
                std::auto_ptr<TA_Base_Core::IEntityData> entitydata(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointName, true));
                dataPointKey = entitydata->getKey();
            }
        }
        catch (...) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get the datapoint key[%s] from the database",dataPointName.c_str() );            
            return false;
        }
    	return isDataPointMmsInhibited(dataPointKey);
	}

	bool MmsScadaUtility::isMmsGlobalInhibited()
	{
		bool inhibit = false;
       
        TA_THREADGUARD(m_globalInhibitLock);

        if (false == m_globalInhibitProxyCreated &&
            true == m_globalInhibitDpExist)
        {
            TA_Base_Core::IData * dbdata = NULL;
        
            try
            {
                // get the agent name
                std::string agentName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
                dbdata = TA_Base_Core::EntityAccessFactory::getInstance().getParameter (agentName, TA_Base_Core::DataPointEntityData::GLOBAL_MMS_INHIBITED_DATAPOINT );

                // if there is one
                std::string globalInhibitDpName("");
                if ( 0 != dbdata )
                {
                    // if there is data returned
                    if ( 0 < dbdata->getNumRows() )
                    {
                        globalInhibitDpName = dbdata->getStringData ( 0, "VALUE" );
                    }
                    // else do nothing
                }

                if (true == globalInhibitDpName.empty())
                {
                    m_globalInhibitDpExist = false;
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Global Inhibit Datapoint Name not configured for this agent.");
                }
                else
                {
                    //get the global entitykey
					//cl-20961
                    //std::auto_ptr<TA_Base_Core::IEntityData> entitydata( TA_Base_Core::EntityAccessFactory::getInstance().getEntity(globalInhibitDpName) );
                    //unsigned long globaldpkey = entitydata->getKey();
					TA_Base_Core::IEntityData* entitydata = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(globalInhibitDpName);
					TA_Base_Core::DataPointEntityData* dpentitydata = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entitydata);
                    try
                    {
						//cl-20961
                        //m_proxyFactory->createDataPointProxy(globaldpkey, *this, m_globalInhibitProxy);
						m_proxyFactory->createDataPointProxy(boost::shared_ptr<TA_Base_Core::DataPointEntityData>(dpentitydata), *this, m_globalInhibitProxy);
                        m_globalInhibitProxyCreated = true;

                        unsigned int retries = 0;

                        // Give this dp 5 seconds to get configurstion
                        // Will default to false if this is not enough time
                        while (false == m_globalInhibitProxy->isConfigValid() && retries < 50)
                        {
                            TA_Base_Core::Thread::sleep(100);
                            ++retries;
                        }
                    }
                    catch(TA_Base_Core::ScadaProxyException& e)
                    {
                        std::string text = "isMmsGlobalInhibited() - " + std::string(e.what());
                        LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text.c_str());
                    }
                }
            }
            catch ( TA_Base_Core::DatabaseException & e)
            {
                m_globalInhibitDpExist = false;

                std::string text = "isMmsGlobalInhibited() - " + std::string(e.what());
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text.c_str());
            }
            catch ( TA_Base_Core::DataException & e)
            {
                m_globalInhibitDpExist = false;

                std::string text = "isMmsGlobalInhibited() - " + std::string(e.what());
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text.c_str());
            }
            catch(...)
            {
                m_globalInhibitDpExist = false;
                TA_THROW(TA_Base_Core::ScadaCommonException("Caught unknown exception in isMmsGlobalInhibited()"));
            }

            delete dbdata;
            dbdata = 0;
		}

        if (true == m_globalInhibitProxyCreated)
        {
            try
            {
                if (true == m_globalInhibitProxy->isConfigValid())
                {
                    inhibit = m_globalInhibitProxy->getValueAsBool();
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Mintao++(TD13650): Global Inhibit Datapoint status: %d", inhibit);
                }
                else
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Global Inhibit Datapoint configuration not loaded.");
                }
            }
            catch(TA_Base_Core::ScadaProxyException& e)
            {
                std::string text = "isMmsGlobalInhibited() - " + std::string(e.what());
                LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text.c_str());
            }
        }
		
		return inhibit;
	}

    
	void MmsScadaUtility::processEntityUpdateEvent(unsigned long entityKey, ScadaEntityUpdateType updateType) 
    {
        /*LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Mintao++(TD13650): The entity key updated is %ul", entityKey);
        if (updateType == TA_Base_Bus::ValueStateUpdate)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Mintao++(TD13650): The type of update is ValueStateUpdate");
        }*/
    }


	bool MmsScadaUtility::isMmsDpInhibited(unsigned long dataPointKey, const std::string& agentName)
	{
		bool inhibit = false;

		try
		{
			if (m_localEntityMap == NULL || m_localAgentName.compare(agentName) != 0 )
			{
                // should not use named object instead should use local proxies, since this dp is likely to hold by the agent itself.
             
                /*TA_Base_Bus::DataPointAgentInterface dataPointNamedObject;
                dataPointNamedObject.setCorbaName(agentName , agentName + TA_Base_Bus::DP_ACCESS_INTERFACE);
				try{
					inhibit = dataPointNamedObject->getIsMMSAlarmInhibited(dataPointKey) == true;
				}//TD18706,jianghp, handle the operation mode
				catch (TA_Base_Core::OperationModeException &) 
				{
					dataPointNamedObject.clear();
					inhibit = dataPointNamedObject->getIsMMSAlarmInhibited(dataPointKey) == true;
				}*/
                
                TA_Base_Bus::DataPointProxySmartPtr pDataPoint;
				m_proxyFactory->createDataPointProxy(dataPointKey, *this, pDataPoint);
                try
                {
                    inhibit = pDataPoint->isMmsAlarmInhibited();
                }
                catch (TA_Base_Core::ScadaProxyException& e)
				{
					// Not always available
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Failed to get isMmsDpInhibited status for entity: %lu, due to ScadaProxyException: %s", dataPointKey, e.what());
				}

				return inhibit;
			}
			else
			{
				std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator entityIt = 
					m_localEntityMap->find(dataPointKey);

				if (entityIt != m_localEntityMap->end())
				{
					TA_Base_Bus::DataPoint* dataPointLocalObject = NULL;
					dataPointLocalObject = 
						dynamic_cast<TA_Base_Bus::DataPoint*>((*entityIt).second);

					if (dataPointLocalObject != NULL)
					{
						inhibit = dataPointLocalObject->getMMSAlarmInhibitedState() == true;
						return inhibit;
					}
					else
					{
						LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
							"isMmsDpInhibited() - No local Datapoint Entity");

						return false;
					}
				}
				else
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"isMmsDpInhibited() - No local Datapoint Entity");

					return false;
				}
			}
		}
		catch (TA_Base_Core::OperationModeException& e)
		{
			std::string text = "isMmsDpInhibited() - " + std::string(e.reason);
 			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", text);

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"isMmsDpInhibited() - Can not determine Datapoint MMS Inhibit state");

			return false;
		}
        catch (TA_Base_Core::BadParameterException& e)
        {
			std::string text = "isMmsDpInhibited() - " + std::string(e.reason);
 			LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", text);

            return false;
        }
		catch ( const TA_Base_Core::ObjectResolutionException & ex )
		{
			std::string text = "isMmsDpInhibited() - " + std::string(ex.what());
 			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", text);

            return false;
		}
        catch (TA_Base_Core::NotAvailableException& e)
		{
			std::string text = "isMmsDpInhibited() - " + std::string(e.reason);
 			LOG_EXCEPTION_CATCH(SourceInfo, "NotAvailableException", text);

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"isMmsDpInhibited() - Can not determine Datapoint MMS Inhibit state");

			return false;
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "isMmsDpInhibited() - Exception thrown while calling a CorbaFunction: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", exceptionMsg);

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"isMmsDpInhibited() - Can not determine Datapoint MMS Inhibit state");

			return false;
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "isMmsDpInhibited() - Unknown Exception");

			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"isMmsDpInhibited() - Can not determine Datapoint MMS Inhibit state");

			return false;
		}
	}

    bool MmsScadaUtility::isMmsSubsystemInhibited(unsigned long subsystemkey)
	{
		bool inhibit = false;
        /*TA_Base_Core::IEntityData* entitydata = NULL;
        unsigned long subsystemkey = 0;
        try
        {
            entitydata = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataPointKey);
            unsigned long dataNodeKey = entitydata->getParent();
            delete entitydata;
            if (dataNodeKey != 0)
            {
                entitydata = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(dataNodeKey);
                subsystemkey = entitydata->getParent();
            }
        }
        catch (...) 
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to get the datapoint key[%d] from the database",dataPointKey );            
            delete entitydata;
            return false;
        }*/


		if(subsystemkey != 0)
		{
			TA_THREADGUARD(m_subsystemNodesLock);

			std::map<unsigned long, DataNodeProxySmartPtr*>::iterator it =
			m_subsystemNodes.find(subsystemkey);

			if (it == m_subsystemNodes.end())
			{
				try
				{
					DataNodeProxySmartPtr *ssProxy = new DataNodeProxySmartPtr();
					ScadaProxyFactory::getInstance().createDataNodeProxy(subsystemkey, *this, *ssProxy);

					m_subsystemNodes[subsystemkey] = ssProxy;

					// Give this dp 5 seconds to get configuration
					// Will default to false if this is not enough time
					unsigned int retries = 0;

					while (false == (*ssProxy)->isConfigValid() && retries < 50)
					{
						TA_Base_Core::Thread::sleep(100);
						++retries;
					}

					if (true == (*ssProxy)->isConfigValid())
					{
						 inhibit = (*ssProxy)->getInhibitMms();
						 return inhibit;
					}
				}
			   catch (TA_Base_Core::ScadaProxyException& e)
			   {
					std::string text = "isMmsSubsystemInhibited() - " + std::string(e.what());
					LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text);
			   }
			   catch (...)
			   {
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "isMmsSubsystemInhibited() - Unknown Exception");
			   }
			}
			else
			{
				DataNodeProxySmartPtr *ssProxy = it->second;
				try
				{
					if (true == (*ssProxy)->isConfigValid())
					{
						inhibit = (*ssProxy)->getInhibitMms();
						return inhibit;
					}
				}
			   catch (TA_Base_Core::ScadaProxyException& e)
			   {
					std::string text = "isMmsSubsystemInhibited() - " + std::string(e.what());
					LOG_EXCEPTION_CATCH(SourceInfo, "ScadaProxyException", text);
			   }
			   catch (...)
			   {
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "isMmsSubsystemInhibited() - Unknown Exception");
			   }
			}
		}
		return false;
 }


	std::vector<ScadaUtilities::MaintenanceData> MmsScadaUtility::getMaintenanceData(unsigned long subsystemKey,
		const std::vector<unsigned int>& periods)
	{

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getMaintenanceData for subsystem: %lu", subsystemKey);

		TA_Base_Core::IEntityDataList subsystemDpEntities;

        {
            // synchronize access to the map
            TA_THREADGUARD(m_mapLock);
            // try to find the name list in our cache map first
            bool entityListCached = false;
            bool subsystemMapExists = false;
            std::string datapointEntityType = TA_Base_Core::DataPointEntityData::getStaticType();
            std::map<unsigned long, std::map<std::string, TA_Base_Core::IEntityDataList> >::iterator subsystemMap = 
                                        m_physicalSubsystemEntitiesMap.find(subsystemKey);

            if (subsystemMap != m_physicalSubsystemEntitiesMap.end())
            {
                subsystemMapExists = true;

                std::map<std::string, TA_Base_Core::IEntityDataList>::iterator subsystemIt = 
                                                subsystemMap->second.find(datapointEntityType);
                if (subsystemIt != subsystemMap->second.end())
                {
                    subsystemDpEntities = subsystemIt->second;
                    entityListCached = true;
                }
            }

            if (!entityListCached)
            {
		        try
		        {
			        subsystemDpEntities =
				        TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeInPhysicalSubsystem(
						        TA_Base_Core::DataPointEntityData::getStaticType(), subsystemKey);

                    // add the value to the map of map
                    if (subsystemMapExists)
                    {
                        subsystemMap->second[datapointEntityType] = subsystemDpEntities;
                    }
                    else
                    {
                        std::map<std::string, TA_Base_Core::IEntityDataList> tempSubSystemMap;
                        tempSubSystemMap[datapointEntityType] = subsystemDpEntities;
                        m_physicalSubsystemEntitiesMap[subsystemKey] = tempSubSystemMap;
                    }

		        }
		        catch (TA_Base_Core::DatabaseException& e)
		        {
			        std::string text = "getMaintenanceData() - " + std::string(e.what());
 			        LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

			        TA_THROW(TA_Base_Core::ScadaCommonException("Can not find find DataPoint Entities in subsystem"));
		        }
		        catch (TA_Base_Core::DataException& e)
		        {
			        std::string text = "getMaintenanceData() - " + std::string(e.what());
 			        LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

			        TA_THROW(TA_Base_Core::ScadaCommonException("Can not find DataPoint Entities in subsystem"));
		        }
            }
        }

		// We have the list of DataPoints for the Physical Subsystem, now find matching DataPoints

		TA_Base_Core::IEntityDataList::iterator dpIt;
		std::vector<ScadaUtilities::MaintenanceData> dataList;

		for (dpIt = subsystemDpEntities.begin();
			 dpIt != subsystemDpEntities.end();
			 dpIt++)
		{
			
			// zn++ CL-21212 reduce DB access
			// First check the Report Type
			//std::string typeString = getMmsReportType((*dpIt)->getName());
			std::string typeString("");

			// Cast to an entity data and assign the value
			TA_Base_Core::EntityData* entity = dynamic_cast< TA_Base_Core::EntityData* >(*dpIt);

			if (NULL != entity)
			{
				typeString = entity->getHelper()->getParameter("MMSReportType");
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Can not get param for pt: %s", (*dpIt)->getName().c_str());
				continue;
			}
			// ++zn

			if (typeString == "CPT" || typeString == "VAL")
			{
				// zn++ CL-21212 reduce DB access
				// We have a winner - now check the period
				//int period = getMmsReportPeriod((*dpIt)->getName());
				int period = atoi(entity->getHelper()->getParameter("MMSReportPeriod").c_str());
				//++zn

				if (period != -1)
				{
					std::vector<unsigned int>::const_iterator periodIt;
					
					for (periodIt = periods.begin();
						 periodIt != periods.end();
						 periodIt++)
					{
						if ((*periodIt) == (unsigned int)period)
						{
							// We've found one - now let's populate the structure
							// and get out of here

                            try
                            {
							    dataList.push_back(getMmsMaintData((*dpIt), typeString));
                            }
		                    catch (TA_Base_Core::OperationModeException& )
		                    {
                                //do nothing, since it is already logged before we catched here
                                continue;
		                    }
		                    catch( const CORBA::Exception& )
		                    {
                                //do nothing, since it is already logged before we catched here
                                continue;
		                    }
                            catch (TA_Base_Core::ScadaCommonException& )
                            {
                                //do nothing, since it is already logged before we catched here
                                continue;
                            }
		                    catch (...)
		                    {
                                //do nothing, since it is already logged before we catched here
                                continue;
		                    }
						}
					}
				}
			}
		}

		return dataList;
	}

// CL-21212 reduce DB access
// 	std::string MmsScadaUtility::getMmsReportType(const std::string& objectName)
// 	{
//         // synchronize access to the map
//         TA_THREADGUARD(m_mapLock);
// 
//         // try to find in our cache map first
//         std::map<std::string, std::string>::iterator reportIt = m_reportTypeMap.find(objectName);
//         if (reportIt != m_reportTypeMap.end())
//         {
//             return reportIt->second;
//         }
// 
// 
// 		TA_Base_Core::IData* typePar = NULL;
// 
// 		try
// 		{
// 			typePar = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
// 					objectName, TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_REPORT_TYPE );
// 		}
// 		catch (TA_Base_Core::DatabaseException& e)
// 		{
// 			std::string text = "getMmsReportType() - " + std::string(e.what());
//  			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
// 
// 			return "";
// 		}
// 		catch (TA_Base_Core::DataException& e)
// 		{
// 			std::string text = "getMmsReportType() - " + std::string(e.what());
//  			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
// 
// 			return "";
// 		}
// 
// 
// 		std::string typeString;
// 
// 		if (typePar != NULL)
// 		{
// 			if (typePar->getNumRows() == 1)
// 			{
// 				try
// 				{
// 					typeString = typePar->getStringData(0, "VALUE");
// 				}
// 				catch (TA_Base_Core::DataException& e)
// 				{
// 					std::string text = "getMmsReportType() - " + std::string(e.what());
//  					LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
// 
// 					typeString = "";
// 				}
// 			}
// 			else
// 			{
// 				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
// 					 "getMmsReportType() - MMSReportType Entity Parameter missing for %s",
// 					 objectName.c_str());
// 			}
// 
//     		m_reportTypeMap[objectName] = typeString; // assign to our cache map
// 
// 			delete typePar;
// 		}
// 		else
// 		{
// 			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
// 				 "getMmsReportType() - MMSReportType Entity Parameter can not be retrieved for %s",
// 				 objectName.c_str());
// 		}
// 
// 		return typeString;
// 	}

// CL-21212 reduce DB access
// 	int MmsScadaUtility::getMmsReportPeriod(const std::string& objectName)
// 	{
//         // synchronize access to the map
//         TA_THREADGUARD(m_mapLock);
// 
//         // try to find in our cache map first
//         std::map<std::string, int>::iterator reportIt = m_reportPeriodMap.find(objectName);
//         if (reportIt != m_reportPeriodMap.end())
//         {
//             return reportIt->second;
//         }
// 
// 		TA_Base_Core::IData* periodPar = NULL;
// 
// 		try
// 		{
// 			periodPar = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(
// 					objectName, TA_Base_Core::DataPointEntityData::DATAPOINT_MMS_REPORT_PERIOD );
// 		}
// 		catch (TA_Base_Core::DatabaseException& e)
// 		{
// 			std::string text = "getMmsReportPeriod() - " + std::string(e.what());
//  			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);
// 
// 			return 0;
// 		}
// 		catch (TA_Base_Core::DataException& e)
// 		{
// 			std::string text = "getMmsReportPeriod() - " + std::string(e.what());
//  			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
// 
// 			return 0;
// 		}
// 
// 		int period = -1;
// 
// 		if (periodPar != NULL)
// 		{
// 			if (periodPar->getNumRows() == 1)
// 			{
// 				//int found = false;
// 
// 				try
// 				{
// 					period = periodPar->getIntegerData(0, "VALUE");
// 				}
// 				catch (TA_Base_Core::DataException& e)
// 				{
// 					std::string text = "getMmsReportPeriod() - " + std::string(e.what());
//  					LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);
// 
// 					period = 0;
// 				}
// 			}
// 			else
// 			{
// 				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
// 					 "getMmsReportPeriod() - MMSReportPeriod Entity Parameter can not be retrieved for %s",
// 					 objectName.c_str());
// 
// 				period = 0;
// 			}
// 
//             m_reportPeriodMap[objectName] = period; // assign to our cache map
// 
// 			delete periodPar;
// 		}
// 
// 		return period;
// 	}

	ScadaUtilities::MaintenanceData MmsScadaUtility::getMmsMaintData
		(TA_Base_Core::IEntityData* entitydata, 
		 const std::string& reportType)
	{

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "getMmsMaintData for pt: %s", entitydata->getName().c_str());

		ScadaUtilities::MaintenanceData data;

		if (reportType == "CPT")
		{
			data.type = ScadaUtilities::MaintenanceData::CPT;
		}
		else
		{
			data.type = ScadaUtilities::MaintenanceData::VAL;
		}

 		data.assetName = "Error Unkown";
        try

        {
            if (entitydata->getType() == TA_Base_Core::DataNodeEntityData::getStaticType())
            {
                TA_Base_Core::DataNodeEntityData* dndata = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entitydata);
                if (dndata != NULL)
                {
                    data.assetName = dndata->getAssetName();
                }
            }
            else if (entitydata->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
            {
                // first try to get the entity from DataNodeAccessFactory cache
                TA_Base_Core::IEntityDataPtr entityData = TA_Base_Core::DataNodeAccessFactory::getInstance()->getEntityData(entitydata->getParent());
                if (entityData.get() != NULL)
                {
                    if(entityData->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) 
                    {
                        TA_Base_Core::DataNodeEntityData* dndata = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData.get());
                        data.assetName = dndata->getAssetName();
                    }
                }
                else
                {
                    // get configuration data from database
                    std::auto_ptr<TA_Base_Core::IEntityData> dn( TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entitydata->getParent()) );
                    if(dn->getType() == TA_Base_Core::DataNodeEntityData::getStaticType()) 
                    {
                        TA_Base_Core::DataNodeEntityData* dndata = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(dn.get());
                        data.assetName = dndata->getAssetName();
                    }
                }
            }
        }
        catch (...)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Error occur while trying to get asset name of %d", entitydata->getKey());
        }
		
		std::string objectName = entitydata->getName();
		std::string::size_type pos = objectName.find_last_of ( ".", objectName.length() );
		if ( pos > 0 )
		{
			data.pointName = objectName.substr ( pos+1, objectName.length() );
		}
		else
		{
			data.pointName = "";
		}

        //get the datapoint description
        data.description = entitydata->getDescription();

		// Next step is to get hold of the DataPoint named object

		try
		{
            boost::shared_ptr<TA_Base_Bus::DataPointValue> dpState;
			data.readFailed = true;

			if (m_localEntityMap == NULL || m_localAgentName != entitydata->getAgentName())
			{
                TA_Base_Bus::DataPointAgentInterface dataPointNamedObject;
                std::string angentcorbaname = entitydata->getAgentName() + TA_Base_Bus::DP_ACCESS_INTERFACE;
                dataPointNamedObject.setCorbaName(entitydata->getAgentName() , angentcorbaname);
		
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				    "get the raw data value");

                dpState = boost::shared_ptr<TA_Base_Bus::DataPointValue>(new TA_Base_Bus::DataPointValue);

				TA_Base_Bus::ScadaCorbaTypes::UTag* rawdpState = NULL;

                CORBA_CALL_RETURN( rawdpState, dataPointNamedObject, getFieldValue, ( entitydata->getKey() ) );//limin
                
                convertDataPointValue(*rawdpState, *dpState);
                delete rawdpState;
						    
			}
			else
			{
				std::map<unsigned long, TA_Base_Bus::IEntity*>::const_iterator entityIt = 
					m_localEntityMap->find(entitydata->getKey());

				if (entityIt != m_localEntityMap->end())
				{
					TA_Base_Bus::DataPoint* dataPointLocalObject = NULL;
					dataPointLocalObject = 
						dynamic_cast<TA_Base_Bus::DataPoint*>((*entityIt).second);

					if (dataPointLocalObject != NULL)
                    {						
						
						dpState	= dataPointLocalObject->getFieldValue();		// TD18924 - use engineering value instead of raw value
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				            "get the raw data value");						
					}
				}
            }
            //now get the value
			if (dpState .get()!= NULL)
			{
				switch ( dpState->dpType )
				{
				case TA_Base_Bus::DPT_INT16_TYPE:
				case TA_Base_Bus::DPT_INT32_TYPE:
				case TA_Base_Bus::DPT_INT16S_TYPE:
				case TA_Base_Bus::DPT_INT32S_TYPE:
				case TA_Base_Bus::DPT_UINT16_TYPE:
				case TA_Base_Bus::DPT_UINT32_TYPE:
				case TA_Base_Bus::DPT_IEEE32_TYPE:
					data.value = dpState->floatValue;
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "The raw data float value is %f" , data.value);
					data.readFailed = false;
					break;
				case TA_Base_Bus::DPT_ENUM_TYPE:
					data.value = dpState->enumValue;
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "The raw data float value is %f" , data.value);
					data.readFailed = false;
                    break;

				case TA_Base_Bus::DPT_BOOL_TYPE:
					if (dpState->booleanValue)
					{
						data.value = 1;
					}
					else
					{
						data.value = 0;
					}
					data.readFailed = false;
					break;

				case TA_Base_Bus::DPT_TEXT_TYPE:
				default:
					data.value = 0;
					data.readFailed = true;
					break;
				}

			}
        }
		catch (TA_Base_Core::OperationModeException& e)
		{
			std::string text = "getMmsMaintData() - " + std::string(e.reason);
 			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", text);

			data.readFailed = true;		
		}
		catch( const CORBA::Exception& cex )
		{
			std::string exceptionMsg( "getMmsMaintData() - Exception thrown while calling a CorbaFunction: " );
			exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", exceptionMsg);

			data.readFailed = true;			
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "getMmsMaintData() - Unknown Exception");

			TA_THROW(TA_Base_Core::ScadaCommonException("Can not read MMS Maint Data"));
		}
		
		return data;
	}

}




