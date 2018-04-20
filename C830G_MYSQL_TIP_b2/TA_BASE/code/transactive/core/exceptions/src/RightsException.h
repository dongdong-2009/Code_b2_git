#ifndef RIGHTSEXCEPTION_H_INCLUDED
#define RIGHTSEXCEPTION_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/RightsException.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include <string>
#include "core/exceptions/src/TransactiveException.h"

namespace TA_Base_Core
{
	class RightsException : public TransactiveException
		{
		public:
            enum ERightsFailType
            {
                UNCONTACTABLE, // agent is not contactable
                INDETERMINATE, // rights is not determinable
                UNAPPLICABLE,  // rights is unapplicable
                GENERAL        // some general error
            };
			RightsException() throw();
			RightsException(const std::string& msg, ERightsFailType type = GENERAL) throw();
			virtual const char* what() const throw();
			virtual ~RightsException() throw();
            ERightsFailType getFailType() const;
		private:
			const std::string m_msg;
            const ERightsFailType m_type;
		};
}; // TA_Base_Core

#endif // RIGHTSEXCEPTION_H_INCLUDED
