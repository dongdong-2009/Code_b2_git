#if !defined (Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableModeTimeAccessFactory)
#define Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableModeTimeAccessFactory
#if _MSC_VER > 1000
#pragma once  
#endif
#include "ace/Singleton.h"
#include <string>
#include <vector>
#include <map>
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"

namespace TA_IRS_Core
{
	class ModeTime
	{
	public:
		ModeTime(const std::string& value)
			:m_time(value), m_bIsValid(false)
		{
			if (false == checkTimeValid(value))
			{
				std::string strError("Invalid mode begin time:");
				strError += value;
				//TA_THROW(TA_Base_Core::TransactiveException(strError));
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strError.c_str());
			}
			else
			{
				m_bIsValid = true;
			}
			//m_time = value; move to initialize member
		}

		ModeTime& operator = (const std::string& value)
		{
			m_time = value;
			m_bIsValid = false;
			if (false == checkTimeValid(value))
			{
				std::string strError("Invalid mode begin time:");
				strError += value;
				//TA_THROW(TA_Base_Core::TransactiveException(strError));
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, strError.c_str());
			}
			else
			{
				m_bIsValid = true;
			}
			return *this;
		}
	private:
		ModeTime(){};
		//ModeTime& operator =(const ModeTime&){};
		
	public:
		std::string getTimeString(bool withCheck = true) const
		{
			if (false == withCheck)
			{
				return m_time;
			} 
			else
			{
				if (false == getIsValid())
				{
					std::string strError("Invalid mode begin time:");
					strError += m_time;
					TA_THROW(TA_Base_Core::TransactiveException(strError));
				}
			}
			return m_time;
		}

		bool getIsValid() const
		{
			return m_bIsValid;
		}
	private:
		bool checkTimeValid(const std::string& value)
		{
			bool bSuccess = false;
			do 
			{
				if (4 != value.size() || false == IsNumeric(value))
				{
					break;
				}
				if (value.compare("2359") > 0)
				{
					break;
				}
				bSuccess = true;
			} while (false);
			return bSuccess;
		}

		inline bool IsNumeric(const std::string& value)
		{
			for(size_t i = 0; i < value.length(); i++)
			{
				if(value[i] < '0' || value[i] > '9')
				{
					return false;
				}
			}
			return true;
		}
	private:
		std::string m_time;
		bool m_bIsValid;
	};

	class ModeTimeInfo
	{
	public:
		ModeTimeInfo()
		: pkey(0), modeKey(0), beginTime("")
		{
		}
		ModeTimeInfo(unsigned long key, unsigned long mode, const ModeTime& time)
		: pkey(key), modeKey(mode), beginTime(time)
		{
		}
	public:
		unsigned long pkey;
		unsigned long modeKey;
		ModeTime beginTime;
	};
	typedef std::map<unsigned long, ModeTimeInfo> ModeTimeInfoMap;
	typedef ModeTimeInfoMap::iterator ModeTimeInfoMapIterator;

	class BASTimeTableModeTimeAccessFactory
	{
	public:
		BASTimeTableModeTimeAccessFactory(void);
		~BASTimeTableModeTimeAccessFactory(void);
		static std::string getLocalDatabaseName();
		static std::string constructAQString(const std::string& strSqlString);
	public:
		ModeTimeInfo getModeTimeInfoByKey(unsigned long pkey);

		void getAllModeTimeInfoByKey(ModeTimeInfoMap& modeTimeMap, const std::vector<unsigned long>& modeTimeKeyVct);

		unsigned long addModeTime(unsigned long modeKey, ModeTime beginTime);

		void updateModeTime(unsigned long pkey, ModeTime newTime);

		void deleteModeTime(unsigned long pkey);

		void deleteModeTime(std::vector<unsigned long> pkeyVct);
	private:
		std::string getSqlInPart(std::vector<unsigned long> vctValue, bool withBrackets);
		unsigned long getNextPkey();
	public:
		static const std::string TableName;
		static const std::string ColumnName_Pkey;
		static const std::string ColumnName_ModeKey;
		static const std::string ColumnName_BeginTime;

		static std::string m_localDatabase;
	};
	typedef ACE_Singleton<BASTimeTableModeTimeAccessFactory,ACE_Recursive_Thread_Mutex> BASTimeTableModeTimeAccessFactoryInstance;

}

#endif //Transactive_3002_GF_Core_Bas_TimeTable_BASTimeTableModeTimeAccessFactory
