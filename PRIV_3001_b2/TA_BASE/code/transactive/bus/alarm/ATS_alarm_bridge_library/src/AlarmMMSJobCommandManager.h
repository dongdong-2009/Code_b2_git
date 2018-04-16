// AlarmMMSJobCommandManager.h: interface for the AlarmMMSJobCommandManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AlarmMMSJobCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_)
#define AFX_AlarmMMSJobCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "CacheDataManager.h"
#include "DataStructInfo.h"
 
namespace TA_Base_Bus
{
	class AlarmMMSJobCommandManager : public CacheDataManager<std::string,  I_ATS_ISCS_ALARM::AlarmMmsJobCommand>
	{
	public:
		AlarmMMSJobCommandManager(long maxBlockCount,long blockByteSize);
		virtual ~AlarmMMSJobCommandManager();	 
		
// 		virtual unsigned long readCachDataKeyFromMem(ShareMemoryHelper* mem, char* pos);
// 		virtual unsigned long getCacheDataKey(AlarmMmsJobCommand* cacheData);
		virtual bool writeCacheDataToMem(I_ATS_ISCS_ALARM::AlarmMmsJobCommand* cacheData,ShareMemoryHelper* mem, char* pos);
		virtual bool readCacheDataFromMem(I_ATS_ISCS_ALARM::AlarmMmsJobCommand* cacheData,ShareMemoryHelper* mem, char* pos);
// 		struct AlarmMmsJobCommand
// 		{
// 			std::string AssetName;
// 			std::string AlarmId;
// 			std::string OperatorId; // ��Requestor��
// 			unsigned long targetDate; // 0 if no target completion date.
// 			std::string problemReport;
// 		};

         //the following attribute should be updated based on the "information" related tables in database.
		enum AlarmMMSJobCommandAttributeSize
		{	
		 
			AlarmIDSize=I_ATS_ISCS_ALARM::ALARM_ID_SIZE+LongSharedMemoryData,			 
            AssetNameSize=I_ATS_ISCS_ALARM::ASSERT_NAME_SIZE+LongSharedMemoryData,		
			OperatorIdSize=I_ATS_ISCS_ALARM::OPERATOR_ID_SIZE+LongSharedMemoryData,	
            TargetDateSize=ULongSharedMemoryData,
			ProblemReportSize=I_ATS_ISCS_ALARM::PROBLEM_REPORT_SIZE+LongSharedMemoryData,			 
			TotalSizeLength=AlarmIDSize+AssetNameSize
			+ OperatorIdSize + TargetDateSize +ProblemReportSize,
			MaxItemCount=200
		};
	};
		
	 
}

#endif // !defined(AFX_AlarmMMSJobCommandManager_H__2BA8A561_4EB6_4F8C_A003_B1CC88A86160__INCLUDED_)
