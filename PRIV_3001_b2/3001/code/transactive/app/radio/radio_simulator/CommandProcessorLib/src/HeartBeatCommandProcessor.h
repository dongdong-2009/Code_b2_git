/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: \\depot\3001_TIP\3001\transactive\app\radio\radio_simulator\CommandProcessorLib\src\HeartBeatCommandProcessor.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/09/10 10:05:25 $
  * Last modified by:  $Author: builder $
  *
  * This class is used for HeartBeat command processing.
  */

#ifndef _HEARTBEAT_COMMAND_PROCESSOR_H_
#define _HEARTBEAT_COMMAND_PROCESSOR_H_
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "core/threads/src/Thread.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/MotorolaSDSProtocolHelper.h"
#include "app/radio/radio_simulator/TrainSimulatorLib/src/IRadioCommsObserver.h"

#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <vector>

namespace TA_IRS_App
{
	class HeartBeatCommandProcessor : public TA_Base_Core::Thread, public TA_Base_Core::ReEntrantThreadLockable
	{
	public:
		/**
		 * Constructor
		 */
		HeartBeatCommandProcessor(TrainHeartBeatInfo info, MotorolaSDSProtocolHelper* sdsProtolHelper);

		/**
		 * Destructor
		 */
		virtual ~HeartBeatCommandProcessor();

		virtual void run();
		/**
		 * Ends the thread, only ever called on agent shutdown.
		 */
		virtual void terminate();

		void processTrainHeartBeat(std::vector<unsigned char>& heartBeatProtocol);

		std::string getTrainStatus() const;

		/**
		 * setDataSender
		 */
		void registerDataSender(IRadioCommsObserver* radioCommsObserver);

		TrainHeartBeatInfo getHeartBeatInfo();
			

private:
		unsigned int m_trainID;
		bool         m_running;
		unsigned int m_delay;

		MotorolaSDSProtocolHelper* m_sdsProtocolHelper;

		static const unsigned short CRITICAL_ALARM_LENGTH;
		static const unsigned char  CRITICAL_ALARM_VALUE;
		static const unsigned short FLAG;
		static const unsigned short COMMAND_VALUE;
		static const unsigned short ORIGIN;
		static const unsigned short PROTOCOL_LENGTH;

		std::vector<unsigned char> buildCriticalAlarmTable();
		std::vector<unsigned char> buildHeartBeatProtocol();

		IRadioCommsObserver* m_radioCommsObserver;
		
	};
}
#endif