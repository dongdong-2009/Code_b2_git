#ifndef ACTIVEPLANMESSAGEBLOCK_H_INCLUDED
#define ACTIVEPLANMESSAGEBLOCK_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_Review_Branch/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageBlock.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #5 $
  *
  *  Last modification: $DateTime: 2011/06/20 20:46:25 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActivePlanMessageBlock : public ACE_Message_Block
	{
	public:
			ActivePlanMessageBlock( const TA_Base_Core::ActivePlanId& plan,
								IActivePlanMessage* data );

		~ActivePlanMessageBlock(void);

		IActivePlanMessage* getControlMessage( void ) const;

		// find out who the message is for
        const TA_Base_Core::ActivePlanId& getPlan( void ) const{ return( m_plan ); }

	private:
		// Make sure these don't get called
		ActivePlanMessageBlock &operator= (const ActivePlanMessageBlock &);
		ActivePlanMessageBlock (const ActivePlanMessageBlock &);

        const TA_Base_Core::ActivePlanId m_plan;
		IActivePlanMessage* m_control;

	}; // ActivePlanMessageBlock
}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEBLOCK_H_INCLUDED
