#if !defined(TA_3002_transactive_core_data_access_interface_bas_ModeAccessFactory_H)
#define TA_3002_transactive_core_data_access_interface_bas_ModeAccessFactory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <vector>
#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/bas/src/Mode.h"

namespace TA_IRS_Core
{
	typedef boost::shared_ptr<TA_IRS_Core::Mode> Mode_SmartPtr;
	class ModeAccessFactory
	{
	public:
		~ModeAccessFactory();

		static ModeAccessFactory& getInstance();

		std::vector<Mode_SmartPtr> getAllModes();

		bool getAllModes(std::vector<Mode_SmartPtr>& vctModes);

		std::vector<Mode_SmartPtr> getModes(unsigned int stationKey, unsigned int subsystemKey, unsigned int typeKey);

		bool getModes(unsigned int stationKey, std::vector<Mode_SmartPtr>& modes);
	protected:
		ModeAccessFactory();

	private:
		ModeAccessFactory(const ModeAccessFactory&);
		ModeAccessFactory& operator=(const ModeAccessFactory&);

	private:
		static ModeAccessFactory* m_instance;
		// the table name of modes table
		static const std::string m_tableName_modes_V;
		// the column name of modes table
		static const std::string m_column_pkey;
		static const std::string m_column_name;
		static const std::string m_column_value;
		static const std::string m_column_locationKey;
		static const std::string m_column_locationName;
		static const std::string m_column_subsystemKey;
		static const std::string m_column_subsystemName;
		static const std::string m_column_typeKey;
		static const std::string m_column_typeName;
		static const std::string m_column_description;
	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_ModeAccessFactory_H
