/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/CachedConfig.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Caches local parameters specific to MMSController application
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include <string>
#include <vector>

#include "StdAfx.h"
#include "CachedConfig.h"
#include "IRightsChangeListener.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#include "bus/security/rights_library/src/RightsLibrary.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core; using namespace TA_IRS_Core;

namespace TA_IRS_App
{
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    CachedConfig::CachedConfig()
    : m_rightsLibrary(0)
    {
        try
        {
            // Have us listen out for changes to session ID
            TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

    	    TA_Base_Bus::RightsLibraryFactory   rlf;
            // TA_Bus::ALWAYS_TRUE_RL
            m_rightsLibrary = rlf.buildRightsLibrary();
	        TA_ASSERT(m_rightsLibrary != NULL,"Rights library not created.");
        }
        catch (...)
        {
        }
    }


    CachedConfig::~CachedConfig()
    {
    }


    CachedConfig& CachedConfig::getInstance()
    {      
        static CachedConfig instance;
        return instance;
    }


    void CachedConfig::removeInstance( )
    {
	    // Nothing to do
    }


    void CachedConfig::setSessionId(const std::string& sessionId)
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        m_sessionId.setValue(sessionId);
    }


    void CachedConfig::setEntityKey(unsigned long param)
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        m_entityKey.setValue(param);
    }


    void CachedConfig::onRunParamChange(const std::string& name, const std::string& value)
    {        
        if (RPARAM_SESSIONID == name)
        {
            // The session ID has changed, so must propogate associated rights changes throughout GUI
            setSessionId(value);
            
            CachedConfig::getInstance().flagChangeInRights();
        }
    }


    std::string CachedConfig::getSessionId()
    {
        ThreadGuard guard( m_lockForConfigUpdate );
        return m_sessionId.getValue();
    }


    void CachedConfig::registerRightsChangeListener(IRightsChangeListener& listener)
    {
        TA_Base_Core::ThreadGuard sync(m_listenerLock);
    
        if (!m_rightsChangeListeners.insert(&listener).second)
        {
            TA_ASSERT(FALSE, "Listener already registered");
        }
    }
    
    
    void CachedConfig::deRegisterRightsChangeListener(IRightsChangeListener& listener) 
    {
        TA_Base_Core::ThreadGuard sync(m_listenerLock);
    
        std::set<IRightsChangeListener*>::iterator itr = m_rightsChangeListeners.find(&listener);
    
        if (itr == m_rightsChangeListeners.end())
        {
            TA_ASSERT(FALSE, "Could not find listener to deRegister");
        }
        else
        {
            m_rightsChangeListeners.erase(itr);
        }
    }
    
    
    void CachedConfig::flagChangeInRights()
    {
        TA_Base_Core::ThreadGuard sync(m_listenerLock);
    
        for (std::set<IRightsChangeListener*>::iterator itr = m_rightsChangeListeners.begin();
                itr != m_rightsChangeListeners.end();
                itr ++)
        {
            (*itr)->onRightsChanged();
        }
    }
    

    bool CachedConfig::isFunctionPermitted(TA_Base_Bus::AccessControlledAction action)
    {    
        bool        isPermitted = false;
        static bool haveWarnedUser = false;
        std::string reason;
                
        if (getSessionId().size() == 0 || 
            (stricmp(getSessionId().c_str(), "debug") == 0))
        {
            return true;
        }

        try
        {
            if (0 != m_rightsLibrary && m_entityKey.hasBeenSet())
            {
                TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
                std::string sessionId = getSessionId();
        
                std::auto_ptr<TA_Base_Core::IResource> resource
                    (TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_entityKey.getValue()));
    
                unsigned long resourceID = resource->getKey();
        
                isPermitted =  m_rightsLibrary->isActionPermittedOnResource(sessionId, 
        													          resourceID, 
        													          action, 
        													          reason, 														 
        													          decisionModule);
                haveWarnedUser = false;  // Working again now!!
            }
        }
        catch (...)
        {
            // Error checking rights
            // Log the problem
            std::string messageText = "Unable to check access rights, reason: " + reason;
            LOG_EXCEPTION_CATCH(SourceInfo, "isFunctionPermitted Exception",reason.c_str());
    
            // Let the user know (but only the first time)
            if (!haveWarnedUser)
            {
    	        //AfxMessageBox(messageText.c_str());
    	        haveWarnedUser = true;
            }
        }
    
        return isPermitted;
    }

} // end namespace TA_IRS_App
