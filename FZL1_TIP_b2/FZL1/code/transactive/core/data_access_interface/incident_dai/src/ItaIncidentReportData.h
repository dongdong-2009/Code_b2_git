/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/incident_dai/src/ItaIncidentReportData.h $
  * @author:  Albertus Zakaria
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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

#ifndef ItaIncidentReportData_D274B06D_91FE_432f_8B1E_289ADE04354C__INCLUDED_
#define ItaIncidentReportData_D274B06D_91FE_432f_8B1E_289ADE04354C__INCLUDED_

#include <vector>
//Mintao ++
//Bug 652 (TD12313)
#include <string>
//Mintao ++
//Bug 652 (TD12313)
#include <time.h>

#include "core/data_access_interface/incident_dai/src/AssetType.h"
#include "core/data_access_interface/incident_dai/src/PlanType.h"

namespace TA_IRS_Core
{
	
	class ItaStationReportData;
	
	class ItaIncidentReportData
	{
	public:
		
		virtual ~ItaIncidentReportData() {};
		
		/**
		* Returns a copy of the instance.
		*/
		virtual ItaIncidentReportData* clone() const =0;
		
		/**
		* Returns the associated station report.  If the incident is not currently
		* associated with any station report, it will create a new one and return that.
		*/
		virtual ItaStationReportData& getStationReport() =0;
		/**
		* Sets the closes the incident using current date and time and save this change
		* in database.
		* @throw IncidentException if fails to close the incident in the database
		* successfully.
		*/
		virtual void close() =0;
		/**
		* Clears the close date and save this in the database
		* @throw IncidentException if fails to reopen the incident in the database
		* successfully.
		*/
		virtual void reopen() =0;
		/**
		* Deletes this report from the database.  If the report is associated with a
		* station report, deleting this one should automatically delete that one too, so
		* we don't need to explicitly delete the station report.
		* 
		* @throws IncidentException when it fails to delete the incident report
		* successfully.
		*/
		virtual void deleteThisReport() =0;
		
		virtual bool isClosed() =0;
		virtual time_t getCloseDate() = 0;

		// TD15865 Yanrong++
		virtual void openForEdit() = 0;
		virtual void endEdit() = 0;
		virtual bool canClose() = 0;
		// ++Yanrong
		
		//Mintao ++
		//Bug 652 (TD12313)
		virtual std::string getReportId() =0;
		//Mintao ++
		//Bug 652 (TD12313)
		/**
		* @param title
		* 
		*/
		virtual void setTitle(const std::string& title) =0;
		virtual std::string getTitle() =0;
		/**
		* Does not return an IOperator* in case the operator that created the report is
		* deleted from the database BEFORE the report.
		* 
		* ItaIncidentReportData determines the creator by looking up the session id at
		* the time of creation and determining the associated operator name. There is no
		* need for a "set" for the creator name.
		*/
		virtual std::string getCreator() =0;
		/**
		* Does not return an IProfile* in case the profile is deleted from the database
		* BEFORE the report.
		* 
		* ItaIncidentReportData determines the creator by looking up the session id at
		* the time of creation and determining the associated profile. There is no need
		* for a "set" for the creator profile.
		*/
		virtual std::string getCreatorProfile() =0;
		/**
		* Automatically set at time of creation.
		*/
		virtual time_t getCreatedDateTime() =0;
		/**
		* ItaIncidentReportData determines the modifier by looking up the session id at
		* the time of modification and determining the associated operator name. There is
		* no need for a "set" for the modifier name.
		*/
		virtual std::string getModifier() =0;
		/**
		* ItaIncidentReportData determines the modifier profile by looking up the session
		* id at the time of modification and determining the associated profile. There is
		* no need for a "set" for the modifier profile.
		*/
		virtual std::string getModifierProfile() =0;
		/**
		* Automatically set at time of modification.
		*/
		virtual time_t getModifiedDateTime() =0;
		/**
		* @param state
		* 
		*/
		virtual void setFalseIncident(bool state) =0;
		virtual bool isFalseIncident() =0;
		/**
		* @param asset
		* 
		*/
		virtual void setAsset(AssetType* asset) =0;
		/**
		* Returns the associated asset.
		* asset.id = 0, description = subsystem = "" if there is no asset associated with
		* this incident.
		* 
		* @exception DatabaseException Thrown if there is a database error.
		* @exception DataException Thrown if the associated asset is invalid
		*/
		virtual AssetType getAsset() =0;
		/**
		* @param train
		* 
		*/
		virtual void setTrain(const std::string& train) =0;
		virtual std::string getTrain() =0;
		/**
		* @param state
		* 
		*/
		virtual void setInnerTrack(bool state) =0;
		virtual bool getInnerTrack() =0;
		/**
		* @param state
		* 
		*/
		virtual void setOuterTrack(bool state) =0;
		virtual bool getOuterTrack() =0;
		/**
		* @param kilometres
		* 
		*/
		virtual void setKilometreLoss(unsigned long kilometres) =0;
		virtual unsigned long getKilometreLoss() =0;
		/**
		* @param location
		* 
		*/
		virtual void setLocation1(std::string location) =0;
		virtual std::string getLocation1() =0;
		/**
		* @param location
		* 
		*/
		virtual void setLocation2(std::string location) =0;
		virtual std::string getLocation2() =0;
		/**
		* @param dateTime
		* 
		*/
		virtual void setStartDateTime(time_t dateTime) =0;
		virtual time_t getStartDateTime() =0;
		/**
		* @param dateTime
		* 
		*/
		virtual void setEndDateTime(time_t dateTime) =0;
		virtual time_t getEndDateTime() =0;
		/**
		* @param delay
		* 
		*/
		virtual void setInitialDelay(unsigned long delay) =0;
		virtual unsigned long getInitialDelay() =0;
		/**
		* @param delay
		* 
		*/
		virtual void setAccumulatedDelay(unsigned long delay) =0;
		virtual unsigned long getAccumulatedDelay() =0;
		/**
		* Returns the details of the associated alarm
		*/
		virtual std::string getAssociatedAlarm() =0;
		/**
		* @param description
		* 
		*/
		virtual void setAdditionalAlarmDescription(const std::string& description) =0;
		virtual std::string getAdditionalAlarmDescription() =0;
		/**
		* Associate the incident with the plans that have been executed in response to
		* the incident.  If the incident already got associated plans when this method is
		* called, it will compare the new list with the old one.  Any plan that does not
		* exist in the old one will be added and any that is not in the new list will be
		* removed.
		* @param plans
		* 
		*/
		virtual void setPlansExecuted(std::vector<PlanType> plans) =0;
		/**
		* Returns an array of plans.  It's empty if it's not associated with any plans.
		*/
		virtual std::vector<PlanType> getPlansExecuted() =0;
		/**
		* @param operators
		* 
		*/
		virtual void setOperators(const std::string& operators) =0;
		virtual std::string getOperators() =0;
		/**
		* @param comments
		* 
		*/
		virtual void setComments(const std::string& comments) =0;
		virtual std::string getComments() =0;
		/**
		* @param recommendation
		* 
		*/
		virtual void setRecommendation(const std::string& recommendation) =0;
		virtual std::string getRecommendation() =0;
		
		//Mintao ++
		//bug 644
		/**
		* @param associatedAlarmDetail
		* 
		*/
		virtual std::string getAssociatedAlarmDetail() = 0;
		/**
		* assigns a stations report to this incident report and commit to database
		*/
		//Mintao ++
		//bug 644
		
		//Mintao ++
		//Bug 652 (TD12313)
		virtual void updateStationReportId(std::string stationId) = 0;
		//Mintao ++
		//Bug 652 (TD12313)
		
		virtual unsigned long getCreateLocationId() = 0;
		
		/**
		* Commits the incident to the database.
		* @throws IncidentException when it fails to save the incident report
		* successfully.
		*/
		virtual void save() =0;
		/**
		* Reloads the data from the database.
		*/
		virtual void reload() =0;
		/**
		* Returns true if the data has been modified since the last database fetch/commit.
		* 
		* @return true if the data has been modified since the last database fetch/commit.
		* 
		*/
		virtual bool isModified() =0;
		/**
		* Returns true if the incident is not yet available in the database.
		*/
		virtual bool isNew() =0;
		
		
		/**
		* long getCreatorProfileId
		* 
		* get the creator id
		* 
		* @return virtual unsigned 
		* 
		* @exception <exceptions> Optional
		*/
		virtual unsigned long getCreatorProfileId() =0;
		
		//Mintao ++
		//Bug 778 (TD12858)
		virtual std::string getStationId() = 0;
		//Mintao ++
		//Bug 778 (TD12858)

		virtual std::string getDisplayReportId() = 0;

		virtual bool isStationReportValid() = 0;

    };
}// end TA_IRS_Core
#endif

