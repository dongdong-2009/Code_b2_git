
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaTrainTableParser.h"
#include "app/ats/AtsAgent/GenaProtocolHandler/src/GenaAtsDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsHelper.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/types/public_types/Constants.h"
#include "boost/algorithm/string.hpp"


namespace TA_IRS_App
{
	const char * DriveMode[] = 
	{
		"OFF",	// standby
		"RMR",	// manual reverse
		"RMF25",// manual reverse 25 
		"RMF60",// manual forward 60 
		"IATP", // ...
		"ATPM", // manual ATP mode 
		"ATO",	// automation
		"WM",	// washing mode
		"DTM",	// ...
		"UNKNOWN"// unknown
	};
	enum TrainDriveModeEnum {OFF=0,RMR,RMF25,RMF60,IATP, ATPM, ATO, WM, DTM, DRIVEMODEUNKONW};
	#define TrainDriveModeLength	10

	const char * TrainDirection [] =
	{
		"UPTRACK",
		"DOWNTRACK",
		"UNKNOWN"
	};
	enum TrainDirectionEnum { UPTRACK=0,DOWNTRACK,  DIRUNKNOWN};
	#define TrainDirLength	3

	const char * Congestion[] = 
	{
		"TRUE",		// standby
		"FALSE"	// manual reverse
	};
	enum TrainCongestionEnum {NOTCONGEST=0,CONGEST,CONGESTIONUNKONW};
	#define TrainCongestionLength	2

	GenaTrainTableParser::GenaTrainTableParser()
		//: BaseTableParser(EntityData),
		 // m_numberOfTrains(0)
	{
		FUNCTION_ENTRY("GenaTrainTableParser");

		m_BranchMap = TA_IRS_Bus::AtsHelper::getAllBranchInformation();

		FUNCTION_EXIT;
	}

	GenaTrainTableParser::~GenaTrainTableParser()
	{
		FUNCTION_ENTRY("~GenaTrainTableParser");
		m_BranchMap.clear();
		FUNCTION_EXIT;
	}

	void GenaTrainTableParser::parseGenaAtsTable(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("parseAtsTable");

		m_trainInfoVec.clear();

		if(rawTable.getTopicData() != NULL)
		{
			if(boost::to_upper_copy(rawTable.getTopicData()->getTopicName())=="TRAIN")
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start parse Ats Table %s message", rawTable.getTopicData()->getUpdateType().c_str());
				importRecord(rawTable);
			}
		}
		
		//NOTE: If the Server Status Quality is BAD, it means there is no Data to be processed 
		// instead only DataPoints are need to be updated into Bad Quality State.

		//Check if received train Info data or data point bad quality message, 
		//and then send notification to related processor
		if((!m_trainInfoVec.empty())|| (rawTable.getDataPointQuality() == TA_Base_Core::ITA_QUALITY_BAD))
		{
			TA_IRS_Bus::TrainDataNotificationPtr trainDataPtr(new TA_IRS_Bus::TrainDataNotification(&m_trainInfoVec));
			//Set Train Info Data Quality
			trainDataPtr->setDataQuality(rawTable.getDataPointQuality());
			//Notify AtsData Event
			m_atsNotification->notifyAtsDataEvent(trainDataPtr);
		}	
		FUNCTION_EXIT;
	}

	void GenaTrainTableParser::importRecord(TA_IRS_App::GenaRawTable& rawTable)
	{
		FUNCTION_ENTRY("importRecord");

		bool isFound = false;
		TA_Base_Bus::GENATopicDataPtr TrainData = rawTable.getTopicData();

		TA_IRS_Bus::TrainInfo newRecord;
		//To give train info new record an init value
		constructEmptyValue(newRecord);

		//Get old record from data cache by train ID
		TA_IRS_Bus::TrainInfo oldRecord;
		
		std::string strTrainID = TrainData->getObjectID();
		oldRecord = TA_IRS_Bus::DataCache::getInstance()->getTrainLocationData(strTrainID);

		//copy the value from old record
		//if(oldRecord != NULL)
		{
			newRecord = oldRecord;
		}

		newRecord.TrainID = TrainData->getObjectID();

		newRecord.msgType = rawTable.getTopicData()->getUpdateType();

		//Update value get from Gena client
		//Congestion
		std::string tmpData;
		tmpData.clear();
		tmpData = TrainData->getAttributeValue(TRAINCONGESTION);
		if(tmpData.compare(""))
		{			
			signed short congestion = convertStrToEmun(Congestion,tmpData, TrainCongestionLength);
			if(congestion != -1)
			{
				newRecord.TrainCongestion = congestion;
			}
			else
			{
				newRecord.TrainCongestion = CONGESTIONUNKONW;
			}
		}
		//Direction
		tmpData.clear();
		tmpData = TrainData->getAttributeValue(TRAINDIRECTION);
		if(tmpData.compare(""))
		{			
			signed short direction = convertStrToEmun(TrainDirection,tmpData, TrainDirLength);
			newRecord.genaTrainDirection = direction;
		}
		//Service Number
		tmpData.clear();
		tmpData = TrainData->getAttributeValue(TRAINSERVICENUM);
		if(tmpData.compare(""))
		{			
			newRecord.TrainServiceNumber = tmpData;
		}
		//Drive Mode
		tmpData.clear();
		tmpData = TrainData->getAttributeValue(DRIVEMODE);
		if(tmpData.compare(""))
		{			
			signed short driveMode = convertStrToEmun(DriveMode,tmpData, TrainDriveModeLength);
			newRecord.TrainDriveMode = driveMode;
		}

		findBranchID(TrainData, newRecord);

		m_trainInfoVec.push_back(newRecord);
		
		FUNCTION_EXIT;
	}


	void GenaTrainTableParser::constructEmptyValue(TA_IRS_Bus::TrainInfo& emptyRecord)
	{
		FUNCTION_ENTRY("constructEmptyValue");

		emptyRecord.TrainID				= TA_IRS_Bus::EMPTY_VAL;
		emptyRecord.TrainServiceNumber	= " ";
		emptyRecord.TrainDirection		= TA_IRS_Bus::EMPTY_VAL;
		emptyRecord.TrainTrackLocation	= TA_IRS_Bus::EMPTY_VAL;
		emptyRecord.TrainCongestion		= CONGESTIONUNKONW;
		emptyRecord.lastTrainRecord		= false;
		emptyRecord.shownTrainOnTrack	= false;
		emptyRecord.TrainDriveMode		= DRIVEMODEUNKONW;
		emptyRecord.genaTrainDirection	= DIRUNKNOWN;
		emptyRecord.msgType				= TA_IRS_Bus::EMPTY_VAL;
		emptyRecord.isTrainPosLost		= false;

		FUNCTION_EXIT;
	}

	void GenaTrainTableParser :: findBranchID(TA_Base_Bus::GENATopicDataPtr TrainData, TA_IRS_Bus::TrainInfo& newRecord)
	{
		FUNCTION_ENTRY("findBranchID");

		long branchOffset, edgeID;
		branchOffset = -1;
		edgeID = -1;
		std::string tmpData = TrainData->getAttributeValue(TRAINEDGEID);
		if(tmpData.compare(""))
		{	
			edgeID = atol(tmpData.c_str());
		}

		tmpData.clear();
		tmpData = TrainData->getAttributeValue(BRANCHOFFSET);
		if(tmpData.compare(""))
		{
			branchOffset = atol(tmpData.c_str());
		}

		if((0 == edgeID)&&(0 == branchOffset))
		{
			newRecord.isTrainPosLost = true;
		}		
		else
		{
			TA_IRS_Bus::BranchMapItr itr = m_BranchMap.find(edgeID);

			if(itr != m_BranchMap.end())
			{
				TA_IRS_Bus::BranchInfoList branchList = itr->second;

				TA_IRS_Bus::BranchInfoListItr it = branchList.begin();
				for(it ; it != branchList.end(); ++it)
				{
					TA_IRS_Bus::BranchInfo branchInfo = (*it);
					if((branchOffset >= branchInfo.minABSCISSA) && (branchOffset <= branchInfo.maxABSCISSA))
					{
						newRecord.TrainTrackLocation = branchInfo.branchName;
						return;
					}
				}
			}
		}
		FUNCTION_EXIT;
	}

}