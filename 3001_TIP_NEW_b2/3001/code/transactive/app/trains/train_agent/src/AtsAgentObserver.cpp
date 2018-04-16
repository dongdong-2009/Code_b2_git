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


#include "app/trains/train_agent/src/AtsAgentObserver.h"
#include "app/trains/train_agent/src/IAtsAgentInterface.h"
#include "app/trains/train_agent/src/IAtsMessageProcessor.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"

#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/OperationNotSupportedException.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

    AtsAgentObserver::AtsAgentObserver( ITrainAgentConfiguration& configuration,
                                        IAtsMessageProcessor& atsMessageProcessor,
                                        IAtsAgentInterface& atsAgentInterface )
            : m_atsAgentInterface( atsAgentInterface ),
              m_atsMessageProcessor( atsMessageProcessor )
    {
        FUNCTION_ENTRY( "AtsAgentObserver" );

        //subscribes to comms messages using subscribeToMessages
        subscribeToMessages( configuration );

        //then load the initial state using loadInitialState
        loadInitialState();

        FUNCTION_EXIT;
    }


    AtsAgentObserver::~AtsAgentObserver()
    {
        FUNCTION_ENTRY( "~AtsAgentObserver" );

        unsubscribeToMessages();

        FUNCTION_EXIT;
    }


    void AtsAgentObserver::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // extract the message into the ATS Train Information structure (defined in the ATS IDL)
        TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef* atsTrainMessageCorba = NULL;

        if ( payload.messageState >>= atsTrainMessageCorba )
        {
            // convert from the corba type to the C++ type, and pass the data onto m_atsMessageProcessor
            AtsTrainInformation cppTrainInfo;
            cppTrainInfo.physicalTrainNumber = atsTrainMessageCorba->physicalTrainNumber;
            cppTrainInfo.trainValid = atsTrainMessageCorba->trainValid;
            cppTrainInfo.localisation.currentCctvZone = atsTrainMessageCorba->localisation.currentCctvZone;
            cppTrainInfo.localisation.currentLocation = atsTrainMessageCorba->localisation.currentLocation;
            cppTrainInfo.localisation.isMute = atsTrainMessageCorba->localisation.isMute;
            cppTrainInfo.localisation.lastLocation = atsTrainMessageCorba->localisation.lastLocation;
            cppTrainInfo.localisation.localisationValid = atsTrainMessageCorba->localisation.localisationValid;
            cppTrainInfo.localisation.physicalTrainService = atsTrainMessageCorba->localisation.physicalTrainService;

            switch ( atsTrainMessageCorba->localisation.track )
            {

                case TA_IRS_Bus::TrackInnerbound:
                    // fall through

                case TA_IRS_Bus::TrackCentralInnerbound:
                    cppTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackInner;
                    break;

                case TA_IRS_Bus::TrackOuterbound:
                    // fall through

                case TA_IRS_Bus::TrackCentralOuterbound:
                    cppTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackOuter;
                    break;

                case TA_IRS_Bus::TrackUnknown:
                    // fall through

                default:
                    cppTrainInfo.localisation.track = TA_IRS_Bus::TrainInformationTypes::AtsTrackUnknown;
                    break;
            }

            cppTrainInfo.localisation.upDirection = atsTrainMessageCorba->localisation.upDirection;

            m_atsMessageProcessor.processAtsInformation( cppTrainInfo );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                         "Received unknown message content" );
        }

        FUNCTION_EXIT;
    }


    void AtsAgentObserver::loadInitialState()
    {
        FUNCTION_ENTRY( "loadInitialState" );

        try
        {
            // load the initial states from the ats agent using m_atsAgentInterface->getTrainInformationList
            AtsTrainInformationList trainInformationList = m_atsAgentInterface.getTrainInformationList();

            // for each entry returned, pass the structure onto m_atsMessageProcessor

            for ( AtsTrainInformationList::iterator iter = trainInformationList.begin();
                  iter != trainInformationList.end(); ++iter )
            {
                m_atsMessageProcessor.processAtsInformation( *iter );
            }
        }

        //catch any exceptions and log them
        catch ( TA_IRS_Bus::OperationNotSupportedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::OperationNotSupportedException", e.what() );
        }
        catch ( TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );
        }

        FUNCTION_EXIT;
    }


    void AtsAgentObserver::subscribeToMessages( ITrainAgentConfiguration& configuration )
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        // subscribe to the following message types:
        // ATSAgentComms::AtsTrainUpdate
        // using the m_atsAgentInterface->getAtsEntitykey() and the current location key (from the configuration)

        TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::ATSAgentComms::AtsTrainUpdate,
                this,
                m_atsAgentInterface.getAtsEntityKey(),
                0,
                configuration.getLocationKey() );

        FUNCTION_EXIT;
    }


    void AtsAgentObserver::unsubscribeToMessages()
    {
        FUNCTION_ENTRY( "unsubscribeToMessages" );

        // unsubscribe from comms messages
        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        FUNCTION_EXIT;
    }


}// TA_IRS_App
