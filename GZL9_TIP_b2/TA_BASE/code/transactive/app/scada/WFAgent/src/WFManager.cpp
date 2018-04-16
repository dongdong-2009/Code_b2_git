/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/3002_TIP/3002/transactive/app/scada/WFAgent/src/WFManager.cpp $
  * @author:	HoaVu
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2009/10/01 16:36:38 $
  * Last modified by:	$Author: grace.koh $
  *
  * Description:
  *
  *	The WfManager object is responsible for managing WfUnits' running mode 
  * And processing WfUnit' poll/write data
  *
  */

#include "app/scada/WFAgent/src/WFManager.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/data_access_interface/entity_access/src/RTUEntityData.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
    WfManager::WfManager()
        :
        m_agentAssetName(""),
        m_stationName(""),
        m_pWfUnit(NULL)
    {
		FUNCTION_ENTRY("WfManager::WfManager");
		m_qualityStatus.clear();
		FUNCTION_EXIT("WfManager::WfManager");
   }
    

    WfManager::~WfManager()
    {
		FUNCTION_ENTRY("WfManager::~WfManager");
		FUNCTION_EXIT("WfManager::~WfManager");
   }
        

    TA_Base_Bus::IEntity* WfManager::createWfUnitEntity ( TA_Base_Core::IEntityDataPtr EntityData )
    {
		FUNCTION_ENTRY("createWfUnitEntity");
       // cast the EntityData to RTUEntityData
        TA_Base_Core::RTUEntityDataPtr wfEntityData = boost::dynamic_pointer_cast<TA_Base_Core::RTUEntityData>(EntityData);
        
        TA_ASSERT( NULL != wfEntityData.get(), "wfEntityData is NULL in createWfUnitEntity" );
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating WfUnit entity for [%s]...", wfEntityData->getName().c_str());
        // create WfUnit entity
		WfUnit* pWfUnit;
		if (m_pWfUnit == NULL)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start to Create a new WfUnit");
			pWfUnit = new WfUnit(*this, wfEntityData);

			TA_Base_Bus::IEntity* pEntity = pWfUnit;
			pEntity->start();

			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Create WfUnit[%p] for [%s]", pWfUnit,pWfUnit->getName().c_str());
			m_pWfUnit = pWfUnit;
			//TD10586
			m_wfOutputWorker.addWfUnit(*pWfUnit);
			//jinmin++
			m_wfPollingWorker.addWfUnit(*pWfUnit);
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Add WfUnit [%s] finished", pWfUnit->getName().c_str());
		}
		else
		{
			m_pWfUnit->addLink(wfEntityData);
			pWfUnit = m_pWfUnit;
		}

		FUNCTION_EXIT("createWfUnitEntity");

        return pWfUnit;
    }

    void WfManager::setAgentAssetName(std::string& assetName)
    {
		FUNCTION_ENTRY("setAgentAssetName");
        m_agentAssetName = assetName;
		FUNCTION_EXIT("setAgentAssetName");
   }

    std::string& WfManager::getAgentAssetName()
    {
		FUNCTION_ENTRY("getAgentAssetName");
		FUNCTION_EXIT("getAgentAssetName");
       return m_agentAssetName;
    }

    void WfManager::setStationName(std::string& stationName)
    {
		FUNCTION_ENTRY("setStationName");
		m_stationName = stationName;
		FUNCTION_EXIT("setStationName");
    }

    std::string& WfManager::getStationName()
    {
		FUNCTION_ENTRY("getStationName");
		FUNCTION_EXIT("getStationName");
       return m_stationName;
    }
    
    bool WfManager::updateWfUnitConfiguration(const TA_Base_Core::ConfigUpdateDetails & updateEvent)
    {
		FUNCTION_ENTRY("updateWfUnitConfiguration");
        if (updateEvent.getKey() == m_pWfUnit->getPKey())
        {
			if (m_pWfUnit != NULL)
			{
				m_pWfUnit->updateWfUnitConfiguration(updateEvent);
			}

            // return true to indicate no need to go further processing
            return true;
        }

		FUNCTION_EXIT("updateWfUnitConfiguration");
       // not WfUnit configuration update
        return false;
    }

    bool WfManager::addDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("addDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;
        bool isAdded = false;

        //Add output data point to RTU to be processed by StationSystemOutputWirter
        if (dp->isWriteable())
        {
			if (m_pWfUnit != NULL)
			{
				// output data points are in both rtus, so ingore the return value
				m_pWfUnit->addDataPoint(dp);
			}

			FUNCTION_EXIT("addDataPoint");
            return true;
        }
        else
        {
			if (m_pWfUnit != NULL)
			{
			   isAdded = m_pWfUnit->addDataPoint(dp);
			}
			FUNCTION_EXIT("addDataPoint");
            return isAdded;
        }
    }
        
    bool WfManager::removeDataPoint(TA_Base_Bus::DataPoint* dp)
    {
		FUNCTION_ENTRY("removeDataPoint");
        TA_ASSERT( NULL != dp, "datapoint is NULL" );

        unsigned long i = 0;

        //remove data point to WfUnit to be processed by StationSystemOutputWirter
		if (m_pWfUnit != NULL)
		{
			m_pWfUnit->removeDataPoint(dp);
		}
		FUNCTION_EXIT("removeDataPoint");
        return true;
    }
        
    void WfManager::setToControl()
    {
		FUNCTION_ENTRY("setToControl");
		if (m_pWfUnit != NULL)
		{
			m_pWfUnit->setToControlMode();
			m_wfOutputWorker.start();
			m_wfPollingWorker.setInServiceState(true);
		}
		FUNCTION_EXIT("setToControl");
   }

    void WfManager::setToMonitor()
    {
		FUNCTION_ENTRY("setToMonitor");
		if (m_pWfUnit != NULL)
		{
			m_pWfUnit->setToMonitorMode();
			// before stop output thread, need to clear unsent write requests
			m_wfOutputWorker.clearWriteQueue();
			m_wfOutputWorker.terminateAndWait();
			m_wfPollingWorker.setInServiceState(false);
		}
		FUNCTION_EXIT("setToMonitor");

    }

	void WfManager::stopProcess()
	{
		FUNCTION_ENTRY("stopProcess");
		if (m_pWfUnit != NULL)
		{
			m_pWfUnit->stop();
		}
		m_wfOutputWorker.clearWriteQueue();
		m_wfOutputWorker.terminateAndWait();
		m_wfPollingWorker.setInServiceState(false);
		FUNCTION_EXIT("stopProcess");
	}  
}
