
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventOa1Table.h"

using namespace TA_IRS_App;

AtsEventOa1Table::AtsEventOa1Table(boost::shared_ptr<AtsEvent> event)
: AtsEventReader(event, EVENT_ATS_OA1TABLE)
{
}

AtsEventOa1Table::~AtsEventOa1Table(void)
{
}

unsigned char AtsEventOa1Table::getTrainId()
{
	return getParameterChar(EVENT_HEADER_TRAIN_ID_NUMBER);
}

unsigned char AtsEventOa1Table::getCRC0()
{
	return getParameterChar(EVENT_HEADER_CRC0);
}

unsigned char AtsEventOa1Table::getCRC1()
{
	return getParameterChar(EVENT_HEADER_CRC1);
}