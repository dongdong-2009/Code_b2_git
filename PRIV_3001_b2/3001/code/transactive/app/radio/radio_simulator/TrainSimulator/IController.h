
#ifndef _I_CONTROLLER_H_
#define _I_CONTROLLER_H_
#include "GlobalDataTypes.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include <string>
#include <vector>
#include "core/threads/src/Thread.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"

#include "AutoSendDataModel.h"

namespace TA_IRS_App
{
	class IController
	{
	public:
		virtual void onBtnClick(const std::string& caption, unsigned int ID) = 0;
		virtual void logViewID(ResourceInfo infoResource) = 0;
		virtual void pecEvent( const std::string& pecMessage) = 0;
		virtual HeartBeatInfo getTrainHeartBeatInfo() = 0;
		virtual void addTrainHeartBeat(int trainID, int delay) = 0;
		virtual TA_Base_Core::Thread::ThreadState_t getCurrentState() = 0;
		virtual int getCommsObserverStatus() = 0;
		virtual void sendStatusTrainHeartBeat(TrainStatus stat, int trainID) = 0;
		virtual std::vector<TA_IRS_App::HeartBeatCommandProcessor*> getHeartBeatProcessor() = 0;
		virtual void addTrainResponseStrategy(const TrainStrategyInfo& info) = 0;
		virtual std::map<int, TrainStrategyInfo> getTrainStrategyMap() = 0;
		virtual void removeTrainResponseStrategy(int trainID) = 0;
		virtual bool sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains, bool isUsePrimary) = 0;
		

		// for auto sending configuration procedure
		virtual void onAutoSendConfPECClick (AutoSendingPecConfInfo autoSendInfo) =0;
		virtual void onAutoSendConfPaClick (AutoSendingPAConfInfo autoSendInfo) =0;
		virtual void onAutoSendConfTTISClick (AutoSendingTTISConfInfo autoSendInfo) =0;
		virtual void onChangeOverClick (ChangeOverConInfo autoSendInfo) =0;
		virtual void onTrainCountCmboSelChange (TrainCountInfo autoSendInfo) =0;
		virtual void onRadioIdComboSelChange (RadioIdSetupInfo radioIdSetupInfo) = 0;
		virtual IConfigDataItemPtrType getUpdateConfForAutoChanging (EAutoSendType autoSendType) =0;
		
		// for state change
		virtual void onSendState (bool bRelease =  false) = 0;
	};
}
#endif