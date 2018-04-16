#ifndef TRAIN_TIME_SCHEDULE_ENTRY_HELPER_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_ENTRY_HELPER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleEntryHelper.h $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TrainTimeScheduleEntryHelper contains the get and set methods that do the 
  * actual database work for the TrainTimeScheduleEntryData class.
  * It accesses both the ti_train_time_schedule and ti_train_time_schedule_entry DB tables.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>

#include "core/data_access_interface/tis_agent/src/ITrainTimeScheduleEntry.h"

namespace TA_IRS_Core
{
    class TrainTimeScheduleEntryHelper
    {
    public:
        
        /**
         * TrainTimeScheduleEntryHelper
         * 
         * This constructor creates a new TrainTimeScheduleEntryHelper.
         * All writeable fields must be set before saving.
         *
         * @param       unsigned long TrainTimeScheduleEntryPKey
         *              The database key for the TrainTimeScheduleEntry parent of this helper.
         */
        TrainTimeScheduleEntryHelper();

       /**
         * TrainTimeScheduleEntryHelper
         * 
         * This constructor creates a new TrainTimeScheduleEntryHelper for the specified key.
         * The configuration is loaded from the database when first requested.
         *
         * @param       unsigned long TrainTimeScheduleEntryPKey
         *              The database key for the TrainTimeScheduleEntry parent of this helper.
         */
        TrainTimeScheduleEntryHelper( unsigned long trainTimeScheduleEntryPKey );

        /**
          * TrainTimeScheduleEntryHelper
          *
          * Standard copy constructor.
          */
        TrainTimeScheduleEntryHelper( const TrainTimeScheduleEntryHelper& theTrainTimeScheduleEntryHelper );

        /**
         * ~TrainTimeScheduleEntryHelper
         *
         * Standard destructor.
         */
        virtual ~TrainTimeScheduleEntryHelper() {};

        /**
         * getKey
         *
         * Gets the pkey of the time schedule entry.
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
         * getParentKey
         *
         * Gets the pkey of the time schedule that is the parent of this entry.
         *
         * @return      The primary key of the time schedule owning this entry.
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
        virtual unsigned long getParentKey();


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
        virtual ITrainTimeScheduleEntry::MessageTableType getMessageTable();

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
        virtual void setMessageTable(ITrainTimeScheduleEntry::MessageTableType messageTable);

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
         * invalidate
         *
         * Marks the data contained by this TrainTimeScheduleEntry as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this TrainTimeScheduleEntry was initially loaded from the database
         *                  OR     - writeTrainTimeScheduleEntry() has already been called, as it does 
         *                           not make any sense to invalidate a TrainTimeScheduleEntry that has not 
         *                           yet been written to the database.
         */
	    virtual void invalidate();


        /** 
         * writeTrainTimeSheduleEntryData
         *
         * Writes the modified TrainTimeScheduleEntry to the database.
         * It is added if it is new, updated if it is existing.
         *
         * @exception TA_Base_Core::DatabaseException A TA_Base_Core::DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        virtual void writeTrainTimeSheduleEntryData();

        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged() {return m_hasChanged;}


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew() {return m_isNew;}

        /**
         * deleteTrainTimeSheduleEntryData
         *
         * Removes this time schedule from the database. 
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
        virtual void deleteTrainTimeSheduleEntryData();

    private:
        //
        // Disable the assignment operator.
        //

		TrainTimeScheduleEntryHelper& operator=( const TrainTimeScheduleEntryHelper& );
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this TrainTimeScheduleEntry was initially loaded from the database
         *                  OR     - writeTrainTimeScheduleEntryData() has already been called.
         */
	    void reload();


        /** 
         * addNewTrainTimeScheduleEntry
         *
         * write a new train time schedule entry to the database
         *
         * @exception TA_Base_Core::DatabaseException A TA_Base_Core::DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewTrainTimeScheduleEntry();

        /** 
         * updateExistingTrainTimeScheduleEntry
         *
         * update an existing train time schedule entry in the database
         *
         * @exception TA_Base_Core::DatabaseException A TA_Base_Core::DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void updateExistingTrainTimeScheduleEntry();

        //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new TrainTimeScheduleEntry or an existing one
        // loaded from the database.
        //
        bool m_isNew;

        //
        // Flag to indicate whether the dat ahas changed
        //
        bool m_hasChanged;
        
        //
        // The TrainTimeScheduleEntry database key
        //
        unsigned long m_key;

        // 
        // The key of the parent time schedule
        //
        unsigned long m_parentScheduleKey;

        // 
        // The start and end time of the time schedule
        //
		time_t m_startTime;
		time_t m_endTime;

        // 
        // The repeat interval
        //
		unsigned short m_repeatInterval;

        // 
        // The message to use
        //
		ITrainTimeScheduleEntry::MessageTableType m_messageTable;
		unsigned short m_messageId;

        // 
        // The priority of the time schedule
        //
		unsigned short m_priority;
    };

} // closes TA_Base_Core

#endif
