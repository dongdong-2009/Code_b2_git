#include "StdAfx.h"
#include "app/bas/TunnelModeManager/src/TunnelModeManagerDataHelper.h"

namespace TA_IRS_App
{
	TunnelModeManagerDataHelper::TunnelModeManagerDataHelper(void)
	{
	}

	TunnelModeManagerDataHelper::~TunnelModeManagerDataHelper(void)
	{
	}

	TA_IRS_Core::StationSectionInfo TunnelModeManagerDataHelper::getStationSectionByKey(unsigned long sectionKey)
	{
		FUNCTION_ENTRY("getStationSectionByKey");
		if (m_stationSections.empty())
		{
			getAllStationSections();
		}

		TA_IRS_Core::StationSectionInfo info;
		std::vector<TA_IRS_Core::StationSectionInfo>::iterator it;
		for (it = m_stationSections.begin(); it != m_stationSections.end(); it++)
		{
			if ((*it).pkey == sectionKey)
			{
				info = (*it);
				break;
			}
		}

		if (info.strName.empty())
		{
			TA_THROW(TA_Base_Core::TransactiveException("Invalid Station Section Key"));
		}
		FUNCTION_EXIT;
		return info;
	}

	std::vector<TA_IRS_Core::StationSectionInfo> TunnelModeManagerDataHelper::getAllStationSections()
	{
		FUNCTION_ENTRY("getAllStationSections");
		if (m_stationSections.empty())
		{
			TA_IRS_Core::BasStationSectionAccessFactoryInstance::instance()->getStationSectionInfo(m_stationSections);
		}

		FUNCTION_EXIT;
		return m_stationSections;
	}

	TA_IRS_Core::CongestionModeInfoVct TunnelModeManagerDataHelper::getCongestionModesByTrackNumber(unsigned long trackNumber)
	{
		FUNCTION_ENTRY("getCongestionModesByTrackNumber");
		TA_IRS_Core::CongestionModeInfoVct modeVct;
		modeVct.clear();
		if (m_congestionModes.end() == m_congestionModes.find(trackNumber))
		{
			TA_IRS_Core::BasTunnelCongestionModeAccessFactoryInstance::instance()->getCongestionModes(trackNumber, modeVct);
			m_congestionModes.insert(std::make_pair(trackNumber, modeVct));
		}
		else
		{
			modeVct = m_congestionModes[trackNumber];
		}
		FUNCTION_EXIT;
		return modeVct;
	}

	TA_IRS_Core::CongestionModeInfoVct TunnelModeManagerDataHelper::getCongestionModesByTrackNameAndSectionAndLineType( const std::string& strTrackName, unsigned long sectionKey, TA_IRS_Core::LineType lineType)
	{
		FUNCTION_ENTRY("getCongestionModesByTrackNameAndSectionAndLineType");

		TA_IRS_Core::TrackInfoVct trackInfosVct;
        trackInfosVct = getTrackInfosByTrackName(strTrackName);

		TA_IRS_Core::TrackInfoVct::iterator itTrackInfo;
		TA_IRS_Core::CongestionModeInfoVct modeVct;
		TA_IRS_Core::CongestionModeInfoVct modeVctTemp;
		for (itTrackInfo = trackInfosVct.begin(); itTrackInfo != trackInfosVct.end(); ++itTrackInfo)
		{
			if((*itTrackInfo)->sectionKey == sectionKey && (*itTrackInfo)->lineType == lineType)
			{
				modeVctTemp = getCongestionModesByTrackNumber((*itTrackInfo)->trackNumber);
				if(modeVctTemp.empty())
				{
				   continue;
				}

                modeVct.insert(modeVct.end(),modeVctTemp.begin(),modeVctTemp.end());
				modeVctTemp.clear();
		    }
		}
		
		FUNCTION_EXIT;
		return modeVct;
	}

	TA_IRS_Core::TunnelFireModeInfoVct TunnelModeManagerDataHelper::getFireModesByTrackNumber(unsigned long trackNumber)
	{
		FUNCTION_ENTRY("getFireModesBySection");
		TA_IRS_Core::TunnelFireModeInfoVct modeVct;
		modeVct.clear();

		std::map<unsigned long, TA_IRS_Core::TunnelFireModeInfoVct>::iterator itFireMode = m_fireModes.find(trackNumber);
		if (m_fireModes.end() == itFireMode)
		{
			TA_IRS_Core::BasTunnelFireModeAccessFactoryInstance::instance()->getFireModes(trackNumber, modeVct);				
			m_fireModes.insert(std::make_pair(trackNumber, modeVct));
		}
		else
		{
			modeVct = itFireMode->second;
		}
		FUNCTION_EXIT;
		return modeVct;
	}

	TA_IRS_Core::TunnelFireModeInfoVct TunnelModeManagerDataHelper::getFireModesBySelectedCondition(const std::string& strTrackName, unsigned long sectionKey, TA_IRS_Core::LineType lineType,TA_IRS_Core::TrainPositionByShaft positionByShaft, unsigned long fireCaseKey)
	{
		FUNCTION_ENTRY("getFireModesBySelectedCondition");

		TA_IRS_Core::TrackInfoVct trackInfosVct;
		trackInfosVct = getTrackInfosByTrackName(strTrackName);

		TA_IRS_Core::TrackInfoVct::iterator itTrackInfo;
		TA_IRS_Core::TunnelFireModeInfoVct modeVct;
		TA_IRS_Core::TunnelFireModeInfoVct modeVctTemp;
        
		//Judge whether to filter fire case
		bool isFilterFireCase = false;
		for (TA_IRS_Core::TunnelFireCaseInfoVctIter itFireCaseInfo = m_fireCaseInfos.begin(); itFireCaseInfo != m_fireCaseInfos.end(); itFireCaseInfo++)
		{
			if(itFireCaseInfo->pkey == fireCaseKey)
			{
				isFilterFireCase = true;
				break;
			}
		}
		
		bool isSearchByShaft = (TA_IRS_Core::enum_TrainPos_Any == positionByShaft) ? false : true;
		bool bJudgeSearchCondition = false;

		for (itTrackInfo = trackInfosVct.begin(); itTrackInfo != trackInfosVct.end(); ++itTrackInfo)
		{
			bJudgeSearchCondition = (*itTrackInfo)->sectionKey == sectionKey && ((*itTrackInfo)->lineType == lineType);
			if(isSearchByShaft)
			{
				bJudgeSearchCondition = bJudgeSearchCondition && ((*itTrackInfo)->firePosition == positionByShaft);
			}
			if( bJudgeSearchCondition )
			{
				modeVctTemp = getFireModesByTrackNumber((*itTrackInfo)->trackNumber);
				if(modeVctTemp.empty())
				{
					continue;
				}

				for(TA_IRS_Core::TunnelFireModeInfoVct::iterator itFireMode = modeVctTemp.begin(); itFireMode != modeVctTemp.end(); ++itFireMode )
				{
					if( (*itFireMode)->FireCaseKey != fireCaseKey && isFilterFireCase )
					{
						continue;
					}
					modeVct.push_back(*itFireMode);
				}
				modeVctTemp.clear();
			}
		}

		FUNCTION_EXIT;
		return modeVct;
	}

	TA_IRS_Core::Mode_SmartPtr TunnelModeManagerDataHelper::getModeByKey(unsigned long locationKey, unsigned long modeKey)
	{
		FUNCTION_ENTRY("getModeByKey");
		using namespace TA_IRS_Core;
		std::vector<Mode_SmartPtr> modes;
		std::map<unsigned long, std::vector<TA_IRS_Core::Mode_SmartPtr>>::iterator itFoundMode = m_allModes.find(locationKey);
		if (m_allModes.end() == itFoundMode)
		{
			TA_IRS_Core::ModeAccessFactory::getInstance().getModes(locationKey, modes);
			m_allModes.insert(std::make_pair(locationKey, modes));
		}
		else
		{
			modes = itFoundMode->second;
		}
		
		TA_IRS_Core::Mode_SmartPtr modePtr;
		for(std::vector<Mode_SmartPtr>::iterator itMode = modes.begin(); itMode != modes.end(); itMode++)
		{
			if ((*itMode)->getKey() == modeKey)
			{
				modePtr = (*itMode);
				break;
			}
		}

		if (modePtr.get() == NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Not found the mode info. modeKey=%d", modeKey);

            FUNCTION_EXIT;
            TA_THROW(TA_Base_Core::TransactiveException("Not found the mode info."));
		}

		FUNCTION_EXIT;
		return modePtr;
	}

	void TunnelModeManagerDataHelper::getTrackInfosByTrackName(const std::string& strTrackName, TA_IRS_Core::TrackInfoVct& approxiTrackInfoVct, TA_IRS_Core::TrackInfoVct& accuTrackInfoVct)
	{
		FUNCTION_ENTRY("getTrackInfosByTrackName");
		if (m_trackInfos.empty())
		{
			TA_IRS_Core::BasTrackAccessFactoryInstance::instance()->getTrackInfos(m_trackInfos);		
		}

		for (TA_IRS_Core::TrackInfoVctIter it = m_trackInfos.begin(); it != m_trackInfos.end(); it++)
		{
			if ((*it)->isApproximateMatchTrack(strTrackName))
			{
				approxiTrackInfoVct.push_back(*it);
			}
			if ((*it)->isAccurateMatchTrack(strTrackName))
			{
				accuTrackInfoVct.push_back(*it);
			}
		}
		
		FUNCTION_EXIT;
	}

	TA_IRS_Core::TrackInfoVct TunnelModeManagerDataHelper::getTrackInfosByTrackName(const std::string& strTrackName)
	{
		FUNCTION_ENTRY("getTrackInfosByTrackName");
		if (m_trackInfos.empty())
		{
			TA_IRS_Core::BasTrackAccessFactoryInstance::instance()->getTrackInfos(m_trackInfos);			
		}

		TA_IRS_Core::TrackInfoVct trackInfosVct;
		for (TA_IRS_Core::TrackInfoVctIter it = m_trackInfos.begin(); it != m_trackInfos.end(); it++)
		{
			if ((*it)->isApproximateMatchTrack(strTrackName))
			{
				trackInfosVct.push_back(*it);
			}
		}

		FUNCTION_EXIT;
		return trackInfosVct;
	}

    TA_IRS_Core::TunnelFireCaseInfoVct TunnelModeManagerDataHelper::getAllFireCases()
	{
		FUNCTION_ENTRY("getAllFireCases");
		if (m_fireCaseInfos.empty())
		{
			TA_IRS_Core::BasTunnelFireCaseAccessFactoryInstance::instance()->getAllFireCaseInfo(m_fireCaseInfos);		
		}

		FUNCTION_EXIT;
		return m_fireCaseInfos;
	}
}
