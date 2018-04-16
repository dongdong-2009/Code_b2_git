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

#include "bus/trains/TrainCommonLibrary/src/CctvTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace CctvTypes
    {

        std::string cctvSwitchModeToString( ECctvSwitchMode cctvSwitchMode )
        {
            switch ( cctvSwitchMode )
            {

                case CctvInactive:
                    return "Inactive";
                    break;

                case CctvSequence:
                    return "Sequence";
                    break;

                case CctvCamera:
                    return "Camera";
                    break;

                case CctvQuad:
                    return "Quad";
                    break;

                default:
                    break;
            }

            return "Unknown ECctvSwitchMode";
        }


        std::string CctvSwitchState::getLogString() const
        {
            std::ostringstream result;

            result << "CctvSwitchState [trainId: "
                   << static_cast< unsigned int>( trainId )
                   << "][preConflictTrain: " << static_cast< unsigned int>( preConflictTrain )
                   << "][stageId: " << stageId
                   << "][cctvState: " << cctvSwitchModeToString( cctvState )
                   << "]";

            switch ( cctvState )
            {

                case CctvSequence:
                    result << "["
                           << ProtocolCctvTypes::cctvSequenceNumberToString( static_cast< ProtocolCctvTypes::ECctvSequenceNumber >( item1 ) )
                           << "]";
                    break;

                case CctvCamera:
                    result << "["
                    << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( item1 ) )
                    << "]";
                    break;

                case CctvQuad:
                    result << "["
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( item1 ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( item2 ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( item3 ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( item4 ) ) << "]";
                    break;

                case CctvInactive:
                    // fall through

                default:
                    break;
            }

            return result.str();
        }


        // Note this should call a templated function with a function parameter to do the conversion to string
        // However visual c++ 6 has issues with templates of templates
        std::string trainCameraSequenceToString( const TrainCameraSequence& trainCameraSequence )
        {
            std::ostringstream result;

            for ( TrainCameraSequence::const_iterator iter = trainCameraSequence.begin();
                  trainCameraSequence.end() != iter; ++iter )
            {
                if ( trainCameraSequence.begin() != iter )
                {
                    result << ", ";
                }

                result << ProtocolCctvTypes::cctvCameraToString( *iter );
            }

            return result.str();
        }

    }

}
