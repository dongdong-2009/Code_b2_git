/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/EntityCreationFactory.cpp $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Creates and stores the appropriate entity from the given entity data.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <memory>

#include "app/cctv/video_switch_agent/src/EntityCreationFactory.h"
#include "app/cctv/video_switch_agent/src/Camera.h"
#include "app/cctv/video_switch_agent/src/SwitchManager.h"
#include "app/cctv/video_switch_agent/src/VideoMonitor.h"
#include "app/cctv/video_switch_agent/src/VideoOutputGroup.h"
#include "app/cctv/video_switch_agent/src/VideoSwitchAgent.h"
#include "app/cctv/video_switch_agent/src/Quad.h"
//#include "app/cctv/video_switch_agent/src/BVSStage.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "app/cctv/video_switch_agent/src/RecordingUnit.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
	EntityCreationFactory* EntityCreationFactory::s_singleton = NULL;
	TA_Base_Core::ReEntrantThreadLockable EntityCreationFactory::s_singletonLock;

	//
	// getInstance
	//
	EntityCreationFactory& EntityCreationFactory::getInstance()
	{
		if ( s_singleton == NULL )
		{
			TA_Base_Core::ThreadGuard guard( s_singletonLock );
			if ( s_singleton == NULL )
			{
				s_singleton = new EntityCreationFactory();
			}
		}
		return *s_singleton;
	}


	//
	// shutdown
	//
	void EntityCreationFactory::shutdown()
	{
		TA_Base_Core::ThreadGuard guard( s_singletonLock );
		if ( s_singleton != NULL )
		{
			delete s_singleton;
			s_singleton = NULL;
		}
	}


    // 
    // EntityCreationFactory
    //
    EntityCreationFactory::EntityCreationFactory()
    {
        registerFunction( TA_Base_Core::Camera::getStaticType(), createCamera );
        registerFunction( TA_Base_Core::VideoMonitor::getStaticType(), createVideoMonitor );
        registerFunction( TA_Base_Core::VideoOutputGroup::getStaticType(), createVideoOutputGroup );
        registerFunction( TA_Base_Core::Quad::getStaticType(), createQuad );
    //    registerFunction( TA_Base_Core::BVSStage::getStaticType(), createBVSStage );
        registerFunction( TA_Base_Core::Sequence::getStaticType(), createSequence );
        registerFunction( TA_Base_Core::RecordingUnit::getStaticType(), createRecordingUnit );
    }


    //
    // registerFunction
    //
    void EntityCreationFactory::registerFunction( const std::string& type, CreatorFunction function )
    {
        TA_ASSERT( m_functions.end() == m_functions.find( type ),
            "A function is already registered for this type." );
        m_functions[ type ] = function;
    }


    //
    // createEntity
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createEntity( TA_Base_Core::IEntityDataPtr entityData )
    {
        std::string type = entityData->getType();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"EntityType:%s", type.c_str() );
        FunctionMap::iterator it = m_functions.find( type );
        if ( m_functions.end() == it )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "Entity could not be created because it is of an unrecognised type: %s", type.c_str() );
            return 0;
        }
        return it->second( entityData );
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


	// 
    // createCamera
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createCamera( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT( TA_Base_Core::Camera::getStaticType() == entityData->getType(), 
            std::string( "Expecting entity of type: " + TA_Base_Core::Camera::getStaticType() ).c_str() );

        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< Camera > camera = std::auto_ptr< Camera >( new Camera( entityData ) );
            if ( 0 == camera.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new Camera entity for unknown reason." );
                return 0;
            }
            SwitchManager::getInstance().addCamera( *camera );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return camera.release(); 
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new Camera entity" ); 
       }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new Camera entity" ); 
        }

        return 0;
    }


    // 
    // createVideoMonitor
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createVideoMonitor( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT( TA_Base_Core::VideoMonitor::getStaticType() == entityData->getType(), 
            std::string( "Expecting entity of type: " + TA_Base_Core::VideoMonitor::getStaticType() ).c_str() );

        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< VideoMonitor > videoMonitor = 
                std::auto_ptr< VideoMonitor >( new VideoMonitor( entityData ) );
            if ( 0 == videoMonitor.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new VideoMonitor entity for unknown reason." );
                return 0;
            }
            SwitchManager::getInstance().addVideoMonitor( *videoMonitor );
			unsigned long consoleKey=videoMonitor->getConsoleKey();
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"createVideoMonitor Console key:%ld,entityKey:%ld",consoleKey, videoMonitor->getKey());
			
            // Release the auto_ptr so the caller can take ownership of the pointer.

            return videoMonitor.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new VideoMonitor entity" ); 
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new VideoMonitor entity" ); 
        }

        return 0;
    }


    // 
    // createVideoOutputGroup
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createVideoOutputGroup( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT( TA_Base_Core::VideoOutputGroup::getStaticType() == entityData->getType(), 
            std::string( "Expecting entity of type " + TA_Base_Core::VideoOutputGroup::getStaticType() ).c_str() );

        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< VideoOutputGroup > videoOutputGroup = 
                std::auto_ptr< VideoOutputGroup >( new VideoOutputGroup( entityData ) );
            if ( 0 == videoOutputGroup.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new VideoOutputGroup entity for unknown reason." );
                return 0;
            }
            SwitchManager::getInstance().addVideoOutputGroup( *videoOutputGroup );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return videoOutputGroup.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new VideoOutputGroup entity" ); 
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new VideoOutputGroup entity" ); 
        }

        return 0;
    }


	//
	// createQuad
	//
	TA_Base_Bus::IEntity* EntityCreationFactory::createQuad( TA_Base_Core::IEntityDataPtr entityData )
	{
    	TA_ASSERT( TA_Base_Core::Quad::getStaticType() == entityData->getType(),
            		std::string( "Expecting entity of type " + TA_Base_Core::Quad::getStaticType() ).c_str() );
		try
		{
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< Quad > quad = std::auto_ptr< Quad >( new Quad( entityData ) );
            if ( 0 == quad.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new Quad entity for unknown reason." );
                return NULL;
            }
            SwitchManager::getInstance().addQuad( *quad );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return quad.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new Quad entity" ); 
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new Quad entity" ); 
        }

        return NULL;
    }


	//
	// createBVSStage
	//
	/*
	TA_Base_Bus::IEntity* EntityCreationFactory::createBVSStage( TA_Base_Core::IEntityDataPtr entityData )
	{
    	TA_ASSERT( TA_Base_Core::BVSStage::getStaticType() == entityData->getType(),
            		std::string( "Expecting entity of type " + TA_Base_Core::BVSStage::getStaticType() ).c_str() );
		try
		{
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< BVSStage > bvsStage = std::auto_ptr< BVSStage >( new BVSStage( entityData ) );
            if ( 0 == bvsStage.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new BVSStage entity for unknown reason." );
                return NULL;
            }
            SwitchManager::getInstance().addBVSStage( *bvsStage );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return bvsStage.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new BVSStage entity" ); 
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new BVSStage entity" ); 
        }

        return NULL;
    }
	*/

    //
    // createSequence
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createSequence( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT( TA_Base_Core::Sequence::getStaticType() == entityData->getType(),
                    std::string( "Expecting entity of type " + TA_Base_Core::Sequence::getStaticType() ).c_str() );
        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< Sequence > sequence = std::auto_ptr< Sequence >( new Sequence( entityData ) );
            if ( 0 == sequence.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new Sequence entity for unknown reason." );
                return NULL;
            }
            SwitchManager::getInstance().addSequence( *sequence );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return sequence.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new Sequence entity" );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new Sequence entity" );
        }

        return NULL;
    }


    //
    // createRecordingUnit
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createRecordingUnit( TA_Base_Core::IEntityDataPtr entityData )
    {
        TA_ASSERT( TA_Base_Core::RecordingUnit::getStaticType() == entityData->getType(),
                    std::string( "Expecting entity of type " + TA_Base_Core::RecordingUnit::getStaticType() ).c_str() );
        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.

            std::auto_ptr< RecordingUnit > recordingUnit = std::auto_ptr< RecordingUnit >( new RecordingUnit( entityData ) );
            if ( 0 == recordingUnit.get() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Failed to create new RecordingUnit entity for unknown reason." );
                return NULL;
            }
            SwitchManager::getInstance().addRecordingUnit( *recordingUnit );

            // Release the auto_ptr so the caller can take ownership of the pointer.

            return recordingUnit.release();
        }
        catch( const TA_Base_Core::VideoSwitchAgentInternalException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentInternalException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new RecordingUnit entity" );
        }
        catch( const TA_Base_Bus_GZL9::VideoSwitchAgentException& e )
        {
            const char * what_the_ = e.what; // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH( SourceInfo, "VideoSwitchAgentException", what_the_ );
            LOG_EXCEPTION_DETAILS( SourceInfo, "VideoSwitchAgentException", "Failed to create new RecordingUnit entity" );
        }

        return NULL;
    }
			

} // TA_IRS_App
