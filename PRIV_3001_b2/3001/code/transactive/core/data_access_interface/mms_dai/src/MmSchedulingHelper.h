/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/mms_dai/src/MmSchedulingHelper.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * MmSchedulingHelper provides read/write support the the MM_SCHEDULING table.
  */


#if !defined(MmSchedulingHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
#define MmSchedulingHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_


#include <string>
#include <ctime>

namespace TA_IRS_Core
{

    class MmSchedulingHelper
    {

    public:


		/**
		 * MmSchedulingHelper 
		 *
		 * Construct a MmSchedulingHelper class for the entry represented by the specified data.
		 * 
		 *
		 * @param key The key of record in the database
		 * @param subsystemKey: the key of the associated physical subsystem
         * @param time_t scheduledTime: the time the subsystem generates MMS messages
         * @param time_t lastRunDate: the last time/date that the subsystem was reported
		 * @param subName: the name of the physical subsystem
		 */
		MmSchedulingHelper( unsigned long key, 
						  unsigned long subsystemKey,
						  time_t scheduledTime,
                          time_t lastRunDate,
						  const std::string& subName);



        /**
         * Constructor
         *
         * Construct a MmSchedulingHelper class based around an existing association. This will
		 * copy all the paramters of the existing alarm plan association.
         *
         * @param rhs The alarm plan association to copy
         */
		MmSchedulingHelper( const MmSchedulingHelper& rhs);
		

        /**
         * Destructor
         */
        virtual ~MmSchedulingHelper();



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
         * Make the data contained by this alarm plan association as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

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

	//IConfigMmScheduling methods:

  
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


    private:
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... 
         * or set... method is called and the data state is not valid.
         *
         * pre: Either - this entity was initially loaded from the database
         *      OR     - writeEntityData() has already been called
         */
        void reload();


        /**
         * modifyExisting
         *
         * This will update an existing alarm plan association in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the alarm plan association object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExisting();

		//not implimented
        MmSchedulingHelper& operator=(const MmSchedulingHelper &);

		unsigned long m_key;
		unsigned long m_subsystemKey;
		std::string m_subsystemName;
		time_t m_scheduledTime;
        time_t m_lastRunDate;

		//original data, prior to calling set methods
		unsigned long m_subsystemKeyOrig;
		std::string m_subsystemNameOrig;
		time_t m_scheduledTimeOrig;
        time_t m_lastRunDateOrig;

        bool m_isValidData;
    };
} // closes TA_IRS_Core

#endif // !defined(MmSchedulingHelper_B14A02F4_8549_4115_916A_CC81775C1C32__INCLUDED_)
