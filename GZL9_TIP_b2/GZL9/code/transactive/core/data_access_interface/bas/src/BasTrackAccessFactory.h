#if !defined(Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTrackAccessFactory)
#define Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTrackAccessFactory
#pragma once

#include "core/data_access_interface/bas/src/CommonDefine.h"
namespace TA_IRS_Core
{
	class TrackInfo
	{
	public:
		bool isApproximateMatchTrack(const std::string& strTrackName);
		bool isAccurateMatchTrack(const std::string& strTrackName);
		static bool isTrackValid(const std::string& trackNum);
	public:
		unsigned long trackNumber;
		std::string trackName;
		unsigned long sectionKey;
		LineType lineType;
		TrainPositionByShaft firePosition;
	};
	typedef boost::shared_ptr<TrackInfo> TrackInfo_SmartPtr;
	typedef std::vector<TrackInfo_SmartPtr> TrackInfoVct;
	typedef TrackInfoVct::iterator TrackInfoVctIter;

	class BasTrackAccessFactory
	{
	public:
		BasTrackAccessFactory(void);
		~BasTrackAccessFactory(void);
	public:
		void getTrackInfos(TrackInfoVct& trackInfoVct);
	private:
		static const std::string TableName;
		static const std::string ColumnName_TrackNumber;
		static const std::string ColumnName_TrackName;
		static const std::string ColumnName_SectionKey;
		static const std::string ColumnName_UpDownBound;
		static const std::string ColumnName_PositionByShaft;
	};

	typedef ACE_Singleton<BasTrackAccessFactory,ACE_Recursive_Thread_Mutex> BasTrackAccessFactoryInstance;
}


#endif //Transactive_GF_3002_Core_Bas_TunnelModeManager_BasTrackAccessFactory
