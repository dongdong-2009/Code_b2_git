/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/incident_dai/src/CtaStationReportData.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2014/05/29 17:21:44 $
  * Last modified by:  $Author: Shiva $
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

#include "core/data_access_interface/incident_dai/src/CtaStationReportData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include "core/exceptions/src/DataException.h"

namespace TA_IRS_Core
{
	
	static const std::string IN_STATION_ID_COL	        = "INSTAT_ID";
	static const std::string IN_SUBMITTED_BY_COL        = "SUBMITTED_BY";
	static const std::string IN_EP_COL                  = "EP";
	static const std::string IN_INCIDENTDATE_COL        = "INCIDENT_DATE";
	static const std::string IN_TYPE_COL                = "TYPE";
	static const std::string IN_OTHERTYPE_COL           = "OTHER_TYPE";
	static const std::string IN_LOCATION_COL            = "LOCATION";
	static const std::string IN_OTHERLOCATION_COL       = "OTHER_LOCATION";
	static const std::string IN_CAUSE_COL               = "CAUSE";
	static const std::string IN_OTHERCAUSE_COL          = "OTHER_CAUSE";
	static const std::string IN_CONDITION_COL           = "CONDITION";
	static const std::string IN_OTHERCONDITION_COL      = "OTHER_CONDITION";
	static const std::string IN_ACTION_COL              = "ACTION";
	static const std::string IN_STAFF_COL               = "STAFF";
	static const std::string IN_DESCRIPTION_COL         = "DESCRIPTION";
	static const std::string IN_CREATELOCATION_ID_COL   = "CREATE_LOCATI_ID";
	
	//Mintao ++
	//Bug 652 (TD12313)
	
	CtaStationReportData::CtaStationReportData() :
	m_stationReportHelper(new StationReportHelper()),
        m_incidentId (""),
        m_isModified(false)
	{
		FUNCTION_ENTRY("constructor");
		FUNCTION_EXIT;
	}
	
	// TD15204 - pass the incident report id if a new station report is created
	CtaStationReportData::CtaStationReportData(const std::string reportId, bool bIsNew) :
	//m_stationReportHelper(new StationReportHelper(reportId)),
	m_incidentId ("")
	{
		FUNCTION_ENTRY("user constructor");
		if (bIsNew)
		{
			m_stationReportHelper = new StationReportHelper();
			m_stationReportHelper->setIncidentReportId(reportId);
		}
		else
		{
			m_stationReportHelper = new StationReportHelper(reportId);
		}
		FUNCTION_EXIT;
	}
	
	CtaStationReportData::CtaStationReportData(const std::string& submittedBy, 
        const std::string& employeeParticulars, 
        const time_t& date, 
        IncidentType type, 
        IncidentLocationType location, 
        IncidentConditionType condition, 
        IncidentCauseType cause, 
        const std::string& action, 
        const std::string& staff, 
        const std::string& description) :        
	m_isModified(false),
        m_incidentId ("")
		
	{
		FUNCTION_ENTRY("user constructor");
		
		m_stationReportHelper = new StationReportHelper(submittedBy, employeeParticulars, date, 
            type, location, condition, cause, action, staff, description, -1);
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	CtaStationReportData::~CtaStationReportData()
	{
		FUNCTION_ENTRY("destructor");
		FUNCTION_EXIT;
	}

	//TD16056 Mintao++	
	void CtaStationReportData::setClosedDate(time_t date)
    {
		FUNCTION_ENTRY("setClosedDate");
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getDate() != date)
		{
			m_stationReportHelper->setClosedDate(date);
			m_isModified = true;
		}
		FUNCTION_EXIT;
    }

	time_t CtaStationReportData::getClosedDate()
    {
		FUNCTION_ENTRY("getClosedDate");
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		time_t incidentDate = m_stationReportHelper->getClosedDate();
		
		FUNCTION_EXIT;
		return incidentDate;
    }
	//TD16056 Mintao++	

	void CtaStationReportData::setSubmittedBy(const std::string &submitter)
	{
		FUNCTION_ENTRY("setSubmittedBy");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getSubmittedBy().compare(submitter) != 0)
		{
			m_stationReportHelper->setSubmittedBy(submitter);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setEmployeeParticulars(const std::string &ep)
	{
		FUNCTION_ENTRY("setEmployeeParticulars");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getEmployeeParticulars().compare(ep) != 0)
		{
			m_stationReportHelper->setEmployeeParticulars(ep);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setDate(time_t date)
	{
		FUNCTION_ENTRY("setDate");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getDate() != date)
		{
			m_stationReportHelper->setDate(date);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setType(IncidentType type)
	{
		FUNCTION_ENTRY("setType");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentType actualType = m_stationReportHelper->getType();
		
		if ( (actualType.arrestOfPerson != type.arrestOfPerson) ||
			(actualType.assault != type.assault) ||
			(actualType.criminalAct!= type.criminalAct) ||
			(actualType.damagedFacilityOrProperty != type.damagedFacilityOrProperty) ||
			(actualType.derailment != type.derailment) ||
			(actualType.equipmentAbnormally  != type.equipmentAbnormally) ||
			(actualType.equipmentMalfunction != type.equipmentMalfunction) ||
			(actualType.fire != type.fire) ||
			(actualType.heatedArgument != type.heatedArgument) ||
			(actualType.trainServiceDelay != type.trainServiceDelay) ||
			(actualType.trespassing != type.trespassing) ||
			(actualType.other.compare(type.other) != 0)
			) 
		{
			m_stationReportHelper->setType(type);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setLocation(IncidentLocationType location)
	{
		FUNCTION_ENTRY("setLocation");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentLocationType actualLocation = m_stationReportHelper->getLocation();
		
		if ( (actualLocation.train != location.train) || 
			(actualLocation.trackside != location.trackside) || 
			(actualLocation.station != location.station) || 
			(actualLocation.depot != location.depot) ||
			(actualLocation.other.compare(location.other) != 0)
			) 
		{
			m_stationReportHelper->setLocation(location);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setCondition(IncidentConditionType condition)
	{
		FUNCTION_ENTRY("setCondition");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentConditionType actualCondition = m_stationReportHelper->getCondition();
		
		if ( (actualCondition.clean != condition.clean) ||
			(actualCondition.dirty != condition.dirty) ||
			(actualCondition.dry != condition.dry) ||
			(actualCondition.poorlyLit != condition.poorlyLit) ||
			(actualCondition.wellLit != condition.wellLit) ||
			(actualCondition.wet != condition.wet) ||
			(actualCondition.other.compare(condition.other) != 0)
			) 
		{
			m_stationReportHelper->setCondition(condition);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setCause(IncidentCauseType cause)
	{
		FUNCTION_ENTRY("setCause");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentCauseType actualCause = m_stationReportHelper->getCause();
		
		if ( (actualCause.bLS != cause.bLS) ||
			(actualCause.eSP != cause.eSP) ||
			(actualCause.pax != cause.pax) ||
			(actualCause.point != cause.point) ||
			(actualCause.power != cause.power) ||
			(actualCause.signal != cause.signal) ||
			(actualCause.tkCct != cause.tkCct) ||
			(actualCause.other.compare(cause.other) != 0)
			) 
		{
			m_stationReportHelper->setCause(cause);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setAction(std::string action)
	{
		FUNCTION_ENTRY("setAction");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getAction().compare(action) != 0)
		{
			m_stationReportHelper->setAction(action);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	void CtaStationReportData::setStaff(std::string staff)
	{
		FUNCTION_ENTRY("setStaff");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getStaff().compare(staff) != 0)
		{
			m_stationReportHelper->setStaff(staff);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	void CtaStationReportData::setIncidentId(const std::string incidentId)
	{
		FUNCTION_ENTRY("setIncidentId");
		
		TA_ASSERT(!incidentId.empty(),"Incident Id cannot be empty");
		
		m_incidentId = incidentId;
		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	void CtaStationReportData::setDescription(const std::string& description)
	{
		FUNCTION_ENTRY("setDescription");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		if (m_stationReportHelper->getDescription().compare(description) != 0)
		{
			m_stationReportHelper->setDescription(description);
			m_isModified = true;
		}
		
		FUNCTION_EXIT;
	}
	
	
	bool CtaStationReportData::isModified()
	{
		FUNCTION_ENTRY("isModified");
		FUNCTION_EXIT;
		return m_isModified;
	}
	
	bool CtaStationReportData::isNew()
	{
		FUNCTION_ENTRY("isNew");
		FUNCTION_EXIT;
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		return m_stationReportHelper->isNew();  
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	std::string CtaStationReportData::getReportId()
	{
		FUNCTION_ENTRY("getReportId");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string reportId = m_stationReportHelper->getReportId();
		
		FUNCTION_EXIT;
		return reportId;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	std::string CtaStationReportData::getSubmittedBy()
	{
		FUNCTION_ENTRY("getSubmittedBy");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string submitter = m_stationReportHelper->getSubmittedBy();
		
		FUNCTION_EXIT;
		return submitter;
	}
	
	std::string CtaStationReportData::getEmployeeParticulars()
	{
		FUNCTION_ENTRY("getEmployeeParticulars");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string ep = m_stationReportHelper->getEmployeeParticulars();
		
		FUNCTION_EXIT;
		return ep;
	}
	
	time_t CtaStationReportData::getDate()
	{
		FUNCTION_ENTRY("getDate");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		time_t incidentDate = m_stationReportHelper->getDate();
		
		FUNCTION_EXIT;
		return incidentDate;
	}
	
	IncidentType CtaStationReportData::getType()
	{
		FUNCTION_ENTRY("getType");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentType type = m_stationReportHelper->getType();
		
		FUNCTION_EXIT;
		return type;
	}
	
	IncidentLocationType CtaStationReportData::getLocation()
	{
		FUNCTION_ENTRY("getLocation");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentLocationType location = m_stationReportHelper->getLocation();
		
		FUNCTION_EXIT;
		return location;
	}
	
	IncidentConditionType CtaStationReportData::getCondition()
	{
		FUNCTION_ENTRY("getCondition");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentConditionType condition = m_stationReportHelper->getCondition();
		
		FUNCTION_EXIT;
		return condition;
	}
	
	IncidentCauseType CtaStationReportData::getCause()
	{
		FUNCTION_ENTRY("getCause");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		IncidentCauseType cause = m_stationReportHelper->getCause();
		
		FUNCTION_EXIT;
		return cause;
	}
	
	std::string CtaStationReportData::getAction()
	{
		FUNCTION_ENTRY("getAction");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string action = m_stationReportHelper->getAction();
		
		FUNCTION_EXIT;
		return action;
	}
	
	std::string CtaStationReportData::getStaff()
	{
		FUNCTION_ENTRY("getStaff");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string staff = m_stationReportHelper->getStaff();
		
		FUNCTION_EXIT;
		return staff;
	}
	
	std::string CtaStationReportData::getDescription()
	{
		FUNCTION_ENTRY("getDescription");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		std::string description = m_stationReportHelper->getDescription();
		
		FUNCTION_EXIT;
		return description;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	std::string CtaStationReportData::getIncidentId()
	{
		FUNCTION_ENTRY("getIncidentId");    
		FUNCTION_EXIT;
		return m_incidentId;
	}
	//Mintao ++
	//Bug 652 (TD12313)
	
	unsigned long CtaStationReportData::getCreatorProfile()
	{
		FUNCTION_ENTRY("getCreatorProfile");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		unsigned long  creatorProfile = m_stationReportHelper->getCreatorProfile();
		
		FUNCTION_EXIT;
		return creatorProfile;
	}
	
	unsigned long CtaStationReportData::getCreateLocationId()
	{
		FUNCTION_ENTRY("getCreateLocationId");
		
		TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
		
		unsigned long  locationId = m_stationReportHelper->getCreateLocationId();
		
		FUNCTION_EXIT;
		return locationId;
	}
	
	void CtaStationReportData::save()
	{
		FUNCTION_ENTRY("save");
		if (isModified() == true)
		{
			
			TA_ASSERT(m_stationReportHelper != NULL, "StationReportHelper is NULL");
			
			m_stationReportHelper->writeStationReportData();    
		}
		//reset modified flag
		m_isModified = false;
		FUNCTION_EXIT;
		
	}
	
	//Shiva ++
	//DTL-1174 - display of 7 digit number
	std::string CtaStationReportData::getDisplayReportId()
	{
		FUNCTION_ENTRY("getDisplayReportId");
		TA_ASSERT(m_stationReportHelper != NULL, "Helper is NULL");	
		std::string reportId = m_stationReportHelper->getReportId();
		std::string displayReportId = "";
		if(reportId != "")
		{
			std::size_t index = reportId.find("-");
			if (index != std::string::npos)
			{
				displayReportId = reportId.substr (0,index);   
			}
			//old format support
			if(displayReportId.length() != 7)
			{
				displayReportId = "00" + displayReportId;
			}
		}
		 FUNCTION_EXIT;
		 return displayReportId;
	}

}// end TA_IRS_Core

