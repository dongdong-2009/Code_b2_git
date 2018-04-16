#if !defined(TA_GF_3002_app_bas_BasParameterManager_EquipmentManager)
#define TA_GF_3002_app_bas_BasParameterManager_EquipmentManager
#pragma once
#include "ace/Singleton.h"
#include "boost/shared_ptr.hpp"
#include "app/bas/BasParameterManager/src/Equipment.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

namespace TA_IRS_App
{
	typedef std::vector<Equipment_SmartPtr> EquipmentVct;
	typedef std::map<unsigned long /*datapointType*/, EquipmentVct> EquipmentMap;
	typedef EquipmentMap::iterator EquipmentMapIter;
	typedef std::map<unsigned long /*locationKey*/, EquipmentMap> LocationEquipmentMap;
	typedef LocationEquipmentMap::iterator LocationEquipmentMapIter;

	class IEquipmentUpdateInterface
	{
	public:
		virtual void UpdateDatapointValue(unsigned long datapointKey, const std::string& newValue) = 0;
		virtual void UpdateDatapointStatus(unsigned long datapointKey, const std::string& strStatus) = 0;
	};
	class EquipmentManager
	{
	public:
		friend Equipment;
		EquipmentManager(IEquipmentUpdateInterface* listener);
		~EquipmentManager(void);
	public:
		// The data points actions
		void getLocationEquipmentMap(unsigned long locationKey, EquipmentMap& equipments);

		void loadLocationEquipments(unsigned long locationKey);

		void prepareToExist();

		void setEquipmentValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double value);

		bool getEquipmentLimitedValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double& lowValue, double& highValue);

		void setExecuteStatus( unsigned long locationKey,unsigned long typeKey, unsigned long entityKey, const std::string& strStatus);
	protected:
		void updataDatapointValue(unsigned long datapointKey, const std::string& strValue);
		void updataDatapointStatus(unsigned long datapointKey, const std::string& strStatus);		
	private:
		std::string constructTokenString(const std::string& strLocation, const std::string& token);
	private:
		// for the data point
		TA_Base_Core::NonReEntrantThreadLockable m_LocationEquipmentLock;
		LocationEquipmentMap m_locationEquipmentMap;

		static const std::string DataNodeEntityTypeName;
		static const std::string DataPointEntityTypeName;

		IEquipmentUpdateInterface* m_equipmentUpdateListener;

		volatile bool m_isPrepareToExit;
	};
}

#endif //TA_GF_3002_app_bas_BasParameterManager_EquipmentManager
