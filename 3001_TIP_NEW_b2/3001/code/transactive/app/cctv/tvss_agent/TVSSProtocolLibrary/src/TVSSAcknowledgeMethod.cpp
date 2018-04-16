#include "TVSSAcknowledgeMethod.h"
#include "TVSSProtocol.h"

#define ACKNOWLEDGE_METHOD_PARAMETER_LENGTH           3
#define ACKNOWLEDGE_METHOD_MONITORID_START_POSITION   6
#define ACKNOWLEDGE_CAMERA_X_POSITION                 7

using namespace TA_IRS_App;

TVSSAcknowledgeMethod::TVSSAcknowledgeMethod(unsigned char trainID,unsigned char monitorId, unsigned char cameraX,unsigned char cameraY)
: TVSSMethod (ACKNOWLEDGE_METHOD_PARAMETER_LENGTH ,METHOD_ACKNOWLEDGE_EVENT )
{
	setByte( TRAINID_START_POSITION,trainID);
	setByte( ACKNOWLEDGE_METHOD_MONITORID_START_POSITION , monitorId );
	setByte( ACKNOWLEDGE_CAMERA_X_POSITION , cameraX );
	setByte( ACKNOWLEDGE_CAMERA_X_POSITION+1 , cameraY );
}

TVSSAcknowledgeMethod::~TVSSAcknowledgeMethod(void)
{
}



