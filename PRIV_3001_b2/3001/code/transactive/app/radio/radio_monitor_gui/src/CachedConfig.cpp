/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/CachedConfig.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Caches local parameter specific to RadioMonitor application
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4284)
#endif // defined _MSC_VER

#include <string>
#include <vector>

#include "app/radio/radio_monitor_gui/src/StdAfx.h"
#include "app/radio/radio_monitor_gui/src/CachedConfig.h"
#include "app/radio/radio_monitor_gui/src/IRightsChangeListener.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorErrorHandler.h"

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

namespace TA_IRS_App
{
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    CachedConfig::CachedConfig()
    {
        // Have us listen out for changes to session ID
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

    	TA_Base_Bus::RightsLibraryFactory   rlf;
        // TA_Bus::ALWAYS_TRUE_RL
        m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
	    TA_ASSERT(m_rightsLibrary != NULL,"Rights library not created.");
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


    void CachedConfig::setEntityKey(unsigned long param)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_entityKey.setValue(param);
    }


    void CachedConfig::setEntityName(const std::string& param)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_entityName.setValue(param);
    }


    void CachedConfig::setEntityTypeKey(unsigned long param)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_entityTypeKey.setValue(param);
    }


    void CachedConfig::setSessionId(const std::string& sessionId)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_sessionId.setValue(sessionId);
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


    void CachedConfig::setEntityLocationKey(unsigned long param)
    {
        if (param == 0 || param == -1)
        {
            TA_THROW( TA_Base_Core::TransactiveException("Invalid LocationKey set for Radio Monitor") );
        }

        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_entityLocationKey.setValue(param);
    }

    void CachedConfig::setEntitySubsystemKey(unsigned long param)
    {
        if ( param == -1)
        {
            TA_THROW( TA_Base_Core::TransactiveException("Invalid SubsystemKey set for Radio Monitor") );
        }

        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_entitySubsystemKey.setValue(param);
    }


    void CachedConfig::setPhysicalLocationKey(unsigned long key)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_physicalLocationKey.setValue(key);
    }


    void CachedConfig::setLocalRadioAgentEntityName(const std::string& name)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_localRadioAgentEntityName = name;
    
        m_agentObject.setEntityName(m_localRadioAgentEntityName,false);
    }

	void CachedConfig::setLocalRadioAgentEntityKey(unsigned long key)
	{
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_localRadioAgentEntityKey = key;
	}

	unsigned long CachedConfig::getLocalRadioAgentEntityKey()
	{
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
		return m_localRadioAgentEntityKey;
	}

    void CachedConfig::setLocalRadioSessionAgentEntityName(const std::string& name)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_localRadioSessionAgentEntityName = name;
    
        m_radioSessionAgent.setEntityName(m_localRadioSessionAgentEntityName,false);
    }


    void CachedConfig::setLocalRadioDirectoryAgentEntityName(const std::string& name)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_localRadioDirectoryAgentEntityName = name;
    
        m_radioDirectoryAgent.setEntityName(m_localRadioDirectoryAgentEntityName,false);
    }

	void CachedConfig::setLocalRadioEntityName(const std::string& name)
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        m_localRadioEntityName = name;
    
        m_radioAgent.setEntityName(m_localRadioEntityName,false);
    }


    unsigned long CachedConfig::getPhysicalLocationKey()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_physicalLocationKey.getValue();
    }


    unsigned long CachedConfig::getEntityKey()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_entityKey.getValue();
    }

    std::string CachedConfig::getEntityName()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_entityName.getValue();
    }

    unsigned long CachedConfig::getEntityTypeKey()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_entityTypeKey.getValue();
    }

    unsigned long CachedConfig::getEntityLocationKey()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_entityLocationKey.getValue();
    }

    unsigned long CachedConfig::getEntitySubsystemKey()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
        return m_entitySubsystemKey.getValue();
    }

    std::string CachedConfig::getSessionId()
    {
        TA_Base_Core::ThreadGuard guard( m_lockForConfigUpdate );
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
                
        try
        {
            TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
            std::string sessionId = getSessionId();
    
            TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().
                                                getResourceFromEntity(m_entityKey.getValue());
            unsigned long resourceID = resource->getKey();
            delete resource;
    
//            isPermitted =  m_rightsLibrary->isActionPermittedOnResource(sessionId, 
//    													          resourceID, 
//    													          action, 
//    													          reason, 														 
//    													          decisionModule);
			isPermitted = m_rightsLibrary->isActionPermittedOnResourceAndLocSub(sessionId,
																resourceID,
																m_entityLocationKey.getValue(),
																m_entitySubsystemKey.getValue(),
																action,
																reason,
																decisionModule);


            haveWarnedUser = false;  // Working again now!!
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
