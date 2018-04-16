#ifndef  TEST_AUDIT_MESSAGE_TYPES
#define TEST_AUDIT_MESSAGE_TYPES
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/message/types/TESTAUDIT_MessageTypes.h $
 * @author:  Karen Graham
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * Message contexts and types for test purposes
 *
 */

#include "MessageTypes.h"

namespace TA_Base_Core
{
	namespace TESTAUDIT 
	{
		//
		// Test Contexts
		//
		static MessageContext Context("Audit","TESTAUDIT", AuditMessage);

		// Individual Message Types
		//
		static const MessageType TestAuditType1(Context, "99998");
		static const MessageType TestAuditType2(Context, "99999");
	};
};

#endif
