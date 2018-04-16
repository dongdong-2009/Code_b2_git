#include "app/response_plans/plan_step_plugin/common_editor/src/TrainIDEditorHelper.h"
#include <sstream>
#include <iomanip>
#include <ace/Guard_T.h>

namespace TA_Base_App
{

TrainIDEditorHelper::TrainIDEditorHelper()
{
}

TrainIDEditorHelper::TrainIDEditorHelper(const unsigned int trainIdBegin, const unsigned int trainIdEnd)
{
	ACE_Write_Guard<ACE_RW_Mutex> gudWrite(m_mtxTrainIds);
	for(unsigned int i = trainIdBegin; i <= trainIdEnd; ++i)
	{
		m_trainIds.insert(i);
	}
}

TrainIDEditorHelper::TrainIDEditorHelper(const std::set<unsigned int>& trainIds) : m_trainIds(trainIds)
{
}

TrainIDEditorHelper::~TrainIDEditorHelper()
{

}

std::vector<ListItem> TrainIDEditorHelper::getTrainIdItems()
{
	ACE_Write_Guard<ACE_RW_Mutex> gudWrite(m_mtxTrainIds);
	if(m_trainIdItems.empty())
	{
		unsigned int itemWidth = 1;
		std::set<unsigned int>::const_reverse_iterator rit = m_trainIds.rbegin();
		if(rit != m_trainIds.rend())
		{
			unsigned int tmp = *rit;
			while(tmp /= 10)
			{
				itemWidth++;
			}
		}

	    std::stringstream ss;
		for(std::set<unsigned int>::const_iterator it = m_trainIds.begin(); it != m_trainIds.end(); ++it)
	    {
	        ss.clear();
	        ss.str("");
	        ss << std::setw(itemWidth) << std::setfill('0') << *it;
	        ListItem tmp = {*it, ss.str()};
	        m_trainIdItems.push_back(tmp);
	    }
	}
    return m_trainIdItems;
}

bool TrainIDEditorHelper::isTrainIdValid(const unsigned int trainId )
{
	ACE_Read_Guard<ACE_RW_Mutex> gudRead(m_mtxTrainIds);
	return m_trainIds.find(trainId) != m_trainIds.end();
}

void TrainIDEditorHelper::initHelper(const std::set<unsigned int>& trainIds )
{
	ACE_Write_Guard<ACE_RW_Mutex> gudWrite(m_mtxTrainIds);
	m_trainIds = trainIds;
	m_trainIdItems.clear();
}

}