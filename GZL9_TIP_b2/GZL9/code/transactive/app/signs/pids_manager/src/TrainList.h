#pragma once

#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include <boost/noncopyable.hpp>
#include <vector>
#include <string>
#include <utility>
#include <set>

namespace TA_IRS_App
{

typedef std::pair<std::string, int> Train;

class ITrainList
{
public:
	virtual void updateTrainList(TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var& trainInfoList) = 0;
};

class TrainList;

class ITrainListUpdateObserver
{
public:
	virtual void notifyTrainListUpdate(const std::vector<Train>& trainList) = 0;
};

class TrainList : public ITrainList, boost::noncopyable
{
public:
	static TrainList& instance();
	void registerObserver(ITrainListUpdateObserver* observer);
	void unregisterObserver(ITrainListUpdateObserver* observer);
	std::vector<Train> getTrainList();
	virtual void updateTrainList(TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var& trainInfoList);

private:
	TrainList(void);
	~TrainList(void);

	std::vector<Train> m_trainList;
	std::set<ITrainListUpdateObserver*> m_observers;
	TA_Base_Core::ReEntrantThreadLockable m_observerLock;
	TA_Base_Core::ReEntrantThreadLockable m_trainListLock;
};

}

