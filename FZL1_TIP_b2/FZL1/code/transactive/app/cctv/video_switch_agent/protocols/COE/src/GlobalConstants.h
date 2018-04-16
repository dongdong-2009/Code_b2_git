#ifndef GLOBALCONSTANTS_H
#define GLOBALCONSTANTS_H

#include "bus/cctv/video_switch_agent/IDL/src/ExceptionsCorbaDef.h"

// Sequence
const unsigned short PREDEFINE_DWELL_TIME = 6;
const int MAX_SOURCES_PER_SEQUENCE = 64;
// Error defination
const std::string ERROR_CONNECTION_WITH_VSS = "Unable to establish Connection with the VSS System";
const std::string ERROR_ENCOUNTER_WHEN_PERFORMING_REQUEST = "Error encounter when performing the requested operation";
const TA_Base_Bus::VideoSwitchAgentException EXCEPTION_CONNECTION_WITH_VSS(ERROR_CONNECTION_WITH_VSS.c_str());
const TA_Base_Bus::VideoSwitchAgentException EXCEPTION_PERFORMING_REQUEST(ERROR_ENCOUNTER_WHEN_PERFORMING_REQUEST.c_str());

#endif