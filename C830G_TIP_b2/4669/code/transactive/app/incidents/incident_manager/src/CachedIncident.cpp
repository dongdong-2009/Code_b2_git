/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/CachedIncident.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// CachedIncident.cpp: implementation of the CachedIncident class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include "app/incidents/incident_manager/src/CachedIncident.h"
#include "app/incidents/incident_manager/src/stdafx.h"
#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "core/synchronisation/src/ThreadGuard.h" //TD12923 


using namespace std;

namespace TA_IRS_App
{
	CachedIncident* CachedIncident::m_me = NULL;
	TA_Base_Core::NonReEntrantThreadLockable CachedIncident::m_singletonLock; // //TD12923 
	
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
	
	CachedIncident::CachedIncident():
	m_incidentRights(NULL)
	{
		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
	}
	
	CachedIncident::~CachedIncident()
	{
		FUNCTION_ENTRY("Destructor");
		m_cachedIncidentControl.clear();
		m_cachedIncidentView.clear();
		FUNCTION_EXIT;
	}
	
	
	CachedIncident& CachedIncident::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		
		TA_Base_Core::ThreadGuard guard(m_singletonLock); //TD12923
		if ( 0 == m_me )
		{
			m_me = new CachedIncident();
		}
		
		FUNCTION_EXIT;
		return *m_me;
	}
	
	void CachedIncident::removeInstance( )
	{
		FUNCTION_ENTRY("removeInstance");
		
		TA_Base_Core::ThreadGuard guard(m_singletonLock); //TD12923 
		if ( m_me != NULL )
		{
			delete m_me;
			m_me = NULL;
		}
		FUNCTION_EXIT;
	}
	
	void CachedIncident::setIncidentRights(ItaIncidentRights& incidentRights)
	{
		FUNCTION_ENTRY("setIncidentRights");
		m_incidentRights = &incidentRights;
		FUNCTION_EXIT;
	}
	
	void CachedIncident::setCachedIncidentRight(std::vector<ItaIncidentReportData*>& cachedincident)
	{
		FUNCTION_ENTRY("setCachedIncidentRight");
		
		TA_ASSERT(m_incidentRights != NULL, "The incident rights in the cached incidents has not been set up!");
		
		m_cachedIncidentControl.clear();
		m_cachedIncidentView.clear();
		
		std::vector<ItaIncidentReportData*>::iterator it;
		for (it = cachedincident.begin(); it != cachedincident.end() ; ++it)
		{
			std:string reportid = (*it)->getReportId();
			//cached the control right
			bool reportright = m_incidentRights->isControlPermitted(*it);
			m_cachedIncidentControl.insert(pair<string, bool>(reportid, reportright));
			//cached the view right
			//TD12923 
			// only check if view is permitted if control not permitted, otherwise, if control is 
			// permitted, then view will definitely be permitted
			if (!reportright)   
			{
				reportright = m_incidentRights->isViewPermitted(*(*it));
			}
			m_cachedIncidentView.insert(pair<string, bool>(reportid, reportright));
		}
		FUNCTION_EXIT;
	}
	
	void CachedIncident::reloadCachedIncidentRight(std::vector<ItaIncidentReportData*>& cachedincident)
	{
		FUNCTION_ENTRY("reloadCachedIncidentRight");
		//just call the set
		setCachedIncidentRight(cachedincident);
		FUNCTION_EXIT;
	}
	
	// (TD13864) - lizettejl 
	bool CachedIncident::getCachedIncidentControlRight(std::string& id)
	{
		FUNCTION_ENTRY("getCachedIncidentRight");
		bool bIncidentControlRight = false;    //(TD13864) 
		std::map<std::string, bool>::iterator it = m_cachedIncidentControl.find(id);
		// (TD13864)  
		// removed assert statement to handle an Incident id that is not found in the CachedIncidents
		//TA_ASSERT(it != m_cachedIncidentControl.end(), "Can not find the cached incident rights!");
		if (it != m_cachedIncidentControl.end())
			bIncidentControlRight = it->second;
		FUNCTION_EXIT;    
		//return it->second; (TD13864) 
		return bIncidentControlRight;
	}
	
	// (TD13864) - lizettejl 
	bool CachedIncident::getCachedIncidentViewRight(std::string& id)
	{
		FUNCTION_ENTRY("getCachedIncidentRight");
		bool bIncidentControlViewRight = false;     //(TD13864) 
		std::map<std::string, bool>::iterator it = m_cachedIncidentView.find(id);
		//(TD13864) 
		// removed assert statement to handle an Incident id that is not found in the CachedIncidents
		// TA_ASSERT(it != m_cachedIncidentView.end(), "Can not find the cached incident rights!");
		if(it != m_cachedIncidentView.end())
			bIncidentControlViewRight = it->second;
		FUNCTION_EXIT;
		//(TD13864) 
		//return it->second;
		return bIncidentControlViewRight;
		
	}
	
	void CachedIncident::addCachedIncidentRight(ItaIncidentReportData* incidentData)
	{
		FUNCTION_ENTRY("addCachedIncidentRight");
		TA_ASSERT(m_incidentRights != NULL, "The incident rights in the cached incidents has not been set up!");
		std::map<std::string, bool>::iterator it = m_cachedIncidentControl.find(incidentData->getReportId());
		if (it == m_cachedIncidentControl.end())
		{
			m_cachedIncidentControl.insert(pair<string, bool>(incidentData->getReportId(), m_incidentRights->isControlPermitted(incidentData)));
		}
		
		it = m_cachedIncidentView.find(incidentData->getReportId());
		if (it == m_cachedIncidentView.end())
		{
			m_cachedIncidentView.insert(pair<string, bool>(incidentData->getReportId(), m_incidentRights->isViewPermitted(*incidentData)));
		}
		
		FUNCTION_EXIT;
	}
	
	void CachedIncident::removeCachedIncidentRight(std::string& id)
	{
		FUNCTION_ENTRY("removeCachedIncidentRight");
		std::map<std::string, bool>::iterator it = m_cachedIncidentControl.find(id);
		if (it != m_cachedIncidentControl.end())
		{
			m_cachedIncidentControl.erase(it);
		}
		
		it = m_cachedIncidentView.find(id);
		if (it != m_cachedIncidentView.end())
		{
			m_cachedIncidentView.erase(it);
		}
		FUNCTION_EXIT;
	}
	
}// end TA_IRS_App
