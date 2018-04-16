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
#if !defined(IRightsChecker_H)
#define IRightsChecker_H

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

#include <string>

namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:47:43 PM
     */
    class IRightsChecker
    {

    public:

        /**
         * Checks to see if the given operator has rights to control cctv functions
         *
         * Note that due to conflict management rules, the train location control is checked separately
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canControlCctv( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to display a TTIS free text message
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDisplayTtisFreeText( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to display a TTIS predefined message
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDisplayTtisPredefined( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to perform a TTIS reset
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canResetTtisMessage( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to download/upgrade the TTIS library
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDownloadTtisLibrary(TA_IRS_Bus::CommonTypes::TrainIdType trainId,const std::string& sessionId ) = 0;
		virtual bool canDownloadTtisLibrary(const std::string& sessionId ) = 0;

        /**
         * Checks to see if the given operator has rights to download/upgrade the TTIS schedule
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDownloadTtisSchedule( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to make a PA live broadcast
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaLive( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to make a PA DVA broadcast on the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaDva( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to make a Cyclic PA DVA broadcast on the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaCyclicDva( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to call or answer a call from trains
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canCallTrain( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to answer or reset a PEC from on the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param trainId    The train to check rights for
         * @param sessionId    The operator session ID
         */
        virtual bool canAnswerPec( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                   const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to modify train selection lists
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canModifySelectionList( const std::string& sessionId ) = 0;


        /**
         * Checks to see if the given operator has rights to forcefuly take control of trains at this location.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canForceTrainControl( const std::string& sessionId ) = 0;

    };

} // TA_IRS_App

#endif // !defined(IRightsChecker_H)
