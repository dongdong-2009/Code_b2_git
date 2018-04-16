/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/incidents/incident_manager/src/IIncidentUpdate.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This interface will be called by the IncidentHandler when there is a change in the 
  * incident list
  */
#if !defined(IIncidentUpdate_E868378C_3CE5_436c_8CB1_F9F6AC2D3BE7__INCLUDED_)
#define IIncidentUpdate_E868378C_3CE5_436c_8CB1_F9F6AC2D3BE7__INCLUDED_

/**
 * This interface will be called by the IncidentHandler when there is a change in
 * the incident list.
 * @author San Teo
 * @version 1.0
 * @created 16-May-2005 12:18:58 PM
 */
#include <string>
namespace TA_IRS_Core
{
	class ItaIncidentReportData;
}

using namespace TA_IRS_Core;

namespace TA_IRS_App
{	
	class IIncidentUpdate
	{
		
	public:
	/**
	* Could be called when there is a change in the incident list.
		*/
		virtual void reloadIncidents() =0;
		/**
		* Display the dialog for creating an incident report
		* @param alarmId
		* 
		*/
		virtual void createIncident(const std::string& alarmId) =0;
		/**
		* Updates the rights on the selected incident based on the operator's new
		* privileges.
		*/
		virtual void updateRights() =0;
		/**
		* Remove the specified incident from the list.
		* @param incidentId
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual void removeIncident(std::string incidentId) =0;
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* Updates the specified incident on the displayable list.
		* @param incident
		* 
		*/
		virtual void updateIncident(ItaIncidentReportData* incident) =0;
		
	};
}// end TA_IRS_App
#endif // !defined(IIncidentUpdate_E868378C_3CE5_436c_8CB1_F9F6AC2D3BE7__INCLUDED_)