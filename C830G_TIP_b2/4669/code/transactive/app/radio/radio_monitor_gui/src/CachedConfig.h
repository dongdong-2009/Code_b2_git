/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_monitor_gui/src/CachedConfig.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Caches local parameter specific to RadioMonitor application
  *
  */

#if !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_

#include <string>
#include <map>
#include <set>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

namespace TA_Base_Bus
{
    class RightsLibrary;
}

namespace TA_IRS_App
{
    interface IRightsChangeListener;

    class CachedConfig  
    : 
    public TA_Base_Core::RunParamUser
    {
    
    public:
        
        CachedConfig();
        virtual ~CachedConfig();

        /*
         * onRunParamChange (interface implementation)
         *
         * @see RunParamUser::onRunParamChange for more details
         *
         */
    	virtual void onRunParamChange(const std::string& name, const std::string& value);
    
        /**
         * getInstance
    	 *
    	 * Returns an instance of the class
         *
         * @return the pointer to this singleton instance
         *
         */
    	static CachedConfig& getInstance();
    
        /**
         * removeInstance
    	 *
    	 * Removes the instance of the class (if already created) and cleanup the members.  Primarily 
         * used upon program termination (e.g. from main()) so that Purify does not consider this class
         * and its members to be leaks.
         *
         */
        static void removeInstance();
    
        void setEntityKey(unsigned long param);
        void setEntityName(const std::string& param);
        void setEntityTypeKey(unsigned long param);
        void setEntityLocationKey(unsigned long param);
        void setEntitySubsystemKey(unsigned long param);
    
        /**
         * setSessionId
         *
         * Sets the session Id, and in doing so sets some internal data related
         *  to the session Id (via database access)
         *     
         */
        void setSessionId(const std::string& sessionId);    
    
        // The standard entity details below should not assert / throw since we may need to raise 
        // alarms with half-complete configuration.
        unsigned long       getEntityKey();
        std::string         getEntityName();
        unsigned long       getEntityTypeKey();
        unsigned long       getEntityLocationKey();
        unsigned long       getEntitySubsystemKey();
        std::string         getSessionId();
    
        void                setPhysicalLocationKey(unsigned long key);
        unsigned long       getPhysicalLocationKey();
    
        RadioMonitor::RadioAgentNamedObject& getAgentObject() { return m_agentObject; }
        RadioMonitor::IRadio& getRadioCorbaDef() { return m_radioAgent; }
        RadioMonitor::IRadioDirectory& getRadioDirectoryCorbaDef() { return m_radioDirectoryAgent; }
        RadioMonitor::IRadioSession& getRadioSessionCorbaDef() { return m_radioSessionAgent; }

        /**
         * flagChangeInRights
         *
         * This is called whenever part of the using application has knowledge
         *  that there has been a change in rights - subsequently
         *  notifications are sent out to all registered rights change listeners
         * 
         * @see registerRightsChangeListener
         *
         */
        void flagChangeInRights();
    
        /**
         * registerRightsChangeListener
         *
         * Registers a listener that is to receive notification of a rights change
         *
         * @pre listener is not already registered
         *
         */
        void registerRightsChangeListener(IRightsChangeListener& listener);
    
        /**
         * deRegisterRightsChangeListener
         *
         * Deregisters a listener that had been registered with registerRightsChangeListener
         *
         * @pre listener has been registered with registerRightsChangeListener
         *
         */
        void deRegisterRightsChangeListener(IRightsChangeListener& listener);

        /**
         * setLocalRadioAgentEntityName
         *
         * Sets the name of radio agent, to enable CORBA comms
         *
         * @param name the name of the radio agent (as extracted from database)
         *
         */
        void setLocalRadioAgentEntityName(const std::string& name);

		void setLocalRadioAgentEntityKey(const unsigned long key);
		unsigned long getLocalRadioAgentEntityKey();

        /**
         * setLocalRadioDirectoryAgentEntityName
         *
         * Sets the name of radio directory agent, to enable CORBA comms
         *
         * @param name the name of the radio directory agent (as extracted from database)
         *
         */
		void setLocalRadioEntityName(const std::string& name);

        void setLocalRadioDirectoryAgentEntityName(const std::string& name);

        /**
         * setLocalRadioSessionAgentEntityName
         *
         * Sets the name of radio session agent, to enable CORBA comms
         *
         * @param name the name of the radio session agent (as extracted from database)
         *
         */
        void setLocalRadioSessionAgentEntityName(const std::string& name);

       /**
         *  isFunctionPermitted
         *
         * @return true if we have the privelege to carry out the specified action
         *
         * @param action defines the action we're checking rights for
         */
        bool isFunctionPermitted(TA_Base_Bus::AccessControlledAction action);

    private:
    
        CachedConfig(const CachedConfig&);
    	CachedConfig& operator=(CachedConfig&);
        
        TA_Base_Core::NonReEntrantThreadLockable m_lockForConfigUpdate;
        TA_Base_Core::NonReEntrantThreadLockable m_listenerLock;       
        
        // global parameters
        TA_Base_Core::PrimitiveWrapper<unsigned long>       m_entityKey;
        TA_Base_Core::PrimitiveWrapper<std::string>         m_entityName;
        TA_Base_Core::PrimitiveWrapper<unsigned long>       m_entityTypeKey;
        TA_Base_Core::PrimitiveWrapper<unsigned long>       m_entityLocationKey;
        TA_Base_Core::PrimitiveWrapper<unsigned long>       m_entitySubsystemKey;
    
        TA_Base_Core::PrimitiveWrapper<unsigned long>       m_physicalLocationKey;
    
        // The Session (or GUI) identifier for this operator terminal
        TA_Base_Core::PrimitiveWrapper<std::string>         m_sessionId;
            
        std::string             m_localRadioAgentEntityName;        
		unsigned long			m_localRadioAgentEntityKey;
        RadioMonitor::RadioAgentNamedObject   m_agentObject;    
        std::string             m_localRadioDirectoryAgentEntityName;
        RadioMonitor::IRadioDirectory         m_radioDirectoryAgent;
		std::string             m_localRadioEntityName;
        RadioMonitor::IRadio         m_radioAgent;
        std::string             m_localRadioSessionAgentEntityName;
        RadioMonitor::IRadioSession           m_radioSessionAgent;

        std::set<IRightsChangeListener*> m_rightsChangeListeners;
        TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	
    };
               
} // end namespace TA_IRS_App
    
#endif // !defined(AFX_CachedConfig_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
    
