/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/SessionException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This file outlines a simple exception class for use with
  * the SessionManager class in the Control Station. It is
  * derived from TransactiveException.
  * This exception class is used when errors are encountered 
  * when attempting login, load a list of Sessions etc etc.
  */


#include "core\exceptions\src\SessionException.h"
#include "core\utilities\src\DebugUtil.h"

namespace TA_Base_Core
{

    SessionException::SessionException(const char *msg) throw()
        : m_failType(GENERAL_ERROR), TransactiveException(msg)
	{
	}

    SessionException::SessionException(const EFailType failType, const char *msg) throw()
        : m_failType(failType), TransactiveException(msg)
    {
            using TA_Base_Core::DebugUtil;

        switch(m_failType)
        {
        case(AUTHENTICATION_FAILED):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "Authentication failed");
            break;
        case(PASSWORD_MISMATCH):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "Change password failed as the new passwords are different");
            break;
        case(INVALID_PASSWORD_LENGTH):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "Change password failed as the new passwords are shorter than minimum length allowed");
            break;
        case(INVALID_SESSION_CONFIGURATION):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "The operator/profile/location configuration used in this session has been deleted.");
            break;
        case(INVALID_SESSION):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "The session is invalid.");
            break;
        case(AUTHENTICATION_AGENT_FAILED):
            LOG( SourceInfo, DebugUtil::ExceptionDetails, "SessionException", "The authentication agent has possibly died/couldn't be resolved.");
            break;
        default:
            break;
        }
    }

	SessionException::~SessionException() throw()
	{

	}

} // Closes TA_Base_Core
