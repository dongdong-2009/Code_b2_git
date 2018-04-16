#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/StationTisStepEditorParameter.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/common/StationTisStepParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "bus/signs/tis_agent_access/src/TISAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"
#include <algorithm>
#include <sstream>
#include <boost/foreach.hpp>

namespace TA_Base_App
{
StationTisStepEditorParameter::StationTisStepEditorParameter(StationTisStepParameter& refParameter) :
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_predefinedMsgHelper( TisPredefinedMsgEditorHelper::STATION )
{
}

StationTisStepEditorParameter::~StationTisStepEditorParameter()
{
}

bool StationTisStepEditorParameter::failureIgnorable()
{
    return true;
}

bool StationTisStepEditorParameter::skippable()
{
    return true;
}

bool StationTisStepEditorParameter::delayable()
{
    return true;
}

bool StationTisStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void StationTisStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamStationValid = false;
	bool ParamAllStationsValid = true;
	bool ParamStationPidsValid = false;
	bool ParamMessageValid = false;
	bool ParamPriorityValid = false;
	bool ParamDurationValid = false;

	std::string valueStationPids;
	BOOST_FOREACH(const std::string& i, m_refParameter.stationPids)
	{
		valueStationPids += (i + ",");
	}
	if(!valueStationPids.empty())
	{
		valueStationPids.resize(valueStationPids.size() - 1);
	}

	std::string valueMessage =
		m_predefinedMsgHelper.getPredefinedMsgBySectionAndMessageId(m_refParameter.message.librarySection, m_refParameter.message.messageId).description;

	std::string valuePriority = m_priorityHelper.getPriorityDescription(m_refParameter.priority);

	std::string valueDuration = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmSsTime);

    ItemPair itmNameValue;

	if(m_refParameter.allStations)
	{
		ParamStationValid = true;
		ParamStationPidsValid = true;
	}
	else
	{
		ParamStationValid = !m_refParameter.station.empty();
		ParamStationPidsValid  = !valueStationPids.empty();
	}

	ParamMessageValid = !valueMessage.empty();
	ParamPriorityValid = !valuePriority.empty();
	ParamDurationValid = !valueDuration.empty();

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.station, ParamStation, ParamStation, eList, ParamStationValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.allStations, ParamAllStations, ParamAllStations, eBoolean, ParamAllStationsValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueStationPids, ParamStationPids, ParamStationPids, eList, ParamStationPidsValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMessage, ParamMessage, ParamMessage, eList, ParamMessageValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePriority, ParamPriority, ParamPriority, eList, ParamPriorityValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueDuration, ParamDuration, ParamDuration, eHhMmSsTime, ParamDurationValid);
}

std::string StationTisStepEditorParameter::displayValue( const std::string& strName )
{
	if (strName == ParamPriority)
	{
		return m_priorityHelper.getPriorityDescription(m_refParameter.priority);
	}
	else if (strName == ParamStationPids)
	{
		std::string valueStationPids;
		BOOST_FOREACH(const std::string& i, m_refParameter.stationPids)
		{
			valueStationPids += (i + ",");
		}
		if(!valueStationPids.empty())
		{
			valueStationPids.resize(valueStationPids.size() - 1);
		}

		return valueStationPids;
	}

	return "";
}

IBoolEditor* StationTisStepEditorParameter::getBoolEditor(const std::string& strName)
{
    return this;
}

IListPicker* StationTisStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

bool StationTisStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamAllStations))
    {
		bool oldValue = m_refParameter.allStations;
        m_refParameter.allStations = bValue;
        convertToDisplayValue(m_refParameter.allStations, strDisplayValue);
        bValid = true;
		if(!oldValue && bValue)
		{
			m_refParameter.station = "";
			return true;
		}
		else if(oldValue && !bValue)
		{
			m_refParameter.stationPids.clear();
			return true;
		}
    }
    return false;
}

void StationTisStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamAllStations))
    {
        bValue = m_refParameter.allStations;
    }
}

void StationTisStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamStation)
    {
        vecKeyItems = m_stationHelper.getStationItems();
        unsigned int selected = m_stationHelper.getIdByStation(m_refParameter.station);
        if(TisStationEditorHelper::INVALID_ID != selected)
        {
            vecSelected.push_back(selected);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamStationPids)
    {
        vecKeyItems = m_pidHelper.getPidItems(m_refParameter.station);
        BOOST_FOREACH(const std::string & i, m_refParameter.stationPids)
        {
            unsigned int selected = m_pidHelper.getIdByPid(i);
            if(TisPidEditorHelper::INVALID_ID == selected)
            {
                continue;
            }
            vecSelected.push_back(selected);
        }
        nMaxSelection = vecKeyItems.size();
    }
    else if(strName == ParamMessage)
    {
        vecKeyItems = m_predefinedMsgHelper.getPredefinedMsgItems();
        m_refParameter.message.libraryVersion = m_predefinedMsgHelper.getLibraryVersion();
        unsigned int selectedItem =
            m_predefinedMsgHelper.getIdBySectionAndMessageId(m_refParameter.message.librarySection, m_refParameter.message.messageId);
        if(TisPredefinedMsgEditorHelper::INVALID_ID != selectedItem)
        {
            vecSelected.push_back(selectedItem);
        }
        nMaxSelection = 1;
    }
    else if(strName == ParamPriority)
    {
        vecKeyItems = m_priorityHelper.getPriorityItems();
        if(m_priorityHelper.isValidPriority(m_refParameter.priority))
        {
            vecSelected.push_back(m_refParameter.priority);
        }
        nMaxSelection = 1;
    }
}

bool StationTisStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamStation)
    {
		if(m_refParameter.allStations)
		{
			return false;
		}
		if (vecSelectedKeys.empty())
		{
			if(m_refParameter.station == "")
			{
				return false;
			}
			m_refParameter.station = "";
			m_refParameter.stationPids.clear();
			return true;
		}
		const std::string selStation = m_stationHelper.getStationById(vecSelectedKeys[0]);
		if(selStation != m_refParameter.station)
		{
			m_refParameter.station = selStation;
			m_refParameter.stationPids.clear();
			return true;
		}
		strDisplayValue = m_refParameter.station;
    }
    else if(strName == ParamStationPids)
    {
		m_refParameter.stationPids.clear();
		if(vecSelectedKeys.empty())
		{
			return false;
		}

        BOOST_FOREACH(const unsigned int& i, vecSelectedKeys)
        {
            const std::string pids = m_pidHelper.getPidById(i);
            if(pids.empty())
            {
                continue;
            }
            m_refParameter.stationPids.push_back(pids);
            strDisplayValue += (pids + ",");
        }
        if(!strDisplayValue.empty())
        {
            strDisplayValue.resize(strDisplayValue.size() - 1);
        }
    }
    else if(strName == ParamMessage)
    {
		if(vecSelectedKeys.empty())
		{
			return false;
		}
        const TisConfig::PredefinedTisMessage msg = m_predefinedMsgHelper.getPredefinedMsgById(vecSelectedKeys[0]);
        if(msg.messageTag != TisConfig::PredefinedTisMessage::TIS_UNDEFINED_MESSAGE_ID)
        {
            m_refParameter.message.librarySection = msg.librarySection;
            m_refParameter.message.messageId = msg.messageTag;
        }
		strDisplayValue = msg.description;
    }
    else if(strName == ParamPriority)
    {
        m_refParameter.priority = vecSelectedKeys[0];
		strDisplayValue = m_priorityHelper.getPriorityDescription(m_refParameter.priority);
    }
	return false;
}

ITimeEditor* StationTisStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

bool StationTisStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDuration)
    {
		m_refParameter.duration = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmSsTime);
        bValid = true;
    }
	return false;
}

void StationTisStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDuration)
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.duration, eHhMmSsTime);
    }
}
}