/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\ATSDefinitions.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This represents the ATS Definition Constants
  */

#ifndef _ATS_DEFINITIONS_H_
#define _ATS_DEFINITIONS_H_

#include "bus/ats/ats_agent/CommonATS/src/AtsPasData.h"
#include "bus/ats/ats_agent/CommonATS/src/IProtocolParser.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataDispatcher.h"
#include "bus/ats/ats_agent/CommonATS/src/RawTable.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataSync.h"
#include "bus/ats/ats_agent/CommonATS/src/PowerData.h"
#include "bus/ats/ats_agent/CommonATS/src/DataProcessorThread.h"
#include <map>
#include <string>
#include <vector>

namespace TA_IRS_Bus
{
	enum AtsMessageType { ePA, eTRAIN, ePIDS, ePIS, eUNKOWN };
	typedef std::map<unsigned short, TA_IRS_Bus::PowerData*> PowerDataMap;
	typedef PowerDataMap::iterator PowerDataMapIt;

	typedef std::map<TA_IRS_Bus::PlatformIdType, TA_IRS_Bus::AtsPasData*> PasDataMap;
	typedef PasDataMap::iterator PasDataMapIt;

	typedef	std::vector<IDataDispatcher*> DispatcherVec;
	typedef DispatcherVec::iterator DispatchVecIt;

	typedef std::map<AtsMessageType, IProtocolParser*> ParserProcessorMap;
	typedef ParserProcessorMap::iterator ParserProcessorMapIter;

	typedef std::map<AtsMessageType, IDataDispatcher*> DispatcherProcessorMap;
	typedef DispatcherProcessorMap::iterator DispatcherProcessorMapIter;

	typedef std::map<AtsMessageType, IDataSync*> DataSyncProcessorMap;
	typedef DataSyncProcessorMap::iterator DataSyncProcessorMapIter;

	typedef std::map<AtsMessageType, DataProcessorThread*> AtsDataProcessorMap;
	typedef AtsDataProcessorMap::iterator AtsDataProcessorMapIter;

	struct PAStationInfo
	{
		TA_IRS_Bus::PlatformIdType PlatformId;
		TA_IRS_Bus::StationIdType DestinationStationId;
		bool Skip;
		bool InDepot;
		bool Arrival;
		bool PreArrival;
	};

	struct TrainInfo
	{
		bool TrainCongestion;
		std::string TrainServiceNumber;
		std::string TrainDirection;
		std::string TrainTrackLocation;
		bool lastTrainRecord;
		bool shownTrainOnTrack; //the train has updated and been shown on track.
	};

	struct PISInfo 
	{
		unsigned long PlatformId;
		unsigned long Train1st_ArrivalTime;
	};

	typedef std::vector<PAStationInfo>  PAStationInfoVec;
	typedef PAStationInfoVec::iterator PAStationInfoVecIt;

	typedef std::vector<TrainInfo>  TrainInfoVec;
	typedef TrainInfoVec::iterator TrainInfoVecIt;

	typedef std::vector<PISInfo>  PISInfoVec;
	typedef PISInfoVec::iterator PISInfoVecIt;

	typedef std::string TrainServiceNumber;
	typedef std::string TrackName;

	typedef std::vector<TrainServiceNumber> ActiveTrainServNum;
	typedef ActiveTrainServNum::iterator ActiveTrainServNumIt;

	typedef std::map<TrainServiceNumber,TrainInfo*> TrainInfoMap;
	typedef TrainInfoMap::iterator TrainInfoMapIt;

	typedef std::vector<TrackName> TrackInfo;
	typedef TrackInfo::iterator TrackInfoIt;

	//The Information below is Constant value based on ICD Requirement.
	//Constant value for Power Status
	const int POWER_STATUS_SIZE     = 16;  //Power Status size in Bytes
	const std::string EMPTY_VAL     = ""; //Empty value for Datapoint

}
#endif