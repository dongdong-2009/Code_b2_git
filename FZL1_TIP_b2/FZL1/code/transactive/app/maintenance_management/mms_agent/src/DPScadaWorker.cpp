/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
* 
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/mms_agent/src/MaintenanceDataWrapper.cpp $
* @author:   huirong.luo
* @version:  $Revision: #1 $
*
* Last modification: $DateTime: 2011/02/28 15:06:30 $
* Last modified by:  $Author: weikun.lin $
*
* 
*/
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif
 
#include "app/maintenance_management/mms_agent/src/DPScadaWorker.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "bus/scada/common_library/src/ScadaUtilities.h"
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::NonReEntrantThreadLockable;
using TA_Base_Core::Semaphore;

namespace TA_IRS_App
{

	//
	// Constructor
	//
	DPScadaWorker::DPScadaWorker(unsigned long locationKey, IDPScadaWorkerCallback* callback)
		: m_terminated(false), m_queueSemaphore(0), m_locaitonKey(locationKey),m_callback(callback)
	{

	}


	//
	// Destructor
	//
	DPScadaWorker::~DPScadaWorker()
	{
		clear();
	}


	//
	// push
	// 
	void DPScadaWorker::push( std::vector<MaintenanceDataWrapper*>& dataVec )
	{
		LOG0( SourceInfo, DebugUtil::DebugDebug, "push");

		ThreadGuard guard(m_lock);	 
		if( !m_terminated )
		{            
			std::vector<MaintenanceDataWrapper*>::iterator iter=dataVec.begin();
			for (;iter!=dataVec.end();iter++)
			{
				MaintenanceDataWrapper* dataWrapper=*iter;
				ScadaUtilities::MaintenanceData*  data=dataWrapper->getData();
				unsigned long entityKey=data->entityKey;
				UpdateMap::iterator  findItr=m_updateMap.find(entityKey);
				if (findItr!=m_updateMap.end())
				{										
					LOG1(SourceInfo, DebugUtil::DebugWarn,"Entity %ld is out of data", entityKey );
					MaintenanceDataWrapper* oldData=findItr->second;
					m_updateMap.erase(findItr);
					delete oldData;
					
				}
				m_updateMap.insert(std::pair<unsigned long, MaintenanceDataWrapper*>
					(entityKey, dataWrapper));
			}		 
			m_queueSemaphore.post();
		}
	}
 
	//
	// clear
	//
	void DPScadaWorker::clear()
	{
		ThreadGuard guard(m_lock);
		UpdateMap::iterator p;
		UpdateMap::iterator begin = m_updateMap.begin();
		UpdateMap::iterator end = m_updateMap.end();
		for( p = begin; p != end; p++ )
		{
			if( p->second != NULL )
			{
				delete (p->second);
			}
		}
		m_updateMap.clear();
	}
	
	//
	// terminate
	//
	void DPScadaWorker::terminate()
	{		 
		ThreadGuard guard(m_lock);
		m_terminated = true;
		m_queueSemaphore.post();
	}

	void DPScadaWorker::run()
	{
		while( !m_terminated )
		{
			m_queueSemaphore.wait();

			if( m_terminated )
			{
				break;
			}

			//copy data to a temporary cache
			std::vector<MaintenanceDataWrapper*>  vecDataWrapper;
			std::vector<ScadaUtilities::MaintenanceData*>  vecData;
			{
				ThreadGuard guard(m_lock);	 
				UpdateMap::iterator  mapIter=m_updateMap.begin();
				for (;mapIter!=m_updateMap.end();mapIter++)
				{
					vecData.push_back(mapIter->second->getData());
					vecDataWrapper.push_back(mapIter->second);
				}
				m_updateMap.clear();
			}

			//call to agent to get data;
			try
			{
				TA_Base_Bus::ScadaUtilities::getInstance().getMaintenanceDataValue(vecData);
			}		 
			catch (...)
			{
				LOG1(SourceInfo, DebugUtil::DebugError,"cannot get maintenance data value loc=%ld", m_locaitonKey);
			}

			if ( NULL != m_callback )
			{
                try
                {
                    m_callback->dataNotify(m_locaitonKey,vecDataWrapper);
                }
                catch ( const std::exception& expWhat )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, typeid(expWhat).name(), expWhat.what() );
                }
                catch ( ... )
                {
                    LOG_EXCEPTION_CATCH( SourceInfo, "UnkonwnException", 
                        "Caught unknown exception when callback dataNotify" );
                }
			}

            std::vector<MaintenanceDataWrapper*>::iterator begin = vecDataWrapper.begin();
            std::vector<MaintenanceDataWrapper*>::iterator end = vecDataWrapper.end();
            for(std::vector<MaintenanceDataWrapper*>::iterator p = begin; p != end; p++ )
            {
				if (NULL != *p)
				{
					delete *p;
					*p = NULL;
				}
            }
            vecDataWrapper.clear();
            vecData.clear();
		}
	}

 
};
