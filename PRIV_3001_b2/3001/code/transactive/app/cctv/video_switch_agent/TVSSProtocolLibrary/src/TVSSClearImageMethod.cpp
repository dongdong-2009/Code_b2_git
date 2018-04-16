#include "TVSSClearImageMethod.h"
#include "TVSSProtocol.h"

#define CLEAR_IMAGE_METHOD_PARAMETER_LENGTH            1
#define CLEAR_IMAGE_METHOD_MONITORID_START_POSITION    5


using namespace TA_IRS_App ;

TVSSClearImageMethod::TVSSClearImageMethod(unsigned char monitorId )
: TVSSMethod (CLEAR_IMAGE_METHOD_PARAMETER_LENGTH ,METHOD_CLEAR_IMAGE ) 
{
   setByte ( CLEAR_IMAGE_METHOD_MONITORID_START_POSITION , monitorId ) ;
}

TVSSClearImageMethod::~TVSSClearImageMethod(void)
{
}
