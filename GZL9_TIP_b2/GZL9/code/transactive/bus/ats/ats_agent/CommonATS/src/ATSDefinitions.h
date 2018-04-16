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
	enum AtsMessageType { ePA, eTRAIN, ePIDS, ePIS, eUNKOWN, eSIG, eBLK, eFG, eESB, eSW, ePlatform, ePlatformDoor};

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
		bool InDepot;
		bool Arrival;
		bool PreArrival;
		bool Skip;
		bool hold;
	};

	struct PISInfo 
	{
		std::string PlatformId;
		std::string Train1st_ArrivalTime;
		bool isNeedUpdateTime;
	};

	struct PlatformInfo
	{
		std::string PlatformId;
		unsigned short Skip;
		unsigned short hold;
		bool isNeedUpdateSkip;
		bool isNeedUpdateHold;
	};

	struct PlatformDoorInfo
	{
		std::string PlatformDoorId;
		unsigned short	OverrideStatus;
		bool isNeedUpdateOverride;
	};


	struct TrainInfo
	{
		unsigned short TrainCongestion; //Train Congestion Status: 0=Congestion/1=Not Congestion/2=Invalid Value
		std::string TrainServiceNumber;
		std::string TrainDirection;
		std::string TrainTrackLocation;
		bool lastTrainRecord;
		bool shownTrainOnTrack; //the train has updated and been shown on track.
		std::string TrainID;
		unsigned short TrainDriveMode;
		unsigned short genaTrainDirection;
		bool isTrainPosLost;	//true - train position lost, false - train position is found
		std::string msgType; //birth, update, death, unknown
	};


	struct SignalInfo 
	{
		std::string		 SignalId;
		unsigned long	 Status;			//Signal Display
		unsigned long	 healthStatus;		//Signal Health Status
		bool			 isNeedUpdateStatus;
		bool			 isNeedUpdateMode;
	};

	struct BlkInfo 
	{
		std::string		BlockId;
		unsigned short	BlkStatus;	//Block Status
		bool isNeedUpdateBlkStatus;
	};

	struct SwtichInfo 
	{
		std::string		SwitchId;
		unsigned short Position;	//Switch Position
		unsigned short FailedStatus;//Switch Failed Status: 0=Normal/1=Disturbed/2=Invaild Value
		unsigned short ResStatus;	//Switch Reserved Status: 0=Reserved/1=Unreserved/2=Invalid Value
		bool isNeedUpdatePos;
		bool isNeedUpdateFailed;
		bool isNeedUpdateRes;
	};

	struct EsbInfo 
	{
		std::string		EsbId;
		unsigned short Status;	//Emergency Stop Button Status
		bool isNeedUpdateEsbStatus;
	};

	struct FloodGateInfo 
	{
		std::string	FloodGateId;
		unsigned short	RequestClose;		//Flood gate closed request signal£º0=request/1=not request/2=Invalid Value
		unsigned short	PermissionClose;	//Flood gate closed permission signal£º0=permission/1=not permission/2=Invalid Value
		bool	isNeedUpdateReq;
		bool	isNeedUpdatePermission;
	};

	struct ServerStatus 
	{
		std::string		 serverId;
		bool			 isOnline; /*true - online; false - offline*/
	};

	
	typedef std::vector<SignalInfo>  SignalInfoVec;
	typedef SignalInfoVec::iterator SignalInfoVecIt;

	typedef std::vector<BlkInfo>  BlkInfoVec;
	typedef BlkInfoVec::iterator BlkInfoVecIt;

	typedef std::vector<SwtichInfo>  SwtichInfoVec;
	typedef SwtichInfoVec::iterator SwtichInfoVecIt;

	typedef std::vector<EsbInfo>  EsbInfoVec;
	typedef EsbInfoVec::iterator EsbInfoVecIt;

	typedef std::vector<FloodGateInfo>  FloodGateInfoVec;
	typedef FloodGateInfoVec::iterator FloodGateInfoVecIt;

	typedef std::vector<PAStationInfo>  PAStationInfoVec;
	typedef PAStationInfoVec::iterator PAStationInfoVecIt;

	typedef std::vector<TrainInfo>  TrainInfoVec;
	typedef TrainInfoVec::iterator TrainInfoVecIt;

	typedef std::vector<PISInfo>  PISInfoVec;
	typedef PISInfoVec::iterator PISInfoVecIt;

	typedef std::vector<PlatformInfo>  PlatformInfoVec;
	typedef PlatformInfoVec::iterator PlatformInfoVecIt;

	typedef std::vector<PlatformDoorInfo>  PlatformDoorInfoVec;
	typedef PlatformDoorInfoVec::iterator PlatformDoorInfoVecIt;
	
	typedef std::string TrainServiceNumber;
	typedef std::string TrackName;

	typedef std::vector<TrainServiceNumber> ActiveTrainServNum;
	typedef ActiveTrainServNum::iterator ActiveTrainServNumIt;

	typedef std::map<TrainServiceNumber,TrainInfo*> TrainInfoMap;
	typedef TrainInfoMap::iterator TrainInfoMapIt;

	typedef std::vector<TrackName> TrackInfo;
	typedef TrackInfo::iterator TrackInfoIt;

	typedef std::string TrainID;

	typedef std::vector<TrainID> ActiveTrainID;
	typedef ActiveTrainID::iterator ActiveTrainIDIt;

	typedef std::map<TrainID,TrainInfo*> IDBaseTrainInfoMap;
	typedef IDBaseTrainInfoMap::iterator IDBaseTrainInfoMapIt;

	typedef double EdgeID;

	struct BranchInfo
	{ 
		std::string branchName;
		unsigned long minABSCISSA;
		unsigned long maxABSCISSA;
	};

	typedef std::vector <BranchInfo> BranchInfoList;
	typedef BranchInfoList::iterator BranchInfoListItr;

	typedef std::map<EdgeID, BranchInfoList> BranchMap;
	typedef BranchMap::iterator BranchMapItr;

	//The Information below is Constant value based on ICD Requirement.
	//Constant value for Power Status
	const int POWER_STATUS_SIZE     = 16;  //Power Status size in Bytes
	const std::string EMPTY_VAL     = ""; //Empty value for Datapoint

	//eTrainServiceNumFull - update all train info at one time
	//eTrainIDSingle - update train info one by one
	enum TrainDataType { eTrainServiceNumFull, eTrainIDSingle};

	enum PowerStatus
	{
		ENERGIZED	= 0x01,
		DEENERGIZED	= 0x02,
		UNKNOWN		= 0x03
	};

	typedef std::map<unsigned short, std::string> PowerStatusMap;
	typedef PowerStatusMap::iterator PowerStatusMapIt;

	#define BOOLEAN_TYPE_INVALID_VALUE	2 //Use to check if received an invalid no., if it is an invalid no. set datapoint quality to bad

}
#endif