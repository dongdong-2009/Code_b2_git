/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubMmsConnectiontEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBMMSCONNECTIONTENTITYDATA_H_
#define STUBMMSCONNECTIONTENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/MmsConnectiontEntityData.h"

namespace TA_IRS_App_Test
{
    class StubMmsConnectiontEntityData : public TA_Base_Core::MmsConnectiontEntityData
    {
        public:
            StubMmsConnectiontEntityData()
            {
            }
            
            virtual ~StubMmsConnectiontEntityData()
            {
            }
            
            unsigned long getKey()
            {
                std::cout << "StubMmsConnectiontEntityData::getKey" <<  std::endl;
                return 1;
            }
            
            std::string getName()
            {
                std::cout << "StubMmsConnectiontEntityData::getName" <<  std::endl;
                return "OccMmsAgent";
            }
            
            void invalidate()
            {
                std::cout << "StubMmsConnectiontEntityData::invalidate" <<  std::endl;
            }        
    };
}
#endif /*STUBMMSCONNECTIONTENTITYDATA_H_*/
