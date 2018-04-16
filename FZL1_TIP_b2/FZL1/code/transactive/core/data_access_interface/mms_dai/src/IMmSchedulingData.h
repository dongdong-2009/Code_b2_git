/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/mms_dai/src/IMmSchedulingData.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * IMmSchedulingData is an interface to the MM_SCHEDULING table. It provides read-only
  * access for existing MmSchedulingiations
  */

#if !defined(IMM_SCHEDULING_DATA_H)
#define IMM_SCHEDULING_DATA_H

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IItem.h"

namespace TA_IRS_Core
{
    class IMmSchedulingData : public TA_Base_Core::IItem
	{
	public:
	
		/** 
		  * getSubsystemName
		  *
		  * Returns the subsystem name for this record
		  *
		  * @return std::string subsystem name for this record
		  */
		virtual std::string getSubsystemName() = 0;

		/** 
		  * getSubsystemKey
		  *
		  * Returns the subsystem key for this record
		  *
		  * @return unsigned long key of the subsystem
		  */
		virtual unsigned long getSubsystemKey() = 0;

		/** 
		  * getLastReported
		  *
		  * Returns the date/time this subsystem was last reported
		  *
		  * @return time_t indicating the date/time this subsystem was last reported
		  */
		virtual time_t getLastReported() = 0;

		/** 
		  * getScheduledTime
		  *
		  * Returns the time of day this subsytem should be reported
		  *
		  * @return time_t indicating the time of day this subsytem should be reported
		  */
		virtual time_t getScheduledTime() = 0;


		virtual ~IMmSchedulingData() {}

	};

} // end TA_IRS_Core

#endif // IMM_SCHEDULING_DATA_H

