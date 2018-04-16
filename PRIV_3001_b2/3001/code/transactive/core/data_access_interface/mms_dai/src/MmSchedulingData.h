/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/mms_dai/src/MmSchedulingData.h $
 * @author:  Cameron Rochester
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 * 
 * MmSchedulingData is an implementation of IMmSchedulingData. It holds the data specific to 
 * alarm plan associations.
 */

#if !defined(MM_SCHEDULING_DATA_H)
#define MM_SCHEDULING_DATA_H

#include <string>

#include "core/data_access_interface/mms_dai/src/IMmSchedulingData.h"

namespace TA_IRS_Core
{
	class MmSchedulingHelper;	

	class MmSchedulingData : public IMmSchedulingData
	{

	public:

		/**
		 * MmSchedulingData 
		 *
		 * Construct a MmSchedulingData class for the entry represented by the specified data.
		 * 
		 *
		 * @param key The key of record in the database
		 * @param subsystemKey: the key of the associated physical subsystem
         * @param time_t scheduledTime: the time the subsystem generates MMS messages
         * @param time_t lastRunDate: the last time/date that the subsystem was reported
		 * @param subName: the name of the physical subsystem
		 */
		MmSchedulingData( unsigned long key, 
						  unsigned long subsystemKey,
						  time_t scheduledTime,
                          time_t lastRunDate,
						  const std::string& subName);


		virtual ~MmSchedulingData();
	
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

	private: //data

		MmSchedulingHelper* m_helper;

	private: //methods:

		MmSchedulingData& operator=(const MmSchedulingData &);
		MmSchedulingData( const MmSchedulingData& theMmScheduling);




	};
} // closes TA_IRS_Core

#endif // !defined(MM_SCHEDULING_DATA_H)
