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

#include "bus/trains/TrainCommonLibrary/src/StateUpdateTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include <sstream>

namespace TA_IRS_Bus
{
    namespace StateUpdateTypes
    {

        std::string NonDutyTrainTSI::getLogString() const
        {
            std::ostringstream result;

            result << "NonDutyTrainTSI [trainId: " << static_cast< unsigned int>( trainId )
                   << "][trainTsi: " << trainTsi
                   << "][trainLocation: " << trainLocation
                   << "]";

            return result.str();
        }


        std::string TrainCctvSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainCctvSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][preConflictTrain: " << static_cast< unsigned int>( preConflictTrain )
                   << "][timestamp: " << timestamp
                   << "][cctvState: " << CctvTypes::cctvSwitchModeToString( cctvState )
                   << "]";

            switch ( cctvState )
            {

                case CctvTypes::CctvSequence:
                    result << "["
                           << ProtocolCctvTypes::cctvSequenceNumberToString( static_cast< ProtocolCctvTypes::ECctvSequenceNumber >( items[0] ) )
                           << "]";
                    break;

                case CctvTypes::CctvCamera:
                    result << "["
                    << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( items[0] ) )
                    << "]";
                    break;

                case CctvTypes::CctvQuad:
                    result << "["
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( items[0] ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( items[1] ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( items[2] ) ) << ", "
                           << ProtocolCctvTypes::cctvCameraToString( static_cast< ProtocolCctvTypes::ECctvCamera >( items[3] ) ) << "]";
                    break;

                case CctvTypes::CctvInactive:
                    // fall through

                default:
                    break;
            }

            return result.str();
        }


        std::string cctvStateListToString( const CctvStateList& cctvStateList )
        {
            return structListToString< CctvStateList >( cctvStateList );
        }


        std::string TrainCommsSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainCommsSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][primaryMpuTsi: " << primaryMpuTsi
                   << "][secondaryMpuTsi: " << secondaryMpuTsi
                   << "][activeMpu: " << ProtocolCommonTypes::timsOriginToString( activeMpu )
                   << "][paLibraryVersion: " << paLibraryVersion
                   << "][ttisLibraryVersion: " << ttisLibraryVersion
                   << "][ttisScheduleVersion: " << ttisScheduleVersion
                   << "][oa1Valid: " << oa1Valid
                   << "][atcValid: " << atcValid
                   << "][communicatingLocation: " << communicatingLocation
                   << "][timestamp: " << timestamp
                   << "]";

            return result.str();
        }


        std::string commsStateListToString( const CommsStateList& commsStateList )
        {
            return structListToString< CommsStateList >( commsStateList );
        }


        std::string FullGlobalState::getLogString() const
        {
            std::ostringstream result;

            result << "FullGlobalState " << std::endl;
            result << commsStateListToString( commsStates ) << std::endl;
            result << cctvStateListToString( cctvStates ) << std::endl;
            result << RadioGroupTypes::trainRadioGroupMapToString( radioGroupMap ) << std::endl;
                   
            return result.str();
        };
        
        
        std::string TrainAlarmSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainAlarmSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][atcAlarmId: " << atcAlarmId
                   << "][oa1AlarmId: " << oa1AlarmId
                   << "][radioModeAlarmId: " << radioModeAlarmId
                   << "][cctvPreConflictAlarmId: " << cctvPreConflictAlarmId
                   << "][timestamp: " << timestamp
                   << "]";

            return result.str();
        }
        
        
        std::string alarmStateListToString( const AlarmStateList& alarmStateList )
        {
            return structListToString< AlarmStateList >( alarmStateList );
        }


        std::string TrainCallSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainCallSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][incomingCall: " << incomingCall
                   << "][incomingCallTime: " << incomingCallTime
                   << "][sessionId: " << sessionId
                   << "][consoleId: " << consoleId
                   << "][callId: " << callId
                   << "][timestamp: " << timestamp
                   << "][callType: " << CallTypes::trainCallTypeTypeToString( callType )
                   << "]";

            return result.str();
        }


        std::string callStateListToString( const CallStateList& callStateList )
        {
            return structListToString< CallStateList >( callStateList );
        }


        std::string TrainPaSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainPaSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][consoleId: " << consoleId
                   << "][sessionId: " << sessionId
                   << "][liveAnnouncementCallId: " << liveAnnouncementCallId
                   << "][liveAnnouncementId: " << static_cast< unsigned int>( liveAnnouncementId )
                   << "][liveAnnouncementStaticGroup: " << liveAnnouncementStaticGroup
                   << "][autoContinueAckCount: " << static_cast< unsigned int>( autoContinueAckCount )
                   << "][continueRequired: " << continueRequired
                   << "][timestamp: " << timestamp
                   << "]";

            return result.str();
        }


        std::string paStateListToString( const PaStateList& paStateList )
        {
            return structListToString< PaStateList >( paStateList );
        }


        std::string TrainPecSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainPecSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][consoleId: " << consoleId
                   << "][sessionId: " << sessionId
                   << "][callId: " << callId
                   << "][autoContinueCounter: " << static_cast< unsigned int>( autoContinueCounter )
                   << "][continueRequired: " << continueRequired
                   << "][timestamp: " << timestamp
                   << "][activePecPoints: " << PecTypes::activePecPointListToString( activePecPoints )
                   << "]";

            return result.str();
        }


        std::string pecStateListToString( const PecStateList& pecStateList )
        {
            return structListToString< PecStateList >( pecStateList );
        }

        
        std::string TrainTisSyncState::getLogString() const
        {
            std::ostringstream result;

            result << "TrainTisSyncState [trainId: " << static_cast< unsigned int>( trainId )
                   << "][downloadType: " << TtisTypes::ttisDownloadTypeToString( downloadType )
                   << "][currentDownloadVersion: " << currentDownloadVersion
                   << "][currentDownload: " << currentDownload.size() << " frames"
                   << "][sessionId: " << sessionId
                   << "][transferTsi: " << transferTsi
                   << "][timestamp: " << timestamp
                   << "]";

            return result.str();
        }
        
        
        std::string tisStateListToString( const TisStateList& tisStateList )
        {
            return structListToString< TisStateList >( tisStateList );
        }


        bool RadioCallDetails::operator<( const RadioCallDetails& rhs ) const
        {
            if ( consoleId < rhs.consoleId )
            {
                return true;
            }
            else if ( consoleId == rhs.consoleId )
            {
                return callId < rhs.callId;
            }

            return false;
        }


        bool RadioCallDetails::operator==( const RadioCallDetails& rhs ) const
        {
            return (callId == rhs.callId) && (consoleId == rhs.consoleId);
        }


        std::string RadioCallDetails::getLogString() const
        {
            std::ostringstream logString;

            logString << "[consoleId: " << consoleId
                      << "][callId: " << callId
                      << "][sessionId: " << sessionId
                      << "][resourceType: " << CommonTypes::trainResourceToString( resourceType )
                      << "]";
            
            return logString.str();
        }


        std::string radioCallDetailsSetToString( const RadioCallDetailsSet& radioCallDetailsSet )
        {
            return structListToString< RadioCallDetailsSet >( radioCallDetailsSet );
        }


        std::string trainSelectionMapToString( const TrainSelectionMap& trainSelectionMap )
        {
            std::ostringstream result;

            result << "TrainSelectionMap: [size: " << trainSelectionMap.size() << "]";

            for ( TrainSelectionMap::const_iterator iter = trainSelectionMap.begin();
                  trainSelectionMap.end() != iter; ++iter )
            {
                result << std::endl;
                result << "[Selection Name: " << iter->first
                       << "][Train List: " << CommonTypes::trainIdListToString( iter->second )
                       << "]";
            }

            return result.str();
        }


        std::string FullLocalState::getLogString() const
        {
            std::ostringstream result;

            result << "FullLocalState " << std::endl;
            result << operationMode.getLogString() << std::endl;;
            result << radioCallDetailsSetToString( ongoingCalls ) << std::endl;
            result << trainSelectionMapToString( selectionLists ) << std::endl;
            result << alarmStateListToString( alarmStates );
            result << callStateListToString( callStates );
            result << paStateListToString( paStates );
            result << pecStateListToString( pecStates );
            result << tisStateListToString( tisStates );
                   
            return result.str();
        };
    
    }

}
