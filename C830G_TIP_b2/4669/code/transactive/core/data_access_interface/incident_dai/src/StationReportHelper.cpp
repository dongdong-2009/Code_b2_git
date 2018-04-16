/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/StationReportHelper.cpp $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * StationReportHelper is an object that is held by CtaIncidentReportData objects. 
  * It contains all data used by IncidentReport, and manipulation
  * methods for the data. It helps avoid re-writing code for both CtaIncidentReportData
  */
#include <time.h>
#include <sstream>
#include <iomanip>

#include "core/data_access_interface/incident_dai/src/StationReportHelper.h"

#include "ace/OS.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/ISession.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
//Mintao ++
//Bug 652 (TD12313)
#include "core/uuid/src/TAuuid.h"
//Mintao ++
//Bug 652 (TD12313)

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/IncidentException.h"

namespace TA_IRS_Core
{
    //Mintao++
    //for archive manager
    static const std::string IN_INCIDENT_CLOSED_DATE_COL        = "INCIDENT_CLOSED_DATE";
    //Mintao++
	static const std::string IN_STATION_ID_COL	        = "INSTAT_ID";
	static const std::string IN_SUBMITTED_BY_COL        = "SUBMITTED_BY";
	static const std::string IN_EP_COL                  = "EP";
	static const std::string IN_INCIDENTDATE_COL        = "INCIDENT_DATE";
	static const std::string IN_TYPE_COL                = "INCIDENT_TYPE";
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
	static const std::string IN_CREATE_SEPROF_ID_COL    = "CREATE_SEPROF_ID";

	static const std::string INCIDENT_CAUSE_TYPE_BLS    = "BLS";
	static const std::string INCIDENT_CAUSE_TYPE_ESP    = "ESP";
	static const std::string INCIDENT_CAUSE_TYPE_PAX    = "PAX";
	static const std::string INCIDENT_CAUSE_TYPE_POINT  = "Point";
	static const std::string INCIDENT_CAUSE_TYPE_POWER  = "Power";
	static const std::string INCIDENT_CAUSE_TYPE_SIGNAL = "Signal";
	static const std::string INCIDENT_CAUSE_TYPE_TKCCT  = "Tk Cct";

	static const std::string INCIDENT_LOCATION_DEPOT        = "Depot";
	static const std::string INCIDENT_LOCATION_STATION      = "Station";
	static const std::string INCIDENT_LOCATION_TRACKSIDE    = "Trackside";
	static const std::string INCIDENT_LOCATION_TRAIN        = "Train";

	static const std::string INCIDENT_CONDITION_CLEAN       = "Clean";
	static const std::string INCIDENT_CONDITION_DIRTY       = "Dirty";
	static const std::string INCIDENT_CONDITION_DRY         = "Dry";
	static const std::string INCIDENT_CONDITION_POORLYLIT   = "Poorly Lit";
	static const std::string INCIDENT_CONDITION_WELLLIT     = "Well Lit";
	static const std::string INCIDENT_CONDITION_WET         = "Wet";

	static const std::string INCIDENT_TYPE_ARRESTOFPERSION  = "Arrest of Person";
	static const std::string INCIDENT_TYPE_ASSAULT          = "Assault";
	static const std::string INCIDENT_TYPE_CRIMINALACT      = "Criminal Act";
	static const std::string INCIDENT_TYPE_DEMAGEFACPROP    = "Demaged Facility/Property";
	static const std::string INCIDENT_TYPE_DERAILMENT       = "Derailment";
	static const std::string INCIDENT_TYPE_EQUIPABNORMALLY  = "Equipment Abnormally";
	static const std::string INCIDENT_TYPE_EQUIPMALF        = "Equipment Malfunction";
	static const std::string INCIDENT_TYPE_FIRE             = "Fire";
	static const std::string INCIDENT_TYPE_HEATEDARGUMENT   = "Heated Argument";
	static const std::string INCIDENT_TYPE_TRAINDELAY       = "Train Service Delay";
	static const std::string INCIDENT_TYPE_TRESPASSING      = "Traspassing";

	const short StationReportHelper::INCIDENTCAUSE_LENGTH       = 7;
	const short StationReportHelper::INCIDENTLOCATION_LENGTH    = 4;
	const short StationReportHelper::INCIDENTCONDITION_LENGTH   = 6;
	const short StationReportHelper::INCIDENTTYPE_LENGTH        = 11;


	StationReportHelper::StationReportHelper() :
			m_submittedBy(""),
			m_ep(""),
			m_incidentDate(0),
            m_incidentClosedDate(0),
			m_action(""),
			m_staff(""),
			m_description(""),
			m_creatorProfile(0),
			m_createLocationId(-1),
			m_isValidData(false),
			m_isNew(true)
	{
		FUNCTION_ENTRY("default constructor");
		//Mintao ++
		//Bug 652 (TD12313)
		// m_reportId = TA_Base_Core::TA_Uuid::generateUUID(); // TD15204
		//Mintao ++
		//Bug 652 (TD12313)

		//Bugzilla Bug 152: Mintao
		//initiliazed
		m_type.arrestOfPerson = false;
		m_type.assault = false;
		m_type.criminalAct = false;
		m_type.damagedFacilityOrProperty = false;
		m_type.derailment = false;
		m_type.equipmentAbnormally = false;
		m_type.equipmentMalfunction = false;
		m_type.fire = false;
		m_type.heatedArgument = false;
		m_type.other = "";
		m_type.trainServiceDelay = false;
		m_type.trespassing = false;
    
		m_location.depot = false;
		m_location.other = "";
		m_location.station = false;
		m_location.trackside = false;
		m_location.train = false;

		m_cause.bLS = false;
		m_cause.eSP = false;
		m_cause.other = "";
		m_cause.pax = false;
		m_cause.point = false;
		m_cause.power = false;
		m_cause.signal = false;
		m_cause.tkCct = false;

		m_condition.clean = false;
		m_condition.dirty = false;
		m_condition.dry = false;
		m_condition.other = "";
		m_condition.poorlyLit = false;
		m_condition.wellLit = false;
		m_condition.wet = false;

		std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

		try
		{
			TA_Base_Core::ISession* session = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);

			m_creatorProfile = session->getProfileKey();
			m_createLocationId = session->getLocationKey();
		}
		catch (std::exception&)
		{
			//TODO: temporary catching exception
			m_creatorProfile = 1;
			m_createLocationId = 0;
		}


		FUNCTION_EXIT;
	}

	//Mintao ++
	//Bug 652 (TD12313)
			StationReportHelper::StationReportHelper(const std::string idKey) :
			m_reportId(idKey),
			m_submittedBy(""),
			m_ep(""),
			m_incidentDate(0),
            m_incidentClosedDate(0),
			m_action(""),
			m_staff(""),
			m_description(""),
			m_creatorProfile(0),
			m_createLocationId(-1),
			m_isValidData(false),
			m_isNew(false)
	{
		FUNCTION_ENTRY("User Constructor 1");

		//Bugzilla Bug 152: Mintao
		//initiliazed
		m_type.arrestOfPerson = false;
		m_type.assault = false;
		m_type.criminalAct = false;
		m_type.damagedFacilityOrProperty = false;
		m_type.derailment = false;
		m_type.equipmentAbnormally = false;
		m_type.equipmentMalfunction = false;
		m_type.fire = false;
		m_type.heatedArgument = false;
		m_type.other = "";
		m_type.trainServiceDelay = false;
		m_type.trespassing = false;
    
		m_location.depot = false;
		m_location.other = "";
		m_location.station = false;
		m_location.trackside = false;
		m_location.train = false;

		m_cause.bLS = false;
		m_cause.eSP = false;
		m_cause.other = "";
		m_cause.pax = false;
		m_cause.point = false;
		m_cause.power = false;
		m_cause.signal = false;
		m_cause.tkCct = false;

		m_condition.clean = false;
		m_condition.dirty = false;
		m_condition.dry = false;
		m_condition.other = "";
		m_condition.poorlyLit = false;
		m_condition.wellLit = false;
		m_condition.wet = false;

		FUNCTION_EXIT;
	}
	//Mintao ++
	//Bug 652 (TD12313)

	StationReportHelper::StationReportHelper( const StationReportHelper& theStationReportHelper) :
			m_submittedBy(theStationReportHelper.m_submittedBy),
			m_ep(theStationReportHelper.m_ep),
			m_incidentDate(theStationReportHelper.m_incidentDate),
            m_incidentClosedDate(theStationReportHelper.m_incidentClosedDate),
			m_action(theStationReportHelper.m_action),
			m_staff(theStationReportHelper.m_staff),
			m_description(theStationReportHelper.m_description),
			m_createLocationId(theStationReportHelper.m_createLocationId),
			m_creatorProfile(theStationReportHelper.m_creatorProfile),
			m_isValidData(false),
			m_isNew(true),
			m_type(theStationReportHelper.m_type),
			m_location(theStationReportHelper.m_location),
			m_cause(theStationReportHelper.m_cause),
			m_condition(theStationReportHelper.m_condition)
	{
		FUNCTION_ENTRY("User Constructor 2");
		//Mintao ++
		//Bug 652 (TD12313)
		// m_reportId = TA_Base_Core::TA_Uuid::generateUUID(); //TD15204
		//Mintao ++
		//Bug 652 (TD12313)
		FUNCTION_EXIT;
	}

	StationReportHelper::StationReportHelper(std::string submitter, std::string employeeParticulars, 
			time_t date, IncidentType type, IncidentLocationType location, 
			IncidentConditionType condition, IncidentCauseType cause, 
			std::string action, std::string staff, std::string description, 
			unsigned long createLocationId) :
			m_submittedBy(submitter),
			m_ep(employeeParticulars),
			m_incidentDate(date),
            m_incidentClosedDate(0),
			m_type(type),
			m_location(location),
			m_condition(condition),
			m_cause(cause),
			m_action(""),
			m_staff(""),
			m_description(""),
			m_creatorProfile(0),
			m_createLocationId(createLocationId),
			m_isValidData(false),
			m_isNew(true)
	{
		FUNCTION_ENTRY("user constructor 4");

		//Mintao ++
		//Bug 652 (TD12313)
		// m_reportId = TA_Base_Core::TA_Uuid::generateUUID(); //TD15204
		//Mintao ++
		//Bug 652 (TD12313)

		FUNCTION_EXIT;
	}

	StationReportHelper::~StationReportHelper()
	{
		FUNCTION_ENTRY("destructor");
        FUNCTION_EXIT;
    }
    
    //this field is not going to be used by the incident manager
    //this is a workaround for the archive manager
	void StationReportHelper::setClosedDate(time_t date)
    {
		FUNCTION_ENTRY("setClosedDate");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		m_incidentClosedDate = date;
    }

	time_t StationReportHelper::getClosedDate()
    {
		FUNCTION_ENTRY("getClosedDate");
		
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_incidentClosedDate;
    }

    void StationReportHelper::writeStationReportData()
	{
		FUNCTION_ENTRY("writeStationReportData");

		// Check all values exist. If not add them to the dataConfiguratonException
		std::vector<std::string> fieldNames;
		//incident date
		if (m_incidentDate == 0)
		{
			fieldNames.push_back(IN_INCIDENTDATE_COL);
		}

		if ( !fieldNames.empty() ) // Need to throw an exception
		{
			std::ostringstream reasonMessage;
			reasonMessage << "StationReport data not fully specified. Data cannot be written to database. Missing data = ";

			std::vector<std::string>::iterator itr;        
			for (itr = fieldNames.begin(); itr != fieldNames.end(); itr++)
			{       
				reasonMessage << (*itr) << ", ";
			}

			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATA_ERROR) );
		}

		// Now write to database

		// Need to check if this is a new record or an existing record - 
		// as a new record will be inserted, while an existing record will be updated
		if (m_isNew) // This is a new record
		{
			addNewRecord();
		}
		else // This is an existing record
		{
			modifyExistingRecord();
		}

		// Now that the data has been written, what we hold is valid data.
		m_isValidData = true;
		m_isNew = false;

		FUNCTION_EXIT;
	}

	// all the get methods
	std::string StationReportHelper::getAction()
	{
		FUNCTION_ENTRY("getAction");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_action;
	}

	IncidentCauseType StationReportHelper::getCause()
	{
		FUNCTION_ENTRY("getCause");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_cause;
	}

	IncidentLocationType StationReportHelper::getLocation()
	{
		FUNCTION_ENTRY("getLocation");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_location;
	}


	time_t StationReportHelper::getDate()
	{
		FUNCTION_ENTRY("getDate");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_incidentDate;
	}

	std::string StationReportHelper::getDescription()
	{
		FUNCTION_ENTRY("getDescription");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_description;
	}


	std::string StationReportHelper::getEmployeeParticulars()
	{
		FUNCTION_ENTRY("getEmployeeParticulars");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_ep;
	}

	IncidentConditionType  StationReportHelper::getCondition()
	{
		FUNCTION_ENTRY("getCondition");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_condition;
	}

	//Mintao ++
	//Bug 652 (TD12313)
	std::string StationReportHelper::getReportId()
	{
		FUNCTION_ENTRY("getReportId");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_reportId;
	}
	//Mintao ++
	//Bug 652 (TD12313)

	std::string StationReportHelper::getStaff()
	{
		FUNCTION_ENTRY("getStaff");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_staff;
	}
	std::string StationReportHelper::getSubmittedBy()
	{
		FUNCTION_ENTRY("getSubmittedBy");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_submittedBy;
	}

	IncidentType StationReportHelper::getType()
	{
		FUNCTION_ENTRY("getType");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_type;
	}

	unsigned long  StationReportHelper::getCreatorProfile()
	{
		FUNCTION_ENTRY("getCreatorProfile");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_creatorProfile;
	}

	unsigned long StationReportHelper::getCreateLocationId()
	{
		FUNCTION_ENTRY("getCreateLocationId");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		return m_createLocationId;
	}

	void StationReportHelper::getDateData(TA_Base_Core::IData* data, const unsigned long rowIndex, const std::string& fieldName, time_t& out)
	{
		try
		{
			if (data == NULL)
			{
				std::ostringstream reasonMessage;
				reasonMessage << "Unable to execute Query";
				TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATA_ERROR) );
			}

			out = data->getDateData( rowIndex, fieldName);
			struct tm newtm;

			// Set up the date created structure to equal the value we set it to when we get NULL
			// from the database
			newtm.tm_sec = 0;		//seconds
			newtm.tm_min = 0;		// minutes 
			newtm.tm_hour = 12;		// hours 
			newtm.tm_mday = 1;		// day of the month 
			newtm.tm_mon = 0;	// month of Year - goes from 0 to 11
			newtm.tm_year = 90;	// years since 1900 - this structure only deals with years 
										// between 1901 and 2038
			newtm.tm_isdst = 0;			// disable daylight savings - this could have implications

			time_t value;
			value = mktime(&newtm);

			if (value == out) // The date was actually NULL
			{
				out = 0;
			}
		}
		catch (TA_Base_Core::DataException& ex)
		{
			//If we catch this exception, need to check if its a "NO_VALUE" type
			if (TA_Base_Core::DataException::NO_VALUE != ex.getFailType())
			{   // Need to on-throw this exception
				throw;
			}
			else // The date-modified column was null - this is not fatal
			{
				out = 0;
			}
		}
	}


	void StationReportHelper::invalidate()
	{
		FUNCTION_ENTRY("invalidate" );

		// This method cannot be called until the key has been set
		TA_ASSERT(!m_isNew,"Attempted to call invalidate() on a new StationReport");

		m_isValidData = false;

		FUNCTION_EXIT;
	}

	bool StationReportHelper::isNew() const
	{
		FUNCTION_ENTRY("isNew" );
		FUNCTION_EXIT;
		return m_isNew;
	}

	//all the set method
	void StationReportHelper::setAction(const std::string& action)
	{
		FUNCTION_ENTRY("setAction");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_action = action;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setCause(const IncidentCauseType cause)
	{
		FUNCTION_ENTRY("setCause");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_cause = cause;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setLocation(const IncidentLocationType location)
	{
		FUNCTION_ENTRY("setLocation");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_location = location;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setDate(const time_t date)
	{
		FUNCTION_ENTRY("setDate");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_incidentDate = date;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setDescription(const std::string desc)
	{
		FUNCTION_ENTRY("setDescription");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_description = desc;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setEmployeeParticulars(const std::string ep)
	{
		FUNCTION_ENTRY("setDescription");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		m_ep = ep;
		FUNCTION_EXIT;
	}

	void StationReportHelper::setCondition(const IncidentConditionType condition)
	{
		FUNCTION_ENTRY("setCondition");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		m_condition = condition;
	}


	void StationReportHelper::setStaff(const std::string staff)
	{
		FUNCTION_ENTRY("setStaff");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		m_staff = staff;
	}

	void StationReportHelper::setSubmittedBy(const std::string submitter)
	{
		FUNCTION_ENTRY("setSubmittedBy");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		m_submittedBy = submitter;
	}

	void StationReportHelper::setType(const IncidentType type)
	{
		FUNCTION_ENTRY("setType");
		if (!m_isValidData && !m_isNew)
		{
			reload();
		}
		FUNCTION_EXIT;
		m_type = type;
	}



	//all the private methods
	void StationReportHelper::reload()
	{
		FUNCTION_ENTRY("reload");

		// Ensure the Station report has already been written to the database. It doesn't make
		// any sense to reload an Station Report that doesn't exist in the database.
		TA_ASSERT(!m_isNew, "This Station Report does not yet exist in the database. Therefore it cannot be reloaded.");

		// get a connection to the database
		TA_Base_Core::IDatabase* databaseConnection = 
				TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Read);

		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot connect to database";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
		}

		// create the SQL string to retrieve the data of the Station Report
		// based upon the key
		// create the SQL string to retrieve the data of the StationReportData
		std::ostringstream sql;
		sql << "select " 
			<< IN_SUBMITTED_BY_COL      << ", " 
			<< IN_EP_COL                << ", "
			<< "TO_CHAR(" << IN_INCIDENTDATE_COL << ",'YYYYMMDDHH24MMSS')" << ", " 
			<< IN_TYPE_COL              << ", " 
			<< IN_OTHERTYPE_COL         << ", " 
			<< IN_LOCATION_COL          << ", " 
			<< IN_OTHERLOCATION_COL     << ", " 
			<< IN_CAUSE_COL             << ", " 
			<< IN_OTHERCAUSE_COL        << ", " 
			<< IN_CONDITION_COL         << ", " 
			<< IN_OTHERCONDITION_COL    << ", " 
			<< IN_ACTION_COL            << ", " 
			<< IN_STAFF_COL             << ", " 
			<< IN_DESCRIPTION_COL       << ", " 
			<< IN_CREATELOCATION_ID_COL << ", "
			<< IN_CREATE_SEPROF_ID_COL  << ", "
            << "TO_CHAR(" << IN_INCIDENT_CLOSED_DATE_COL  << ",'YYYYMMDDHH24MMSS')"
			<< " from IN_STATION_REPORT where "
			<< IN_STATION_ID_COL << " = '" 
			<< m_reportId << "'";

		// Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		columnNames.push_back(IN_SUBMITTED_BY_COL);
		columnNames.push_back(IN_EP_COL);
		columnNames.push_back(IN_INCIDENTDATE_COL);
		columnNames.push_back(IN_TYPE_COL);
		columnNames.push_back(IN_OTHERTYPE_COL);
		columnNames.push_back(IN_LOCATION_COL);
		columnNames.push_back(IN_OTHERLOCATION_COL);
		columnNames.push_back(IN_CAUSE_COL);
		columnNames.push_back(IN_OTHERCAUSE_COL);
		columnNames.push_back(IN_CONDITION_COL);
		columnNames.push_back(IN_OTHERCONDITION_COL);
		columnNames.push_back(IN_ACTION_COL);
		columnNames.push_back(IN_STAFF_COL);
		columnNames.push_back(IN_DESCRIPTION_COL);
		columnNames.push_back(IN_CREATELOCATION_ID_COL);
		columnNames.push_back(IN_CREATE_SEPROF_ID_COL);
		columnNames.push_back(IN_INCIDENT_CLOSED_DATE_COL);

		// Execute the query. The method can throw a DatabaseException.
		// This is documented in the comment of this method.
		// We are responsible for deleting the returned IData object when we're done with it
		TA_Base_Core::IData* data = databaseConnection->executeQuery(sql.str(),columnNames);

		if (data == NULL)
		{
			std::ostringstream reasonMessage;
			reasonMessage << "Unable to execute Query";
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATA_ERROR) );
		}

		if (0 == data->getNumRows()) 
		{
			// no record found
        
			delete data;
			data = NULL;

			std::ostringstream reasonMessage;
			reasonMessage << "No data found for station Id = " << m_reportId;			
			TA_THROW( TA_Base_Core::DataException(reasonMessage.str().c_str(),TA_Base_Core::DataException::NO_VALUE,"station Id" ) );
		}
    
		// Assign the data as appropriate to the member variables.
		// These calls can throw DataExceptions, as documented in the comment of this method.
		m_submittedBy       = data->getStringData(0, IN_SUBMITTED_BY_COL);
		m_ep                = data->getStringData(0, IN_EP_COL);

		//get incident date
		getDateData(data, 0, IN_INCIDENTDATE_COL, m_incidentDate);

		//convert type to IncidentType
		std::string type            = data->getStringData(0, IN_TYPE_COL);
		std::string otherType       = data->getStringData(0, IN_OTHERTYPE_COL);
		m_type                      = convertBitString2Type(type, otherType);

		//convert location to IncidentLocationType
		std::string location        = data->getStringData(0, IN_LOCATION_COL);
		std::string otherLocation   = data->getStringData(0, IN_OTHERLOCATION_COL);
		m_location                  = convertBitString2Location(location, otherLocation);

		//convert cause to IncidentCauseType   
		std::string cause             = data->getStringData(0, IN_CAUSE_COL);
		std::string otherCause        = data->getStringData(0, IN_OTHERCAUSE_COL);
		m_cause = convertBitString2Cause(cause, otherCause);

		//convert condition to IncidentConditionType
		std::string condition       = data->getStringData(0, IN_CONDITION_COL);
		std::string otherCondition  = data->getStringData(0, IN_OTHERCONDITION_COL);
		m_condition                 = convertBitString2Condition(condition, otherCondition);

		m_action            = data->getStringData(0, IN_ACTION_COL);
		m_staff             = data->getStringData(0, IN_STAFF_COL);
		m_description       = data->getStringData(0, IN_DESCRIPTION_COL);
		m_createLocationId  = data->getUnsignedLongData(0, IN_CREATELOCATION_ID_COL);
		m_creatorProfile    = data->getUnsignedLongData(0, IN_CREATE_SEPROF_ID_COL);

		//get incident date
		getDateData(data, 0, IN_INCIDENT_CLOSED_DATE_COL, m_incidentClosedDate);

		// Now that we're done with the IData object, it's our responsibility to delete it
		delete data;
		data = NULL;
    
		// Need to record that we now have valid data
		m_isValidData = true;
 
		FUNCTION_EXIT;
	}

	void StationReportHelper::deleteRecord()
	{
		FUNCTION_ENTRY("deleteRecord");

		TA_ASSERT(!m_isNew, "This Station Report does not yet exist in the database, and therefore cannot be deleted");
    
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);

		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot connect to database";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
		}

		std::ostringstream sql;    
		sql.str("");
		sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "delete IN_STATION_REPORT where " << IN_STATION_ID_COL << " = ''" << m_reportId << "''";
		sql << "');";
		sql << "END;";

		databaseConnection->executeModification(sql.str());

		FUNCTION_EXIT;
	}

	void StationReportHelper::addNewRecord()
	{
		FUNCTION_ENTRY("addNewRecord");
    
	//Mintao ++
	//Bug 652 (TD12313)
		if (m_reportId.empty())
		{
			// m_reportId = TA_Base_Core::TA_Uuid::generateUUID(); // 15204
		}
	//Mintao ++
	//Bug 652 (TD12313)

		LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
			"Station Report created by %s is a new Station Report. It will be added to the database.", m_submittedBy.c_str());//TD14337 yezh++

		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);

		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot connect to database";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
		}


		////////////////////////////////////////////////////////////////////////////
		//
		// Good to go...
		//
		////////////////////////////////////////////////////////////////////////////
		std::string type        = convertType2BitString(); //convert type to bit-string to save to database
		std::string location    = convertLocation2BitString(); //convert location to bit-string to save to database
		std::string cause       = convertCause2BitString();    //convert cause to bit-string to save to database
		std::string condition   = convertCondition2BitString(); //convert condtiont to bit-string to save to database
		std::string incidentDateString = convertIncidentDate2Tmstruct(); //conver to incident date to tm struct to save to database
        std::string incidentCloseDateString = convertIncidentClosedDate2Tmstruct(); ///conver to incident closed date to tm struct to save to database
		//Bugzilla Bug 405: Mintao
		//breaking down the sql statement into a few section
		std::ostringstream sql;
		sql << "BEGIN ";
    		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "insert into IN_STATION_REPORT ("
			<< IN_STATION_ID_COL        << ", "
			<< IN_SUBMITTED_BY_COL      << ", "
			<< IN_EP_COL                << ", "
			<< IN_INCIDENTDATE_COL      << ", "
			<< IN_TYPE_COL              << ", "
			<< IN_OTHERTYPE_COL         << ", "
			<< IN_LOCATION_COL          << ", "
			<< IN_OTHERLOCATION_COL     << ", "
			<< IN_CAUSE_COL             << ", ";

		sql << IN_OTHERCAUSE_COL        << ", "
			<< IN_CONDITION_COL         << ", "
			<< IN_OTHERCONDITION_COL    << ", "
			<< IN_ACTION_COL            << ", "
			<< IN_STAFF_COL             << ", "
			<< IN_DESCRIPTION_COL       << ", "
			<< IN_CREATELOCATION_ID_COL << ", "
			<< IN_CREATE_SEPROF_ID_COL  << ", "
			<< IN_INCIDENT_CLOSED_DATE_COL  << ") ";

		sql << "values ("
			<< "''" << m_reportId << "'', " 
			<< "''" << (databaseConnection->escapeAQSQLString(m_submittedBy))       << "'', "        
			<< "''" << (databaseConnection->escapeAQSQLString(m_ep))                << "'', " 
			<< "TO_DATE(''" << incidentDateString << "'',''dd/mm/yyyy hh24:mi:ss''), "
			<< "''" << (databaseConnection->escapeAQSQLString(type))                << "'', " 
			<< "''" << (databaseConnection->escapeAQSQLString(m_type.other))        << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(location))            << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_location.other))    << "'', ";

		sql << "''" << (databaseConnection->escapeAQSQLString(cause))               << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_cause.other))       << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(condition))           << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_condition.other))   << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_action))            << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_staff))             << "'', "
			<< "''" << (databaseConnection->escapeAQSQLString(m_description))       << "'', "
			<< m_createLocationId << ", "
			<< m_creatorProfile << ", "
			<< "TO_DATE(''" << incidentCloseDateString << "'',''dd/mm/yyyy hh24:mi:ss'')"
			<< ")";
		sql << "');";
		sql << "END;";

		databaseConnection->executeModification(sql.str());

		FUNCTION_EXIT;
	}

	void StationReportHelper::modifyExistingRecord()
	{
		FUNCTION_ENTRY("modifyExistingRecord");

		LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
			"Station Report modified by %s already exists. It's data will be updated.",m_submittedBy.c_str());//TD14337 yezh++
    
		TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);

		////////////////////////////////////////////////////////////////////////////
		//
		// Good to go...
		//
		////////////////////////////////////////////////////////////////////////////
		//Bugzilla Bug 406: Mintao
		//breaking down the sql statement into a few section
		std::ostringstream sql;
		sql << "BEGIN ";
		sql << "audit_data_queue_pkg.enqueue_audit_data('" << getLocalDatabaseName() << "','PUBLIC', '";
		sql << "update IN_STATION_REPORT set " 
			<< IN_SUBMITTED_BY_COL                                                  << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_submittedBy))              << "'', "
			<< IN_EP_COL                                                            << " = ''"
			<< (databaseConnection->escapeAQSQLString(m_ep))                       << "'', "
			<< IN_INCIDENTDATE_COL                                                  << " = "    
			<< "TO_DATE(''" << convertIncidentDate2Tmstruct() << "'',''dd/mm/yyyy hh24:mi:ss''), ";

		sql << IN_TYPE_COL                                                          << " = ''"   
			<< (databaseConnection->escapeAQSQLString(convertType2BitString()))    << "'', "
			<< IN_OTHERTYPE_COL                                                     << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_type.other))               << "'', "
			<< IN_LOCATION_COL                                                      << " = ''"   
			<< (databaseConnection->escapeAQSQLString(convertLocation2BitString()))  << "'', "
			<< IN_OTHERLOCATION_COL                                                 << " = ''" ;

    sql << (databaseConnection->escapeAQSQLString(m_location.other))           << "'', "
			<< IN_CAUSE_COL                                                         << " = ''"   
			<< (databaseConnection->escapeAQSQLString(convertCause2BitString()))   << "'', "
			<< IN_OTHERCAUSE_COL                                                    << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_cause.other))              << "'', ";

		// TD14437 - lizettejl
		// sql << IN_CONDITION_COL                                                     << " = '"   
		sql << IN_CONDITION_COL                                                     << " = ''"   
			<< (databaseConnection->escapeAQSQLString(convertCondition2BitString()))  << "'', "
			<< IN_OTHERCONDITION_COL                                                << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_condition.other))          << "'', "
			<< IN_ACTION_COL                                                        << " = ''" ;

    sql << (databaseConnection->escapeAQSQLString(m_action))                   << "'', "
			<< IN_STAFF_COL                                                         << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_staff))                    << "'', "
			<< IN_DESCRIPTION_COL                                                   << " = ''"   
			<< (databaseConnection->escapeAQSQLString(m_description))              << "'', ";

	sql		<< IN_INCIDENT_CLOSED_DATE_COL                                                  << " = "    
			<< "TO_DATE(''" << convertIncidentClosedDate2Tmstruct() << "'',''dd/mm/yyyy hh24:mi:ss'')";

		sql << " where " 
			<< IN_STATION_ID_COL                                                    << " = ''"    
			<< m_reportId << "''";
		sql << "');";
		sql << "END;";


		if (databaseConnection == NULL)
		{
			//cannot connect to database, throw IncidentException
			std::ostringstream reasonMessage;
			reasonMessage << "Cannot connect to database";                        
			TA_THROW( TA_Base_Core::IncidentException(reasonMessage.str(),TA_Base_Core::IncidentException::DATABASE_ERROR) );
		}

		databaseConnection->executeModification(sql.str());

		FUNCTION_EXIT;
	}

	IncidentCauseType StationReportHelper::convertBitString2Cause(const std::string cause, const std::string otherCause)
	{
		FUNCTION_ENTRY("convertBitString2Cause");

		IncidentCauseType toReturn;
		toReturn.eSP    = false;
		toReturn.eSP    = false;
		toReturn.pax    = false;
		toReturn.point  = false;
		toReturn.power  = false;
		toReturn.signal = false;
		toReturn.tkCct  = false;
		toReturn.other  = "";

		//cause parameter is less than INCIDENTCAUSE_LENGTH, then return default value.
		if (cause.length() >= INCIDENTCAUSE_LENGTH)
		{
			toReturn.eSP    = (cause[0] == '1');
			toReturn.bLS    = (cause[1] == '1');
			toReturn.pax    = (cause[2] == '1');
			toReturn.point  = (cause[3] == '1');
			toReturn.power  = (cause[4] == '1');
			toReturn.signal = (cause[5] == '1');
			toReturn.tkCct  = (cause[6] == '1');
			toReturn.other = otherCause;
		}
		FUNCTION_EXIT;
		return toReturn;
	}

	IncidentLocationType StationReportHelper::convertBitString2Location(const std::string location, const std::string otherLocation)
	{
		FUNCTION_ENTRY("convertBitString2Location");

		IncidentLocationType toReturn;
		toReturn.train      = false;
		toReturn.trackside  = false;
		toReturn.station    = false;
		toReturn.depot      = false;
		toReturn.other      = "";
		//location parameter is less than INCIDENTLOCATION_LENGTH, then return default value.
		if (location.length() >= INCIDENTLOCATION_LENGTH)
		{
			toReturn.train      = (location[0] == '1');
			toReturn.trackside  = (location[1] == '1');
			toReturn.station    = (location[2] == '1');
			toReturn.depot      = (location[3] == '1');
			toReturn.other      = otherLocation;
		}
		FUNCTION_EXIT;
		return toReturn;
	}

	IncidentConditionType StationReportHelper::convertBitString2Condition(const std::string condition, 
			const std::string otherCondition)
	{
		FUNCTION_ENTRY("convertBitString2Condition");

		IncidentConditionType toReturn;
		toReturn.dry        = false;
		toReturn.wet        = false;
		toReturn.dirty      = false;
		toReturn.clean      = false;
		toReturn.wellLit    = false;
		toReturn.poorlyLit  = false;
		toReturn.other      = "";

		//condition parameter is less than INCIDENTCONDITION_LENGTH, then return default value.
		if (condition.length() >= INCIDENTCONDITION_LENGTH)
		{
			toReturn.dry        = (condition[0] == '1');
			toReturn.wet        = (condition[1] == '1');
			toReturn.clean      = (condition[2] == '1');
			toReturn.dirty      = (condition[3] == '1');
			toReturn.wellLit    = (condition[4] == '1');
			toReturn.poorlyLit  = (condition[5] == '1');
			toReturn.other = otherCondition;
		}
		FUNCTION_EXIT;
		return toReturn;

	}

	IncidentType StationReportHelper::convertBitString2Type(const std::string type, const std::string otherType)
	{
		FUNCTION_ENTRY("convertBitString2Type");

		IncidentType toReturn;
		toReturn.trainServiceDelay          = false;
		toReturn.derailment                 = false;
		toReturn.damagedFacilityOrProperty  = false;
		toReturn.fire                       = false;
		toReturn.equipmentAbnormally        = false;
		toReturn.equipmentMalfunction       = false;
		toReturn.trespassing                = false;
		toReturn.criminalAct                = false;
		toReturn.assault                    = false;
		toReturn.arrestOfPerson             = false;
		toReturn.heatedArgument             = false;
		toReturn.other                      = otherType;

		//type parameter is less than INCIDENTTYPE_LENGTH, then return default value.
		if (type.length() >= INCIDENTTYPE_LENGTH)
		{
			toReturn.trainServiceDelay          = (type[0] == '1');
			toReturn.derailment                 = (type[1] == '1');
			toReturn.damagedFacilityOrProperty  = (type[2] == '1');
			toReturn.fire                       = (type[3] == '1');
			toReturn.equipmentAbnormally        = (type[4] == '1');
			toReturn.equipmentMalfunction       = (type[5] == '1');
			toReturn.trespassing                = (type[6] == '1');
			toReturn.criminalAct                = (type[7] == '1');
			toReturn.assault                    = (type[8] == '1');
			toReturn.arrestOfPerson             = (type[9] == '1');
			toReturn.heatedArgument             = (type[10] == '1');
			toReturn.other                      = otherType;
		}
		FUNCTION_EXIT;
		return toReturn;
	}

	std::string StationReportHelper::convertType2BitString()
	{
		FUNCTION_ENTRY("convertType2BitString");
    
		std::ostringstream toReturn;
		toReturn << m_type.trainServiceDelay 
				<< m_type.derailment
				<< m_type.damagedFacilityOrProperty
				<< m_type.fire
				<< m_type.equipmentAbnormally
				<< m_type.equipmentMalfunction
				<< m_type.trespassing
				<< m_type.criminalAct
				<< m_type.assault
				<< m_type.arrestOfPerson
				<< m_type.heatedArgument;

		FUNCTION_EXIT;
		return toReturn.str();
	}

	std::string StationReportHelper::convertLocation2BitString()
	{
		FUNCTION_ENTRY("convertLocation2BitString");

		std::ostringstream toReturn;
		toReturn << m_location.train
				<< m_location.trackside
				<< m_location.station
				<< m_location.depot;

		FUNCTION_EXIT;
		return toReturn.str();
	}

	std::string StationReportHelper::convertCause2BitString()
	{
		FUNCTION_ENTRY("convertCause2BitString");

		std::ostringstream toReturn;
		toReturn << m_cause.eSP
				<<m_cause.bLS
				<< m_cause.pax
				<< m_cause.point
				<< m_cause.power
				<< m_cause.signal
				<< m_cause.tkCct;

		FUNCTION_EXIT;
		return toReturn.str();
	}

	std::string StationReportHelper::convertCondition2BitString()
	{
		FUNCTION_ENTRY("convertCondition2BitString");

		std::stringstream toReturn;
		toReturn << m_condition.dry
					<< m_condition.wet
					<< m_condition.clean
					<< m_condition.dirty
					<< m_condition.wellLit
					<< m_condition.poorlyLit;

		FUNCTION_EXIT;
		return toReturn.str();
	}

	std::string StationReportHelper::convertIncidentDate2Tmstruct()
	{
		FUNCTION_ENTRY("convertIncidentDate2Tmstruct");
		
		struct tm incidentDateStruct;
		std::ostringstream incidentDataStream;

		if (ACE_OS::localtime_r(&m_incidentDate, &incidentDateStruct) != NULL)
		{
			incidentDataStream << std::setfill('0')
					<< std::setw(2) << incidentDateStruct.tm_mday
					<< '/'
					<< std::setw(2) << incidentDateStruct.tm_mon + 1
					<< '/'
					<< std::setw(4) << incidentDateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
					<< ' '
					<< std::setw(2) << incidentDateStruct.tm_hour
					<< ':'
					<< std::setw(2) << incidentDateStruct.tm_min 
					<< ':'
					<< std::setw(2) << incidentDateStruct.tm_sec;
		}

		FUNCTION_EXIT;
		return incidentDataStream.str();
	}

	std::string StationReportHelper::convertIncidentClosedDate2Tmstruct()
	{
		FUNCTION_ENTRY("convertIncidentDate2Tmstruct");
		
		struct tm incidentDateStruct;
		std::ostringstream incidentDataStream;

		if (ACE_OS::localtime_r(&m_incidentClosedDate, &incidentDateStruct) != NULL)
		{
			incidentDataStream << std::setfill('0')
					<< std::setw(2) << incidentDateStruct.tm_mday
					<< '/'
					<< std::setw(2) << incidentDateStruct.tm_mon + 1
					<< '/'
					<< std::setw(4) << incidentDateStruct.tm_year + 1900  // date time, struct tm start from 1900, ie year 2005 = 105
					<< ' '
					<< std::setw(2) << incidentDateStruct.tm_hour
					<< ':'
					<< std::setw(2) << incidentDateStruct.tm_min 
					<< ':'
					<< std::setw(2) << incidentDateStruct.tm_sec;
		}

		FUNCTION_EXIT;
		return incidentDataStream.str();
	}

    std::string StationReportHelper::getLocalDatabaseName()
	{
		if (m_localDatabase.empty())
		{
			// have to find out what is the local db name
			// this should be the first in the connection list so grab it
			if (! TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE) )
			{
				throw TA_Base_Core::DatabaseException("db-connection-file not set");
			}
			try
			{
				TA_Base_Core::DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
				TA_Base_Core::DataConnections connections = connectionStrings.getConnectionList(TA_Base_Core::Incident_Ad, TA_Base_Core::Write);
				m_localDatabase = connections[0].first;  
			}
			catch(...) // most likely a file not found error
			{
				throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
			}

		}
		return m_localDatabase;
	}

	//TD15204 - sets the report id using the incident's report id
	void StationReportHelper::setIncidentReportId(std::string  incidentId)
	{
		m_reportId = incidentId;
	}

}// end TA_IRS_Core
