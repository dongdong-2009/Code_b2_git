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

#include "app/trains/train_agent/src/AtsMessageProcessor.h"

#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"


namespace TA_IRS_App
{

    AtsMessageProcessor::AtsMessageProcessor( ITrainStatusManager& statusManager )
            : m_statusManager( statusManager )
    {
        FUNCTION_ENTRY( "AtsMessageProcessor" );

        start();

        FUNCTION_EXIT;
    }


    AtsMessageProcessor::~AtsMessageProcessor()
    {
        FUNCTION_ENTRY( "~AtsMessageProcessor" );

        terminateAndWait();

        FUNCTION_EXIT;
    }


    void AtsMessageProcessor::processAtsInformation( const AtsTrainInformation& trainInformation )
    {
        FUNCTION_ENTRY( "processAtsInformation" );

        // Just call insert with the item (it needs to be copied with new and put into a boost shared pointer)
        boost::shared_ptr<AtsTrainInformation> copyOfInformation( new AtsTrainInformation( trainInformation ) );
        insert( copyOfInformation );

        FUNCTION_EXIT;
    }


    void AtsMessageProcessor::processEvent( boost::shared_ptr<AtsTrainInformation> newEvent )
    {
        FUNCTION_ENTRY( "processEvent" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                     "Received %s", newEvent->getLogString().c_str() );

        try
        {
        // Get the TrainCommsState from m_statusManager (for physicalTrainNumber)
        TrainCommsStatePtr state = m_statusManager.getTrainCommsState( newEvent->physicalTrainNumber );

        AtsTrainInformation oldInformation;

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( state );

            // populate oldInformation with the data stored in the train state, and where detail is missing, use the new information:
            oldInformation.physicalTrainNumber = newEvent->physicalTrainNumber;
            oldInformation.trainValid = state->isAtsValid();
            oldInformation.localisation.localisationValid = state->isLocalisationValid();
            oldInformation.localisation.physicalTrainService = state->getServiceNumber();
            oldInformation.localisation.currentLocation = state->getLocation();
            oldInformation.localisation.lastLocation = state->getLocation(); // not used, so use the current location
            oldInformation.localisation.currentCctvZone = ( state->getCurrentCctvZone() )(); // use the double operator
            oldInformation.localisation.upDirection = newEvent->localisation.upDirection;
            oldInformation.localisation.track = state->getAtsTrack();
            oldInformation.localisation.isMute = state->isMute();

            // set the new train information
            if ( true == newEvent->trainValid )
            {
                state->setAtsDetails( newEvent->localisation.currentLocation,
                                      newEvent->localisation.currentCctvZone,
                                      newEvent->localisation.physicalTrainService,
                                      newEvent->localisation.track,
                                      newEvent->localisation.isMute,
                                      newEvent->localisation.localisationValid );
            }
            else
            {
                state->invalidateAts();
            }

            // release the state lock
        }

        // now the observers must be notified.
        // use getObservers() then iterate through the observers, giving them the old and new train information.
        ObserverList observers = getObservers();

        for ( ObserverList::iterator iter = observers.begin(); iter != observers.end(); ++iter )
        {
            ( *iter )->processAtsUpdate( oldInformation, *newEvent );
            }
        }
        catch ( TA_IRS_Bus::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App
