#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSSequencingScreenMethod.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#define SEQUENCING_SCREEN_METHOD_PARAMETER_LENGTH           2
#define SEQUENCING_SCREEN_METHOD_MONITORID_START_POSITION   6
#define SEQUENCING_SCREEN_ID_POSITION                       7

using namespace TA_IRS_App;

TVSSSequencingScreenMethod::TVSSSequencingScreenMethod(unsigned char trainID,unsigned char monitorId, unsigned char sequenceId)
: TVSSMethod (SEQUENCING_SCREEN_METHOD_PARAMETER_LENGTH ,METHOD_SEQUENCE_SCREEN )
{
	setByte(TRAINID_START_POSITION,trainID);
	setByte( SEQUENCING_SCREEN_METHOD_MONITORID_START_POSITION , monitorId );
	setByte( SEQUENCING_SCREEN_ID_POSITION , sequenceId );
}

TVSSSequencingScreenMethod::~TVSSSequencingScreenMethod(void)
{
}


