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

#include "app/trains/train_agent/src/TrainStateHelper.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/utilities/src/DebugUtil.h"

// include this last, because it it really more of an implementation than a header file.
// in fact all the code can go inside this cpp file, but it is better if it is contained elsewhere
// for readability and maintainability
#include "app/trains/train_agent/src/TrainStateHelperTemplates.h"

namespace TA_IRS_App
{

    TA_IRS_Bus::CommonTypes::TrainIdList TrainStateHelper::getTrainsInCall( const TA_IRS_Bus::StateUpdateTypes::RadioCallDetails& callDetails,
                                                                            const ITrainStatusManager& trainStatusManager )
    {
        FUNCTION_ENTRY( "getTrainsInCall" );

        // get all the train states for the given resource type from the status manager
        TA_IRS_Bus::TrainIdToStateMap states = trainStatusManager.getAllTrainStates( callDetails.resourceType );

        switch ( callDetails.resourceType )
        {
            // valid types this operation works with

            case TA_IRS_Bus::CommonTypes::TrainPaResource:
                {
                    // create the FindTrainsWithCall object with the type SpecificStatePtr, and the parameters consoleId and callId.
                    // Use the object in the std::for_each function
                    FindTrainsWithCall< TrainPaStatePtr > result =
                        std::for_each( states.begin(),
                                       states.end(),
                                       FindTrainsWithCall< TrainPaStatePtr >( callDetails.consoleId, callDetails.callId ) );

                    FUNCTION_EXIT;
                    return result.getTrains();
                }
                break;

            case TA_IRS_Bus::CommonTypes::TrainCallResource:
                {
                    // create the FindTrainsWithCall object with the type SpecificStatePtr, and the parameters consoleId and callId.
                    // Use the object in the std::for_each function
                    FindTrainsWithCall< TrainCallStatePtr > result =
                        std::for_each( states.begin(),
                                       states.end(),
                                       FindTrainsWithCall< TrainCallStatePtr >( callDetails.consoleId, callDetails.callId ) );

                    FUNCTION_EXIT;
                    return result.getTrains();
                }
                break;

            case TA_IRS_Bus::CommonTypes::TrainPecResource:
                {
                    // create the FindTrainsWithCall object with the type SpecificStatePtr, and the parameters consoleId and callId.
                    // Use the object in the std::for_each function
                    FindTrainsWithCall< TrainPecStatePtr > result =
                        std::for_each( states.begin(),
                                       states.end(),
                                       FindTrainsWithCall< TrainPecStatePtr >( callDetails.consoleId, callDetails.callId ) );

                    FUNCTION_EXIT;
                    return result.getTrains();
                }
                break;

            // invalid types this operation can not be used with

            case TA_IRS_Bus::CommonTypes::CoreResource:
                // fall through

            case TA_IRS_Bus::CommonTypes::TrainAlarmResource:
                // fall through

            case TA_IRS_Bus::CommonTypes::TrainCctvResource:
                // fall through

            case TA_IRS_Bus::CommonTypes::TrainTisResource:
                // fall through

            default:
                std::ostringstream resource;
                resource << callDetails.resourceType;

                TA_THROW( TA_IRS_Bus::InvalidParameterException( "Invalid call type", "Resource", resource.str() ) );
                break;
        }

        TA_IRS_Bus::CommonTypes::TrainIdList emptyList;

        FUNCTION_EXIT;
        return emptyList;
    }


    TA_IRS_Bus::CommonTypes::TrainIdType TrainStateHelper::getTrainIdFromTsi( const std::string& tsi,
                                                                              const ITrainStatusManager& trainStatusManager )
    {
        FUNCTION_ENTRY( "getTrainIdFromTsi" );

        // go through all train comms states and see if any return true to isThisTrain()
        // Does this a smart way using std::find_if
        // For the comparison function, use TrainStateHelper::FindTrainWithTsi

        TA_IRS_Bus::TrainIdToStateMap commsStates = trainStatusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::CoreResource );

        TA_IRS_Bus::TrainIdToStateMap::const_iterator findIter = std::find_if( commsStates.begin(),
                                                                               commsStates.end(),
                                                                               FindTrainWithTsi( tsi ) );

        // if a train state pointer iterator is returned, get the train ID and return it, otherwise return 0
        if ( commsStates.end() != findIter )
        {
            FUNCTION_EXIT;
            return findIter->second->getTrainId();
        }

        FUNCTION_EXIT;
        return 0;
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TrainStateHelper::getTrainsInAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                                    bool continueRequired,
                                                                                    const ITrainStatusManager& trainStatusManager )
    {
        FUNCTION_ENTRY( "getTrainsInAnnouncement" );

        // get all the train states for TrainPaResource from the status manager
        TA_IRS_Bus::TrainIdToStateMap states = trainStatusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPaResource );

        // create the FindTrainsWithAnnouncementId object, and the parameter announcementId and continueRequired.
        // Use the object in the std::for_each function
        FindTrainsWithAnnouncementId result = std::for_each( states.begin(), states.end(),
                                                             FindTrainsWithAnnouncementId( announcementId, continueRequired ) );

        // return the train IDs collected by the FindTrainsWithAnnouncementId object

        FUNCTION_EXIT;
        return result.getTrains();
    }


    TA_IRS_Bus::StateUpdateTypes::RadioCallDetails TrainStateHelper::getCallDetailsForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                                                        const ITrainStatusManager& trainStatusManager )
    {
        FUNCTION_ENTRY( "getCallDetailsForLiveAnnouncement" );

        // get all the train states for TrainPaResource from the status manager
        TA_IRS_Bus::TrainIdToStateMap states = trainStatusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPaResource );

        // create the FindCallForAnnouncementAnnouncement object, with the parameter announcementId
        // Use the object in the std::find_if function
        TA_IRS_Bus::TrainIdToStateMap::iterator findIter = std::find_if( states.begin(), states.end(),
                                                                         FindCallForAnnouncement( announcementId ) );

        // if the std::find_if returns end, throw a TrainRadioException - with an invalid call id error

        if ( states.end() == findIter )
        {
            TA_THROW( TA_IRS_Bus::TrainRadioException( "No call ID set for announcement", TA_IRS_Bus::CommonTypes::RadioCallInvalid ) );
        }
        
        TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaStatePtr::element_type >( findIter->second );
        TA_ASSERT( NULL != paState.get(), "Invalid state object, cannot down cast" );

        // return the call details from the matching iterator
        TA_IRS_Bus::StateUpdateTypes::RadioCallDetails callDetails;
        callDetails.consoleId = paState->getConsoleId();
        callDetails.callId = paState->getCallId();
        callDetails.sessionId = paState->getSessionId();
        callDetails.resourceType = TA_IRS_Bus::CommonTypes::TrainPaResource;

        FUNCTION_EXIT;
        return callDetails;
    }


    std::string TrainStateHelper::getStaticGroupForLiveAnnouncement( TA_IRS_Bus::PaTypes::AnnouncementId announcementId,
                                                                     const ITrainStatusManager& trainStatusManager )
    {
        FUNCTION_ENTRY( "getStaticGroupForLiveAnnouncement" );

        // get all the train states for TrainPaResource from the status manager
        TA_IRS_Bus::TrainIdToStateMap states = trainStatusManager.getAllTrainStates( TA_IRS_Bus::CommonTypes::TrainPaResource );

        // create the FindStaticGroupForAnnouncement object, with the parameter announcementId
        // Use the object in the std::find_if function
        TA_IRS_Bus::TrainIdToStateMap::iterator findIter = std::find_if( states.begin(), states.end(),
                                                                         FindStaticGroupForAnnouncement( announcementId ) );

        // if the std::find_if returns end, return ""
        // otherwise return the group TSI from the state

        if ( states.end() != findIter )
        {
            TrainPaStatePtr paState = boost::dynamic_pointer_cast< TrainPaStatePtr::element_type >( findIter->second );
            TA_ASSERT( NULL != paState.get(), "Invalid state object, cannot down cast" );
            
            FUNCTION_EXIT;
            return paState->getAnnouncementStaticGroup();
        }

        FUNCTION_EXIT;
        return "";
    }

} // TA_IRS_App

