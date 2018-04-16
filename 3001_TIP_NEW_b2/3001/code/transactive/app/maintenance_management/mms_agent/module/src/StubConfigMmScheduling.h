/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubConfigMmScheduling.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBCONFIGMMSCHEDULING_H_
#define STUBCONFIGMMSCHEDULING_H_

#include "core/data_access_interface/mms_dai/src/IConfigMmScheduling.h"

namespace TA_IRS_App_Test
{
    class StubConfigMmScheduling : public TA_IRS_Core::IConfigMmScheduling
    {
        public:
            StubConfigMmScheduling()
            :m_subsystemName("TestSubsystem"),
             m_subsystemKey(1),
             m_name("name"),
             m_key(1)
            {
            }
            
            virtual ~StubConfigMmScheduling()
            {
            }
            
            //IConfigMmScheduling
            bool hasChanged()
            {
                return false;
            }
            
            void applyChanges()
            {
                if ( CppTest_IsCurrentTestCase ("test_recordSubsystemsSent_UnknownException") ||
                     CppTest_IsCurrentTestCase ("test_updateObsoleteLastSent_UnknownException") 
                   )
                {
                    throw std::exception("unknown exception thrown in StubConfigMmScheduling::applyChanges");
                }
            }
            
            void setScheduledTime(time_t newTime)
            {
            }
            
            void setLastReportedTime(time_t newTime)
            {
            }
             
            //IMmScheduleData
            std::string getSubsystemName()
            {
                return m_subsystemName;
            }
            
            unsigned long getSubsystemKey()
            {
                return m_subsystemKey;
            }
            
            time_t getLastReported() 
            {
                if ( CppTest_IsCurrentTestCase ("test_subsystemIsDue_ReturnFalse_2") )
                {
                    return 1;
                }
                
                return 0;
            }
            
            time_t getScheduledTime()
            {
                if ( CppTest_IsCurrentTestCase ("test_subsystemIsDue_ReturnFalse_2") )
                {
                    return 100;
                }
                
                return 0;
            }
            
            // IItem methods:
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
            
        public:
            std::string m_subsystemName;
            unsigned long m_subsystemKey;
            std::string m_name;
            unsigned long m_key;
            
    };
}
#endif /*STUBCONFIGMMSCHEDULING_H_*/
