/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubAlarmTypeData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBALARMTYPEDATA_H_
#define STUBALARMTYPEDATA_H_

#include "core/data_access_interface/alarm_rule/src/MMSRuleData.h"
#include "core/data_access_interface/src/IAlarmData.h"

namespace TA_IRS_App_Test
{
    class StubAlarmTypeData : public TA_Base_Core::IAlarmTypeData
    {
        public:
            StubAlarmTypeData(unsigned long key,
                              std::string name,
                              std::string description,
                              unsigned long severityKey,
                              std::string severityName,
                              unsigned long contextKey,
                              std::string contextName,
                              bool isPrinted,
                              bool isSystemAlarm,
                              std::string alarmValue,
                              std::string normalValue,
                              bool isPersisted,
                              bool isVisible,
                              bool isMMSEnabled,
                              TA_Base_Core::IAlarmData::EMmsState mmsstate
                             )
              : m_key(key),
                m_name(name),
                m_description(description),
                m_severityKey(severityKey),
                m_severityName(severityName),
                m_contextKey(contextKey),
                m_contextName(contextName),
                m_isPrinted(isPrinted),
                m_isSystemAlarm(isSystemAlarm),
                m_alarmValue(alarmValue),
                m_normalValue(normalValue),
                m_isPersisted(isPersisted),
                m_isVisible(isVisible),
                m_isMMSEnabled(isMMSEnabled),
                m_MMSstate(mmsstate)
            {
            }
            
            virtual ~StubAlarmTypeData()
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
    
            unsigned long getSeverityKey()
            {
                return m_severityKey;
            }
    

            std::string getSeverityName()
            {
                return m_severityName;
            }
    
            unsigned long getContextKey()
            {
                return m_contextKey;
            }
    
            std::string getContextName()
            {
                return m_contextName;
            }
    
            bool toBePrinted()
            {
                return m_isPrinted;
            }
    
            bool isSystemAlarm()
            {
                return m_isSystemAlarm;
            }
    
            std::string getAlarmValue()
            {
                return m_alarmValue;
            }
            
            std::string getNormalValue()
            {
                return m_normalValue;
            }
            
            bool toBePersisted()
            {
                return m_isPersisted;
            }
    
            bool isVisible()
            {
                return m_isVisible;
            }
    
            time_t getDateCreated()
            {
                return 0;
            }
    
            time_t getDateModified()
            {
                return 0;
            }
    
            bool    isMMSEnabled()
            {
                return m_isMMSEnabled;
            }
    
            TA_Base_Core::MMSRuleData* getRelatedMMSRule()  
            {
                return NULL;
            }
    
            TA_Base_Core::IAlarmData::EMmsState getRelatedMMSType()  
            {
                return m_MMSstate;
            }
            
            void invalidate()
            {
            }
            
            bool isPhysicalAlarm()
            {
                return m_isSystemAlarm;
            }
        private:
            unsigned long m_key;
            std::string m_name;
            std::string m_description;
            unsigned long m_severityKey;
            std::string m_severityName;
            unsigned long m_contextKey;
            std::string m_contextName;
            bool m_isPrinted;
            bool m_isSystemAlarm;
            std::string m_alarmValue;
            std::string m_normalValue;
            bool m_isPersisted;
            bool m_isVisible;
            bool m_isMMSEnabled;
            TA_Base_Core::IAlarmData::EMmsState m_MMSstate;
    };
}
#endif /*STUBALARMTYPEDATA_H_*/
