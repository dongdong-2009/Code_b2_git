#ifndef ITRAIN_TIME_SCHEDULE_ENTRY_H_INCLUDED
#define ITRAIN_TIME_SCHEDULE_ENTRY_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/ITrainTimeScheduleEntry.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This interface contains the database retrieval and write methods
  * for Train Time Schedule entries for the Traveller Information System (TIS) Agent.
  *
  * A train time schedule entry is part of a train time schedule. It can only be saved
  * when saving a train time schedule, that is why there is no way to apply changes using
  * this interface.
  *
  * Time schedule entries are created, deleted, saved etc using the time schedule interface,
  * as it makes no sense to save a time schedule entry before saving a time schedule.
  */

#include <string>

namespace TA_IRS_Core
{
	
    class ITrainTimeScheduleEntry
    {
    public:

        typedef enum {EmergencyPreDefined = 0, NormalPreDefined = 1, FreeText = 9} MessageTableType;

    public:

        /**
         * ~ITrainTimeScheduleEntry
         *
         * Standard destructor.
         */

        virtual ~ITrainTimeScheduleEntry() {};


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
        virtual unsigned long getKey() = 0;

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
		virtual time_t getStartTime() = 0;

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
		virtual time_t getEndTime() = 0;

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
		virtual unsigned short getRepeatInterval() = 0;

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
        virtual MessageTableType getMessageTable() = 0;

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
        virtual unsigned short getMessageId() = 0;

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
        virtual unsigned short getPriority() = 0;

                    
        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        virtual void invalidate() = 0;

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
		virtual void setStartTime(time_t startTime) = 0;

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
		virtual void setEndTime(time_t endTime) = 0;

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
		virtual void setRepeatInterval(unsigned short repeatInterval) = 0;

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
        virtual void setMessageTable(MessageTableType messageTable) = 0;

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
        virtual void setMessageId(unsigned short messageId) = 0;

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
        virtual void setPriority(unsigned short priority) = 0;

    };

} //end namespace TA_IRS_Core

#endif
