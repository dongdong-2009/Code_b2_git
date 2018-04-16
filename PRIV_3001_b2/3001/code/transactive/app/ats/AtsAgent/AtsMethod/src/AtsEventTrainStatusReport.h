
#ifndef ATS_EVENT_TRAIN_STATUS_REPORT_H
#define ATS_EVENT_TRAIN_STATUS_REPORT_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsEventReader.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodProtocol.h"

namespace TA_IRS_App
{
	class AtsEventTrainStatusReport : public AtsEventReader
	{
	public:
		AtsEventTrainStatusReport(boost::shared_ptr<AtsEvent> event);
		virtual ~AtsEventTrainStatusReport(void);
	public:
		unsigned char getTrainId();
		unsigned char getCRC0();
		unsigned char getCRC1();
	};
};
#endif // end of ATS_EVENT_TRAIN_STATUS_REPORT_H