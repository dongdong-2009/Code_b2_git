/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $Source$
 * @author:  adamr
 * @version: $Revision$
 *
 * Last modification: $Date$
 * Last modified by:  $Author$
 *
 */

#include "app/trains/train_agent/src/AtsDataTypes.h"

#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_App
{

    std::string LocalisationData::getLogString() const
    {
        std::ostringstream result;

        result << "[localisationValid: " << localisationValid
               << "][physicalTrainService: " << physicalTrainService
               << "][currentLocation: " << currentLocation
               << "][lastLocation: " << lastLocation
               << "][currentCctvZone: " << currentCctvZone
               << "][upDirection: " << upDirection
               << "][track: " << TA_IRS_Bus::TrainInformationTypes::atsTrackToString( track )
               << "][isMute: " << isMute
               << "]";

        return result.str();
    }


    std::string AtsTrainInformation::getLogString() const
    {
        std::ostringstream result;

        result << "AtsTrainInformation: [TrainID: " << static_cast< unsigned long >( physicalTrainNumber )
               << "][trainValid: " << trainValid
               << "]"
               << localisation.getLogString();

        return result.str();
    }


    std::string atsTrainInformationListToString( const AtsTrainInformationList& atsTrainInformationList )
    {
        return structListToString< AtsTrainInformationList >( atsTrainInformationList );
    }

} // TA_IRS_App


