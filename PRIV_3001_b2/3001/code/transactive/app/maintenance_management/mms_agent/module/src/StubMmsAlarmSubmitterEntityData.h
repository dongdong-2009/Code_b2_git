/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubMmsAlarmSubmitterEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBMMSALARMSUBMITTERENTITYDATA_H_
#define STUBMMSALARMSUBMITTERENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/MmsAlarmSubmitterEntityData.h"

namespace TA_IRS_App_Test
{
    class StubMmsAlarmSubmitterEntityData : public TA_Base_Core::MmsAlarmSubmitterEntityData
    {
        public :
            StubMmsAlarmSubmitterEntityData()
            {
            }
            
            virtual ~StubMmsAlarmSubmitterEntityData()
            {
            }

            unsigned long getKey()
            {
                std::cout << "StubMmsAlarmSubmitterEntityData::getKey" <<  std::endl;
                return 1;
            }
            
            std::string getName()
            {
                std::cout << "StubMmsAlarmSubmitterEntityData::getName" <<  std::endl;
                return "OccMmsAgent";
            }
            
    };
}
#endif /*STUBMMSALARMSUBMITTERENTITYDATA_H_*/
