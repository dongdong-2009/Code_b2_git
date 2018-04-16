/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/trains/train_agent/src/TrainAgentConstants.h $
 * @author:  Khalil Dowlut
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2010/08/12 15:10:25 $
 * Last modified by:  $Author: builder $
 * 
 * This holds all constants like entity parameter names, and datapoint address
 * patterns
 */

#if !defined(TRAINAGENTCONSTANTS_H__E90F5934_962E_4d3e_87EE_B57F89B516A5__INCLUDED_)
#define TRAINAGENTCONSTANTS_H__E90F5934_962E_4d3e_87EE_B57F89B516A5__INCLUDED_

#include <string>


class TrainAgentConstants
{
private:

	/**
	  * Default constructor (disabled)
	  */
     TrainAgentConstants();

	/**
	  * Copy constructor (disabled)
	  * 
	  * @return 
	  * @param : const TrainAgentConstants&
	  */
    TrainAgentConstants(const TrainAgentConstants&);

	/**
	  * Assignment operator (disabled)
	  * 
	  * @return TrainAgentConstants& 
	  * @param : const TrainAgentConstants&
	  */
    TrainAgentConstants& operator=(const TrainAgentConstants&);

public:
    
	//static const std::string PARAMETER_TRAIN_CALL_GROUP;
	static const std::string PARAMETER_DEPOT_CALL_GROUP;

	static const std::string PARAMETER_REDLINE_CALL_GROUP;

	static const std::string PARAMETER_OCC_TELEGRAM_CARGROUP_LENGTH;

	static const std::string PARAMETER_ORANGELINE_CALL_GROUP;

	static const std::string PARAMETER_COMMAND_RETRIES;

	static const std::string PARAMETER_COMMAND_TIMEOUT;

	static const std::string PARAMETER_RADIO_CALL_INTERVAL;

    static const std::string PARAMETER_ALL_TRAINS_PA_LIVE_DELAY;
    
    static const std::string PARAMETER_STATUS_REQUEST_TIMEOUT;

    static const std::string PARAMETER_STATUS_REQUEST_POLL_TIME;

    /**
	 * Used to identify OCC Telegram datapoints by address, they will have the 
	 * format "TelegramOCC:TrainXX:<byte>.<bit>"
	 */
	static const std::string DATAPOINT_OCC_TELEGRAM_PREFIX;

	/**
	 * Used to identify MIS Telegram datapoints by address, they will have the 
	 * format "TelegramMIS:TrainXX:<byte>.<bit>"
	 */
	static const std::string DATAPOINT_MIS_TELEGRAM_PREFIX;

	/**
	 * Used to identify MIS Telegram datapoints by address, 
     * "TelegramMIS:TrainXX:OperatingMileage"
	 */
	static const std::string DATAPOINT_MIS_MILEAGE_NAME;

    // ss++ TD16428

    /**
	 * Used to identify datapoints by address, 
     * "TelegramOCC:TrainXX:RadioNumber"
	 */
	static const std::string DATAPOINT_OCC_RADIO_NUMBER;

    /**
	 * Used to identify OCC Telegram datapoints by address, 
     * "TelegramOCC:TrainXX:ServiceSerial"
	 */
	static const std::string DATAPOINT_OCC_SERVICE_SERIAL_NUMBER;

    /**
	 * Used to identify OCC Telegram datapoints by address, 
     * "TelegramOCC:TrainXX:CarGroupNumber"
	 */
	static const std::string DATAPOINT_OCC_CAR_GROUP_NUMBER;

    /**
	 * Used to identify OCC Telegram datapoints by address, 
     * "TelegramOCC:TrainXX:CurrentLineNumber"
	 */
	static const std::string DATAPOINT_OCC_CURRENT_LINE_NUMBER;
    
    // ++ss
    

	/**
	 * Separates the the datapoint prefix from the byte/bit address
	 */
	static const char DATAPOINT_SEPARATOR;

	static const char DATAPOINT_BYTE_BIT_SEPARATOR;
    

    const static std::string RADIO_MFT_AGENT_TYPE;

    // ss++
    // TD 14871
    const static std::string RADIO_MFT_MANAGER;

    const static std::string RADIO_SERVER_COMMAND_TYPE;
    // ++ss

    const static std::string SIGNALLING_AGENT_TYPE;

};
#endif // !defined(TRAINAGENTCONSTANTS_H__E90F5934_962E_4d3e_87EE_B57F89B516A5__INCLUDED_)
