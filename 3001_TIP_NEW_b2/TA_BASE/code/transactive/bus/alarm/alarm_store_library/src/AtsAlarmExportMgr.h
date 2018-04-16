
#ifndef __ATS_ALARM_EXPORT_MANAGER_H_INCLUDED__
#define __ATS_ALARM_EXPORT_MANAGER_H_INCLUDED__

#include "bus/alarm/ATS_alarm_bridge_library/src/DataStoreReceiverATSAlarmInfo.h"
#include "core/threads/src/QueueProcessor.h"

using namespace I_ATS_ISCS_ALARM;
namespace TA_Base_Bus
{
	class AtsAlarmXmlWriter;

	class AtsAlarmExportMgr
		: public TA_Base_Core::QueueProcessor<ATSAlarmQueueItem>
	{
	public:
		AtsAlarmExportMgr(const std::string& folderPath, unsigned long exportInterval);
		~AtsAlarmExportMgr();

		virtual void processEvent( boost::shared_ptr<ATSAlarmQueueItem> newEvent );

		void startProcessing();
		void stopProcessing();

	protected:

	private:
		AtsAlarmXmlWriter* m_pAtsXmlWriter;
		
	};
}

#endif //__ATS_ALARM_EXPORT_MANAGER_H_INCLUDED__
