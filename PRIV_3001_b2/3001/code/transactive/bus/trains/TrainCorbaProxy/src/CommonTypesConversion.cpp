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

#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Bus
{

    TA_Base_Bus::ITrainCommonCorba::TrainId CommonTypesConversion::convertToCorba( const CommonTypes::TrainIdType& cppType )
    {
        return cppType;
    }


    TA_Base_Bus::ITrainCommonCorba::TrainList CommonTypesConversion::convertToCorba( const CommonTypes::TrainIdList& cppType )
    {
        TA_Base_Bus::ITrainCommonCorba::TrainList trainList;

        trainList.length( cppType.size() );

        unsigned long i = 0;

        for ( CommonTypes::TrainIdList::const_iterator ite = cppType.begin(); ite != cppType.end(); ++ite, ++i )
        {
            trainList[ i ] = *ite;
        }

        return trainList;
    }


    TA_Base_Bus::ITrainExceptionsCorba::ERadioErrorType CommonTypesConversion::convertToCorba( const CommonTypes::ERadioErrorType& cppType )
    {
        TA_Base_Bus::ITrainExceptionsCorba::ERadioErrorType eRadioErrorType;

        switch ( cppType )
        {

            case CommonTypes::RadioCallInvalid:
                eRadioErrorType = TA_Base_Bus::ITrainExceptionsCorba::RadioCallInvalid;
                break;

            case CommonTypes::CallSetupFailed:
                eRadioErrorType = TA_Base_Bus::ITrainExceptionsCorba::CallSetupFailed;
                break;

            case CommonTypes::CallSelectFailed:
                eRadioErrorType = TA_Base_Bus::ITrainExceptionsCorba::CallSelectFailed;
                break;

            case CommonTypes::CallEndFailed:
                eRadioErrorType = TA_Base_Bus::ITrainExceptionsCorba::CallEndFailed;
                break;

            case CommonTypes::DgnaFailed:
                eRadioErrorType = TA_Base_Bus::ITrainExceptionsCorba::DgnaFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eRadioErrorType;
    }


    TA_Base_Bus::ITrainExceptionsCorba::EBadCommandError CommonTypesConversion::convertToCorba( const ProtocolCommonTypes::EBadCommandError& cppType )
    {
        TA_Base_Bus::ITrainExceptionsCorba::EBadCommandError eBadCommandError;

        switch ( cppType )
        {

            case ProtocolCommonTypes::BadChecksum:
                eBadCommandError = TA_Base_Bus::ITrainExceptionsCorba::BadChecksum;
                break;

            case ProtocolCommonTypes::UnknownCommand:
                eBadCommandError = TA_Base_Bus::ITrainExceptionsCorba::UnknownCommand;
                break;

            case ProtocolCommonTypes::BadParameter:
                eBadCommandError = TA_Base_Bus::ITrainExceptionsCorba::BadParameter;
                break;

            case ProtocolCommonTypes::UnexpectedCommand:
                eBadCommandError = TA_Base_Bus::ITrainExceptionsCorba::UnexpectedCommand;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eBadCommandError;
    }


    CommonTypes::TrainIdType CommonTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCommonCorba::TrainId& CorbaType )
    {
        return CorbaType;
    }


    CommonTypes::TrainIdList CommonTypesConversion::convertToCpp( const TA_Base_Bus::ITrainCommonCorba::TrainList& CorbaType )
    {
        CommonTypes::TrainIdList trainIdList;

        for ( unsigned long i = 0; i < CorbaType.length(); ++i )
        {
            trainIdList.insert( CorbaType[ i ] );
        }

        return trainIdList;
    }


    CommonTypes::ERadioErrorType CommonTypesConversion::convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::ERadioErrorType& CorbaType )
    {
        CommonTypes::ERadioErrorType eRadioErrorType;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainExceptionsCorba::RadioCallInvalid:
                eRadioErrorType = CommonTypes::RadioCallInvalid;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::CallSetupFailed:
                eRadioErrorType = CommonTypes::CallSetupFailed;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::CallSelectFailed:
                eRadioErrorType = CommonTypes::CallSelectFailed;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::CallEndFailed:
                eRadioErrorType = CommonTypes::CallEndFailed;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::DgnaFailed:
                eRadioErrorType = CommonTypes::DgnaFailed;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eRadioErrorType;
    }


    ProtocolCommonTypes::EBadCommandError CommonTypesConversion::convertToCpp( const TA_Base_Bus::ITrainExceptionsCorba::EBadCommandError& CorbaType )
    {
        ProtocolCommonTypes::EBadCommandError eBadCommandError;

        switch ( CorbaType )
        {

            case TA_Base_Bus::ITrainExceptionsCorba::BadChecksum:
                eBadCommandError = ProtocolCommonTypes::BadChecksum;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::UnknownCommand:
                eBadCommandError = ProtocolCommonTypes::UnknownCommand;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::BadParameter:
                eBadCommandError = ProtocolCommonTypes::BadParameter;
                break;

            case TA_Base_Bus::ITrainExceptionsCorba::UnexpectedCommand:
                eBadCommandError = ProtocolCommonTypes::UnexpectedCommand;
                break;

            default:
                TA_ASSERT( false, "Invalid Enum value" );
                break;
        }

        return eBadCommandError;
    }

}
