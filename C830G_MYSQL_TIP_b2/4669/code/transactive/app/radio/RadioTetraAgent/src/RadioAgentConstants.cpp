/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioAgentConstants.cpp,v $
  * @author  Adam Radics
  * @version $Revision: 1.1.2.1 $
  * Last modification : $Date: 2006/12/15 03:08:56 $
  * Last modified by : $Author: adamr $
  *
  */

#include "app/radio/RadioTetraAgent/src/RadioAgentConstants.h"

namespace TA_IRS_App
{

    const short RadioAgentConstants::SYSTEM_ERROR_THRESHOLD_OTE_DEFAULT = 4;
    
    const unsigned long RadioAgentConstants::FALLBACK_TIMEOUT_OTE_DEFAULT = 30;

    const std::string RadioAgentConstants::FALLBACK_DATAPOINT_PREFIX = "BaseStationFallback";

    const char RadioAgentConstants::FALLBACK_DATAPOINT_SEPARATOR = ':';

    const unsigned long RadioAgentConstants::GET_NE_WAIT_TIME = 5;

    const std::string RadioAgentConstants::TCP_SERVER_DATAPOINT_PREFIX = "TcpServer:";

    const std::string RadioAgentConstants::DEFAULT_OCC_LOGIN_NAME = "OCCA";
    
    const std::string RadioAgentConstants::DEFAULT_STATION_LOGIN_SUFFIX = "SMS";
    
    const std::string RadioAgentConstants::DEFAULT_SDS_QUERY_LOGIN_NAME = "DBQ";

	const unsigned long RadioAgentConstants::DEFAULT_AUDIO_VOLUME = 8;

	const ACE_Time_Value RadioAgentConstants::RADIO_EVENT_ALIVE_TIME (200);

	const std::string RadioAgentConstants::RADIO_LINK_DP_SUFFIX = "aiiRadio-NormalFallBack";

	const unsigned long RadioAgentConstants::KEEP_ALIVE_DEFAULT_INTERVAL = 3;

	const std::string RadioAgentConstants::RADIOSESSION_ISCS_DEFAULT_NAME = "ISCSSessionName";

} // namespace TA_IRS_App

