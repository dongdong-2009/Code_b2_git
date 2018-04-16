#if !defined(Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerDataHelper)
#define Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerDataHelper
#pragma once

#include "ace/Singleton.h"
#include "app/bas/TunnelModeManager/src/CommonDefine.h"
#include "core/data_access_interface/bas/src/BasTunnelCongestionModeAccessFactory.h"
#include "core/data_access_interface/bas/src/BasTunnelFireModeAccessFactory.h"
#include "core/data_access_interface/bas/src/BasStationSectionAccessFactory.h"
#include "core/data_access_interface/bas/src/ModeAccessFactory.h"
#include "core/data_access_interface/bas/src/BasTrackAccessFactory.h"
#include "core/data_access_interface/bas/src/BasTunnelFireCaseAccessFactory.h"
namespace TA_IRS_App
{
	class TunnelModeManagerDataHelper
	{
	public:
		TunnelModeManagerDataHelper(void);
		~TunnelModeManagerDataHelper(void);
	public:

		TA_IRS_Core::StationSectionInfo getStationSectionByKey(unsigned long sectionKey);

		std::vector<TA_IRS_Core::StationSectionInfo> getAllStationSections();

		TA_IRS_Core::TunnelFireCaseInfoVct getAllFireCases();

		TA_IRS_Core::CongestionModeInfoVct getCongestionModesByTrackNumber(unsigned long trackNumber);

		TA_IRS_Core::CongestionModeInfoVct getCongestionModesByTrackNameAndSectionAndLineType( const std::string& strTrackName, unsigned long sectionKey, TA_IRS_Core::LineType lineType);

		TA_IRS_Core::TunnelFireModeInfoVct getFireModesByTrackNumber(unsigned long trackNumber);

		TA_IRS_Core::TunnelFireModeInfoVct getFireModesBySelectedCondition( const std::string& strTrackName, unsigned long sectionKey, TA_IRS_Core::LineType lineType, TA_IRS_Core::TrainPositionByShaft positionByShaft, unsigned long fireCaseKey );

		TA_IRS_Core::Mode_SmartPtr getModeByKey(unsigned long locationKey, unsigned long modeKey);

		void getTrackInfosByTrackName(const std::string& strTrackName, TA_IRS_Core::TrackInfoVct& approxiTrackInfoVct, TA_IRS_Core::TrackInfoVct& accuTrackInfoVct);

		TA_IRS_Core::TrackInfoVct getTrackInfosByTrackName(const std::string& strTrackName);

	private:
		std::vector<TA_IRS_Core::StationSectionInfo> m_stationSections; // the section infos
		std::map<unsigned long, TA_IRS_Core::CongestionModeInfoVct> m_congestionModes; // the all congestion mode infos
		std::map<unsigned long, TA_IRS_Core::TunnelFireModeInfoVct> m_fireModes; // the all congestion mode infos
		TA_IRS_Core::TrackInfoVct m_trackInfos;
		TA_IRS_Core::TunnelFireCaseInfoVct m_fireCaseInfos;
		std::map<unsigned long, std::vector<TA_IRS_Core::Mode_SmartPtr>> m_allModes;
	};
    typedef ACE_Singleton<TunnelModeManagerDataHelper,ACE_Recursive_Thread_Mutex> TunnelModeManagerDataHelperInstance;
}


#endif //Transactive_3002_GF_App_Bas_TunnelModeManager_TunnelModeManagerDataHelper