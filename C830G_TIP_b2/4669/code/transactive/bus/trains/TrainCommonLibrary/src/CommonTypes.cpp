/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace CommonTypes
    {

        std::string trainIdListToString( const TrainIdList& trainIdList )
        {
            return integerListToString< TrainIdList >( trainIdList );
        }


        std::string trainResourceToString( ETrainResource trainResource )
        {
            switch ( trainResource )
            {

                case CoreResource:
                    return "Comms";
                    break;

                case TrainAlarmResource:
                    return "Alarm";
                    break;

                case TrainCctvResource:
                    return "CCTV";
                    break;

                case TrainTisResource:
                    return "TIS";
                    break;

                case TrainPaResource:
                    return "PA";
                    break;

                case TrainCallResource:
                    return "Call";
                    break;

                case TrainPecResource:
                    return "PEC";
                    break;

                default:
                    break;
            }

            return "Unknown ETrainResource";
        }


        std::string radioErrorTypeToString( ERadioErrorType radioErrorType )
        {
            switch ( radioErrorType )
            {

                case RadioCallInvalid:
                    return "RadioCallInvalid";
                    break;

                case CallSetupFailed:
                    return "CallSetupFailed";
                    break;

                case CallSelectFailed:
                    return "CallSelectFailed";
                    break;

                case CallEndFailed:
                    return "CallEndFailed";
                    break;

                case DgnaFailed:
                    return "DgnaFailed";
                    break;

                default:
                    break;
            }

            return "Unknown ERadioErrorType";
        }

    }

}

