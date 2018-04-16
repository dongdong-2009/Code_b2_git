// PopulateEventLevelsThread.cpp: implementation of the CPopulateEventLevelsThread class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "app\event\event_viewer\src\EventViewer.h"
#include "app/event/event_viewer/src/PopulateEventLevelsThread.h"
#include "app\event\event_viewer\EventListComponent\src\EventListComponent.h"

#include "core\data_access_interface\entity_access\src\ConfigurationEditor.h"
#include "core\utilities\src\FunctionEntry.h"

CPopulateEventLevelsThread::CPopulateEventLevelsThread()
{

}

CPopulateEventLevelsThread::~CPopulateEventLevelsThread()
{

}

void CPopulateEventLevelsThread::run()
{
	FUNCTION_ENTRY("CPopulateEventLevelsThread::run");
	
	unsigned long numLevels = TA_Base_Core::ConfigurationEditor::getEventLevels();
	PostMessage(AfxGetMainWnd()->GetSafeHwnd(),  WM_USER_POPULATE_EVENT_LEVELS, numLevels, 0);
  
	FUNCTION_EXIT;
}

void CPopulateEventLevelsThread::terminate()
{
}
