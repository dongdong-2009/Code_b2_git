#include "TVSSEquipmentQueryMethod.h"
#include "TVSSProtocol.h"

#define EQUIPMENT_ALARM_QUERY_METHOD_PARAMETER_LENGTH       1
#define EQUIPMENT_ALARM_QUERY_METHOD_SERVER_START_POSITION  5


using namespace TA_IRS_App;

TVSSEquipmentQueryMethod::TVSSEquipmentQueryMethod(unsigned char tmf_server)
: TVSSMethod (EQUIPMENT_ALARM_QUERY_METHOD_PARAMETER_LENGTH ,METHOD_EQUIPMENT_ALARM_QUERY )
{
   setByte( EQUIPMENT_ALARM_QUERY_METHOD_SERVER_START_POSITION , tmf_server );
}

TVSSEquipmentQueryMethod::~TVSSEquipmentQueryMethod(void)
{
}



