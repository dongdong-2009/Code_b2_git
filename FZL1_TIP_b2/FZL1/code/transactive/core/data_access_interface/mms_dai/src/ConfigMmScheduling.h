/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/mms_dai/src/ConfigMmScheduling.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * ConfigMmScheduling is an implementation of IConfigMmScheduling. It holds the data specific to 
 * alarm plan associations and allows reads and writes.
 */

#if !defined(CONFIG_MM_SCHEDULING_H)
#define CONFIG_MM_SCHEDULING_H

#include <string>

#include "core/data_access_interface/mms_dai/src/IConfigMmScheduling.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingData.h"

namespace TA_IRS_Core
{
	class MmSchedulingHelper;
	
	class ConfigMmScheduling : public IConfigMmScheduling
	{

	public:

		/**
		 * ConfigMmScheduling (constructor)
		 *
		 * Construct a ConfigMmScheduling class for the entry represented by the specified data.
		 * The ConfigMmScheduling will be constructed with valid data.
		 *
		 * @param unsigned long key The key of this record in the database
		 * @param unsigned long subsystemKey: The physical subsystem
         * @param time_t scheduledTime 
         * @param time_t lastRunTime
		 */
		ConfigMmScheduling( unsigned long key, unsigned long subsystemKey, 
			time_t scheduledTime, time_t lastRunTime, const std::string& subName);


		virtual ~ConfigMmScheduling();
	
	 // IItem methods:

		/**
         * getKey
         *
         * Returns the key for this MmSchedulingData
         *
         * @return The key for this MmSchedulingData as an unsigned long.
         */
		virtual unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this item. For alarm plan associations this is a name built up from the
         * entity or entity type and the alarm type.
         *
         * @return The name for this item as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();

		/**
         * invalidate
         *
         * Mark the data contained by this Alarm type as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

	 // end IItem methods
		
     // IMmSchedulingData methods:

		/** 
		  * getSubsystemName
		  *
		  * Returns the subsystem name for this record
		  *
		  * @return std::string subsystem name for this record
		  */
		virtual std::string getSubsystemName();

		/** 
		  * getSubsystemKey
		  *
		  * Returns the subsystem key for this record
		  *
		  * @return unsigned long key of the subsystem
		  */
		virtual unsigned long getSubsystemKey();

		/** 
		  * getLastReported
		  *
		  * Returns the date/time this subsystem was last reported
		  *
		  * @return time_t indicating the date/time this subsystem was last reported
		  */
		virtual time_t getLastReported();

		/** 
		  * getScheduledTime
		  *
		  * Returns the time of day this subsytem should be reported
		  *
		  * @return time_t indicating the time of day this subsytem should be reported
		  */
		virtual time_t getScheduledTime();


	// IConfigMmScheduling methods:
		
        
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
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
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
		  * setScheduledTime
		  *
		  * Sets the time of day that this subsystem will generate periodic maintenance messages
		  *
		  * @param time_t The time the subsystem should run
		  */
        virtual void setScheduledTime(time_t newTime);

        /** 
		  * setLastRunTime
		  *
		  * Sets the time of day that this subsystem was last reported
		  *
		  * @param time_t The time the subsystem was last reported
		  */
        virtual void setLastReportedTime(time_t newTime);

	private: //data

		MmSchedulingHelper* m_helper;

	private:
		ConfigMmScheduling& operator=(const ConfigMmScheduling &);

        /**
         * updateChanges
         *
         * This updates the map recording the changes to the alarm plan association. Whenever a call to set
         * is made in this class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param unsigned long - The old key that was stored for this attribute
         * @param unsigned long - The new key to be stored for this attribute
         */
        void updateChanges(const std::string& name, unsigned long oldKey, unsigned long newKey);

        bool m_isValidData;

	};
} // end TA_IRS_Core

#endif // !defined(CONFIG_MM_SCHEDULING_H)
