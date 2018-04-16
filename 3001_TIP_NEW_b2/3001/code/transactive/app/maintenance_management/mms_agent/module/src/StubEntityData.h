/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_ModuleTest/CODE_4669_T00271301/transactive/app/maintenance_management/mms_agent/test/module/src/StubEntityData.h $
  * @author:  Grace Koh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2007/10/25 11:49:33 $
  * Last modified by:  $Author: grace.koh $
  * 
  */
#ifndef STUBENTITYDATA_H_
#define STUBENTITYDATA_H_

#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_IRS_App_Test
{
    class StubEntityData : public TA_Base_Core::IEntityData
    {
        public:
            StubEntityData()
                    :   m_type(""), 
                        m_typeKey(0),
                        m_address(""),
                        m_desc(""),
                        m_subSystem(0),
                        m_subSystemName(""),
                        m_plysicalSubSystem(0),
                        m_plysicalSubSystemName(""),
                        m_location(0),
                        m_locationName(""),
                        m_region(0),
                        m_regionName(""),
                        m_parent(0),
                        m_parentName(""),
                        m_agent(0),
                        m_agentName(""),
                        m_dateCreated(0),
                        m_dataModified(0),
                        m_alarmDisplay(""),
                        m_key(0),
                        m_name("")
            {
            }
            
            virtual ~StubEntityData()
            {
            }
            
            virtual std::string getType()
            {
                return m_type;
            }
            
            virtual unsigned long getTypeKey()
            {
                return m_typeKey;
            }
            
            virtual std::string getAddress()
            {
                return m_address;
            }
            
            virtual std::string getDescription()
            {
                return m_desc;
            }
            
            virtual unsigned long getSubsystem()
            {
                return m_subSystem;
            }
            
            virtual std::string getSubsystemName() 
            {
                return m_subSystemName;
            }
            
            virtual unsigned long getPhysicalSubsystem()
            {
                return m_plysicalSubSystem;
            }
            
            virtual std::string getPhysicalSubsystemName()
            {
                return m_plysicalSubSystemName;
            }
            
            virtual unsigned long getLocation()
            {
                return m_location;
            }
            
            virtual std::string getLocationName()
            {
                return m_locationName;
            }
            
            virtual unsigned long getRegion()
            {
                return m_region;
            }
            
            virtual std::string getRegionName()
            {
                return m_regionName;
            }
            
            virtual unsigned long getParent()
            {
                return m_parent;
            }
                
            virtual std::string getParentName()
            {
                return m_parentName;
            }
            
            virtual unsigned long getAgent()
            {
                return m_agent;
            }
            
            virtual std::string getAgentName()
            {
                return m_agentName;
            }
            
            virtual time_t getDateCreated()
            {
                return m_dateCreated;
            }
            
            virtual time_t getDateModified()
            {
                return m_dataModified;
            }
            
            virtual std::string getAlarmDisplay()
            {
                return m_alarmDisplay;
            }
            
            virtual TA_Base_Core::EntityStatusData getEntityStatusData(TA_Base_Core::EEntityStatusType statustype)
            {
                return m_entityStatusData;
            }
            
            virtual TA_Base_Core::EntityStatusData getEntityStatusData(unsigned int statustype)
            {
                return m_entityStatusData;
            }
            
            virtual bool getBoolEntityStatusValue(TA_Base_Core::EEntityStatusType statustype)
            {
                return true;
            }
            
            virtual bool getBoolEntityStatusValue(unsigned int statustype) 
            {
                return true;
            }
            
            //from IItem
            virtual unsigned long getKey() 
            {
                return m_key;
            }
            
            virtual std::string getName()
            {
                return m_name;
            }
            
            virtual void invalidate()
            {
            }
            
        public:
            std::string m_type;
            unsigned long m_typeKey;
            std::string m_address;
            std::string m_desc;
            unsigned long m_subSystem;
            std::string m_subSystemName;
            unsigned long m_plysicalSubSystem;
            std::string m_plysicalSubSystemName;
            unsigned long m_location;
            std::string m_locationName;
            unsigned long m_region;
            std::string m_regionName;
            unsigned long m_parent;
            std::string m_parentName;
            unsigned long m_agent;
            std::string m_agentName;
            time_t m_dateCreated;
            time_t m_dataModified;
            std::string m_alarmDisplay;
            TA_Base_Core::EntityStatusData m_entityStatusData;
            unsigned long m_key;
            std::string m_name;        
    };
}
#endif /*STUBENTITYDATA_H_*/
