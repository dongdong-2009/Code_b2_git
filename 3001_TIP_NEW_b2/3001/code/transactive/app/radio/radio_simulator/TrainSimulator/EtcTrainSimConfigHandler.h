#pragma once

#include "app/radio/radio_simulator/CommandProcessorLib/src/TrainCommandProcessorThread.h"
#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include "core\utilities\src\DebugUtil.h"
#include "AutoSendDataModel.h"

#include <string>
#include <vector>
#include "boost\scoped_ptr.hpp"
#include "PECAutoSendConfigHandler.h"
#include "PAAutoSendConfigHandler.h"
#include "TtisAutoSendConfigHandler.h"
#include "ChangeOverConfHandler.h"
#include "TrainCountAutoSendConfHandler.h"
#include "RadioIdSetupConfigHandler.h"

class EtcTrainSimConfigHandler
{
	public:
		
		~EtcTrainSimConfigHandler(void);
		static EtcTrainSimConfigHandler * getInstance ();
		static void deleteInstace ();

		void updateEtcCConfiguration (IConfigDataItemPtrType autoSendDataPtr);
		IConfigDataItemPtrType getAutoSendingData (EAutoSendType autoSendType);

		std::vector<unsigned long> getControlledLocation (bool bRelease = true);
		long getChangeOverFailureStatus ();

	private:

		EtcTrainSimConfigHandler(void);
	
		static EtcTrainSimConfigHandler * m_iniConfInstance;
		static TA_Base_Core::ReEntrantThreadLockable m_threadLock;

		std::string convertToString (bool bConvert);
		IConfigDataItemPtrType createAutoSendingData (EAutoSendType autoSendType);

		PECAutoSendConfigHandler * m_pecHandler;
		PAAutoSendConfigHandler * m_paHandler;
		TtisAutoSendConfigHandler * m_ttisHandler;
		ChangeOverConfHandler * m_changeOverhandler;
		TrainCountAutoSendConfHandler * m_trainCountHandler;
		RadioIdSetupConfigHandler * m_radioConfHandler;

		std::vector <unsigned long> parseData (const std::string & strData);
		long stringToLong (const std::string & strData);

};


