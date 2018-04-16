#include "app/signs/pids_manager/src/TrainList.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include <boost/lexical_cast.hpp>
#include <boost/phoenix.hpp>
#include <algorithm>

namespace TA_IRS_App
{

using namespace boost::phoenix;
using namespace boost::phoenix::placeholders;

TrainList::TrainList(void)
{
}

TrainList::~TrainList(void)
{
}

TrainList& TrainList::instance()
{
	static TrainList _trainList;
	return _trainList;
}

void TrainList::registerObserver( ITrainListUpdateObserver* observer )
{
	TA_Base_Core::ThreadGuard guard(m_observerLock);
	m_observers.insert(observer);
}

void TrainList::unregisterObserver( ITrainListUpdateObserver* observer )
{
	TA_Base_Core::ThreadGuard guard(m_observerLock);
	m_observers.erase(observer);
}

std::vector<Train> TrainList::getTrainList()
{
	TA_Base_Core::ThreadGuard guard(m_trainListLock);
	return m_trainList;
}

void TrainList::updateTrainList( TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList_var& trainInfoList )
{
	if(!trainInfoList.operator ->())
	{
		return;
	}
	std::vector<Train> newTrainList;
	TA_Base_Core::ThreadGuard guard(m_trainListLock);
	for(int i = 0, sz = trainInfoList->length(); i < sz; ++i)
	{
		const TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef& trainInfo = trainInfoList[i];
		if(trainInfo.trainValid)
		{
			Train t;
			t.first = trainInfo.localisation.physicalTrainService.in();
			t.second = boost::lexical_cast<int>(trainInfo.physicalTrainNumber.in());
			newTrainList.push_back(t);
		}
		
	}
	std::sort(newTrainList.begin(), newTrainList.end(), bind(&Train::first, arg1) < bind(&Train::first, arg2));

	if(newTrainList != m_trainList)
	{
		m_trainList = newTrainList;

		TA_Base_Core::ThreadGuard guard(m_observerLock);
		std::for_each(m_observers.begin(), m_observers.end(), bind(&ITrainListUpdateObserver::notifyTrainListUpdate, arg1, m_trainList));
	}
}

}
