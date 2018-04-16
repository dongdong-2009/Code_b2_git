// TVSSServerLinkStatusUpdateThread.h: interface for the TVSSServerLinkStatusUpdateThread class.
//
//////////////////////////////////////////////////////////////////////

#ifndef TVSSServerLinkStatusUpdateThread__INCLUDED_
#define TVSSServerLinkStatusUpdateThread__INCLUDED_


#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/common_library/src/CommonDefs.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/Thread.h"
#include "core/timers/src/TimedWaitSemaphore.h"

#include <bitset>

namespace TA_Base_Bus
{
    class DataPoint;
}

class TVSSServerLinkStatusUpdateThread  : public TA_Base_Core::Thread,
										 public TA_Base_Bus::IEntityUpdateEventProcessor
{
public:

	enum DpServerStatus
	{
		ServerFallback = 0,
		Server0Down,
		Server1Down
	};

public:
	TVSSServerLinkStatusUpdateThread();
	virtual ~TVSSServerLinkStatusUpdateThread();
	bool setDataPoint( TA_Base_Bus::DataPoint* dataPoint );
	void setLinkStatus(DpServerStatus statusCode, bool value);

	void run();
	void terminate();

	void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

private:
	
	TA_Base_Bus::DataPoint* 				m_dataPoint;
	bool									m_terminate;

	TA_Base_Core::TimedWaitSemaphore		m_semaphore;
	std::bitset<8>							m_DpServerStatus;
	TA_Base_Core::ReEntrantThreadLockable	m_setValueLock;
};

#endif // TVSSServerLinkStatusUpdateThread__INCLUDED_
