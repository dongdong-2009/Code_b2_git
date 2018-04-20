/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2016/11/30 11:21:38 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * AbstractPAAgentEntityData implements the interface IAbstractPAAgentEntityData to provide read-only access
  * to AbstractPAAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"


static std::string toUpper(const std::string& in)
{
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(), toupper);
    return out;
}




namespace TA_Base_Core
{

    static const unsigned long DEFAULT_FAST_POLL_RATE                       
        = 500;
    static const unsigned long DEFAULT_WORKER_THREADS_SLEEP_TIME            
        = 100;
    static const unsigned long DEFAULT_SOCKET_TIMEOUT
        = 3;
    static const unsigned long DEFAULT_TIS_DISPLAY_DURATION                 
        = 30;
    static const unsigned long DEFAULT_KEEP_TERMINATED_BROADCASTS   
        = 60*60*24;  // 1 day
    static const unsigned long DEFAULT_RETRIES                              
        = 3;
    static const std::string   DEFAULT_PRIMARY_FEPC_ADDRESS             
        = "";
    static const std::string    DEFAULT_SECONDARY_FEPC_ADDRESS           
        = "";
    static const unsigned long  DEFAULT_PRIMARY_FEPC_PORT                
        = 27000;
    static const unsigned long  DEFAULT_SECONDARY_FEPC_PORT              
        = 27000;
    static const std::string    DEFAULT_MY_MAJOR_SUMMARY_ALARM_TOKEN          
        = "diiPAC-MajorAlarm";
    static const std::string    DEFAULT_MY_MINOR_SUMMARY_ALARM_TOKEN          
        = "diiPAC-MinorAlarm";
    static const std::string    DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_TOKEN        
        = "diiPAC-FireCountdown";
    static const std::string    DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN 
        = "dioPAC-FireCountdown";
    static const unsigned long  DEFAULT_MAX_BROADCASTS                   
        = 100;

    // AZ++ hardware timeout

    static const unsigned long  DEFAULT_LIVE_BROADCAST_TIMEOUT_IN_MSECS                   
        = 500;
    static const unsigned long  DEFAULT_DVA_BROADCAST_TIMEOUT_IN_MSECS                   
        = 3000;

    // ++AZ

    const std::string AbstractPAAgentEntityData::FAST_POLL_RATE
        = "FastPollRateInMSecs";
    const std::string AbstractPAAgentEntityData::WORKER_THREADS_SLEEP_TIME
        = "WorkerThreadsSleepTimeInMSecs";
    const std::string AbstractPAAgentEntityData::SOCKET_TIMEOUT
        = "SocketTimeoutInSecs";
    const std::string AbstractPAAgentEntityData::TIS_DISPLAY_DURATION
        = "TisDisplayDurationInSecs";
    const std::string AbstractPAAgentEntityData::KEEP_TERMINATED_BROADCASTS
        = "KeepTerminatedBroadcastsInSecs";
    const std::string AbstractPAAgentEntityData::RETRIES
        = "Retries";
    const std::string AbstractPAAgentEntityData::PRIMARY_FEPC_ADDRESS
        = "PrimaryFEPCAddress";
    const std::string AbstractPAAgentEntityData::SECONDARY_FEPC_ADDRESS
        = "SecondaryFEPCAddress";
    const std::string AbstractPAAgentEntityData::PRIMARY_FEPC_PORT
        = "PrimaryFEPCPort";
    const std::string AbstractPAAgentEntityData::SECONDARY_FEPC_PORT
        = "SecondaryFEPCPort";
    const std::string AbstractPAAgentEntityData::MY_MAJOR_SUMMARY_ALARM_TOKEN              
        = "MyMajorSummaryAlarmToken";
    const std::string AbstractPAAgentEntityData::MY_MINOR_SUMMARY_ALARM_TOKEN              
        = "MyMinorSummaryAlarmToken";
    const std::string AbstractPAAgentEntityData::MY_FIRE_COUNTDOWN_ACTIVE_TOKEN            
        = "MyFireCountdownActiveToken";
    const std::string AbstractPAAgentEntityData::MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN     
        = "MyFireCountdownActiveOutputToken";
    const std::string AbstractPAAgentEntityData::MAX_BROADCASTS                          
        = "MaxBroadcasts";

    // AZ++ hardware timeout

    const std::string AbstractPAAgentEntityData::LIVE_BROADCAST_TIMEOUT_IN_MSECS                          
        = "LiveBroadcastTimeoutInMsecs";
    const std::string AbstractPAAgentEntityData::DVA_BROADCAST_TIMEOUT_IN_MSECS                          
        = "DvaBroadcastTimeoutInMsecs";

    // ++AZ

    const std::string AbstractPAAgentEntityData::FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND = "FireAlarmPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND = "PasZonePollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND = "SystemFaultPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND = "DvaLibPollingIntervalInMillisecond";
    const std::string AbstractPAAgentEntityData::PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND = "PasDvaRecordingMaxDurationInSecond";

	const std::string AbstractPAAgentEntityData::ASSET_NAME = "AssetName";

    static const unsigned long DEFAULT_FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND = 500;
    static const unsigned long DEFAULT_PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND = 200;
    static const unsigned long DEFAULT_SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND = 500;
    static const unsigned long DEFAULT_DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND = 2000;
    static const unsigned long DEFAULT_PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND = 60;

	AbstractPAAgentEntityData::AbstractPAAgentEntityData()
    {
    }


    AbstractPAAgentEntityData::~AbstractPAAgentEntityData()
    {
    }


    /**
     * getTisDisplayDuration
     *
     * Returns the TIS display duration
     *
     * @return unsigned long display duration in secs
     */
    unsigned long AbstractPAAgentEntityData::getTisDisplayDurationInSecs()
    {
        FUNCTION_ENTRY("getTisDisplayDurationInSecs");

		std::string value = getHelper()->getParameter(TIS_DISPLAY_DURATION);

		unsigned long tisDisplayDuration = DEFAULT_TIS_DISPLAY_DURATION;

		if(!value.empty())
		{
			tisDisplayDuration = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return tisDisplayDuration;
	}


    unsigned long AbstractPAAgentEntityData::getKeepTerminatedBroadcastsInSecs()
    {
        FUNCTION_ENTRY("getKeepTerminatedBroadcastsInSecs");

		std::string value = getHelper()->getParameter(KEEP_TERMINATED_BROADCASTS);

		unsigned long keepTerminatedBroadcasts = DEFAULT_KEEP_TERMINATED_BROADCASTS;

		if(!value.empty())
		{
			keepTerminatedBroadcasts = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return keepTerminatedBroadcasts;
	}

    /**
     * getFastPollRate
     *
     * Returns the processing rate for the fast poll loop in msec.  This is the sleep time of the 
     * main PAS table reading loop + the actual time taken to read them.
     *
     * @return processing rate for the fast poll loop in msec.
     */
    unsigned long AbstractPAAgentEntityData::getFastPollRateInMSecs()
    {
        FUNCTION_ENTRY("getFastPollRateInMSecs");
		std::string value = getHelper()->getParameter(FAST_POLL_RATE);
		unsigned long fastPollRate = DEFAULT_FAST_POLL_RATE;

		if(!value.empty())
		{
			fastPollRate = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return fastPollRate;
	}


    unsigned long AbstractPAAgentEntityData::getWorkerThreadsSleepTimeInMSecs()
    {
        FUNCTION_ENTRY("getWorkerThreadsSleepTimeInMSecs");
		std::string value = getHelper()->getParameter(WORKER_THREADS_SLEEP_TIME);

		unsigned long workerThreadsSleepTime = DEFAULT_WORKER_THREADS_SLEEP_TIME;
		if(!value.empty())
		{
			workerThreadsSleepTime = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return workerThreadsSleepTime;
	}


    unsigned long AbstractPAAgentEntityData::getSocketTimeoutInSecs()
    {
        FUNCTION_ENTRY("getSocketTimeoutInSecs");
		unsigned long socketTimeout = DEFAULT_SOCKET_TIMEOUT;

		std::string value = getHelper()->getParameter(SOCKET_TIMEOUT);
		if(!value.empty())
		{
			socketTimeout = EntityHelper::getUnsignedLongData(value);
		}
        FUNCTION_EXIT;
        return socketTimeout;
	}

    /**
     * getRetries
     *
     * Returns the number of retries for FEPC communications failures before switching to the 
     * redundant FEPC.
     *
     * @return number of retries for FEPC comms failures.
     */
    unsigned long AbstractPAAgentEntityData::getRetries()
    {
        FUNCTION_ENTRY("getRetries");
		unsigned long retries = DEFAULT_RETRIES;

		std::string value = getHelper()->getParameter(RETRIES);
		if(!value.empty())
		{
			retries = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return retries;
	}
    
    /**
     * getPrimaryFEPCAddress
     *
     * Returns the IP address of the primary FEPC.
     *
     * @return IP address of the primary FEPC
     */
    std::string AbstractPAAgentEntityData::getPrimaryFEPCAddress()
    {
        FUNCTION_ENTRY("getPrimaryFEPCAddress");
		std::string primaryFEPCAddress(DEFAULT_PRIMARY_FEPC_ADDRESS);
		std::string value = getHelper()->getParameter(PRIMARY_FEPC_ADDRESS);
		if(!value.empty())
		{
			primaryFEPCAddress = value;
		}

        FUNCTION_EXIT;
        return primaryFEPCAddress;
	}
    
    /**
     * getSecondaryFEPCAddress
     *
     * Returns the IP address of the secondary FEPC.
     *
     * @return IP address of the primary FEPC
     */
    std::string AbstractPAAgentEntityData::getSecondaryFEPCAddress()
    {
        FUNCTION_ENTRY("getSecondaryFEPCAddress");
		std::string secondaryFEPCAddress(DEFAULT_SECONDARY_FEPC_ADDRESS);
		std::string value = getHelper()->getParameter(SECONDARY_FEPC_ADDRESS);
		if(!value.empty())
		{
			secondaryFEPCAddress = value;
		}

        FUNCTION_EXIT;
        return secondaryFEPCAddress;
	}
    
    /**
     * getPrimaryFEPCPort
     *
     * Returns the TCP/IP port of the primary FEPC.
     *
     * @return port of the primary FEPC
     */
    unsigned long AbstractPAAgentEntityData::getPrimaryFEPCPort()
    {
        FUNCTION_ENTRY("getPrimaryFEPCPort");
		unsigned long primaryFEPCPort = DEFAULT_PRIMARY_FEPC_PORT;
		std::string value = getHelper()->getParameter(PRIMARY_FEPC_PORT);
		if(!value.empty())
		{
			primaryFEPCPort = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return primaryFEPCPort;
	}
    
    /**
     * getSecondaryFEPCPort
     *
     * Returns the TCP/IP port of the secondary FEPC.
     *
     * @return port of the primary FEPC
     */
    unsigned long AbstractPAAgentEntityData::getSecondaryFEPCPort()
    {
        FUNCTION_ENTRY("getSecondaryFEPCPort");
		unsigned long  secondaryFEPCPort = DEFAULT_SECONDARY_FEPC_PORT;
		std::string value = getHelper()->getParameter(SECONDARY_FEPC_PORT);
		if(!value.empty())
		{
			secondaryFEPCPort = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return secondaryFEPCPort;
	}
     
    unsigned long AbstractPAAgentEntityData::getMaxBroadcasts()
    {
        FUNCTION_ENTRY("getMaxBroadcasts");
		unsigned long maxBroadcasts = DEFAULT_MAX_BROADCASTS;
		std::string value = getHelper()->getParameter(MAX_BROADCASTS);
		if(!value.empty())
		{
			maxBroadcasts = EntityHelper::getUnsignedLongData(value);
		}

        FUNCTION_EXIT;
        return maxBroadcasts;
	}

    std::string AbstractPAAgentEntityData::getMyMajorSummaryAlarmToken()
    {
        FUNCTION_ENTRY("getMyMajorSummaryAlarmToken");
		std::string myMajorSummaryAlarmToken(DEFAULT_MY_MAJOR_SUMMARY_ALARM_TOKEN);
		std::string value = getHelper()->getParameter(MY_MAJOR_SUMMARY_ALARM_TOKEN);
		if(!value.empty())
		{
			myMajorSummaryAlarmToken = value;
		}

        FUNCTION_EXIT;
        return myMajorSummaryAlarmToken;
	}

    std::string AbstractPAAgentEntityData::getMyMinorSummaryAlarmToken()
    {
        FUNCTION_ENTRY("getMyMinorSummaryAlarmToken");
		std::string myMinorSummaryAlarmToken(DEFAULT_MY_MINOR_SUMMARY_ALARM_TOKEN);
		std::string value = getHelper()->getParameter(MY_MINOR_SUMMARY_ALARM_TOKEN);
		if(!value.empty())
		{
			myMinorSummaryAlarmToken = value;
		}

        FUNCTION_EXIT;
        return myMinorSummaryAlarmToken;
	}

    std::string AbstractPAAgentEntityData::getMyFireCountdownActiveToken()
    {
        FUNCTION_ENTRY("getMyFireCountdownActiveToken");
		std::string myFireCountdownActiveToken(DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_TOKEN);
		std::string value = getHelper()->getParameter(MY_FIRE_COUNTDOWN_ACTIVE_TOKEN);
		if(!value.empty())
		{
			myFireCountdownActiveToken = value;
		}

        FUNCTION_EXIT;
        return myFireCountdownActiveToken;
	}

    std::string AbstractPAAgentEntityData::getMyFireCountdownActiveOutputToken()
    {
        FUNCTION_ENTRY("getMyFireCountdownActiveOutputToken");
		std::string myFireCountdownActiveOutputToken(DEFAULT_MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN);
		std::string value = getHelper()->getParameter(MY_FIRE_COUNTDOWN_ACTIVE_OUTPUT_TOKEN);
		if(!value.empty())
		{
			myFireCountdownActiveOutputToken = value;
		}

        FUNCTION_EXIT;
        return myFireCountdownActiveOutputToken;
	}

    // AZ++ hardware timeout

    unsigned long AbstractPAAgentEntityData::getLiveBroadcastTimeoutInMsecs()
    {
        FUNCTION_ENTRY("getLiveBroadcastTimeoutInMsecs");
		unsigned long liveBroadcastTimeoutInMsecs = DEFAULT_LIVE_BROADCAST_TIMEOUT_IN_MSECS;
		std::string value = getHelper()->getParameter(LIVE_BROADCAST_TIMEOUT_IN_MSECS);
		if(!value.empty())
		{
			liveBroadcastTimeoutInMsecs = EntityHelper::getUnsignedLongData(value);
		}
        
        FUNCTION_EXIT;
        return liveBroadcastTimeoutInMsecs;
    }

    unsigned long AbstractPAAgentEntityData::getDvaBroadcastTimeoutInMsecs()
    {
        FUNCTION_ENTRY("getDvaBroadcastTimeoutInMsecs");
		unsigned long dvaBroadcastTimeoutInMsecs = DEFAULT_DVA_BROADCAST_TIMEOUT_IN_MSECS;
		std::string value = getHelper()->getParameter(DVA_BROADCAST_TIMEOUT_IN_MSECS);
		if(!value.empty())
		{
			dvaBroadcastTimeoutInMsecs = EntityHelper::getUnsignedLongData(value);
		}
        
        FUNCTION_EXIT;
        return dvaBroadcastTimeoutInMsecs;
    }

    // ++AZ

	std::string AbstractPAAgentEntityData::getAssetName()
	{
        FUNCTION_ENTRY("getAssetName");
		std::string assetName("Unknown");
		std::string value = getHelper()->getParameter(ASSET_NAME);
		if(!value.empty())
		{
			assetName = value;
		}
        
        FUNCTION_EXIT;
        return assetName;
	}

    void AbstractPAAgentEntityData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        getHelper()->invalidate();

        FUNCTION_EXIT;
    }

    unsigned long AbstractPAAgentEntityData::getFireAlarmPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("getFireAlarmPollingIntervalInMillisecond");
        unsigned long fireAlarmPollingIntervalInMillisecond = DEFAULT_FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND;

        std::string value = getHelper()->getParameter(FIRE_ALARM_POLLING_INTERVAL_IN_MILLISECOND);
        if(!value.empty())
        {
            fireAlarmPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return fireAlarmPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getPasZonePollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("getPasZonePollingIntervalInMillisecond");
        unsigned long pasZonePollingIntervalInMillisecond = DEFAULT_PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND;

        std::string value = getHelper()->getParameter(PAS_ZONE_POLLING_INTERVAL_IN_MILLISECOND);
        if(!value.empty())
        {
            pasZonePollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return pasZonePollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getSystemFaultPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("SystemFaultPollingIntervalInMillisecond");
        unsigned long systemFaultPollingIntervalInMillisecond = DEFAULT_SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND;

        std::string value = getHelper()->getParameter(SYSTEM_FAULT_POLLING_INTERVAL_IN_MILLISECOND);
        if(!value.empty())
        {
            systemFaultPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return systemFaultPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getDvaLibPollingIntervalInMillisecond()
    {
        FUNCTION_ENTRY("DvaLibPollingIntervalInMillisecond");
        unsigned long dvaLibPollingIntervalInMillisecond = DEFAULT_DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND;

        std::string value = getHelper()->getParameter(DVA_LIB_POLLING_INTERVAL_IN_MILLISECOND);
        if(!value.empty())
        {
            dvaLibPollingIntervalInMillisecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return dvaLibPollingIntervalInMillisecond;
    }

    unsigned long AbstractPAAgentEntityData::getPasDvaRecordingMaxDurationInSecond()
    {
        FUNCTION_ENTRY("PasDvaRecordingMaxDurationInSecond");
        unsigned long pasDvaRecordingMaxDurationInSecond = DEFAULT_PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND;

        std::string value = getHelper()->getParameter(PAS_DVA_RECORDING_MAX_DURATION_IN_SECOND);
        if(!value.empty())
        {
            pasDvaRecordingMaxDurationInSecond = EntityHelper::getUnsignedLongData(value);
        }

        FUNCTION_EXIT;
        return pasDvaRecordingMaxDurationInSecond;
    }

} //close namespace TA_Base_Core
