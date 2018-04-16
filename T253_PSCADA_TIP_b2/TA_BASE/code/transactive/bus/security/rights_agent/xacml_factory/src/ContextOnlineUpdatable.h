#ifndef CONTEXTONLINEUPDATABLE_H_INCLUDED
#define CONTEXTONLINEUPDATABLE_H_INCLUDED

/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/security/rights_agent/xacml_factory/src/ContextOnlineUpdatable.h $
 * @author:   Nick Jardine
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 *
 * Receives online updates for the rights context factory.
 *
 */

namespace TA_Base_Bus
{
    class ContextOnlineUpdatable : public TA_Base_Core::IOnlineUpdatable
    {
    public:
		
		ContextOnlineUpdatable( RightsType::ProfileMap& profileMap,
			ACE_RW_Mutex& profileLock, RightsType::ActionMap& actionMap,
			ACE_RW_Mutex& actionLock, RightsType::ResourceMap& resourceMap,
			ACE_RW_Mutex& resourceLock, RightsType::SubsystemMap& subsystemMap,
			ACE_RW_Mutex& subsystemLock, RightsType::ActionGroupMap& actionGroupMap,
			ACE_RW_Mutex& dutyLock );

		~ContextOnlineUpdatable();

		// Staisfies the IOnlineUpdatable interface
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    private:

        ContextOnlineUpdatable(const ContextOnlineUpdatable& );
        ContextOnlineUpdatable& operator=(const ContextOnlineUpdatable& );

		RightsType::ProfileMap& m_profileMap;
		ACE_RW_Mutex& m_profileLock;

		RightsType::ActionMap& m_actionMap;
		ACE_RW_Mutex& m_actionLock;

		RightsType::ResourceMap& m_resourceMap;
		ACE_RW_Mutex& m_resourceLock;

		RightsType::SubsystemMap& m_subsystemMap;
		ACE_RW_Mutex& m_subsystemLock;

		RightsType::ActionGroupMap& m_actionGroupMap;
		ACE_RW_Mutex& m_dutyLock;
    };
}

#endif // CONTEXTONLINEUPDATABLE_H_INCLUDED
