/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/mms_scheduling/src/CachedConfig.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Caches local parameters specific to MMSController application
  *
  */

#pragma once

#include <string>
#include <map>
#include <set>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
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
    
        /**
         * setSessionId
         *
         * Sets the session Id, and in doing so sets some internal data related
         *  to the session Id (via database access)
         *     
         */
        void setSessionId(const std::string& sessionId);    
        std::string         getSessionId();
        
        void setEntityKey(unsigned long param);
        
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
        
        TA_Base_Core::NonReEntrantThreadLockable m_listenerLock;    
        TA_Base_Core::ReEntrantThreadLockable m_lockForConfigUpdate;   
        
        TA_Base_Core::PrimitiveWrapper<unsigned long>    m_entityKey;
        // The Session (or GUI) identifier for this operator terminal
        TA_Base_Core::PrimitiveWrapper<std::string>         m_sessionId;
            
        std::set<IRightsChangeListener*> m_rightsChangeListeners;
        TA_Base_Bus::RightsLibrary*          m_rightsLibrary;	
    };
               
} // end namespace TA_IRS_App
    
