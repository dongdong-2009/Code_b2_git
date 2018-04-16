/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/mms_dai/src/IConfigMmScheduling.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * IConfigMmScheduling is an interface to the MM_SCHEDULING table. 
  *It provides read and write access for records.
  */

#if !defined(ICONFIG_MM_SCHEDULING_H)
#define ICONFIG_MM_SCHEDULING_H

#include <string>
#include "core/data_access_interface/mms_dai/src/IMmSchedulingData.h"
#include "core/data_access_interface/src/IConfigItem.h"

namespace TA_IRS_Core
{
    class IConfigMmScheduling : public IMmSchedulingData
	{
	public:

		virtual ~IConfigMmScheduling() {}
        
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
        virtual void applyChanges() =0;


        /** 
		  * setScheduledTime
		  *
		  * Sets the time of day that this subsystem will generate periodic maintenance messages
		  *
		  * @param time_t The time the subsystem should run
		  */
        virtual void setScheduledTime(time_t newTime) =0;

        /** 
		  * setLastRunTime
		  *
		  * Sets the time of day that this subsystem was last reported
		  *
		  * @param time_t The time the subsystem was last reported
		  */
        virtual void setLastReportedTime(time_t newTime) =0;

	};

} // end TA_IRS_Core

#endif // ICONFIG_MM_SCHEDULING_H

