#ifndef RADIO_AGENT_CONSTANTS_H
#define RADIO_AGENT_CONSTANTS_H
/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $Source: /cvs/1TS-9999-T/code/transactive/app/radio/RadioTetraAgent/src/Attic/RadioAgentConstants.h,v $
  * @author  Adam Radics
  * @version $Revision: 1.1.2.1 $
  * Last modification : $Date: 2006/12/15 03:08:56 $
  * Last modified by : $Author: adamr $
  *
  * Holds some constants
  */

#include <string>
#include "ace/Time_Value.h"

namespace TA_IRS_App
{
    class RadioAgentConstants
    {
    public:

        ~RadioAgentConstants();

        /**
         * The system error threshold the radio system defaults to
         */
        static const short SYSTEM_ERROR_THRESHOLD_OTE_DEFAULT;

        /**
         * The timer used to trigger base station fallback
         */
        static const unsigned long FALLBACK_TIMEOUT_OTE_DEFAULT;

        /**
         * All Fallback datapoints start with this
         * BaseStationFallback:<scnId>:<scId>
         */
        static const std::string FALLBACK_DATAPOINT_PREFIX;

        /**
         * All Fallback datapoints are separated with this ':'
         */
        static const char FALLBACK_DATAPOINT_SEPARATOR;

        /**
         * The time to wait in seconds, before calling GetNe after a login
         */
        static const unsigned long GET_NE_WAIT_TIME;

        /**
         * All tcp server datapoints start with this
         * TcpServer:<server number>
         */
        static const std::string TCP_SERVER_DATAPOINT_PREFIX;

        /**
         * The default login name for the OCC radio session (if none configured) 'OCCA'
         */
        static const std::string DEFAULT_OCC_LOGIN_NAME;

        /**
         * The default login suffix for station radio sessions (if none configured) 'SMS'
         */
        static const std::string DEFAULT_STATION_LOGIN_SUFFIX;

        /**
         * The default login name for the SDS Query radio session (if none configured) 'DBQ'
         */
        static const std::string DEFAULT_SDS_QUERY_LOGIN_NAME;

        /**
         * The default volume for a call
         */
        static const unsigned long DEFAULT_AUDIO_VOLUME;

		/**
		  * The expire time of Event in RadioAnget's Event buffer of Monitor mode.
          *
		  **/
		static const ACE_Time_Value RADIO_EVENT_ALIVE_TIME;

		static const std::string RADIO_LINK_DP_SUFFIX;

		static const unsigned long KEEP_ALIVE_DEFAULT_INTERVAL;

		static const std::string RADIOSESSION_ISCS_DEFAULT_NAME;

    private:
        RadioAgentConstants();
        RadioAgentConstants(const RadioAgentConstants& rhs);
        RadioAgentConstants& operator = (const RadioAgentConstants &);

    };
} // namespace TA_IRS_App

#endif // #ifndef RADIO_AGENT_CONSTANTS_H
