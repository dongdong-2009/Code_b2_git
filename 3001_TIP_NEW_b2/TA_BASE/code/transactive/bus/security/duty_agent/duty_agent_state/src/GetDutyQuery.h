/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/GetDutyQuery.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Queries duty for the session at the subsystem/region combo.
  */

#if !defined(GETDUTYQUERY_H_INCLUDED)
#define GETDUTYQUERY_H_INCLUDED

namespace TA_Base_Bus
{
	class GetDutyQuery : public IDutyQuery
	{

	public:
		GetDutyQuery( const TA_Base_Bus::SessionInfo& session, const DASType::DbKey& region, const DASType::DbKey& subsystem );

		virtual std::auto_ptr<IDutyQuery> clone() const;

		virtual void execute(DutyState& state, DASType::DutyAgentPeer* peer );

		virtual void execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet);
		virtual void execute(DutyState& state, const DASType::DbKey& subsystem,
			const DASType::DutySet& dutySet, const SubsystemState& subState );
		virtual bool isValidToCheckSubsystems();

		virtual const DASType::SubsystemSet& getSubsystems() const;
		virtual const DASType::RegionSet& getRegions()    const;

	private:
		const TA_Base_Bus::SessionInfo& m_session;
		DASType::SubsystemSet m_subsystems;
		DASType::RegionSet    m_regions;

		std::vector<DASType::DbKey> m_profiles;
		DASType::DutySet m_regionDuty;
	};
}

#endif // !defined(GETDUTYQUERY_H_INCLUDED)
