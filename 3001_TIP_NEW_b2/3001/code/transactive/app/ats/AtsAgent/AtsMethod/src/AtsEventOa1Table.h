
#ifndef ATS_EVENT_OA1_TABLE_H	
#define ATS_EVENT_OA1_TABLE_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsEventReader.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodProtocol.h"

namespace TA_IRS_App
{
	class AtsEventOa1Table : public AtsEventReader
	{
	public:
		AtsEventOa1Table(boost::shared_ptr<AtsEvent> event);
		virtual ~AtsEventOa1Table(void);
	public:
		unsigned char getTrainId();
		unsigned char getCRC0();
		unsigned char getCRC1();
	};
};
#endif //end of ATS_EVENT_OA1_TABLE_H