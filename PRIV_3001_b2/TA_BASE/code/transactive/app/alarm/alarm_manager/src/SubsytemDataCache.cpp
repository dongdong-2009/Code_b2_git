/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/shared_alarm/src/SubsytemDataCache.h$
  * @author:  Lin Weikun
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/03/01 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * SubsystemDataCache is a singleton  that get subsystem from subsystem table
  */

#include "core\exceptions\src\DatabaseException.h"  


#include "app/alarm/alarm_manager/src/SubsytemDataCache.h"
#include "app/event/ATSBridgeUtil/src/ATSBridgeUtil_i.h"
#include "app/event/ATSBridgeUtil/src/ATSBridgeUtil_i.c"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include <atlconv.h>
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
	// Initialise statics
	SubsystemDataCache* SubsystemDataCache::m_instance = 0;
	TA_Base_Core::NonReEntrantThreadLockable SubsystemDataCache::m_singletonLock;
	unsigned long ATSSubsystemDataCache::atsKey=0;
	void SubsystemDataCache::InitializeSystem()
	{
		try
		{
			m_allSystemData = TA_Base_Core::SystemAccessFactory::getInstance().getAllSystems();
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to retrieve subsystems matching the one passed in");
			for (unsigned int i = 0;i < m_allSystemData.size(); ++i)
			{
				delete m_allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the severity information from the database.");
			for (unsigned int i = 0;i < m_allSystemData.size(); ++i)
			{
				delete m_allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
			for (unsigned int i = 0;i < m_allSystemData.size(); ++i)
			{
				delete m_allSystemData[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving subsystem information."));
		}
		return;
	}
	void SubsystemDataCache::InitializeSubsystem()
	{
		try
		{
			//load subsystem from database.
			m_subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(false);
		}
		catch(TA_Base_Core::DataException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
				"Errors occured while trying to retrieve subsystems matching the one passed in");
			for (unsigned int i = 0;i < m_subsystems.size(); ++i)
			{
				delete m_subsystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
				"Errors occured while trying to retrieve the severity information from the database.");
			for (unsigned int i = 0;i < m_subsystems.size(); ++i)
			{
				delete m_subsystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Failed to retrieve subsystem information from database. Aborting."));
		}
		catch(...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "UnknownException", 
				"Unknown error whilst retrieving subsystem information!");
			for (unsigned int i = 0;i < m_subsystems.size(); ++i)
			{
				delete m_subsystems[i];
			}
			TA_THROW(TA_Base_Core::TransactiveException("Unknown exception caught whilst retrieving subsystem information."));
		}
		return;
	}
	void SubsystemDataCache::getAllSystems(TA_Base_App::ListItems &systemList)
	{
		m_atsSubsystemDataCache.getAllSystems(systemList);
		for (int i=0;i<m_allSystemData.size();i++)
		{
			ListItem system(m_allSystemData[i]->getPkey(),m_allSystemData[i]->getName());
			systemList.push_back(system);
		}
	}

	SubsystemDataCache& SubsystemDataCache::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);

		if ( m_instance == NULL )
		{
			m_instance = new SubsystemDataCache();
			m_instance->InitializeSubsystem();
			m_instance->InitializeSystem();
		}

		return *m_instance;
	}

	unsigned long SubsystemDataCache::getSystemFormSubsystem(unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSystemFormSubsystem");
	
		for(std::vector<TA_Base_Core::ISubsystem*>::iterator itr = m_subsystems.begin();itr != m_subsystems.end(); itr++)
		{
			//change ISubsystem to Subsystem
			TA_Base_Core::Subsystem * pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(*itr);
			if(pSubSystem->getKey()==subsystemKey)
			{
				FUNCTION_EXIT;
				return pSubSystem->getSystemKey();
			}	
		}
		
		FUNCTION_EXIT;
		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Cann't find system_key where subsystem_key=%ul, please check configuration",  subsystemKey);
		return 0;
	}

	TA_Base_App::ListItems SubsystemDataCache::getSubsystemsFormSystems(TA_Base_App::ListItems systemKeys)
	{
		FUNCTION_ENTRY("getSubsystemsFormSystems");
		TA_Base_App::ListItems subsystemItems;
		if (0 == systemKeys.size())
		{
			m_atsSubsystemDataCache.getAllSubsystems(subsystemItems);
			for (int i=0;i<m_subsystems.size();i++)
			{
				ListItem subsystem(m_subsystems[i]->getKey(),m_subsystems[i]->getName());
				subsystemItems.push_back(subsystem);
			}
			return subsystemItems;
		}
		
		for(ListItems::iterator systemItr=systemKeys.begin();systemItr!=systemKeys.end();systemItr++)
		{
			//get ats subsytems and iscs subsytems
			std::list<std::string> atsSubsystem;
			if ((*systemItr).pkey >= ATS_SYSTEM_SUBSYSTEM_KEY)
			{
				m_atsSubsystemDataCache.getSubsystemNamesFormSystem((*systemItr).str,subsystemItems);
			}
			else
			{
				for(int i=0;i<m_subsystems.size();i++)
				{
					TA_Base_Core::Subsystem * pSubSystem = dynamic_cast<TA_Base_Core::Subsystem *>(m_subsystems[i]);

					if(pSubSystem->getSystemKey()==(*systemItr).pkey)
					{
						ListItem subsystem(m_subsystems[i]->getKey(),m_subsystems[i]->getName());
						subsystemItems.push_back(subsystem);
					}
				}			
			}
			

		}
		
		FUNCTION_EXIT;
		return subsystemItems;
	}
	SubsystemDataCache::~SubsystemDataCache()
	{
		//release the memory
		for (unsigned int i = 0;i < m_subsystems.size(); ++i)
		{
			delete m_subsystems[i];
		}
	}



	ATSSubsystemDataCache::ATSSubsystemDataCache()
	{
		USES_CONVERSION;

		FUNCTION_ENTRY("ATSSubsystemDataCache");
		CoInitialize(0);
		{
			CComPtr<IATSEventSystemList> pEventSystems;
			CComPtr<IATSStringList> pEventTypes;
			CComPtr<IATSStringList> pAlarmTypes;
			CComPtr<IATSBridgeUtility> pATSBridgeUtility;
			HRESULT hr = pATSBridgeUtility.CoCreateInstance(CLSID_ATSBridgeUtility, NULL, CLSCTX_INPROC_SERVER);
			if (!FAILED(hr))
			{
				long err;
				hr = pATSBridgeUtility->DoEventInfoQuery (&pEventSystems, &pEventTypes, &pAlarmTypes, &err);
				if(err == 0 /*ATS_QUERY_OK*/)
				{
					long lSystemSize=0;
					pEventSystems->GetSize(&lSystemSize);
					for(int i=0; i<lSystemSize; i++)
					{
						CComPtr<IATSEventSystem> pATSEventSystem;
						ATSAlarmSystemType atsAlarmSystem;
						pEventSystems->GetItem(i,&pATSEventSystem);

						CComBSTR bstrName;
						pATSEventSystem->get_Name(&bstrName);
						atsAlarmSystem.Name = OLE2A(bstrName);

						atsAlarmSystem.systemKey = ATS_SYSTEM_SUBSYSTEM_KEY + atsKey++;

						CComPtr<IATSStringList> pSubSystemList;
						pATSEventSystem->GetSubSystemList( &pSubSystemList);
						long lSubSystemSize=0;
						pSubSystemList->GetSize(&lSubSystemSize);
						for( int j=0; j<lSubSystemSize; j++)
						{
							CComBSTR bstrSubSystemName;
							pSubSystemList->GetItem(j, &bstrSubSystemName);
							std::string subName = OLE2A(bstrSubSystemName);

							atsAlarmSystem.SubsystemNames.push_back(ATSAlarmSubsystem(subName.c_str(),ATS_SYSTEM_SUBSYSTEM_KEY + atsKey++));
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data %d: %s", j+1, subName.c_str());
						}

						m_alarmSystems.push_back(atsAlarmSystem);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Data %d: %s", i+1, atsAlarmSystem.Name.c_str());
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Unable to load ATSBridgeUtility.dll. ATSBridgeUtility may not be installed correctly.");
			}
		}
		CoUninitialize();
		FUNCTION_EXIT;
	}
    void ATSSubsystemDataCache::getSubsystemNamesFormSystem(const std::string Name, TA_Base_App::ListItems &atsSubsystemList)
	{
		for (std::list<ATSAlarmSystemType>::iterator itr=m_alarmSystems.begin();itr!=m_alarmSystems.end();itr++)
		{
			if (itr->Name == Name)
			{
				for (std::list<ATSAlarmSubsystem>::iterator itrSubsytem=itr->SubsystemNames.begin();itrSubsytem!=itr->SubsystemNames.end();itrSubsytem++)
				{
					atsSubsystemList.push_back(ListItem(itrSubsytem->subsystemKey,itrSubsytem->Name.c_str()));
				}
				return;
			}
		}
		return;
	}
	void ATSSubsystemDataCache::getAllSystems(TA_Base_App::ListItems &systemList)
	{
		for (std::list<ATSAlarmSystemType>::iterator itr=m_alarmSystems.begin();itr!=m_alarmSystems.end();itr++)
		{
			ListItem system(itr->systemKey,itr->Name.c_str());
			systemList.push_back(system);
		}
		return;
	}
	void ATSSubsystemDataCache::getAllSubsystems(TA_Base_App::ListItems &subsystemList)
	{
		for (std::list<ATSAlarmSystemType>::iterator itrSystem=m_alarmSystems.begin();itrSystem!=m_alarmSystems.end();itrSystem++)
		{
			for (std::list<ATSAlarmSubsystem>::iterator itrSubsystem=itrSystem->SubsystemNames.begin();itrSubsystem!=itrSystem->SubsystemNames.end();itrSubsystem++)
			{
				ListItem system(itrSubsystem->subsystemKey,itrSubsystem->Name.c_str());
				subsystemList.push_back(system);
			}
		}
		return;
	}
}
