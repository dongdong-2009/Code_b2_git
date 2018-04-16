/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaStationReportData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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

#if !defined(ItaStationReportData_24D45300_A247_4e8d_8A25_DFD3AFDA49B0__INCLUDED_)
#define ItaStationReportData_24D45300_A247_4e8d_8A25_DFD3AFDA49B0__INCLUDED_
//Mintao ++
//Bug 652 (TD12313)
#include <string>
//Mintao ++
//Bug 652 (TD12313)

#include "core/data_access_interface/incident_dai/src/IncidentType.h"
#include "core/data_access_interface/incident_dai/src/IncidentLocationType.h"
#include "core/data_access_interface/incident_dai/src/IncidentConditionType.h"
#include "core/data_access_interface/incident_dai/src/IncidentCauseType.h"

#include "core/exceptions/src/IncidentException.h"

/**
 * Local storage for Station Reports. Reads Station Reports from the database.
 * Writes Station Reports to the database.
 * 
 * There are a large number of methods in this class. This is because the
 * relationships between the data elements have not been specified in the
 * requirements. Thus assumptions can not be made. The current arrangement of
 * methods also follows the likely useage patterns for this class. That is: update
 * one or more elements then write the result to the database.
 * @author Gregg Kirkpatrick
 * @version 1.0
 * @created 16-May-2005 12:33:58 PM
 */
namespace TA_IRS_Core
{	
	class ItaStationReportData
	{
		
	public:
		virtual ~ItaStationReportData();//{};
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getReportId() =0;
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* @param submitter
		* 
		*/
		virtual void setSubmittedBy(const std::string& submitter) =0;
		virtual std::string getSubmittedBy() =0;
		/**
		* @param ep
		* 
		*/
		virtual void setEmployeeParticulars(const std::string& ep) =0;
		virtual std::string getEmployeeParticulars() =0;
		/**
		* @param date
		* 
		*/
		virtual void setDate(time_t date) =0;
		virtual time_t getDate() =0;
		virtual void save() =0;
		virtual IncidentType getType() =0;
		/**
		* @param type
		* 
		*/
		virtual void setType(IncidentType type) =0;
		virtual void setLocation(IncidentLocationType location) =0;
		/**
		* Returns the location type
		* @return the location type
		*/
		virtual IncidentLocationType getLocation() =0;
		/**
		* @param condition
		* 
		*/
		virtual void setCondition(IncidentConditionType condition) =0;
		virtual IncidentConditionType getCondition() =0;
		virtual IncidentCauseType getCause() =0;
		/**
		* @param cause
		* 
		*/
		virtual void setCause(IncidentCauseType cause) =0;
		virtual std::string getAction() =0;
		/**
		* @param action
		* 
		*/
		virtual void setAction(std::string action) =0;
		virtual std::string getStaff() =0;
		/**
		* @param staff
		* 
		*/
		virtual void setStaff(std::string staff) =0;
		/**
		* Returns true if the data has been modified since the last database fetch/commit.
		* 
		* @return true if the data has been modified since the last database fetch/commit.
		* 
		*/
		virtual bool isModified() =0;
		virtual std::string getDescription() =0;
		/**
		* @param description
		* 
		*/
		virtual void setDescription(const std::string& description) =0;
		
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getIncidentId() = 0;
		virtual void setIncidentId(const std::string incidentId) = 0;
		//Mintao ++
		//Bug 652 (TD12313)
		
		virtual unsigned long getCreatorProfile() = 0;
		
		virtual unsigned long getCreateLocationId() = 0;
		
		
		/**
		* isNew
		* 
		* return whether the sation report is new or not
		* 
		* @return virtual bool 
		* 
		* @exception <exceptions> Optional
		*/
		virtual bool isNew() = 0;
		
		//TD16056   Mintao ++
        //for archive manager
		virtual void setClosedDate(time_t date) =0;
		virtual time_t getClosedDate() =0;
		//Mintao ++
        //for archive manager

		//Shiva ++
		//DTL-1174 - display of 7 digit number
		virtual std::string getDisplayReportId() = 0;
};
}// end TA_IRS_Core
#endif // !defined(ItaStationReportData_24D45300_A247_4e8d_8A25_DFD3AFDA49B0__INCLUDED_)