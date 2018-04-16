/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubLocation.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef LOCATIONSTUB_H_
#define LOCATIONSTUB_H_

#include "core/data_access_interface/src/ILocation.h"

namespace TA_IRS_App_Test
{
    class StubLocation : public TA_Base_Core::ILocation
    {
        public:
            StubLocation(unsigned long key, 
                         std::string description, 
                         unsigned long orderId, 
                         std::string name,
                         TA_Base_Core::ILocation::ELocationType locationType)
            : m_key(key),
              m_description(description),
              m_orderId(orderId),
              m_name(name),
              m_locationType(locationType)
            {
            }
            
            virtual ~StubLocation()
            {
            }
            
            std::string getDescription()
            {
                return m_description;
            }
            
            unsigned long getOrderId()
            {
                return m_orderId;
            }
            
            std::string getDefaultDisplay(unsigned long profileKey, int displayNumber)
            {
                return "Default Display";
            }
            
            TA_Base_Core::ILocation::ELocationType getLocationType()
            {
                return m_locationType;
            }
            
            bool isProfileAssociated(unsigned long profileId)
            {
                return false;
            }
            
            unsigned long getKey()
            {
                return m_key;
            }
            
            std::string getName()
            {
                return m_name;
            }
            
            void invalidate()
            {
            }
            bool isDisplayOnly()
            {
                return true;
            }
        private:
            unsigned long m_key;
            std::string m_description;
            unsigned long m_orderId;
            std::string m_name;
            TA_Base_Core::ILocation::ELocationType m_locationType;
        
    };
}
#endif /*LOCATIONSTUB_H_*/
