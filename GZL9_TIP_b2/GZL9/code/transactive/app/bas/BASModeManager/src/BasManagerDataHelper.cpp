#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/bas/src/Subsystem.h"
#include "core/utilities/src/DataConversion.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include <sstream>

namespace TA_IRS_App
{


	#define InitializeDataOnce InitializeClass initObject(*this);
	bool BasManagerDataHelper::InitializeClass::m_isInitialize = false;
	BasManagerDataHelper::InitializeClass::InitializeClass(BasManagerDataHelper& helper)
	{
		FUNCTION_ENTRY("InitializeClass");
		if (false == m_isInitialize)
		{
	        helper.loadLocationsFromDatabase();
	        helper.loadModeTypesFromDatabase();
	        helper.loadModeSubsystemFromDatabase();
			helper.loadModesFromDatabase();
			m_isInitialize = true;
		}
		FUNCTION_EXIT;
	}

	BasManagerDataHelper::InitializeClass::~InitializeClass(void)
	{
		FUNCTION_ENTRY("~InitializeClass");
		FUNCTION_EXIT;
	}

	BasManagerDataHelper::BasManagerDataHelper(void)
	:
	m_basManagerEntityKey(0),
	m_basManagerEntityName(""),
	m_basManagerEntityTypeKey(0),
	m_basManagerSubsystemKey(0),
	m_basManagerLocationKey(0)
	{
		FUNCTION_ENTRY("BasManagerDataHelper");
		// why the boost singleton template call this before app initialize.
		//loadModesFromDatabase();
		//loadLocationsFromDatabase();
		//loadModeTypesFromDatabase();
		//loadModeSubsystemFromDatabase();
		m_executeModeTimeOut = 0;
		FUNCTION_EXIT;
	}

	BasManagerDataHelper::~BasManagerDataHelper(void)
	{
		FUNCTION_ENTRY("~BasManagerDataHelper");
		FUNCTION_EXIT;
	}

	void BasManagerDataHelper::loadLocationsFromDatabase()
	{
		FUNCTION_ENTRY("loadLocationsFromDatabase");	

	    TA_THREADGUARD(m_modeLocationLock);
	    m_mapKeyToLocations.clear();
	    m_locations.clear();

	    std::set<unsigned long> setLocKeys = TA_IRS_Core::SubsystemModeTypeMappingAccessFactoryInstance::instance()->getAllValidLocationKeys();

	    std::vector<TA_Base_Core::ILocation*> vctLocations;
		BAS_TRY
		{
	        vctLocations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

			for (std::vector<TA_Base_Core::ILocation*>::iterator it = vctLocations.begin(); it != vctLocations.end(); it++)
			{
	            ILocation_SmartPtr temp(*it);

	            if ( setLocKeys.end() != setLocKeys.find( temp->getKey() ))
	            {
				    m_mapKeyToLocations.insert(std::make_pair(temp->getKey(), temp));
	                m_locations.push_back( temp );
	            }
			}
		}
		BAS_CATCH;

		if (BAS_WhetherHaveError)
	    {
	        m_mapKeyToLocations.clear();
	        m_locations.clear();
		}

		FUNCTION_EXIT;
	}

	void BasManagerDataHelper::loadModesFromDatabase()
	{
		FUNCTION_ENTRY("loadModesFromDatabase");	

		TA_THREADGUARD(m_modesLock);
		m_mapKeyToModes.clear();
	    m_mapLocToKeyModes.clear();

		BAS_TRY
		{
	        std::vector<boost::shared_ptr<Mode>> vecModes = TA_IRS_Core::ModeAccessFactory::getInstance().getAllModes();

	        for (std::vector<boost::shared_ptr<Mode>>::iterator it = vecModes.begin(); it != vecModes.end(); it++)
	        {
	            m_mapKeyToModes.insert(std::make_pair((*it)->getKey(), *it));
	            m_mapLocToKeyModes[(*it)->getLocationKey()].insert(std::make_pair((*it)->getKey(), *it));
	        }
		}
		BAS_CATCH;
	
		if (BAS_WhetherHaveError)
		{
			m_mapKeyToModes.clear();
	        m_mapLocToKeyModes.clear();
		}
		FUNCTION_EXIT;
	}

	void BasManagerDataHelper::loadModeTypesFromDatabase()
	{
		FUNCTION_ENTRY("loadModeTypesFromDatabase");	
		using namespace TA_IRS_Core;
		TA_THREADGUARD(m_modeTypesLock);
		m_modeTypes.clear();

		if(false == ModeTypeAccessFctory::getInstance().getModeTypes(m_modeTypes))
		{
			m_modeTypes.clear();
		}	
		FUNCTION_EXIT;
	}

	void BasManagerDataHelper::loadModeSubsystemFromDatabase()
	{
		FUNCTION_ENTRY("loadModeSubsystemFromDatabase");	

		TA_THREADGUARD(m_modeSubSystemLock);
		m_modeSubSystems.clear();
	    m_mapLocToOrderedSubsystems.clear();
	    TA_IRS_Core::AllLocationSubsystems mapLocSubsystems;

		BAS_TRY
		{
	        if(!TA_IRS_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(mapLocSubsystems))
			{
				m_modeSubSystems.clear();
	            m_mapLocToOrderedSubsystems.clear();
			}
	        else
	        {
	            for ( TA_IRS_Core::AllLocationSubsystems::iterator itLocLoop = mapLocSubsystems.begin(); mapLocSubsystems.end() != itLocLoop; ++itLocLoop )
	            {
	                MapKeyToSubsystem mapKeyToSubsystem;
	                std::pair<MapLocationToKeySubsystem::iterator, bool> pirKeyRes = m_modeSubSystems.insert( std::make_pair( itLocLoop->first, mapKeyToSubsystem ));
	                if ( pirKeyRes.second && ( m_modeSubSystems.end() != pirKeyRes.first ))
	                {
	                    MapKeyToSubsystem& refNewItem = pirKeyRes.first->second;
	                    for ( TA_IRS_Core::LocationSubsystem::iterator itSubLoop = itLocLoop->second.begin(); itLocLoop->second.end() != itSubLoop; ++itSubLoop )
	                    {
	                        refNewItem.insert( std::make_pair( (*itSubLoop)->getKey(), *itSubLoop ));
	                    }
	                }

	                MapOrderToSubsystem mapOrderToSubsystem;
	                std::pair<MapLocationToOrderedSubsystem::iterator, bool> pirOrderRes = m_mapLocToOrderedSubsystems.insert( std::make_pair( itLocLoop->first, mapOrderToSubsystem ));
	                if ( pirOrderRes.second && ( m_mapLocToOrderedSubsystems.end() != pirOrderRes.first ))
	                {
	                    MapOrderToSubsystem& refNewItem = pirOrderRes.first->second;
	                    for ( TA_IRS_Core::LocationSubsystem::iterator itSubLoop = itLocLoop->second.begin(); itLocLoop->second.end() != itSubLoop; ++itSubLoop )
	                    {
	                        refNewItem.insert( std::make_pair( (*itSubLoop)->getOrderID(), *itSubLoop ));
	                    }
	                }
	            }
	        }
		}
		BAS_CATCH;

		if (BAS_WhetherHaveError)
		{
			m_modeSubSystems.clear();
	        m_mapLocToOrderedSubsystems.clear();
		}
		FUNCTION_EXIT;
	}

	// The location actions
	ILocation_SmartPtr BasManagerDataHelper::getLocalLocation()
	{
	    FUNCTION_ENTRY("getLocalLocation");

	    if ( NULL == m_ptrLocalStation.get() )
	    {
	        BAS_TRY
	        {
	            m_ptrLocalStation.reset(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( getManagerLocationKey() ));
	        }
	        BAS_CATCH;
	    }

	    FUNCTION_EXIT;
	    return m_ptrLocalStation;
	}

	std::vector<ILocation_SmartPtr> BasManagerDataHelper::getAllLocations()
	{
		FUNCTION_ENTRY("getAllLocations");
		InitializeDataOnce;	// load the data from DB

		TA_THREADGUARD(m_modeLocationLock);

		FUNCTION_EXIT;
		return m_locations;
	}

	ILocation_SmartPtr BasManagerDataHelper::getLocationByKey(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getLocationByKey");
		InitializeDataOnce;	// load the data from DB
	    TA_THREADGUARD(m_modeLocationLock);
	    MapKeyToLocation::iterator itFound = m_mapKeyToLocations.find( locationKey );

	    if ( itFound != m_mapKeyToLocations.end() )
	    {
	        FUNCTION_EXIT;
	        return itFound->second;
	    }

	    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Location %lu is not in cache", locationKey );
	    static ILocation_SmartPtr ptrNullLoc( static_cast<TA_Base_Core::ILocation*>(NULL) );
	    FUNCTION_EXIT;
	    return ptrNullLoc;
	}

	std::string BasManagerDataHelper::getLocationDispNameByKey( unsigned long locationKey )
	{
	    FUNCTION_ENTRY("getLocationByKey");
	    ILocation_SmartPtr ptrLoc = getLocationByKey( locationKey );

	    std::string strLocDispName = "δ֪";

	    if ( NULL != ptrLoc.get() )
	    {
	        strLocDispName = ptrLoc->getDisplayName();
	    }

	    FUNCTION_EXIT;
	    return strLocDispName;
	}

	// The Modes actions
	std::vector<Mode_SmartPtr> BasManagerDataHelper::getModes(unsigned long location, unsigned long subsystem, unsigned long modeType)
	{
		FUNCTION_ENTRY("getModes");
		InitializeDataOnce;	// load the data from DB

		TA_THREADGUARD(m_modesLock);
	    static unsigned long allSubSystemItemKey = getAllSubsystemItemKey();
		static unsigned long allModeTypesItemKey = getAllModeTypeItemKey();
	    std::vector<Mode_SmartPtr> vctReturn;

	    // filter location
	    MapLocationToKeyMode::iterator itFound = m_mapLocToKeyModes.find( location );
	    if ( m_mapLocToKeyModes.end() == itFound )
	    {
	        return vctReturn;
	    }

		for (MapKeyToMode::iterator it = itFound->second.begin(); it != itFound->second.end(); it++)
	    {
	        // filter filter subsystem
	        if (( allSubSystemItemKey != subsystem ) && ( it->second->getSubsystemKey() != subsystem ))
	        {
	            continue;
	        }

	        // filter mode type
	        if (( allModeTypesItemKey != modeType ) && ( it->second->getTypeKey() != modeType ))
	        {
	            continue;
	        }

	        vctReturn.push_back(it->second);
		}

		FUNCTION_EXIT;
		return vctReturn;
	}

	SubSystem_SmartPtr BasManagerDataHelper::getSubsystemByKey(unsigned long locationKey, unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSubsystemByKey");
		InitializeDataOnce;	// load the data from DB

		SubSystem_SmartPtr subsystem_SmartPtr;
		MapLocationToKeySubsystem::iterator IterSubsystem = m_modeSubSystems.find(locationKey);
		if (IterSubsystem != m_modeSubSystems.end())
	    {
	        MapKeyToSubsystem::iterator itFound = IterSubsystem->second.find( subsystemKey );
	        if (itFound != IterSubsystem->second.end())
	        {
	            subsystem_SmartPtr = itFound->second;
	        }
		}
		FUNCTION_EXIT;
		return subsystem_SmartPtr;
	}

	std::vector<Mode_SmartPtr> BasManagerDataHelper::getModes(unsigned long location, std::vector<unsigned long> subsystems, unsigned long modeType)
	{
		FUNCTION_ENTRY("getModes");
		InitializeDataOnce;	// load the data from DB

		std::vector<Mode_SmartPtr> vctReturn;
		std::vector<unsigned long>::iterator it;
		for (it = subsystems.begin(); it != subsystems.end(); it++)
		{
	        std::vector<Mode_SmartPtr> vctModes = getModes(location, (*it),modeType);
	        vctReturn.insert(vctReturn.end(),vctModes.begin(), vctModes.end());
		}
		FUNCTION_EXIT;
		return vctReturn;
	}

	std::vector<Mode_SmartPtr> BasManagerDataHelper::getModeByModeCodeName(const std::string& modeCodeName)
	{
		FUNCTION_ENTRY("getModeByModeCodeName");
		InitializeDataOnce;	// load the data from DB
		TA_THREADGUARD(m_modesLock);
		std::vector<Mode_SmartPtr> vctReturn;
		for (MapKeyToMode::iterator it = m_mapKeyToModes.begin(); it != m_mapKeyToModes.end(); it++)
		{
			std::string strModeCodeName = it->second->getName();
			std::string strSearchName = modeCodeName;
			std::transform(strModeCodeName.begin(), strModeCodeName.end(), strModeCodeName.begin(), toupper);
			std::transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), toupper);
			if (std::string::npos != strModeCodeName.find( strSearchName.c_str()))
			{
				vctReturn.push_back(it->second);
			}
		}
		FUNCTION_EXIT;
		return vctReturn;
	}

	std::vector<Mode_SmartPtr> BasManagerDataHelper::getModeByModeCodeName(const std::string& modeCodeName, unsigned long locationKey)
	{
		FUNCTION_ENTRY("getModeByModeCodeName");
		InitializeDataOnce;	// load the data from DB

		TA_THREADGUARD(m_modesLock);

	    std::vector<Mode_SmartPtr> vctReturn;
	    MapLocationToKeyMode::iterator itFound = m_mapLocToKeyModes.find( locationKey );
	    if ( m_mapLocToKeyModes.end() == itFound )
	    {
	        return vctReturn;
	    }

		for (MapKeyToMode::iterator it = itFound->second.begin(); it != itFound->second.end(); it++)
	    {
	        std::string strModeCodeName = it->second->getName();
	        std::string strSearchName = modeCodeName;
	        std::transform(strModeCodeName.begin(), strModeCodeName.end(), strModeCodeName.begin(), toupper);
	        std::transform(strSearchName.begin(), strSearchName.end(), strSearchName.begin(), toupper);
	        if (std::string::npos != strModeCodeName.find( strSearchName.c_str()))
	        {
	            vctReturn.push_back(it->second);
	        }
		}
		FUNCTION_EXIT;
		return vctReturn;
	}

	Mode_SmartPtr BasManagerDataHelper::getModeByKey(unsigned long modeKey)
	{
		FUNCTION_ENTRY("getModeByKey");
		InitializeDataOnce;	// load the data from DB
		TA_THREADGUARD(m_modesLock);
		Mode_SmartPtr retPtr;
		MapKeyToMode::iterator itFound = m_mapKeyToModes.find( modeKey );
		if ( m_mapKeyToModes.end() != itFound )
		{
	        retPtr = itFound->second;
		}
		FUNCTION_EXIT;
		return retPtr;
	}

	Mode_SmartPtr BasManagerDataHelper::getModeByLocAndKey(unsigned long locationKey, unsigned long modeKey)
	{
	    FUNCTION_ENTRY("getModeByLocAndKey");
	    InitializeDataOnce;	// load the data from DB
	    TA_THREADGUARD(m_modesLock);
	    Mode_SmartPtr retPtr;
	    MapLocationToKeyMode::iterator itFound = m_mapLocToKeyModes.find( locationKey );
	    if ( m_mapLocToKeyModes.end() != itFound )
	    {
	        MapKeyToMode::iterator itMode = itFound->second.find( modeKey );
	        if ( itFound->second.end() != itMode )
	        {
	            retPtr = itMode->second;
	        }
	    }
	    FUNCTION_EXIT;
	    return retPtr;
	}

	Mode_SmartPtr BasManagerDataHelper::getModeByValue(unsigned long locationKey, unsigned int modeValue)
	{
		FUNCTION_ENTRY("getModeByValue");
		InitializeDataOnce;	// load the data from DB

		TA_THREADGUARD(m_modeSubSystemLock);

	    Mode_SmartPtr retPtr;

	    MapLocationToKeyMode::iterator itFound = m_mapLocToKeyModes.find( locationKey );
	    if ( m_mapLocToKeyModes.end() == itFound )
	    {
	        return retPtr;
	    }

		for (MapKeyToMode::iterator it = itFound->second.begin(); it != itFound->second.end(); it++)
		{
			if (it->second->getValue() == modeValue)
			{
				retPtr = it->second;
				break;
			}
		}
		FUNCTION_EXIT;
		return retPtr;
	}

	// The Subsystem actions
	MapOrderToSubsystem BasManagerDataHelper::getAllSubsystems(unsigned long locationKey, bool withSystemConfig /*= true*/)
	{
		FUNCTION_ENTRY("getAllSubsystems");
		InitializeDataOnce;	// load the data from DB
		TA_THREADGUARD(m_modeSubSystemLock);
		MapOrderToSubsystem retVct;
		MapLocationToOrderedSubsystem::iterator IterSubsystem = m_mapLocToOrderedSubsystems.find(locationKey);
		if( IterSubsystem != m_mapLocToOrderedSubsystems.end())
	    {
	        retVct = IterSubsystem->second;

			if (!withSystemConfig)
			{
				for (MapKeyToSubsystem::iterator it = retVct.begin(); it != retVct.end();)
				{
					if (it->second->getIsSystem())
					{
						retVct.erase( it++ );
					}
	                else
	                {
	                    ++it;
	                }
				}
			}
		}

		FUNCTION_EXIT;
		return retVct;
	}

	// The Mode types actions
	ModeTypeMap BasManagerDataHelper::getAllModeTypes()
	{
		FUNCTION_ENTRY("getAllModeTypes");
		InitializeDataOnce;	// load the data from DB
		TA_THREADGUARD(m_modeTypesLock);
		FUNCTION_EXIT;
		return m_modeTypes;
	}

	std::string BasManagerDataHelper::getTypeName( const unsigned long ulTypeKey )
	{
	    FUNCTION_ENTRY("getTypeName");
	    InitializeDataOnce;	// load the data from DB
	    TA_THREADGUARD(m_modeTypesLock);
	    FUNCTION_EXIT;
	    return m_modeTypes[ulTypeKey].modeTypeName;
	}

	unsigned long BasManagerDataHelper::getAllSubsystemItemKey()
	{
		FUNCTION_ENTRY("getAllSubsystemItemKey");

	    static unsigned long pkey = TA_IRS_Core::SubsystemAccessFactory::getInstance().getAllSubSystemKey();

		FUNCTION_EXIT;
		return pkey;
	}

	unsigned long BasManagerDataHelper::getAllModeTypeItemKey()
	{
		FUNCTION_ENTRY("getAllModeTypeItemKey");
		InitializeDataOnce;	// load the data from DB
		TA_THREADGUARD(m_modeTypesLock);

		unsigned long pkey = 0;
		ModeTypeMap::iterator it;
		for (it = m_modeTypes.begin(); it != m_modeTypes.end(); it++)
		{
			if (false != it->second.isSystem)
			{
				pkey = it->first;
			}
		}
		FUNCTION_EXIT;
		return pkey;
	}

	SubSystemModeTypeMap BasManagerDataHelper::getSubsystemModeTypeMappingByLocation(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getSubsystemModeTypeMappingByLocation");

		LocationSubSystemModeTypeMap::iterator itFound = m_subsystemModeTypeMap.find(locationKey);
		if (m_subsystemModeTypeMap.end() == itFound)
		{
			SubSystemModeTypeMap subSystemModeTypeMap;
			SubsystemModeTypeMappingAccessFactoryInstance::instance()->getSubSystemModeTypeMapByLocation(locationKey, subSystemModeTypeMap);

			itFound = m_subsystemModeTypeMap.insert(std::make_pair(locationKey, subSystemModeTypeMap)).first;
		}
		FUNCTION_EXIT;
		return (itFound->second);
	}

	TA_IRS_Core::ModeDevices BasManagerDataHelper::getModeDevicesByModeKey(unsigned long locationKey, unsigned long modeKey)
	{
		FUNCTION_ENTRY("getModeDevicesByModeKey");

		bool bFoundByModeKey = false;
		ModeToDevicesMap::iterator itFoundModeDevice;

		LocationModeDevices::iterator itFoundLocationModeDevice = m_modeDeviceMap.find(locationKey);
		bool bFoundByLocationKey = (m_modeDeviceMap.end() != itFoundLocationModeDevice);	
		if (bFoundByLocationKey)
		{
			itFoundModeDevice = itFoundLocationModeDevice->second.find(modeKey);
			bFoundByModeKey = (itFoundLocationModeDevice->second.end() != itFoundModeDevice);
		}
		else
		{
			ModeToDevicesMap tempDevicesMap;
			itFoundLocationModeDevice = m_modeDeviceMap.insert(std::make_pair(locationKey,tempDevicesMap)).first;
		}
	
		if( !bFoundByModeKey )
		{
	        ModeDevices modeDevices;
	        ModeDeviceAccessFactory::getInstance().getModeDevicesByModeKey(locationKey, modeKey, modeDevices);
		
			itFoundModeDevice = itFoundLocationModeDevice->second.insert(std::make_pair(modeKey, modeDevices)).first;
		}

		FUNCTION_EXIT;
		return itFoundModeDevice->second;
	}

	unsigned long BasManagerDataHelper::getModeExecuteTimeOutTime()
	{
		FUNCTION_ENTRY("getModeExecuteTimeOutTime");
		using namespace TA_Base_Core;

		m_executeModeTimeOut = 61;
		if (61 == m_executeModeTimeOut)
		{
			static const std::string strEntityName = "BasModeManager";
			static const std::string strParameterName = "ExecuteModeTimeOut";
			IData* data = EntityAccessFactory::getInstance().getParameter(strEntityName, strParameterName);
			if (NULL != data)
			{
	            m_executeModeTimeOut = data->getUnsignedLongData(0, "VALUE", 60);
	            delete data;
	            data = NULL;
			}
		}

		FUNCTION_EXIT;
		return m_executeModeTimeOut;
	}

	std::string BasManagerDataHelper::getFireModeRecoverDataPointName()
	{
		FUNCTION_ENTRY("getFireModeRecoverDataPointName");

		if (m_fireModeRecoverDatapointName.empty())
		{
			static const std::string strEntityName = "BasModeManager";
			static const std::string strParameterName = "FireModeRecoverDpTag";
	        TA_Base_Core::IData* data = TA_Base_Core::EntityAccessFactory::getInstance().getParameter(strEntityName, strParameterName);
			if (NULL != data)
			{
	            m_fireModeRecoverDatapointName = data->getStringData(0, "VALUE", "");
	            delete data;
	            data = NULL;
			}
		}

		FUNCTION_EXIT;
		return m_fireModeRecoverDatapointName;
	}

	bool BasManagerDataHelper::getIsManagerInFireMode()
	{
		return m_isManagerInFireMode;
	}
	void BasManagerDataHelper::setManagerinFireMode(bool bFireMode)
	{
		m_isManagerInFireMode = bFireMode;
	}

	void BasManagerDataHelper::loadBasManagerEntityDataFromDatabase()
	{
		FUNCTION_ENTRY("loadBasManagerEntityDataFromDatabase");

		BAS_TRY
		{
			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	        TA_Base_Core::IEntityData* pData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);

	        if (NULL != pData)
	        {
	            m_basManagerEntityKey = pData->getKey();
	            m_basManagerEntityName = pData->getName();
	            m_basManagerEntityTypeKey = pData->getTypeKey();
	            m_basManagerSubsystemKey = pData->getSubsystem();
	            delete pData;
	            pData = NULL;
	        }
		}
		BAS_CATCH;

		FUNCTION_EXIT;
	}
	unsigned long BasManagerDataHelper::getManagerEntityKey()
	{
		FUNCTION_ENTRY("getManagerEntityKey");
		if (0 == m_basManagerEntityKey)
		{
			loadBasManagerEntityDataFromDatabase();
		}
		FUNCTION_EXIT;
		return m_basManagerEntityKey;

	}
	std::string BasManagerDataHelper::getManagerEntityName()
	{
		FUNCTION_ENTRY("getManagerEntityName");
		if ("" == m_basManagerEntityName)
		{
			loadBasManagerEntityDataFromDatabase();
		}
		FUNCTION_EXIT;
		return m_basManagerEntityName;
	}
	unsigned long BasManagerDataHelper::getManagerEntitySubsystemKey()
	{
		FUNCTION_ENTRY("getManagerEntitySubsystemKey");
		if (0 == m_basManagerSubsystemKey)
		{
			loadBasManagerEntityDataFromDatabase();
		}
		FUNCTION_EXIT;
		return m_basManagerSubsystemKey;
	}

	unsigned long BasManagerDataHelper::getManagerEntityTypeKey()
	{
		FUNCTION_ENTRY("getManagerEntityTypeKey");
		if(0 == m_basManagerEntityTypeKey)
		{
			loadBasManagerEntityDataFromDatabase();
		}
		FUNCTION_EXIT;
		return m_basManagerEntityTypeKey;
	}

	unsigned long BasManagerDataHelper::getManagerLocationKey()
	{
		FUNCTION_ENTRY("getManagerLocationKey");

		if (0 == m_basManagerLocationKey)
		{
			std::string strLocationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);

			m_basManagerLocationKey = TA_Base_Core::DataConversion::toUnsignedInteger( strLocationKey );
		}

		FUNCTION_EXIT;
		return m_basManagerLocationKey;
	}

	bool BasManagerDataHelper::isAtStation()
	{
	    try
	    {
	        std::auto_ptr<TA_Base_Core::ILocation> ptrLoc(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( getManagerLocationKey() ));

        	return ( ptrLoc->getLocationType() == TA_Base_Core::ILocation::STATION || ptrLoc->getLocationType() == TA_Base_Core::ILocation::DPT );
    	}
    catch ( ... )
    {
    }

	    return false;
	}

	unsigned long BasManagerDataHelper::getManagerModeValue()
	{
	    FUNCTION_ENTRY("getManagerModeValue");

	    std::string strModeValue = TA_Base_Core::RunParams::getInstance().get(RPARAM_MANAGERMODE_VALUE);

	    FUNCTION_EXIT;
	    return TA_Base_Core::DataConversion::toUnsignedInteger( strModeValue );;
	}

	unsigned long BasManagerDataHelper::getFireLocationKey()
	{
	    std::string strLocationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_FIRELOCATIONKEY);

	    // get the local location key
	    return TA_Base_Core::DataConversion::toUnsignedInteger( strLocationKey );
	}
}

