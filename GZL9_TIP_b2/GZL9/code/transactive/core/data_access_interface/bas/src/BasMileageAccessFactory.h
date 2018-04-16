#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasMileageAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasMileageAccessFactory
#pragma once  

#include "ace/Singleton.h"
#include "core/data_access_interface/bas/src/CommonDefine.h"



namespace TA_IRS_Core
{
	class MileageInfo
	{
	public:
		bool isMatchMileage(const std::string& strMileage, bool leftLine);
		bool isMatchMileage(unsigned long mileage, bool leftLine);
		static std::string getMileageString(const std::string& strMileage);
		static unsigned long getMileageNumber(const std::string& strMileage);
		static LineType getLineTypeByString(const std::string& strMileage);
	private:
		unsigned long getBeginMileage();
		unsigned long getEndMileage();
	public:
		unsigned long mileageNumber;
		unsigned long sectionKey;
		LineType updownBound;
		unsigned long beginMileage;
		unsigned long endMileage;
		SegmentType segment;
		TrainPositionByShaft positionByShaft;
	};
	typedef boost::shared_ptr<MileageInfo> MileageInfo_Smart_Ptr;
	typedef std::vector<MileageInfo_Smart_Ptr> MileageInfoVct;
	typedef MileageInfoVct::iterator MileageInfoVctIter;

	class BasMileageAccessFactory
	{
	public:
		BasMileageAccessFactory(void);
		~BasMileageAccessFactory(void);
	public:
		void getAllMileageInfos(MileageInfoVct& mileageInfoVct);
	private:
		static const std::string TableName;
		static const std::string ColumnName_MileageNumber;
		static const std::string ColumnName_SectionKey;
		static const std::string ColumnName_UpDownBound;
		static const std::string ColumnName_BeginMileage;
		static const std::string ColumnName_EndMileage;
		static const std::string ColumnName_SegmentName;
		static const std::string ColumnName_PositionByShaft;

	};
    typedef ACE_Singleton<BasMileageAccessFactory,ACE_Recursive_Thread_Mutex> BasMileageAccessFactoryInstance;
}

#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasMileageAccessFactory
