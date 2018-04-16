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

#if !defined(TrainInformationTypes_H)
#define TrainInformationTypes_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include <string>
#include <vector>
#include <map>

namespace TA_IRS_Bus
{

    namespace TrainInformationTypes
    {

        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:26 PM
         */
        enum EChangeType
        {
            /**
             * A saved selection list has been added/updated
             */
            SelectionListUpdate = 0,
            /**
             * A saved selection list has been deleted
             */
            SelectionListDeletion = 1
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:27 PM
         */

        struct TrainSelectionUpdate
        {
            EChangeType change;

            std::string selectionName;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Train selection list management
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:27 PM
         */
        typedef std::vector< std::string > TrainSelectionList;


        /**
         * Non corba representation of the ATS track
         * @author Adam Radics
         * @version 1.0
         * @created 13-Nov-2007 3:50:27 PM
         */
        enum EAtsTrack
        {
            AtsTrackUnknown = 0,
            AtsTrackInner = 1,
            AtsTrackOuter = 2
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:27 PM
         */

        struct TrainDetails
        {
            /**
             * The physical train number
             */
            CommonTypes::TrainIdType trainId;

            /**
             * Whether the train is sending OA1/ATC tables over radio
             */
            bool communicationValid;

            /**
             * This is where the train is currently sending OA1/ATC tables.
             * If the communication is not valid, this is the last place they were sent,
             * or 0 if they were never sent.
             */
            unsigned long communicatingLocation;

            /**
             * The current primary ITSI of the train (the one currently communicating)
             */
            std::string primaryTsi;

            /**
             * The current secondary ITSI of the train (the one not currently communicating)
             */
            std::string secondaryTsi;

            /**
             * Whether the train is reported in ATS or not.
             */
            bool atsValid;

            /**
             * If the ATS is valid, is the localisation information held by ATS valid
             */
            bool isLocalisationValid;

            /**
             * The current location key of the train, or the last known location if the train
             * is not valid.
             */
            unsigned long currentLocation;

            /**
             * The CCTV zone of the train (not taking into acount pre conflict or conflict)
             */
            unsigned long currentCctvZone;

            /**
             * The service number of the train, or the last known service number if ats is not
             * valid.
             */
            std::string serviceNumber;

            /**
             * Whether the train is mute or not (ATS flag)
             */
            bool isMute;

            /**
             * The track the train is on (from ATS)
             */
            EAtsTrack track;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;


            /**
             * Compares for equality
             */
            bool operator==( const TrainDetails& rhs ) const;
            bool operator!=( const TrainDetails& rhs ) const;
        };


        /**
         * @author Robert van Hugten
         * @version 1.0
         * @created 18-Mar-2008 3:18:39 PM
         */
        typedef std::vector< TrainDetails > TrainDetailsList;


        /**
         * @author adamr
         * @version 1.0
         * @created 18-Mar-2008 3:18:40 PM
         */
        typedef std::map< CommonTypes::TrainIdType, TrainDetails > TrainDetailsMap;


        /**
         * This is the operation mode of the agent and includes what areas it is in
         * control of. It can be queried and is sent out as an update.
         * @author Robert van Hugten
         * @version 1.0
         * @created 13-Nov-2007 3:50:27 PM
         */

        struct AgentOperationMode
        {

            bool fallbackMode;

            bool doubleAtsFailure;

            bool groupOffline;

            bool localDuty;

            std::set< unsigned long > controlledLocations;

            time_t lastUpdateTime;

            /**
             * Prints the contents into a formatted string for logging purposes
             */
            std::string getLogString() const;
        };


        /**
         * Utility functions to print simple types to string
         */
        std::string changeTypeToString( EChangeType changeType );
        std::string trainSelectionListToString( const TrainSelectionList& trainSelectionList );
        std::string atsTrackToString( EAtsTrack atsTrack );
        std::string trainDetailsListToString( const TrainDetailsList& trainDetailsList );
        std::string trainDetailsMapToString( const TrainDetailsMap& trainDetailsMap );

    }

}

#endif // !defined(TrainInformationTypes_H)
