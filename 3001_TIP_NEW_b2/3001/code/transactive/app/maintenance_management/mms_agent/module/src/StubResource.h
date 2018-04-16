/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubResource.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBRESOURCE_H_
#define STUBRESOURCE_H_

#include "core/data_access_interface/src/IResource.h"

namespace TA_IRS_App_Test
{
    class StubResource : public TA_Base_Core::IResource
    {
        public:
            StubResource()
            : m_key(1),
              m_subsystem(1)
            {
            }
            
            StubResource(unsigned long key, unsigned long subsystem)
            : m_key(key),
              m_subsystem(subsystem)
            {
            }
            
            virtual ~StubResource()
            {
            }
            
            unsigned long getKey()
            {
                return m_key;
            }
            
            unsigned long getSubsystem()
            {
                return m_subsystem;
            }
            
        private:
            unsigned long m_key;
            unsigned long m_subsystem;
    };
}
#endif /*STUBRESOURCE_H_*/
