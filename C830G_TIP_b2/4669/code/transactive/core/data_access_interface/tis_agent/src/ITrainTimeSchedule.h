#ifndef ITRAIN_TIME_SCHEDULE_H_INCLUDED
#define ITRAIN_TIME_SCHEDULE_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This interface contains the database retrieval and write methods
  * for Train Time Schedules for the Traveller Information System (TIS) Agent.
  */

#include "core/data_access_interface/tis_agent/src/ITrainTimeScheduleEntry.h"

#include <string>
#include <vector>

namespace TA_IRS_Core
{

    class ITrainTimeSchedule
    {
    public:

        /**
         * ~ITrainTimeSchedule
         *
         * Standard destructor.
         */
        virtual ~ITrainTimeSchedule() {};

        /**
         * getName
         *
         * Gets the name of the time schedule.
         *
         * @return      std::string
         *              The name of the schedule.
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
        virtual std::string getName() = 0;

        /**
         * getKey
         *
         * Gets the primary key of this time schedule.
         * Can only be called if this is not a new schedule
         *
         * @return      the primary key.
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
        virtual unsigned long getKey() = 0;


        enum AdhocMessageNumber
        {
            Adhoc1 = 1,
            Adhoc2 = 2,
            Adhoc3 = 3,
            Adhoc4 = 4,
            Adhoc5 = 5,
            Adhoc6 = 6
        };


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
        virtual std::string getAdHocMessageTitle(AdhocMessageNumber messageNumber) = 0;


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
        virtual std::string getAdHocMessage(AdhocMessageNumber messageNumber) = 0;


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
        virtual void setName(std::string name) = 0;


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
                                     std::string message) = 0;


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
                                          std::string messageTitle) = 0;


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
        virtual std::vector<ITrainTimeScheduleEntry*> getEntries() = 0;


        /** 
         * createNewEntry
         *
         * As above, but creates a new entry that can be configured and is saved
         * when the time schedule is saved.
         *
         * @return a pointer to an object conforming to the ITrainTimeScheduleEntry interface
         */
        virtual ITrainTimeScheduleEntry* createNewEntry() = 0;


        /** 
         * deleteEntry
         *
         * Delete an existing entry from a train time schedule.
         * The caller is not responsible for deleting the object.
         *
         * @param entryToDelete A pointer to the time schedule entry to delete.
         *
         */
        virtual void deleteEntry(ITrainTimeScheduleEntry* entryToDelete) = 0;


        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged() =0;


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew() =0;

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
        virtual void applyChanges() =0;

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
        virtual void deleteThisObject() = 0;

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
        virtual void invalidate() = 0;

    };

} //end namespace TA_IRS_Core

#endif
