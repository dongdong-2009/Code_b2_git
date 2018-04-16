/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/IncidentReportHelper.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * IncidentReportHelper is an object that is held by CtaIncidentReportData objects. 
  * It contains all data used by IncidentReport, and manipulation
  * methods for the data. It helps avoid re-writing code for both CtaIncidentReportData
  */

#ifndef INCIDENTREPORTHELPER_INCIDENTDAI_HH
#define INCIDENTREPORTHELPER_INCIDENTDAI_HH

#include <string>
#include <vector>
#include <time.h>
#include "core/data_access_interface/incident_dai/src/AssetType.h"
#include "core/data_access_interface/incident_dai/src/PlanType.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IAlarmData.h"


namespace TA_Base_Core
{
    class IData;
    class IDatabase;
}

namespace TA_IRS_Core
{
	
	class IncidentReportHelper 
	{
	public:
    /** 
	* IncidentReportHelper
	*
	* This constructor creates a new IncidentReportHelper for the specified report id (primary key).
	*
	* @param  idKey the reportId (primary key)
	*
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		IncidentReportHelper(const std::string idKey, bool isIDKey);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/** 
		* IncidentReportHelper
		*
		* using shallow copy constructor.
		*/
		//    IncidentReportHelper( const IncidentReportHelper& theIncidentReportHelper);
		
		//Mintao ++
		//Bug 652 (TD12313)
		IncidentReportHelper(std::string& id, time_t createDate, const std::string& title, 
			const std::string& location1, const std::string& location2, bool innerTrack, bool outerTrack, 
			const std::string& train, const std::string& alarmId, const std::string& asset, 
			const std::string& subsystem, const std::string& creator, const std::string& profile, 
			time_t closeDate);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/** 
		* IncidentReportHelper
		*
		* This constructor creates a new IncidentReportHelper, and get a new report Id
		*
		*/
		IncidentReportHelper();
		
		/**
		* Creates an incident with an associated alarm.
		* @param alarmId    The alarm Id
		* 
		*/
		IncidentReportHelper(const std::string & alarmId, const std::string & alarmValue);
		
		/** 
		* ~IncidentReportHelper
		*
		* Destructor for basic class cleanup.
		*
		*/
		~IncidentReportHelper();
		
		// TD15865 Yanrong++
		/**
		*	increaseEditOperator		edit operator increase, update edit operator count to DB
		*	@param Nil 
		*/
		virtual void increaseEditOperator( );

		/**
		*	decreaseEditOperator		edit operator decrease, update edit operator count to DB                                                                    
		*	@param Nil
		*/
		virtual void decreaseEditOperator();

		/**
		*	getEditOperatorCound	get edit operator count from DB
		*	@param count		(output param)edit operator count retrieved from DB
		*
		*	@return bool		indicate if retrieve data successfully.
		*/
		virtual bool getEditOperatorCount(unsigned int& count);
		// ++Yanrong
		
		/**
		* isNew
		* 
		* return report new status
		*/
		virtual bool isNew() const
		{
			return m_isNew;
		};
		
		/**
		* Sets the closes the incident using current date and time and save this change
		* in database.
		* @throw IncidentException if fails to close the incident in the database
		* successfully.
		*/
		virtual void close();
		/**
		* Clears the close date and save this in the database
		* @throw IncidentException if fails to reopen the incident in the database
		* successfully.
		*/
		virtual void reopen();
		
		/**
		* Deletes this report from the database.  If the report is associated with a
		* station report, deleting this one should automatically delete that one too, so
		* we don't need to explicitly delete the station report.
		* 
		* @throws IncidentException when it fails to delete the incident report
		* successfully.
		*/
		virtual void deleteThisReport();
		
		
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* assigns a stations report to this incident report and commit to database
		*/
		virtual void updateStationReportId(std::string  stationId);
		//Mintao ++
		//Bug 652 (TD12313)
		
		
		/**
		* writeData
		* 
		* save data to database
		*/
		virtual void writeData();
		
		/**
		* @param closeDate
		* 
		*/
		virtual time_t getCloseDate();
		
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getReportId();
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* @param title
		* 
		*/
		virtual void setTitle(const std::string& title);
		virtual std::string getTitle();
		/**
		* Does not return an IOperator* in case the operator that created the report is
		* deleted from the database BEFORE the report.
		* 
		* ItaIncidentReportData determines the creator by looking up the session id at
		* the time of creation and determining the associated operator name. There is no
		* need for a "set" for the creator name.
		*/
		virtual std::string getCreator();
		/**
		* Does not return an IProfile* in case the profile is deleted from the database
		* BEFORE the report.
		* 
		* ItaIncidentReportData determines the creator by looking up the session id at
		* the time of creation and determining the associated profile. There is no need
		* for a "set" for the creator profile.
		*/
		virtual std::string getCreatorProfile();
		/**
		* Automatically set at time of creation.
		*/
		virtual time_t getCreatedDateTime();
		/**
		* ItaIncidentReportData determines the modifier by looking up the session id at
		* the time of modification and determining the associated operator name. There is
		* no need for a "set" for the modifier name.
		*/
		virtual std::string getModifier();
		/**
		* ItaIncidentReportData determines the modifier profile by looking up the session
		* id at the time of modification and determining the associated profile. There is
		* no need for a "set" for the modifier profile.
		*/
		virtual std::string getModifierProfile();
		/**
		* Automatically set at time of modification.
		*/
		virtual time_t getModifiedDateTime();
		/**
		* @param state
		* 
		*/
		virtual void setFalseIncident(bool state);
		virtual bool isFalseIncident();
		/**
		* @param asset
		* 
		*/
		virtual void setAsset(AssetType* asset);
		/**
		* Returns the associated asset.
		* asset.id = 0, description = subsystem = "" if there is no asset associated with
		* this incident.
		* 
		* @exception DatabaseException Thrown if there is a database error.
		* @exception DataException Thrown if the associated asset is invalid
		*/
		virtual AssetType getAsset();
		/**
		* @param train
		* 
		*/
		virtual void setTrain(const std::string& train);
		virtual std::string getTrain();
		/**
		* @param state
		* 
		*/
		virtual void setInnerTrack(bool state);
		virtual bool getInnerTrack();
		/**
		* @param state
		* 
		*/
		virtual void setOuterTrack(bool state);
		virtual bool getOuterTrack();
		/**
		* @param kilometres
		* 
		*/
		virtual void setKilometreLoss(unsigned long kilometres);
		virtual unsigned long getKilometreLoss();
		/**
		* @param location
		* 
		*/
		virtual void setLocation1(std::string location);
		virtual std::string getLocation1();
		/**
		* @param location
		* 
		*/
		virtual void setLocation2(std::string location);
		virtual std::string getLocation2();
		/**
		* @param dateTime
		* 
		*/
		virtual void setStartDateTime(time_t dateTime);
		virtual time_t getStartDateTime();
		/**
		* @param dateTime
		* 
		*/
		virtual void setEndDateTime(time_t dateTime);
		virtual time_t getEndDateTime();
		/**
		* @param delay
		* 
		*/
		virtual void setInitialDelay(unsigned long delay);
		virtual unsigned long getInitialDelay();
		/**
		* @param delay
		* 
		*/
		virtual void setAccumulatedDelay(unsigned long delay);
		virtual unsigned long getAccumulatedDelay();
		/**
		* Returns the details of the associated alarm
		*/
		virtual std::string getAssociatedAlarm();
		/**
		* @param description
		* 
		*/
		virtual void setAdditionalAlarmDescription(const std::string& description);
		virtual std::string getAdditionalAlarmDescription();
		/**
		* Associate the incident with the plans that have been executed in response to
		* the incident.  If the incident already got associated plans when this method is
		* called, it will compare the new list with the old one.  Any plan that does not
		* exist in the old one will be added and any that is not in the new list will be
		* removed.
		* @param plans
		* 
		*/
		virtual void setPlansExecuted(std::vector<PlanType> plans);
		/**
		* Returns an array of plans.  It's empty if it's not associated with any plans.
		*/
		virtual std::vector<PlanType> getPlansExecuted();
		/**
		* @param operators
		* 
		*/
		virtual void setOperators(const std::string& operators);
		virtual std::string getOperators();
		/**
		* @param comments
		* 
		*/
		virtual void setComments(const std::string& comments);
		virtual std::string getComments();
		/**
		* @param recommendation
		* 
		*/
		virtual void setRecommendation(const std::string& recommendation);
		virtual std::string getRecommendation();
		
		
		/**
		* @param stationId
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getStationId();
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* @param create location id
		* 
		*/
		virtual unsigned long getCreateLocationId();
		
		
		//Mintao++
		//bug 644
		/**
		* getAssociatedAlarmDetail
		* 
		* return the assocated alarm detail
		* 
		* @return virtual std::string 
		* 
		* @exception <exceptions> Optional
		*/
		virtual std::string getAssociatedAlarmDetail();
		//Mintao++
		//bug 644
		
		/**
		* reload()
		*
		* This method reloads the data from the database. It is called when a get... method
		* is called and the data state is not valid.
		*
		* pre:    This IncidentReport was initially loaded from the database
		*         or writeECSZoneData() has already been called
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
		* getDateData
		* 
		* helper function to retrieve date data from a table
		*/
		static void getDateData(TA_Base_Core::IData* data, const unsigned long rowIndex, const std::string& fieldName, time_t& out);
		
		/**
		* getConsoleLocationKey
		* 
		* helper function to retrieve the console location key only when it is not initialized
		*/
		static unsigned long getConsoleLocationKey();
		
		/**
		* getConsoleLocationName
		* 
		* helper function to retrieve the console location name only when it is not initialized
		*/
		static std::string getConsoleLocationName();
		
		/**
		* long getCreatorId
		* 
		* get the creator id
		* 
		* @return virtual unsigned 
		* 
		* @exception <exceptions> Optional
		*/
		virtual unsigned long getCreatorProfileId();
		
	private:
	/**
	* getUpdateDate
	* 
	* creates the string to be used for a date SQL insert/update statement
		*/
		std::string getUpdateDate();
		
		/**
		* updateReportIDCounter
		* 
		* update the Report ID counter and save to the counter table
		*/
		void updateReportIDCounter();
		
		/**
		* retrieveReportID
		*
		* returns a reportID generated from database
		*/
		std::string retrieveReportID();
		/**
		* loadPlans
		* 
		* load associated plans
		*/
		void loadPlans(TA_Base_Core::IDatabase* database);
		
		/**
		* savePlans
		* 
		* save associated plans
		*/
		void savePlans(TA_Base_Core::IDatabase* database);
		
		/**
		* addNewRecord
		*
		* This will add a new data in the database with the internals currently set in this 
		* instance.
		*
		* @throws  DatabaseException 
		*          - if there is a problem writing the data to the database.  
		*
		*/
		void addNewRecord();
		
		/**
		* modifyExistingRecord
		*
		* This will update an existing report in the database with the internals currently 
		* set in this instance.
		*
		* @throws  DatabaseException 
		*          - if there is a problem writing the data to the database.  
		*
		*/
		void modifyExistingRecord();
		
		
		/**
		* initialize
		* 
		* initialize all fields to default values
		*/
		void initialize();
		
		
		/**
		* getForeignKeyValues
		* 
		* get id values for location1, location2
		* 
		*/
		void getForeignKeyValues(TA_Base_Core::IDatabase* database, std::string& locationId1, std::string& locationId2);
		
		
		/**
		* getInsertDateSql
		* 
		* creates the string to be used for a date SQL insert/update statement
		*/
		std::string getUpdateDateSql(time_t datetime);
		
		
		/**
		* updateModifyData
		* 
		* get the values for modifier profile and modify date
		* 
		*/
		void updateModifyData();
		
		
		/**
		* getAlarmDetailFromID
		* 
		* get the Alarm detail from the provided alarm id
		* 
		* @return void 
		* @param : std::string alarmid
		* 
		* @exception <exceptions> Optional
		*/
		void getAlarmDetailFromID(const std::string & alarmId, const std::string & alarmValue);
		
		void loadAssetDetailsFromAlarm(std::string alarmid);

		void loadAssetDetails(TA_Base_Core::IDatabase* databaseConnection);

		void loadPlanFromAlarm(std::string alarmid);		//TD15871, hongzhi
		
		/**
		* getLocalDatabaseName
		*
		* returns the name of the local database
		*/
		std::string getLocalDatabaseName();
		
		//TD15870 xinsong ++
		std::string getMmsStateDesc( TA_Base_Core::IAlarmData::EMmsState alarmEmmsState );
		std::string getAlarmAVLStateDesc( TA_Base_Core::IAlarmData::EAlarmAVLStatus alarmState );
		std::string getTimeDesc(time_t & time);
private:
		// The name of the local database to send updates to
		std::string m_localDatabase;
		
		//Mintao ++
		//Bug 652 (TD12313)
		std::string m_reportId;               //ININCI_ID
		//Mintao ++
		//Bug 652 (TD12313)
		std::string m_title;                    //TITLE
		
		unsigned long m_createOprId;            //CREATE_OPERAT_ID
		unsigned long m_createProfileOprId;     //CREAT_SEPROF_ID
		std::string m_creator;
		std::string m_creatorProfile;
		time_t m_createDate;                    //CREATE_DATE
		
		unsigned long m_modifyOprId;            //MODIFY_OPERAT_ID
		unsigned long m_modifyProfileOprId;     //MODIFY_SEPROF_ID
		std::string m_modifier;
		std::string m_modifierProfile;
		time_t m_modifyDate;                    //MODIFY_DATE
		
		bool m_falseIncident;                   //FALSE_INCIDENT
		
		unsigned long m_entityId;               //ENTITY_ID
		
		unsigned long m_locationId1;            //LOCATI_ID_1
		unsigned long m_locationId2;            //LOCATI_ID_2
		std::string m_location1;
		std::string m_location2;
		
		std::string m_train;
		
		time_t m_startDate;                     //START_DATE
		time_t m_endDate;                       //END_DATE
		bool m_innerTrack;                      //INNER_TRACK
		bool m_outerTrack;                      //OUTER_TRACK
		unsigned long m_kmLoss;                 //KM_LOSS
		unsigned long m_initialDelay;           //INITIAL_DELAY
		
		unsigned long m_totalDelay;             //TOTAL_DELAY
		
		std::string m_alarmId;                  //ALARM_ID
		std::string m_alarmValueFromCmdline;
		//Mintao++
		//bug 644
		std::string m_alarmAdditinalDesc;       //ALARM_DESC
		std::string m_alarmDetail;              
		//Mintao++
		//bug 644
		
		
		std::string m_operators;                //OPERATORS
		std::string m_comments;                 //COMMENTS
		std::string m_recommendation;           //RECOMMENDATION
		
		std::string m_stationId;              //INSTAT_ID
		time_t m_closeDate;                     //CLOSE_DATE
		unsigned long m_createLocationId;       //CREATE_LOCATI_ID
		unsigned long m_reportIdCounter;    ///REPORT ID COUNTER //TD15204
		
		AssetType m_assetType;
		std::vector<PlanType> m_plansExecuted;
		
		bool m_isValidData;
		bool m_isValidSummaryData;
		bool m_isNew;
		
		
		bool m_plansChanged; // true if we need to update plans executed data
		bool m_location1Changed;
		bool m_location2Changed;
		//TD15204 - retrieve console location key
		static unsigned long m_consoleLocationKey; //PHYSICAL_LOCATION_KEY
		static std::string m_consoleLocationName; //PHYSICAL_LOCATION_NAME
		TA_Base_Core::IDatabase* m_dbConnection;
		
		static const std::string IN_ININCI_ID_COL;
		static const std::string IN_TITLE_COL;
		static const std::string IN_CREATE_OPERATOR_ID_COL;
		static const std::string IN_CREATE_OPERATOR_PROFILE_ID_COL;
		static const std::string IN_CREATE_DATE_COL;
		static const std::string IN_MODIFY_OPERATOR_ID_COL;
		static const std::string IN_MODIFY_OPERATOR_PROFILE_ID_COL;
		static const std::string IN_MODIFY_DATE_COL;
		static const std::string IN_FALSE_INCIDENT_COL;
		static const std::string IN_ENTITY_ID_COL;
		static const std::string IN_LOCATION_ID_1_COL;
		static const std::string IN_LOCATION_ID_2_COL;
		static const std::string IN_TRAIN_ID_COL;
		static const std::string IN_START_DATE_COL;
		static const std::string IN_END_DATE_COL;
		static const std::string IN_INNER_TRACK_COL;
		static const std::string IN_OUTER_TRACK_COL;
		static const std::string IN_KM_LOSS_COL;
		static const std::string IN_INITIAL_DELAY_COL;
		static const std::string IN_TOTAL_DELAY_COL;
		static const std::string IN_ALARM_ID_COL;
		static const std::string IN_ALARM_DESC_COL;
		static const std::string IN_ALARM_VALUE_COL;
		static const std::string IN_OPERATORS_COL;
		static const std::string IN_COMMENTS_COL;
		static const std::string IN_RECOMMENDATION_COL;
		static const std::string IN_STATION_ID_COL;
		static const std::string IN_CLOSE_DATE_COL;
		static const std::string IN_CREATE_LOCATION_ID_COL;
		
		static const std::string IN_LOCATION_NAME_1_COL;
		static const std::string IN_LOCATION_NAME_2_COL;
		
		static const std::string IN_PLAN_ID_COL;
		static const std::string IN_PLAN_DESC_COL;
		
		//TD15204 
		static const int MAX_INCIDENTCOUNTER;

		// TD15865 Yanrong++
		//TA_Base_Core::NonReEntrantThreadLockable m_editCountLock;
		void updateIncidentTable( const std::string & column, const std::string & value );
		// ++Yanrong
	};
}// end TA_IRS_Core
#endif

