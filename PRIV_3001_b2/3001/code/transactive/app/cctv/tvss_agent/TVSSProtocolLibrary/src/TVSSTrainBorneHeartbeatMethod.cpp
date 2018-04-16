#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainBorneHeartbeatMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define TRAINBORNE_HEARTBEAT_PARAMETER_LENGTH           2
#define TRAINBORNE_HEARTBEAT_MONITORID_START_POSITION   6

namespace TA_IRS_App
{


TVSSTrainBorneHeartbeatMethod::TVSSTrainBorneHeartbeatMethod()
: TVSSMethod (TRAINBORNE_HEARTBEAT_PARAMETER_LENGTH, METHOD_TRAINBORNE_HEARTBEAT )
{
	setByte(TRAINID_START_POSITION,0);
	setByte( TRAINBORNE_HEARTBEAT_MONITORID_START_POSITION , 0 );
   	setByte( TRAINBORNE_HEARTBEAT_MONITORID_START_POSITION + 1 , 0xff );
}

TVSSTrainBorneHeartbeatMethod::~TVSSTrainBorneHeartbeatMethod()
{
}

}