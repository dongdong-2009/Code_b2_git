// AlarmsCollection.cpp: implementation of the AlarmsCollection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "alarmgenerator.h"
#include "AlarmsCollection.h"
#include "agDebug.h"
#include "resource.h"

#include "core\corba\src\CorbaUtil.h"
#include "core\alarm\src\AlarmHelperManager.h"
#include "core\alarm\src\AlarmModificationHelper.h"
#include "core\alarm\src\NonUniqueAlarmHelper.h"
//#include "core\data_access_interface\src\AlarmAccessFactory.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\NoAlarmAgentException.h"
#include "core\uuid\src\TAuuid.h"
#include "core\message\src\MessageConfig.h"
#include "core/alarm/src/AlarmConfig.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AlarmsCollection::AlarmsCollection()
: m_bRunning( false ), m_UpdateObserver(0)
{
	InitializeCriticalSection(&m_cs);

	// clear thread handles array
	ZeroMemory(m_hThreads,sizeof(HANDLE)*256);
	m_iUncloseAlarmSize = 0;
	m_nThreadsCount=0;

	
}

void AlarmsCollection::setObserver(IUpdateObserver * pUpdateObserver)
{
	m_UpdateObserver = pUpdateObserver;
}

AlarmsCollection::~AlarmsCollection()
{
	// clear alarms queue
	while (!m_alarms.empty())
	{
		delete m_alarms.front();
		m_alarms.pop();
	}

	for (int i = 0;i<m_nThreadsCount;i++)
	{
		if (m_hThreads[i])
		{
			TerminateThread( m_hThreads[i], 0 );
			CloseHandle(m_hThreads[i]);
			m_hThreads[i] = 0;
		}
	}
	// clear critical section 
	DeleteCriticalSection(&m_cs);
}

void AlarmsCollection::add(agAlarmData* alarm)
{
	EnterCriticalSection(&m_cs);
	m_alarms.push(alarm);
	LeaveCriticalSection(&m_cs);

	// gui
	updateGUI();
}




int AlarmsCollection::run()
{
	size_t cntTotal = 0;
	int cnt = 0;
	std::string uuid;
	agAlarmData* alarm = 0;
	TA_Base_Core::AlarmDetailCorbaDef* p_aStoreAlarm = NULL;


	// loop
	while (m_bRunning)
	{
		// check, get
		EnterCriticalSection(&m_cs);
		alarm = 0;
		if(!m_alarms.empty())
		{
			alarm = m_alarms.front();
			if(alarm)
			{
				if( time(0) - alarm->m_tSubmit > m_tExpiry )
				{
					m_alarms.pop();
				}
				else
				{
					alarm = 0;
				}
			}
		}
		LeaveCriticalSection(&m_cs);
		
		// acknowledege and close alarm
		if(m_bSendAlarmStore && alarm)
		{
			TA_Base_Core::AlarmUpdateCorbaDef updateAlarm;
			updateAlarm.alarmHasBeenClosed=false;
			updateAlarm.alarmHasBeenAcknowledged=true;
			updateAlarm.alarmID=alarm->m_strID.c_str();
			updateAlarm.ackTime;
			updateAlarm.alarmAcknowledgedBy = std::string("TransActiveSystemOperator").c_str();;
			updateAlarm.alarmComments = std::string("").c_str();
			updateAlarm.alarmDescription = std::string("").c_str();
			updateAlarm.locationKey = 0;
			updateAlarm.mmsState = TA_Base_Core::MMS_AUTO;
			updateAlarm.messageTypeKey = 0;
			updateAlarm.messageSequenceNumber = 0;
			updateAlarm.decisionSupportState = TA_Base_Core::NoRelatedPlan;
			updateAlarm.assetName = std::string("").c_str();
			updateAlarm.closeValue = std::string("").c_str();
			updateAlarm.messageContext = std::string("").c_str();
			updateAlarm.session = std::string("").c_str();

			m_UpdateObserver->UpdateToAlarmStore(updateAlarm);
			
			Sleep(10); 

			updateAlarm.alarmHasBeenClosed = true;
			m_UpdateObserver->UpdateToAlarmStore(updateAlarm);

			// gui update
			updateGUI();	
		}
		else if (alarm)
		{
			acknowledge(alarm);
			close(alarm);
		
			// gui update
			updateGUI();
		}
		else
		{
			Sleep(100);
		}

		if (alarm != 0)
		{
			delete alarm;
			alarm = NULL;
		}
	}
	
	return 1;
}

TA_Base_Core::AlarmDetailCorbaDef* AlarmsCollection::createAlarmWithParams( const TA_Base_Core::MessageType& alarmType,
														unsigned long entityKey,
														unsigned long entityTypeKey,
														const TA_Base_Core::DescriptionParameters& description,
														const std::string& entityName, 
														unsigned long locationKey, 
														unsigned long subsystemKey,
														unsigned long alarmSeverityKey,
														timeb sourceTime,
														const std::string& assetName,
														bool isUnique,
														TA_Base_Core::MmsStateType mmsState)
{
	// Create the AlarmDetail object
	TA_Base_Core::AlarmDetailCorbaDef* alarm = new TA_Base_Core::AlarmDetailCorbaDef();

	// Fill in all the necessary default fields
	alarm->alarmAcknowledgedBy = CORBA::string_dup("");
	alarm->alarmID = CORBA::string_dup( TA_Base_Core::TA_Uuid::generateUUID().c_str() );
	alarm->state = TA_Base_Core::AlarmOpen;
	alarm->transactiveTime.time = sourceTime.time;
	alarm->transactiveTime.millitm = sourceTime.millitm;
	alarm->ackTime = 0;
	alarm->closeTime = 0;
	alarm->alarmComments = CORBA::string_dup("");
	alarm->parentAlarmID = CORBA::string_dup("");
	alarm->avalancheHeadID = CORBA::string_dup("");

	// Fill in the provided fields
	// Wenguang TD18266
	std::stringstream stringStream;
	stringStream << alarmType.getTypeKey();
	stringStream >> alarm->messageTypeKey;
	alarm->messageContext = CORBA::string_dup( alarmType.getContextObj().getContextName().c_str() );
	alarm->assocEntityKey = entityKey;
	alarm->assocEntityTypeKey = entityTypeKey;
	//alarm->entityName = CORBA::string_dup(entityName.c_str());
	// Wenguang++ Alarm Message Size Reduction
	alarm->entityName = CORBA::string_dup("");
	alarm->locationKey = locationKey;
	alarm->sourceTime.time = sourceTime.time;
	alarm->sourceTime.millitm = sourceTime.millitm;

	// Fill in the display time as the source time upon initialisation
	alarm->displayTime = alarm->sourceTime;

	alarm->subsystemKey = subsystemKey;
	alarm->alarmSeverity = alarmSeverityKey;
	alarm->assetName = CORBA::string_dup(assetName.c_str());
	alarm->messageSequenceNumber = 0;  // Used by agent notifications only

	// Get the alarm parameters
	std::string alarmParameters;
	TA_Base_Core::gGetStringFromParameters( description, alarmParameters );
	alarm->alarmParameterList = CORBA::string_dup( alarmParameters.c_str() );
	alarm->alarmValue = CORBA::string_dup( getValueFromParameters( alarmType, description ).c_str() );

	// Create the alarm description
	//alarm->alarmDescription = CORBA::string_dup(
	//	AlarmTypeTableAccessor::getInstance().getExpandedDescription( alarmType, description ).c_str() );

	// Wenguang++ Alarm Message Size Reduction
	alarm->alarmDescription = CORBA::string_dup("");

	alarm->isHeadOfAvalanche = CORBA::Boolean( false );
	alarm->isChildOfAvalanche = CORBA::Boolean( false );
	alarm->isUniqueAlarm = CORBA::Boolean( isUnique );

	alarm->decisionSupportState = TA_Base_Core::NoRelatedPlan;
	//Mintao++ TD15302
	if (TA_Base_Core::AlarmTypeTableAccessor::getInstance().isSystemAlarm(alarmType))
	{
		if (TA_Base_Core::AlarmTypeTableAccessor::getInstance().isMMSEnabled(alarmType))
		{
			//alarm->mmsState = TA_Base_Core::MMS_AUTO;                        
			//TD17862 marc_bugfix support other mms types for System alarms (semi-automatic, none)
			switch(TA_Base_Core::AlarmTypeTableAccessor::getInstance().getRelatedMMSType(alarmType)) 
			{
			case  TA_Base_Core::IAlarmData::TYPE_NONE:
				LOG( SourceInfo,
					TA_Base_Core::DebugUtil::GenericLog,
					TA_Base_Core::DebugUtil::DebugInfo,
					"marc>>current alarm is mms none ");

				alarm->mmsState = TA_Base_Core::MMS_NONE;
				break;
			case TA_Base_Core::IAlarmData::TYPE_SEMI:
				alarm->mmsState = TA_Base_Core::MMS_SEMI;
				LOG( SourceInfo,
					TA_Base_Core::DebugUtil::GenericLog,
					TA_Base_Core::DebugUtil::DebugInfo,
					"marc>>current alarm is mms semi ");

				break;
			case TA_Base_Core::IAlarmData::TYPE_AUTO:
			default:
				LOG( SourceInfo,
					TA_Base_Core::DebugUtil::GenericLog,
					TA_Base_Core::DebugUtil::DebugInfo,
					"marc>>current alarm is mms auto  ");

				alarm->mmsState = TA_Base_Core::MMS_AUTO;
				break;
			}
		}
		else
		{
			alarm->mmsState = mmsState;
		}
	}
	else
	{
		alarm->mmsState = mmsState;
	}
	//Mintao++ TD15302

	return alarm;
}

std::string AlarmsCollection::getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description)
{
	std::string value("");

	// check whether it is system alarm return ALARM if yes		
	bool isSystem = TA_Base_Core::AlarmTypeTableAccessor::getInstance().isSystemAlarm(type);
	std::string alarmType = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmTypeName(type);


	if (isSystem)
	{
		// Obtain the value from DB configuration
		// Attribute's name = "AlarmValue" in alarm type data
		value = TA_Base_Core::AlarmTypeTableAccessor::getInstance().getAlarmValue(type);
	}
	//jinhua++TD17255
	else if (alarmType.find( "CctvTriggeringAlarm", 0  ) != std::string::npos)// this is CCTVTriggeringAlarm** type 		
	{
		value = TA_Base_Core::AlarmTypeTableAccessor::DEFAULT_SYSTEM_ALARM_ALARM_VALUE;
	}
	//++jinhua TD17255
	else // this is not system alarm and can be a datapoint type
	{        
		TA_Base_Core::ConstDescriptionParametersIterator valueParam;
		bool found=false;
		for (valueParam=description.begin(); valueParam!=description.end(); ++valueParam)
		{
			if ( (*valueParam)->name.compare("DataPointValue") == 0)
			{
				found = true;
				break;
			}
		}

		// TD 12785 Only display the value without the alarm type if there is parameter value
		// if not, use the alarm type description
		if(found==true && valueParam != description.end())
		{
			// Check if the text contains DpGeneral, DpFalse and DpTrue
			// Take them out // this is a quick fix for TD 12785
			bool isBoolDatapt = false;

			//TD16813++
			if(alarmType.find("DpGeneral",0) != std::string::npos || alarmType.find("DpFalse",0) !=std::string::npos || alarmType.find("DpTrue",0) !=std::string::npos )
				isBoolDatapt = true;


			if (isBoolDatapt)
			{
				// value is just the value of the bool datapoint
				value = (*valueParam)->value.c_str();
			}
			else
			{
				// value = alarm type + " "(space) + value of the datapoint
				value = alarmType + " ";
				value += (*valueParam)->value.c_str();
			}


		}
		// there is no value
		else 
		{
			// append the alarm type to value
			value = alarmType;
		}

	}

	return value;
}



void AlarmsCollection::acknowledge(agAlarmData* alarm)
{
	TA_Base_Core::AlarmDetailCorbaDef* alarmMsg = 0;
	std::vector<TA_Base_Core::AlarmDetailCorbaDef*> selectedCorbaAlarms;

	alarmMsg = alarmDataToAlarmMessage(alarm);

	agDebug("================================================");
	agDebug("Alarm Acknowledge-> Pkey: ",alarm->m_strID.c_str());
	agDebug("Alarm Acknowledge-> Location: ",alarm->m_nLocationKey);
	agDebug("Alarm Acknowledge-> Entity Key: ",alarm->m_nEntityKey);
	agDebug("Alarm Acknowledge-> Entity Type Key: ",alarm->m_nEntityTypeKey);
	agDebug("================================================");
	
	selectedCorbaAlarms.push_back(alarmMsg);
	
	try
	{
		// To simplify implementation and testing
		// Use the super session id instead of requesting one from authentication agent
		if (!selectedCorbaAlarms.empty()&&(m_SessionId.empty()==false)) 
		{
			TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().acknowledgeAlarms(selectedCorbaAlarms, m_SessionId.c_str());
		}
		else if(!selectedCorbaAlarms.empty()) 
		{
			TA_Base_Core::AlarmHelperManager::getInstance().getModificationAlarmHelper().acknowledgeAlarms(selectedCorbaAlarms, "TransActive Super Session ID");
		}
			
		
	}
	catch(TA_Base_Core::NoAlarmAgentException& e)
	{
		agDebug("***** ERROR ***** : ",e.what());
	}
	catch(...)
	{
		std::string reason ("Acknowledge Error is caught." );
		AfxMessageBox(reason.c_str());
	}
	
	// delete the alarm detail and clear the vector
	for (unsigned int j=0; j<selectedCorbaAlarms.size(); ++j)
	{
		delete (selectedCorbaAlarms[j]);
	}
	selectedCorbaAlarms.clear();
}

void AlarmsCollection::close(agAlarmData* alarm)
{
	TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(alarm->m_strID, alarm->m_nLocationKey );
}








//
// alarmDataToAlarmMessage
//
TA_Base_Core::AlarmDetailCorbaDef* AlarmsCollection::alarmDataToAlarmMessage( agAlarmData* p_alarmData )
{
    TA_Base_Core::AlarmDetailCorbaDef* alarmMsg = new TA_Base_Core::AlarmDetailCorbaDef();


    alarmMsg->subsystemKey = p_alarmData->m_nSubsystemKey;
    alarmMsg->locationKey = p_alarmData->m_nLocationKey;
	alarmMsg->entityName = CORBA::string_dup("");
    alarmMsg->assocEntityTypeKey = p_alarmData->m_nEntityTypeKey;
    alarmMsg->alarmID = CORBA::string_dup(p_alarmData->m_strID.c_str());
	alarmMsg->alarmDescription = CORBA::string_dup("");
    alarmMsg->parentAlarmID = CORBA::string_dup("");
    alarmMsg->messageSequenceNumber = 0;
	alarmMsg->state = TA_Base_Core::AlarmOpen;

	alarmMsg->mmsState = TA_Base_Core::MMS_NONE;


	alarmMsg->isHeadOfAvalanche = false;
	alarmMsg->isChildOfAvalanche = false;

	alarmMsg->isUniqueAlarm = false;

	// TD 10687 - getOperatorDelimList uses a monster query on the event table...
    // Until we have a better (and faster!) way to retrieve session override details
    // it should remain disabled.
    std::string tempAckedBy;
	tempAckedBy = "Alarm Generator";
	alarmMsg->alarmAcknowledgedBy = CORBA::string_dup(tempAckedBy.c_str());
	alarmMsg->alarmAcknowledgedSession = CORBA::string_dup("Alarm Generator Session");
	alarmMsg->decisionSupportState = convertToDecisionSupportStateType(TA_Base_Core::AlarmData::NO_ASSOC_PLAN);

	alarmMsg->alarmValue = CORBA::string_dup(""); // leave it empty

    return alarmMsg;
}




//
// convertToDecisionSupportStateType
//
TA_Base_Core::DecisionSupportStateType AlarmsCollection::convertToDecisionSupportStateType(const std::string& stateTypeString)
{
    TA_Base_Core::DecisionSupportStateType decisionStateType; 
	
	//    if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
	//    {
	//        decisionStateType = TA_Base_Core::RelatedAutomaticPlanNotRun;
	//    }
	if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
	{
        decisionStateType = TA_Base_Core::RelatedSemiAutomaticPlanNotRun;
	}
	//    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
	//    {
	//        decisionStateType = TA_Base_Core::RelatedPlanHasBeenRun;
	//    }
	//    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
	//    {
	//        decisionStateType = TA_Base_Core::RelatedPlanHasBeenPostponed;
	//    }
    else if( 0 == stateTypeString.compare(TA_Base_Core::AlarmData::NO_ASSOC_PLAN) )
    {
        decisionStateType = TA_Base_Core::NoRelatedPlan;
    }
	
    return decisionStateType;
}

TA_Base_Core::IAlarmData* AlarmsCollection::fetch()
{
	return NULL;
}

DWORD WINAPI AlarmsCollectionThreadProc(LPVOID pAlarmsCollection)
{
	AlarmsCollection* p = (AlarmsCollection*)pAlarmsCollection;

	p->run();

	return 0;
}

int AlarmsCollection::start(time_t tExpiry, unsigned int nThreadCnt )
{
	int i = 0;

	// threads running flag
	m_bRunning = true;

	// threads count
	m_nThreadsCount = nThreadCnt;

	// prepare threads
	/*
	for (i = 0;i<512;i++)
	{
		if (m_hThreads[i])
		{
			TerminateThread( m_hThreads[i], 0 );
			CloseHandle(m_hThreads[i]);
			m_hThreads[i] = 0;
		}
	}
	*/
	
	// set expiry time
	m_tExpiry = tExpiry;

	// go!
	for (i=0;i<nThreadCnt;i++)
	{
		m_hThreads[i] = CreateThread( NULL, NULL, AlarmsCollectionThreadProc, this, NULL, NULL );
		//SetThreadPriority( m_hThreads[i],THREAD_PRIORITY_ABOVE_NORMAL);
	}

	

	return 0;
}

unsigned int AlarmsCollection::getCount()
{
	size_t cnt = 0;
	
	EnterCriticalSection(&m_cs);
	cnt = m_iUncloseAlarmSize + m_alarms.size();
	LeaveCriticalSection(&m_cs);

	return (unsigned int)cnt;
}


void AlarmsCollection::updateGUI()
{
	CWnd* pText = NULL;
	char str[20];
	unsigned int cnt = 0;
	
	cnt = getCount();
	ltoa(cnt,str,10);
	pText= AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDIT_UNCLOSED_AUTO_ALARM_COUNT);
	pText->SetWindowText(str);
}

void AlarmsCollection::add(std::string uuid)
{
	EnterCriticalSection(&m_cs);
	m_uuids.push_back(uuid);
	LeaveCriticalSection(&m_cs);
}


TA_Base_Core::IAlarmData* AlarmsCollection::getAlarmFromDatabase(const std::string& p_key)
{
	TA_Base_Core::IAlarmData* alarm=NULL;
	
	try
	{
//		alarm = TA_Base_Core::AlarmAccessFactory::getInstance().getAlarm(p_key);
	}
	catch(TA_Base_Core::DataException& ex)
	{
		if (alarm!=NULL)
		{
			delete alarm;
			alarm = NULL;
		}
		agDebug("***** ERROR ***** : ", ex.what());
	}
	catch(TA_Base_Core::DatabaseException& ex)
	{
		if (alarm!=NULL)
		{
			delete alarm;
			alarm = NULL;
		}
		agDebug("***** ERROR ***** : ", ex.what());
	}
	
	return alarm;
}

void AlarmsCollection::stop()
{
	m_bRunning = false;
}

bool AlarmsCollection::IsStopped()
{
	DWORD dwRes = WaitForMultipleObjects(m_nThreadsCount,m_hThreads,TRUE,0);
	if (dwRes == WAIT_OBJECT_0)
	{
		return true;
	}
	else
	{
		return false;
	}
	
}
