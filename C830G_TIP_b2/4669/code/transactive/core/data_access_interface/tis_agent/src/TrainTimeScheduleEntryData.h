#ifndef TRAIN_TIME_SCHEDULE_ENTRY_DATA_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_ENTRY_DATA_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryData.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This contains the database retrieval methods
  * for Train Time Schedule entries for the Traveller Information System (TIS) Agent.
  */

#include "core/data_access_interface/tis_agent/src/ITrainTimeScheduleEntry.h"
#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryHelper.h"
#include <string>

namespace TA_IRS_Core
{
	
    class TrainTimeScheduleEntryData : public ITrainTimeScheduleEntry
    {
    public:

        /**
         * TrainTimeScheduleEntryData
         *
         * Standard constructor.
         * Used when creating a new entry
         *
         */
        TrainTimeScheduleEntryData();

        /**
         * TrainTimeScheduleEntryData
         *
         * Standard constructor.
         *
         * @param pkey the pkey of the entry to load      
         */
        TrainTimeScheduleEntryData( unsigned long pkey );

        /**
         * TrainTimeScheduleEntryData
         *
         * Copy constructor.
         * Used when creating a new entry
         */
        TrainTimeScheduleEntryData( const TrainTimeScheduleEntryData& ttseData );

        /**
         * ~TrainTimeScheduleEntryData
         *
         * Standard destructor.
         */

        virtual ~TrainTimeScheduleEntryData();


        /**
         * getKey
         *
         * Gets the pkey of the time schedule entry.
         * Can only be called if this is not a new schedule entry
         *
         * @return      The primary key.
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
        virtual unsigned long getKey();

        /**
         * getStartTime
         *
         * Gets the start time of the time schedule entry
         *
         * @return      The start time.
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
		virtual time_t getStartTime();

        /**
         * getEndTime
         *
         * Gets the end time of the time schedule entry
         *
         * @return      The end time.
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
		virtual time_t getEndTime();

        /**
         * getRepeatInterval
         *
         * Gets the repeat interval of the time schedule entry
         *
         * @return      The repeat interval.
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
		virtual unsigned short getRepeatInterval();

        /**
         * getMessageTable
         *
         * Gets the message table to use for the time schedule entry
         *
         * @return      The message table.
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
        virtual MessageTableType getMessageTable();

        /**
         * getMessageId
         *
         * Gets the id of the message in the message table to use for the time schedule entry
         *
         * @return      The message id.
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
        virtual unsigned short getMessageId();

        /**
         * getPriority
         *
         * Gets the priority of the time schedule entry
         *
         * @return      The message priority.
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
        virtual unsigned short getPriority();

                    
        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        virtual void invalidate();

        /**
         * setParentKey
         *
         * Sets the pkey of the time schedule that is the parent of this entry.
         *
         * @param   parentKey  the pkey of the time schedule that is the parent of this entry
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
        virtual void setParentKey(unsigned long parentKey);


        /**
         * setStartTime
         *
         * Sets the start time of the time schedule entry
         *
         * @param   startTime   The start time.
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
		virtual void setStartTime(time_t startTime);

        /**
         * setEndTime
         *
         * Sets the end time of the time schedule entry
         *
         * @param   endTime The end time.
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
		virtual void setEndTime(time_t endTime);

        /**
         * setRepeatInterval
         *
         * Sets the repeat interval of the time schedule entry
         *
         * @param   repeatInterval  The repeat interval.
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
		virtual void setRepeatInterval(unsigned short repeatInterval);

        /**
         * setMessageTable
         *
         * Sets the message table to use for the time schedule entry
         *
         * @param   messageTable    The message table.
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
        virtual void setMessageTable(MessageTableType messageTable);

        /**
         * setMessageId
         *
         * Sets the id of the message in the message table to use for the time schedule entry
         *
         * @param   messageId   The message id.
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
        virtual void setMessageId(unsigned short messageId);

        /**
         * setPriority
         *
         * Sets the priority of the time schedule entry
         *
         * @param   priority    The message priority.
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
        virtual void setPriority(unsigned short priority);

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


    private:

        // The database access used to do the IO
        TrainTimeScheduleEntryHelper*   m_trainTimeScheduleEntryHelper;

    };

} //end namespace TA_IRS_Core

#endif
