/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/incidents/incident_rights/src/ItaIncidentRights.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Wraps the ItaRights interface for the Incident Manager. This manager has access
  * control rules that can not be implemented using rights. These rules will be
  * implemented by the class that realises this interface.
  */
#ifndef ItaIncidentRights_DC617C68_36A8_4571_BEF4_6CD664427F4A__INCLUDED_
#define ItaIncidentRights_DC617C68_36A8_4571_BEF4_6CD664427F4A__INCLUDED_
#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

// TD15251  - moved to core
//#include <map>
//#include "bus/incidents/incident_rights/src/EAccessType.h"

namespace TA_IRS_Core
{
	class ItaIncidentReportData;
	class ItaStationReportData;
}

using namespace TA_IRS_Core;

namespace TA_IRS_Bus
{
	// TD15251  - moved to core
	//typedef std::map<unsigned long, EAccessType> ProfileAccess; //just a mapping of profile group to access level
	//typedef std::map<unsigned long, ProfileAccess> AccessMatrix;
	
	class ItaIncidentRights
	{
		
	public:
		virtual ~ItaIncidentRights() {};
		/**
		* Decides if a given session can perform create/edit/close/reopen/delete/view on
		* a given Incident Report.  Note: If it is just gettting the rights for viewing
		* just use isViewPermitted.
		* @return bool true=permitted, false=denied.
		* @param incidentReport    For edit, view, close, reopen, delete actions,
		* ItaIncidentReportData should contain a non-NULL value.  Pass NULL for 'new'
		* action.
		* 
		*/
		virtual bool isControlPermitted(ItaIncidentReportData* incidentReport) =0;
		/**
		* Decides if a given session can perform create/edit/close/reopen/delete/view on
		* a given Station Report.  Note: If it is just gettting the rights for viewing
		* just use isViewPermitted.
		* @return bool true=permitted, false=denied.
		* @param stationReport    For edit, view, close, reopen, delete actions,
		* ItaStationReportData should contain a non-NULL value.  Pass NULL for 'new'
		* action.
		* 
		*/
		virtual bool isControlPermitted(ItaStationReportData* stationReport) =0;
		/**
		* Returns true if the current session is allowed to view the given incident
		* report.
		* @return true if the current session is allowed to view the given incident
		* report.
		* @param incidentReport
		* 
		*/
		virtual bool isViewPermitted(ItaIncidentReportData& incidentReport) =0;
		/**
		* Returns true if the current session is allowed to view the given station report.
		* 
		* @return true if the current session is allowed to view the given station report.
		* 
		* @param stationReport
		* 
		*/
		virtual bool isViewPermitted(ItaStationReportData& stationReport) =0;
		/**
		* Returns true if the profile and location used to create/modify the incident
		* matches the current session's profile and location.
		* @param incident
		* 
		*/
		virtual bool doesIncidentSessionMatch(ItaIncidentReportData& incident) =0;
		
	};
}// end TA_IRS_Bus
#endif
