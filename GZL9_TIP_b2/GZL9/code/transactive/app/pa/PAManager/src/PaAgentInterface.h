#pragma once

#include "bus/pa/pa_agent/IDL/src/IStationPaMethod.h"
#include "core/naming/src/NamedObject.h"

interface PaAgentInterface
{
    // Static named object
    static TA_Base_Core::NamedObject<TA_Base_Bus::IStationPaMethod, TA_Base_Bus::IStationPaMethod_ptr, TA_Base_Bus::IStationPaMethod_var>* s_pPaAgentNamedObject;

    static void createCorbaObject( const std::string& strEntityName );

    static void removeCorbaObject();

    //static int startDVABroadcast( std::string& strBroadcastId, std::vector<unsigned long> vecZoneKeys, unsigned long ulDVAMsgKey, int nRepeatTimes, time_t tmStartTimeInDay );

	static int PaAgentInterface::startDVABroadcast( std::string& strBroadcasstId, 
		std::vector<unsigned long> vecZoneKeys, 
		std::vector<unsigned long> ulDVAMsgKeys, 
		int nCylicTimes, 
		int nDwellTime,
		time_t tmStartTimeInDay );
	static int startBroadcastMonitor( std::vector<unsigned long> vecZoneKeys);//added by lixiaoxia

	static int stopBroadcastMonitor( std::vector<unsigned long> vecZoneKeys );//added by lixiaoxia

    static int stopDVABroadcast( std::vector<unsigned long> vecZoneKeys );

    static int removeScheduleDVABroadcast( unsigned long ulScheduleKey );

    static int startDVAMsgPlayback( std::string& strBroadcastId, unsigned long ulDVAMsgKey );

    static int stopDVAMsgPlayback();

    static int startLiveBroadcast( std::string& strBroadcastId, std::vector<unsigned long> vecZoneKeys );

    static int stopLiveBroadcast( std::vector<unsigned long> vecZoneKeys );

	static int startBackgroundMusicBroadcast( std::vector<unsigned long> vecZoneKeys );

	static int stopBackgroundMusicBroadcast( std::vector<unsigned long> vecZoneKeys );

    static int getBackupKeyStatus( const std::string& strBackupName, int& nBackupKey );
};
