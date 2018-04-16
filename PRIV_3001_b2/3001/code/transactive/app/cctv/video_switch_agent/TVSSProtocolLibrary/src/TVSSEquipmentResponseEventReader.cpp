#include "TVSSEquipmentResponseEventReader.h"
#include "TVSSProtocol.h"

#define EQUIPMENT_ALARM_RESPONSE_METHOD_PARAMETER_LENGTH        10
#define EQUIPMENT_ALARM_RESPONSE_TMF_TVSS_START_POSITION         5
#define ALARM_RESPONSE_DATA_START                                6
#define ALARM_RESPONSE_DATA_LENTH                                4

using namespace TA_IRS_App;

TVSSEquipmentResponseEventReader::TVSSEquipmentResponseEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_EQUIPMENT_ALARM_RESPONSE,EQUIPMENT_ALARM_RESPONSE_METHOD_PARAMETER_LENGTH)
{
	initializeAlarmResponseMap();
}

TVSSEquipmentResponseEventReader::~TVSSEquipmentResponseEventReader(void)
{
}

unsigned char TVSSEquipmentResponseEventReader::getAlarmResponseData(int dataIndex)
{
	return getParameterChar ( AlarmResponseData[dataIndex] );
}

void TVSSEquipmentResponseEventReader::initializeAlarmResponseMap(void)
{
	int dataStart = ALARM_RESPONSE_DATA_START;
	for(int x = 1; x <=  ALARM_RESPONSE_DATA_LENTH; x++)
		AlarmResponseData[x] = dataStart++;
}

unsigned char TVSSEquipmentResponseEventReader::getAlarmResponseOrigin(void)
{
	return getParameterChar ( EQUIPMENT_ALARM_RESPONSE_TMF_TVSS_START_POSITION );
}