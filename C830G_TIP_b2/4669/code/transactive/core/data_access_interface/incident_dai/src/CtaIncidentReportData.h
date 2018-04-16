/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/incident_dai/src/CtaIncidentReportData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Local storage for Incident Reports. Reads Incident Reports from the database.
  * Writes Incident Reports to the database.
  * 
  * There are a large number of methods in this class. This is because the
  * relationships between the data elements have not been specified in the
  * requirements. Thus assumptions can not be made. The current arrangement of
  * methods also follows the likely useage patterns for this class. That is: update
  * one or more elements then write the result to the database.
  * 
  * The class does intelligent loading of the data. Its inital load (on creation)
  * will be of all the required summary data that is displayed by the Incident
  * Manager in list form. When data that is not part of the summary is accessed by
  * the Incident Manager this class will load all remaining data in one operation.
  */
#ifndef CtaIncidentReportData_2D2F8B40_C93B_4684_80B7_E903E6499243__INCLUDED_
#define CtaIncidentReportData_2D2F8B40_C93B_4684_80B7_E903E6499243__INCLUDED_

#include "core/data_access_interface/incident_dai/src/ItaIncidentReportData.h"

namespace TA_IRS_Core
{
	
	class IncidentReportHelper;
	
	class CtaIncidentReportData : public ItaIncidentReportData 
	{
		
	public:
		CtaIncidentReportData();
		virtual ~CtaIncidentReportData();
		
		/**
		* Used when a third party (factory) has performed a bulk SQL query and is
		* providing all the necessary data. No further database access is required until
		* one of the data elements that was not specified is accessed.
		* @param title
		* @param location
		* @param innerTrack
		* @param outerTrack
		* @param train
		* @param alarmDesc
		* @param asset
		* @param subsystem
		* @param operator
		* @param profile
		* @param closed
		* 
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		CtaIncidentReportData(std::string id, time_t createDate, const std::string& title, 
			const std::string& location1, const std::string& location2, bool innerTrack, bool outerTrack, 
			const std::string& train, const std::string& alarmDesc, const std::string& asset, 
			const std::string& subsystem, const std::string& operator1, const std::string& profile, 
			time_t closeDate);
		//Mintao ++
		//Bug 652 (TD12313)
		
		/**
		* Copy constructor
		* @param incidentReport
		* 
		*/
		CtaIncidentReportData(const CtaIncidentReportData& incidentReport);
		
		/**
		* Creates an incident with an associated alarm.
		* @param alarmId    The alarm Id
		* 
		*/
		CtaIncidentReportData(const std::string & alarmId, const std::string & alarmValue);
		//Mintao ++
		//Bug 652 (TD12313)
		
		CtaIncidentReportData(std::string id, bool isIDKey);
		//Mintao ++
		//Bug 652 (TD12313)
		// TD15865 Yanrong++
		virtual void openForEdit();
		virtual void endEdit();
		virtual bool canClose();
		// ++Yanrong
		
		/**
		* Returns the associated station report.  If the incident is not currently
		* associated with any station report, it will create a new one and return that.
		* It only fetches the station report when this method is called (does not preload
		* it)
		*/
		virtual ItaStationReportData& getStationReport();
		
		/**
		* Sets the closes the incident using current date and time and save this change
		* in database.
		* @return true if the incident is allowed to be closed; return false if the
		* incident is already closed.
		* @throw IncidentException if fails to close the incident in the database
		* successfully.
		*/
		virtual void close();
		
		/**
		* Clears the close date and save this in the database
		* @return true if the incident is allowed to be reopened; return false if the
		* incident is already open.
		* @throw IncidentException if fails to reopen the incident in the database
		* successfully.
		*/
		virtual void reopen();
		
		/**
		* Returns a copy of the instance.
		*/
		virtual ItaIncidentReportData* clone() const;
		
		/**
		* Deletes this report from the database.  If the report is associated with a
		* station report, deleting this one should automatically delete that one too, so
		* we don't need to explicitly delete the station report.
		* 
		* @throws IncidentException when it fails to delete the incident report
		* successfully.
		*/
		virtual void deleteThisReport();
		
		virtual bool isClosed();
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
		* Return the details of the associated alarm
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
		* assigns a stations report to this incident report and commit to database
		*/
		//Mintao ++
		//Bug 652 (TD12313)
		
		virtual void updateStationReportId(std::string  stationId);
		//Mintao ++
		//Bug 652 (TD12313)
		
		//Mintao ++
		//bug 644
		virtual std::string getAssociatedAlarmDetail();
		//Mintao ++
		//bug 644
		
		virtual unsigned long getCreateLocationId();
		
		/**
		* Commits the incident to the database.
		* @throws IncidentException when it fails to save the incident report
		* successfully.
		*/
		virtual void save();
		/**
		* Reloads the data from the database.
		*/
		virtual void reload();
		/**
		* Returns true if the data has been modified since the last database fetch/commit.
		* 
		* @return true if the data has been modified since the last database fetch/commit.
		* 
		*/
		virtual bool isModified();
		/**
		* Returns true if the incident is not yet available in the database.
		*/
		virtual bool isNew();
		
		/**
		* long getCreatorProfileId
		* 
		* get the creator id
		* 
		* @return virtual unsigned 
		* 
		* @exception <exceptions> Optional
		*/
		virtual unsigned long getCreatorProfileId();
		
		/**
		* @param stationId
		* 
		*/
		//Mintao ++
		//Bug 778 (TD12858)
		virtual std::string getStationId();
		//Mintao ++
		//Bug 778 (TD12858)
private:
    bool m_isModified;
    IncidentReportHelper* m_incidentReportHelper;
	
    /**
	* associated station report data
	*/
    ItaStationReportData* m_stationReportData;
	
	// TD15865 Yanrong++
	bool m_canClose;
	// ++Yanrong
	
};
} // end TA_IRS_Core
#endif
