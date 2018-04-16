/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert V
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(RadioGroupTypes_H)
#define RadioGroupTypes_H

#include "core/defaults/src/DefaultEnvironment.h"

#include <map>
#include <set>
#include <string>

namespace TA_IRS_Bus
{

    namespace RadioGroupTypes
    {

        /**
         * @author Robert V
         * @version 1.0
         * @created 13-Nov-2007 3:50:49 PM
         */
        typedef std::set< unsigned long > GroupLocationList;


        /**
         * @author Robert V
         * @version 1.0
         * @created 13-Nov-2007 3:50:49 PM
         */
        struct TrainRadioGroup
        {
            /**
             * A string of group name
             */
            std::string groupName;

            /**
             * A string of groupTsi
             */
            std::string groupTsi;
			/**
             * GroupID unsigned long
             */
            unsigned long groupId;

            /**
             * A list of group location
             */
            GroupLocationList locationCoverage;

            /**
             * A bool to identify the lock is locked or not
             */
            bool locked;

            /**
             * last updated time
             */
            time_t lastUpdateTime;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

			friend bool operator < (const TrainRadioGroup& left, const TrainRadioGroup& right)
			{
				return (left.groupId < right.groupId);
			}
        };


        /**
         * This is a map of group TSI to group information.
         * @author Robert V
         * @version 1.0
         * @created 13-Nov-2007 3:50:49 PM
         */
        typedef std::map< std::string, TrainRadioGroup > TrainRadioGroupMap;


        /**
         * Utility functions to print simple types to string
         */
        std::string groupLocationListToString( const GroupLocationList& groupLocationList );
        std::string trainRadioGroupMapToString( const TrainRadioGroupMap& trainRadioGroupMap );

    }

}

#endif // !defined(RadioGroupTypes_H)
