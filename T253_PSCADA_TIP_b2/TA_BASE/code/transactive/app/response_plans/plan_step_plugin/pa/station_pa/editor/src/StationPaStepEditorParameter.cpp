#include "app/response_plans/plan_step_plugin/pa/station_pa/editor/src/StationPaStepEditorParameter.h"
#include "app/response_plans/plan_step_plugin/pa/station_pa/common/StationPaStepParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"
#include "core/data_access_interface/pa/src/IPaZone.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/pa/src/IPaDvaMessage.h"
#include "core/data_access_interface/pa/src/IPaTisMessageMap.h"
#include "core/data_access_interface/pa/src/PaTisMessageMappingAccessFactory.h"
#include "core/data_access_interface/pa/src/PaStationAccessFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{
//////////////////////////////////////////////////////////////////////////
//  StationPaStepEditorParameter
//////////////////////////////////////////////////////////////////////////
StationPaStepEditorParameter::StationPaStepEditorParameter(StationPaStepParameter& refParameter) :
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter )
{
}

StationPaStepEditorParameter::~StationPaStepEditorParameter()
{
}

bool StationPaStepEditorParameter::failureIgnorable()
{
    return true;
}

bool StationPaStepEditorParameter::skippable()
{
    return true;
}

bool StationPaStepEditorParameter::delayable()
{
    return true;
}

bool StationPaStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void StationPaStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	std::string valueZones;
	BOOST_FOREACH(const unsigned long& i, m_refParameter.zones)
	{
		TA_Base_Core::IPaZone* zone = m_zoneEditorHelper.getPaZoneByKey(i);
		if(NULL != zone)
		{
			valueZones += (zone->getLabel() + ",");
		}
	}
	if(!valueZones.empty())
	{
		valueZones.resize(valueZones.size() - 1);
	}

	TA_Base_Core::IPaDvaMessage* msg =  m_dvaMsgEditorHelper.getPaDvsMsgByKey(m_refParameter.message.messageId);
	std::string valueMessage;
	if(NULL != msg)
	{
		valueMessage = msg->getLabel();
	}

	std::string valueDuration = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmSsTime);
	std::string valueInterval = TimeConvertHelper::converToDisplayValue(m_refParameter.interval, eSsTime);

    ItemPair itmNameValue;

	bool ParamStationValid = false;
	bool ParamZonesValid = false;
	bool ParamMessageValid = false;
	bool ParamDurationValid = false;
	bool ParamIntervalValid = false;

	if(m_refParameter.allStations)
	{
		ParamStationValid = true;
		ParamZonesValid = true;
	}
	else
	{
		ParamStationValid = !m_refParameter.station.empty();
		ParamZonesValid = !m_refParameter.zones.empty();
	}

	ParamMessageValid = !valueMessage.empty();
	ParamDurationValid = !valueDuration.empty();
	ParamIntervalValid = !valueInterval.empty();

    //EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.locationKey, ParamLocationKey, ParamLocationKey, eUnsignedInteger);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.station, ParamStation, ParamStation, eList, ParamStationValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.allStations, ParamAllStations, ParamAllStations, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueZones, ParamZones, ParamZones, eList, ParamZonesValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMessage, ParamMessage, ParamMessage, eList, ParamMessageValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.chime, ParamChime, ParamChime, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.cyclic, ParamCyclic, ParamCyclic, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.TISSynch, ParamTISSynch, ParamTISSynch, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueDuration, ParamDuration, ParamDuration, eHhMmSsTime, ParamDurationValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueInterval, ParamInterval, ParamInterval, eSsTime, ParamIntervalValid);
}

std::string StationPaStepEditorParameter::displayValue(const std::string& strName)
{
    if (strName == ParamZones)
    {
		std::string valueZones;
		BOOST_FOREACH(const unsigned long& i, m_refParameter.zones)
		{
			TA_Base_Core::IPaZone* zone = m_zoneEditorHelper.getPaZoneByKey(i);
			if(NULL != zone)
			{
				valueZones += (zone->getLabel() + ",");
			}
		}
		if(!valueZones.empty())
		{
			valueZones.resize(valueZones.size() - 1);
		}

		return valueZones;
    }

	return "";
}

IBoolEditor* StationPaStepEditorParameter::getBoolEditor(const std::string& strName)
{
    return this;
}

IListPicker* StationPaStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

bool StationPaStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamAllStations))
    {
        m_refParameter.allStations = bValue;
        convertToDisplayValue(m_refParameter.allStations, strDisplayValue);
        bValid = true;
    }
    else if(0 == strName.compare(ParamChime))
    {
        m_refParameter.chime = bValue;
        convertToDisplayValue(m_refParameter.chime, strDisplayValue);
        bValid = true;
    }
    else if(0 == strName.compare(ParamCyclic))
    {
        m_refParameter.cyclic = bValue;
        convertToDisplayValue(m_refParameter.cyclic, strDisplayValue);
        bValid = true;
    }
    else if(0 == strName.compare(ParamTISSynch))
    {
        m_refParameter.TISSynch = bValue;
        convertToDisplayValue(m_refParameter.TISSynch, strDisplayValue);
        bValid = true;
    }
    return false;
}

void StationPaStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamAllStations))
    {
        bValue = m_refParameter.allStations;
    }
    else if(0 == strName.compare(ParamChime))
    {
        bValue = m_refParameter.chime;
    }
    else if(0 == strName.compare(ParamCyclic))
    {
        bValue = m_refParameter.cyclic;
    }
    else if(0 == strName.compare(ParamTISSynch))
    {
        bValue = m_refParameter.TISSynch;
    }
}

void StationPaStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamStation)
    {
		TA_Base_Core::IPaStations paStations;
		try
		{
			paStations = TA_Base_Core::PaStationAccessFactory::getInstance().getAllPaStations();
		}
		catch(...)
		{
			// TODO
		}

		std::vector<ListItem> items;
		BOOST_FOREACH(TA_Base_Core::IPaStation* i, paStations)
		{
			TA_Base_Core::ILocation* location;
			location = m_locationEditorHelper.getLocationByKey(i->getLocationKey());
			if(NULL == location)
			{
				continue;
			}
	        ListItem tmp = {location->getKey(), location->getDisplayName()};
			vecKeyItems.push_back(tmp);
			if(vecSelected.empty() && m_refParameter.locationKey == tmp.ID)
			{
				vecSelected.push_back(tmp.ID);
			}
		}

        nMaxSelection = 1;
    }
    else if(strName == ParamZones)
    {
		vecKeyItems = m_zoneEditorHelper.getPaZoneItemsByLocation(m_refParameter.locationKey);
        BOOST_FOREACH(unsigned long i, m_refParameter.zones)
        {
            vecSelected.push_back(i);
        }
        nMaxSelection = vecKeyItems.size();
    }
    else if(strName == ParamMessage)
    {
		vecKeyItems = m_dvaMsgEditorHelper.getPaDvaMsgItemsByLocation(m_refParameter.locationKey);
		if(NULL != m_dvaMsgEditorHelper.getPaDvsMsgByKey(m_refParameter.message.messageId))
        {
            vecSelected.push_back(m_refParameter.message.messageId);
        }
        nMaxSelection = 1;
    }
}

bool StationPaStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0 */)
{
    if(strName == ParamStation)
    {
        if(vecSelectedKeys.empty())
        {
			m_refParameter.station = "";
            return false;
        }
        m_refParameter.locationKey = vecSelectedKeys[0];
		TA_Base_Core::ILocation* location = m_locationEditorHelper.getLocationByKey(m_refParameter.locationKey);
		if(NULL == location)
		{
			return false;
		}
		m_refParameter.station = location->getDisplayName();
        strDisplayValue = m_refParameter.station;
    }
    else if(strName == ParamZones)
    {
        m_refParameter.zones.clear();
        strDisplayValue = "";
        for(size_t i = 0; i < vecSelectedKeys.size(); ++i)
        {
            m_refParameter.zones.push_back(vecSelectedKeys[i]);
			TA_Base_Core::IPaZone* zone = m_zoneEditorHelper.getPaZoneByKey(vecSelectedKeys[i]);
			if(NULL == zone)
			{
				continue;
			}
			strDisplayValue += (zone->getLabel() + ",");
        }
		// Remove last ","
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
        m_refParameter.message.messageId = vecSelectedKeys[0];
        try
        {
            std::auto_ptr<TA_Base_Core::IPaTisMessageMap> tmp(TA_Base_Core::PaTisMessageMappingAccessFactory::getInstance().getPaTisMessageMappingByMessageId(vecSelectedKeys[0]));
            m_refParameter.message.libVersion = tmp->getTisLibraryVersion();
            m_refParameter.message.libSection = tmp->getTisLibrarySection();
        }
        catch(...)
        {
            // TODO
        }
		TA_Base_Core::IPaDvaMessage* dvaMsg = m_dvaMsgEditorHelper.getPaDvsMsgByKey(vecSelectedKeys[0]);
		if(NULL == dvaMsg)
		{
			return false;
		}
        strDisplayValue = dvaMsg->getLabel();
    }
	return false;
}

ITimeEditor* StationPaStepEditorParameter::getTimeEditor( const std::string& strName )
{
	return this;
}

bool StationPaStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
	if(strName == ParamDuration)
	{
		m_refParameter.duration = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmSsTime);
		bValid = true;
	}
	else if(strName == ParamInterval)
	{
		unsigned long ulInterval = TimeConvertHelper::convertToParamTime(tmValue, eSsTime);
		if(0 <= ulInterval && 10 >= ulInterval)
		{
		    m_refParameter.interval = TimeConvertHelper::convertToParamTime(tmValue, eSsTime);
			strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.interval, eSsTime);
			bValid = true;
		}
		else
		{
			strDisplayValue = TimeConvertHelper::converToDisplayValue(ulInterval, eSsTime);
			bValid = false;
		}
	}
	return false;
}

void StationPaStepEditorParameter::getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/ )
{
	if(strName == ParamDuration)
	{
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.duration, eHhMmSsTime);
	}
	else if(strName == ParamInterval)
	{
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.interval, eSsTime);
	}
}
}