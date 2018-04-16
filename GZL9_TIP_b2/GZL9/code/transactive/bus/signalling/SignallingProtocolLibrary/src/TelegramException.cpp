/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/bus/signalling/SignallingProtocolLibrary/src/TelegramException.cpp $
  * @author:  Mark See
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 10:00:53 $
  * Last modified by:  $Author: builder $
  *
  * TelegramException.cpp: implementation of the TelegramException class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/signalling/SignallingProtocolLibrary/src/TelegramException.h"


TelegramException::TelegramException(EErrorType errorType, 
                                     const std::string& description, 
                                     const std::string& fieldName)
                                      throw():TA_Base_Core::TransactiveException(description), 
                                     m_errorType(errorType),
                                     m_fieldName(fieldName),
                                     m_msg()
{
    FUNCTION_ENTRY("TelegramException");
    std::string strError;

    switch(m_errorType)
    {
        case FieldNotDefined:
            strError = "FieldNotDefined";
            break;

        case InvalidValue:
            strError = "InvalidValue";
            break;

        case IncompletePacket:
            strError = "IncompletePacket";
            break;
            
        default:
            strError = "UnknownError";
    }

    m_msg += description.c_str(); //std::string(TA_Base_Core::TransactiveException().what());
    m_msg += " Error Type: ";
    m_msg += strError;
    
    if(m_fieldName.empty() != true)
    {
        m_msg += " Caused by ";
        m_msg += m_fieldName;
    }
    
    FUNCTION_EXIT;
}

TelegramException::~TelegramException() throw()
{
    FUNCTION_ENTRY("~TelegramException");
    FUNCTION_EXIT;
}

const char* TelegramException::what() const throw()
{
    FUNCTION_ENTRY("what");
    FUNCTION_EXIT;
    return m_msg.c_str();
}

TelegramException::EErrorType TelegramException::getError() const
{
    FUNCTION_ENTRY("getError");
    FUNCTION_EXIT;
    return m_errorType;    
}