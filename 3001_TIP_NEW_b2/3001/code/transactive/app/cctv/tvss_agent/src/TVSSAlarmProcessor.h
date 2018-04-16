#ifndef _TVSS_ALARM_PROCESSOR_H_FILE_
#define _TVSS_ALARM_PROCESSOR_H_FILE_
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/video_switch_agent/src/TVSSStationIdAccessFactory.h"
#include <string>
#include <map>

static const std::string m_gGWName = "diiGW-NormalAlarm";
static const std::string m_gAPName = "diiAP-NormalAlarm";
static const std::string m_gSBName = "diiSB-NormalAlarm";
static const std::string m_gLTMName = "diiLTM-NormalAlarm";
static const std::string m_gOccSvrName = "diiSVR-NormalAlarm";
namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
class TVSSServant;

class TVSSAlarmProcessor: public TA_Base_Core::Thread
{
public:
	TVSSAlarmProcessor(std::string loactionname,TVSSServant* servant,unsigned long checkAlaramInterval=-1);
	virtual ~TVSSAlarmProcessor();
	bool AddDataPoint(std::string strName,TA_Base_Bus::DataPoint* dataPoint);
	bool SetDataPointValue(int alarmIndex,bool alramValue);
	void setDataPointsAsBadValue();
	void SetControlMode();
	void SetMonitorMode();

    int GetDPCount();
private:
	void ClearDpList();
	void run();
	void terminate();
private:
    bool isDataPointNeedUpdate(TA_Base_Bus::DataPoint* dataPoint, const TA_Base_Bus::DataPointState& newState);
	std::string m_locationName;
	TVSSServant* m_servant;
	std::map<std::string,TA_Base_Bus::DataPoint*> m_dpList;
    std::string* m_dpNames;
	TA_Base_Core::ReEntrantThreadLockable m_lock;
	bool m_terminate;
	int m_dpCount;
	unsigned long m_equipmentAlarmCheckInterval;
	//Store db access result
	TA_IRS_Core::TVSSStationIdAccessFactory::StationIdToLocationKeyMap stationIdToLocationKeyMap;
};
}
#endif