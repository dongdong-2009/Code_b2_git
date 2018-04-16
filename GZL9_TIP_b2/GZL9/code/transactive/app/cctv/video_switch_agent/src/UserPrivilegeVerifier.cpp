/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/UserPrivilegeVerifier.cpp $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/11/04 16:46:04 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Verifies the user privileges for the given function.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_IRS_App
{
	InternalUserPrivilegeVerifier* UserPrivilegeVerifier::s_pInstance = 0;
	TA_Base_Core::NonReEntrantThreadLockable UserPrivilegeVerifier::s_singletonLock;

	UserPrivilegeVerifier::UserPrivilegeVerifier()
	{
		if ( 0 == s_pInstance )
		{
			TA_Base_Core::ThreadGuard guard(s_singletonLock);
			if ( 0 == s_pInstance )
			{
				s_pInstance = new InternalUserPrivilegeVerifier();
				TA_ASSERT( ( 0 != s_pInstance ), "Can't not create instance of InternalUserPrivilegeVerifier" );
			}
		}        	
	}

	void UserPrivilegeVerifier::verifyRights( const std::string& sessionId, TA_Base_Core::IEntityDataPtr data,
		unsigned long actionKey, const std::string& actionName )
	{
		s_pInstance->verifyRights(sessionId, data, actionKey, actionName);
	}
    //
    // UserPrivilegeVerifier
    //
    InternalUserPrivilegeVerifier::InternalUserPrivilegeVerifier()
        : m_rightsLibrary( m_rightsFactory.buildRightsLibrary() ),
        //: m_rightsLibrary( m_rightsFactory.buildRightsLibrary( TA_Base_Bus_GZL9::ALWAYS_TRUE_RL ) ),
          m_decisionModule( TA_Base_Bus::UNDEFINED_DM )
    {
        // Do nothing
    }


    //
    // verifyRights
    //
    void InternalUserPrivilegeVerifier::verifyRights( const std::string& sessionId, 
        TA_Base_Core::IEntityDataPtr data, unsigned long actionKey, 
        const std::string& actionName )
    {
        // Check that the rights library has built successfully.

        if ( 0 == m_rightsLibrary.get() )
        {
		    std::stringstream error;
            error << "UE-020056 - The system could not determine the rights of the current operator to " << actionName << ".";
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Rights Library failed to build." );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }

     	// Obtain resource id for this entity.
        // We are responsible for cleaning up the resource object, so make it an auto_ptr
        // so it is automatically cleaned up.

        std::auto_ptr< TA_Base_Core::IResource > resource;
        unsigned long resourceKey = 0;
        try
        {
            resource = std::auto_ptr< TA_Base_Core::IResource >(
                TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( data->getKey() ) );
            if( 0 == resource.get() )
            {
		        std::stringstream error;
                error << "UE-020056 - The system could not determine the rights of the current operator to " << actionName << ".";
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to determine source from entity." );
		        throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
            }
            resourceKey = resource->getKey();
        }
        catch( const TA_Base_Core::DatabaseException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DatabaseException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException." );
		    std::stringstream error;
            error << "UE-020056 - The system could not determine the rights of the current operator to " << actionName << ".";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }
        catch( const TA_Base_Core::DataException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "DataException", "Converting to ExceptionsCorbaDef::VideoSwitchAgentException." );
		    std::stringstream error;
            error << "UE-020056 - The system could not determine the rights of the current operator to " << actionName << ".";
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
        }

        // Check the rights.

        try
		{
		    std::string rightsDeniedReason;
			if ( false == m_rightsLibrary->isActionPermittedOnResource( sessionId, resourceKey, 
                actionKey, rightsDeniedReason, m_decisionModule ) )
			{
		        std::stringstream error;
				// 16879
                error << "UE-010021";
				// 16879
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, error.str().c_str() );
                LOGMORE( SourceInfo, "rightsDeniedReason: %s, Decision module: %d", rightsDeniedReason.c_str(), m_decisionModule );
                throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
            }
		}
		catch( const TA_Base_Core::RightsException& e )
		{
		    std::stringstream error;
            error << "UE-020056 - The system could not determine the rights of the current operator to " << actionName << ".";
			LOG_EXCEPTION_CATCH( SourceInfo, "RightsException", e.what() );
            LOG_EXCEPTION_DETAILS( SourceInfo, "RightsException", error.str().c_str() );
            throw TA_Base_Bus_GZL9::VideoSwitchAgentException( error.str().c_str() );
		}
    }

} // TA_IRS_App
