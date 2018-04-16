/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubDataPointEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBDATAPOINTENTITYDATA_H_
#define STUBDATAPOINTENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

namespace TA_IRS_App_Test
{
    class StubDataPointEntityData : public TA_Base_Core::DataPointEntityData
    {
        public:
            StubDataPointEntityData()
            : DataPointEntityData(0),
              m_GlobalMMSInhibitFlag(false),
              m_MMSServerStatusFlag(false)
            {
            }
            
            virtual ~StubDataPointEntityData()
            {
            }
            
            unsigned long getKey()
            {
                std::cout << "StubDataPointEntityData::getKey" <<  std::endl;
                return 1;
            }
            
            std::string getName()
            {
                std::cout << "StubDataPointEntityData::getName" <<  std::endl;
                return "OccMmsAgent";
            }
            
            std::string getDescription()
            {
                if (m_GlobalMMSInhibitFlag)
                {
                    return "globalmmsinhibit";
                }
                
                if (m_MMSServerStatusFlag)
                {
                    return "mmsserverstatus";
                }
                
                return "other";
            }
            
            unsigned long getAgent()
            {
                std::cout << "StubDataPointEntityData::getAgent" <<  std::endl;
                return 1;
            }
            
            std::string getAgentName()
            {
                std::cout << "StubDataPointEntityData::getAgentName" <<  std::endl;
                return "OccMmsAgent";
            }
            
            unsigned long getTypeKey()
            {
                std::cout << "StubDataPointEntityData::getTypeKey" <<  std::endl;
                return 1;
            }
       
            void setGlobalMMSInhibitFlag(bool flag)
            {
                m_GlobalMMSInhibitFlag = flag;
            }
            
            void setMmsServerStatusFlag(bool flag)
            {
                m_MMSServerStatusFlag = flag;
            }
        private:
            bool m_GlobalMMSInhibitFlag;
            bool m_MMSServerStatusFlag;       
    };
}
#endif /*STUBDATAPOINTENTITYDATA_H_*/
