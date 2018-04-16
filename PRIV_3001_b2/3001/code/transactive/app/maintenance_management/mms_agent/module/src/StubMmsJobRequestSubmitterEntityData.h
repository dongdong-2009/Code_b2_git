/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubMmsJobRequestSubmitterEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBMMSJOBREQUESTSUBMITTERENTITYDATA_H_
#define STUBMMSJOBREQUESTSUBMITTERENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/MmsJobRequestSubmitterEntityData.h"

namespace TA_IRS_App_Test
{
    class StubMmsJobRequestSubmitterEntityData : public TA_Base_Core::MmsJobRequestSubmitterEntityData
    {
        public:
            StubMmsJobRequestSubmitterEntityData()
            {
            }
            
            virtual ~StubMmsJobRequestSubmitterEntityData()
            {
            }
            
            unsigned long getKey()
            {
                std::cout << "StubMmsJobRequestSubmitterEntityData::getKey" <<  std::endl;
                return 1;
            }
            
            std::string getName()
            {
                std::cout << "StubMmsJobRequestSubmitterEntityData::getName" <<  std::endl;
                return "OccMmsAgent";
            }
            
    };
}
#endif /*STUBMMSJOBREQUESTSUBMITTERENTITYDATA_H_*/
