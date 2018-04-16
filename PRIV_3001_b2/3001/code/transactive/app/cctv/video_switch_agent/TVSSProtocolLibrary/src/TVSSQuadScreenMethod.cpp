#include "TVSSQuadScreenMethod.h"
#include "TVSSProtocol.h"

#define QUAD_SCREEN_METHOD_PARAMETER_LENGTH             5
#define QUAD_SCREEN_METHOD_MONITORID_POSITION           5
#define QUAD_SCREEN_CAMERA_START_POSITION               6 
#define NUMBER_OF_QUAD_SCREEN_CAMERA                    4               

using namespace TA_IRS_App;

TVSSQuadScreenMethod::TVSSQuadScreenMethod(unsigned char monitorId, Camera data)
: TVSSMethod (QUAD_SCREEN_METHOD_PARAMETER_LENGTH ,METHOD_QUAD_SCREEN )
{
	setByte( QUAD_SCREEN_METHOD_MONITORID_POSITION , monitorId );
    setCameraData(data);
}

TVSSQuadScreenMethod::~TVSSQuadScreenMethod(void)
{
}

void TVSSQuadScreenMethod::setCameraData(Camera setData)
{
	m_cameraData = setData;
	int cameraId = QUAD_SCREEN_CAMERA_START_POSITION;
	
	for( int x = 0; x < NUMBER_OF_QUAD_SCREEN_CAMERA ; x++)
	{
		setByte( cameraId ++, m_cameraData[x]);
	}
}



