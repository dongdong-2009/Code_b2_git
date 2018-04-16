
#ifndef _I_MODEL_H_
#define _I_MODEL_H_
#include <string>
#include "IView.h"
#include "GlobalDataTypes.h"
#include "app/radio/radio_simulator/TrainSimulator/UDPSocketManager.h"
#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"
#include "core/threads/src/Thread.h"
#include "app/radio/radio_simulator/CommandProcessorLib/src/HeartBeatCommandProcessor.h"
#include <map>
#include <vector>

#include "AutoSendDataModel.h"

namespace TA_IRS_App
{
	class IModel
	{
	public:
		virtual void setCaption(const std::string& caption, unsigned int ID) = 0;
		virtual void attach(IView* view) = 0;
		virtual void dettach(IView* view) = 0;
		virtual void logViewID(ResourceInfo infoResource) = 0;
		virtual void pecEvent(const std::string& pecMsge) = 0;
		virtual TA_IRS_App::CUDPSocketManager* getUDPSocketManager() = 0;
		virtual ResourceInfo getLogViewId() = 0;
		virtual void beginDownload(std::vector<unsigned char>& data) = 0;
	
		virtual HeartBeatInfo getTrainHeartBeatInfo() = 0;
		virtual void addTrainHeartBeat(int trainID, int delay) = 0;
		virtual TA_Base_Core::Thread::ThreadState_t getCurrentState() = 0;
		virtual int getCommsObserverStatus() = 0;
		virtual void sendStatusTrainHeartBeat(TrainStatus stat, int trainID) = 0;
		virtual std::vector<TA_IRS_App::HeartBeatCommandProcessor*> getHeartBeatProcessor() = 0;
		virtual void addTrainResponseStrategy(const TrainStrategyInfo& info) = 0;
		virtual std::map<int, TrainStrategyInfo> getTrainStrategyMap() = 0;
		virtual void removeTrainResponseStrategy(int trainID) = 0;

		virtual bool sendTIMSCommand(CommandType cmdType, int status, const std::vector<int>& trains,bool bIsPrimary) = 0;
		
		// for auto sending configuration procedure
		virtual void updateEctConfiguration (IConfigDataItemPtrType autoSendingConfData) =0;
		virtual IConfigDataItemPtrType getUpdateConfForAutoChanging (EAutoSendType autoSendType) =0;
		

		// for state change sending
		virtual void sendStateUpdate (bool bRelease = false) = 0;
	};
}
#endif