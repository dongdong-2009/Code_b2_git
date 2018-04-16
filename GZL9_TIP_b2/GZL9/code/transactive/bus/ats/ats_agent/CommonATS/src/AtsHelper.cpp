/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\AgentOperationMode.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * A object helper class responsible for printing raw data logs and converting int to HexStr.
  */

#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/algorithm/string.hpp"
#include <sstream>
#include "core/data_access_interface/ats/src/IAtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/AtsFixedBlockMapAccessFactory.h"
#include <sstream>
#include <iomanip>
#include <ctime>
namespace TA_IRS_Bus
{
	AtsHelper::AtsHelper()
	{
		FUNCTION_ENTRY("AtsHelper");
		FUNCTION_EXIT;
	}

	AtsHelper::~AtsHelper()
	{
		FUNCTION_ENTRY("~AtsHelper");
		FUNCTION_EXIT;
	}

	std::string AtsHelper::IntToStr(int val)
	{
		FUNCTION_ENTRY("IntToStr");

		std::stringstream ss;
		ss << val;

		FUNCTION_EXIT;
		return ss.str();
	}

	std::string AtsHelper::IntToHexStr(int val)
	{
		FUNCTION_ENTRY("intToStr");

		std::stringstream ss;
		ss << std::hex << val;
		std::string _ret = ss.str();
		boost::to_upper(_ret);

		FUNCTION_EXIT;
		return _ret;
	}

	TrackInfo AtsHelper::getAllTrackInformation()
	{
		FUNCTION_ENTRY("getAllTrackInformation");

		TrackInfo infoTrack;

		TA_IRS_Core::IAtsFbMapRecords fbMapRecord = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAllAtsFbMapRecords();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"DB Section Record Count : %d",fbMapRecord.size());

		for (int nCounter = 0; nCounter < fbMapRecord.size(); nCounter++)
		{
			std::string strTrackName = fbMapRecord.at (nCounter)->getBranchName();
			boost::to_upper(strTrackName);
			infoTrack.push_back(strTrackName);
		}

		FUNCTION_EXIT;
		return infoTrack;
	}

	BranchMap AtsHelper::getAllBranchInformation()
	{
		FUNCTION_ENTRY("getAllTrackInformation");

		BranchMap infoBranch;

		TA_IRS_Core::IAtsFbMapRecords fbMapRecord = TA_IRS_Core::AtsFixedBlockMapAccessFactory::getInstance().getAllAtsFbMapRecords();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"DB Section Record Count : %d",fbMapRecord.size());

		for (int nCounter = 0; nCounter < fbMapRecord.size(); nCounter++)
		{
			unsigned long edgeID = fbMapRecord.at (nCounter)->getBranchId();
			BranchInfo trackInfo;
			trackInfo.branchName = fbMapRecord.at (nCounter)->getBranchName();
			boost::to_upper(trackInfo.branchName);
			trackInfo.minABSCISSA = fbMapRecord.at (nCounter)->getMinAbscissa();
			trackInfo.maxABSCISSA = fbMapRecord.at (nCounter)->getMaxAbscissa();

			BranchMap::iterator itr = infoBranch.find(edgeID);
			if(itr == infoBranch.end())
			{
				BranchInfoList branchlist;
				branchlist.push_back(trackInfo);
				infoBranch[edgeID] = branchlist;
			}
			else
			{
				BranchInfoList& branchlist =(*itr).second;
				branchlist.push_back(trackInfo);
			}
		}

		FUNCTION_EXIT;
		return infoBranch;
	}

	void AtsHelper::printRawData(const std::vector<unsigned char>& rawdata, const std::string& sourceMsg)
	{
		if (TA_Base_Core::DebugUtil::DebugDebug <= TA_Base_Core::DebugUtil::getInstance().getLevel())
		{
			std::ostringstream packetData;
			packetData << "Source:" << sourceMsg <<", Size = " << rawdata.size() <<" Bytes, Hex Raw Data= ";

			std::vector<unsigned char>::const_iterator iter = rawdata.begin();
			for(iter; iter != rawdata.end(); iter++)
			{
				packetData << std::setw(2) << std::setfill('0')<< std::hex << std::uppercase << static_cast<int>(*iter) <<" ";
			}

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,packetData.str().c_str() );
		}	
	}

	void AtsHelper::printRawData(const TA_Base_Bus::DataBlock<WORD>& rawData, const std::string& sourceMsg)
	{
		std::vector<unsigned char> data;
		for (int nLoop = rawData.start(); nLoop <= rawData.end(); nLoop++)
		{
 			unsigned short rawValue = rawData[nLoop];
			unsigned char lowByte = rawValue & 0xff;         //For Low Byte
			unsigned char highByte = (rawValue >> 8) & 0xff; //For High Byte
			//For logging purposes it should be put by character.
			data.push_back(highByte);
			data.push_back(lowByte);
		}

		printRawData(data,sourceMsg);
	}

	std::string AtsHelper::getLocalTimeFormat(unsigned long arrivalTime)
	{
		//Convert UTC time to local Time
		time_t _UTCTime = arrivalTime;
		struct tm* ltm = localtime(&_UTCTime);
		char timeBuffer [10];

		strftime(timeBuffer,10,"%H:%M:%S",ltm);
		std::string strTime(timeBuffer);

		return strTime;
	}

	bool AtsHelper::isGoodDatapointQuality(TA_Base_Core::EDataPointQuality quality)
	{
		FUNCTION_ENTRY("isGoodDatapointQuality");

		bool _datapointQuality;
		if ( TA_Base_Core::ITA_QUALITY_GOOD == quality || 
			TA_Base_Core::ITA_QUALITY_GOOD_LOCAL_OVERRIDE == quality )
		{
			_datapointQuality = true;
		}
		else
			_datapointQuality = false;

		FUNCTION_EXIT;
		return _datapointQuality;	
	}
}