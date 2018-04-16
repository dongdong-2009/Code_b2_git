// RTUTaskManager.cpp: implementation of the RTUTaskManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RTUTaskManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

std::vector< Run* > RTUTaskManager::m_vttask;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RTUTaskManager::RTUTaskManager()
{

}

RTUTaskManager::~RTUTaskManager()
{

}
