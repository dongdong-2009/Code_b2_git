/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File$
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision$
  *
  *  Last modification: $DateTime$
  *  Last modified by:  $Author$
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
