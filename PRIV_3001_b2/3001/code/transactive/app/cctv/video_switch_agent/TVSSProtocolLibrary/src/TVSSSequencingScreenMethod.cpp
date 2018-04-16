#include "TVSSSequencingScreenMethod.h"
#include "TVSSProtocol.h"

#define SEQUENCING_SCREEN_METHOD_PARAMETER_LENGTH           2
#define SEQUENCING_SCREEN_METHOD_MONITORID_START_POSITION   5
#define SEQUENCING_SCREEN_ID_POSITION                       6

using namespace TA_IRS_App;

TVSSSequencingScreenMethod::TVSSSequencingScreenMethod(unsigned char monitorId, unsigned char sequenceId)
: TVSSMethod (SEQUENCING_SCREEN_METHOD_PARAMETER_LENGTH ,METHOD_SEQUENCE_SCREEN )
{
   setByte( SEQUENCING_SCREEN_METHOD_MONITORID_START_POSITION , monitorId );
   setByte( SEQUENCING_SCREEN_ID_POSITION , sequenceId );
}

TVSSSequencingScreenMethod::~TVSSSequencingScreenMethod(void)
{
}


