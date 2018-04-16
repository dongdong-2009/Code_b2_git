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
#if !defined(CommonTypes_H)
#define CommonTypes_H

#include "core/defaults/src/DefaultEnvironment.h"

#include <set>
#include <string>

namespace TA_IRS_Bus
{

    namespace CommonTypes
    {

        /**
         * Typedef for a train id, a train id is a 1 byte number, so an unsigned char.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:44 PM
         */
        typedef unsigned char TrainIdType;

        
        /**
         * A list of train IDs
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:44 PM
         */
        typedef std::set< TrainIdType > TrainIdList;

        
        /**
         * This is a forward declaration of the train resource enumeration.
         * 
         * The Protocol specific component of the train agent will define this enumeration
         * and the values that are applicable to that implementation.
         * 
         * Note the value -1 is used by CoreTrainState - it is a special case and is not
         * covered by the enumeration, although the protocol layer can include this in the
         * definition of the enumeration.
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:44 PM
         */
        enum ETrainResource
        {
            CoreResource = -1,
            TrainAlarmResource = 0,
            TrainCctvResource = 1,
            TrainTisResource = 2,
            TrainPaResource = 3,
            TrainCallResource = 4,
            TrainPecResource = 5
        };


        /**
         * Indicates the failure type of the Radio operation
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:44 PM
         */
        enum ERadioErrorType
        {
            RadioCallInvalid = 0,
            CallSetupFailed = 1,
            CallSelectFailed = 2,
            CallEndFailed = 3,
            DgnaFailed = 4
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string trainIdListToString( const TrainIdList& trainIdList );
        std::string trainResourceToString( ETrainResource trainResource );
        std::string radioErrorTypeToString( ERadioErrorType radioErrorType );
    }

}
#endif // !defined(CommonTypes_H)
