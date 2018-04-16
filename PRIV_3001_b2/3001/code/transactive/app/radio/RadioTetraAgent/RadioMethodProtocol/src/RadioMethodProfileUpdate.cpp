
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodProfileUpdate.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Length of All data
#define PROFILE_UPDATE_LENGTH                                              21

//Character Start Position
#define METHOD_HEADER_PROFILE_UPDATE_TIMESTAMP_START_POSITION              3
#define METHOD_HEADER_PROFILE_UPDATE_MESSAGE_ID_START_POSITION             17
#define METHOD_HEADER_PROFILE_UPDATE_ETX_START_POSITION                           20
#define PROFILE_UPDATE_MESSAGE_ID                                         "M42"

using namespace TA_IRS_App;

RadioMethodProfileUpdate::RadioMethodProfileUpdate()
: RadioMethod(PROFILE_UPDATE_LENGTH,STX_VALUE)
{
	FUNCTION_ENTRY("RadioMethodCallForwardRequest");

	//Set the timestamp
	setParameterString(METHOD_HEADER_PROFILE_UPDATE_TIMESTAMP_START_POSITION,getTimestamp());

	//Set the Message ID
	setParameterString(METHOD_HEADER_PROFILE_UPDATE_MESSAGE_ID_START_POSITION,PROFILE_UPDATE_MESSAGE_ID);

	//Set the ETX
	setByte(METHOD_HEADER_PROFILE_UPDATE_ETX_START_POSITION,ETX_VALUE);
	FUNCTION_EXIT;
}

RadioMethodProfileUpdate::~RadioMethodProfileUpdate()
{
	FUNCTION_ENTRY("~RadioMethodProfileUpdate");

	FUNCTION_EXIT;
}