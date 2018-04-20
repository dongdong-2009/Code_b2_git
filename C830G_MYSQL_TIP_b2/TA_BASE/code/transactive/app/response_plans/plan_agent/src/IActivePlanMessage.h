#ifndef IACTIVEPLANMESSAGE_H_INCLUDED
#define IACTIVEPLANMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/IActivePlanMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class IActivePlanMessage
	{
	// Public methods
	public:
		IActivePlanMessage(){}
		virtual ~IActivePlanMessage(){}

		virtual void execute( ActivePlan& activePlan ) const = 0;
	
	private:
		IActivePlanMessage& operator=( const IActivePlanMessage& );
		IActivePlanMessage( const IActivePlanMessage& );

	}; // IActivePlanMessage
}; // TA_Base_App

#endif // IACTIVEPLANMESSAGE_H_INCLUDED
