/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/BVSStage.cpp $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/cctv/video_switch_agent/src/BVSStage.h"
#include "app/cctv/video_switch_agent/src/AgentModeMonitor.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDBVSStage.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{

    BVSStage::BVSStage( TA_Base_Core::IEntityDataPtr entityData )
    : VideoInput( entityData )
    , m_BVSStageEntityData( boost::dynamic_pointer_cast<TA_Base_Core::BVSStage>( entityData ) )
    , m_pdBVSStage( NULL )
    , m_entityKey( entityData->getKey() )
    {
		if ( m_BVSStageEntityData.get() == NULL )
		{
			TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException(
                      TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                      "Failed to cast entity data to BVSStage data" ) );
        }

		m_pdBVSStage = std::auto_ptr< IPDBVSStage >( PDInterfaceFactory::createBVSStage( m_BVSStageEntityData ) );
		activateServantAtInit();
    }


    BVSStage::~BVSStage()
	{
		// Nothing yet
		deactivateServantAtClose();
    }


    TA_Base_Bus::BVSStageCorbaDef::BVSStageState BVSStage::getBVSStageState()
	{
		AgentModeMonitor::getInstance().verifyInControlMode( "getBVSStageState" );
        if ( m_pdBVSStage.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        return m_pdBVSStage->getBVSStageState();
	}


	void BVSStage::updateBVSStageState(TA_Base_Bus::BVSStageCorbaDef::BVSStageState state)
	{
		if ( m_pdBVSStage.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
        m_pdBVSStage->updateBVSStageState(state);
	}


	void BVSStage::setActiveTrain( CORBA::Octet activeTrainID )
	{

		// verify control mode
		AgentModeMonitor::getInstance().verifyInControlMode( "setActiveTrain" );

        if ( m_pdBVSStage.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
		// pass request to protocol dependent class
        m_pdBVSStage->setActiveTrain( activeTrainID );
	}

	bool BVSStage::isInAnAlarmStack()
	{
		// verify control mode
		AgentModeMonitor::getInstance().verifyInControlMode( "setActiveTrain" );

        if ( m_pdBVSStage.get() == NULL )
        {
            throw TA_Base_Bus::VideoSwitchAgentException( "Internal error: protocol dependent component not initialised" );
        }
		// pass request to protocol dependent class
        return m_pdBVSStage->isInAnAlarmStack();
	}

	void BVSStage::start()
	{
		VideoInput::start();
	}

	void BVSStage::stop()
	{
		deactivateServant();
		VideoInput::stop();
	}

	void BVSStage::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
	{
		TA_ASSERT( m_BVSStageEntityData.get() != NULL, "Entity data has not been initialised" );
		m_BVSStageEntityData->invalidate();

		VideoInput::update( updateEvent );
	}


	void BVSStage::remove()
	{
        try
        {
            SwitchManager::getInstance().removeBVSStage( m_entityKey );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Failed to remove BVSStage from SwitchManager." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Failed to remove BVSStage from SwitchManager." );
        }

        stop();
        VideoInput::remove();
	}


	void BVSStage::activateServantAtInit()
	{
        try
        {
            activateServantWithName( m_BVSStageEntityData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "A BVSStage cannot be activated." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "A BVSStage cannot be activated." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "A BVSStage cannot be activated." );
        }
        catch( ... )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "A BVSStage cannot be activated." );
        }
	}


	void BVSStage::deactivateServantAtClose()
	{
		deactivateServant();
	}
}

