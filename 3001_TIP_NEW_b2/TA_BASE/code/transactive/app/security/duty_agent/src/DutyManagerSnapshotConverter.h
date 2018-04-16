/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/duty_agent/src/DutyManagerSnapshotConverter.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2017/12/22 15:43:41 $
 * Last modified by:  $Author: Ouyang $
 *
 * Converts DutyState to a sequence of Duty Manager updates..
 */

#if !defined(DUTYMANAGERSNAPSHOTCONVERTER_H_INCLUDED)
#define DUTYMANAGERSNAPSHOTCONVERTER_H_INCLUDED

namespace TA_Base_App
{
	class DutyManagerSnapshotConverter : public TA_Base_Bus::IDutyStateConverter
	{

	public:

		DutyManagerSnapshotConverter( const std::string& session );
		
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence* getSubsystemState();
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence* getSubsystemDuty();
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence*    getRegionDuty();

		virtual void setSession(const std::string& session);
		virtual void setAction(const std::string& action);
	
		virtual void setRegionCount( unsigned long count );

		virtual void setRegion(const TA_Base_Bus::DASType::DbKey& region);
		
		virtual void setSubsystemCount( unsigned long count );

		virtual void setSubsystem(const TA_Base_Bus::DASType::DbKey& subsystem,
			const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState );
		
		virtual void setRegionAddCount   ( unsigned long count );
		virtual void setRegionRemoveCount( unsigned long count );

		virtual void regionAdd   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void regionRemove( const TA_Base_Bus::DutyPrimitive& session );
		
		virtual void setSubsystemAddCount    ( unsigned long count );
		virtual void setSubsystemRemoveCount ( unsigned long count );

		virtual void subsystemAdd    ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemRemove ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemDeny   ( const TA_Base_Bus::DutyPrimitive& session );
		virtual void subsystemTimeout( const TA_Base_Bus::DutyPrimitive& session );

		virtual void subsystemComplete();
		virtual void regionComplete();

	private:
		
        TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemStateSequence_var m_subsystemState;
		TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemDutySequence_var m_subsystemDuty;
		TA_Base_Bus::DutyAgentTypeCorbaDef::RegionDutySequence_var  m_regionDuty;

		unsigned long m_subsystemStateIndex;
		unsigned long m_regionIndex;
		unsigned long m_subsystemIndex;

		TA_Base_Bus::DASType::DbKey m_region;
		TA_Base_Bus::DASType::DbKey m_subsystem;
		TA_Base_Bus::SubsystemState::State m_subState;

		const std::string m_session;
	};
}

#endif // !defined(DUTYMANAGERSNAPSHOTCONVERTER_H_INCLUDED)
