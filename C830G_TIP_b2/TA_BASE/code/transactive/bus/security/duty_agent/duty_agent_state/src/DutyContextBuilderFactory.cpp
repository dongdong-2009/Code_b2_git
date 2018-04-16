/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyContextBuilderFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements DutyContextBuilderFactoryImpl
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	/*********** RAIL ***********/
	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::buildAllocRegionContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& regionKey ) const
	{
		context.m_dstPrfType = DutyDbCache::instance()->getProfileType( session.m_profile );

		setRegionEquality( context, session, regionKey );

		// Put this into a place holder so it can be processed when the source session details
		// are available (when setSrcSession() is called).
		context.m_dstPrfGrp = DutyDbCache::instance()->getProfileGroup( session.m_profile );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::buildAllocSubContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& subsystemKey,
		const SubsystemState::State& preState, const SubsystemState::State& postState ) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::buildXferSubContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& subsystemKey,
		const SubsystemState::State& preState, const SubsystemState::State& postState ) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::buildXferRegionContext(
		DutyContext& context, const DutyPrimitive& session ) const
	{
		context.m_dstPrfGrp = DutyDbCache::instance()->getProfileGroup( session.m_profile );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::buildXferSubViaRegionContext(
		DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
		context.m_dstPrfGrp = DutyDbCache::instance()->getProfileGroup( session.m_profile );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::setSrcSession(
		DutyContext& context, const DutyPrimitive& session ) const
	{
		setProfileGroupEquality( context, session );
		setSrc( context, session );
	}

	void DutyContextBuilderFactoryImpl::RailDutyContextBuilder::setNullSrcSession( DutyContext& context ) const
	{
		setNullSrc( context );
	}

	/*********** ROAD ***********/
	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::buildAllocRegionContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& regionKey ) const
	{
		context.m_dstPrfType = DutyDbCache::instance()->getProfileType( session.m_profile );

		setRegionEquality( context, session, regionKey );
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::buildAllocSubContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& subsystemKey,
		const SubsystemState::State& preState, const SubsystemState::State& postState ) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::buildXferSubContext(
		DutyContext& context, const DutyPrimitive& session, const DASType::DbKey& subsystemKey,
		const SubsystemState::State& preState, const SubsystemState::State& postState ) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::buildXferRegionContext(
		DutyContext& context, const DutyPrimitive& session ) const
	{
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::buildXferSubViaRegionContext(
		DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState) const
	{
		setSubContext( context, session, subsystemKey, preState, postState );
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::setSrcSession(
		DutyContext& context, const DutyPrimitive& session ) const
	{
		setSrc( context, session );
	}

	void DutyContextBuilderFactoryImpl::RoadDutyContextBuilder::setNullSrcSession( DutyContext& context ) const
	{
		setNullSrc( context );
	}

	/*********** FACTORY ***********/
	DutyContextBuilderFactoryImpl::DutyContextBuilderFactoryImpl()
	{
		try
		{
			m_dutyContextBuilder.push_back( new RailDutyContextBuilder() );
			m_dutyContextBuilder.push_back( new RoadDutyContextBuilder() );
		}
		catch(...)
		{
			std::vector<IDutyContextBuilder*>::iterator it;

			for( it=m_dutyContextBuilder.begin(); it!=m_dutyContextBuilder.end(); it++ )
			{
				delete *it;
			}

			throw;
		}
	}

	DutyContextBuilderFactoryImpl::~DutyContextBuilderFactoryImpl()
	{
		delete m_dutyContextBuilder[DASConst::POLICY_TYPE_RAIL];
		delete m_dutyContextBuilder[DASConst::POLICY_TYPE_ROAD];
	}

	const IDutyContextBuilder& DutyContextBuilderFactoryImpl::getDutyContextBuilder() const
	{
		return( *(m_dutyContextBuilder[TransactiveAgentInfo::m_policyType]) );
	}
}
