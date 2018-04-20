/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/EntityTypeException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "EntityTypeException.h"

namespace TA_Base_Core
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	EntityTypeException::EntityTypeException(const char *msg,
                      const std::string& type,
					  const unsigned long key)
		throw(): TransactiveException(msg),
          m_type(type),
		  m_key(key)
	{
		logException();
	}


	EntityTypeException::~EntityTypeException() throw()
	{

	}

	//////////////////////////////////////////////////////////////////////
	// Member Functions
	//////////////////////////////////////////////////////////////////////

	void EntityTypeException::logException()
	{
		//TODO: Add DebugUtil logging of the exception here.

	}

} // Closes TA_Base_Core

