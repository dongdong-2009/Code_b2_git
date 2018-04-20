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

#include "bus/trains/TrainCommonLibrary/src/RadioGroupTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace RadioGroupTypes
    {

        std::string groupLocationListToString( const GroupLocationList& groupLocationList )
        {
            return listToString< GroupLocationList >( groupLocationList );
        }


        std::string TrainRadioGroup::getLogString() const
        {
            std::ostringstream result;

            result << "TrainRadioGroup [groupName: " << groupName
                   << "][groupTsi: " << groupTsi
                   << "][locked: " << locked
                   << "][lastUpdateTime: " << lastUpdateTime
                   << "][locationCoverage: " << listToString< GroupLocationList >( locationCoverage )
                   << "]";

            return result.str();
        }


        std::string trainRadioGroupMapToString( const TrainRadioGroupMap& trainRadioGroupMap )
        {
            std::ostringstream result;

            for ( TrainRadioGroupMap::const_iterator iter = trainRadioGroupMap.begin();
                  trainRadioGroupMap.end() != iter; ++iter )
            {
                if ( trainRadioGroupMap.begin() != iter )
                {
                    result << std::endl;
                }

                result << iter->second.getLogString();
            }

            return result.str();
        }

    }

}

