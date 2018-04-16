#include "app/ats/AtsAgent/TimetableManager/src/TimetableManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_IRS_App{

TimetableManager::TimetableManager(unsigned long TimetblCheckInterval, std::string TimetbleDirPath):
m_timetableProcessor(NULL),
m_timetableService(NULL)
{	
	FUNCTION_ENTRY("TimetableManager()");

	m_timetableProcessor =  new TimetableProcessor();
	m_timetableService = new TimetableService(*m_timetableProcessor, TimetblCheckInterval, TimetbleDirPath);


	FUNCTION_EXIT;

}

TimetableManager::~TimetableManager()
{	

	if(m_timetableService != NULL)
	{
		m_timetableService->terminateAndWait();
		delete m_timetableService;
		m_timetableService = NULL;
	}

	if(m_timetableProcessor != NULL)
	{
		delete m_timetableProcessor;
		m_timetableProcessor = NULL;
	}


}

void TimetableManager::start()
{
	if(m_timetableService != NULL)
	{
		m_timetableService->start();
	}
}


void TimetableManager::OnControlMode()
{
	if(m_timetableService != NULL)
	{
		m_timetableService->OnControlMode();
	}
}

void TimetableManager::OnMonitorMode()
{
	if(m_timetableService != NULL)
	{
		m_timetableService->OnMonitorMode();
	}
}
}