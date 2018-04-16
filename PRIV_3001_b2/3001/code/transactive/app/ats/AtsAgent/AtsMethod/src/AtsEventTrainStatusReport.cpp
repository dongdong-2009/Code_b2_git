
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventTrainStatusReport.h"

using namespace TA_IRS_App;

AtsEventTrainStatusReport::AtsEventTrainStatusReport(boost::shared_ptr<AtsEvent> event)
: AtsEventReader(event, EVENT_ATS_TRAIN_STATUS_REPORT)
{
}

AtsEventTrainStatusReport::~AtsEventTrainStatusReport(void)
{
}

unsigned char AtsEventTrainStatusReport::getTrainId()
{
	return getParameterChar(EVENT_HEADER_TRAIN_ID_NUMBER);
}

unsigned char AtsEventTrainStatusReport::getCRC0()
{
	return getParameterChar(EVENT_HEADER_CRC0);
}

unsigned char AtsEventTrainStatusReport::getCRC1()
{
	return getParameterChar(EVENT_HEADER_CRC1);
}