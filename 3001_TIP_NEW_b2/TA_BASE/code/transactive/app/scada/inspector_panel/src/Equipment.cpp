/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scada/inspector_panel/src/Equipment.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class provides access to a piece of equipment and its associated data points.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>
#include <string>
#include <limits>

#include "app/scada/Inspector_Panel/src/stdafx.h"

#include "app/scada/inspector_panel/src/InspectorPanelDlg.h"
#include "app/scada/inspector_panel/src/InspectorPanel.h"
#include "app/scada/inspector_panel/src/Equipment.h"

#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataNodeProxy.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/exceptions/src/ScadaProxyException.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	CEquipment::CEquipment(std::string equipmentName) : m_maxDataPoints (1000), 
														m_hasControlPoint (false),
														m_hasInformationPoint (false),
														m_hasMeteredPoint ( false ),
														m_haveAllDataPointConfigured ( false ),
														m_proxyFactory ( 0 )

	{
		FUNCTION_ENTRY ("CEquipment()");

		// get a handle to the ScadaProxyFactory
		m_proxyFactory = &(TA_Base_Bus::ScadaProxyFactory::getInstance());

		TA_ASSERT ( ( 0 != m_proxyFactory ), "SCADA Proxy Factory not created");

		m_proxyFactory->setProxiesToControlMode();

		m_dataPoints.clear();

		m_datapointsKeyIndexMap.clear();
		m_writeableIpoint.clear();
		m_notWriteableIpoint.clear();
		m_meterIpoint.clear();
		m_limitIpoint.clear(); 

        TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(equipmentName);
        TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

        TA_ASSERT(dnEntityData != NULL, "Equipment is not defined as datanode entity");

        m_proxyFactory->createDataNodeProxy(boost::shared_ptr<TA_Base_Core::DataNodeEntityData>(dnEntityData), *this, m_dataNode);
		
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Equipment [%s] is created as observer [%u]", equipmentName.c_str(), this);

        FUNCTION_EXIT;
	}

	CEquipment::~CEquipment()
	{
		FUNCTION_ENTRY ("~CEquipment()");

		// remove handle to ScadaProxyFactory
		m_proxyFactory->setProxiesToMonitorMode();
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		m_proxyFactory = NULL;
		// clear map/vectors
		m_datapointsKeyIndexMap.clear();
		m_writeableIpoint.clear();
		m_notWriteableIpoint.clear();
		m_meterIpoint.clear();
		m_limitIpoint.clear(); 
		clearDataPointObjects();

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "Equipment [%u] is destroyed", this);
        
        FUNCTION_EXIT;
	}

	bool CEquipment::loadDataPoints()
	{
		FUNCTION_ENTRY ("loadDataPoints()");

		bool result = false;

		if (m_dataNode->isConfigValid())
		{
			// Create the data points objects
			result = createDataPointObjects();
		}

		FUNCTION_EXIT;

		return result;
	}

	TA_Base_Bus::DataNodeProxySmartPtr& CEquipment::getDataNode()
	{
		FUNCTION_ENTRY ("getDataNode()");
		
//		TA_ASSERT (m_dataNode != NULL,"Invalid data node (getDataNode).");

		FUNCTION_EXIT;

		return m_dataNode;
	}

	std::vector<TA_Base_Bus::DataPointProxySmartPtr*>& CEquipment::getChildDataPointVector()
	{
		FUNCTION_ENTRY ("getChildDataPointVector()");

        FUNCTION_EXIT;

		return m_dataPoints;
	}

	TA_Base_Bus::DataPointProxySmartPtr& CEquipment::getChildDataPoint(UINT iPoint)
	{
		FUNCTION_ENTRY ("getChildDataPoint()");

		TA_ASSERT (iPoint<m_dataPoints.size(),"Invalid data point (getChildDataPoint).");

        FUNCTION_EXIT;

		return (*m_dataPoints[iPoint]);
	}

	TA_Base_Bus::DataPointProxySmartPtr& CEquipment::getChildDataPointFromEntityKey(unsigned long key)
	{
		FUNCTION_ENTRY ("getChildDataPointFromEntityKey()");

        // Replace linear search
		DatapointsKeyIndexIt iter  = m_datapointsKeyIndexMap.find(key);
		if (iter != m_datapointsKeyIndexMap.end())
		{			
			return (*m_dataPoints[iter->second]);
		} 
	
		// If we got here then not found
		std::ostringstream message;
		message << "Error: DataPoint for entity key ";
		message << key;
		message << " not found";
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"getChildDataPointFromEntityKey() - %s", message.str().c_str());

		FUNCTION_EXIT;

		// For some reason, calling the plain throw was not caught in the 
        // catch(...).  To be safe, we will use the ScadaProxyException for now.
        // Need to get a proper exception for this at some stage.  
		TA_THROW(TA_Base_Core::ScadaProxyException(message.str()));
	}


	UINT CEquipment::getChildDataPointIndexFromEntityKey(unsigned long key)
	{
		FUNCTION_ENTRY ("getChildDataPointIndexFromEntityKey()");
	
        // Replace linear search
		DatapointsKeyIndexIt iter = m_datapointsKeyIndexMap.find(key);
		if (iter != m_datapointsKeyIndexMap.end())
		{			
			return iter->second;
		} 

		// If we got here then not found
		std::ostringstream message;
		message << "Data point for entity key ";
		message << key;
		message << " not found";
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"getChildDataPointIndexFromEntityKey() - %s", message.str().c_str());

		FUNCTION_EXIT;

		// Return an invalid index greater than the maximum
		return m_dataPoints.size();
	}

	unsigned int CEquipment::getChildDataPointCount()
	{
		FUNCTION_ENTRY ("getChildDataPointCount()");
        
        FUNCTION_EXIT;

		return m_dataPoints.size();
	}

	void CEquipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY ("processEntityUpdateEvent()");

		// Tell inspector panel to update the displayed values
		CWinApp *app = AfxGetApp();
		if (app != NULL && app->m_pMainWnd != NULL)
		{
			// need to check if all datapoints are config valid only when not config valid yet
            if (! haveAllDataPointsConfigured())
            {
                checkAllConfigValid();
            }
                
            // if all datapoints are config valid or datanode config valid for first time,
            // notify main window to process the update
            if (haveAllDataPointsConfigured() || (m_dataNode->isConfigValid() && m_dataPoints.size() == 0))
            {
                app->m_pMainWnd->PostMessage(UPDATED_PROXY_MESSAGE, (WPARAM)entityKey, (LPARAM)updateType);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                    "Not all datapoints are config valid or datanode is not config valid, update [%d] of entity [%u] is ingored.", updateType, entityKey);
            }
		}

		FUNCTION_EXIT;
	}

	//********************
	// Protected functions
	//********************

	bool CEquipment::createDataPointObjects()
	{
		FUNCTION_ENTRY ("createDataPointObjects()");

		TA_THREADGUARD(m_indexVectorLock);
        // only load datapoints once, 
        // if already loaded, just send a message to refresh window and return true
        //clearDataPointObjects();
        if (m_dataPoints.size() > 0)
        {
            if (haveAllDataPointsConfigured())
            {
                CWinApp *app = AfxGetApp();
                if (app != NULL && app->m_pMainWnd != NULL)
                {
                    app->m_pMainWnd->PostMessage(UPDATED_PROXY_MESSAGE, (WPARAM)(*m_dataPoints[0])->getEntityKey(), (LPARAM) TA_Base_Bus::ConfigAvailable);
                }
            }

            return true;
        }
			
		std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> > dataPointEntities = m_dataNode->getChildDataPointEntities();
		std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> >::iterator it;

		unsigned int numberDataPoints = 0;

		for (it=dataPointEntities.begin();it!=dataPointEntities.end() && numberDataPoints<m_maxDataPoints;it++)
		{
			try
			{
                boost::shared_ptr<TA_Base_Core::DataPointEntityData> dpEntityData = boost::dynamic_pointer_cast<TA_Base_Core::DataPointEntityData>(*it);
                TA_ASSERT(dpEntityData.get() != NULL, "Child datapoint is not defined as datapoint in database");

				TA_Base_Bus::DataPointProxySmartPtr *pDataPoint = new TA_Base_Bus::DataPointProxySmartPtr();
				m_proxyFactory->createDataPointProxy(dpEntityData, *this, *pDataPoint);

                //for now, we already know datapoint's display order, so insert proxy into vector in order
                std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator itDp;
                for (itDp = m_dataPoints.begin(); itDp != m_dataPoints.end(); ++ itDp)
                {	
                    if (dpEntityData->getDisplayOrder() <= (*(*itDp))->getDisplayOrder())
                    {
                        itDp = m_dataPoints.insert(itDp, pDataPoint);	// must get the return iterator
                        break;
                    }	
                }
                
                if (itDp == m_dataPoints.end())
                {
                    m_dataPoints.push_back(pDataPoint);
                }
                
				numberDataPoints++;
			}
			catch(TA_Base_Core::ScadaProxyException& e)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"createDataPointObjects() - %s", e.what());
			}
			catch(...)
			{
				std::string  message;
				message = "Error: Internal error connecting to DataPoint - ";
				message+= (*it)->getName();
				message+= " from agent ";
				message+= (*it)->getAgentName();

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"createDataPointObjects() - %s", message.c_str());
			}
		}

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
            "all [%u] datapoints created", m_dataPoints.size());
        
		// Initialise the entity key/ datapoints' index map, check datapoints config valid
		initMap();

        if (!haveAllDataPointsConfigured())
        {
		    checkAllConfigValid();

            // need to post a meesge to notify main window to display datapoints when all become config valid for first time
            // because the proxy notification of the last datapoint may be faster than adding the proxy into datapoints map in this function
            //      and the notification was ingored.
            if (haveAllDataPointsConfigured() && numberDataPoints > 0)
            {
                CWinApp *app = AfxGetApp();
                if (app != NULL && app->m_pMainWnd != NULL)
                {
                    app->m_pMainWnd->PostMessage(UPDATED_PROXY_MESSAGE, (WPARAM)(*m_dataPoints[0])->getEntityKey(), (LPARAM) TA_Base_Bus::ConfigAvailable);
                }
            }
        }

		FUNCTION_EXIT;

		return ( 0 < numberDataPoints );
	}

	void CEquipment::clearDataPointObjects()
	{
		FUNCTION_ENTRY ("clearDataPointObjects()");
		
		TA_THREADGUARD(m_indexVectorLock);
		std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator itDp;
		for (itDp = m_dataPoints.begin(); itDp != m_dataPoints.end(); ++ itDp)
		{
			TA_Base_Bus::DataPointProxySmartPtr * pDataPoint = *itDp;
			if (pDataPoint!=NULL)
			{
				delete pDataPoint;
			}
		}
		
        m_dataPoints.clear();

		m_hasControlPoint = false;
		m_hasInformationPoint = false;
		m_hasMeteredPoint = false;
		m_haveAllDataPointConfigured = false;

		FUNCTION_EXIT;
	}

	// Already set the flags when sorting
	bool CEquipment::hasControlPoint()
	{
		return m_hasControlPoint;
	}

	bool CEquipment::hasInformationPoint()
	{
		return m_hasInformationPoint;
	}

	bool CEquipment::hasMeteredPoint()
	{
		return m_hasMeteredPoint;
	}

	bool CEquipment::haveAllDataPointsConfigured()
	{
		return m_haveAllDataPointConfigured;
	}

	// Create a map to replace linear search to find the datapoint index by entity key
	void CEquipment::initMap()
	{
        m_datapointsKeyIndexMap.clear();

		for (UINT iPoint=0;iPoint<m_dataPoints.size();iPoint++)
			m_datapointsKeyIndexMap.insert(DatapointsKeyIndex::value_type((*m_dataPoints[iPoint])->getEntityKey(), iPoint));
	}

	// Check whether all datapoints are config valid
	void CEquipment::checkAllConfigValid()
	{
        if (!m_dataNode->isConfigValid())
        {
            return;
        }
        
        std::vector<TA_Base_Bus::DataPointProxySmartPtr*>::iterator    it;
		bool allDataPointConfigured = true;
		TA_THREADGUARD(m_indexVectorLock);
		for (it=m_dataPoints.begin();it!=m_dataPoints.end();it++)
		{
			if ( false == (*(*it))->isConfigValid() )
			{
				allDataPointConfigured = false;
				break;
			}
		}

        // all created proxies are configured valid, but this doesn't mean all the child datapoints are created.
        if (false == m_haveAllDataPointConfigured && true == allDataPointConfigured)
        {
            std::vector<boost::shared_ptr<TA_Base_Core::IEntityData> > dataPointEntities = m_dataNode->getChildDataPointEntities();
            
            if (m_dataPoints.size() == dataPointEntities.size())
            {
                // all the child datapoints are created and configured valid exactly for first time.
                m_haveAllDataPointConfigured = true;
                
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
                    "all [%u] datapoints config valid", m_dataPoints.size());
                
                initListsHelper();
            }
        }
    }

	// Store different datapoints's index for list display according to their required config
	// Vectors will only be available after sorting (i.e. all config valid)
	void CEquipment::initListsHelper()
	{
		TA_THREADGUARD(m_indexVectorLock);

        m_writeableIpoint.clear();
		m_notWriteableIpoint.clear();
		m_meterIpoint.clear();
		m_limitIpoint.clear(); 
		
		for (unsigned long iPoint=0;iPoint<m_dataPoints.size();iPoint++)
		{
			if( (*m_dataPoints[iPoint])->getDisplayOrder() >= 0 )
			{
				if( false == (*m_dataPoints[iPoint])->isWriteable() )
				{
					// Information/Override lists/Inhibit Lists
					m_notWriteableIpoint.push_back(iPoint);

					// Meter List
                    if( (*m_dataPoints[iPoint])->getType() == TA_Base_Bus::DPT_METERED )
					{
						m_meterIpoint.push_back(iPoint);
                        m_hasMeteredPoint = true;
                    }

					// Limit List
					if ( ( false == (*m_dataPoints[iPoint])->isBoolean() ) &&		// if not a boolean dp
					     ( false == (*m_dataPoints[iPoint])->isText() ) &&		// if not a text dp
					     ( false == (*m_dataPoints[iPoint])->isDerived() ) )		// if not a derived dp
					{
						m_limitIpoint.push_back(iPoint);
					}

                    m_hasInformationPoint = true;
				}
				else
				{
					m_writeableIpoint.push_back(iPoint); // Control lists/ Inhibit Lists
                    m_hasControlPoint = true;
				}
			}
		}
	}

	// Accessors to get the index vectors

	std::vector<unsigned long>& CEquipment::getWriteableIndex()
	{
		FUNCTION_ENTRY ("getWriteableIndex()");
		FUNCTION_EXIT;

		return m_writeableIpoint;
	}

	std::vector<unsigned long>&	CEquipment::getNotWriteableIndex()
	{
		FUNCTION_ENTRY ("getNotWriteableIndex()");
		FUNCTION_EXIT;

		return m_notWriteableIpoint;
	}

	std::vector<unsigned long>& CEquipment::getLimitIndex()
	{
		FUNCTION_ENTRY ("getLimitIndex()");
		FUNCTION_EXIT;

		return m_limitIpoint;
	}

	std::vector<unsigned long>& CEquipment::getMeterIndex()
	{
		FUNCTION_ENTRY ("getMeterIndex()");
		FUNCTION_EXIT;

		return m_meterIpoint;
	}
}


