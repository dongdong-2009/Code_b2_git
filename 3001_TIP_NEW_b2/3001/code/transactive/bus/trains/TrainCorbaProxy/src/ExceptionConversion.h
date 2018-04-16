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

#if !defined(ExceptionConversion_H)
#define ExceptionConversion_H

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/BadCommandException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvConflictException.h"
#include "bus/trains/TrainCommonLibrary/src/CctvException.h"
#include "bus/trains/TrainCommonLibrary/src/GroupInUseException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidSelectionListException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"
#include "bus/trains/TrainCommonLibrary/src/MpuChangeoverException.h"
#include "bus/trains/TrainCommonLibrary/src/OperationNotSupportedException.h"
#include "bus/trains/TrainCommonLibrary/src/PaException.h"
#include "bus/trains/TrainCommonLibrary/src/PecException.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainCallException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimeoutException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTimerException.h"
#include "bus/trains/TrainCommonLibrary/src/TrainTransmissionFailureException.h"
#include "bus/trains/TrainCommonLibrary/src/TransactionCancelledException.h"
#include "bus/trains/TrainCommonLibrary/src/TtisException.h"

#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h"

#include "bus/trains/TrainCorbaProxy/src/CommonTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/CctvTypesConversion.h"
#include "bus/trains/TrainCorbaProxy/src/PecTypesConversion.h"

#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/utilities/src/DebugUtil.h"


/**
 * This is a macro.
 * It takes a corba function call as an argument,
 * and wraps it in a try/catch  block.
 * It is used to catch CORBA exceptions and rethrow c++ exceptions.
 * @param corba_function_call
 */

#define CORBA_CATCH(operation) \
    try\
    {\
        operation;\
    }\
    catch( TA_Base_Core::OperationModeException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", e.reason.in() );\
        \
        TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.reason.in() ) );\
    }\
    catch( TA_Base_Core::OperationNotSupportedException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationNotSupportedException", e.reason.in() );\
        \
        throw TA_IRS_Bus::OperationNotSupportedException( e.reason.in() );\
    }\
    catch( TA_Base_Core::AccessDeniedException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", e.reason.in() );\
        \
        throw TA_IRS_Bus::TrainAccessDeniedException( e.reason.in() );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::InvalidTrainException( e.details.in(), TA_IRS_Bus::CommonTypesConversion::convertToCpp( e.trainId ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::InvalidParameterException( e.details.in(), e.parameterName.in(), e.parameterValue.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::TrainTransmissionFailureException( e.details.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::TrainTimeoutException( e.details.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::TransactionCancelledException( e.details.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::MpuChangeoverException( e.details.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::BadCommandException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::BadCommandException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::BadCommandException( e.details.in(), TA_IRS_Bus::CommonTypesConversion::convertToCpp( e.errorType ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TrainRadioException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TrainRadioException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::TrainRadioException( e.details.in(), TA_IRS_Bus::CommonTypesConversion::convertToCpp( e.errorType ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvConflictException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TrainCctvConflictException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::CctvConflictException( e.details.in(), TA_IRS_Bus::CommonTypesConversion::convertToCpp( e.owningTrain ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::CctvException( e.details.in(), TA_IRS_Bus::CctvTypesConversion::convertToCpp( e.errorType ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::TrainPecException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::TrainPecException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::PecException( e.details.in(), TA_IRS_Bus::PecTypesConversion::convertToCpp( e.errorType ) ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::InvalidSelectionListException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::InvalidSelectionListException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::InvalidSelectionListException( e.details.in() ) );\
    }\
    catch( TA_Base_Bus::ITrainExceptionsCorba::GroupInUseException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::GroupInUseException", e.details.in() );\
        \
        TA_THROW( TA_IRS_Bus::GroupInUseException( e.details.in() ) );\
    }\
    catch( CORBA::Exception& e )\
    {\
        std::string error = TA_Base_Core::CorbaUtil::getInstance().exceptionToString( e );\
        LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", error.c_str() );\
        \
        TA_THROW( TA_IRS_Bus::AgentCommunicationException( error ) );\
    } \
    catch( TA_Base_Core::ObjectResolutionException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException", e.what() );\
        \
        TA_THROW( TA_IRS_Bus::AgentCommunicationException( e.what() ) );\
    }






/**
 * This is a macro.
 * It takes a C++ function call as an argument, and wraps it in a try/catch block.
 *
 * It is used to catch C++ exceptions and rethrow CORBA exceptions.
 *
 * @param cpp_function_call
 */

#define CPP_CATCH(operation) \
    try\
    {\
        operation;\
    }\
    catch( TA_IRS_Bus::TrainAccessDeniedException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );\
        \
        throw TA_Base_Core::AccessDeniedException( e.what() );\
    }\
    catch( TA_IRS_Bus::OperationNotSupportedException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::OperationNotSupportedException", e.what() );\
        \
        throw TA_Base_Core::OperationNotSupportedException( e.what() );\
    }\
    catch( TA_IRS_Bus::InvalidParameterException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException( e.what(), e.getParameterName().c_str(), e.getParameterValue().c_str() );\
    }\
    catch( TA_IRS_Bus::InvalidTrainException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidTrainException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException( e.what(), TA_IRS_Bus::CommonTypesConversion::convertToCorba( e.getTrainId() ) );\
    }\
    catch( TA_IRS_Bus::TrainTransmissionFailureException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTransmissionFailureException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TransmissionFailureException( e.what() );\
    }\
    catch( TA_IRS_Bus::TrainTimerException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimerException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException( e.what() );\
    }\
    catch( TA_IRS_Bus::TrainTimeoutException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainTimeoutException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TransactionTimeoutException( e.what() );\
    }\
    catch( TA_IRS_Bus::TransactionCancelledException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactionCancelledException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TransactionCancelledException( e.what() );\
    }\
    catch( TA_IRS_Bus::InvalidSelectionListException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidSelectionListException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidSelectionListException( e.what() );\
    }\
    catch( TA_IRS_Bus::MpuChangeoverException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::MpuChangeoverException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::MpuChangeoverException( e.what() );\
    }\
    catch( TA_IRS_Bus::BadCommandException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::BadCommandException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::BadCommandException( e.what(), TA_IRS_Bus::CommonTypesConversion::convertToCorba( e.getErrorType() ) );\
    }\
    catch( TA_IRS_Bus::CctvConflictException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::CctvConflictException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TrainCctvConflictException( e.what(), TA_IRS_Bus::CommonTypesConversion::convertToCorba( e.getConflictingTrain() ) );\
    }\
    catch( TA_IRS_Bus::CctvException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::CctvException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TrainCctvException( e.what(), TA_IRS_Bus::CctvTypesConversion::convertToCorba( e.getErrorType() ) );\
    }\
    catch( TA_IRS_Bus::PecException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::PecException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TrainPecException( e.what(), TA_IRS_Bus::PecTypesConversion::convertToCorba( e.getErrorType() ) );\
    }\
    catch( TA_IRS_Bus::TrainRadioException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainRadioException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::TrainRadioException( e.what(), TA_IRS_Bus::CommonTypesConversion::convertToCorba( e.getErrorType() ) );\
    }\
    catch( TA_IRS_Bus::ProtocolException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::ProtocolException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException( e.what(), e.getFieldName().c_str(), e.getFieldValue().c_str() );\
    }\
    catch( TA_IRS_Bus::TrainException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException( e.what(), "", "" );\
    }\
    catch( TA_Base_Core::TransactiveException& e )\
    {\
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TransactiveException", e.what() );\
        \
        throw TA_Base_Bus::ITrainExceptionsCorba::InvalidParameterException( e.what(), "", "" );\
    }


#endif // !defined(ExceptionConversion_H)
