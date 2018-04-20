#ifndef VIDEO_SWITCH_AGENT_EXCEPTIONS_H
#define VIDEO_SWITCH_AGENT_EXCEPTIONS_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/Exceptions.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Specifies all the exceptions thrown by the VideoSwitchAgent.
  */

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
    class ConstructionFailedException : public TA_Base_Core::TransactiveException
    {
    public:
        ConstructionFailedException( const std::string& what ) throw();
    //    virtual ~ConstructionFailedException() throw();
    };

    class UserDoesNotHoldLockException : public TA_Base_Core::TransactiveException
    {
    public:
        UserDoesNotHoldLockException( const std::string& lockOwner ) throw();
        virtual ~UserDoesNotHoldLockException() throw();

    private:
        std::string m_lockOwner;
    };



    class ObjectAlreadyExistsException : public TA_Base_Core::TransactiveException
    {
    public:
        ObjectAlreadyExistsException( const std::string& what ) throw();
        virtual ~ObjectAlreadyExistsException() throw();
    };

    class NotSupportedException : public TA_Base_Core::TransactiveException
    {
    public:
        NotSupportedException( const std::string& what ) throw();
        virtual ~NotSupportedException() throw();
    };


    class InsufficientPrivilegesException : public TA_Base_Core::TransactiveException
    {
    public:
        InsufficientPrivilegesException( const std::string& what ) throw();
        virtual ~InsufficientPrivilegesException() throw();
    };


    class InvalidConfigurationDataException : public TA_Base_Core::TransactiveException
    {
    public:
        InvalidConfigurationDataException( const std::string& what ) throw();
        virtual ~InvalidConfigurationDataException() throw();
    };


    class MaximumTextLengthExceededException : public TA_Base_Core::TransactiveException
    {
    public:
        MaximumTextLengthExceededException( const std::string& what ) throw();
        virtual ~MaximumTextLengthExceededException() throw();
    };


    class UnknownVideoInputException : public TA_Base_Core::TransactiveException
    {
    public:
        UnknownVideoInputException( const std::string& what ) throw();
        virtual ~UnknownVideoInputException() throw();
    };

    class InvalidTypeException : public TA_Base_Core::TransactiveException
    {
    public:
        InvalidTypeException( const std::string& what ) throw();
        virtual ~InvalidTypeException() throw();
    };
}

#endif // VIDEO_SWITCH_AGENT_EXCEPTIONS_H

