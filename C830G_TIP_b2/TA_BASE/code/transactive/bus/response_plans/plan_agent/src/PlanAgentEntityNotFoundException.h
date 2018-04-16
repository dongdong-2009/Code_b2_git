//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/response_plans/plan_agent/src/PlanAgentEntityNotFoundException.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// Thrown by PlanAgentLibrary when no entity of type PlanAgent can be found at the specified location.

#if !defined(_PLANAGENTENTITYNOTFOUNDEXCEPTION__INCLUDED_)
#define _PLANAGENTENTITYNOTFOUNDEXCEPTION__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"


namespace TA_Base_Bus
{
    class PlanAgentEntityNotFoundException : public TA_Base_Core::TransactiveException
    {
    // Operations
    public:
        PlanAgentEntityNotFoundException() throw();
        PlanAgentEntityNotFoundException(std::string msg) throw();
        virtual ~PlanAgentEntityNotFoundException() throw();
    };

} // namespace TA_Base_Bus

#endif // !defined(_PLANAGENTENTITYNOTFOUNDEXCEPTION__INCLUDED_)
