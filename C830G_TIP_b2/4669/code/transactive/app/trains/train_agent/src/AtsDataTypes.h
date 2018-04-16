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
#if !defined(AtsDataTypes_H)
#define AtsDataTypes_H

#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"

namespace TA_IRS_App
{

    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:18 PM
     */
    struct LocalisationData
    {

        /**
         * Whether or not the trains location data is valid
         */
        bool localisationValid;


        /**
         * Physical Train Number/service e.g. 45/123
         */
        std::string physicalTrainService;


        /**
         * Station or depot location key
         */
        unsigned long currentLocation;


        /**
         * Station or depot location key
         */
        unsigned long lastLocation;


        /**
         * CCTV Zone ID
         */
        double currentCctvZone;


        /**
         * direction of train travel
         */
        bool upDirection;


        /**
         * Track - innerbound or outerbound
         */
        TA_IRS_Bus::TrainInformationTypes::EAtsTrack track;


        /**
         * isTalkative flag from ATS
         */
        bool isMute;

        
        /**
         * Prints the contents into a formatted string for logging purposes
         */
        std::string getLogString() const;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:18 PM
     */
    struct AtsTrainInformation
    {

        /**
         * Physical Train number
         */
        TA_IRS_Bus::CommonTypes::TrainIdType physicalTrainNumber;


        /**
         * Whether there is any train information in ATS
         */
        bool trainValid;


        /**
         * Localisation data - may or may not be valid
         */
        LocalisationData localisation;


        /**
         * Prints the contents into a formatted string for logging purposes
         */
        std::string getLogString() const;

    };


    /**
     * @author Adam Radics
     * @version 1.0
     * @created 01-Apr-2008 2:47:18 PM
     */
    typedef std::vector< AtsTrainInformation > AtsTrainInformationList;


    std::string atsTrainInformationListToString( const AtsTrainInformationList& atsTrainInformationList );

} // TA_IRS_App

#endif // !defined(AtsDataTypes_H)
