/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/TransferSubsystemsMO.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  The ACE Method Object that executes the transfer (subsystems) method.
  */

#ifndef TRANSFERSUBSYSTEMSMO_H_INCLUDED
#define TRANSFERSUBSYSTEMSMO_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPeerTask;
}

namespace TA_Base_Bus
{
	//Class declaration
	class TransferSubsystemsMO : public DutyPeerTaskMO
	{
	// Public methods
	public:
		TransferSubsystemsMO( DutyPeerTask& task, const ACE_Future<bool>& future,
			DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems );

	protected:

		virtual void execute();

	private:

		TransferSubsystemsMO& operator=( const TransferSubsystemsMO& );
		TransferSubsystemsMO( const TransferSubsystemsMO& );

		DutyPeerTask& m_task;
		DutyState& m_state;
		const TA_Base_Bus::SessionInfo& m_session;
		const DASType::SubsystemMap& m_subsystems;
	}; 
} // TA_Base_Bus

#endif //TRANSFERSUBSYSTEMSMO_H_INCLUDED
