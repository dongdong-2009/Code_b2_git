/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyPeer.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Interface abstraction for local and remote duty peers. Local duty peers
  *  interface to the duty peer library, remote duty peers interface to the
  *  duty peer IDL.
  */
#ifndef IDUTYPEER_H_INCLUDED
#define IDUTYPEER_H_INCLUDED

namespace TA_Base_Bus
{
	class IDutyPeer
	{
	public:
		virtual ~IDutyPeer(){}

		/**
		 * An operator has logged on.
		 * @param state   Communicates any changes in state.
		 * @param session Operator details
		 * 
		 */
		virtual void sessionBegin        ( DutyState& state, const TA_Base_Bus::SessionInfo& session ) = 0;
		virtual void sessionEnd          ( DutyState& state, const TA_Base_Bus::SessionInfo& session,
			const DASType::RegionSet& regions ) = 0;
		virtual void sessionOverrideBegin( DutyState& state, const TA_Base_Bus::SessionInfo& session ) = 0;
		virtual void sessionOverrideEnd  ( DutyState& state, const TA_Base_Bus::SessionInfo& session ) = 0;
		
		virtual void transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions ) = 0;
		virtual void transfer( DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::SubsystemMap& subsystems ) = 0;
		
		virtual void degrade( DutyState& state, const DASType::SubsystemSet& subsystems) = 0;

		virtual void executeQuery( DutyState& state, IDutyQuery& query ) = 0;
	};
}
#endif //  IDUTYPEER_H_INCLUDED
