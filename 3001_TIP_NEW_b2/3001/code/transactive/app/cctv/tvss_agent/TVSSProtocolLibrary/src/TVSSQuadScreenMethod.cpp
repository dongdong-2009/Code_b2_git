#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSQuadScreenMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define QUAD_SCREEN_METHOD_PARAMETER_LENGTH             2
#define QUAD_SCREEN_METHOD_MONITORID_POSITION           6
#define QUAD_SCREEN_QUAD_START_POSITION					7 
#define NUMBER_OF_QUAD_SCREEN_CAMERA                    4               

using namespace TA_IRS_App;

TVSSQuadScreenMethod::TVSSQuadScreenMethod(unsigned char trainID,unsigned char monitorId, unsigned char quadId)
: TVSSMethod (QUAD_SCREEN_METHOD_PARAMETER_LENGTH ,METHOD_QUAD_SCREEN )
{
	setByte( TRAINID_START_POSITION,trainID);
	setByte( QUAD_SCREEN_METHOD_MONITORID_POSITION , monitorId );
   // setCameraData(data);
	setByte( QUAD_SCREEN_QUAD_START_POSITION , quadId );
}

TVSSQuadScreenMethod::~TVSSQuadScreenMethod(void)
{
}

void TVSSQuadScreenMethod::setCameraData(Camera setData)
{
//	m_cameraData = setData;
//	int cameraId = QUAD_SCREEN_CAMERA_START_POSITION;
//	
//	for( int x = 0; x < NUMBER_OF_QUAD_SCREEN_CAMERA ; x++)
//	{
//		setByte( cameraId ++, m_cameraData[x]);
//	}
}


