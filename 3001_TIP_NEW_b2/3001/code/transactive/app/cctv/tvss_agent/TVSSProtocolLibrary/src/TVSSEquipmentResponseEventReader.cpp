#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#define EQUIPMENT_ALARM_RESPONSE_LENGTH                      11
#define OCC_EQUIPMENT_ALARM_RESPONSE_LENGTH                 248
#define EQUIPMENT_ALARM_RESPONSE_PARAMETER_LENGTH             5
#define OCC_EQUIPMENT_ALARM_RESPONSE_PARAMETER_LENGTH       242
#define EQUIPMENT_ALARM_RESPONSE_TMF_TVSS_START_POSITION      6
#define ALARM_RESPONSE_DATA_START_POSITION                    7

using namespace TA_IRS_App;

TVSSEquipmentResponseEventReader::TVSSEquipmentResponseEventReader(boost::shared_ptr<TVSSEvent> event)
  : TVSSEventReader(event,EVENT_EQUIPMENT_ALARM_RESPONSE,EQUIPMENT_ALARM_RESPONSE_PARAMETER_LENGTH,OCC_EQUIPMENT_ALARM_RESPONSE_PARAMETER_LENGTH)
{
	//initializeAlarmResponseMap();
}

TVSSEquipmentResponseEventReader::~TVSSEquipmentResponseEventReader(void)
{
}

unsigned char TVSSEquipmentResponseEventReader::getAlarmResponseData(int dataIndex)
{
	return getParameterChar ( ALARM_RESPONSE_DATA_START_POSITION+dataIndex );
}

void TVSSEquipmentResponseEventReader::initializeAlarmResponseMap(void)
{
	int dataStart = ALARM_RESPONSE_DATA_START_POSITION;
	int length = 0;
	unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
	TA_Base_Core::ILocation* location =
		TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
	if (location->getLocationType() == TA_Base_Core::ILocation::OCC)
	{
		length = OCC_EQUIPMENT_ALARM_RESPONSE_LENGTH - dataStart;
	} 
	else
	{
		length = EQUIPMENT_ALARM_RESPONSE_LENGTH - dataStart;
	}
	for(int x = 0; x < length; x++)
		AlarmResponseData.push_back(dataStart + x);
}

unsigned char TVSSEquipmentResponseEventReader::getAlarmResponseOrigin(void)
{
	return getParameterChar ( EQUIPMENT_ALARM_RESPONSE_TMF_TVSS_START_POSITION );
}

int TVSSEquipmentResponseEventReader::getLengthAlarmResponseData()
{
	return AlarmResponseData.size();
}