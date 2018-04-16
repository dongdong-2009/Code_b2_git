/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/incidents/incident_rights/src/CtaIncidentRights.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Incident Rights Class
  */

#if !defined(CtaIncidentRights_F734ABC9_0ED5_464a_A16F_0CB814A03015__INCLUDED_)
#define CtaIncidentRights_F734ABC9_0ED5_464a_A16F_0CB814A03015__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

#include <map>
#include <vector>

//TD15251 
#include "core/data_access_interface/incident_dai/src/ItaReportAccessFactory.h"

#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "core/utilities/src/RunParams.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h" //TD15203

namespace TA_Base_Bus
{
    class AuthenticationLibrary;
}

namespace TA_IRS_Core
{
	class ItaReportAccessFactory;
}

using namespace TA_IRS_Core;

namespace TA_IRS_Bus
{	
	
	class CtaIncidentRights : public virtual ItaIncidentRights, public virtual TA_Base_Core::RunParamUser,
		public virtual TA_Base_Core::IOnlineUpdatable
	{
		
	public:
		CtaIncidentRights() ;
		virtual ~CtaIncidentRights();
		
	public:
	/**
	* Decides if a given session can perform create/edit/close/reopen/delete/view on
	* a given Incident Report.  Note: If it is just gettting the rights for viewing
	* just use isViewPermitted.
	* @return bool true=permitted, false=denied.
	* @param incident    For edit, view, close, reopen, delete actions,
	* ItaIncidentReportData should contain a non-NULL value.  Pass NULL for 'new'
	* action.
	* 
		*/
		virtual bool isControlPermitted(ItaIncidentReportData* incident);
		/**
		* Returns true if the profile and location used to create/modify the incident
		* matches the current session's profile and location.
		* @param incident
		* 
		*/
		virtual bool doesIncidentSessionMatch(ItaIncidentReportData& incident);
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
		virtual bool isControlPermitted(ItaStationReportData* stationReport);
		/**
		* Set the server state if it has changed.
		* @param isOnline    Set this to true if the server is online.  Set it to false
		* otherwise.
		* 
		*/
		void setServerState(bool isOnline);
		/**
		* Returns true if the current session is allowed to view the given incident.
		* @return true if the current session is allowed to view the given incident.
		* @param incident
		* 
		*/
		virtual bool isViewPermitted(ItaIncidentReportData& incident);
		/**
		* Returns true if the current session is allowed to view the given station report.
		* 
		* @return true if the current session is allowed to view the given station report.
		* 
		* @param stationReport
		* 
		*/
		virtual bool isViewPermitted(ItaStationReportData& stationReport);
		
		/**
		* @param name
		* @param value
		* 
		*/
		virtual void onRunParamChange(const std::string& name, const std::string& value);
		virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);
		
	private:
		
		void createMatrix(AccessMatrix& matrix, bool isIncident);
		// This is used to get the current session profiles
		TA_Base_Bus::AuthenticationLibrary* m_authMgr;
		// All profile keys associated with this session
		std::vector<unsigned long> m_sessionProfiles;
		
		std::map<unsigned long, unsigned long> m_profileGroupMap;
		//TD12923 - added to keep track of the diff location keys
		unsigned long m_occLocationKey;
		unsigned long m_locationKey;
		unsigned long m_consoleLocationKey;
		
		
		
	private:
		ItaReportAccessFactory* m_incidentReportAccess;
		AccessMatrix m_incidentReportAccessMatrix;
		AccessMatrix m_stationReportAccessMatrix;
		
		bool m_bOnline;
		
		//TD15203 - Added incident types to make retrieval of access matrices
		// more meaningful
		enum IncidentType
		{
			STATION_REPORT = 0,   // not incident
				INCIDENT_REPORT = 1,   // is incident            
		};
	};
		
}// end TA_IRS_Bus
#endif // !defined(CtaIncidentRights_F734ABC9_0ED5_464a_A16F_0CB814A03015__INCLUDED_)