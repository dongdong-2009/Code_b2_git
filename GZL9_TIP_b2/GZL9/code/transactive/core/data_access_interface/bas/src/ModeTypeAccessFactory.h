#if !defined(TA_3002_transactive_core_data_access_interface_bas_ModeTypeAccessFctory_H) 
#define TA_3002_transactive_core_data_access_interface_bas_ModeTypeAccessFctory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <map>
#include <sstream>
#include <algorithm>
#include "boost/smart_ptr.hpp"

namespace TA_IRS_Core
{
	typedef struct _ModeTypeInfo_
	{
		unsigned long pkey;
		std::string modeTypeName;
		std::string Description;
		bool isSystem;
	}ModeTypeInfo;
	typedef std::map<unsigned long, ModeTypeInfo> ModeTypeMap;
	typedef ModeTypeMap::iterator ModeTypeMapIter;

	class ModeTypeAccessFctory
	{
	public:
		~ModeTypeAccessFctory();
		enum{modeTypeKey_All = 0, modeTypeKey_Normal = 1, modeTypeKey_Congestion = 2, modeTypeKey_Fire = 3 };
	public:
		static ModeTypeAccessFctory& getInstance();

		ModeTypeMap getModeTypes();

		bool getModeTypes(ModeTypeMap& modeTypes);

	protected:
		ModeTypeAccessFctory();
		ModeTypeAccessFctory(const ModeTypeAccessFctory&);
		ModeTypeAccessFctory& operator=(const ModeTypeAccessFctory&);

	private:
		static bool stringToBoolean(const std::string& boolean);

	private:
		static ModeTypeAccessFctory* m_instance;
	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_ModeTypeAccessFctory_H