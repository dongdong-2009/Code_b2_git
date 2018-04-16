/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/Exceptions.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Specifies all the exceptions thrown by the VideoSwitchAgent.
  */

#include "app/cctv/video_switch_agent/src/Exceptions.h"

namespace TA_Base_Core
{
        ConstructionFailedException::ConstructionFailedException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
//        ConstructionFailedException::~ConstructionFailedException() throw() {};

        UserDoesNotHoldLockException::UserDoesNotHoldLockException( const std::string& lockOwner ) throw()
            : TransactiveException( "User does not hold lock" ),
              m_lockOwner( lockOwner ) 
        {
            // Do nothing.
        }
        UserDoesNotHoldLockException::~UserDoesNotHoldLockException() throw() {};

        ObjectAlreadyExistsException::ObjectAlreadyExistsException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        ObjectAlreadyExistsException::~ObjectAlreadyExistsException() throw() {};

        NotSupportedException::NotSupportedException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        NotSupportedException::~NotSupportedException() throw() {};


        InsufficientPrivilegesException::InsufficientPrivilegesException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        InsufficientPrivilegesException::~InsufficientPrivilegesException() throw() {};


        InvalidConfigurationDataException::InvalidConfigurationDataException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        InvalidConfigurationDataException::~InvalidConfigurationDataException() throw() {};


        MaximumTextLengthExceededException::MaximumTextLengthExceededException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        MaximumTextLengthExceededException::~MaximumTextLengthExceededException() throw() {};


        UnknownVideoInputException::UnknownVideoInputException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        UnknownVideoInputException::~UnknownVideoInputException() throw() {};

        InvalidTypeException::InvalidTypeException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        InvalidTypeException::~InvalidTypeException() throw() {};
}

