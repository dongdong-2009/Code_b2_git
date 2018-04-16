#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSClearImageMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define CLEAR_IMAGE_METHOD_PARAMETER_LENGTH            1
#define CLEAR_IMAGE_METHOD_MONITORID_START_POSITION    6


using namespace TA_IRS_App ;

TVSSClearImageMethod::TVSSClearImageMethod(unsigned char trainID,unsigned char monitorId )
 : TVSSMethod (CLEAR_IMAGE_METHOD_PARAMETER_LENGTH ,METHOD_CLEAR_IMAGE ) 
{
	setByte ( TRAINID_START_POSITION,trainID);
	setByte ( CLEAR_IMAGE_METHOD_MONITORID_START_POSITION , monitorId ) ;
}

TVSSClearImageMethod::~TVSSClearImageMethod(void)
{
}
