#ifndef TRAIN_TIME_SCHEDULE_DATA_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleData.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Train Time Schedules
  * and their associated schedule entries, for the Traveller Information System (TIS) Agent.
  * It implements the ITrainTimeSchedule interface.
  */

#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleHelper.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryData.h"

namespace TA_IRS_Core
{
    class TrainTimeScheduleData : public ITrainTimeSchedule
    {
    public:

        /**
         * TrainTimeScheduleData
         *
         * Standard constructor for a new time schedule.
         */
        TrainTimeScheduleData();


        /**
         * TrainTimeScheduleData
         *
         * Standard constructor.
         *
         * @param pkey the primary key for the time schedule
         */
        TrainTimeScheduleData( unsigned long pkey );


        /**
         * TrainTimeScheduleData
         *
         * Copy constructor for a new time schedule.
         */
        TrainTimeScheduleData( TrainTimeScheduleData& ttsData );


        /**
         * ~TrainTimeScheduleData
         *
         * Standard destructor.
         */
        virtual ~TrainTimeScheduleData();

        
        /**
         * getKey
         *
         * Gets the primary key for this time schedule.
         *
         * @return      the primary key
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
        virtual unsigned long getKey();


        /**
         * getName
         *
         * Gets the name of this time schedule.
         *
         * @return      the name
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if the data cannot be converted to the correct format.
         */
		virtual std::string getName();

        /**
         * getAdHocMessageTitle
         *
         * Gets the specified ad-hoc message title in this time schedule.
         *
         * @param messageNumber the message id.
         *
         * @return      the message title.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual std::string getAdHocMessageTitle(AdhocMessageNumber messageNumber);


		/**
         * getAdHocMessage
         *
         * Gets the specified ad-hoc message in this time schedule.
         *
         * @param messageNumber the message id.
         *
         * @return      the message string.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual std::string getAdHocMessage(AdhocMessageNumber messageNumber);

        /**
         * setName
         *
         * sets the name of the time schedule.
         *
         * @param       name    The name of the schedule.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual void setName(std::string name);

        /**
         * setAdHocMessage
         *
         * Sets the specified ad-hoc message in this time schedule.
         *
         * @param messageNumber the message id.
         * @param message       the message string.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual void setAdHocMessage(AdhocMessageNumber messageNumber,
                                     std::string message);


        /**
         * setAdHocMessageTitle
         *
         * Sets the specified ad-hoc message title in this time schedule.
         *
         * @param messageNumber the message id.
         * @param messageTitle  the message title.
         *
         * @exception   TA_Base_Core::DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */
        virtual void setAdHocMessageTitle(AdhocMessageNumber messageNumber,
                                          std::string messageTitle);

        /** 
         * getEntries
         *
         * Gets all the train time schedule entries belonging to this
         * time schedule. A vector of pointers is used, these pointers
         * belong to the time schedule, any changes to the entries
         * are applied when the time schedule is applied.
         * The objects pointed to by these pointers will be deleted when
         * the time schedule is deleted.
         *
         *
         * @return A vector of pointers to objects conforming to the ITrainTimeScheduleEntry interface
         */
        virtual std::vector<ITrainTimeScheduleEntry*> getEntries();


        /** 
         * createNewEntry
         *
         * As above, but creates a new entry that can be configured and is saved
         * when the time schedule is saved.
         *
         * @return a pointer to an object conforming to the ITrainTimeScheduleEntry interface
         */
        virtual ITrainTimeScheduleEntry* createNewEntry();


        /** 
         * deleteEntry
         *
         * Delete an existing entry from a train time schedule.
         * The caller is not responsible for deleting the object.
         *
         * @param entryToDelete A pointer to the time schedule entry to delete.
         *
         */
        virtual void deleteEntry(ITrainTimeScheduleEntry* entryToDelete);


        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception TA_Base_Core::DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges();

        /**
         * deleteThisObject
         *
         * Removes this time schedule entry from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception TA_Base_Core::DatabaseException A TA_Base_Core::DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this entry was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This entry has not been deleted
         */
        virtual void deleteThisObject();

        /**
		  * invalidate
		  *
		  * Marks the data contained by this object as invalid. The next call to get...() 
		  * following a call to invalidate() will cause all the data to be reloaded from
		  * the database.
          * 
          * Any entries returned from getEntries will also become invalid.
          * reload all entries after calling this method using getEntries()
		  */
		virtual void invalidate();


    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        TrainTimeScheduleData& operator=( const TrainTimeScheduleData& );

        /** 
          * reloadEntries
          *
          * Loads all the time schedule entries for this time schedule
          *
          */
        void reloadEntries();


        // the object that does the actual database work
        TrainTimeScheduleHelper* m_trainTimeScheduleHelper;

        // a train time schedule will have 0 or more train time schedule
        // entries
        std::vector<TrainTimeScheduleEntryData*> m_trainTimeScheduleEntries;

        // These are the time schedule entries to be removed when the time schedule is saved
        std::vector<TrainTimeScheduleEntryData*> m_trainTimeScheduleEntriesToRemove;

        // a flag that indicates whether the time schedule entries have been loaded or not
        bool m_entriesAreValid;

    };

} //end namespace TA_IRS_Core

#endif
