#pragma once

#include "ace/Singleton.h"
#include "boost/shared_ptr.hpp"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/data_access_interface/bas/src/ModeTypeAccessFactory.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/bas/src/SubsystemModeTypeMappingAccessFactory.h"
#include "core/data_access_interface/bas/src/ModeDeviceAccessFactory.h"

namespace TA_IRS_App
{

	typedef boost::shared_ptr<TA_Base_Core::ILocation> 					ILocation_SmartPtr;
	typedef std::map<unsigned long, TA_IRS_Core::Mode_SmartPtr> 		MapKeyToMode;
	typedef std::map<unsigned long, TA_IRS_Core::SubSystem_SmartPtr> 	MapKeyToSubsystem;
	typedef std::map<unsigned long, TA_IRS_Core::SubSystem_SmartPtr> 	MapOrderToSubsystem;
	typedef std::map<unsigned long, MapKeyToMode> 						MapLocationToKeyMode;
	typedef std::map<unsigned long, MapKeyToSubsystem> 					MapLocationToKeySubsystem;
	typedef std::map<unsigned long, MapOrderToSubsystem> 				MapLocationToOrderedSubsystem;
	typedef std::map<unsigned long, ILocation_SmartPtr> 				MapKeyToLocation;
	
	using namespace TA_IRS_Core;
	
	class BasManagerDataHelper
	{
	public:
		friend ACE_Singleton<BasManagerDataHelper,ACE_Recursive_Thread_Mutex>;
		~BasManagerDataHelper(void);
	public:
		// The location actions
		ILocation_SmartPtr 				getLocalLocation();

		std::vector<ILocation_SmartPtr> getAllLocations();
	    bool 							isAtStation();

		ILocation_SmartPtr 				getLocationByKey(unsigned long locationKey);
	    std::string 					getLocationDispNameByKey( unsigned long locationKey );
		
		// The Modes actions
	    std::vector<Mode_SmartPtr> getModes(unsigned long location, unsigned long subsystem, unsigned long modeType);
		std::vector<Mode_SmartPtr> getModes(unsigned long location, std::vector<unsigned long> subsystems, unsigned long modeType);

		std::vector<Mode_SmartPtr> getModeByModeCodeName(const std::string& modeCodeName);
		std::vector<Mode_SmartPtr> getModeByModeCodeName(const std::string& modeCodeName, unsigned long locationKey);

		Mode_SmartPtr getModeByKey(unsigned long modeKey);
	    Mode_SmartPtr getModeByLocAndKey(unsigned long locationKey, unsigned long modeKey);
		Mode_SmartPtr getModeByValue(unsigned long locationKey, unsigned int modeValue);

		// The Subsystem actions
		MapOrderToSubsystem getAllSubsystems(unsigned long locationKey, bool withSystemConfig = true);
	
		// The system config item for filter
		unsigned long getAllSubsystemItemKey();

		// The Mode types actions
		ModeTypeMap getAllModeTypes();

	    std::string getTypeName( const unsigned long ulTypeKey );

		// The system config item for filter
		unsigned long getAllModeTypeItemKey();

		SubSystemModeTypeMap getSubsystemModeTypeMappingByLocation(unsigned long locationKey);

		TA_IRS_Core::ModeDevices getModeDevicesByModeKey(unsigned long locationKey, unsigned long modeKey);

		unsigned long getModeExecuteTimeOutTime();

		std::string getFireModeRecoverDataPointName();

		SubSystem_SmartPtr getSubsystemByKey(unsigned long locationKey, unsigned long subsystemKey);

		bool getIsManagerInFireMode();
		void setManagerinFireMode(bool bFireMode);

		//Get Bas Manager entity information. cuilike++
		unsigned long getManagerEntityKey();
		std::string getManagerEntityName();
		unsigned long getManagerEntityTypeKey();
		unsigned long getManagerEntitySubsystemKey();
		unsigned long getManagerLocationKey();
	    unsigned long getFireLocationKey();
	    unsigned long getManagerModeValue();
		
	protected:
		class InitializeClass
		{
		public:
			InitializeClass(BasManagerDataHelper& helper);
			~InitializeClass(void);
		private:
			static bool m_isInitialize;
		};
		
		friend InitializeClass;
		void loadLocationsFromDatabase();
		void loadModesFromDatabase();
		void loadModeTypesFromDatabase();
		void loadModeSubsystemFromDatabase();

		//load Bas Manager entity data from DB. cuilike++
		//m_basManagerLocationKey must be set before call this method.
		void loadBasManagerEntityDataFromDatabase();
	private:
		BasManagerDataHelper(void);
	    MapKeyToLocation 				m_mapKeyToLocations;
	    std::vector<ILocation_SmartPtr> m_locations;		// The locations
	    ILocation_SmartPtr 				m_ptrLocalStation;
		MapLocationToKeySubsystem 		m_modeSubSystems;	// The mode subSystems
	    MapLocationToOrderedSubsystem 	m_mapLocToOrderedSubsystems;
	    MapLocationToKeyMode 			m_mapLocToKeyModes;
	    MapKeyToMode 					m_mapKeyToModes;
		ModeTypeMap 					m_modeTypes;					// The mode types
		
		TA_Base_Core::NonReEntrantThreadLockable m_modeLocationLock;	// The location Lock
		TA_Base_Core::NonReEntrantThreadLockable m_modesLock;			// The bas modes Lock
		TA_Base_Core::NonReEntrantThreadLockable m_modeTypesLock;		// The mode types Lock
		TA_Base_Core::NonReEntrantThreadLockable m_modeSubSystemLock;	// The mode subSystems Lock

		LocationSubSystemModeTypeMap 	m_subsystemModeTypeMap; // The mapping between subsystem and mode type

		LocationModeDevices 			m_modeDeviceMap;	// mode device map

		unsigned long 					m_executeModeTimeOut; // the execute mode time out
		std::string 					m_fireModeRecoverDatapointName; // the fire Mode recover data point name , config in db

		unsigned long	m_basManagerLocationKey;
		unsigned long	m_basManagerEntityKey;
		unsigned long	m_basManagerEntityTypeKey;
		std::string		m_basManagerEntityName;
		unsigned long	m_basManagerSubsystemKey;
	
		bool			m_isManagerInFireMode;
	};
	//typedef boost::details::pool::singleton_default<BasManagerDataHelper> BasManagerDataHelperInstance;
	typedef ACE_Singleton<BasManagerDataHelper, ACE_Recursive_Thread_Mutex> BasManagerDataHelperInstance;
}

