/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/CtaStationReportData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Local storage for Station Reports. Reads Station Reports from the database.
  * Writes Station Reports to the database.
  * 
  * There are a large number of methods in this class. This is because the
  * relationships between the data elements have not been specified in the
  * requirements. Thus assumptions can not be made. The current arrangement of
  * methods also follows the likely useage patterns for this class. That is: update
  * one or more elements then write the result to the database.
  */
#if !defined(CtaStationReportData_178D8052_B5C0_4543_9DA7_25AD99E2B6D2__INCLUDED_)
#define CtaStationReportData_178D8052_B5C0_4543_9DA7_25AD99E2B6D2__INCLUDED_

/**
 * @version 1.0
 * @created 16-May-2005 12:33:47 PM
 */

#include <time.h>
//Mintao ++
//Bug 652 (TD12313)

#include <string>
//Mintao ++
//Bug 652 (TD12313)

#include "core/data_access_interface/incident_dai/src/IncidentType.h"
#include "core/data_access_interface/incident_dai/src/IncidentConditionType.h"
#include "core/data_access_interface/incident_dai/src/IncidentCauseType.h"
#include "core/data_access_interface/incident_dai/src/ItaStationReportData.h"
#include "core/data_access_interface/incident_dai/src/StationReportHelper.h"

 //TD15251 
//#include "bus/generic_server_simulator/src/GssCommon.h"

namespace TA_IRS_Core
{	
	class CtaStationReportData : public virtual ItaStationReportData 
	{
		//Mintao ++
		//Bug 652 (TD12313)
		
	public:    
		CtaStationReportData();
		//TD15204 - added boolean to identify the new station report
		CtaStationReportData(const std::string reportId, bool bIsNew = false);
		virtual ~CtaStationReportData();
		
	public:
		virtual std::string getReportId();
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* @param submitter
		* 
		*/
		virtual void setSubmittedBy(const std::string& submitter);
		virtual std::string getSubmittedBy();
		/**
		* @param ep
		* 
		*/
		virtual void setEmployeeParticulars(const std::string& ep);
		virtual std::string getEmployeeParticulars();
		/**
		* @param date
		* 
		*/
		virtual void setDate(time_t date);
		virtual time_t getDate();
		virtual void save();
		virtual IncidentType getType();
		/**
		* @param type
		* 
		*/
		virtual void setType(IncidentType type);
		virtual void setLocation(IncidentLocationType location);
		/**
		* Returns the location type
		* @return the location type
		*/
		virtual IncidentLocationType getLocation();
		/**
		* @param condition
		* 
		*/
		virtual void setCondition(IncidentConditionType condition);
		virtual IncidentConditionType getCondition();
		virtual IncidentCauseType getCause();
		/**
		* @param cause
		* 
		*/
		virtual void setCause(IncidentCauseType cause);
		virtual std::string getAction();
		/**
		* @param action
		* 
		*/
		virtual void setAction(std::string action);
		virtual std::string getStaff();
		/**
		* @param staff
		* 
		*/
		virtual void setStaff(std::string staff);
		/**
		* Returns true if the data has been modified since the last database fetch/commit.
		* 
		* @return true if the data has been modified since the last database fetch/commit.
		* 
		*/
		virtual bool isModified();
		/**
		* Associates the report to the incident.
		* @param incidentId    incident ID which is going to be the parent of this
		* station report.
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual void setIncidentId(const std::string incidentId);
		virtual std::string getIncidentId();
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* isNew
		* 
		* return whether the sation report is new or not
		* 
		* @return virtual bool 
		* 
		* @exception <exceptions> Optional
		*/
		virtual bool isNew();
		
		
		/**
		* @param description
		* 
		*/
		virtual std::string getDescription();
		virtual void setDescription(const std::string& description);
		
		virtual unsigned long getCreatorProfile();
		
		virtual unsigned long getCreateLocationId();
		
		//TD16056 Mintao ++
        //for archive manager
		virtual void setClosedDate(time_t date);
		virtual time_t getClosedDate();
		//Mintao ++
        //for archive manager
		/**
		* Initialise the station report with specified data.
		* @param submittedBy
		* @param employeeParticulars
		* @param date
		* @param type
		* @param location
		* @param condition
		* @param cause
		* @param action
		* @param staff
		* @param description
		* 
		*/
		CtaStationReportData(const std::string& submittedBy, 
            const std::string& employeeParticulars, 
            const time_t& date, 
            IncidentType type, 
            IncidentLocationType location,
            IncidentConditionType condition, 
            IncidentCauseType cause, 
            const std::string& action, 
            const std::string& staff, 
            const std::string& description
            );
private:
    bool m_isModified;
	//Mintao ++
	//Bug 652 (TD12313)
    std::string m_incidentId;
	//Mintao ++
	//Bug 652 (TD12313)
    StationReportHelper *m_stationReportHelper;
};

}// end TA_IRS_Core
#endif // !defined(CtaStationReportData_178D8052_B5C0_4543_9DA7_25AD99E2B6D2__INCLUDED_)