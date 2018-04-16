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


#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_Bus
{
    TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate TrainInformationConversion::convertToCorba( const TrainInformationTypes::TrainSelectionUpdate& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate trainSelectionUpdate;

        trainSelectionUpdate.change = convertToCorba( cppType.change );
        trainSelectionUpdate.selectionName = cppType.selectionName.c_str();

        return trainSelectionUpdate;
    }



    TA_Base_Bus::ITrainInformationCorba::EChangeType TrainInformationConversion::convertToCorba( const TrainInformationTypes::EChangeType& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::EChangeType eChangeType;

        switch ( cppType )
        {

            case TA_IRS_Bus::TrainInformationTypes::SelectionListUpdate:
                eChangeType = TA_Base_Bus::ITrainInformationCorba::SelectionListUpdate;
                break;

            case TA_IRS_Bus::TrainInformationTypes::SelectionListDeletion:
                eChangeType = TA_Base_Bus::ITrainInformationCorba::SelectionListDeletion;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        };

        return eChangeType;
    }


    TA_Base_Bus::ITrainInformationCorba::TrainSelectionList TrainInformationConversion::convertToCorba( const TrainInformationTypes::TrainSelectionList& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::TrainSelectionList trainSelectionList;

        trainSelectionList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            trainSelectionList[i] = CORBA::string_dup( cppType[i].c_str() );
        }

        return trainSelectionList;
    }


    TA_Base_Bus::ITrainInformationCorba::AgentOperationMode TrainInformationConversion::convertToCorba( const TrainInformationTypes::AgentOperationMode& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::AgentOperationMode agentOperationMode;

        std::set< unsigned long >::const_iterator ite = cppType.controlledLocations.begin();
        agentOperationMode.controlledLocations.length( cppType.controlledLocations.size() );

        for ( unsigned long i = 0; ite != cppType.controlledLocations.end(); ++ite, ++i )
        {
            agentOperationMode.controlledLocations[i] = *ite;
        }

        agentOperationMode.doubleAtsFailure = cppType.doubleAtsFailure;

        agentOperationMode.fallbackMode = cppType.fallbackMode;
        agentOperationMode.groupOffline = cppType.groupOffline;
        agentOperationMode.lastUpdateTime = cppType.lastUpdateTime;
        agentOperationMode.localDuty = cppType.localDuty;

        return agentOperationMode;
    }


    TA_Base_Bus::ITrainInformationCorba::EAtsTrack TrainInformationConversion::convertToCorba( const TrainInformationTypes::EAtsTrack& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::EAtsTrack eAtsTrack;

        switch ( cppType )
        {

            case TA_IRS_Bus::TrainInformationTypes::AtsTrackUnknown:
                eAtsTrack = TA_Base_Bus::ITrainInformationCorba::TrackUnknown;
                break;

            case TA_IRS_Bus::TrainInformationTypes::AtsTrackInner:
                eAtsTrack = TA_Base_Bus::ITrainInformationCorba::TrackInbound;
                break;

            case TA_IRS_Bus::TrainInformationTypes::AtsTrackOuter:
                eAtsTrack = TA_Base_Bus::ITrainInformationCorba::TrackOutbound;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        };

        return eAtsTrack;
    }


    TA_Base_Bus::ITrainInformationCorba::TrainDetails TrainInformationConversion::convertToCorba( const TrainInformationTypes::TrainDetails& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::TrainDetails trainDetails;

        trainDetails.trainId = cppType.trainId;
        trainDetails.communicationValid = cppType.communicationValid;
        trainDetails.communicatingLocation = cppType.communicatingLocation;
        trainDetails.primaryTsi = cppType.primaryTsi.c_str();
        trainDetails.secondaryTsi = cppType.secondaryTsi.c_str();
        trainDetails.atsValid = cppType.atsValid;
        trainDetails.isLocalisationValid = cppType.isLocalisationValid;
        trainDetails.currentLocation = cppType.currentLocation;
        trainDetails.currentCctvZone = cppType.currentCctvZone;
        trainDetails.serviceNumber = cppType.serviceNumber.c_str();
        trainDetails.isMute = cppType.isMute;
        trainDetails.track = convertToCorba( cppType.track );
        trainDetails.atsValid = cppType.atsValid;

        return trainDetails;
    }


    TA_Base_Bus::ITrainInformationCorba::TrainDetailsList TrainInformationConversion::convertToCorba( const TrainInformationTypes::TrainDetailsList& cppType )
    {
        TA_Base_Bus::ITrainInformationCorba::TrainDetailsList trainDetailsList;

        trainDetailsList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            trainDetailsList[i] = convertToCorba( cppType[i] );
        }

        return trainDetailsList;
    }


    TrainInformationTypes::TrainSelectionUpdate TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainSelectionUpdate& CorbaType )
    {
        TrainInformationTypes::TrainSelectionUpdate trainSelectionUpdate;

        trainSelectionUpdate.change = convertToCpp( CorbaType.change );
        trainSelectionUpdate.selectionName = CorbaType.selectionName;

        return trainSelectionUpdate;
    }


    TrainInformationTypes::EChangeType TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::EChangeType& CorbaType )
    {
        TrainInformationTypes::EChangeType eChangeType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainInformationCorba::SelectionListUpdate:
                eChangeType = TA_IRS_Bus::TrainInformationTypes::SelectionListUpdate;
                break;

            case TA_Base_Bus::ITrainInformationCorba::SelectionListDeletion:
                eChangeType = TA_IRS_Bus::TrainInformationTypes::SelectionListDeletion;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        };

        return eChangeType;
    }


    TrainInformationTypes::TrainSelectionList TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainSelectionList& CorbaType )
    {
        TrainInformationTypes::TrainSelectionList trainSelectionList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            trainSelectionList.push_back( std::string( CorbaType[i] ) );
        }

        return trainSelectionList;
    }


    TrainInformationTypes::AgentOperationMode TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::AgentOperationMode& CorbaType )
    {
        TrainInformationTypes::AgentOperationMode agentOperationMode;


        for ( unsigned long i = 0; i < CorbaType.controlledLocations.length(); ++i )
        {
            agentOperationMode.controlledLocations.insert( CorbaType.controlledLocations[i] );
        }

        agentOperationMode.doubleAtsFailure = CorbaType.doubleAtsFailure;
        agentOperationMode.fallbackMode = CorbaType.fallbackMode;
        agentOperationMode.groupOffline = CorbaType.groupOffline;
        agentOperationMode.localDuty = CorbaType.localDuty;

        agentOperationMode.lastUpdateTime = CorbaType.lastUpdateTime;

        return agentOperationMode;
    }


    TrainInformationTypes::EAtsTrack TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::EAtsTrack& CorbaType )
    {
        TrainInformationTypes::EAtsTrack eAtsTrack;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainInformationCorba::TrackUnknown:
                eAtsTrack = TA_IRS_Bus::TrainInformationTypes::AtsTrackUnknown;
                break;

            case TA_Base_Bus::ITrainInformationCorba::TrackInbound:
                eAtsTrack = TA_IRS_Bus::TrainInformationTypes::AtsTrackInner;
                break;

            case TA_Base_Bus::ITrainInformationCorba::TrackOutbound:
                eAtsTrack = TA_IRS_Bus::TrainInformationTypes::AtsTrackOuter;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        };

        return eAtsTrack;
    }


    TrainInformationTypes::TrainDetails TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainDetails& CorbaType )
    {
        TrainInformationTypes::TrainDetails trainDetails;

        trainDetails.trainId = CorbaType.trainId;
        trainDetails.communicationValid = CorbaType.communicationValid;
        trainDetails.communicatingLocation = CorbaType.communicatingLocation;
        trainDetails.primaryTsi = CorbaType.primaryTsi.in();
        trainDetails.secondaryTsi = CorbaType.secondaryTsi.in();
        trainDetails.atsValid = CorbaType.atsValid;
        trainDetails.isLocalisationValid = CorbaType.isLocalisationValid;
        trainDetails.currentLocation = CorbaType.currentLocation;
        trainDetails.currentCctvZone = CorbaType.currentCctvZone;
        trainDetails.serviceNumber = CorbaType.serviceNumber.in();
        trainDetails.isMute = CorbaType.isMute;
        trainDetails.track = convertToCpp( CorbaType.track );
        trainDetails.atsValid = CorbaType.atsValid;

        return trainDetails;
    }


    TrainInformationTypes::TrainDetailsList TrainInformationConversion::convertToCpp( const TA_Base_Bus::ITrainInformationCorba::TrainDetailsList& CorbaType )
    {
        TrainInformationTypes::TrainDetailsList trainDetailsList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            trainDetailsList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return trainDetailsList;
    }
}
