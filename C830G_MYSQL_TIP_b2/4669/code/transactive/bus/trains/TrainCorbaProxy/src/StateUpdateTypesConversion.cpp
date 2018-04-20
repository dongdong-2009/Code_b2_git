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


#include "bus/trains/TrainCorbaProxy/src/CallTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/RadioGroupTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/StateUpdateTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainStateUpdateCorba::CctvState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainCctvSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CctvState  cctvState;

        cctvState.trainId = cppType.trainId;
        cctvState.cctvSwitchMode = CctvTypesConversion::convertToCorba( cppType.cctvState );
        cctvState.item1 = cppType.items[0];
        cctvState.item2 = cppType.items[1];
        cctvState.item3 = cppType.items[2];
        cctvState.item4 = cppType.items[3];
        cctvState.preConflictTrain = cppType.preConflictTrain;
        cctvState.timestamp = cppType.timestamp;

        return cctvState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::FullLocalState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState fullLocalState;

        fullLocalState.alarmStates = convertToCorba( cppType.alarmStates );
        fullLocalState.callStates = convertToCorba( cppType.callStates );
        fullLocalState.ongoingCalls = convertToCorba( cppType.ongoingCalls );
        fullLocalState.operationMode = TrainInformationConversion::convertToCorba( cppType.operationMode );
        fullLocalState.paStates = convertToCorba( cppType.paStates );
        fullLocalState.pecStates = convertToCorba( cppType.pecStates );
        fullLocalState.selectionLists = convertToCorba( cppType.selectionLists );
        fullLocalState.tisStates = convertToCorba( cppType.tisStates );

        return fullLocalState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::ETrainResource StateUpdateTypesConversion::convertToCorba( const CommonTypes::ETrainResource& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::ETrainResource eTrainResource;

        switch ( cppType )
        {

            case CommonTypes::CoreResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::CoreResource;
                break;

            case CommonTypes::TrainAlarmResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainAlarmResource;
                break;

            case CommonTypes::TrainCctvResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainCctvResource;
                break;

            case CommonTypes::TrainTisResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainTisResource;
                break;

            case CommonTypes::TrainPaResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainPaResource;
                break;

            case CommonTypes::TrainCallResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainCallResource;
                break;

            case CommonTypes::TrainPecResource:
                eTrainResource = TA_Base_Bus::ITrainStateUpdateCorba::TrainPecResource;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTrainResource;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::ETimsOrigin StateUpdateTypesConversion::convertToCorba( const ProtocolCommonTypes::ETimsOrigin& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::ETimsOrigin eTimsOrigin;

        switch ( cppType )
        {

            case ProtocolCommonTypes::MpuBackup:
                eTimsOrigin = TA_Base_Bus::ITrainStateUpdateCorba::MpuBackup;
                break;

            case ProtocolCommonTypes::MpuMain:
                eTimsOrigin = TA_Base_Bus::ITrainStateUpdateCorba::MpuMain;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTimsOrigin;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::CommsState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainCommsSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CommsState commsState;

        commsState.activeMpu = convertToCorba( cppType.activeMpu );
        commsState.trainId = cppType.trainId;
        commsState.primaryMpuTsi = cppType.primaryMpuTsi.c_str();
        commsState.secondaryMpuTsi = cppType.secondaryMpuTsi.c_str();
        commsState.paLibraryVersion = cppType.paLibraryVersion;
        commsState.ttisLibraryVersion = cppType.ttisLibraryVersion;
        commsState.ttisScheduleVersion = cppType.ttisScheduleVersion;
		commsState.nextTtisLibraryVersion = cppType.nextTtisLibraryVersion;
		commsState.nextTtisScheduleVersion = cppType.nextTtisScheduleVersion;
        commsState.oa1Valid = cppType.oa1Valid;
        commsState.atcValid = cppType.atcValid;
        commsState.communicatingLocation = cppType.communicatingLocation;
        commsState.timestamp = cppType.timestamp;

        return commsState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::CctvStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::CctvStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CctvStateList cctvStateList;

        cctvStateList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            cctvStateList[i] = convertToCorba( cppType[i] );
        }

        return cctvStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::FullGlobalState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState fullGlobalState;

        fullGlobalState.cctvStates = convertToCorba( cppType.cctvStates );
        fullGlobalState.commsStates = convertToCorba( cppType.commsStates );
        fullGlobalState.radioGroupList = RadioGroupTypesConversion::convertToCorba( cppType.radioGroupMap );

        return fullGlobalState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::CommsStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::CommsStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CommsStateList commsStateList;

        commsStateList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            commsStateList[i] = convertToCorba( cppType[i] );
        }

        return commsStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::TisState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainTisSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::TisState tisState;

        tisState.currentDownload = TisTypesConversion::convertToCorba( cppType.currentDownload );
        tisState.currentDownloadVersion = cppType.currentDownloadVersion;
        tisState.timestamp = cppType.timestamp;
        tisState.trainId = cppType.trainId;
        tisState.downloadType = TisTypesConversion::convertToCorba( cppType.downloadType );
        tisState.sessionId = cppType.sessionId.c_str();
        tisState.transferTsi = cppType.transferTsi.c_str();

        return tisState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::PecState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainPecSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::PecState pecState;

        pecState.trainId = cppType.trainId;
        pecState.activePecPoints = PecTypesConversion::convertToCorba( cppType.activePecPoints );
        pecState.consoleId = cppType.consoleId;
        pecState.sessionId = cppType.sessionId.c_str();
        pecState.callId = cppType.callId;
        pecState.autoContinueCounter = cppType.autoContinueCounter;
        pecState.continueRequired = cppType.continueRequired;
        pecState.timestamp = cppType.timestamp;

        return pecState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::PaState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainPaSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::PaState paState;

        paState.autoContinueAckCount = cppType.autoContinueAckCount;
        paState.liveAnnouncemenConsoleId = cppType.consoleId;
        paState.continueRequired = cppType.continueRequired;
        paState.liveAnnouncementCallId = cppType.liveAnnouncementCallId;
        paState.liveAnnouncementId = cppType.liveAnnouncementId;
        paState.liveAnnouncementStaticGroup = cppType.liveAnnouncementStaticGroup.c_str();
        paState.sessionId = cppType.sessionId.c_str();
        paState.timestamp = cppType.timestamp;
        paState.trainId = cppType.trainId;

        return paState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::TisStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TisStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::TisStateList tisStateList;

        tisStateList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); i++ )
        {
            tisStateList[i] = convertToCorba( cppType[i] );
        }

        return tisStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainSelectionMap& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence savedTrainSelectionSequence;

        StateUpdateTypes::TrainSelectionMap::const_iterator ite = cppType.begin();

        savedTrainSelectionSequence.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i, ++ite )
        {
            savedTrainSelectionSequence[i].name = ite->first.c_str();
            savedTrainSelectionSequence[i].trains = CommonTypesConversion::convertToCorba( ite->second );
        }

        return savedTrainSelectionSequence;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::PaStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::PaStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::PaStateList paStateList;

        paStateList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i )
        {
            paStateList[i] = convertToCorba( cppType[i] );
        }

        return paStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetails StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::RadioCallDetails& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetails radioCallDetails;

        radioCallDetails.callId = cppType.callId;
        radioCallDetails.consoleId = cppType.consoleId;
        radioCallDetails.resourceType = convertToCorba( cppType.resourceType );
        radioCallDetails.sessionId = cppType.sessionId.c_str();

        return radioCallDetails;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::RadioCallDetailsSet& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList radioCallDetailsList;

        StateUpdateTypes::RadioCallDetailsSet::const_iterator ite = cppType.begin();

        radioCallDetailsList.length( cppType.size() );

        for ( unsigned int i = 0; i < cppType.size(); ++i, ++ite )
        {
            radioCallDetailsList[i] = convertToCorba( *ite );
        }

        return radioCallDetailsList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::CallState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainCallSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CallState callState;

        callState.callId = cppType.callId;
        callState.callType = CallTypesConversion::convertToCorba( cppType.callType );
        callState.consoleId = cppType.consoleId;
        callState.incomingCall = cppType.incomingCall;
        callState.incomingCallTime = cppType.incomingCallTime;
        callState.sessionId = cppType.sessionId.c_str();
        callState.timestamp = cppType.timestamp;
        callState.trainId = cppType.trainId;

        return callState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::AlarmStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::AlarmStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::AlarmStateList alarmStateList;

        alarmStateList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            alarmStateList[i] = convertToCorba( cppType[i] );
        }

        return alarmStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::AlarmState StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::TrainAlarmSyncState& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::AlarmState alarmState;

        alarmState.atcAlarmId = cppType.atcAlarmId.c_str();
        alarmState.cctvPreConflictAlarmId = cppType.atcAlarmId.c_str();
        alarmState.radioModeAlarmId = cppType.radioModeAlarmId.c_str();
        alarmState.timestamp = cppType.timestamp;
        alarmState.trainId = cppType.trainId;

        return alarmState;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::CallStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::CallStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::CallStateList callStateList;

        callStateList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            callStateList[i] = convertToCorba( cppType[i] );
        }

        return callStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::PecStateList StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::PecStateList& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::PecStateList pecStateList;

        pecStateList.length( cppType.size() );

        for ( unsigned long i = 0; i < cppType.size(); ++i )
        {
            pecStateList[i] = convertToCorba( cppType[i] );
        }

        return pecStateList;
    }


    TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI StateUpdateTypesConversion::convertToCorba( const StateUpdateTypes::NonDutyTrainTSI& cppType )
    {
        TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI nonDutyTrainTSI;

        nonDutyTrainTSI.trainId = cppType.trainId;
        nonDutyTrainTSI.trainLocation = cppType.trainLocation;
        nonDutyTrainTSI.trainTsi = cppType.trainTsi.c_str();

        return nonDutyTrainTSI;
    }


    StateUpdateTypes::FullGlobalState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::FullGlobalState& CorbaType )
    {
        StateUpdateTypes::FullGlobalState fullGlobalState;

        fullGlobalState.cctvStates = convertToCpp( CorbaType.cctvStates );
        fullGlobalState.commsStates = convertToCpp( CorbaType.commsStates );
        fullGlobalState.radioGroupMap = RadioGroupTypesConversion::convertToCpp( CorbaType.radioGroupList );

        return fullGlobalState;
    }


    StateUpdateTypes::TisStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TisStateList& CorbaType )
    {
        StateUpdateTypes::TisStateList tisStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            tisStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return tisStateList;
    }


    StateUpdateTypes::TrainPaSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PaState& CorbaType )
    {
        StateUpdateTypes::TrainPaSyncState trainPaSyncState;

        trainPaSyncState.autoContinueAckCount = CorbaType.autoContinueAckCount;
        trainPaSyncState.consoleId = CorbaType.liveAnnouncemenConsoleId;
        trainPaSyncState.continueRequired = CorbaType.continueRequired;
        trainPaSyncState.liveAnnouncementCallId = CorbaType.liveAnnouncementCallId;
        trainPaSyncState.liveAnnouncementId = CorbaType.liveAnnouncementId;
        trainPaSyncState.liveAnnouncementStaticGroup = CorbaType.liveAnnouncementStaticGroup;
        trainPaSyncState.sessionId = CorbaType.sessionId.in();
        trainPaSyncState.timestamp = CorbaType.timestamp;
        trainPaSyncState.trainId = CorbaType.trainId;

        return trainPaSyncState;
    }


    StateUpdateTypes::TrainPecSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PecState& CorbaType )
    {
        StateUpdateTypes::TrainPecSyncState trainPecSyncState;

        trainPecSyncState.trainId = CorbaType.trainId;
        trainPecSyncState.activePecPoints = PecTypesConversion::convertToCpp( CorbaType.activePecPoints );
        trainPecSyncState.consoleId = CorbaType.consoleId;
        trainPecSyncState.sessionId = CorbaType.sessionId.in();
        trainPecSyncState.callId = CorbaType.callId;
        trainPecSyncState.autoContinueCounter = CorbaType.autoContinueCounter;
        trainPecSyncState.continueRequired = CorbaType.continueRequired;
        trainPecSyncState.timestamp = CorbaType.timestamp;

        return trainPecSyncState;
    }


    StateUpdateTypes::TrainTisSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::TisState& CorbaType )
    {
        StateUpdateTypes::TrainTisSyncState trainTisSyncState;

        trainTisSyncState.currentDownload = TisTypesConversion::convertToCpp( CorbaType.currentDownload );
        trainTisSyncState.currentDownloadVersion = CorbaType.currentDownloadVersion;
        trainTisSyncState.timestamp = CorbaType.timestamp;
        trainTisSyncState.trainId = CorbaType.trainId;
        trainTisSyncState.downloadType = TisTypesConversion::convertToCpp( CorbaType.downloadType );
        trainTisSyncState.sessionId = CorbaType.sessionId.in();
        trainTisSyncState.transferTsi = CorbaType.transferTsi.in();

        return trainTisSyncState;
    }


    StateUpdateTypes::CommsStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CommsStateList& CorbaType )
    {
        StateUpdateTypes::CommsStateList commsStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            commsStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return commsStateList;
    }


    StateUpdateTypes::CctvStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CctvStateList& CorbaType )
    {
        StateUpdateTypes::CctvStateList cctvStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            cctvStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return cctvStateList;
    }


    StateUpdateTypes::TrainCommsSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CommsState& CorbaType )
    {
        StateUpdateTypes::TrainCommsSyncState trainCommsSyncState;

        trainCommsSyncState.activeMpu = convertToCpp( CorbaType.activeMpu );
        trainCommsSyncState.trainId = CorbaType.trainId;
        trainCommsSyncState.primaryMpuTsi = CorbaType.primaryMpuTsi.in();
        trainCommsSyncState.secondaryMpuTsi = CorbaType.secondaryMpuTsi.in();
        trainCommsSyncState.paLibraryVersion = CorbaType.paLibraryVersion;
        trainCommsSyncState.ttisLibraryVersion = CorbaType.ttisLibraryVersion;
        trainCommsSyncState.ttisScheduleVersion = CorbaType.ttisScheduleVersion;
		trainCommsSyncState.nextTtisLibraryVersion = CorbaType.nextTtisLibraryVersion;
		trainCommsSyncState.nextTtisScheduleVersion = CorbaType.nextTtisScheduleVersion;
        trainCommsSyncState.oa1Valid = CorbaType.oa1Valid;
        trainCommsSyncState.atcValid = CorbaType.atcValid;
        trainCommsSyncState.communicatingLocation = CorbaType.communicatingLocation;
        trainCommsSyncState.timestamp = CorbaType.timestamp;

        return trainCommsSyncState;
    }


    StateUpdateTypes::PaStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PaStateList& CorbaType )
    {
        StateUpdateTypes::PaStateList paStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            paStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return paStateList;
    }


    StateUpdateTypes::TrainCctvSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CctvState& CorbaType )
    {
        StateUpdateTypes::TrainCctvSyncState trainCctvSyncState;

        trainCctvSyncState.trainId = CorbaType.trainId;
        trainCctvSyncState.cctvState = CctvTypesConversion::convertToCpp( CorbaType.cctvSwitchMode );
        trainCctvSyncState.items[0] = CorbaType.item1;
        trainCctvSyncState.items[1] = CorbaType.item2;
        trainCctvSyncState.items[2] = CorbaType.item3;
        trainCctvSyncState.items[3] = CorbaType.item4;
        trainCctvSyncState.preConflictTrain = CorbaType.preConflictTrain;
        trainCctvSyncState.timestamp = CorbaType.timestamp;

        return trainCctvSyncState;
    }


    StateUpdateTypes::TrainSelectionMap StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::SavedTrainSelectionSequence& CorbaType )
    {
        StateUpdateTypes::TrainSelectionMap trainSelectionMap;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            std::string str = CorbaType[i].name.in();
            trainSelectionMap.insert( StateUpdateTypes::TrainSelectionMap::value_type(
                                          str, CommonTypesConversion::convertToCpp( CorbaType[i].trains ) ) );
        }

        return trainSelectionMap;
    }


    ProtocolCommonTypes::ETimsOrigin StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETimsOrigin& CorbaType )
    {
        ProtocolCommonTypes::ETimsOrigin eTimsOrigin;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainStateUpdateCorba::MpuMain:
                eTimsOrigin = ProtocolCommonTypes::MpuMain;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::MpuBackup:
                eTimsOrigin = ProtocolCommonTypes::MpuBackup;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTimsOrigin;
    }


    StateUpdateTypes::PecStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::PecStateList& CorbaType )
    {
        StateUpdateTypes::PecStateList pecStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            pecStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return pecStateList;
    }


    StateUpdateTypes::CallStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CallStateList& CorbaType )
    {
        StateUpdateTypes::CallStateList callStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            callStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return callStateList;
    }


    StateUpdateTypes::TrainAlarmSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::AlarmState& CorbaType )
    {
        StateUpdateTypes::TrainAlarmSyncState trainAlarmSyncState;

        trainAlarmSyncState.trainId = CorbaType.trainId;
        trainAlarmSyncState.atcAlarmId = CorbaType.atcAlarmId.in();
        trainAlarmSyncState.oa1AlarmId = CorbaType.oa1AlarmId.in();
        trainAlarmSyncState.radioModeAlarmId = CorbaType.radioModeAlarmId.in();
        trainAlarmSyncState.cctvPreConflictAlarmId = CorbaType.cctvPreConflictAlarmId.in();
        trainAlarmSyncState.timestamp = CorbaType.timestamp;

        return trainAlarmSyncState;
    }


    StateUpdateTypes::AlarmStateList StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::AlarmStateList& CorbaType )
    {
        StateUpdateTypes::AlarmStateList alarmStateList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            alarmStateList.push_back( convertToCpp( CorbaType[i] ) );
        }

        return alarmStateList;
    }


    CommonTypes::ETrainResource StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::ETrainResource& CorbaType )
    {
        CommonTypes::ETrainResource eTrainResource;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainStateUpdateCorba::CoreResource:
                eTrainResource = CommonTypes::CoreResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainAlarmResource:
                eTrainResource = CommonTypes::TrainAlarmResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainCctvResource:
                eTrainResource = CommonTypes::TrainCctvResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainTisResource:
                eTrainResource = CommonTypes::TrainTisResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainPaResource:
                eTrainResource = CommonTypes::TrainPaResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainCallResource:
                eTrainResource = CommonTypes::TrainCallResource;
                break;

            case TA_Base_Bus::ITrainStateUpdateCorba::TrainPecResource:
                eTrainResource = CommonTypes::TrainPecResource;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eTrainResource;
    }


    StateUpdateTypes::RadioCallDetailsSet StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetailsList& CorbaType )
    {
        StateUpdateTypes::RadioCallDetailsSet radioCallDetailsSet;

        for ( unsigned int i = 0; i < CorbaType.length(); ++i )
        {
            radioCallDetailsSet.insert( convertToCpp( CorbaType[i] ) );
        }

        return radioCallDetailsSet;
    }


    StateUpdateTypes::RadioCallDetails StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::RadioCallDetails& CorbaType )
    {
        StateUpdateTypes::RadioCallDetails radioCallDetails;

        radioCallDetails.callId = CorbaType.callId;
        radioCallDetails.calledTsi = CorbaType.calledTsi;
        radioCallDetails.consoleId = CorbaType.consoleId;
        radioCallDetails.resourceType = convertToCpp( CorbaType.resourceType );
        radioCallDetails.sessionId = CorbaType.sessionId;

        return radioCallDetails;
    }


    StateUpdateTypes::NonDutyTrainTSI StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::NonDutyTrainTSI& CorbaType )
    {
        StateUpdateTypes::NonDutyTrainTSI nonDutyTrainTSI;

        nonDutyTrainTSI.trainId = CorbaType.trainId;
        nonDutyTrainTSI.trainLocation = CorbaType.trainLocation;
        nonDutyTrainTSI.trainTsi = CorbaType.trainTsi;

        return nonDutyTrainTSI;
    }


    StateUpdateTypes::FullLocalState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::FullLocalState& CorbaType )
    {
        StateUpdateTypes::FullLocalState fullLocalState;

        fullLocalState.alarmStates = convertToCpp( CorbaType.alarmStates );
        fullLocalState.callStates = convertToCpp( CorbaType.callStates );
        fullLocalState.ongoingCalls = convertToCpp( CorbaType.ongoingCalls );
        fullLocalState.operationMode = TrainInformationConversion::convertToCpp( CorbaType.operationMode );
        fullLocalState.paStates = convertToCpp( CorbaType.paStates );
        fullLocalState.pecStates = convertToCpp( CorbaType.pecStates );
        fullLocalState.selectionLists = convertToCpp( CorbaType.selectionLists );
        fullLocalState.tisStates = convertToCpp( CorbaType.tisStates );

        return fullLocalState;
    }


    StateUpdateTypes::TrainCallSyncState StateUpdateTypesConversion::convertToCpp( const TA_Base_Bus::ITrainStateUpdateCorba::CallState& CorbaType )
    {
        StateUpdateTypes::TrainCallSyncState trainCallSyncState;

        trainCallSyncState.callId = CorbaType.callId;
        trainCallSyncState.callType = CallTypesConversion::convertToCpp( CorbaType.callType );
        trainCallSyncState.consoleId = CorbaType.consoleId;
        trainCallSyncState.incomingCall = CorbaType.incomingCall;
        trainCallSyncState.incomingCallTime = CorbaType.incomingCallTime;
        trainCallSyncState.sessionId = CorbaType.sessionId.in();
        trainCallSyncState.timestamp = CorbaType.timestamp;
        trainCallSyncState.trainId = CorbaType.trainId;

        return trainCallSyncState;
    }
}
