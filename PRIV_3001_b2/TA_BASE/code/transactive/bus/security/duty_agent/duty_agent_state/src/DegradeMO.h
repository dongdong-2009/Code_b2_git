/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DegradeMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  The ACE Method Object that executes the degrade method.
  */

#ifndef DEGRADEMO_H_INCLUDED
#define DEGRADEMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class DegradeMO : public DutyPeerTaskMO
	{
	// Public methods
	public:
		DegradeMO( DutyPeerTask& task, const ACE_Future<bool>& future,
			DutyState& state, const DASType::SubsystemSet& subsystems );

	protected:

		virtual void execute();

	private:

		DegradeMO& operator=( const DegradeMO& );
		DegradeMO( const DegradeMO& );

		DutyPeerTask& m_task;
		DutyState& m_state;
		const DASType::SubsystemSet& m_subsystems;
	}; 
} // TA_Base_Bus

#endif //DEGRADEMO_H_INCLUDED
