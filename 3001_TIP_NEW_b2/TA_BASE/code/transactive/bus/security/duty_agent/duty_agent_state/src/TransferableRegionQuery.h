/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/TransferableRegionQuery.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Gets a list of transferable regions.
  */

#if !defined(EA_60B72BB0_FB68_4916_85B7_F13D2DDA188F__INCLUDED_)
#define EA_60B72BB0_FB68_4916_85B7_F13D2DDA188F__INCLUDED_

namespace TA_Base_Bus
{
	class TransferableRegionQuery : public IDutyQuery
	{

	public:
		TransferableRegionQuery( const TA_Base_Bus::SessionInfo& session, bool checkSubs );

		virtual std::auto_ptr<IDutyQuery> clone() const; 

		virtual void execute(DutyState& state, DASType::DutyAgentPeer* peer );

		virtual void execute(DutyState& state, const DASType::DbKey& region, const DASType::DutySet& dutySet);
		virtual void execute(DutyState& state, const DASType::DbKey& subsystem,
			const DASType::DutySet& dutySet, const SubsystemState& subState );
		virtual bool isValidToCheckSubsystems();

		virtual const DASType::SubsystemSet& getSubsystems() const;
		virtual const DASType::RegionSet&    getRegions()    const;

	private:
		const TA_Base_Bus::SessionInfo&    m_session;
		DASType::SubsystemSet m_subsystems;
		DASType::RegionSet    m_regions;
		bool                  m_checkSubs;
		DASType::DutySet      m_transfered;
		DutyPrimitive         m_emptyDuty;
		std::vector<DASType::DbKey> m_profiles;

	};
}







#endif // !defined(EA_60B72BB0_FB68_4916_85B7_F13D2DDA188F__INCLUDED_)
