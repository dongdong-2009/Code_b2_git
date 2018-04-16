
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventBegin.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_IRS_App
{

	const unsigned long RadioPDSEventBegin::EVENT_MESSAGE_TYPE = 40;

	RadioPDSEventBegin::RadioPDSEventBegin(boost::shared_ptr<RadioPDSEvent> event)
		: RadioPDSEventReader(event, EVENT_MESSAGE_TYPE )
	{
		FUNCTION_ENTRY( "RadioPDSEventBegin" );
		FUNCTION_EXIT;
	}

	RadioPDSEventBegin::~RadioPDSEventBegin()
	{
		FUNCTION_ENTRY( "RadioPDSEventBegin" );
		FUNCTION_EXIT;
	}

}