/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/shared_alarm/src/SubsytemDataCache.h$
  * @author:  Lin Weikun
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/03/01 12:12:40 $
  * Last modified by:  $Author: grace.koh $
  *
  * SubsystemDataCache is a singleton  that get subsystem from subsystem table
  */
#if !defined(ALARM_APP_SUBSYSTEMCACHE_H_H_H)
#define ALARM_APP_SUBSYSTEMCACHE_H_H_H

#include <vector>
#include <list>
#include <atlcomcli.h>

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/Subsystem.h"

//#import "../../../../../cots/IATSAlarmEvent/bin/ATSBridgeUtil.dll" named_guids
//using namespace ATSBridgeUtilLib;

#include "core/data_access_interface/src/SystemData.h"
#include "core/data_access_interface/src/SystemAccessFactory.h"


const unsigned long ATS_SYSTEM_SUBSYSTEM_KEY=99999999;

namespace TA_Base_App
{
	// A ListItem is used to map a PKEY to a description string.
	// They are used for data from the locations, subsystems, alarmTypes and operators tables.

	struct ListItem
	{
		ListItem(unsigned long k, std::string s)
		{
			pkey = k;
			str = s.c_str();
			char ch[100]={0};
			sprintf(ch,"%u",pkey);
			keyType = ch;
		}

		ListItem(unsigned long k, std::string s, std::string strKey)
		{
			pkey = k;
			str = s.c_str();
			keyType = strKey.c_str();
		}
		ListItem(const ListItem & listItem)
		{
			pkey = listItem.pkey;
			str = listItem.str.c_str();
			keyType = listItem.keyType.c_str();
		}
		bool operator<(const ListItem& rhs)
		{
			return _stricmp(str.c_str(), rhs.str.c_str()) < 0; //str.compare(rhs.str) < 0;
		}

		std::string  keyType;
		unsigned long	pkey;
		std::string		str;
	};

	// Use a list so we can sort it
	typedef std::list<ListItem> ListItems;

	/// The ATSAlarmSystem structure describes an ATS system that can be associated with an ATS alarm
	typedef struct ATSAlarmSystem
	{
		/// The system name. Max 10 chars.
		std::string Name;

		/// The names of the system's subsystems.  Max 10 chars (each).
		std::list<std::string> SubsystemNames;

	}ATSAlarmSystem;


	typedef struct ATSAlarmSubsystem
	{
		/// The system name. Max 10 chars.
		std::string Name;
		unsigned long subsystemKey;
		ATSAlarmSubsystem(std::string name,unsigned long subsystem)
		{
			Name = name.c_str();
			subsystemKey=subsystem;
		}
	}ATSAlarmSubsystem;

	/// The ATSAlarmSystem structure describes an ATS system that can be associated with an ATS alarm
	typedef struct ATSAlarmSystemType
	{
		/// The system name. Max 10 chars.
		std::string Name;
		unsigned long systemKey;

		/// The names of the system's subsystems.  Max 10 chars (each).
		std::list<ATSAlarmSubsystem> SubsystemNames;

	}ATSAlarmSystemType;



	class ATSSubsystemDataCache
	{
	public:
		static unsigned long atsKey;
		ATSSubsystemDataCache();
		void getSubsystemNamesFormSystem(const std::string Name, TA_Base_App::ListItems &atsSubsystemList);
		void getAllSystems(TA_Base_App::ListItems &systemList);
		void getAllSubsystems(TA_Base_App::ListItems &subsystemList);
	private:
		std::list<ATSAlarmSystemType> m_alarmSystems;

	};
	



	class SubsystemDataCache
	{
	public:
        virtual ~SubsystemDataCache();

		/**
		 * getInstance
		 *
		 * Creates and returns an instance of this object.
		 *
		 * @return A reference to an instance of an SubsystemDataCache object.
		 */
		static SubsystemDataCache& getInstance();
		TA_Base_App::ListItems getSubsystemsFormSystems(ListItems systemKeys);
		unsigned long getSystemFormSubsystem(unsigned long subsystemKey);
		void InitializeSystem();
		void InitializeSubsystem();
		void getAllSystems(TA_Base_App::ListItems &systemList);

	private:
		std::vector<TA_Base_Core::SystemData*> m_allSystemData;
		std::vector<TA_Base_Core::ISubsystem*> m_subsystems;
		//
        // These are private as this method is a singleton
        //
		SubsystemDataCache(){};

		static SubsystemDataCache* m_instance;
		/**
		* protect singleton creation
		*/
		static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;
		ATSSubsystemDataCache m_atsSubsystemDataCache;
	};

}


#endif 