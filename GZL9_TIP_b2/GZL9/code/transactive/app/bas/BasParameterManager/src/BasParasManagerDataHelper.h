#if !defined(TA_GF_3002_app_bas_BasParameterManager_BasParasManagerDataHelper)
#define TA_GF_3002_app_bas_BasParameterManager_BasParasManagerDataHelper
#pragma once

#include "ace/Singleton.h"
#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/data_access_interface/bas/src/BasParameterValueType.h"

namespace TA_IRS_App
{
	////////////////////////////////////////////////////////////////////////////////
	typedef boost::shared_ptr<TA_Base_Core::ILocation> ILocation_SmartPtr;

	class BasParasManagerDataHelper
	{
	public:
		BasParasManagerDataHelper(void);
		~BasParasManagerDataHelper(void);
	public:
		// The location actions
		unsigned long getLocalLocationKey();
		ILocation_SmartPtr getLocalLocation();
		std::vector<ILocation_SmartPtr> getAllLocations();
		ILocation_SmartPtr getLocationByKey(unsigned long locationKey);
		bool isAtStation();

		// The parameter value type
		void getParameterValueType(TA_IRS_Core::ParameterValueTypeInfoVct& valueTypes);
	private:
		void loadLocationsFromDatabase();

	private:
		TA_Base_Core::NonReEntrantThreadLockable m_LocationLock;	// The location Lock
		std::vector<ILocation_SmartPtr> m_locations;		// The locations
		ILocation_SmartPtr m_ptrLocalStation;
		unsigned long	m_basLocalLocationKey;
		TA_IRS_Core::ParameterValueTypeInfoVct m_parameterValueTypeVct;			// The parameter value types

	};
	typedef ACE_Singleton<BasParasManagerDataHelper,ACE_Recursive_Thread_Mutex> BasParasManagerDataHelperInstance;
}
#endif //TA_GF_3002_app_bas_BasParameterManager_BasParasManagerDataHelper