
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Length of All data
#define SYSTEM_STATUS_LENGTH                                              7

//Character Start Position
#define METHOD_HEADER_SYSTEM_STATUS_MESSAGE_ID_START_POSITION             3
#define METHOD_HEADER_SYSTEM_STATUS_ETX_START_POSITION                    6
//#define SYSTEM_STATUS_MESSAGE_ID                                         "M37"

#define OCC_SYSTEM_STATUS_MESSAGE_ID                                     "M37"
#define STATION_SYSTEM_STATUS_MESSAGE_ID                                 "M38"

using namespace TA_IRS_App;

RadioMethodSystemStatus::RadioMethodSystemStatus(bool occ)
: RadioMethod(SYSTEM_STATUS_LENGTH,STX_VALUE), isOcc(occ)
{
	FUNCTION_ENTRY("RadioMethodSystemStatus");

	// Set Message ID
	if (isOcc) {
		setParameterString(METHOD_HEADER_SYSTEM_STATUS_MESSAGE_ID_START_POSITION, OCC_SYSTEM_STATUS_MESSAGE_ID);
	}
	else {
		setParameterString(METHOD_HEADER_SYSTEM_STATUS_MESSAGE_ID_START_POSITION, STATION_SYSTEM_STATUS_MESSAGE_ID);
	}

	// Set ETX
	setByte(METHOD_HEADER_SYSTEM_STATUS_ETX_START_POSITION, ETX_VALUE);
	FUNCTION_EXIT;
}

RadioMethodSystemStatus::~RadioMethodSystemStatus()
{
	FUNCTION_ENTRY("~RadioMethodSystemStatus");

	FUNCTION_EXIT;
}