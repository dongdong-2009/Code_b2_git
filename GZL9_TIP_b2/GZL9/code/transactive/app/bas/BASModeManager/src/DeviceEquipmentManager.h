#if !defined(APP_BASBasDevieceEquipmentManager_EquipmentManager)
#define APP_BASBasDevieceEquipmentManager_EquipmentManager
#pragma once
#include "ace/Singleton.h"
#include "boost/shared_ptr.hpp"
#include "app/bas/BASModeManager/src/DeviceEquipment.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{
	typedef std::vector<DeviceEquipment_SmartPtr> EquipmentVct;
	typedef std::map<unsigned long /*mode key*/, EquipmentVct> DeviceEquipmentMap;
	typedef DeviceEquipmentMap::iterator DeviceEquipmentMapIter;
	typedef std::map<unsigned long /*locationKey*/, DeviceEquipmentMap> LocationDeviceEquipmentMap;
	typedef LocationDeviceEquipmentMap::iterator LocationDeviceEquipmentMapIter;

	class IEquipmentUpdateInterface
	{
	public:
		virtual void UpdateDatapointValue(unsigned long deviceKey, const std::string& newValue, bool status) = 0;
	};
	class DeviceEquipmentManager
	{
	public:
		friend DeviceEquipment;
		DeviceEquipmentManager();
		~DeviceEquipmentManager(void);
	public:
		// The data points actions
		void getLocationDeviceEquipments(unsigned long locationKey, unsigned long modeKey, EquipmentVct& equipments);

		void resetListener(IEquipmentUpdateInterface* listener);

		bool stopLoadDeviceEquipments();

		void prepareClose();
	protected:
		void updataDatapointValue(unsigned long deviceKey, const std::string& strValue, bool status);

		void loadLocationDeviceEquipments(unsigned long locationKey, unsigned long modeKey);
	private:
		// for the data point
		TA_Base_Core::NonReEntrantThreadLockable m_LocationEquipmentLock;
		//for the listener. GF-78. like++
		TA_Base_Core::NonReEntrantThreadLockable m_ListenerLock;
		LocationDeviceEquipmentMap m_locationEquipmentMap;

		static const std::string DataNodeEntityTypeName;
		static const std::string DataPointEntityTypeName;

		IEquipmentUpdateInterface* m_equipmentUpdateListener;

		volatile bool m_isStopLoading;
	};
	typedef ACE_Singleton<DeviceEquipmentManager,ACE_Recursive_Thread_Mutex> DeviceEquipmentManagerInstance;
}

#endif //APP_BASBasDevieceEquipmentManager_EquipmentManager
