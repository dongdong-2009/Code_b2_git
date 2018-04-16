/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace TrainInformationTypes
    {

        std::string changeTypeToString( EChangeType changeType )
        {
            switch ( changeType )
            {

                case SelectionListUpdate:
                    return "SelectionListUpdate";
                    break;

                case SelectionListDeletion:
                    return "SelectionListDeletion";
                    break;

                default:
                    break;
            }

            return "Unknown EChangeType";
        }


        std::string TrainSelectionUpdate::getLogString() const
        {
            std::ostringstream result;

            result << "TrainSelectionUpdate [change: " << changeTypeToString( change )
                   << "][selectionName: " << selectionName
                   << "]";

            return result.str();
        }


        std::string trainSelectionListToString( const TrainSelectionList& trainSelectionList )
        {
            return listToString< TrainSelectionList >( trainSelectionList );
        }


        std::string atsTrackToString( EAtsTrack atsTrack )
        {
            switch ( atsTrack )
            {

                case AtsTrackUnknown:
                    return "Unknown";
                    break;

                case AtsTrackInner:
                    return "IT";
                    break;

                case AtsTrackOuter:
                    return "OT";
                    break;

                default:
                    break;
            }

            return "Unknown EAtsTrack";
        }


        std::string TrainDetails::getLogString() const
        {
            std::ostringstream result;

            result << "TrainDetails [trainId: " << static_cast< unsigned int>( trainId )
                   << "][communicationValid: " << communicationValid
                   << "][communicatingLocation: " << communicatingLocation
                   << "][primaryTsi: " << primaryTsi
                   << "][secondaryTsi: " << secondaryTsi
                   << "][atsValid: " << atsValid
                   << "][isLocalisationValid: " << isLocalisationValid
                   << "][currentLocation: " << currentLocation
                   << "][currentCctvZone: " << currentCctvZone
                   << "][serviceNumber: " << serviceNumber
                   << "][isMute: " << isMute
                   << "][track: " << atsTrackToString( track )
                   << "]";

            return result.str();
        }


        bool TrainDetails::operator==( const TrainDetails& rhs ) const
        {
            return ( ( trainId == rhs.trainId ) &&
                     ( communicationValid == rhs.communicationValid ) &&
                     ( communicatingLocation == rhs.communicatingLocation ) &&
                     ( primaryTsi == rhs.primaryTsi ) &&
                     ( secondaryTsi == rhs.secondaryTsi ) &&
                     ( atsValid == rhs.atsValid ) &&
                     ( isLocalisationValid == rhs.isLocalisationValid ) &&
                     ( currentLocation == rhs.currentLocation ) &&
                     ( currentCctvZone == rhs.currentCctvZone ) &&
                     ( serviceNumber == rhs.serviceNumber ) &&
                     ( isMute == rhs.isMute ) &&
                     ( track == rhs.track ) );
        }


        bool TrainDetails::operator!=( const TrainDetails& rhs ) const
        {
            return !( *this == rhs );
        }


        std::string trainDetailsListToString( const TrainDetailsList& trainDetailsList )
        {
            return structListToString< TrainDetailsList >( trainDetailsList );
        }


        std::string trainDetailsMapToString( const TrainDetailsMap& trainDetailsMap )
        {
            std::ostringstream result;

            for ( TrainDetailsMap::const_iterator iter = trainDetailsMap.begin();
                  trainDetailsMap.end() != iter; ++iter )
            {
                if ( trainDetailsMap.begin() != iter )
                {
                    result << std::endl;
                }

                result << iter->second.getLogString();
            }

            return result.str();
        }


        std::string AgentOperationMode::getLogString() const
        {
            std::ostringstream result;

            result << "AgentOperationMode [fallbackMode: " << fallbackMode
                   << "][doubleAtsFailure: " << doubleAtsFailure
                   << "][groupOffline: " << groupOffline
                   << "][localDuty: " << localDuty
                   << "][lastUpdateTime: " << lastUpdateTime
                   << "][controlledLocations: " << listToString< std::set< unsigned long > >( controlledLocations )
                   << "]";

            return result.str();
        }

    }

}


