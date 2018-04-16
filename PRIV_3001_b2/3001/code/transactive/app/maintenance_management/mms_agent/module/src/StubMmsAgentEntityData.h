/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubMmsAgentEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBMMSAGENTENTITYDATA_H_
#define STUBMMSAGENTENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"

namespace TA_IRS_App_Test
{
    class StubMmsAgentEntityData : public TA_Base_Core::MmsAgentEntityData
    {
        public:
            StubMmsAgentEntityData()
            : MmsAgentEntityData(0),
              m_key(0),
              m_name("TestAgent"),
              m_agentName("TestAgent"),
              m_serverAddress("175.124.10.50")
            {
            }
            
            virtual ~StubMmsAgentEntityData()
            {
            }
            
            std::string getAgentName()
            {
                return m_agentName;
            }
            
            unsigned long getKey()
            {
                return m_key;
            }
            
            std::string getServerAddress()
            {
                return m_serverAddress;
            }
        public:
            unsigned long m_key;
            std::string m_name;
            std::string m_agentName;  
            std::string m_serverAddress;
    };
}
#endif /*STUBMMSAGENTENTITYDATA_H_*/
