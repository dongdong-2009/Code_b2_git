
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventFrameRepeat.h"


//#define  EVENT_MESSAGE_TYPE 41
namespace TA_IRS_App
{	
	const unsigned long RadioPDSEventFrameRepeat::EVENT_MESSAGE_TYPE = 41;

	RadioPDSEventFrameRepeat::RadioPDSEventFrameRepeat(boost::shared_ptr<RadioPDSEvent> event)
		: RadioPDSEventReader(event,EVENT_MESSAGE_TYPE)
	{
		FUNCTION_ENTRY( "RadioPDSEventFrameRepeat" );
		FUNCTION_EXIT;
	}

	RadioPDSEventFrameRepeat::~RadioPDSEventFrameRepeat()
	{
		FUNCTION_ENTRY( "~RadioPDSEventFrameRepeat" );
		FUNCTION_EXIT;
	}
}