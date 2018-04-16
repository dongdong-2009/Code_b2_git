/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaIncidentReportFactory.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Creates ItaIncidentReportData objects. Retrieves ItaIncidentReportData objects
  * from the database. Subscribes to Incident Report database updates. Calls back
  * on the GUI when updates arrive that are of interest to the GUI.
  */
#ifndef ItaIncidentReportFactory_DC69BBF3_70A9_44c2_9C70_BCA0CCAFB4AA__INCLUDED_
#define ItaIncidentReportFactory_DC69BBF3_70A9_44c2_9C70_BCA0CCAFB4AA__INCLUDED_

#include <vector>
#include <vector>
#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"
#include "core/data_access_interface/incident_dai/src/ItaIncidentGuiCallback.h"

namespace TA_IRS_Core
{	
	class ItaIncidentReportFactory
	{
		
	public:
		virtual ~ItaIncidentReportFactory() {};
		/**
		* Retrieve all Incident Reports that are viewable by the current Profile/Location
		* combination. For performance reasons, the factory is responsible performing
		* profile/location checks via SQL to determine viewability.
		* 
		* NOTE: The caller is responsible for deleting these objects
		*/
		virtual std::vector<ItaIncidentReportData*> getIncidentReports() =0;
		/**
		* Only one gui can register. When edit, add, or remove updates arrive
		* ItaIncidentReportFactory will call on this interface with the new information.
		* @param gui
		* 
		*/
		//virtual void register(ItaIncidentGuiCallback* gui) =0;
		virtual void registerCallback(ItaIncidentGuiCallback* gui) =0;
		/**
		* As only one GUI can register, the deregister method does not need a parameter.
		*/
		virtual void deregister() =0;
		/**
		* Returns the incident report data with the given incident ID.
		* @return the incident report data with the given incident ID.   NULL if there is
		* no associated incident.
		* 
		* NOTE: The caller is responsible for deleting these objects
		* @param id    the incident id
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual ItaIncidentReportData* getIncidentReport(std::string id) =0;
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* Returns a new instance. It determines creator details from the current session.
		* Create an incident report with just the basic report details.  The set methods
		* would have to be used to set other attributes.  The method will figure out the
		* creator and creator profile from the session ID and the creation date and time
		* from the current timestamp. 
		*/
		virtual ItaIncidentReportData* createIncidentReport() =0;
		
		/**
		* Creates an incident with an associated alarm.
		* @param alarmId    The alarm Id
		* 
		*/
		virtual ItaIncidentReportData* createIncidentReport(const std::string & alarmId, const std::string & alarmValue ) =0;
	};
}// end TA_IRS_Core
#endif
