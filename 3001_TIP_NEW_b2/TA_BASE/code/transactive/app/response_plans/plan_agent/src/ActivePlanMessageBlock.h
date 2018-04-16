#ifndef ACTIVEPLANMESSAGEBLOCK_H_INCLUDED
#define ACTIVEPLANMESSAGEBLOCK_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanMessageBlock.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "ace/Message_Block.h"

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"

namespace TA_Base_App
{

class IActivePlanMessage;

//Class declaration
class ActivePlanMessageBlock : public ACE_Message_Block
{
public:
    ActivePlanMessageBlock( const TA_Base_Core::ActivePlanId& plan, IActivePlanMessage* data );

	~ActivePlanMessageBlock(void);

	IActivePlanMessage* getControlMessage() const;

	// find out who the message is for
    const TA_Base_Core::ActivePlanId& getPlan() const;

private:
	// Make sure these don't get called
	ActivePlanMessageBlock &operator= (const ActivePlanMessageBlock &);
	ActivePlanMessageBlock (const ActivePlanMessageBlock &);

    const TA_Base_Core::ActivePlanId m_plan;
	IActivePlanMessage* m_control;

}; // ActivePlanMessageBlock

}; // TA_Base_App

#endif // ACTIVEPLANMESSAGEBLOCK_H_INCLUDED
