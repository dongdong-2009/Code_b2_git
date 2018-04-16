#ifndef TRAIN_TIME_SCHEDULE_HELPER_H_INCLUDED
#define TRAIN_TIME_SCHEDULE_HELPER_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleHelper.h $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TrainTimeScheduleHelper contains the get and set methods that do the 
  * actual database work for the TrainTimeScheduleData class.
  * It accesses both the ti_train_time_schedule table.
  */

#pragma warning ( disable : 4786 )

#include <map>
#include <climits>

#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"

#define NUM_ADHOC_MSGS 6

namespace TA_IRS_Core
{
    class TrainTimeScheduleHelper
    {
    public:
        
        /**
         * TrainTimeScheduleHelper
         * 
         * This constructor creates a new TrainTimeScheduleHelper
         *
         */
        TrainTimeScheduleHelper();

       /**
         * TrainTimeScheduleHelper
         * 
         * This constructor creates a TrainTimeScheduleHelper for the specified key.
         *
         * @param       unsigned long trainTimeSchedulePKey
         *              The database key for the TrainTimeSchedule parent of this helper.
         */
        TrainTimeScheduleHelper( unsigned long trainTimeSchedulePKey );

        /**
         * TrainTimeScheduleHelper
         * 
         * Copy constructor
         */
        TrainTimeScheduleHelper( const TrainTimeScheduleHelper& timeScheduleHelper);

        /**
         * ~TrainTimeScheduleHelper
         *
         * Standard destructor.
         */
        virtual ~TrainTimeScheduleHelper() {};

        /**
         * getKey
         *
         * Returns the TrainTimeSchedule key for this TrainTimeSchedule entry.
         *
         * Preconditions:   Either - this VideoOutputGroup entry was initially 
         *                           loaded from the database
         *                  OR     - writeVideoOutputGroupData() has already been called
         *
         * @return      unsigned long
         *              The key for this VideoOutputGroup.
         */
	    unsigned long getKey();

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
         * @param messageNumber the 0 based message index.
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
        virtual std::string getAdHocMessageTitle(unsigned short messageNumber);

		/**
         * getAdHocMessage
         *
         * Gets the specified ad-hoc message in this time schedule.
         *
         * @param messageNumber the 0 based message index.
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
        virtual std::string getAdHocMessage(unsigned short messageNumber);

        /**
         * invalidate
         *
         * Marks the data contained by this TrainTimeSchedule as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this TrainTimeSchedule was initially loaded from the database
         *                  OR     - writeTrainTimeSchedule() has already been called, as it does 
         *                           not make any sense to invalidate a TrainTimeSchedule that has not 
         *                           yet been written to the database.
         */
	    virtual void invalidate();


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
         * @param messageNumber the 0 based message index.
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
        virtual void setAdHocMessage(unsigned short messageNumber,
                                     std::string message);


        /**
         * setAdHocMessageTitle
         *
         * Sets the specified ad-hoc message title in this time schedule.
         *
         * @param messageNumber the 0 based message index.
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
        virtual void setAdHocMessageTitle(unsigned short messageNumber,
                                          std::string messageTitle);

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
         * writeTrainTimeSchedule
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
        virtual void writeTrainTimeSchedule();

        /**
         * deleteTrainTimeSchedule
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
        virtual void deleteTrainTimeSchedule();

        /** 
          * getEntryPkeys
          *
          * Gets the pkeys of time schedule entries associated with this schedule
          *
          * @return a vector of pkeys. Empty if none available.
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
        std::vector<unsigned long> getEntryPkeys();

    private:
        //
        // Disable the assignment operator.
        //

		TrainTimeScheduleHelper& operator=( const TrainTimeScheduleHelper& );
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this TrainTimeSchedule was initially loaded from the database
         *                  OR     - writeTrainTimeScheduleData() has already been called.
         */
	    void reload();

        /** 
         * addNewTrainTimeSchedule
         *
         * write a new train time schedule to the database
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
        void addNewTrainTimeSchedule();

        /** 
         * updateExistingTrainTimeSchedule
         *
         * update an existing train time schedule in the database
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
        void updateExistingTrainTimeSchedule();

        //
        // Flag to indicate if the data needs reloading.
        //
	    bool m_isValidData;

        //
        // Flag to indicate if this is a new TrainTimeSchedule or an existing one
        // loaded from the database.
        //
        bool m_isNew;

        //
        // Flag to indicate whether the data has changed
        //
        bool m_hasChanged;
        
        //
        // The TrainTimeSchedule database key
        //
        unsigned long m_key;

        //
        // The name of the train time schedule entry
        //
        std::string m_name;


        //
        // the 6 ad-hoc messages ans titles
        //
        std::string m_adhocMessages[NUM_ADHOC_MSGS];
        std::string m_adhocMessageTitles[NUM_ADHOC_MSGS];
    };

} // closes TA_Base_Core

#endif
