#if !defined(TA_3002_transactive_core_data_access_interface_bas_BasParameterValueTypeAccessFactory_H)
#define TA_3002_transactive_core_data_access_interface_bas_BasParameterValueTypeAccessFactory_H
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace TA_IRS_Core
{
	typedef struct  
	{
		unsigned long pkey;
		std::string name;
		std::string aiToken;
		std::string aoToken;
	} ParameterValueTypeInfo;
	typedef std::vector<ParameterValueTypeInfo> ParameterValueTypeInfoVct;
	class BasParameterValueTypeAccessFactory
	{
	public:
		~BasParameterValueTypeAccessFactory(void);
	public:
		static BasParameterValueTypeAccessFactory& getInstance();

		void getParameterValueTypeInfos(ParameterValueTypeInfoVct& parameterValueTypeInfoVct);
	private:
		BasParameterValueTypeAccessFactory(void);
	private:
		static BasParameterValueTypeAccessFactory* m_Instance;

		static const std::string TableName;
		static const std::string ColumnName_Pkey;
		static const std::string ColumnName_Name;
		static const std::string ColumnName_AIToken;
		static const std::string ColumnName_AOToken;
	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_BasParameterValueTypeAccessFactory_H