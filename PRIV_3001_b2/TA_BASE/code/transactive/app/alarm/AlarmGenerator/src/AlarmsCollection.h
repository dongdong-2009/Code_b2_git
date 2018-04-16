// AlarmsCollection.h: interface for the AlarmsCollection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMSCOLLECTION_H__718495AD_3AF6_4967_97BF_0F3918B6F010__INCLUDED_)
#define AFX_ALARMSCOLLECTION_H__718495AD_3AF6_4967_97BF_0F3918B6F010__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <queue>

#include "core\data_access_interface\src\AlarmData.h"
#include "core\alarm\IDL\src\AlarmMessageCorbaDef.h"
#include "agAlarmData.h"
#include "core\message\types\DataPointAlarms_MessageTypes.h"
#include "app\alarm\AlarmGenerator\src\IUpdateObserver.h"
#include "core\message\src\NameValuePair.h"

typedef std::queue<agAlarmData*> TypeAlarmsQueue;

class AlarmsCollection  
{

public:
	AlarmsCollection();
	virtual ~AlarmsCollection();
	void setObserver(IUpdateObserver * pUpdateObserver);

	
public:
	unsigned int m_nThreadsCount;
	bool IsStopped();
	void stop();
	time_t m_tExpiry;
	unsigned int getCount();
	void updateGUI();
	void setSendAlarmStore(bool bSendAlarmStore)
	{
		m_bSendAlarmStore = bSendAlarmStore;
	}
	int start(time_t tExpiry, unsigned int nThreadCnt=1);
	TA_Base_Core::IAlarmData* fetch();
	int run();
	
	void add(agAlarmData* );
	void add(std::string uuid);
	void acknowledge(agAlarmData* alarm);
	void close(agAlarmData* alarm);
	void addAlarmSize()
	{
		m_iUncloseAlarmSize++;
	}
	std::string getAlarmSessonId()
	{
		return m_SessionId;
	}
	void setAlarmSessonId(std::string StrSessionId)
	{
		m_SessionId = StrSessionId;
	}
	TA_Base_Core::AlarmDetailCorbaDef* alarmDataToAlarmMessage( agAlarmData* p_alarmData );
	TA_Base_Core::DecisionSupportStateType convertToDecisionSupportStateType(const std::string& stateTypeString);
	TA_Base_Core::IAlarmData* getAlarmFromDatabase(const std::string& p_key);
	/** 
	* createAlarmWithParams
	*
	* This function will populate the default alarm parameters
	* such as:
	*		alarmAcknowledgedBy = ""
	*		alarmOpen = true
	*		transactiveTime = 0
	*		alarmID = TA_Base_Core::TA_Uuid::generateUUID()
	*		ackTime = 0
	*		sourceTime = time( NULL )
	*		closeTime = 0
	*		alarmComments = ""
	*		parentAlarmID = ""
	*		isUnique = true
	*
	* @param  alarmType    The type of alarm as defined in the specific Message Type header file
	* @param  entityKey    The entity that has raised the alarm
	* @param  entityTypeKey    The entityType of the entity that has raised the alarm
	* @param  description    The parameters to be inserted into the description as defined in the AlarmType table
	* @param  entityName    The name of the entity
	* @param  locationKey    The location the entity belongs to
	* @param  subsystemKey    The subsystem the entity belongs to
	* @param  alarmSeverityKey    The severity to associate with this Alarm
	* @param	sourceTime	The time the alarm was first detected.
	* @param	isUnique	A boolean to indicate whether this alarm is unique -> default is unique
	*
	* @return AlarmDetailCorbaDef* the populated alarm
	*/
	TA_Base_Core::AlarmDetailCorbaDef* createAlarmWithParams( const TA_Base_Core::MessageType& alarmType,
		unsigned long entityKey,
		unsigned long entityTypeKey,
		const TA_Base_Core::DescriptionParameters& description,
		const std::string& entityName, 
		unsigned long locationKey, 
		unsigned long subsystemKey,
		unsigned long alarmSeverityKey,
		timeb sourceTime,
		const std::string& assetName,
		bool isUnique = true,
		TA_Base_Core::MmsStateType mmsState = TA_Base_Core::MMS_NONE);
	/** 
	* getValueFromParameters
	*
	* This function will parse the parameters string to obtain the
	* alarm value
	*/
	std::string getValueFromParameters(const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& description);

private:
	HANDLE m_hThreads[256];
	CRITICAL_SECTION m_cs;
	TypeAlarmsQueue m_alarms;
	size_t m_iUncloseAlarmSize;
	std::vector<std::string> m_uuids;
	bool m_bRunning;
	bool m_bSendAlarmStore;
	std::string m_SessionId; 

	IUpdateObserver* m_UpdateObserver;
};


#endif // !defined(AFX_ALARMSCOLLECTION_H__718495AD_3AF6_4967_97BF_0F3918B6F010__INCLUDED_)
