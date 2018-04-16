/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/EntityCreationFactory.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
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
#include "app/cctv/video_switch_agent/src/VideoSwitchAgent.h"
#include "app/cctv/video_switch_agent/src/Sequence.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"
#include "app/cctv/video_switch_agent/protocols/COE/src/COESwitchManager.h"
#include "core/exceptions/src/VideoSwitchAgentInternalException.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


namespace TA_IRS_App
{
    //
    // getInstance
    //
    EntityCreationFactory& EntityCreationFactory::getInstance()
    {
		return *(ACE_Singleton<EntityCreationFactory, ACE_Recursive_Thread_Mutex>::instance());
    }


    EntityCreationFactory::~EntityCreationFactory()
	{
	}
    //
    // shutdown
    //
    void EntityCreationFactory::shutdown()
    {
		ACE_Singleton<EntityCreationFactory, ACE_Recursive_Thread_Mutex>::close();
    }


    //
    // EntityCreationFactory
    //
    EntityCreationFactory::EntityCreationFactory()
    {
        registerFunction(TA_Base_Core::Camera::getStaticType(), createCamera);
        registerFunction(TA_Base_Core::VideoMonitor::getStaticType(), createVideoMonitor);
        registerFunction(TA_Base_Core::Sequence::getStaticType(), createSequence);
    }


    //
    // registerFunction
    //
    void EntityCreationFactory::registerFunction(const std::string& type, CreatorFunction function)
    {
        TA_ASSERT(m_functions.end() == m_functions.find(type), "A function is already registered for this type.");
        m_functions[ type ] = function;
    }


    //
    // createEntity
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createEntity(TA_Base_Core::IEntityDataPtr entityData)
    {
        std::string type = entityData->getType();
        FunctionMap::iterator it = m_functions.find(type);
        if(m_functions.end() == it)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Entity could not be created because it is of an unrecognised type: %s", type.c_str());
            return 0;
        }
        return it->second(entityData);
    }


    ///////////////////////////////////////////////////////////////////////
    //
    // Private methods
    //
    ///////////////////////////////////////////////////////////////////////


    //
    // createCamera
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createCamera(TA_Base_Core::IEntityDataPtr entityData)
    {
        TA_ASSERT(TA_Base_Core::Camera::getStaticType() == entityData->getType(), "");
        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.
            std::auto_ptr< Camera > camera = std::auto_ptr< Camera >(new Camera(entityData));
            SwitchManager::getInstance().addCamera(*camera);
            // Release the auto_ptr so the caller can take ownership of the pointer.
            return camera.release();
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new Camera entity");
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", "Failed to create new Camera entity");
        }
        return 0;
    }


    //
    // createVideoMonitor
    //
    TA_Base_Bus::IEntity* EntityCreationFactory::createVideoMonitor(TA_Base_Core::IEntityDataPtr entityData)
    {
        TA_ASSERT(TA_Base_Core::VideoMonitor::getStaticType() == entityData->getType(),"");
        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.
            std::auto_ptr< VideoMonitor > videoMonitor = std::auto_ptr< VideoMonitor >(new VideoMonitor(entityData));
            SwitchManager::getInstance().addVideoMonitor(*videoMonitor);
            // Release the auto_ptr so the caller can take ownership of the pointer.
            return videoMonitor.release();
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new VideoMonitor entity");
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", "Failed to create new VideoMonitor entity");
        }
        return 0;
    }


    //
    // createQuad
    //
    /*TA_Base_Bus::IEntity* EntityCreationFactory::createQuad( TA_Base_Core::IEntityDataPtr entityData )
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
        catch( const TA_Base_Bus::VideoSwitchAgentException& e )
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
        catch( const TA_Base_Bus::VideoSwitchAgentException& e )
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
    TA_Base_Bus::IEntity* EntityCreationFactory::createSequence(TA_Base_Core::IEntityDataPtr entityData)
    {
        TA_ASSERT(TA_Base_Core::Sequence::getStaticType() == entityData->getType(), "");
        try
        {
            // Make it an auto_ptr so it is automatically cleaned up if an exception is thrown.
            std::auto_ptr< Sequence > sequence = std::auto_ptr< Sequence >(new Sequence(entityData));
            SwitchManager::getInstance().addSequence(*sequence);
            // Release the auto_ptr so the caller can take ownership of the pointer.
            return sequence.release();
        }
        catch(const TA_Base_Core::VideoSwitchAgentInternalException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentInternalException", e.what());
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentInternalException", "Failed to create new Sequence entity");
        }
        catch(const TA_Base_Bus::VideoSwitchAgentException& e)
        {
            const char* what_the_ = e.what;  // uses the CORBA::String_member::operator char*() to return the class _ptr
            LOG_EXCEPTION_CATCH(SourceInfo, "VideoSwitchAgentException", what_the_);
            LOG_EXCEPTION_DETAILS(SourceInfo, "VideoSwitchAgentException", "Failed to create new Sequence entity");
        }
        return NULL;
    }
} // TA_IRS_App
