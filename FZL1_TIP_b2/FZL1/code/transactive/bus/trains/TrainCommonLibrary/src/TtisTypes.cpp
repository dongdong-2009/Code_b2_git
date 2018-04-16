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

#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainCommonLibrary/src/Utility.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>

namespace TA_IRS_Bus
{

    namespace TtisTypes
    {

        std::string libraryDataTypeToString( const LibraryDataType& libraryDataType )
        {
            return integerListToString< LibraryDataType >( libraryDataType );
        }


        std::string TisLibraryVersionUpdate::getLogString() const
        {
            std::ostringstream result;

            result << "TisLibraryVersionUpdate [trainId: " << static_cast< unsigned int>( trainId )
                   << "][libraryVersion: " << libraryVersion
                   << "][scheduleVersion: " << scheduleVersion
                   << "]";

            return result.str();
        }


        std::string tisLibraryVersionListToString( const TisLibraryVersionList& tisLibraryVersionList )
        {
            return structListToString< TisLibraryVersionList >( tisLibraryVersionList );
        }


        std::string tisUpdateTypeToString( ETisUpdateType tisUpdateType )
        {
            switch ( tisUpdateType )
            {

                case LibraryDownloadStart:
                    return "LibraryDownloadStart";
                    break;

                case LibraryDownloadComplete:
                    return "LibraryDownloadComplete";
                    break;

                case LibraryUpgradeResult:
                    return "LibraryUpgradeResult";
                    break;

                case ScheduleDownloadStart:
                    return "ScheduleDownloadStart";
                    break;

                case ScheduleDownloadComplete:
                    return "ScheduleDownloadComplete";
                    break;

                case ScheduleUpgradeResult:
                    return "ScheduleUpgradeResult";
                    break;

                default:
                    break;
            }

            return "Unknown ETisUpdateType";
        }


        std::string ttisCommandResultToString( ETtisCommandResult ttisCommandResult )
        {
            switch ( ttisCommandResult )
            {

                case TtisSuccess:
                    return "Success";
                    break;

                case TtisInUse:
                    return "In Use";
                    break;

                case TtisReset:
                    return "Reset";
                    break;

                case TtisFailed:
                    return "Failed";
                    break;

                case TtisDifferentVersion:
                    return "Different Version";
                    break;

                case BadCommandBadChecksum:
                    return "Bad Checksum";
                    break;

                case BadCommandUnknownCommand:
                    return "Unknown Command";
                    break;

                case BadCommandBadParameter:
                    return "Bad Parameter";
                    break;

                case BadCommandUnexpectedCommand:
                    return "Unexpected Command";
                    break;

                case InvalidTrainError:
                    return "Invalid Train";
                    break;

                case TransmissionFailureError:
                    return "Transmission Failure";
                    break;

                case TransactionTimeoutError:
                    return "Transaction Timeout";
                    break;

                case TransactionCancelledError:
                    return "Transaction Cancelled";
                    break;

                case MpuChangeoverError:
                    return "Mpu Changeover";
                    break;

                default:
                    break;
            }

            return "Unknown ETtisCommandResult";
        }


        std::string downloadResultToString( EDownloadResult downloadResult )
        {
            switch ( downloadResult )
            {

                case Success:
                    return "Success";
                    break;

                case TransferFailed:
                    return "Transfer Failed";
                    break;

                case TimsFailed:
                    return "Tims Failed";
                    break;

                case TimedOut:
                    return "Timeout";
                    break;

                case MpuChangeover:
                    return "Mpu Changeover";
                    break;

                default:
                    break;
            }

            return "Unknown EDownloadResult";
        }


        std::string TisLibraryUpdate::getLogString() const
        {
            std::ostringstream result;

            result << "TisLibraryVersionUpdate [trainId: " << static_cast< unsigned int>( trainId )
                   << "][updateType: " << tisUpdateTypeToString( updateType )
                   << "][commandStatus: " << ttisCommandResultToString( commandStatus )
                   << "][downloadResult: " << downloadResultToString( downloadResult )
                   << "][version: " << version
                   << "]";

            return result.str();
        }


        std::string ttisOriginalDisplayCommandToString( ETtisOriginalDisplayCommand ttisOriginalDisplayCommand )
        {
            switch ( ttisOriginalDisplayCommand )
            {

                case TtisFreeTextCommand:
                    return "Free Text";
                    break;

                case TtisPredefinedCommand:
                    return "Predefined";
                    break;

                case TtisResetCommand:
                    return "Reset";
                    break;

                default:
                    break;
            }

            return "Unknown ETtisOriginalDisplayCommand";
        }


        std::string ttisDownloadTypeToString( ETtisDownloadType ttisDownloadType )
        {
            switch ( ttisDownloadType )
            {

                case TtisNone:
                    return "None";
                    break;

                case TtisLibrary:
                    return "Library";
                    break;

                case TtisSchedule:
                    return "Schedule";
                    break;

                default:
                    break;
            }

            return "Unknown ETtisDownloadType";
        }


        std::string TtisFrameData::getLogString() const
        {
            std::ostringstream result;

            result << "TtisFrameData [frameData: " << libraryDataTypeToString( frameData )
                   << "]";

            return result.str();
        }


        std::string ttisPidListToString( const TtisPidList& ttisPidList )
        {
            std::ostringstream result;

            for ( TtisPidList::const_iterator iter = ttisPidList.begin();
                  ttisPidList.end() != iter; ++iter )
            {
                if ( ttisPidList.begin() != iter )
                {
                    result << ", ";
                }

                result << ProtocolTtisTypes::ttisPidToString( *iter );
            }

            return result.str();
        }


        std::string TimeInterval::getLogString() const
        {
            std::ostringstream result;

            result << "start: " << static_cast< unsigned int>( startHour )
                   << ":" << static_cast< unsigned int>( startMin )
                   << "stop: " << static_cast< unsigned int>( stopHour )
                   << ":" << static_cast< unsigned int>( stopMin );

            return result.str();
        }


        std::string ttisFrameListToString( const TtisFrameList& ttisFrameList )
        {
            return structListToString< TtisFrameList >( ttisFrameList );
        }


        std::string TtisFreeTextDisplayParameters::getLogString() const
        {
            std::ostringstream result;

            result << "TtisFreeTextDisplayParameters [pidList: " << ttisPidListToString( pidList )
                   << "][fontSize: " << ProtocolTtisTypes::ttisFontSizeToString( fontSize )
                   << "][justification: " << ProtocolTtisTypes::ttisJustificationToString( justification )
                   << "][intensity: " << ProtocolTtisTypes::ttisIntensityToString( intensity )
                   << "][displayMode: " << ProtocolTtisTypes::ttisDisplayModeToString( displayMode )
                   << "][priority: " << ProtocolTtisTypes::ttisPriorityToString( priority )
                   << "][displayPeriod: " << displayPeriod.getLogString()
                   << "][repeatInterval: " << static_cast< unsigned int>( repeatInterval )
                   << "][message: " << message
                   << "]";

            return result.str();
        }


        std::string TtisPredefinedDisplayParameters::getLogString() const
        {
            std::ostringstream result;

            result << "TtisPredefinedDisplayParameters [pidList: " << ttisPidListToString( pidList )
                   << "][libraryVersion: " << libraryVersion
                   << "][identifierType: " << static_cast< unsigned int>( identifierType )
                   << "][identifierOffset: " << static_cast< unsigned int>( identifierOffset )
                   << "][priority: " << ProtocolTtisTypes::ttisPriorityToString( priority )
                   << "][displayPeriod: " << displayPeriod.getLogString()
                   << "]";

            return result.str();
        }


        /**
         * @author adamr
         * @version 1.0
         * @created 13-Nov-2007 3:50:38 PM
         */
        std::string TtisResetParameters::getLogString() const
        {
            std::ostringstream result;

            result << "TtisResetParameters [pidList: " << ttisPidListToString( pidList )
                   << "][messageType: " << ProtocolTtisTypes::ttisResetMessageTypeToString( messageType )
                   << "]";

            return result.str();
        }




        DisplayParameters::DisplayParameters()
        {
            FUNCTION_ENTRY( "DisplayParameters" );
            FUNCTION_EXIT;
        }


        DisplayParameters::~DisplayParameters()
        {
            FUNCTION_ENTRY( "~DisplayParameters" );
            FUNCTION_EXIT;
        }


        DisplayParameters::DisplayParameters( const TtisFreeTextDisplayParameters& freeTextParameters )
        {
            FUNCTION_ENTRY( "DisplayParameters" );

            // Sets m_originalCommand to TtisFreeText
            m_originalCommand = TtisFreeTextCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = freeTextParameters;

            FUNCTION_EXIT;
        }


        DisplayParameters::DisplayParameters( const TtisPredefinedDisplayParameters& predefinedParameters )
        {
            FUNCTION_ENTRY( "DisplayParameters" );

            // Sets m_originalCommand to TtisPredefined
            m_originalCommand = TtisPredefinedCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = predefinedParameters;

            FUNCTION_EXIT;
        }


        DisplayParameters::DisplayParameters( const TtisResetParameters& resetParameters )
        {
            FUNCTION_ENTRY( "DisplayParameters" );

            // Sets m_originalCommand to TtisReset
            m_originalCommand = TtisResetCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = resetParameters;

            FUNCTION_EXIT;
        }


        ETtisOriginalDisplayCommand DisplayParameters::getOriginalCommand() const
        {
            FUNCTION_ENTRY( "getOriginalCommand" );
            FUNCTION_EXIT;
            return m_originalCommand;
        }


        void DisplayParameters::operator=( const TtisFreeTextDisplayParameters& freeTextParameters )
        {
            FUNCTION_ENTRY( "operator= ( TtisFreeTextDisplayParameters )" );

            // Sets m_originalCommand to TtisFreeText
            m_originalCommand = TtisFreeTextCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = freeTextParameters;

            FUNCTION_EXIT;
        }


        void DisplayParameters::operator=( const TtisPredefinedDisplayParameters& predefinedParameters )
        {
            FUNCTION_ENTRY( "operator= ( TtisPredefinedDisplayParameters )" );

            // Sets m_originalCommand to TtisPredefined
            m_originalCommand = TtisPredefinedCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = predefinedParameters;

            FUNCTION_EXIT;
        }


        void DisplayParameters::operator=( const TtisResetParameters& resetParameters )
        {
            FUNCTION_ENTRY( "operator= ( TtisResetParameters )" );

            // Sets m_originalCommand to TtisReset
            m_originalCommand = TtisResetCommand;

            // Puts the parameters into m_displayParameters
            m_displayParameters = resetParameters;

            FUNCTION_EXIT;
        }


        TtisFreeTextDisplayParameters DisplayParameters::getFreeTextParameters() const
        {
            FUNCTION_ENTRY( "getFreeTextParameters" );

            // if m_originalCommand is not TtisFreeText TA_ASSERT
            TA_ASSERT( m_originalCommand == TtisFreeTextCommand,
                       "m_originalCommand is not TtisFreeTextCommand" );

            // extract the free text parameters from m_displayParameters
            // if extraction fails, TA_ASSERT

            try
            {
                const TtisFreeTextDisplayParameters& ttisFreeTextDisplayParameters =
                    boost::get<TtisFreeTextDisplayParameters>( m_displayParameters );

                FUNCTION_EXIT;
                return ttisFreeTextDisplayParameters;
            }
            catch ( const boost::bad_get& )
            {
                TA_ASSERT( false, "Extracting the free text parameters from m_displayParameters failed" );
            }

            FUNCTION_EXIT;

            return TtisFreeTextDisplayParameters();
        }


        TtisPredefinedDisplayParameters DisplayParameters::getPredefinedParameters() const
        {
            FUNCTION_ENTRY( "getPredefinedParameters" );

            // if m_originalCommand is not TtisPredefined TA_ASSERT
            TA_ASSERT( m_originalCommand == TtisPredefinedCommand,
                       "m_originalCommand is not TtisPredefinedCommand" );

            // extract the predefined parameters from m_displayParameters
            // if extraction fails, TA_ASSERT

            try
            {
                const TtisPredefinedDisplayParameters& ttisPredefinedDisplayParameters =
                    boost::get<TtisPredefinedDisplayParameters>( m_displayParameters );

                FUNCTION_EXIT;
                return ttisPredefinedDisplayParameters;
            }
            catch ( const boost::bad_get& )
            {
                TA_ASSERT( false, "Extracting the predefined parameters from m_displayParameters failed" );
            }

            FUNCTION_EXIT;

            return TtisPredefinedDisplayParameters();
        }


        TtisResetParameters DisplayParameters::getResetParameters() const
        {
            FUNCTION_ENTRY( "getResetParameters" );

            // if m_originalCommand is not TtisReset TA_ASSERT
            TA_ASSERT( m_originalCommand == TtisResetCommand,
                       "m_originalCommand is not TtisResetCommand" );

            // extract the reset parameters from m_displayParameters
            // if extraction fails, TA_ASSERT

            try
            {
                const TtisResetParameters& ttisResetParameters =
                    boost::get<TtisResetParameters>( m_displayParameters );

                FUNCTION_EXIT;
                return ttisResetParameters;
            }
            catch ( const boost::bad_get& )
            {
                TA_ASSERT( false, "Extracting the predefined parameters from m_displayParameters failed" );
            }

            // return the parameters
            FUNCTION_EXIT;

            return TtisResetParameters();
        }


        std::string DisplayParameters::getLogString() const
        {
            switch ( m_originalCommand )
            {

                case TtisFreeTextCommand:
                    return getFreeTextParameters().getLogString();
                    break;

                case TtisPredefinedCommand:
                    return getPredefinedParameters().getLogString();
                    break;

                case TtisResetCommand:
                    return getResetParameters().getLogString();
                    break;

                default:
                    break;
            }

            return "";
        }


        std::string TisReturnStatus::getLogString() const
        {
            std::ostringstream logString;

            logString << "TisReturnStatus [trainId: " << static_cast< unsigned int>( trainId )
                      << "][sessionId: " << sessionId
                      << "][timestamp: " << timestamp
                      << "][result: " << ttisCommandResultToString( result )
                      << "][originalParameters: " << originalParameters.getLogString()
                      << "]";

            return logString.str();
        }


    }
}
