/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(TrainAgentSubscriber_H)
#define TrainAgentSubscriber_H

#include "app/signs/tis_agent/src/ITrainEventCallback.h"

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolTtisTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TrainInformationTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainInformationUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/ITrainTisUpdateObserver.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"
#include "bus/trains/TrainCorbaProxy/src/TrainTisCorbaProxy.h"

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_IRS_App
{

    class TrainAgentSubscriber : public TA_IRS_Bus::ITrainTisUpdateObserver,
                                 public TA_IRS_Bus::ITrainInformationUpdateObserver
    {

    public:

        /**
         * constructor
         */
        TrainAgentSubscriber(TA_IRS_App::ITrainEventCallback& callback);


        /**
         * destructor
         */
        virtual ~TrainAgentSubscriber();


        /**
         * Given a train ID, get the current details for it.
         * The train might not be valid or at a valid location, but the current known
         * details will be given.
         *
         * @return the details of the given train
         *
         * @exception InvalidTrainException if the train ID given is not known
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetails getTrainDetails(const TA_IRS_Bus::CommonTypes::TrainIdType& trainId);


        /**
         * Get details about all known valid trains.
         *
         * This will get the current details about all known trains, that are valid -
         * meaning they are communicating either by radio, or they are present in ATS.
         *
         * @return a list of all valid trains
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap getTrainList();


        /**
         * This checks to see if a given train is under this agent area control.
         * If this is the depot agent, it is depot trains, if it is the OCC agent it is
         * mainline trains.
         *
         * @return if the train is under this agents control or not
         */
        bool isTrainUnderAgentAreaControl(TA_IRS_Bus::CommonTypes::TrainIdType trainId);


        /**
         * This checks to see if a given train is under this agent area control.
         * If this is the depot agent, it is depot trains, if it is the OCC agent it is
         * mainline trains.
         *
         * @return if the train is under this agents control or not
         */
        bool isTrainUnderAgentAreaControl(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails);


        /**
         * This will receive a TisLibraryVersionUpdate message from the local train agent.
         */
        virtual void processTisLibraryVersionUpdate(const TA_IRS_Bus::TtisTypes::TisLibraryVersionUpdate& event);


        /**
         * This will receive a TisLibraryUpdate message from the local train agent.
         */
        virtual void processTisLibraryUpdate(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate&  event);


        /**
         * This will receive a TisReturnStatus message from the local train agent.
         */
        virtual void processTisReturnStatus(const TA_IRS_Bus::TtisTypes::TisReturnStatus&  event);


        /**
         * This will receive a TrainDetails message from the local train agent.
         */
        void processTrainDetails(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& event);


        /**
         * This will receive a AgentOperationMode message from the local train agent.
         */
        void processAgentOperationMode(const TA_IRS_Bus::TrainInformationTypes::AgentOperationMode& event);


        /**
         * This will receive a TrainSelectionUpdate message from the local train agent.
         */
        void processTrainSelectionUpdate(const TA_IRS_Bus::TrainInformationTypes::TrainSelectionUpdate& event);

    private:

        /**
         * constructor
         */
        TrainAgentSubscriber();


        /**
         * Gets the initial version list from the train agent
         */
        void initialiseInitialVersions();


        /**
         * Converts the enumeration status into a printable string, suitable for user
         * display.
         */
        std::string convertDownloadResultToString(TA_IRS_Bus::TtisTypes::EDownloadResult downloadResult);


        /**
         * Converts the enumeration status into a printable string, suitable for user
         * display.
         */
        std::string convertCommandResultToString(TA_IRS_Bus::TtisTypes::ETtisCommandResult commandStatus);

		std::string convertDownloadStatusToString(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update);

		std::string convertUpgradStatusToString(const TA_IRS_Bus::TtisTypes::TisLibraryUpdate& update);


        /**
         * This will convert the original display parameters returned from the train agent
         * to native TIS types so they can be logged.
         */
        TA_Base_Core::TTISFreeTextMessageParameters convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters& trainParameters);


        /**
         * This will convert the original display parameters returned from the train agent
         * to native TIS types so they can be logged.
         */
        TA_Base_Core::TTISMessageResetParameters convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisResetParameters& trainParameters);


        TA_Base_Core::TrainPIDList convertPidListToTis(const TA_IRS_Bus::TtisTypes::TtisPidList& pidList);


        /**
         * This will convert the original display parameters returned from the train agent
         * to native TIS types so they can be logged.
         */
        TA_Base_Core::TTISPredefinedMessageParameters convertTrainParametersToTisParameters(const TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters& trainParameters);


        TA_Base_Core::ETTISClearType convertResetMessageTypeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisResetMessageType messageType);


        TA_Base_Core::ELibrarySection convertLibrarySectionToTis(unsigned char identifierType);


        TA_Base_Core::EPIDSelection convertPidToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisPid pid);


        TA_Base_Core::ETTISLEDFontSize convertFontSizeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize fontSize);


        TA_Base_Core::EJustification convertJustificationToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisJustification justification);


        TA_Base_Core::ETTISMessagePriority convertMessagePriorityToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority priority);


        TA_Base_Core::EDisplayMode convertDisplayModeToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisDisplayMode displayMode);


        TA_Base_Core::ETTISLEDIntensity convertIntensityToTis(TA_IRS_Bus::ProtocolTtisTypes::ETtisIntensity intensity);


        std::string convertStartTimeToTis(const TA_IRS_Bus::TtisTypes::TimeInterval& displayPeriod);


        std::string convertTimeToTis(unsigned char hour, unsigned char minute);


        std::string convertEndTimeToTis(const TA_IRS_Bus::TtisTypes::TimeInterval& displayPeriod);
		void reloadTrainsDetails();
        /**
         * Status string
         */
        static const std::string SUCCESS_STATUS;
		static const std::string TRAIN_LIBRARY_DOWNLOAD_SUCCESS_STATUS;
		static const std::string TRAIN_LIBRARY_UPGRADE_SUCCESS_STATUS;
		static const std::string TRAIN_LIBRARY_DOWNLOAD_TIME_OUT;


        /**
         * Status string
         */
        static const std::string TTIS_IN_USE_STATUS;


        /**
         * Status string
         */
        static const std::string TTIS_RESET_STATUS;


        /**
         * Status string
         */
        static const std::string TTIS_FAILED_STATUS;


        /**
         * Status string
         */
        static const std::string TTIS_DIFFERENT_VERSION_STATUS;


        /**
         * Status string
         */
        static const std::string INVALID_TRAIN_ERROR_STATUS;


        /**
         * Status string
         */
        static const std::string TRANSMISSION_FAILURE_ERROR_STATUS;


        /**
         * Status string
         */
        static const std::string TRANSACTION_TIMEOUT_ERROR_STATUS;


        /**
         * Status string
         */
        static const std::string TRANSACTION_CANCELLED_ERROR_STATUS;


        /**
         * Status string
         */
        static const std::string BAD_COMMAND_BAD_CHECKSUM_STATUS;


        /**
         * Status string
         */
        static const std::string BAD_COMMAND_UNKNOWN_COMMAND_STATUS;


        /**
         * Status string
         */
        static const std::string BAD_COMMAND_BAD_PARAMETER_STATUS;


        /**
         * Status string
         */
        static const std::string BAD_COMMAND_UNEXPECTED_COMMAND_STATUS;


        /**
         * Status string
         */
        static const std::string MPU_CHANGEOVER_ERROR_STATUS;


        /**
         * Status string
         */
        static const std::string TRANSFER_FAILED_STATUS;


        /**
         * Status string
         */
        static const std::string TIMED_OUT_STATUS;


        /**
         * Status string
         */
        static const std::string TIMS_FAILED_STATUS;


		static const std::string TTIS_TRAIN_FAILED_STATUS;
        /**
         * Listener to events from the local train agent
         */
        TA_IRS_App::ITrainEventCallback& m_callback;


        /**
         * An interface to the local train agent Information interface
         */
        TA_IRS_Bus::TrainInformationCorbaProxyPtr m_trainInformationCorbaProxy;


        /**
         * An interface to the local train agent TIS interface
         */
        TA_IRS_Bus::TrainTisCorbaProxyPtr m_trainTisCorbaProxy;


        /**
         * A map of all known train information
         */
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap m_trainDetails;


        TA_Base_Core::ReEntrantThreadLockable m_trainDetailsLock;


        TA_Base_Core::NonReEntrantThreadLockable m_downloadLock;

    };

}

#endif // !defined(TrainAgentSubscriber_H)
