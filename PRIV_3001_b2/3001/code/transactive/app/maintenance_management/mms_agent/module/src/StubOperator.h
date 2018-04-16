/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubOperator.h $
  * @author:  Grace Koh
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2007/10/25 13:24:07 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBOPERATOR_H_
#define STUBOPERATOR_H_

#include "core/data_access_interface/src/IOperator.h"

namespace TA_IRS_App_Test
{
    class StubOperator : public TA_Base_Core::IOperator
    {
        public :
            StubOperator(unsigned long key, 
                         std::string name,
                         std::string description,
                         std::string password, 
                         bool systemOperator)
            : m_key(key),
              m_description(description),
              m_name(name),
              m_password(password),
              m_systemOperator(systemOperator)
            {
            }
            
            virtual ~StubOperator()
            {
            }
            
            unsigned long getKey()
            {
                return m_key;
            }
            
            std::string getName()
            {
                return m_name;
            }
            
            std::string getDescription()
            {
                return m_description;
            }
            
            std::string getPassword()
            {
                return m_password;
            }
            
            void setPassword(const std::string password)
            {
                m_password = password;
            }
            
            bool isSystemOperator()
            {
                return m_systemOperator;
            }
            
            std::vector<TA_Base_Core::IProfile*> getAssociatedProfiles()
            {
                std::vector<TA_Base_Core::IProfile*> empty;
                
                return empty;    
            }
            
            std::vector<unsigned long> getAssociatedProfileKeys()
            {
                std::vector<unsigned long> empty;
                return empty;   
            }
            
            std::vector<unsigned long> getAssociatedRegionKeys()
            {
                std::vector<unsigned long> empty;
                return empty;   
            }
            
            void invalidate()
            {
            }
            
        private:
            unsigned long m_key;
            std::string m_description;
            std::string m_name;
            std::string m_password;
            bool m_systemOperator;
    };
}
#endif /*STUBOPERATOR_H_*/
