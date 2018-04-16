//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/response_plans/active_plans_display/src/IActivePlanAccess.h $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2017/12/12 14:18:48 $
// Last modified by:  $Author: hoa.le $
//
// This interface is used to access active plan details from a Plan Agent.

#if !defined(AFX_IACTIVEPLANACCESS_H__4A59B4A8_1505_48C8_B741_37B4F4B6F113__INCLUDED_)
#define AFX_IACTIVEPLANACCESS_H__4A59B4A8_1505_48C8_B741_37B4F4B6F113__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Core
{
    class ActivePlanStepsDetails;
}


namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // IActivePlanAccess Interface

    class IActivePlanAccess
    {
    public:
		/**
		  * getAllActivePlanDetailsAtLocation
		  * 
		  * Returns a Corba Container of all the current active plans at the specified location.
          * The caller is reponsible for deallocating the memory.
		  *
          * @param  unsigned long location          -   location key
          *
		  * @return TA_Base_Core::ActivePlanDetails *	-	The container of all active plans
		  *
		  */
        virtual TA_Base_Core::ActivePlanStepsDetails* getAllActivePlanDetailsAtLocation(unsigned long location) = 0;
        
		/** 
		  * Destructor
		  *
		  */
        virtual ~IActivePlanAccess() = 0 {};        
    };

} // namespace TA_Base_Bus

#endif // !defined(AFX_IACTIVEPLANACCESS_H__4A59B4A8_1505_48C8_B741_37B4F4B6F113__INCLUDED_)
