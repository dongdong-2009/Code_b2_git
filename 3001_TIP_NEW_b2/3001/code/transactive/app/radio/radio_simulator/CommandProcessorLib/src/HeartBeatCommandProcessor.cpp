/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\HeartBeatCommandProcessor.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/09/10 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for HeartBeat command processing.
  */

#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"

namespace TA_IRS_App
{
	const unsigned short HeartBeatCommandProcessor::CRITICAL_ALARM_LENGTH = 29;
	const unsigned char HeartBeatCommandProcessor::CRITICAL_ALARM_VALUE = 0xff;
	const unsigned short HeartBeatCommandProcessor::FLAG = 7;
	const unsigned short HeartBeatCommandProcessor::COMMAND_VALUE = 61;
	const unsigned short HeartBeatCommandProcessor::ORIGIN = 0;
	const unsigned short HeartBeatCommandProcessor::PROTOCOL_LENGTH = 36;

	HeartBeatCommandProcessor::HeartBeatCommandProcessor(TrainHeartBeatInfo info, MotorolaSDSProtocolHelper* sdsProtolHelper)
		: m_running(false)
		 , m_radioCommsObserver(NULL)
	{
		FUNCTION_ENTRY("HeartBeatCommandProcessor");

		m_trainID = info.TrainID;
		m_delay   = info.DelayTime;
		m_sdsProtocolHelper = sdsProtolHelper;

		FUNCTION_EXIT;
	}

	HeartBeatCommandProcessor::~HeartBeatCommandProcessor()
	{
		FUNCTION_ENTRY("~HeartBeatCommandProcessor");

		//stop the thread
		terminateAndWait();

		FUNCTION_EXIT;
	}

	void HeartBeatCommandProcessor::run()
	{
		FUNCTION_ENTRY("run");

		m_running = true;

		std::vector<unsigned char> _heartBeatProtocol = buildHeartBeatProtocol();		

		while( true == m_running )
		{		
			//Sleep in seconds
			Thread::sleep(m_delay*1000);

			//Process TrainHeartBeat
			processTrainHeartBeat(_heartBeatProtocol);
			
		}

		FUNCTION_EXIT;
	}

	void HeartBeatCommandProcessor::terminate()
	{
		FUNCTION_ENTRY("terminate");

		m_running = false;

		FUNCTION_EXIT;
	}

	std::vector<unsigned char> HeartBeatCommandProcessor::buildCriticalAlarmTable()
	{
		FUNCTION_ENTRY("buildCriticalAlarmTable");

		std::vector<unsigned char> _ret;
		for (int x = 0; x < CRITICAL_ALARM_LENGTH; x++)
		{
			//Fixed Value of critical alarm
			_ret.push_back(CRITICAL_ALARM_VALUE);
		}

		FUNCTION_EXIT;
		return _ret;
	}

	std::vector<unsigned char> HeartBeatCommandProcessor::buildHeartBeatProtocol()
	{
		FUNCTION_ENTRY("buildHeartBeatProtocol");

		std::vector<unsigned char> _ret;

		_ret.push_back(FLAG);
		_ret.push_back(COMMAND_VALUE);
		_ret.push_back(m_trainID);
		_ret.push_back(PROTOCOL_LENGTH);
		_ret.push_back(ORIGIN);

		std::vector<unsigned char> _criticalAlarm = buildCriticalAlarmTable();
		_ret.insert(_ret.end(),_criticalAlarm.begin(),_criticalAlarm.end());

		FUNCTION_EXIT;
		return _ret;
	}

	void HeartBeatCommandProcessor::processTrainHeartBeat(std::vector<unsigned char>& heartBeatProtocol)
	{
		FUNCTION_ENTRY("processTrainHeartBeat");

		std::vector<unsigned char>* _heartBeatData = m_sdsProtocolHelper->buildMotorolaHeadersHeartBeat(heartBeatProtocol);
	
		if( NULL != m_radioCommsObserver )
		{
			m_sdsProtocolHelper->logger(*_heartBeatData);
			m_radioCommsObserver->sendData(*_heartBeatData);
			
		}
		//make sure to delete the object after use
		delete _heartBeatData;
		_heartBeatData = NULL;

		FUNCTION_EXIT;
	}

	void HeartBeatCommandProcessor::registerDataSender(IRadioCommsObserver* radioCommsObserver)
	{
		FUNCTION_ENTRY("setDataSender");

		m_radioCommsObserver = radioCommsObserver;
		
		FUNCTION_EXIT;
	}

	TrainHeartBeatInfo HeartBeatCommandProcessor::getHeartBeatInfo()
	{
		TrainHeartBeatInfo _info;
		_info.TrainID = m_trainID;
		_info.DelayTime = m_delay;
		_info.StatusInfo = getTrainStatus();

		return _info;
	}

	std::string HeartBeatCommandProcessor::getTrainStatus() const
	{
		std::string _ret = "";
		if ( true == m_running)
		{
			_ret = "Running";
		}
		else
			_ret = "Stopped";

		return _ret;
	}

}