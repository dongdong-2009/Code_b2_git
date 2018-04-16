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
#if !defined(RightsChecker_H)
#define RightsChecker_H

#include "app/trains/train_agent/src/IOperationModeManager.h"
#include "app/trains/train_agent/src/IRightsChecker.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"

#include "bus/security/rights_library/public_interfaces/ItaRights.h"


namespace TA_IRS_App
{

    /**
     * @author adamr
     * @version 1.0
     * @created 01-Apr-2008 2:48:03 PM
     */
    class RightsChecker : public IRightsChecker
    {

    public:

        /**
         * Constructor
         *
         * @param configuration    Agent configuration object
         * @param itaRights    Transactive simple rights checking utility
         * @param operationModeManager    Used to check agent control of a location
         */
        RightsChecker( ITrainAgentConfiguration& configuration,
                       TA_Base_Bus::ItaRights& itaRights,
                       IOperationModeManager& operationModeManager );


        /**
         * Destructor
         */
        virtual ~RightsChecker();


        /**
         * Checks to see if the given operator has rights to control cctv functions
         *
         * Note that due to conflict management rules, the train location control is checked separately
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canControlCctv( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to display a TTIS free text message
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDisplayTtisFreeText( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to display a TTIS predefined message
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDisplayTtisPredefined( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to perform a TTIS reset
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canResetTtisMessage( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to download/upgrade the TTIS library
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDownloadTtisLibrary( TA_IRS_Bus::CommonTypes::TrainIdType trainId,const std::string& sessionId );
		virtual bool canDownloadTtisLibrary(const std::string& sessionId );

        /**
         * Checks to see if the given operator has rights to download/upgrade the TTIS schedule
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canDownloadTtisSchedule( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to make a PA live broadcast to the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaLive( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to make a PA DVA broadcast on the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param trainId    The train to check rights for
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaDva( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to make a Cyclic PA DVA broadcast on the given train
         *
         * Will check for access rights, and whether this train is controlled by this agent.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canBroadcastPaCyclicDva( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to call or answer a calls
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canCallTrain( const std::string& sessionId );


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
                                   const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to modify train selection lists
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canModifySelectionList( const std::string& sessionId );


        /**
         * Checks to see if the given operator has rights to forcefuly take control of trains at this location.
         *
         * @return true if the operator can perform the action, false otherwise
         *
         * @param sessionId    The operator session ID
         */
        virtual bool canForceTrainControl( const std::string& sessionId );


    private:


        /**
         * Constructor
         */
        RightsChecker();


        /**
         * This will do an actual rights check, and log the result.
         *
         * @return true if the action is allowed, false otherwise
         *
         * @param sessionId    The operator trying to perform the action
         * @param actionKey    The action the operator is trying to perform
         * @param actionString    The action name the operator is trying to perform
         * @param trainId    The train the action is being performed on, 0 if not a specific train
         */
        bool isActionPermitted( const std::string& sessionId,
                                unsigned long actionKey,
                                const std::string& actionString,
                                TA_IRS_Bus::CommonTypes::TrainIdType trainId = 0 );


        /**
         * Used to load agent configuration
         */
        ITrainAgentConfiguration& m_configuration;


        /**
         * The actual utility used to check rights
         */
        TA_Base_Bus::ItaRights& m_taRights;


        /**
         * Used to check the location control of this agent
         */
        IOperationModeManager& m_operationModeManager;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string CONTROL_TRAINBORNE_CCTV;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_TIS_FREETEXT_MESSAGE;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_TIS_STORED_MESSAGE;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_TIS_RESET_MESSAGE;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_TIS_DOWNLOAD_LIBRARY;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_TIS_DOWNLOAD_SCHEDULE;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_PA_PRERECORDED_ANNOUNCEMENT;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_PA_CYCLIC_ANNOUNCEMENT;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_PA_LIVE_ANNOUNCEMENT;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string RADIO_MAKE_TRAIN_CALL;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string RADIO_ANSWER_PEC_CALL;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string TRAIN_MANUAL_TAKE_CONTROL;


        /**
         * Description of the access right for logging purposes
         */
        static const std::string RADIO_MANAGE_TRAIN_SELECTIONS;

    };

} // TA_IRS_App

#endif // !defined(RightsChecker_H)
