/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyQuery.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 *
 * Command pattern for hiding all the different queries on the state of the duty
 * agent from the business logic. This isolates the important business logic and
 * allows new queries to be added fairly painlessly.
 */

#if !defined(EA_FC969699_08A6_472e_A1D3_41294DAA712C__INCLUDED_)
#define EA_FC969699_08A6_472e_A1D3_41294DAA712C__INCLUDED_

namespace TA_Base_Bus
{
	class IDutyQuery
	{

	public:

		virtual ~IDutyQuery(){}

		virtual std::auto_ptr<IDutyQuery> clone() const = 0;

		virtual void execute(DutyState& state, DASType::DutyAgentPeer* peer ) = 0;

		virtual void execute(DutyState& state, const unsigned long& region, const DASType::DutySet& dutySet) = 0;
		virtual void execute(DutyState& state, const unsigned long& subsystem,
			const DASType::DutySet& dutySet, const SubsystemState& subState ) = 0;
		virtual bool isValidToCheckSubsystems() = 0;
		virtual const DASType::SubsystemSet& getSubsystems() const = 0;
		virtual const DASType::RegionSet&    getRegions()    const = 0;
	};
}







#endif // !defined(EA_FC969699_08A6_472e_A1D3_41294DAA712C__INCLUDED_)
