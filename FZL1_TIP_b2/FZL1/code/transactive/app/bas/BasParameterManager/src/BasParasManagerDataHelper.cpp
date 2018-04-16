#include "StdAfx.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/bas/src/SubsystemModeTypeMappingAccessFactory.h"
#include "core/data_access_interface/src/DatabaseKey.h"
#include "core/utilities/src/DataConversion.h"
#include "app/bas/BasParameterManager/src/BasParasManagerDataHelper.h"
#include "app/bas/BasParameterManager/src/CommonDefine.h"
namespace TA_IRS_App
{
	////////////////////////////////////////////////////////////////////////////////
	BasParasManagerDataHelper::BasParasManagerDataHelper(void)
	{
		m_basLocalLocationKey = TA_Base_Core::DatabaseKey::getInvalidKey();
	}

	BasParasManagerDataHelper::~BasParasManagerDataHelper(void)
	{
	}

	// The location actions
	unsigned long BasParasManagerDataHelper::getLocalLocationKey()
	{
		FUNCTION_ENTRY("getLocalLocationKey");

		if (TA_Base_Core::DatabaseKey::isInvalidKey(m_basLocalLocationKey))
		{
			std::string strLocationKey = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);

			m_basLocalLocationKey = TA_Base_Core::DataConversion::toUnsignedInteger( strLocationKey );
		}

		FUNCTION_EXIT;
		return m_basLocalLocationKey;
	}

	ILocation_SmartPtr BasParasManagerDataHelper::getLocalLocation()
	{
		FUNCTION_ENTRY("getLocalLocation");

		if ( NULL == m_ptrLocalStation.get() )
		{
			BAS_TRY
			{
				m_ptrLocalStation.reset(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( getLocalLocationKey() ));
			}
			BAS_CATCH;
		}

		FUNCTION_EXIT;
		return m_ptrLocalStation;
	}

	std::vector<ILocation_SmartPtr> BasParasManagerDataHelper::getAllLocations()
	{
		FUNCTION_ENTRY("getAllLocations");

        bool bReload = false;
        {
            TA_THREADGUARD(m_LocationLock);

            bReload = m_locations.empty();
        }

        if ( bReload )
        {
            loadLocationsFromDatabase();
        }

		TA_THREADGUARD(m_LocationLock);

		FUNCTION_EXIT;
		return m_locations;
	}

	ILocation_SmartPtr BasParasManagerDataHelper::getLocationByKey(unsigned long locationKey)
	{
		FUNCTION_ENTRY("getLocationByKey");

        getAllLocations();

		{
			TA_THREADGUARD(m_LocationLock);
			std::vector<ILocation_SmartPtr>::iterator it;
			for (it = m_locations.begin(); it != m_locations.end(); it++)
			{
				if (locationKey == (*it)->getKey())
				{
					FUNCTION_EXIT;
					return *it;
				}
			}
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Location %lu is not in cache", locationKey );
		ILocation_SmartPtr ptrNullLoc( static_cast<TA_Base_Core::ILocation*>(NULL) );
		FUNCTION_EXIT;
		return ptrNullLoc;
	}

	void BasParasManagerDataHelper::loadLocationsFromDatabase()
	{
		FUNCTION_ENTRY("loadLocationsFromDatabase");	

        TA_THREADGUARD(m_LocationLock);
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
                    m_locations.push_back(temp);
                }
            }	
		}
		BAS_CATCH;

		if (BAS_WhetherHaveError)
		{
			vctLocations.clear();
		}
		FUNCTION_EXIT;
	}

	void BasParasManagerDataHelper::getParameterValueType(TA_IRS_Core::ParameterValueTypeInfoVct& valueTypes)
	{
		FUNCTION_ENTRY("getParameterValueType");
		if (m_parameterValueTypeVct.empty())
		{
			TA_IRS_Core::BasParameterValueTypeAccessFactory::getInstance().getParameterValueTypeInfos(m_parameterValueTypeVct);
		}
		FUNCTION_EXIT;
		valueTypes = m_parameterValueTypeVct;
	}

	bool BasParasManagerDataHelper::isAtStation()
	{
		FUNCTION_ENTRY("getParameterValueType");
		BAS_TRY
		{
			std::auto_ptr<TA_Base_Core::ILocation> ptrLoc(TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( getLocalLocationKey() ));

			FUNCTION_EXIT;
			return ( ptrLoc->getLocationType() == TA_Base_Core::ILocation::STATION );
		}
		BAS_CATCH
 
        FUNCTION_EXIT;
		return false;
	}
}
