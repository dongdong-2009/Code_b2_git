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
#if !defined(CctvTypes_H)
#define CctvTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"

#include <vector>
#include <string>

namespace TA_IRS_Bus
{
    
    namespace CctvTypes
    {

        /**
         * The CCTV state of a train (on/off/sequence/cam)
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:36 PM
         */
        enum ECctvSwitchMode
        {
            CctvInactive = 0,
            CctvSequence = 1,
            CctvCamera = 2,
            CctvQuad = 3
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:36 PM
         */
        struct CctvSwitchState
        {

            /**
             * The train ID
             */
            CommonTypes::TrainIdType trainId;

            /**
             * The train ID this train is in pre conflict with, 0 if no pre conflict
             */
            CommonTypes::TrainIdType preConflictTrain;

            /**
             * The stage the train is in
             */
            unsigned long stageId;

            /**
             * The current state of the cctv
             */
            CctvTypes::ECctvSwitchMode cctvState;
            unsigned char item1;
            unsigned char item2;
            unsigned char item3;
            unsigned char item4;


            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;

        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:36 PM
         */
        typedef std::vector< ProtocolCctvTypes::ECctvCamera > TrainCameraSequence;

        
        /**
         * Utility functions to print simple types to string
         */
        std::string cctvSwitchModeToString( ECctvSwitchMode cctvSwitchMode );
        std::string trainCameraSequenceToString( const TrainCameraSequence& trainCameraSequence );

    }

}

#endif // !defined(CctvTypes_H)
