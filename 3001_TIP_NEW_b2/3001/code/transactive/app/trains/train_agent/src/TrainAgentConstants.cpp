/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/TrainAgentConstants.cpp $
 * @author:  Khalil Dowlut
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 * 
 * Implementation of the class TrainAgentConstants.
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "app/trains/train_agent/src/TrainAgentConstants.h"


//const std::string TrainAgentConstants::PARAMETER_TRAIN_CALL_GROUP("TrainLiveAnnouncementCallGroup");
const std::string TrainAgentConstants::PARAMETER_DEPOT_CALL_GROUP("DepotLiveAnnouncementCallGroup");
const std::string TrainAgentConstants::PARAMETER_REDLINE_CALL_GROUP("RedlineLiveAnnouncementCallGroup");
const std::string TrainAgentConstants::PARAMETER_ORANGELINE_CALL_GROUP("OrangeLineLiveAnnouncementCallGroup");
const std::string TrainAgentConstants::PARAMETER_COMMAND_RETRIES("CommandRetries");;
const std::string TrainAgentConstants::PARAMETER_COMMAND_TIMEOUT("CommandTimeoutInSeconds");
const std::string TrainAgentConstants::PARAMETER_RADIO_CALL_INTERVAL("RadioCallIntervalInSeconds");
const std::string TrainAgentConstants::PARAMETER_ALL_TRAINS_PA_LIVE_DELAY("AllTrainsPaLiveDelayInSeconds");
const std::string TrainAgentConstants::PARAMETER_STATUS_REQUEST_TIMEOUT("StatusRequestTimeoutInSeconds");
const std::string TrainAgentConstants::PARAMETER_STATUS_REQUEST_POLL_TIME("StatusRequestPollTimeInSeconds");
const std::string TrainAgentConstants::PARAMETER_OCC_TELEGRAM_CARGROUP_LENGTH("TelegramOCC:CarGroupLength");
const std::string TrainAgentConstants::DATAPOINT_OCC_TELEGRAM_PREFIX("TelegramOCC:Train");
const std::string TrainAgentConstants::DATAPOINT_MIS_TELEGRAM_PREFIX("TelegramMIS:Train");
const std::string TrainAgentConstants::DATAPOINT_MIS_MILEAGE_NAME("OperatingMileage");
const char TrainAgentConstants::DATAPOINT_SEPARATOR(':');
const char TrainAgentConstants::DATAPOINT_BYTE_BIT_SEPARATOR('.');

// ss++ TD16428
const std::string TrainAgentConstants::DATAPOINT_OCC_RADIO_NUMBER("RadioNumber");
const std::string TrainAgentConstants::DATAPOINT_OCC_SERVICE_SERIAL_NUMBER("ServiceSerialNumber");
const std::string TrainAgentConstants::DATAPOINT_OCC_CAR_GROUP_NUMBER("CarGroupNumber");
const std::string TrainAgentConstants::DATAPOINT_OCC_CURRENT_LINE_NUMBER("CurrentLineNumber");
// ++ss

// ss++
// TD 14871
const std::string TrainAgentConstants::RADIO_MFT_MANAGER("RadioManager");
const std::string TrainAgentConstants::RADIO_MFT_AGENT_TYPE("RadioMFTAgent");
const std::string TrainAgentConstants::RADIO_SERVER_COMMAND_TYPE("RadioServerCommand");
// ++ss

const std::string TrainAgentConstants::SIGNALLING_AGENT_TYPE("SignallingAgent");

