/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/IncidentHandler.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class simply maintains the incident list.  Whenever there is a change to
  * the incident list, it will inform the rest of the application through  
  */

#if !defined(IncidentHandler_B9E5383E_FCBD_403f_9ECF_D362F1DC352C__INCLUDED_)
#define IncidentHandler_B9E5383E_FCBD_403f_9ECF_D362F1DC352C__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
    #pragma warning( disable : 4786)
#endif

    //Mintao ++
    //Bug 652 (TD12313)
#include <string>
    //Mintao ++
    //Bug 652 (TD12313)
#include "core/data_access_interface/incident_dai/src/ItaIncidentGuiCallback.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentReportFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include "bus/incidents/incident_rights/src/ItaIncidentRights.h"
#include "app/incidents/incident_manager/src/IIncidentUpdate.h"


/**
 * This class simply maintains the incident list.  Whenever there is a change to
 * the incident list, it will inform the rest of the application through
 * IIncidentUpdatable.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:18:58 PM
 */
using namespace TA_IRS_Bus;

namespace TA_IRS_App
{
	
	class IncidentHandler : public virtual TA_Base_Core::RunParamUser, 
		public virtual ItaIncidentGuiCallback 
	{
		
	public:
		virtual ~IncidentHandler();
		
	public:
	/**
	* @param name
	* @param value
	* 
		*/
		virtual void onRunParamChange(const std::string& name, const std::string& value);
		/**
		* A new Incident Report has been created by another operator and is viewable (at
		* least) by the operator associated with the current session.
		* @param incidentData
		* 
		*/
		virtual void add(ItaIncidentReportData* incidentData);
		/**
		* One of the reports in the list of viewable reports has been removed by another
		* operator.
		* @param incidentId
		* 
		*/
		virtual void remove(const char* incidentId);
		/**
		* A report in the list of reports viewable by the operator associated with the
		* current session has been modified by another operator.
		* @param incidentId
		* 
		*/
		virtual void update(const char* incidentId);
		/**
		* Returns the list of incidents to be displayed.  Returns all incidents or the
		* subset of incidents that has been created by an operator with the same profile
		* and location as the current operator.
		* @return all or a subset of the incidents that the operator has rights to view
		* @param wantAll    Set this to true if want to return all incidents.  False to
		* get only the incidents created by an operator with the same profile and
		* location as the current operator.
		* 
		*/
		std::vector<ItaIncidentReportData*> getIncidents(bool wantAll);
		/**
		* Constructor
		* @param incidentRights
		* @param incidentReportFactory
		* @param incidentCallback    This callback object will be called whenever there
		* is a change in the incident list.
		* 
		*/
		IncidentHandler(ItaIncidentRights& incidentRights, ItaIncidentReportFactory& incidentReportFactory, IIncidentUpdate& incidentCallback);
		/**
		* This method checks for the AlarmId run param.  If it is not empty, then it will
		* call onRunParamChange to perform the sequence that it usually carries out when
		* there is a change in AlarmId run param.
		* 
		* NOTE: This method should only be called during the startup.  It should not be
		* called anytime after that as the onRunParamChange method is called by the
		* RunParam framework.
		*/
		void executeRunParam();
		
	private:
		//disable method
		IncidentHandler();
		IncidentHandler(const IncidentHandler& );
		void operator=(const IncidentHandler&);
		/**
		* Protects the incident list.  Make this mutable.
		*/
		TA_Base_Core::NonReEntrantThreadLockable m_incidentsLock;
		/**
		* Reference to the object that performs actual logic behind incident rights
		*/
		ItaIncidentRights& m_incidentRights;
		ItaIncidentReportFactory& m_incidentReportFactory;
		IIncidentUpdate& m_incidentUpdate;
		std::vector<ItaIncidentReportData*> m_incidents;
		std::vector<ItaIncidentReportData*> m_currentSessionIncidents;
		bool m_listNeedChange;
		
	public:
		static const std::string RPARAM_ALARMID;
		
	};
}// end TA_IRS_App
#endif // !defined(IncidentHandler_B9E5383E_FCBD_403f_9ECF_D362F1DC352C__INCLUDED_)