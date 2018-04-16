/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Katherine Thomson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Verifies the user privileges for the given function.
  */

#include <sstream>

#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

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
        if(0 == s_pInstance)
        {
            TA_Base_Core::ThreadGuard guard(s_singletonLock);
            if(0 == s_pInstance)
            {
                s_pInstance = new InternalUserPrivilegeVerifier();
                TA_ASSERT(0 != s_pInstance, "Can't not create instance of InternalUserPrivilegeVerifier");
            }
        }
    }

    void UserPrivilegeVerifier::verifyRights(const std::string& sessionId, TA_Base_Core::IEntityDataPtr data,
                                             unsigned long actionKey, const std::string& actionName)
    {
        s_pInstance->verifyRights(sessionId, data, actionKey, actionName);
    }
    //
    // UserPrivilegeVerifier
    //
    InternalUserPrivilegeVerifier::InternalUserPrivilegeVerifier()
        : m_rightsLibrary(m_rightsFactory.buildRightsLibrary()),
          m_decisionModule(TA_Base_Bus::UNDEFINED_DM)
    {
        TA_ASSERT(0 != m_rightsLibrary.get(), "");
    }


    //
    // verifyRights
    //
    void InternalUserPrivilegeVerifier::verifyRights(const std::string& sessionId,
                                                     TA_Base_Core::IEntityDataPtr data, unsigned long actionKey,
                                                     const std::string& actionName)
    {
        unsigned long resourceKey = data->getKey();
        // Check the rights.
        try
        {
            std::string rightsDeniedReason;
            if(!m_rightsLibrary->isActionPermittedOnResource(sessionId,
                                                             resourceKey,
                                                             actionKey,
                                                             rightsDeniedReason,
                                                             m_decisionModule))
            {
                LOGMORE(SourceInfo, "rightsDeniedReason: %s, Decision module: %d", rightsDeniedReason.c_str(), m_decisionModule);
                throw TA_Base_Bus::VideoSwitchAgentException("No rights");
            }
        }
        catch(const TA_Base_Core::RightsException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", e.what());
            throw TA_Base_Bus::VideoSwitchAgentException("No rights");
        }
    }

} // TA_IRS_App
