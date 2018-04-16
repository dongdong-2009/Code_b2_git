#if !defined(TA_3002_transactive_core_data_access_interface_bas_BasFireAccessFactory_H)
#define TA_3002_transactive_core_data_access_interface_bas_BasFireAccessFactory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>
#include <map>
#include "boost/smart_ptr.hpp"

namespace TA_IRS_Core
{
	typedef enum {FireStatus_Normal = 0, FireStatus_Fire = 1}BasStationFireStatus;
	typedef struct  
	{
		unsigned long pkey;
		unsigned long locationKey;
		std::string fireToken;
		std::string description;
		unsigned long modeValue;
		std::string users;
		std::string strMftNames;
	}BasFireEquipment;
	typedef boost::shared_ptr<BasFireEquipment> BasFireEquipmentPtr;
	typedef std::vector<BasFireEquipmentPtr> BasFireEquipmentVct;
	typedef std::map<unsigned long/*locationKey*/, BasFireEquipmentVct> BasFireEquipmentMap;

	class BasFireAccessFactory
	{
	private:
		BasFireAccessFactory(void);
	public:
		~BasFireAccessFactory(void);
	public:
		static BasFireAccessFactory& getInstance();
		void getAllBasFireEquipment(BasFireEquipmentMap& basFireEquipments);
		void getAllBasFireEquipmentByLocation(unsigned long locationKey, BasFireEquipmentVct& locationFireEquipments);
	private:
		static BasFireAccessFactory* m_instance;
		const static std::string COLUMN_PKEY;
		const static std::string COLUMN_LOCATIONKEY;
		const static std::string COLUMN_FIRETOKEN;
		const static std::string COLUMN_DESCRIPTION;
		const static std::string COLUMN_MODEVALUE;
		const static std::string COLUMN_SHOWING_MFT;
		const static std::string COLUMN_LOGIN_USER;
		const static std::string TABLE_NAME;
	};
}


#endif //TA_3002_transactive_core_data_access_interface_bas_BasFireAccessFactory_H