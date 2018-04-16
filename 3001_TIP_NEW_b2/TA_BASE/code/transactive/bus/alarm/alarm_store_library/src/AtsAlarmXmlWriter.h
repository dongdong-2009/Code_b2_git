

#ifndef __ATS_ALARM_XML_WRITER_H_INCLUDED__

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Condition.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "AtsAlarmXMLDef.h"

#include <string>
#include <vector>

namespace TA_Base_Bus
{
	class AtsAlarmExportMgr;

	class AtsAlarmXmlWriter
		: public TA_Base_Core::Thread
	{
	public:
		AtsAlarmXmlWriter(const std::string& filePath, unsigned long writeInterval);
		~AtsAlarmXmlWriter();

		virtual void run();
		virtual void terminate();

		void insert(AtsXmlDataPtr atsData);
	protected:
		void writeToFile();
		int getCacheSize();
	private:
		std::string getNextFileName();
		
		TA_Base_Core::Condition m_condition;
		volatile bool m_terminate;

		unsigned long m_fileCount;
		unsigned short m_lastDayLog;
		std::string m_filePath;
		std::string m_defaultFileName;
		unsigned long m_writeInterval;

		TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		std::vector<AtsXmlDataPtr> m_vecCache;
	};
}

#endif //__ATS_ALARM_XML_WRITER_H_INCLUDED__