/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubSubsystem.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBSUBSYSTEM_H_
#define STUBSUBSYSTEM_H_

#include "core/data_access_interface/src/ISubsystem.h"

namespace TA_IRS_App_Test
{
    class StubSubsystem : public TA_Base_Core::ISubsystem
    {
        public :
            StubSubsystem(unsigned long key, 
                          bool physical, 
                          bool exclusive,
                          std::string name)
            : m_key(key),
              m_physical(physical),
              m_exclusive(exclusive),
              m_name(name)                          
            {
            }
            
            virtual ~StubSubsystem()
            {
            }
            
            bool isPhysical()
            {
                return m_physical;
            }
            
            bool isExclusive()
            {
                return m_exclusive;
            }
            
            time_t getDateCreated()
            {
                return 0;
            }
            
            time_t getDateModified()
            {
                return 0;
            }
            
            unsigned long getKey()
            {
                return m_key;
            }
            
            std::string getName()
            {
                return m_name;
            }
            
            virtual unsigned long getSystemKey()
            {
                return m_key;
            }
            void invalidate()
            {
            }
                
        private:
            unsigned long m_key;
            std::string m_name;
            bool m_physical;
            bool m_exclusive;
    };
}
#endif /*STUBSUBSYSTEM_H_*/
