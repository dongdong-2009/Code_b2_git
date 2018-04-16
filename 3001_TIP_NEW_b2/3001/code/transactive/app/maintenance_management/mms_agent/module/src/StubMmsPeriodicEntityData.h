/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubMmsPeriodicEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBMMSPERIODICENTITYDATA_H_
#define STUBMMSPERIODICENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"

namespace TA_IRS_App_Test
{
    class StubMmsPeriodicEntityData : public TA_Base_Core::MmsPeriodicEntityData
    {
        public:
            StubMmsPeriodicEntityData()
            {
            }
            
            virtual ~StubMmsPeriodicEntityData()
            {
            }
            
            unsigned long getKey()
            {
                std::cout << "StubMmsPeriodicEntityData::getKey" <<  std::endl;
                return 1;
            }
            
            std::string getName()
            {
                std::cout << "StubMmsPeriodicEntityData::getName" <<  std::endl;
                return "OccMmsAgent";
            }
            
            void invalidate()
            {
            }        
    };
}

#endif /*STUBMMSPERIODICENTITYDATA_H_*/
