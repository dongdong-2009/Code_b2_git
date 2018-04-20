// SharedAlarm.cpp : Defines the entry point for the DLL application.
//

#define TIME_OUT_MILLISECONDS 1000
#define ACE_NTRACE 0
#define ACE_NDEBUG 0
#define ACE_NLOGGING 0


#include "Windows.h"
#include "core/alarm/src/AlarmItem.h"
#include <crtdbg.h>

#include "core/synchronisation/src/ProcessGuard.h"
#include "core/synchronisation/src/NonReEntrantProcessLockable.h"
#include "core/synchronisation/src/IProcessLockable.h"

#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
#include "bus/alarm/shared_alarm/src/alarmIndices.h"
#include "core/utilities/src/RunParams.h"
#include "ActionFlagGuard.h"
#include "bus/alarm/alarm_store_library/src/AlarmFilterSet.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"

#include "bus/alarm/shared_alarm/src/SharedAlarmMonitor.h"

#include "ProcessGuard2.h"
#include "NonReEntrantProcessLockable2.h"
#include "IProcessLockable2.h"

#include "ProcessRWGuard.h"
#include "ace/Global_Macros.h"
#include "NonReEntrantProcessRWLockable.h"
#include "IProcessRWLockable.h"


#include <ctime>
#include <sstream>
#include <boost/bind.hpp>

const std::string AlarmIndexWithDuty_Name = "shared alarmIndexWithDuty";
const std::string AllAlarmIndex_Name = "all index";
const std::string AlarmInfo_Name = "shared alarm info";
const std::string AlarmIndexWithFilter_Name = "Alarm Index Filter";

typedef struct FilteredIndex
{ 
	FilteredIndex(): filterPtr(0), index(0),filteredIndexUnAckCount(0){}
	TA_Base_Bus::AlarmFilterSet* filterPtr;
	AlarmIndex* index;
	unsigned long filteredIndexUnAckCount;
}FilteredIndex;

//===== Shared Segment data replacement=====// 
typedef struct SharedAlarmInfo
{
	char sc_signal[CLIENTS_NUM][2][64];
	char map_signal[CLIENTS_NUM][2][64];

	unsigned short int mappingClients;
	unsigned short int clients;
	unsigned long gDutyAlarmCounter;
	unsigned int gCounter;
	unsigned int gCounterError;
	//++ Added to improve performance of alarm sound // updated when new alarm is inserted
	unsigned long gNewDutyAlarmComingVersion;

	//++ Added for unacknowledged count
	unsigned long gDutyIndexUnAckCount;
	bool  g_Updating;

	SharedAlarmInfo(): 
		g_Updating(false), 
		gDutyAlarmCounter(0), 
		clients(0), 
		mappingClients(0), 
		gCounter(0), 
		gCounterError(0),
		gDutyIndexUnAckCount(0),
		gNewDutyAlarmComingVersion(0)
	{}
	SharedAlarmInfo& operator = (const SharedAlarmInfo& rhs)
	{
		for (int i=0; i < rhs.clients; i++)
		{ 
			strcpy(sc_signal[i][0], rhs.sc_signal[i][0]);
			strcpy(sc_signal[i][1], rhs.sc_signal[i][1]);
		}

		for (int j=0; j < rhs.mappingClients; j++)
		{
			strcpy(map_signal[j][0], rhs.map_signal[j][0]);
			strcpy(map_signal[j][1], rhs.map_signal[j][1]);
		}

		g_Updating = false;
		clients = rhs.clients;
		mappingClients = rhs.mappingClients;
		gDutyIndexUnAckCount = rhs.gDutyIndexUnAckCount;
		gDutyAlarmCounter = 0;
		gCounter = 0;
		gCounterError = 0;
		gNewDutyAlarmComingVersion = 0;

		return *this;
	}
}SharedAlarmInfo;
//===============================//

// Global variables ===================//
SharedAlarmInfo* g_pShAlarmInfo = NULL;
AlarmIndex* alarmIndexWithDuty = NULL;
FilteredIndex* alarmIndexWithFilter = NULL;
// This is only use in alarmStore
fp g_fp1 = NULL; 
fp2 g_fp2 = NULL;
// ====================================//

// Shared memory lock
TA_Base_Core::NonReEntrantProcessRWLockable TA_Base_Bus::TA_SharedAlarm::m_SharedAlarmDutyLock(DUTYINDEXLOCKNAME);

void setDllLogName(const std::string logName)
{
	std::string fullName = "c:\\transactive\\logs\\" + logName;
	TA_Base_Core::DebugUtil::getInstance().setFile(fullName.c_str());
	TA_Base_Core::DebugUtil::getInstance().setMaxSize(99999999);
	TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugTrace);
}

BOOL APIENTRY DllMain( HANDLE hModule, 
					  DWORD  ul_reason_for_call, 
					  LPVOID lpReserved
					  )
{
    switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{			
			char modelName[200+1] = {0};
			char logName[200] = {0};
			DWORD dwSize = GetModuleFileName(NULL, modelName, 200);
			modelName[dwSize] = 0;
			std::string strAppName=modelName;
            strAppName = strAppName.substr(strAppName.rfind("\\") + 1);
			if(strAppName.length()>0)
			{
				sprintf(logName,"c:\\transActive\\logs\\SharedAlarm_%s.log",strAppName.c_str());			 			
			}
			else
			{
				sprintf(logName, "c:\\transActive\\logs\\SharedAlarm_%p.log", hModule);
			}									
			//TA_Base_Core::DebugUtil::getInstance().setFile("c:\\SharedAlarm.log");
			TA_Base_Core::DebugUtil::getInstance().setFile(logName);
			TA_Base_Core::DebugUtil::getInstance().setMaxSize(2000000);
			
			TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugTrace);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TA_SharedAlarm - DLL_PROCESS_ATTACH] hModule=%p,ul_reason_for_call=%d, lpReserved=%p",
				hModule, ul_reason_for_call, lpReserved);
			//we are failed to move this function into alarm_list_control, because the banner cannot get alarms
			//so we have to use the following methed to improve start speed of alarm store.
			if (strAppName.find("StoreService") == std::string::npos )  
			{
				TA_Base_Core::NonReEntrantProcessLockable procLock(SHARED_MEM_INIT);
				TA_Base_Core::ProcessGuard guard(procLock);

				TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (TA_Base_Bus::TA_SharedAlarm::m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);				
				TA_Base_Core::CSharedMem::mapSharedMem(TA_Base_Core::CSharedMem::_open_create);
			
				TA_Base_Bus::TA_SharedAlarm::initDetail();
			}
			
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[TA_SharedAlarm - DLL_PROCESS_DETACH] hModule=%p,ul_reason_for_call=%d, lpReserved=%p",
				hModule, ul_reason_for_call, lpReserved);
			
			TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (TA_Base_Bus::TA_SharedAlarm::m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);				
			int ret = TA_Base_Core::CSharedMem::unmapSharedMem();
		}
		break;
	default:
		break;
    }

    return TRUE;
}

namespace TA_Base_Bus
{
	// passesfilters
	// functor for checking alarm if it passes the user defined filter by the client
	struct passesfilters : public std::binary_function<const FilteredIndex, TA_Base_Core::AlarmItem*, bool>
	{
	public:
		bool operator()(const FilteredIndex& fa, TA_Base_Core::AlarmItem* pAlarmItem) const
		{
			try
			{
				if (((fa.filterPtr)->*g_fp1)(pAlarmItem))
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "passesfilter true");
					fa.index->insert(pAlarmItem);
					return true;
				}
			}
			catch(...)
			{				
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"struct passesfilters() error ID%s, asset:%s", pAlarmItem->alarmID,pAlarmItem->assetName);
				//return true;
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "passesfilter false");
			return false;
		}
	};

	//////////////////////////////////////////////////////////////////////////
	// DisplayDataRetriever 
	// template functor used to retrieve the alarms with the specified index and range
	// To optimize the retrieval of alarms, the multi index is access based on the position of 
	// beginning index of the query. see illustration below.
	//	
	//					multi index
	//	Ascending			|			
	//		|	case 1:		|	case 4:
	//		|	normal 		|	normal 
	//		v	iterator	|	iterator
	//		________________|_______________	middle size
	//						|
	//			case 2:		|	case 3:		^
	//			reverse		|	reverse		|
	//			iterator	|	iterator	|
	//						|			descending
	//					max size
	//////////////////////////////////////////////////////////////////////////

	template <typename T_Index>
	struct DisplayDataRetriever 
	{
		DisplayDataRetriever(TA_SharedAlarm* pParent, T_Index& index) : m_index(index), m_pParent(0){m_pParent = pParent; };
		bool operator ()(AlarmListT* pDisplayList, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize)
		{
			unsigned long ulIndexSize = m_index.size();
			if (ulIndexSize == 0) return false;
			//pDisplayVec->reserve(ulCacheSize + 1);
			long ulMidIndex = ulIndexSize / 2;
			
			if (bSortAccend)
			{ 
				if (ulIndexBegin < ulMidIndex)
				{
					// case 1: ascending and beginning index is within the first half
					T_Index::iterator itBegin = m_index.begin();
					T_Index::iterator itEnd;
					std::advance(itBegin, ulIndexBegin);

					if (ulIndexBegin + ulCacheSize > ulIndexSize)
					{
						itEnd = m_index.end();
					}
					else
					{
						itEnd = itBegin;
						std::advance(itEnd, ulCacheSize);
					}			
					std::copy(itBegin, itEnd, std::back_inserter(*pDisplayList));
				}
				else
				{ // case 2: ascending and beginning index is within the second half
					T_Index::reverse_iterator itBegin = m_index.rbegin();
					T_Index::reverse_iterator itEnd;
					
					if (ulIndexBegin + ulCacheSize < ulIndexSize)
					{	// calculate end index
						unsigned long ulEndIndex = ulIndexSize - (ulIndexBegin + ulCacheSize);
						std::advance(itBegin, ulEndIndex);
						itEnd = itBegin;
						std::advance(itEnd, ulCacheSize);
					}
					else
					{
						itEnd = itBegin;
						std::advance(itEnd, (ulIndexSize - ulIndexBegin));
					}
					std::copy(itBegin, itEnd, std::front_inserter(*pDisplayList));
				}
			}
			else
			{
				if (ulIndexBegin < ulMidIndex)
				{	// case 3: descending and beginning index is within the first half
					T_Index::reverse_iterator itBegin = m_index.rbegin();
					T_Index::reverse_iterator itEnd;
					std::advance(itBegin, ulIndexBegin);

					if (ulIndexBegin + ulCacheSize > ulIndexSize)
					{
						itEnd = m_index.rend();
					}
					else
					{
						itEnd = itBegin;
						std::advance(itEnd, ulCacheSize);
					}			
					std::copy(itBegin, itEnd, std::back_inserter(*pDisplayList));
				}
				else
				{	// case 4: descending and beginning index is within the second half
					T_Index::iterator itBegin = m_index.begin();
					T_Index::iterator itEnd;
					
					if (ulIndexBegin + ulCacheSize < ulIndexSize)
					{
						unsigned long ulEndIndex = ulIndexSize - (ulIndexBegin + ulCacheSize);
						std::advance(itBegin, ulEndIndex);
						itEnd = itBegin;
						std::advance(itEnd, ulCacheSize);
					}			
					else
					{
						itEnd = itBegin;
						std::advance(itEnd, (ulIndexSize - ulIndexBegin));
					}

					std::copy(itBegin, itEnd, std::front_inserter(*pDisplayList));
				}
			}
			return true;	
		}
	private:
		T_Index& m_index;
		TA_SharedAlarm* m_pParent;
	};

	void TA_SharedAlarm::logAlarm(TA_Base_Core::AlarmItem* alarm)
	{
		if (alarm ==NULL) return;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "address=%ld",  alarm );   
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "alarmID=%s ackby=%s", alarm->alarmID, alarm->alarmAcknowledgedBy );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Comment=%s Description=%s", alarm->alarmComments, alarm->alarmDescription );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Severity=%ld value=%s", alarm->alarmSeverity, alarm->alarmValue );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "asset=%s entityKey=%ld", alarm->assetName, alarm->assocEntityKey );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "entityType=%ld avl=%s", alarm->assocEntityTypeKey, alarm->avalanche );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "avlID=%s dss=%s", alarm->avalancheHeadID, alarm->decisionSupportState );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "entityName=%s location=%ld", alarm->entityName, alarm->locationKey );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "messageTypeKey=%ld mmsState=%s", alarm->messageTypeKey, alarm->mmsState );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "state=%ld subsystemKey=%ld", alarm->state, alarm->subsystemKey );
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "transactiveTime=%ld", alarm->transactiveTime);					
	}
	void TA_SharedAlarm::logAlarm(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  alarmID:%s, assetName:%s, entityKey:%ld, messageType:%ld loc=%ld", 
			alarmUpdate.alarmID.in(), alarmUpdate.assetName.in(), alarmUpdate.assocEntityKey, alarmUpdate.messageTypeKey,alarmUpdate.locationKey);
		
		if (alarmUpdate.alarmHasBeenAcknowledged)
		{ 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  ackby=:%s",alarmUpdate.alarmAcknowledgedBy.in());
		}
		
		if (alarmUpdate.alarmHasBeenClosed)
		{		 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  closeValue=:%s",alarmUpdate.closeValue.in());		
		} 
		
		if (alarmUpdate.commentsWereAdded)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "updateAlarm,  comment=:%s",alarmUpdate.alarmComments.in());	
		}
	}

	void TA_SharedAlarm::init(bool alwaysRecreateMapfile /*= false*/)
	{		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"TA_SharedAlarm::init() come in");
		TA_Base_Core::NonReEntrantProcessLockable procLock(SHARED_MEM_INIT);
		TA_Base_Core::ProcessGuard guard(procLock);
		SharedAlarmInfo* pSharedInfo = NULL;
//		this->remapSharedMem();
		{
			TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
			TA_Base_Core::CSharedMem::mapSharedMem();
		}
	
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"TA_SharedAlarm::init() remapSharedMem finish");

		AlarmIndex* pdi = TA_Base_Core::CSharedMem::find<AlarmIndex>(AlarmIndexWithDuty_Name.c_str());
		FilteredIndex * pFdi = TA_Base_Core::CSharedMem::find<FilteredIndex>(AlarmIndexWithFilter_Name.c_str());

		if (NULL != pdi && NULL != pFdi)  
		{  //found 
			alarmIndexWithDuty = pdi;
			alarmIndexWithFilter = pFdi;

			g_pShAlarmInfo = TA_Base_Core::CSharedMem::find<SharedAlarmInfo>(AlarmInfo_Name.c_str());

			// clean alarmIndexWithDuty and notify client to update display
			clean();

			// allow clients to refresh before we notify clients to unmap the shared memory
			// Note: size of sleep should be higher that the refresh rate of the
			// clients list control.
			Sleep(500);

			// delete the old filters if found
			fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
			for (int nIdx=0; nIdx<g_pShAlarmInfo->clients; nIdx++)
			{
				if (NULL != alarmIndexWithFilter[nIdx].index)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Deleting AlarmIndex with filter for client %s at position %d.", g_pShAlarmInfo->sc_signal[nIdx][0], nIdx);
					alarmIndexWithFilter[nIdx].index->clear();
					AlarmIndex * pTemp = alarmIndexWithFilter[nIdx].index;
					pSegment->destroy_ptr(pTemp);
					alarmIndexWithFilter[nIdx].index = NULL;
					alarmIndexWithFilter[nIdx].filterPtr = NULL;
				}	
			}

			bool recreateMapfile = alwaysRecreateMapfile;
			if(recreateMapfile==false)
			{
				// check shared memory if work normally, if not then need to recreate a new mapfile
				recreateMapfile = !(this->checkSharedMemory());
			}

			if(recreateMapfile==true)
			{
				// Record the existing alarm information before we reset everything
				pSharedInfo = new SharedAlarmInfo();
				*pSharedInfo = *g_pShAlarmInfo;
				// notify client to unmap shared memory, alarm store will recreate a new map file
				notifyClientUnmap();

				// remap shared memory
				this->unmapSharedMem();
				this->remapSharedMem();
			}
			
		}
		// Construct SharedAlarmInfo
		else
		{
			// construct the AlarmInfo
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Creating Shared Alarm Info");
			try
			{
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				g_pShAlarmInfo = pSegment->find_or_construct<SharedAlarmInfo>(AlarmInfo_Name.c_str())();
			}
			catch (...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception ocured while creating Shared Alarm Info");
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Shared Alarm Info Created");
		}

		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
		try
		{
			initDetail();
			if (NULL != pSharedInfo)
			{
				*g_pShAlarmInfo = *pSharedInfo;
				delete pSharedInfo;
				pSharedInfo = NULL;
			}
			// reset all alarm count.
			g_pShAlarmInfo->gDutyAlarmCounter = 0;

		}
		catch (...)
		{
			//pls do not add anyting here, or cause compiling error;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"initDetail error");
		}

		//HP, flush out to the disk;
		//TA_Base_Core::Shmem::sync();   
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"TA_SharedAlarm::init() come out");

		//++++++++++++++++++++++
	}

	int TA_SharedAlarm::addClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName, int& csPos, bool mayExist /*false*/)
	{
		bool realExist = false;
		bool mappingEventExist = false;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_SharedAlarm::addClient Coming in");
		// check input data
		if (strlen(server) == 0 || strlen(client) == 0 || strlen(mapEvName)==0 || strlen(unmapEvName) == 0) return 0;
		
		// set dll log name: the calling process will use the new log file to record log after calling 'addClient'.
		//char logName[128];
		std::string namePostfix = client;
		//sprintf(logName, "SharedAlarm_%s.log", namePostfix.c_str());
		//setDllLogName(logName);        
		{
			TA_Base_Core::NonReEntrantProcessLockable LocalSignalLock(SIGLOCKNAME);
			TA_Base_Core::ProcessGuard   LocalSignalGuard (LocalSignalLock );			 
			if(mayExist)
			{
				int i = 0;
				for(i = 0; i < g_pShAlarmInfo->clients; ++ i)
				{
					if((0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][0], server)) && (0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][1], client)))
					{
						csPos = i;
						realExist = true;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client already existed using Position %d", csPos);
						break;
					}
				}

				for(i = 0; i < g_pShAlarmInfo->mappingClients; ++ i)
				{
					if((0 == lstrcmp(g_pShAlarmInfo->map_signal[i][0], mapEvName)) && (0 == lstrcmp(g_pShAlarmInfo->map_signal[i][1], unmapEvName)))
					{
						mappingEventExist = true;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client map/unmap event names found at position %d", i);
						break;
					}
				}
			}

			if(!realExist)
			{
				// find reusable position
				int nReusePos = -1;
				for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
				{
					if((0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][0], "")) && (0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][1], "")))
					{
						nReusePos = i;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Found reusable Client position at %d", nReusePos);
						break;
					}
				}

				if (nReusePos == -1)
				{
					csPos = g_pShAlarmInfo->clients;
					++ g_pShAlarmInfo->clients;
				}
				else
					csPos = nReusePos;

				lstrcpy(g_pShAlarmInfo->sc_signal[csPos][0], server);
				lstrcpy(g_pShAlarmInfo->sc_signal[csPos][1], client);
				if(g_pShAlarmInfo->clients >= CLIENTS_NUM)
				{
					g_pShAlarmInfo->clients = CLIENTS_NUM - 1;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"clients number out bound");
				}
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New client at position %d", csPos);
			}

			if(!mappingEventExist)
			{
				int nMappingPos = 0;
				int nReusePos = -1;
				for(int i = 0; i < g_pShAlarmInfo->mappingClients; ++ i)
				{
					if((0 == lstrcmp(g_pShAlarmInfo->map_signal[i][0], "")) && (0 == lstrcmp(g_pShAlarmInfo->map_signal[i][1], "")))
					{
						nReusePos = i;
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"Found reusable Client map/unmap event names position at %d", nReusePos);
						break;
					}
				}

				if (nReusePos == -1)
				{
					nMappingPos = g_pShAlarmInfo->mappingClients;
					++ g_pShAlarmInfo->mappingClients;
				}
				else
					nMappingPos = nReusePos;

				lstrcpy(g_pShAlarmInfo->map_signal[nMappingPos][0], mapEvName);
				lstrcpy(g_pShAlarmInfo->map_signal[nMappingPos][1], unmapEvName);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "New map/unmap event names at position %d", nMappingPos);
				if(g_pShAlarmInfo->mappingClients >= CLIENTS_NUM)
				{
					g_pShAlarmInfo->mappingClients = CLIENTS_NUM - 1;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"mappingClients number out bound");
				}
			}
		}


		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
		int ret = 0;
		// add try block to make sure the SharedAlarmLock can be release.
		try
		{
			if(alarmIndexWithFilter[csPos].index != NULL)
			{
				alarmIndexWithFilter[csPos].filterPtr = NULL;
				alarmIndexWithFilter[csPos].index->clear();
				AlarmIndex * pTemp = alarmIndexWithFilter[csPos].index;
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				pSegment->destroy_ptr(pTemp);
				alarmIndexWithFilter[csPos].index = NULL;
			}

			if(alarmIndexWithDuty != NULL && !alarmIndexWithDuty->empty())
			{
				ret = 1;
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Caught exception in TA_SharedAlarm::addClient");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_SharedAlarm::addClient Coming out");
		return ret;
	}
   
	bool TA_SharedAlarm::removeClient(const char* server, const char* client, const char* mapEvName, const char* unmapEvName)
	{
		int pos = 0;
		bool bRetVal = false;
		bool realExist = false;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_SharedAlarm::removeClient Coming in");

		int i = 0;
		{ 
			TA_Base_Core::NonReEntrantProcessLockable LocalSignalLock(SIGLOCKNAME);
			TA_Base_Core::ProcessGuard   LocalSignalGuard (LocalSignalLock );		
			for(i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if((0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][0], server)) && (0 == lstrcmp(g_pShAlarmInfo->sc_signal[i][1], client)))
				{
					pos = i;
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client Position = %d", pos);
					realExist = true;
					break;
				}
			}
			int j = 0;
			int jpos = 0;
			bool mappingExist =false;
			for(j = 0; j < g_pShAlarmInfo->mappingClients; ++ j)
			{
				if((0 == lstrcmp(g_pShAlarmInfo->map_signal[j][0], mapEvName)) && (0 == lstrcmp(g_pShAlarmInfo->map_signal[j][1], unmapEvName)))
				{
					jpos = j;
					mappingExist = true;
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Client map/unmap event names at Position = %d", jpos);
					break;
				}
			}

			// Note: We comment out the code to move the remaining clients to avoid changing 
			// the position of the clients. A problem will occur if the remove a client and its position is at the 
			// beginning or in the middle of the array, the position of the succeeding clients will change. 
			if (realExist)
			{
// 				for(i = pos; i < g_pShAlarmInfo->clients-1; ++ i)
// 				{
// 					lstrcpy(g_pShAlarmInfo->sc_signal[i][0], g_pShAlarmInfo->sc_signal[i+1][0]);
// 					lstrcpy(g_pShAlarmInfo->sc_signal[i][1], g_pShAlarmInfo->sc_signal[i+1][1]);
// 				}
				lstrcpy(g_pShAlarmInfo->sc_signal[pos][0], "");
				lstrcpy(g_pShAlarmInfo->sc_signal[pos][1], "");

//				--g_pShAlarmInfo->clients;
				bRetVal = true;
			}

			if (mappingExist)
			{
// 				for(j = jpos; j < g_pShAlarmInfo->mappingClients-1; ++ j)
// 				{
// 					lstrcpy(g_pShAlarmInfo->map_signal[j][0], g_pShAlarmInfo->map_signal[j+1][0]);
// 					lstrcpy(g_pShAlarmInfo->map_signal[j][1], g_pShAlarmInfo->map_signal[j+1][1]);
// 				}
				lstrcpy(g_pShAlarmInfo->map_signal[jpos][0], "");
				lstrcpy(g_pShAlarmInfo->map_signal[jpos][1], "");

//				--g_pShAlarmInfo->mappingClients;
			}
		}

		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		{	 
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
			if(alarmIndexWithFilter[pos].index != NULL)
			{
				alarmIndexWithFilter[pos].filterPtr = NULL;
				alarmIndexWithFilter[pos].index->clear();
				AlarmIndex * pTemp = alarmIndexWithFilter[pos].index;
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				pSegment->destroy_ptr(pTemp);
				alarmIndexWithFilter[pos].index = NULL;
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "TA_SharedAlarm::removeClient Coming out");
		return bRetVal;
	}

	int handle_remap(EXCEPTION_POINTERS *ep)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "catch a SEH exception, Handle a remap. continue...");
		DWORD ecode = ep->ExceptionRecord->ExceptionCode;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SEH exception code: [%d]", ecode);				
		return EXCEPTION_EXECUTE_HANDLER;
	}

	void updateIndexInfoInternal(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, int cs_pos)
	{
		void* sfi = 0;
		AlarmIndex* pActionIndex = NULL;
		
		if (NULL == alarmIndexWithFilter[cs_pos].index)
		{
			pActionIndex = alarmIndexWithDuty;
		}
		else
		{
			pActionIndex = alarmIndexWithFilter[cs_pos].index;
		}
		memset(&info, 0, sizeof(info));

		ACE_SEH_TRY
		{
			alarmindex_by_time& time_index = boost::multi_index::get<SORT_BY_DATETIME>(*pActionIndex);
			alarmindex_by_default& default_index = boost::multi_index::get<SORT_BY_DEFAULT>(*pActionIndex);
			alarmindex_by_serverityUnAck& serUnAck_index = boost::multi_index::get<SORT_BY_SEVERITYUNACK>(*pActionIndex);
			alarmindex_by_ack& ack_index = boost::multi_index::get<SORT_BY_ACK>(*pActionIndex);
			// Get the total alarm count
			info.totalAlarms = default_index.size();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "DutyIndex Data size = %ld", default_index.size());

			// Get the current version of new duty alarm
			info.dutyAlarmVersion=g_pShAlarmInfo->gNewDutyAlarmComingVersion;
			
			// check if alarm has already exceeded the maximum count
			info.exceededMaxDisplay = info.totalAlarms > MAXDUTYALARMCOUNT;
			TA_Base_Core::AlarmItem* pAlarmItem=NULL;

			// Retrieve the earliest and the latest time of alarms
			if (time_index.size()>0)
			{
				alarmindex_by_time::iterator itr = time_index.begin();
					 pAlarmItem = (*itr);				
				if(pAlarmItem != NULL)
				{
					info.earliestSourceTime.millitm = pAlarmItem->transactiveTime.millitm;
					info.earliestSourceTime.time = pAlarmItem->transactiveTime.time;
				}
				itr = time_index.end();
				-- itr;
				pAlarmItem = (*itr);
				if(pAlarmItem != NULL)
				{
					info.latestSourceTime.millitm = pAlarmItem->transactiveTime.millitm;
					info.latestSourceTime.time = pAlarmItem->transactiveTime.time;
				}
			}

			// Retrieve the highest severity of unacknowledged alarm
			info.highestSeverity=0;
			if (serUnAck_index.size()>0)
			{
 				pAlarmItem = (*serUnAck_index.begin());
 				if((pAlarmItem != NULL) && (strlen(pAlarmItem->alarmAcknowledgedBy) == 0))
				{
					info.highestSeverity = pAlarmItem->alarmSeverity;
				}
			}
			
			info.isLoadingAlarms = false;

			// Retrieve the unacknowledged count
			if (NULL == alarmIndexWithFilter[cs_pos].index)
			{
				info.unackAlarms = g_pShAlarmInfo->gDutyIndexUnAckCount;
			}
			else
			{
				info.unackAlarms = alarmIndexWithFilter[cs_pos].filteredIndexUnAckCount;
			}

// 			char unacktmp[16];
// 			lstrcpy(unacktmp, "");
// 			info.unackAlarms = ack_index.count(boost::make_tuple((char*)unacktmp, TA_Base_Core::ItemMillisecondTime(0,0)), 
// 				boost::multi_index::composite_key_compare<
// 				charptrless<char*>,
// 				alwaysfalse<TA_Base_Core::ItemMillisecondTime> 
// 				>());
		}
		ACE_SEH_EXCEPT (handle_remap (GetExceptionInformation ()))
			//__except(GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION ? 
			//	EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
		{ 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "catch an exception of TA_SharedAlarm::updateIndexInfoInternal()");
			throw;
		}
	}

	void TA_SharedAlarm::updateIndexInfo(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, int cs_pos)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::updateIndexInfo() come in");
		if (NULL == &info) return;
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		{			
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire");
				return;
			}
			updateIndexInfoInternal(info,cs_pos);
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::updateIndexInfo() come out");
        //ReleaseMutex(SharedAlarmLock);
        //CloseHandle(SharedAlarmLock);
	}

	int deleteAlarmByID(alarmindex_by_time::iterator& itFind)
	{
		TA_Base_Core::AlarmItem* pFindAlarm =  *itFind;		
		try
		{   
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "delete oldest id=%s time=%ld prt=%ld", pFindAlarm->alarmID,pFindAlarm->transactiveTime.time,pFindAlarm);
			bool bUnAck = false;
			bUnAck = (strlen(pFindAlarm->alarmAcknowledgedBy) == 0);

			for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if(alarmIndexWithFilter[i].index != NULL)
				{
					AlarmIndex::iterator it2 = alarmIndexWithFilter[i].index->find(pFindAlarm->alarmID);
					if (it2 != alarmIndexWithFilter[i].index->end())
					{
						alarmIndexWithFilter[i].index->erase(it2);
						if (bUnAck)
						{ // update the unAcknowledged alarms
							--(alarmIndexWithFilter[i].filteredIndexUnAckCount);
						}
					}
				}
			}
			{
				alarmindex_by_id::iterator it = alarmIndexWithDuty->find(pFindAlarm->alarmID);
				alarmIndexWithDuty->erase(it);
				--(g_pShAlarmInfo->gDutyAlarmCounter);
				if (true == bUnAck)
				{ // update the unAcknowledged alarms
					--(g_pShAlarmInfo->gDutyIndexUnAckCount);
				}
			}
			pFindAlarm->~AlarmItem();
			TA_Base_Core::CSharedMem::dealloc(pFindAlarm);
			pFindAlarm = NULL;
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "end");
			//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Delete alarm successful...");
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "catch an exception of TA_SharedAlarm::deleteAlarmByID()");
			throw;
		}
        return 0;
	}

	bool TA_SharedAlarm::addAlarmItem(TA_Base_Bus::AlarmDataItem* alarmItem)
	{
		bool bRetVal = false;
		if(alarmItem->state == TA_Base_Core::AlarmSuppressed) return bRetVal;
		{
			// if there is any exception in init(), the 'gAllAlarmCounter' maybe not equal to the count of allAlarm.
			// here make sure the all alarms counter is correct.
			g_pShAlarmInfo->gDutyAlarmCounter = alarmIndexWithDuty->size();
		}

		TA_Base_Core::AlarmItem* ai_ptr = NULL;
		void* data = TA_Base_Core::CSharedMem::alloc(sizeof(TA_Base_Core::AlarmItem));		
		try
		{
			// if alarms already reach the limit then we should remove first before we insert.
			if(g_pShAlarmInfo->gDutyAlarmCounter > MAXDUTYALARMCOUNT)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "!!!reach MAXALLALARMCOUNT, delete oldest. New alarmID:%s",
					alarmItem->alarmID.c_str());				 
				alarmindex_by_time::iterator itFind;			
				//HP++
				try
				{					
					TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);

					alarmindex_by_time& find_id = boost::multi_index::get<SORT_BY_DATETIME>(*alarmIndexWithDuty);
					itFind = find_id.begin();				 
					deleteAlarmByID(itFind); 
				}			
				catch (...)
				{
					//pls do not add anyting here, or cause compiling error;
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError, "deleteAlarm find");
					throw;
				}			
				//return;
			}

			ai_ptr = new(data)TA_Base_Core::AlarmItem(
				alarmItem->alarmID.c_str(), 
				alarmItem->transactiveTime,
				alarmItem->alarmSeverity, 
				alarmItem->assetName.c_str(), 
				alarmItem->alarmDescription.c_str(), 
				alarmItem->alarmValue.c_str(), 
				alarmItem->mmsState.c_str(), 
				alarmItem->state,
				alarmItem->decisionSupportState.c_str(),
				alarmItem->alarmAcknowledgedBy.c_str(),
				alarmItem->alarmComments.c_str(),
				alarmItem->avalanche.c_str(),
				alarmItem->subsystemKey,
				alarmItem->messageTypeKey,
				alarmItem->locationKey,
				alarmItem->assocEntityTypeKey,
				alarmItem->assocEntityKey,
				alarmItem->entityName.c_str(),
				alarmItem->avalancheHeadID.c_str()
				);

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ptr=%ld",  ai_ptr);
			
			inserToDutyIndex(ai_ptr);
			bRetVal = true;
		}
		catch (...)
		{
			SharedAlarmMonitor::getInstance().updateSeqTime();			
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, " unknown exception alarm count=[%d] and log",(g_pShAlarmInfo->gDutyAlarmCounter));	
			try
			{
				logAlarm(ai_ptr);
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "log error");	
			}
			throw;
		}

		return bRetVal;
	}


	void TA_SharedAlarm::inserToDutyIndex(TA_Base_Core::AlarmItem* pAlarmItem)
	{
		if (pAlarmItem == NULL) return;
		try
		{
			bool bUnAck = false;
			bUnAck = (strlen(pAlarmItem->alarmAcknowledgedBy) == 0);
			g_pShAlarmInfo->gNewDutyAlarmComingVersion++;  //update new alarm version, improve performance of alarm sound 
			{
				alarmIndexWithDuty->insert(pAlarmItem);
				++g_pShAlarmInfo->gDutyAlarmCounter;

				// keep record of the unAcknowledged alarms
				if (true == bUnAck)
				{
					g_pShAlarmInfo->gDutyIndexUnAckCount++;
				}
			}
			for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if(alarmIndexWithFilter[i].index != NULL)
				{
					if(passesfilters()(alarmIndexWithFilter[i], pAlarmItem))
					{
						// keep record of the unAcknowledged alarms
						if (true == bUnAck)
						{
							alarmIndexWithFilter[i].filteredIndexUnAckCount++;
						}
					}
				}
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert to DutyIndex Success AlarmID = %s, current size = %ld", pAlarmItem->alarmID, alarmIndexWithDuty->size());
		}
		catch (...)
		{
			SharedAlarmMonitor::getInstance().updateSeqTime();			
		    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, " unknown exception inserToDutyIndex"); 
			try
			{
				logAlarm(pAlarmItem);
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "log error");	
			}
			throw;
		}
	}

	bool TA_SharedAlarm::insertAlarmInternal(TA_Base_Bus::AlarmDataItem* alarmItem)
	{
		bool bRetVal = false;
		try
		{
			alarmindex_by_id ::iterator itFind;
			alarmindex_by_id& find_id = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);

			itFind = find_id.find(const_cast<char*>(alarmItem->alarmID.c_str()));
			if (itFind != find_id.end()) //exits already
			{ 
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "insertAlarmInternal: Alarm is already existing. AlarmID = %s", alarmItem->alarmID.c_str());
				return bRetVal;
			}
			
			bRetVal = addAlarmItem(alarmItem);
		}
		catch (...)
		{
			//pls do not add anyting here, or cause compiling error;
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "caught exception in insertAlarmInternal");
			throw;
		}

		return bRetVal;
	}

	void TA_SharedAlarm::cleanInternal()
	{
		ACE_SEH_TRY
		{
			//alarmIndexWithDuty->clear();
			for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if(alarmIndexWithFilter[i].index != NULL)
				{
					alarmIndexWithFilter[i].index->clear();
					alarmIndexWithFilter[i].filteredIndexUnAckCount = 0;
				}
			}
			
			deleteAllAlarmItems();
		}
		ACE_SEH_EXCEPT (handle_remap (GetExceptionInformation ()))
		{
			//pls do not add anyting here, or cause compiling error;
		}
	}
	void TA_SharedAlarm::clean()
	{
		//HP++
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		{	 
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
			cleanInternal();
			notifyClients();
		}
        //ReleaseMutex(SharedAlarmLock);
        //CloseHandle(SharedAlarmLock);
	}

	struct updateAlarmDetail
	{
		updateAlarmDetail(const TA_Base_Bus::AlarmDataItem& alarmUpdate): m_alarmUpdate(alarmUpdate){}
		//updateAlarmDetail(const TA_Base_Core::AlarmUpdateCorbaDef& alarmUpdate):m_alarmUpdate(alarmUpdate){}
		void operator()(TA_Base_Core::AlarmItem* pFindAlarm)
		{
			pFindAlarm->state = m_alarmUpdate.state;
			pFindAlarm->transactiveTime = m_alarmUpdate.transactiveTime;

			if (strcmp(pFindAlarm->alarmValue, m_alarmUpdate.alarmValue.c_str()) != 0)
			{
				assign(pFindAlarm->alarmValue, m_alarmUpdate.alarmValue.c_str(), TA_Base_Core::ALARM_VALUE_SIZE);
			}

			if (strcmp(pFindAlarm->alarmComments, m_alarmUpdate.alarmComments.c_str()) != 0)
			{
				assign(pFindAlarm->alarmComments, m_alarmUpdate.alarmComments.c_str(), TA_Base_Core::ALARM_COMMENTS_SIZE);
			}

			if(strcmp(pFindAlarm->decisionSupportState, m_alarmUpdate.decisionSupportState.c_str()) != 0)
			{
				assign(pFindAlarm->decisionSupportState, m_alarmUpdate.decisionSupportState.c_str(), TA_Base_Core::DECISION_SUPPORT_STATE_STATE);
			}

			if (strcmp(pFindAlarm->mmsState, m_alarmUpdate.mmsState.c_str()) != 0)
			{
				assign(pFindAlarm->mmsState, m_alarmUpdate.mmsState.c_str(), TA_Base_Core::MMS_STATE_SIZE);
			}

			if (strcmp(pFindAlarm->alarmAcknowledgedBy, m_alarmUpdate.alarmAcknowledgedBy.c_str()) != 0)
			{
				assign(pFindAlarm->alarmAcknowledgedBy, m_alarmUpdate.alarmAcknowledgedBy.c_str(), TA_Base_Core::ALARM_ACK_BY_SIZE);
			}

		}
	private:

		inline void assign(char* buf, const char* value, size_t buf_size)
		{
			size_t cpLen = strlen(value) >= buf_size ? buf_size - 1 : strlen(value);
			strncpy(buf, value, cpLen);
			*(buf+cpLen) = 0;
		}

		const TA_Base_Bus::AlarmDataItem& m_alarmUpdate;
	};
	

	void removeFromAlarmIndex(alarmindex_by_location::iterator& locItem)
	{
 		TA_Base_Core::AlarmItem* tempItem =  *locItem;
		bool bUnAck = (strlen(tempItem->alarmAcknowledgedBy)==0);
		for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
		{
			if(alarmIndexWithFilter[i].index != NULL)
			{
				AlarmIndex::iterator it2 = alarmIndexWithFilter[i].index->find(const_cast<char*>(tempItem->alarmID));
				if (it2 != alarmIndexWithFilter[i].index->end())
				{
					alarmIndexWithFilter[i].index->erase(it2);
					if (true == bUnAck)
					{
						--(alarmIndexWithFilter[i].filteredIndexUnAckCount);
					}
				}
			}
		}

		alarmindex_by_location& index_location = boost::multi_index::get<SORT_BY_LOCATION>(*alarmIndexWithDuty);
		index_location.erase(locItem);
		--(g_pShAlarmInfo->gDutyAlarmCounter);
		if (true == bUnAck)
		{
			--(g_pShAlarmInfo->gDutyIndexUnAckCount);
		}
		tempItem->~AlarmItem();
		TA_Base_Core::CSharedMem::dealloc(tempItem);
		tempItem = NULL;
	}

	//++Noel
	void removeFromFilterIndex(alarmindex_by_location::iterator& subSystemItem)
	{
		TA_Base_Core::AlarmItem* tempItem =  *subSystemItem;

		bool bUnAck = (strlen(tempItem->alarmAcknowledgedBy)==0);
		for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
		{
			if(alarmIndexWithFilter[i].index != NULL)
			{
				AlarmIndex::iterator it2 = alarmIndexWithFilter[i].index->find(const_cast<char*>(tempItem->alarmID));
				if (it2 != alarmIndexWithFilter[i].index->end())
				{
					alarmIndexWithFilter[i].index->erase(it2);
					if (true == bUnAck)
					{
						--(alarmIndexWithFilter[i].filteredIndexUnAckCount);
					}
				}
			}
		}		 

		--g_pShAlarmInfo->gDutyAlarmCounter;
		if (true == bUnAck)
		{
			--(g_pShAlarmInfo->gDutyIndexUnAckCount);
		}

		// We need to deallocate data here
		tempItem->~AlarmItem();
		TA_Base_Core::CSharedMem::dealloc(tempItem);
		tempItem = NULL;
	}


	void TA_SharedAlarm::deleteAllAlarmItems()
	{
		//TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS, true);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[shared alarm store] TA_SharedAlarm::deleteAllAlarmItems");
		try
		{
			if(alarmIndexWithDuty != NULL && !alarmIndexWithDuty->empty())
			{
                std::for_each(alarmIndexWithDuty->begin(), alarmIndexWithDuty->end(),
                    boost::bind(&TA_SharedAlarm::deleteItem, this, _1));
            }
			
			alarmIndexWithDuty->clear();
			g_pShAlarmInfo->gDutyAlarmCounter = 0;
			g_pShAlarmInfo->gDutyIndexUnAckCount = 0;
			//TA_Base_Core::Shmem::sync();
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[shared alarm store]Caught an unknown exception in deleteAllAlarmItems");
		}
      
	}

	void TA_SharedAlarm::buildIndexWithFilter(TA_Base_Bus::AlarmFilterSet* pAlarmFilterSet,fp vfp,int cs_pos,bool isDefault)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::buildIndexWithFilter() come in");
		//check input data
		if (cs_pos < 0 || cs_pos >= g_pShAlarmInfo->clients || 
			((NULL == pAlarmFilterSet || NULL == vfp) && !isDefault)) return;
		
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
		const char* pcClientName = g_pShAlarmInfo->sc_signal[cs_pos][0];
		
		TA_Base_Bus:: AlarmFilterSet* temp = NULL;
		temp = alarmIndexWithFilter[cs_pos].filterPtr;
		alarmIndexWithFilter[cs_pos].filterPtr = NULL;
		alarmIndexWithFilter[cs_pos].filteredIndexUnAckCount = 0; // reset unacknowledged alarm count

		if (temp != NULL)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Deleting old filterSet.");
			delete temp;
			temp = NULL;
		}

		if (isDefault)
		{
			if (NULL != alarmIndexWithFilter[cs_pos].index)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Deleting AlarmIndex with filter.");
				alarmIndexWithFilter[cs_pos].index->clear();
				AlarmIndex * pTemp = alarmIndexWithFilter[cs_pos].index;
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				pSegment->destroy_ptr(pTemp);
				alarmIndexWithFilter[cs_pos].index = NULL;
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Notify Client = %s.", pcClientName);
			notifyClient(pcClientName);
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::buildIndexWithFilter() default come out");
			return;
		}

		alarmIndexWithFilter[cs_pos].filterPtr = pAlarmFilterSet;
		if ((NULL == alarmIndexWithFilter[cs_pos].index) && (!isDefault))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Create AlarmIndex with filter.");
			fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
			alarmIndexWithFilter[cs_pos].index = pSegment->construct<AlarmIndex>(boost::interprocess::anonymous_instance)(AlarmIndex::ctor_args_list(), pSegment->get_allocator<AlarmIndex>());
			alarmIndexWithFilter[cs_pos].filterPtr = pAlarmFilterSet;
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Clear all alarms from AlarmIndex to refilter.");
			alarmIndexWithFilter[cs_pos].index->clear();// =  static_cast<AlarmIndex*>(sfi);
		}
		//TA_Base_Core::Shmem::sync();

		g_fp1 = vfp;
		{
			std::for_each(alarmIndexWithDuty->begin(),alarmIndexWithDuty->end(),std::bind1st(passesfilters(),alarmIndexWithFilter[cs_pos]));

			// recalculate unacknowledged alarms for filtered index
			alarmindex_by_ack& ack_index = boost::multi_index::get<SORT_BY_ACK>(*alarmIndexWithFilter[cs_pos].index);
			char unacktmp[16];
			lstrcpy(unacktmp, "");
			alarmIndexWithFilter[cs_pos].filteredIndexUnAckCount = ack_index.count(boost::make_tuple((char*)unacktmp, TA_Base_Core::ItemMillisecondTime(0,0)), 
				boost::multi_index::composite_key_compare<
				charptrless<char*>,
				alwaysfalse<TA_Base_Core::ItemMillisecondTime> 
				>());
		}		
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "Notify Client = %s.", pcClientName);
		notifyClient(pcClientName);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::buildIndexWithFilter() come out");
	}

	void TA_SharedAlarm::getIndexWithSortColInternal(AlarmListT* alarmList, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize)
	{
		ACE_SEH_TRY
		{
			void* sfi = 0;
			AlarmIndex* pActionIndex = NULL;
			if (NULL == alarmIndexWithFilter[nPos].index)
			{
				pActionIndex = alarmIndexWithDuty;
			}
			else
			{
				pActionIndex = alarmIndexWithFilter[nPos].index;
			}

			if (pActionIndex->size() < ulIndexBegin) 
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Index is out range. IndexBegin=%ld, Index DataSize=%ld", ulIndexBegin, pActionIndex->size());
				return;
			}

			switch(nCol) {
			case SORT_BY_DEFAULT:
				{
					alarmindex_by_default& default_index = boost::multi_index::get<SORT_BY_DEFAULT>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_default>(this, default_index ) (alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_DATETIME:
				{
					alarmindex_by_time& time_index = boost::multi_index::get<SORT_BY_DATETIME>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_time>(this, time_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_SEVERITY:
				{
					alarmindex_by_severity& ser_index = boost::multi_index::get<SORT_BY_SEVERITY>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_severity>(this, ser_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_ASSET:
				{
					alarmindex_by_asset& ass_index = boost::multi_index::get<SORT_BY_ASSET>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_asset>(this,ass_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_DESCRIPTION:
				{
					alarmindex_by_description& description_index = boost::multi_index::get<SORT_BY_DESCRIPTION>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_description>(this,description_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_VALUE:
				{
					alarmindex_by_value& value_index = boost::multi_index::get<SORT_BY_VALUE>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_value>(this,value_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_MMS:
				{
					alarmindex_by_mms& mms_index = boost::multi_index::get<SORT_BY_MMS>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_mms>(this,mms_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_DSS:
				{
					alarmindex_by_dss& dss_index = boost::multi_index::get<SORT_BY_DSS>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_dss>(this,dss_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_AVALANCHE:
				{
					alarmindex_by_avalanche& dss_index = boost::multi_index::get<SORT_BY_AVALANCHE>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_avalanche>(this,dss_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_ACK:
				{
					alarmindex_by_ack& ack_index = boost::multi_index::get<SORT_BY_ACK>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_ack>(this,ack_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			case SORT_BY_COMMENTS:
				{
					alarmindex_by_comment& comment_index = boost::multi_index::get<SORT_BY_COMMENTS>(*pActionIndex);
					DisplayDataRetriever<alarmindex_by_comment>(this,comment_index)(alarmList, bSortAccend, ulIndexBegin, ulCacheSize);
					break;
				}
			default:
				{}
			}		
		}
		ACE_SEH_EXCEPT (handle_remap (GetExceptionInformation ()))
		{
			//pls do not add anyting here, or cause compiling error;
		}
	}

	//*/HP++
	bool TA_SharedAlarm::checkSharedMemoryInternal()
	{
		bool checkResult = false;
		ACE_SEH_TRY
		{
			checkResult = checkSharedMemoryImpl();
		}
		ACE_SEH_EXCEPT (handle_remap (GetExceptionInformation ()))
		{
			//pls do not add anyting here, or cause compiling error;
		}
		return checkResult;

	}
	bool TA_SharedAlarm::checkSharedMemory()
	{
		bool checkResult = false;
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		checkResult=checkSharedMemoryInternal();	 
		return checkResult;		
	}


	bool TA_SharedAlarm::checkSharedMemoryImpl()
	{
		// TODO: check shared memory whether is destroyed.
		// we create a new alarm, read from sharedmap then delete it,
		//	if all those operation succeed, sharedmap is OK

		std::string alarmId= "00000000-0000-0000-0000-000000000001";
		unsigned long severity = 1;
		std::string asset = "DummyAsset";
		std::string description = "DummyDescription";
		std::string value = "DummyValue";

		TA_Base_Core::AlarmItem* ai_ptr = NULL;
		// Insert
		{
			void* data = TA_Base_Core::CSharedMem::alloc(sizeof(TA_Base_Core::AlarmItem));		
			try
			{

				ai_ptr = new(data)TA_Base_Core::AlarmItem(
					alarmId.c_str(),		// alarmID
					TA_Base_Core::ItemMillisecondTime(0, 0),	// transactiveTime
					severity,						// alarmSeverity
					asset.c_str(),					// assetName
					description.c_str(),			// alarmDescription
					value.c_str(),					// alarmValue
					TA_Base_Core::MMS_NONE,			// mmsState
					0,								// state
					TA_Base_Core::NoRelatedPlan,	// decisionSupportState,
					"",						// alarmAcknowledgedBy,
					"",						// alarmComments,
					false,					// isHeadOfAvalanche,
					false,					// isChildOfAvalanche,
					0,						// subsystemKey,
					0,						// messageTypeKey,
					0,						// locationKey,
					0,						// assocEntityTypeKey,
					0,						// assocEntityKey,
					"",						// entityName,
					""						// avalancheHeadID
					);

				if (NULL == ai_ptr)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "no enough memory .");
				}

				alarmIndexWithDuty->insert(ai_ptr);
				++g_pShAlarmInfo->gDutyAlarmCounter;
			}
			catch (...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, " unknow exception when insert new alarm in selfCheck, throw back to handle.");
				return false;
			}
		}

		bool checkResult = false;
		alarmindex_by_id::iterator itFind;
		// Read
		{
			alarmindex_by_id& find_id = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);
			itFind = find_id.find(const_cast<char*>(alarmId.c_str()));
			if (itFind == find_id.end())
			{	// can not found alarm we insert
				return false;
			}

			ai_ptr = *itFind;
			checkResult = ( alarmId.compare(ai_ptr->alarmID) == 0 
				&& severity == ai_ptr->alarmSeverity 
				&& asset.compare(ai_ptr->assetName) == 0 
				&& description.compare(ai_ptr->alarmDescription) == 0 
				&& value.compare(ai_ptr->alarmValue) == 0);
		}

		// Delete
		{
			alarmIndexWithDuty->erase(itFind);
			--g_pShAlarmInfo->gDutyAlarmCounter;
			ai_ptr->~AlarmItem();
			TA_Base_Core::CSharedMem::dealloc(ai_ptr);
			ai_ptr = NULL;
		}

		return checkResult;
	}

	int TA_SharedAlarm::deleteItem(TA_Base_Core::AlarmItem* pItem)
	{
		pItem->~AlarmItem();
		TA_Base_Core::CSharedMem::dealloc(pItem);

		//HP, flush out to the disk;
		//TA_Base_Core::Shmem::sync();
		return 0;	
	}

	int TA_SharedAlarm::handle_remap(EXCEPTION_POINTERS *ep)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "catch a SEH exception, Handle a remap. continue...");
		DWORD ecode = ep->ExceptionRecord->ExceptionCode;
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SEH exception code: [%d]", ecode);
		/*
		if (ecode != EXCEPTION_ACCESS_VIOLATION)
		{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "catch a SEH exception, EXCEPTION_CONTINUE_SEARCH. continue...");
		return EXCEPTION_CONTINUE_SEARCH;
		}
		void *addr = (void *) ep->ExceptionRecord->ExceptionInformation[1];
		if (TA_Base_Core::Shmem::getAllocator()->alloc().memory_pool().remap (addr) == -1)
		{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "catch a SEH exception, after remap(). continue...");
		TA_Base_Core::Shmem::remove();
		TA_Base_Core::Shmem::release();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "re-build the shared memory, EXCEPTION_CONTINUE_SEARCH. continue...");
		TA_Base_Core::Shmem::create();
		initDetail();
		return EXCEPTION_CONTINUE_SEARCH;
		}
		#if __X86__
		// This is 80x86-specific.
		ep->ContextRecord->Edi = (DWORD) addr;
		#elif __MIPS__
		ep->ContextRecord->IntA0 =
		ep->ContextRecord->IntV0 = (DWORD) addr;
		ep->ContextRecord->IntT5 =
		ep->ContextRecord->IntA0 + 3;
		#endif  //__X86__ 
		*/

		return EXCEPTION_EXECUTE_HANDLER;
	}

	//chenlei++
	//When error occurs, the handle_remap() function is called,
	//the handle_remap() function will recreate the map file and 
	//reallocate the memory. But the memory allocation code is within
	//init() function, and init() function needs to get guard.
	//Since every time the handle_remap() is called, the guard is already
	//there, so the init() function cannot be called. Thus, we copy 
	//the memory allocation part code of init() to create this reinit()
	//function. This function can only be used in handle_remap() function.
	void TA_SharedAlarm::initDetail()
	{
		// construct the AlarmInfo
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "initDetail coming in");
		try
		{
			// find first the sharedAlarm information
			g_pShAlarmInfo = TA_Base_Core::CSharedMem::find<SharedAlarmInfo>(AlarmInfo_Name.c_str());
			if (g_pShAlarmInfo == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to find Shared Alarm Info so we have to create it.");
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				g_pShAlarmInfo = pSegment->construct<SharedAlarmInfo>(AlarmInfo_Name.c_str())();
				if (NULL == g_pShAlarmInfo)
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,"Failed to create SharedAlarmInfo");
				else
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create SharedAlarmInfo.");
			}

			// find the alarmIndexWithDuty
			alarmIndexWithDuty = TA_Base_Core::CSharedMem::find<AlarmIndex>(AlarmIndexWithDuty_Name.c_str());
			if (alarmIndexWithDuty == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to find alarmIndexWithDuty so we have to create it.");
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				alarmIndexWithDuty = pSegment->construct<AlarmIndex>(AlarmIndexWithDuty_Name.c_str())(AlarmIndex::ctor_args_list(), pSegment->get_allocator<AlarmIndex>());
				if (NULL == alarmIndexWithDuty)
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,"Failed to create alarmIndexWithDuty with name %s.", AlarmIndexWithDuty_Name.c_str());
				else
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create alarmIndexWithDuty.");
			}

			// find alarmIndexWithFilter
			alarmIndexWithFilter = TA_Base_Core::CSharedMem::find<FilteredIndex>(AlarmIndexWithFilter_Name.c_str());
			if(alarmIndexWithFilter == NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to find alarmIndexWithFilter so we have to create it.");
				fixed_managed_winShmem *pSegment = TA_Base_Core::CSharedMem::getSegmentInstance();
				alarmIndexWithFilter = pSegment->construct<FilteredIndex>(AlarmIndexWithFilter_Name.c_str())[CLIENTS_NUM]();
				if (NULL == alarmIndexWithDuty)
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal,"Failed to create AlarmIndexWithFilter_Name with name %s.", AlarmIndexWithFilter_Name.c_str());
				else
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Create alarmIndexWithFilter.");
			}
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception ocurred while creating Shared Alarm Info");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "initDetail coming out");
	}

	//*/HP++
	unsigned long TA_SharedAlarm::removeLocationAlarmsInternal(unsigned long locationKey)
	{
		unsigned long removedCount = 0;
		SharedAlarmMonitor::getInstance().updateLoad(true, locationKey);
		try
		{
			alarmindex_by_location& location_index = boost::multi_index::get<SORT_BY_LOCATION>(*alarmIndexWithDuty);
			alarmindex_by_location::iterator lIter = location_index.lower_bound(boost::make_tuple(locationKey));
			alarmindex_by_location::iterator hIter = location_index.upper_bound(boost::make_tuple(locationKey));
			
			while(lIter != hIter)
			{
				removeFromAlarmIndex(lIter);
				++removedCount;
				lIter = location_index.lower_bound(boost::make_tuple(locationKey));
				hIter = location_index.upper_bound(boost::make_tuple(locationKey));
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"TA_SharedAlarm::removeLocationAlarmsInternal");
		}
        SharedAlarmMonitor::getInstance().updateLoad(false, locationKey);
		
		if(removedCount > 0)
		{
			notifyClients();
		}
		return removedCount;
	}

	unsigned long TA_SharedAlarm::removeLocationAlarms(unsigned long locationKey)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);

		unsigned long removedCount = 0;
		removedCount=removeLocationAlarmsInternal(locationKey);


		return removedCount;
	}

	bool TA_SharedAlarm::isUpdating()
	{
		if (g_pShAlarmInfo->g_Updating)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"TA_SharedAlarm::isUpdating");
		}
		return g_pShAlarmInfo->g_Updating;
	}
	
	bool TA_SharedAlarm::unmapSharedMem()
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		bool bRetVal = TA_Base_Core::CSharedMem::unmapSharedMem();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[shared alarm store] TA_Base_Core::CSharedmem::unmapSharedMem, ret=%d", bRetVal);
		return bRetVal;
	}

	bool TA_SharedAlarm::remapSharedMem()
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		bool bRetVal = TA_Base_Core::CSharedMem::mapSharedMem();
		// we have to bind the sharedAlarm Information because when the AlarmStore recreates the sharedAlarmInfo
		// clients must also release and renew the shared object pointers
		initDetail();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[shared alarm store] TA_Base_Core::CSharedmem::remapSharedMem, ret=%d", bRetVal);
		return bRetVal;
	}

	//zyr++: for map/unmap shared memory
	void TA_SharedAlarm::notifyClientUnmap()
	{		
		for(unsigned int i = 0; i < g_pShAlarmInfo->mappingClients; ++i)
		{
			const char* pcClientName = g_pShAlarmInfo->map_signal[i][1];
			// notify client to unmap the map file.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[shared alarm store] notifyClient evName=%s", pcClientName);
			notifyClient(pcClientName);
		}
		TA_Base_Core::Thread::sleep(2000);
	}

	void TA_SharedAlarm::notifyClientRemap()
	{
		for(unsigned int i = 0; i < g_pShAlarmInfo->mappingClients; ++i)
		{
			const char* pcClientName = g_pShAlarmInfo->map_signal[i][0];
			// notify client to remap the shared memory.
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[shared alarm store] notifyClient evName=%s", pcClientName);
			notifyClient(pcClientName);
		}
	}
	//++zyr

	void TA_SharedAlarm::notifyClients()
	{
		for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
		{
			HANDLE ev = OpenEvent(EVENT_ALL_ACCESS, FALSE, g_pShAlarmInfo->sc_signal[i][0]);
			if(ev!=NULL)
			{
				SetEvent(ev);
				CloseHandle(ev);
			}			
		}
	}

	int  TA_SharedAlarm::healthMonitor()
	{
		return SharedAlarmMonitor::getInstance().MonitorThreadsState();
		 
	}

	/************************************************************************/
	/* New alarmStore solution functions                                    */
	/************************************************************************/
	bool TA_SharedAlarm::insertAlarm(AlarmDataItem* alarmData)
	{	
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		
		bool bResult = false;
		if (NULL == alarmData) return bResult;
		
		SharedAlarmMonitor::getInstance().updateTimeBegin(alarmData->locationKey);
		{
			try
			{
				//HP++, refactor into a overloaded function
				bResult = insertAlarmInternal(alarmData);
				if (false == bResult)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "insertAlarm: Failed to insert alarm. AlarmID = %s", alarmData->alarmID.c_str());
				}
			}
			catch (...)
			{
				SharedAlarmMonitor::getInstance().updateTimeEnd( alarmData->locationKey);
				
				//pls do not add anyting here, or cause compiling error;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "insertAlarm addAlarmItem");
				// should never reach here, if do, throw a exception.
				
				if (!checkSharedMemoryInternal())
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Shared Memory check failed... Need to remap shared memory!!!");
				}
				throw;
			}
			SharedAlarmMonitor::getInstance().updateTimeEnd( alarmData->locationKey);
		}

		if (bResult)
			notifyClients(); 

		return bResult;	
	}



	bool TA_SharedAlarm::deleteAlarm(std::string& alarmID)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);

		bool bResult = false;
		alarmindex_by_id& indexByID = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);
		alarmindex_by_id::iterator itFind = indexByID.find(const_cast<char*>(alarmID.c_str()));

		if (itFind != indexByID.end())
		{
			TA_Base_Core::AlarmItem* pAlarm = *itFind;
			bool bUnAck = false;
			bUnAck = (strlen(pAlarm->alarmAcknowledgedBy) == 0);
			// remove from filter index
			for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if(alarmIndexWithFilter[i].index != NULL)
				{
					AlarmIndex::iterator it2 = alarmIndexWithFilter[i].index->find(pAlarm->alarmID);
					if (it2 != alarmIndexWithFilter[i].index->end())
					{
						alarmIndexWithFilter[i].index->erase(it2);
						if (true == bUnAck)
						{
							--(alarmIndexWithFilter[i].filteredIndexUnAckCount);
						}
					}
				}
			}

			pAlarm->~AlarmItem();
			TA_Base_Core::CSharedMem::dealloc(pAlarm);
			pAlarm = NULL;

			indexByID.erase(itFind);
			--(g_pShAlarmInfo->gDutyAlarmCounter);
			if (true == bUnAck)
			{ // update the unAcknowledged alarms
				--(g_pShAlarmInfo->gDutyIndexUnAckCount);
			}
		
			bResult = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Delete Alarm ID = %s : current duty alarm size = %ld", alarmID.c_str(), alarmIndexWithDuty->size());
			
			notifyClients();
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable to find alarm with ID = %s", alarmID.c_str());
		}

		return bResult;
	}

	bool TA_SharedAlarm::updateAlarm(AlarmDataItem* alarmUpdate)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
		bool bResult = false;
		if (NULL == alarmUpdate) return bResult;
	
		SharedAlarmMonitor::getInstance().updateTimeBegin( alarmUpdate->locationKey);
		AlarmIndex::iterator itFind;
		
		//HP++
		try
		{
			itFind = alarmIndexWithDuty->find(const_cast<char*>(alarmUpdate->alarmID.c_str()));
			if (itFind == alarmIndexWithDuty->end())
			{
				SharedAlarmMonitor::getInstance().updateTimeEnd( alarmUpdate->locationKey);
				//notifyClients(); // I think there is no need to notify clients
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Unable find alarm for update with ID =%s", alarmUpdate->alarmID.c_str());
				return bResult;				
			}
		}
		catch (...)
		{
			SharedAlarmMonitor::getInstance().updateTimeEnd( alarmUpdate->locationKey);
			//pls do not add anyting here, or cause compiling error;
            SharedAlarmMonitor::getInstance().updateSeqTime();
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError, "updateAlarm find exception");
			throw;
		}
        
		try
		{
			TA_Base_Core::AlarmItem* pFindAlarm =  *itFind;
			bool bAckBeforeUpdate = (strlen(pFindAlarm->alarmAcknowledgedBy) == 0);
			alarmIndexWithDuty->modify(itFind, updateAlarmDetail(*alarmUpdate));
			bool bAckAfterUpdate = (strlen(pFindAlarm->alarmAcknowledgedBy) == 0);

			if (bAckBeforeUpdate == true && bAckAfterUpdate == false)
			{ // Update unAcknowledge count decrement 1
				--(g_pShAlarmInfo->gDutyIndexUnAckCount);
			} 
			else if (bAckBeforeUpdate == false && bAckAfterUpdate == true)
			{ // Update unAcknowledge count increment 1
				++(g_pShAlarmInfo->gDutyIndexUnAckCount);
			}

			for(int i = 0; i < g_pShAlarmInfo->clients; ++ i)
			{
				if(alarmIndexWithFilter[i].index != NULL)
				{
					AlarmIndex::iterator filteritor = alarmIndexWithFilter[i].index->find(const_cast<char*>(alarmUpdate->alarmID.c_str()));
					if (filteritor != alarmIndexWithFilter[i].index->end())
					{
						alarmIndexWithFilter[i].index->modify(filteritor, updateAlarmDetail(*alarmUpdate));
						if (bAckBeforeUpdate == true && bAckAfterUpdate == false)
						{ // Update unAcknowledge count decrement 1
							--(alarmIndexWithFilter[i].filteredIndexUnAckCount);
						} 
						else if (bAckBeforeUpdate == false && bAckAfterUpdate == true)
						{ // Update unAcknowledge count increment 1
							++(alarmIndexWithFilter[i].filteredIndexUnAckCount);
						}
					}
				}
			}
			bResult = true;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Update successful AlarmID = %s", pFindAlarm->alarmID);
		}
		catch (...)
		{
			SharedAlarmMonitor::getInstance().updateTimeEnd( alarmUpdate->locationKey);
			SharedAlarmMonitor::getInstance().updateSeqTime();
			//pls do not add anyting here, or cause compiling error;
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError, "updateAlarmByID find");
			try
			{
				//logAlarm(alarmUpdate);
			}
			catch(...)
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "log error");	
			}
			throw;
		}
		SharedAlarmMonitor::getInstance().updateTimeEnd( alarmUpdate->locationKey);
		
		notifyClients();  
		
		return bResult;		
	}

	bool TA_SharedAlarm::insertAlarmlist(std::vector<AlarmDataItem*>& alarmlist)
	{
		ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);

		std::vector<AlarmDataItem*>::iterator itItem = alarmlist.begin(); 
		while(itItem != alarmlist.end())
		{
			TA_Base_Bus::AlarmDataItem* pAlarmItem = NULL;
			pAlarmItem = (TA_Base_Bus::AlarmDataItem*)*itItem;
			if (NULL == pAlarmItem)
			{
				itItem++;
				continue;
			}
			bool bRetVal = insertAlarmInternal(pAlarmItem);
			if(false == bRetVal)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "insertAlarmlist: Failed to insert alarm. AlarmID = %s", pAlarmItem->alarmID.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Handle=%d", g_pShAlarmInfo->gCounter);				
				g_pShAlarmInfo->gCounter++;
			}
			itItem++;
		}
		
		notifyClients();
		return true;
	}

	long TA_SharedAlarm::removeLocSubSystemDutyAlarms(unsigned long ulLocationKey, std::list<unsigned long>& subSystemKeyList, bool isATSAlarms /*= false*/)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "removeLocSubSystemDutyAlarms coming in");
		unsigned long removedCount = 0;
		SharedAlarmMonitor::getInstance().updateLoad(true, ulLocationKey);
		try
		{
			TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::WRITE_ACCESS);
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);

			alarmindex_by_location& subSystem_index = boost::multi_index::get<SORT_BY_LOCATION>(*alarmIndexWithDuty);
			alarmindex_by_location::iterator itrLoc;// = subSystem_index.lower_bound(ulSubSystemKey);

			std::list<unsigned long>::iterator itr = subSystemKeyList.begin();
			for (itr; itr != subSystemKeyList.end(); itr++)
			{
				T_indexAlarmLocRange rangeLoc = subSystem_index.equal_range( boost::make_tuple(ulLocationKey, isATSAlarms, ((unsigned long)*itr)) );

				itrLoc = rangeLoc.first;

				int nCountPerSubsystem = 0;
				while(itrLoc != rangeLoc.second)
				{
					// Remove from alarmIndex
					removeFromFilterIndex(itrLoc);

					++removedCount;
					itrLoc++;
					nCountPerSubsystem++;
				}

				// remove the range from dutyIndex
				subSystem_index.erase(rangeLoc.first, rangeLoc.second);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Remove alarms from LocationKey = %ld : SubSystemKey = %ld : Count = %ld", ulLocationKey, *itr, nCountPerSubsystem);
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"TA_SharedAlarm::removeLocSubSystemDutyAlarms");
		}
		SharedAlarmMonitor::getInstance().updateLoad(false, ulLocationKey);

		if(removedCount > 0)
		{
			notifyClients();
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "SubSystem Alarms Removed Count %d, from location %ld", removedCount, ulLocationKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugTrace, "removeLocSubSystemDutyAlarms coming out");
		return removedCount;		
	}

	/************************************************************************/
	/* New Display solution functions                                       */
	/************************************************************************/
	void TA_SharedAlarm::getDisplayData(TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& info, std::vector<AlarmDataItem*>& pDisplayData, SORT_COLUMN nCol, int nPos, bool bSortAccend, ULONG ulIndexBegin, ULONG ulCacheSize, bool bUpdateIndexInfo)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);

 		try
 		{			
			if (nPos < 0 || nPos > g_pShAlarmInfo->clients) return;

			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return;
 			}
			
			if (bUpdateIndexInfo)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"before calling UpdateIndexInfo");
				updateIndexInfoInternal(info, nPos);
				if (info.totalAlarms == 0)
				{
					freeDisplayData(pDisplayData); // free the current display data
					return;
				}
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,"after calling UpdateIndexInfo");
			}
	
			AlarmListT vecTempData;
			getIndexWithSortColInternal(&vecTempData, nCol, nPos, bSortAccend, ulIndexBegin, ulCacheSize);
			
			ULONG nCacheCount = 0;
			freeDisplayData(pDisplayData); // free the current display data

			AlarmListT::iterator pos = vecTempData.begin();
			for (pos; pos != vecTempData.end(); pos++)
			{
				AlarmDataItem* pAlarmItem = _newAlarmDataItem((*pos));
				
				if (pAlarmItem != NULL)
				{
					pDisplayData.push_back(pAlarmItem);
					nCacheCount++;
				}
			}	
 		}
 		catch(...)
 		{
 			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to get display data.");
 		}
	}

	AlarmDataItem* TA_SharedAlarm::getAlarmItemByID(std::string strAlarmID)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);

		AlarmDataItem* pAlarmItem = NULL;

		try
		{
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return pAlarmItem;
 			}

			alarmindex_by_id& indexByID = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);
			alarmindex_by_id::iterator itFind = indexByID.find(const_cast<char*>(strAlarmID.c_str()));
			
			if (itFind != indexByID.end())
			{
				pAlarmItem = _newAlarmDataItem(*itFind);
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to find alarm with ID = %s", strAlarmID.c_str());
			}			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to retrieve alarm item.");
		}

		return pAlarmItem;
	}

	AlarmDataItem* TA_SharedAlarm::getAlarmItemByIndex(ULONG ulIndex, SORT_COLUMN nCol, int csPos, bool bSortAccend)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		
		AlarmDataItem* pAlarmItem = NULL;
		try
		{			
			if (csPos < 0 || csPos > g_pShAlarmInfo->clients) return pAlarmItem;

			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return pAlarmItem;
 			}

			AlarmListT vecTempData;
			getIndexWithSortColInternal(&vecTempData, nCol, csPos, bSortAccend, ulIndex, 1);
			
			if (vecTempData.size() != 0)
			{	
				AlarmListT::iterator itr = vecTempData.begin();
				pAlarmItem = _newAlarmDataItem(*itr);
			}

		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to retrieve alarm item.");
		}
		return pAlarmItem;
	}

	std::vector<TA_Base_Core::AlarmModifyItem*> TA_SharedAlarm::getAllModifyItems(SORT_COLUMN nCol, int csPos)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		std::vector<TA_Base_Core::AlarmModifyItem*> vectModifyItems;
		try
		{			
			if (csPos < 0 || csPos > g_pShAlarmInfo->clients) return vectModifyItems;

			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return vectModifyItems;
 			}

			AlarmIndex* pActionIndex = NULL;
			if (NULL == alarmIndexWithFilter[csPos].index)
			{
				pActionIndex = alarmIndexWithDuty;
			}
			else
			{
				pActionIndex = alarmIndexWithFilter[csPos].index;
			}

			AlarmIndex::iterator itr = pActionIndex->begin();
			for (itr; itr != pActionIndex->end(); itr++)
			{
				TA_Base_Core::AlarmModifyItem* pmodifyItem = NULL;

				pmodifyItem = new TA_Base_Core::AlarmModifyItem((*itr)->alarmID, (*itr)->alarmAcknowledgedBy, (*itr)->locationKey);

				vectModifyItems.push_back(pmodifyItem);
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to retrieve alarmModify items.");
		}
		return vectModifyItems;
	}

	long TA_SharedAlarm::findItemPosition(std::string strAlarmID, SORT_COLUMN nCol, int nPos, bool bSortAccend)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		try
		{			
			if (nPos < 0 || nPos > g_pShAlarmInfo->clients) return -1;

			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return -1;
 			}
			
			AlarmListT vecTempData;
			getIndexWithSortColInternal(&vecTempData, nCol, nPos, bSortAccend, 0, alarmIndexWithDuty->size());
			
			ULONG ulIndex = 0;
			
			AlarmListT::iterator itFind = vecTempData.begin();
			for (itFind; itFind != vecTempData.end(); itFind++)
			{
				if (lstrcmpi(strAlarmID.c_str(), (*itFind)->alarmID) == 0)
				{
					return ulIndex; // Item has been found
				}
				ulIndex++;
			}
			
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to find alarm item position in the display.");
		}
		return -1;
	}

	void TA_SharedAlarm::getRangeAlarmID(std::set<std::string>& setAlarmIDs, ULONG ulBegin, ULONG ulEnd, SORT_COLUMN nCol, int nPos, bool bSortAccend)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		try
		{
			if (nPos < 0 || nPos > g_pShAlarmInfo->clients) return;

			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return;
 			}
			
			AlarmListT vecTempData;
			getIndexWithSortColInternal(&vecTempData, nCol, nPos, bSortAccend, ulBegin, (ulEnd - ulBegin)+1);

			if (vecTempData.size() == 0 /*|| vecTempData.size()-1 < ulBegin / *|| vecTempData.size()* /*/) return;
			
			TA_Base_Core::AlarmItem* pAlarm = NULL;
			
			AlarmListT::iterator itr = vecTempData.begin();
			for(itr; itr != vecTempData.end(); itr++)
			{
				pAlarm = (*(itr));
				if (setAlarmIDs.find(pAlarm->alarmID) == setAlarmIDs.end())
				{
					setAlarmIDs.insert(pAlarm->alarmID);
				}
			}			
		}	
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to retrieve ranged alarm IDs.");
		}
	}

	void TA_SharedAlarm::getAvalanchedChildAlarms(int csPos, std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, false);
		try
		{			
			ActionFlagGuard actionGuard(g_pShAlarmInfo->g_Updating);
 			if (SharedAlarmGuard.acquire(TIME_OUT_MILLISECONDS, TA_Base_Core::READ_ACCESS)==false)
 			{
 				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "SharedAlarmGuard.acquire time out");
 				return;
 			}

			AlarmIndex* pActionIndex = NULL;
			if (NULL == alarmIndexWithFilter[csPos].index)
			{
				pActionIndex = alarmIndexWithDuty;
			}
			else
			{
				pActionIndex = alarmIndexWithFilter[csPos].index;
			}

			if (pActionIndex->size() != 0)
			{
			 	// Iterate through the alarms and get those child alarms that are related to this head
			 	for ( AlarmIndex::iterator itr = pActionIndex->begin(); itr!=pActionIndex->end(); ++itr )
			 	{
			 		if (('C' == *((*itr)->avalanche)) && (0 == avalanchedHeadID.compare((*itr)->avalancheHeadID)))
			 		{
			 			vecChildAlarms.push_back(std::string((*itr)->alarmID));
			 		}
			 	}
			}
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception occurred while trying to retrieve related avalance alarms.");
		}
	}

	std::vector<TA_Base_Core::AlarmModifyItem*> TA_SharedAlarm::getAlarmModifyItem(std::set<std::string >& vecAlarmIDs)
	{
		TA_Base_Core::ProcessRWGuard   SharedAlarmGuard (m_SharedAlarmDutyLock, TA_Base_Core::READ_ACCESS);
		
		std::vector<TA_Base_Core::AlarmModifyItem*> vectModifyItems;

		alarmindex_by_id& indexByID = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);
		alarmindex_by_id::iterator itFind; // =	indexByID.find(const_cast<char*>(strAlarmID.c_str()));

		std::set<std::string >::iterator itrID = vecAlarmIDs.begin();
		for (itrID; itrID != vecAlarmIDs.end(); itrID++)
		{
			std::string strAlarmID = *itrID;
			itFind = indexByID.find(const_cast<char*>(strAlarmID.c_str()));

			if (itFind != indexByID.end())
			{
				TA_Base_Core::AlarmModifyItem* pAlarmModifyItem = NULL;
				pAlarmModifyItem = new TA_Base_Core::AlarmModifyItem((*itFind)->alarmID, (*itFind)->alarmAcknowledgedBy, (*itFind)->locationKey);
				
				vectModifyItems.push_back(pAlarmModifyItem);
			}
		}

		return vectModifyItems;
	}


	AlarmDataItem* TA_SharedAlarm::_newAlarmDataItem(TA_Base_Core::AlarmItem* pAlarm)
	{
		AlarmDataItem* pAlarmDataItem = NULL;
		if (pAlarm != NULL)
		{
			try
			{
				pAlarmDataItem = new AlarmDataItem();

				pAlarmDataItem->alarmID				= pAlarm->alarmID;
				pAlarmDataItem->alarmAcknowledgedBy = pAlarm->alarmAcknowledgedBy;
				pAlarmDataItem->alarmComments		= pAlarm->alarmComments;
				pAlarmDataItem->alarmDescription	= pAlarm->alarmDescription;
				pAlarmDataItem->alarmSeverity		= pAlarm->alarmSeverity;
				pAlarmDataItem->alarmValue			= pAlarm->alarmValue;
				pAlarmDataItem->assetName			= pAlarm->assetName;
				pAlarmDataItem->assocEntityKey		= pAlarm->assocEntityKey;
				pAlarmDataItem->assocEntityTypeKey	= pAlarm->assocEntityTypeKey;
				pAlarmDataItem->avalanche			= pAlarm->avalanche;
				pAlarmDataItem->avalancheHeadID		= pAlarm->avalancheHeadID;
				pAlarmDataItem->decisionSupportState = pAlarm->decisionSupportState;
				pAlarmDataItem->entityName			= pAlarm->entityName;
				pAlarmDataItem->locationKey			= pAlarm->locationKey;
				pAlarmDataItem->messageTypeKey		= pAlarm->messageTypeKey;
				pAlarmDataItem->mmsState			= pAlarm->mmsState;
				pAlarmDataItem->state				= pAlarm->state;
				pAlarmDataItem->subsystemKey		= pAlarm->subsystemKey;
				pAlarmDataItem->transactiveTime		= pAlarm->transactiveTime;
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown exception error occurred while trying to create new AlarmItem.");
			}
		}
	
		return pAlarmDataItem;
	}

	void TA_SharedAlarm::freeDisplayData(std::vector<AlarmDataItem*>& pDisplayData)
	{
		std::vector<AlarmDataItem*>::iterator itr = pDisplayData.begin();

		for (itr; itr != pDisplayData.end(); itr++)
		{
			delete (*itr);
			(*itr) = NULL;
		}
		pDisplayData.clear();
	}

	AlarmDataItem* TA_SharedAlarm::getAlarm(std::string alarmID)
	{
		AlarmDataItem* pFindITem = NULL;

		alarmindex_by_id& indexByID = boost::multi_index::get<NOSORT_BY_HASH>(*alarmIndexWithDuty);
		alarmindex_by_id::iterator itFind =	indexByID.find(const_cast<char*>(alarmID.c_str()));

		if (itFind != indexByID.end())
		{
			pFindITem = _newAlarmDataItem(*itFind);
		}

		return pFindITem;
	}

}
