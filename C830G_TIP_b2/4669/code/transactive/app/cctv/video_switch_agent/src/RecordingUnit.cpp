/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/RecordingUnit.cpp $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2010/01/26 19:01:38 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/RecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDRecordingUnit.h"
#include "app/cctv/video_switch_agent/protocols/src/PDInterfaceFactory.h"
#include "app/cctv/video_switch_agent/src/VisualAuditMessageSender.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

namespace TA_IRS_App
{

    //
    // Constructor
    //
    RecordingUnit::RecordingUnit( TA_Base_Core::IEntityDataPtr entityData )
    : VideoSwitchResource( entityData )
    , m_recordingUnitEntityData( boost::dynamic_pointer_cast< TA_Base_Core::RecordingUnit >( entityData ) )
    , m_pdRecordingUnit( NULL )
    , m_userPrivilegeVerifier( UserPrivilegeVerifier::getInstance() ) // HuangQi++
    , m_entityKey( entityData->getKey() )
    {
		if ( m_recordingUnitEntityData.get() == NULL )
		{
			TA_THROW( TA_Base_Core::VideoSwitchAgentInternalException(
                      TA_Base_Core::VideoSwitchAgentInternalException::INVALID_CONFIG,
                      "Failed to cast entity data to RecordingUnit data" ) );
        }

		m_pdRecordingUnit = std::auto_ptr< IPDRecordingUnit >( PDInterfaceFactory::createRecordingUnit( m_recordingUnitEntityData ) );

		activateServantAtInit();
    }


    //
    // Destructor
    //
    RecordingUnit::~RecordingUnit()
    {
        // Nothing yet
		deactivateServantAtClose();
    }


    //
    // updateState
    //
    void RecordingUnit::updateState( const TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState& state )
    {
        FUNCTION_ENTRY( "updateState" );

        m_pdRecordingUnit->updateState( state );     

        FUNCTION_EXIT;
    }


	//
	// getSupportedFeatures
	//
    TA_Base_Bus::RecordingUnitCorbaDef::SupportedRecordingUnitFeatures RecordingUnit::getSupportedFeatures()
    {
        FUNCTION_ENTRY( "getSupportedFeatures" );

        FUNCTION_EXIT;
        return m_pdRecordingUnit->getSupportedFeatures();
    }


	//
    // getRecordingUnitState
	//
	TA_Base_Bus::RecordingUnitCorbaDef::RecordingUnitState RecordingUnit::getRecordingUnitState()
    {
        FUNCTION_ENTRY( "getRecordingUnitState" );

        FUNCTION_EXIT;
        return m_pdRecordingUnit->getRecordingUnitState();
    }


    //	
	// getRecordingSpeeds
    //
	TA_Base_Bus::RecordingUnitCorbaDef::recordingSpeedSequence* RecordingUnit::getRecordingSpeeds()
    {
        FUNCTION_ENTRY( "getRecordingSpeeds" );

        return m_pdRecordingUnit->getRecordingSpeeds();
        FUNCTION_EXIT;
    }


	//
	// play 
	//
	void RecordingUnit::play( const char* sessionId )
    {
    }


	//
	// fastForward
	//
	void RecordingUnit::fastForward( const char* sessionId )
    {
    }


	//
    // rewind
    //
	void RecordingUnit::rewind( const char* sessionId )
    {
    }


	//
	// pause
	//
	void RecordingUnit::pause( const char* sessionId )
    {
    }


	//
	// stopPlayback
	//
	void RecordingUnit::stopPlayback( const char* sessionId )
    {
    }


	//
	// skipToNextChapter
	//
	void RecordingUnit::skipToNextChapter( const char* sessionId )
    {
    }


	//
	// skipToPreviousChapter
	//
	void RecordingUnit::skipToPreviousChapter( const char* sessionId )
    {
    }


	//
	// record
    //
	void RecordingUnit::record( const char* sessionId )
    {
    }


	//
    // stopRecording
	//
	void RecordingUnit::stopRecording( const char* sessionId )
    {
    }


	//
	// setRecordingSpeed
	//
	void RecordingUnit::setRecordingSpeed( unsigned short recordingSpeedIndex, const char* sessionId )   
    {
        FUNCTION_ENTRY( "setRecordingSpeed" );
    
        m_pdRecordingUnit->setRecordingSpeed( recordingSpeedIndex );

        try
        {
            VisualAuditMessageSender::getInstance().sendMessage( VisualAuditMessageSender::ActionSuccess, m_entityKey, sessionId, "Toggle Recording Unit Speed", m_recordingUnitEntityData->getName() );
        }
        catch ( ... )
        {
        }

        FUNCTION_EXIT;
    }


    //
    // getConsoleKey
    //
    unsigned long RecordingUnit::getConsoleKey()
    {
        TA_ASSERT( 0 != m_recordingUnitEntityData.get(), "The Recording Unit database object has not been set." );

        unsigned long key = m_recordingUnitEntityData->getConsoleKey();
        return m_recordingUnitEntityData->getConsoleKey();
    }


    //
    // start
    //
	void RecordingUnit::start()
    {
    }


    //
    // stop
    //
	void RecordingUnit::stop()
    {
        deactivateServant();
    }


    //
    // update
    //
	void RecordingUnit::update( const TA_Base_Core::ConfigUpdateDetails& updateEvent )
    {
    }


    //
    // remove
	void RecordingUnit::remove()
    {
        // Remove the object from the SwitchManager object.

        SwitchManager::getInstance().removeRecordingUnit( m_entityKey );
        stop();
    }


    //
    // setToControlMode
    //
	void RecordingUnit::activateServantAtInit()
    {
        FUNCTION_ENTRY( "setToControlMode" );
       
        try
        {
            activateServantWithName( m_recordingUnitEntityData->getName() );
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "A RecordingUnit cannot be activated." );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "A RecordingUnit cannot be activated." );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "TransactiveException", "A RecordingUnit cannot be activated." );
        }
        catch( ... )
        {
            // There is nothing we can do to recover.

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "A RecordingUnit cannot be activated." );
        }
        
        FUNCTION_EXIT;
    }

	
    //
    // setMonitorMode
    //
    void RecordingUnit::deactivateServantAtClose()
    {
        deactivateServant();
    }

}
