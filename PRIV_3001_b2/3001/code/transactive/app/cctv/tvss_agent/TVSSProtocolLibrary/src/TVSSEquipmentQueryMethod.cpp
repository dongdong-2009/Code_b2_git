#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentQueryMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define EQUIPMENT_ALARM_QUERY_METHOD_PARAMETER_LENGTH       1
#define EQUIPMENT_ALARM_QUERY_METHOD_SERVER_START_POSITION  6


using namespace TA_IRS_App;

TVSSEquipmentQueryMethod::TVSSEquipmentQueryMethod()
: TVSSMethod (EQUIPMENT_ALARM_QUERY_METHOD_PARAMETER_LENGTH ,METHOD_EQUIPMENT_ALARM_QUERY )
{
	setByte( TRAINID_START_POSITION,0);
	setByte( EQUIPMENT_ALARM_QUERY_METHOD_SERVER_START_POSITION , 0 );
}

TVSSEquipmentQueryMethod::~TVSSEquipmentQueryMethod(void)
{
}



