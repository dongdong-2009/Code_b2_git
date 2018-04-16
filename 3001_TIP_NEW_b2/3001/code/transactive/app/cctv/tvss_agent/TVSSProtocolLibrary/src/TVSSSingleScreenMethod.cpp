#include "TVSSSingleScreenMethod.h"
#include "TVSSProtocol.h"

#define SINGLE_SCREEN_METHOD_PARAMETER_LENGTH           2
#define SINGLE_SCREEN_METHOD_MONITORID_START_POSITION   6
#define SINGLE_SINGLE_CAMERA_X_POSITION                 7

using namespace TA_IRS_App;

TVSSSingleScreenMethod::TVSSSingleScreenMethod(unsigned char trainID,unsigned char monitorId, unsigned char cameraX)
: TVSSMethod (SINGLE_SCREEN_METHOD_PARAMETER_LENGTH ,METHOD_SINGLE_SCREEN )
{
	setByte(TRAINID_START_POSITION,trainID);
	setByte( SINGLE_SCREEN_METHOD_MONITORID_START_POSITION , monitorId );
	setByte( SINGLE_SINGLE_CAMERA_X_POSITION , cameraX );
}

TVSSSingleScreenMethod::~TVSSSingleScreenMethod(void)
{
}



