/*
* The source code in this file is the property of 
* Combuilder and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP/3001/transactive/app/maintenance_management/job_request_gui/src/ATSMMSSubmitMgr.cpp $
* @author: Noel R. Tajanlangit
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2012/03/20 14:12:57 $
* Last modified by:  $Noel R. Tajanlangit $
* 
* Implementation file for class ATSMMSSubmitMgr.
*
*/

#include "ATSMMSSubmitMgr.h"
#include "ATSMMSSubmitter.h"

#include "core/synchronisation/src/ThreadGuard.h"

namespace TA_IRS_App
{
	ATSMMSSubmitMgr* ATSMMSSubmitMgr::m_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable ATSMMSSubmitMgr::m_singletonLock;

	ATSMMSSubmitMgr::ATSMMSSubmitMgr(void)
		: m_pATSSubmitter(0)
	{
	}

	ATSMMSSubmitMgr::~ATSMMSSubmitMgr(void) 
	{
		if (NULL != m_pATSSubmitter)
		{
			delete m_pATSSubmitter;
			m_pATSSubmitter = NULL;
		}
	}

	ATSMMSSubmitMgr* ATSMMSSubmitMgr::getInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL == m_pInstance)
		{
			m_pInstance = new ATSMMSSubmitMgr();
		}

		return m_pInstance;
	}

	void ATSMMSSubmitMgr::releaseInstance()
	{
		TA_Base_Core::ThreadGuard guard(m_singletonLock);
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	IATSMMSSubmitter* ATSMMSSubmitMgr::getATSSubmitter()
	{
		TA_Base_Core::ThreadGuard guard(m_lockSubmitter);

		if (NULL == m_pATSSubmitter)
		{
			m_pATSSubmitter = new ATSMMSSubmitter();
		}

		return m_pATSSubmitter;
	}

}
