/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/StationReportHelper.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * StationReportHelper is an object that is held by CtaStationReportData objects. 
  * It contains all data used by Station Report, and manipulation
  * methods for the data. It helps avoid re-writing code for both CtaStationReportData.
  * It access the data stores in IN_STATION_REPORT table.
  */

#ifndef STATIONREPORTHELPER_INCIDENTDAI_HH
#define STATIONREPORTHELPER_INCIDENTDAI_HH

#include <string>

#include "IncidentCauseType.h"
#include "IncidentLocationType.h"
#include "IncidentConditionType.h"
#include "IncidentType.h"
#include "core/data_access_interface/src/IDatabase.h"

 //TD15251 
// #include "bus/generic_server_simulator/src/GssCommon.h"

namespace TA_Base_Core
{
    class IData;
}

namespace TA_IRS_Core
{
	
	class StationReportHelper 
	{
	public:

    //TD16056   Mintao ++
    //for archive manager
	virtual void setClosedDate(time_t date);
	virtual time_t getClosedDate();
	//Mintao ++
    //for archive manager
		
    /** 
	* StationReportHelper
	*
	* This constructor creates a new StationReportHelper for the specified report id (primary key).
	* It is used for modifying existing record, hence the m_isNew flag is set to false.
	*
	* @param  idKey the reportId (primary key)
	*
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		StationReportHelper(const std::string idKey);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/** 
		* StationReportHelper
		*
		* Copy constructor. It is used for create new record and store to the database. Hence, the
		* the m_isNew is set to true.
		*
		* @param   StationReportHelper the original StationReportHelper to copy.
		*
		*/
		StationReportHelper( const StationReportHelper& theStationReportHelper);
		
		/** 
		* StationReportHelper
		*
		* This constructor creates a new object using the input data entered by the user from
		* the StationReportDialog. It is used for create new record and store to the database. 
		* Hence, the m_isNew is set to true.
		*
		* @param submitter             the name of the operator who submitted the report
		*
		* @param employeeParticualars  Employee particulars
		*
		* @param date                  the date that the incident occurred.
		*
		* @param type                  the type of the incident 
		*
		* @param location              the location where the incident has occured
		*
		* @param condition             the condition of the station when the incident occurred.
		*
		* @param cause                 the cause of the incident
		*
		* @param action                the action taken in response to the incident
		*
		* @param staff                 the staff involved in the incident
		*
		* @param description           the detailed description of the incident
		*
		* @param createLocationId      the location where the station report was created at
		*  
		*/
		StationReportHelper(std::string submitter, std::string employeeParticulars, 
            time_t date, IncidentType type, IncidentLocationType location, 
            IncidentConditionType condition, IncidentCauseType cause, 
            std::string action, std::string staff, std::string description, 
            unsigned long createLocationId);
		
			/** 
			* StationReportHelper
			*
			* This constructor creates a new StationReportHelper for configuration, initiailising
			* the primary key to -1.  This primary key will be automatically set to a valid value 
			* when writeStationReportData() is called for the first time.
			*
		*/
		StationReportHelper();
		
		/** 
		* ~StationReportHelper
		*
		* Destructor for basic class cleanup.
		*
		*/
		~StationReportHelper();
		
		/**
		* writeStationReportData
		* 
		* Write this Station Report to the database.
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*         - NO_VALUE if the record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the primary key returns multiple records 
		* @throws  DataConfigurationException
		*          - if the data contained in the StationReport object is not sufficent to create 
		*            an entry in the database. This is thrown as an exception and not tested 
		*            as an assert as it can be directly linked to GUI components and may be 
		*            the result of a user action that occurs prior to filling in all sections.
		*
		*/
		void writeStationReportData();
		
		/**
		* setAction
		*
		* Sets the action taken in response to the station report.The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  action taken in response to the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual void setAction(const std::string& action);
		
		/**
		* getAction
		*
		* Returns the action taken in response to the station report in IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the action taken in response to the station report.
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual std::string getAction();
		
		/**
		* getCause
		*
		* Returns the cause of the incidnet from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the cause of the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual IncidentCauseType getCause();
		
		/**
		* setCause
		*
		* Sets the cause of the incident.The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the cause of the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual void setCause(const IncidentCauseType cause);
		
		/**
		* getLocation
		*
		* Returns the location where the incident has been occurred from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the location where the incident has been occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual IncidentLocationType getLocation();
		
		/**
		* setLocation
		*
		* Sets the location where the incident has been occurred.The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the location where the incident has been occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual void setLocation(const IncidentLocationType location);
		
		/**
		* getDate
		*
		* Returns the date that the incident occurred from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the date that the incident occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual time_t getDate();
		
		/**
		* setDate
		*
		* Sets the date that the incident occurred.The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the date that the incident occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual void setDate(const time_t);
		
		/**
		* getDescription
		*
		* Returns the detail description of the incident from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the detail description of the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual std::string getDescription();
		
		/**
		* setDescription
		*
		* Setsthe detail description of the incident. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the detail description of the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setDescription(const std::string desc);
		
		/**
		* getEmployeeParticulars
		*
		* Returns employee particulars of the operator from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return employee particulars of the operator
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual std::string getEmployeeParticulars();
		
		/**
		* setEmployeeParticulars
		*
		* Setsthe employee particulars of the operator. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  employee particulars of the operator
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setEmployeeParticulars(const std::string ep);
		
		/**
		* getCondition
		*
		* Returns the condition of the station when the incident occurred from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the condition of the station when the incident occurred 
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual IncidentConditionType getCondition();
		
		/**
		* setCondition
		*
		* Sets the the condition of the station when the incident occurred. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the condition of the station when the incident occurred 
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setCondition (const IncidentConditionType condtion);
		
		/**
		* getReportId
		*
		* Returns the report id (key) of the station report from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the report id (key) of the station report
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getReportId();
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* getStaff
		*
		* Returns the staff involved in the incident from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the staff involved in the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual std::string getStaff();
		
		/**
		* setStaff
		*
		* Sets the staff involved in the incident. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the staff involved in the incident
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setStaff(const std::string staff);
		
		/**
		* getSubmittedBy
		*
		* Returns the name of the operator who submitted the report from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the name of the operator who submitted the report 
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual std::string getSubmittedBy();
		
		/**
		* setSubmittedBy
		*
		* Sets the name of the operator who submitted the report. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the name of the operator who submitted the report 
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setSubmittedBy(const std::string submitter);
		
		/**
		* getType
		*
		* Returns the type of incident that has occurred from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the type of incident that has occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual IncidentType getType();
		
		/**
		* setType
		*
		* Sets the type of incident that has occurred. The set values are not written to the database
		* until writeStationReportData() is called.  This method should only be used by the 
		* CtaStationReportData class.
		*
		* @param  the type of incident that has occurred
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/    
		virtual void setType (const IncidentType type);
		
		/**
		* getCreatorProfile
		*
		* Returns the id (key) of the operator who create the station report from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return the id (key) of the operator who create the station report
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual unsigned long getCreatorProfile();
		
		
		/**
		* getCreateLocationId
		*
		* Returns location id (key to LOCATION table) where the report is created who create the station report from IN_STATION_REPORT table.  
		* If this record does not exist in the database (constructed without a key), then data will be retrieved
		* from within the class.  Otherwise, if this is the first time data for this 
		* existing Station Report has been requested, all the data is loaded from the database.
		*
		* pre:    The data exists in the database or has been set by a call to the equivalent 
		*         set... method.
		*
		* @return location id (key to LOCATION table) where the report 
		*
		* @throws DatabaseException 
		*         - if there is a problem establishing a connection with the database.
		*         - if an error is encountered while retrieving data.
		* @throws DataException
		*         - if the data cannot be converted to the required format 
		*           (e.g. INSTATI_ID should be an unsigned long)
		*         - NO_VALUE if the IN_STATION_REPORT record cannot be found for the helper 
		*           constructed with a key.
		*         - NOT_UNIQUE if the key returns multiple records 
		*/
		virtual unsigned long getCreateLocationId();
		
		
		/**
		* invalidate
		*
		* Make the data contained by this StationReport as invalid. The next call to get...() 
		* following a call to invalidate() will cause all the data to be reloaded from
		* the database.  A possible scenario for this method to be used is when it is known
		* that the database has been modified outside the process.
		*
		* It is a precondition that writeStationReportData() has been called prior to calling
		* this method on a new Station Report, as it does not make any sense to invalidate an
		* Station Report that has not yet been written to the database.
		*
		* pre:    This Station Report was initially loaded from the database
		*         or writeStationReportData() has already been called
		*/
		void invalidate();
		
		/**
		* isNew
		*
		* This returns whether this is a new StationReport.  A new StationReport is created by the default
		* constructor.  Once written to the database, it is no longer considered new.
		*
		* @return true if this is a new StationReport
		*/
		virtual bool isNew() const;
		
		/**
		* deleteRecord
		*
		* Remove this StationReport from the database..
		*
		* pre:    This StationReport was initially loaded from the database
		*         or writeStationReportData() has already been called
		*
		*/
		void deleteRecord();
		
		//TD15204 - sets the station report id
		void setIncidentReportId(std::string  incidentId);
		
	private:
		
	/**
	* reload()
	*
	* This method reloads the data from the database. It is called when a get... method
	* is called and the data state is not valid.
	*
	* pre:    This IncidentReport was initially loaded from the database
	*         or writeStationReportData() has already been called
	*
	* @throws DatabaseException 
	*         - if there is a problem establishing a connection with the database.
	*         - if an error is encountered while retrieving data.
	* @throws DataException
	*         - if the data cannot be converted to the required format 
	*           (e.g. ININCI_ID should be an unsigned long)
	*         - NO_VALUE if the Incident Report Id record cannot be found for the helper 
	*           constructed with an Incident Report Id.
	*         - NOT_UNIQUE if the Incident Report Id returns multiple records 
	*
		*/
		void reload();
		
		/**
		* convertBitString2Cause
		*
		* helper method to convert the CAUSE attribute which is in bit-string to IncidentCauseType
		*
		* @param  the bit-string of the cause get from IN_STATION_REPORT table in the following order:
		*          - ESP
		*          - BLS
		*          - PAX
		*          - Point
		*          - Power
		*          - Signal
		*          - Tk Cct
		*
		* @param  the cause other than the onces specified in CAUSE attributes in IN_STATION_REPORT table
		*
		* @return the cause of the incident in IncidentCauseType struct format
		*
		*/    
		IncidentCauseType     convertBitString2Cause(const std::string cause, const std::string otherCause);
		
		/**
		* convertBitString2Location
		*
		* helper method to convert the LOCATION attribute which is in bit-string to IncidentLocationType
		*
		* @param  the bit-string of the location get from IN_STATION_REPORT table in the following order:
		*          - Train
		*          - Trackside
		*          - Station
		*          - Deport
		*
		* @param  the location other than the onces specified in LOCATION attributes in IN_STATION_REPORT table
		*
		* @return the location of the incident in IncidentLocationType struct format
		*
		*/    
		IncidentLocationType  convertBitString2Location(const std::string location, const std::string otherLocation);
		
		/**
		* convertBitString2Condition
		*
		* helper method to convert the CONDITION attribute which is in bit-string to IncidentConditionType
		*
		* @param  the bit-string of the condition get from IN_STATION_REPORT table in the following order:
		*          - Dry
		*          - Wet
		*          - Clean
		*          - Dry
		*          - Well Lit
		*          - Poorly Lit
		*
		* @param  the condition other than the onces specified in CONDITION attributes in IN_STATION_REPORT table
		*
		* @return the condition of the incident in IncidentConditionType struct format
		*
		*/    
		IncidentConditionType convertBitString2Condition(const std::string condition, const std::string otherCondition);
		
		/**
		* convertBitString2Type
		*
		* helper method to convert the TYPE attribute which is in bit-string to IncidentType
		*
		* @param  the bit-string of the type get from IN_STATION_REPORT table in the following order:
		*          - Train Service Delay
		*          - Derailment
		*          - Demaged Facility/Property
		*          - Fir
		*          - Equipment Abnormally
		*          - Equipment Malfunction
		*          - Trespassing
		*          - Criminal Act
		*          - Assault
		*          - Arrest of Person
		*          - Heated Argument
		*
		* @param  the type other than the onces specified in TYPE attributes in IN_STATION_REPORT table
		*
		* @return the type of the incident in IncidentType struct format
		*
		*/    
		IncidentType          convertBitString2Type(const std::string type, const std::string otherType);
    
		/**
		* convertType2BitString
		*
		* helper method to convert the m_condition which is a IncidentType struct to a bit-string to store in database
		*
		* @return the bit-string of the condition get from IN_STATION_REPORT table in the following order:
		*          - Dry
		*          - Wet
		*          - Clean
		*          - Dry
		*          - Well Lit
		*          - Poorly Lit
		*
		*/    
		std::string convertType2BitString();
    
		/**
		* convertLocation2BitString
		*
		* helper method to convert the m_location which is a IncidentLocationType to a bit-string to store in database
		*
		* @return  the bit-string of the location get from IN_STATION_REPORT table in the following order:
		*          - Train
		*          - Trackside
		*          - Station
		*          - Deport
		*
		*/    
		std::string convertLocation2BitString();
		
		/**
		* convertCause2BitString
		*
		* helper method to convert the m_cause which is a IncidentCauseType to a bit-string to store in database
		*
		* @return  the bit-string of the cause get from IN_STATION_REPORT table in the following order:
		*          - ESP
		*          - BLS
		*          - PAX
		*          - Point
		*          - Power
		*          - Signal
		*          - Tk Cct
		*
		*/    
		std::string convertCause2BitString();
		
		/**
		* convertCondition2BitString
		*
		* helper method to convert the m_condition which is a IncidentConditionType to a bit-string to store in database
		*
		* @return  the bit-string of the condition get from IN_STATION_REPORT table in the following order:
		*          - Dry
		*          - Wet
		*          - Clean
		*          - Dry
		*          - Well Lit
		*          - Poorly Lit
		*
		*/    
		std::string convertCondition2BitString();
    
		/**
		* convertIncidentDate2Tmstruct
		*
		* helper method to convert the m_incidentDate to a string format to save to database
		*
		* @return  the string representation of the m_incidentDate
		*
		*/    
		std::string convertIncidentDate2Tmstruct(); 
		
		/**
		* convertIncidentClosedDate2Tmstruct
		*
		* helper method to convert the m_incidentClosedDate to a string format to save to database
		*
		* @return  the string representation of the m_incidentClosedDate
		*
		*/    
        std::string convertIncidentClosedDate2Tmstruct();  //Not used now		
    /**
		* addNewRecord
		*
		* This will add a new Station Report Data in the database with the internals currently set in this 
		* instance.
		*
		* @throws  DatabaseException 
		*          - if there is a problem writing the data to the database.  
		* @throws  DataConfigurationException 
		*          - if the data contained in the StationReport object is not sufficent to create an
		*            entry in the database.  This is thrown as an exception and not tested as 
		*            an assert as it can be directly linked to GUI components and may be the 
		*            result of a user action that occurs prior to filling in all sections.
		* @throws  DataException 
		*          - if there is a problem writing the data to the database.
		*
		*/
		void addNewRecord();
		
		/**
		* modifyExistingRecord
		*
		* This will update an existing Station Report in the database with the internals currently 
		* set in this instance.
		*
		* @throws  DatabaseException 
		*          - if there is a problem writing the data to the database.  
		* @throws  DataConfigurationException 
		*          - if the data contained in the StationReport object is not sufficent to create an
		*            entry in the database.  This is thrown as an exception and not tested as 
		*            an assert as it can be directly linked to GUI components and may be the 
		*            result of a user action that occurs prior to filling in all sections.
		* @throws  DataException 
		*          - if there is a problem writing the data to the database.
		*
		*/
		void modifyExistingRecord();
    
		/**
		* getDateData
		* 
		* helper function to retrieve date data from a table
		* 
		* @param data the IData interface 
		*
		* @param rowIndex the row number in the table
		* 
		* @param fieldName the name of the field in the table
		*
		* @param out the time to return
		*/
		void getDateData(TA_Base_Core::IData* data, const unsigned long rowIndex, const std::string& fieldName, time_t& out);
		void getUpdateDateSql(TA_Base_Core::SQLTypeAdapter& sqlParam, time_t datetime, bool bIsAQ);
		
		// Made private as it is not used
		StationReportHelper& operator=(const StationReportHelper &);
		
	public:
		
		static const short INCIDENTCAUSE_LENGTH;
		static const short INCIDENTLOCATION_LENGTH;
		static const short INCIDENTCONDITION_LENGTH;
		static const short INCIDENTTYPE_LENGTH;
		
	private:
	/**
	* getLocalDatabaseName
	*
	* returns the name of the local database
		*/
		std::string getLocalDatabaseName();
		
		// The name of the local database to send updates to
		std::string m_localDatabase;
		
		std::string m_reportId;           //INSTAT_ID
		std::string m_submittedBy;          //SUBMITTED_BY
		std::string m_ep;                   //EP    
		time_t m_incidentDate;              //INCIDNET_DATE
		IncidentType m_type;                 //TYPE
    
		IncidentLocationType m_location;             //LOCATION    
		IncidentCauseType m_cause;                //CAUSE    
		IncidentConditionType m_condition;            //CONDITION
    
		std::string m_action;               //ACTION
		std::string m_staff;                //STAFF
		std::string m_description;          //DESCRIPTION
		unsigned long  m_creatorProfile;
		unsigned long m_createLocationId;   //CREATE_LOCATI_ID
		bool m_isValidData;
		bool m_isNew;
        //TD16056    Mintao++
        //used for the archive manager
		time_t m_incidentClosedDate;              //INCIDNET_CLOSED_DATE
	};
}// end TA_IRS_Core
#endif //STATIONREPORTHELPER_INCIDENTDAI_HH