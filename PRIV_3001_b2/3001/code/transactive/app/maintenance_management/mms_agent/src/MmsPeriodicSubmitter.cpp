/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/maintenance_management/mms_agent/src/MmsPeriodicSubmitter.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * MmsPeriodicSubmitter class implimentation. See header file for class descrption
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)


#include "app/maintenance_management/mms_agent/src/MmsPeriodicSubmitter.h"
#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"
#include "app/maintenance_management/mms_agent/src/MmsConnectionException.h"

#include "app/maintenance_management/mms_agent/src/MmsMessageUtil.h"
#include "app/maintenance_management/mms_agent/src/MmsServerState.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessageConfig.h"
#include "core/message/src/NameValuePair.h"

#include <sstream>
#include <string>
#include <iomanip> 
#include <algorithm>


namespace TA_IRS_App
{

	MmsPeriodicSubmitter::MmsPeriodicSubmitter()
		:m_thread(NULL) 
	{
		FUNCTION_ENTRY("MmsPeriodicSubmitter");		

		FUNCTION_EXIT;
	}




	MmsPeriodicSubmitter::~MmsPeriodicSubmitter()
	{
		FUNCTION_ENTRY("~MmsPeriodicSubmitter");

		terminateAndDeleteThread();

		FUNCTION_EXIT;
	}

	void MmsPeriodicSubmitter::terminateAndDeleteThread()
	{
		FUNCTION_ENTRY("terminateAndDeleteThread");

		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if(m_thread != 0)
		{
			try
			{
				//stop the thread if required
				if(TA_Base_Core::Thread::THREAD_STATE_UNKNOWN != m_thread->getCurrentState() &&
				   TA_Base_Core::Thread::THREAD_STATE_TERMINATED != m_thread->getCurrentState() )	
				{
					m_thread->terminateAndWait();

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"MmsPeriodicSubmitter::setToMonitorMode() has stopped the thread" );
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
						"MmsPeriodicSubmitter::setToMonitorMode() says the thread is already stopped" );
				}
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"The attempt to stop the thread threw" );
			}

			try
			{
				delete m_thread;
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"The attempt to delete the thread threw" );
			}
			m_thread = 0;
		} 
		else //m_thread == 0
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"m_thread is already set to 0" );
		}


		FUNCTION_EXIT;
	}

	void MmsPeriodicSubmitter::createAndStartThread()
	{
		FUNCTION_ENTRY("createAndStartThread");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if(m_thread == NULL)
		{
			m_thread = new PeriodicThread();
		
			if (0 == m_data.get())
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "failed m_data != 0" );

				FUNCTION_EXIT;
				return;
			}
			m_data->invalidate();
			m_thread->initConfiguration(m_data);
			try
			{
				m_thread->start();
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"attempt to start thread failed" );
			}
			
		}	 

		 
 

		FUNCTION_EXIT;
	}


	void MmsPeriodicSubmitter::initConfiguration(TA_Base_Core::MmsPeriodicEntityDataPtr pData)
	{
		FUNCTION_ENTRY("initConfiguration");

		m_data = pData;

		m_entityName = pData->getName();
        m_entityKey = pData->getKey();

		pData->invalidate();	 
 

		FUNCTION_EXIT;
	}

	//IEntity methods:

    bool MmsPeriodicSubmitter::isValid()
	{
		FUNCTION_ENTRY("isValid");


		FUNCTION_EXIT;

		//Can't think why this would ever not be valid
		return true;
	}


    void MmsPeriodicSubmitter::start()
	{
		FUNCTION_ENTRY("start");
		createAndStartThread();
		FUNCTION_EXIT;
	}


    void MmsPeriodicSubmitter::stop()
	{
		FUNCTION_ENTRY("stop");		
		terminateAndDeleteThread();
		FUNCTION_EXIT;
	}


    void MmsPeriodicSubmitter::update(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		FUNCTION_ENTRY("update");
		
		//verify that this update is for me
		unsigned long keyToUpdate = updateEvent.getKey();
		if(keyToUpdate != m_entityKey)
		{
			FUNCTION_EXIT;
			return;
		}

		
		std::auto_ptr<TA_Base_Core::MmsPeriodicEntityData> pData( 
			dynamic_cast<TA_Base_Core::MmsPeriodicEntityData*>(
			TA_Base_Core::EntityAccessFactory::getInstance().getEntity(updateEvent.getKey())));


		if (pData.get()  == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"update() was called with ConfigUpdateDetails of unexpected subtype" );

			FUNCTION_EXIT;
			return;

		}

		if(m_thread == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"there is no thread to update" );

			FUNCTION_EXIT;
			return;
		}

		pData->invalidate();

        std::vector<std::string> paramNames = updateEvent.getChangedParams();
		std::vector<std::string>::const_iterator itNames;
		for(itNames = paramNames.begin(); itNames != paramNames.end(); itNames++)
		{
			if(TA_Base_Core::MmsPeriodicEntityData::SCAN_PERIOD_MIN == *itNames)
			{
				m_thread->setScanPeriodMinutes( pData->getScanPeriodMinutes() );
			}
			if(TA_Base_Core::MmsPeriodicEntityData::BACKLOG_CHECK_PERIOD_HOURS == *itNames)
			{
				//m_thread->setBacklogPeriodHours( pData->getBackLogCheckPeriodHours() );
			}
		}

		FUNCTION_EXIT;
	}


    void MmsPeriodicSubmitter::remove()
	{
		FUNCTION_ENTRY("remove");

        //does nothing
		FUNCTION_EXIT;
	}


    void MmsPeriodicSubmitter::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToControlMode");
		TA_Base_Core::ThreadGuard guard(m_threadLock);		
		if (m_thread !=NULL)
		{
			m_thread->setToControlMode();
		}
		 

		FUNCTION_EXIT;
	}


    void MmsPeriodicSubmitter::setToMonitorMode()
	{
		FUNCTION_ENTRY("setToMonitorMode");
		LOG0(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "setToMonitorMode");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_thread !=NULL)
		{
			m_thread->setToMonitorMode();
		}
		FUNCTION_EXIT;
	}


	//end IEntity methods



} //end namespace TA_IRS_App



