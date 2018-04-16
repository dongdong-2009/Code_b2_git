/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/IncidentHandler.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class simply maintains the incident list.  Whenever there is a change to
  * the incident list, it will inform the rest of the application through  
  */
#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/IncidentHandler.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/incidents/incident_manager/src/CachedIncident.h"

using namespace TA_Base_Core;

namespace TA_IRS_App
{
	
	const std::string IncidentHandler::RPARAM_ALARMID = "AlarmId";
	
	IncidentHandler::IncidentHandler(ItaIncidentRights& incidentRights, 
        ItaIncidentReportFactory& incidentReportFactory, IIncidentUpdate& incidentCallback) :
	m_incidentRights(incidentRights), m_incidentReportFactory(incidentReportFactory),
        m_incidentUpdate(incidentCallback)
	{
		FUNCTION_ENTRY("IncidentHandler::Constructor");
		
		m_incidents.clear(); //precaution
		m_listNeedChange = true;
		
		m_incidentReportFactory.registerCallback(this);
		m_incidents = m_incidentReportFactory.getIncidentReports();
		//set up the cached incident rights for the current session
		//TD12923 changed CachedIncident instance
		CachedIncident::getInstance().setCachedIncidentRight(m_incidents);
		
		RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
		RunParams::getInstance().registerRunParamUser(this, RPARAM_ALARMID.c_str());
		FUNCTION_EXIT;
	}

		
		IncidentHandler::~IncidentHandler()
		{
			FUNCTION_ENTRY("~IncidentHandler::Destructor");
			m_incidentReportFactory.deregister(); //deregister
			m_currentSessionIncidents.clear();
			//delete all incident objects
			
			// delete all modbus tables processes
			std::vector < ItaIncidentReportData * >::iterator itr;
			for ( itr = m_incidents.begin();
			itr != m_incidents.end();
			itr++ )
			{
				delete ( *itr );
				*itr = NULL;
			}
			
			m_incidents.clear();
			FUNCTION_EXIT;
		}
		
		void IncidentHandler::onRunParamChange(const std::string& name, const std::string& value)
		{
			FUNCTION_ENTRY("IncidentHandler::onRunParamChange");
			
			if (name.compare(RPARAM_SESSIONID) == 0)
			{
				//TD12918 - make sure m_listNeedChange must be set to 'true' 
				// before reloading the incidents to update the GUI correctly.
				//the list also needs to refresh since the right has changed
				m_listNeedChange = true;
				//++lizettejl
				m_incidentUpdate.updateRights();
				//reload the cached incident rights since the right has changed
				CachedIncident::getInstance().reloadCachedIncidentRight(m_incidents); //TD12923  
				
			}
			else if (name.compare(RPARAM_ALARMID) == 0)
			{
				m_incidentUpdate.createIncident(value);
			}
			
			FUNCTION_EXIT;
		}
		
		std::vector<ItaIncidentReportData*> IncidentHandler::getIncidents(bool wantAll)
		{
			FUNCTION_ENTRY("IncidentHandler::getIncidents");
			
			if (m_listNeedChange) //the session has been changed
			{
				m_currentSessionIncidents.clear();
				std::vector < ItaIncidentReportData * >::iterator itr;
				for ( itr = m_incidents.begin();
				itr != m_incidents.end();
				itr++ )
				{
					ItaIncidentReportData *incidentReportData = (*itr);
					if (m_incidentRights.doesIncidentSessionMatch(*incidentReportData) == true)
					{
						m_currentSessionIncidents.push_back(incidentReportData);
					}
				}
				m_listNeedChange = false;
			}
			
			if (wantAll == true) //return all incidents
			{
				FUNCTION_EXIT;
				return m_incidents;
			}
			else 	//return a subset of incidents
			{
				FUNCTION_EXIT;
				return m_currentSessionIncidents;
			}
		}
		
		void IncidentHandler::executeRunParam()
		{
			FUNCTION_ENTRY("IncidentHandler::executeRunParam");
			std::string value = RunParams::getInstance().get(RPARAM_ALARMID.c_str());
			if (value.empty() == false)    
			{
				onRunParamChange(RPARAM_ALARMID, value);
			}
			FUNCTION_EXIT;
		}
		
		void IncidentHandler::add(ItaIncidentReportData* incidentData)
		{
			FUNCTION_ENTRY("IncidentHandler::add");
			{
				//Bugzilla Bug 177: Mintao
				if (NULL == incidentData)
				{
					return;
				}
				
				bool found = false;
				
				ThreadGuard lvLock(m_incidentsLock); 
				//check if incident already existed
				std::vector<ItaIncidentReportData*>::iterator itr;        
				for (itr = m_incidents.begin(); itr != m_incidents.end(); itr++)
				{            
					if ((*itr)->getReportId() == incidentData->getReportId()) //already existed
					{
						found = true;
						break;
					}
				}
				
				if (found == false) //new incident
				{
					m_incidents.push_back(incidentData); //add to m_incodents list
					//update it to the cached incident right map
					CachedIncident::getInstance().addCachedIncidentRight(incidentData); //TD12923 
					m_listNeedChange = true; //make sure the current list get the change too
					m_incidentUpdate.reloadIncidents();  // reload incidents
				}
				else
				{
					// responsible for deleting the pointer
					delete incidentData;
					//Bugzilla Bug 177: Mintao
					incidentData = NULL;
				}
			}
			FUNCTION_EXIT;
		}
		
		//Mintao ++
		//Bug 652 (TD12313)
		void IncidentHandler::remove(const char* incidentId)
		{
			FUNCTION_ENTRY("IncidentHandler::remove");
			{
				ThreadGuard lvLock(m_incidentsLock); 
				std::vector<ItaIncidentReportData*>::iterator itr;
				for (itr = m_incidents.begin(); itr != m_incidents.end(); itr++)
				{
					ItaIncidentReportData *incidentReportData = (*itr);
					std::string reportId = incidentReportData->getReportId();
					if ( 0 == strcmp(reportId.c_str(), incidentId))
					{
						itr = m_incidents.erase(itr);
						m_listNeedChange = true; //make sure the current list get the change too
						//update it to the cached incident right map
						CachedIncident::getInstance().removeCachedIncidentRight(reportId);
						m_incidentUpdate.removeIncident(reportId);
						break;
					}
				}
			}
			FUNCTION_EXIT;
		}
		//Mintao ++
		//Bug 652 (TD12313)
		
		//Mintao ++
		//Bug 652 (TD12313)
		void IncidentHandler::update(const char* incidentId)
		{
			FUNCTION_ENTRY("IncidentHandler::update");
			{
				bool found = false;
				ItaIncidentReportData *reportData = NULL;
				{
					ThreadGuard lvLock(m_incidentsLock);
					std::vector<ItaIncidentReportData*>::iterator itr;        
					for (itr = m_incidents.begin(); itr != m_incidents.end(); itr++)
					{            
						reportData = (*itr);
						if ( 0 == strcmp((reportData->getReportId()).c_str(), incidentId)) 
						{
							found = true;
							reportData->reload();
							m_incidentUpdate.updateIncident(reportData);
							break;
						}
					}
				}
				
				if (found == false)
				{
					ItaIncidentReportData *incidentReportData = m_incidentReportFactory.getIncidentReport(incidentId);
					add(incidentReportData);
					m_incidentUpdate.updateIncident(incidentReportData);            
				}
			}
			FUNCTION_EXIT;
			//Mintao ++
			//Bug 652 (TD12313)
		}}// end TA_IRS_App
