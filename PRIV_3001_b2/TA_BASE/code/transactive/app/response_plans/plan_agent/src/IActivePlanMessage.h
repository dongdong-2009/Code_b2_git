#ifndef IACTIVEPLANMESSAGE_H_INCLUDED
#define IACTIVEPLANMESSAGE_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/IActivePlanMessage.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

namespace TA_Base_App
{
    class ActivePlan;

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
