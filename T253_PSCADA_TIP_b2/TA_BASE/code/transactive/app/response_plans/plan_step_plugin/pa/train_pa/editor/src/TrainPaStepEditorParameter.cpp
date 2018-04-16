#include "app/response_plans/plan_step_plugin/pa/train_pa/common/TrainPaStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/editor/src/TrainPaStepEditorParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainStepParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"
#include "core/data_access_interface/pa/src/IPaTrainDvaMessage.h"
#include "core/data_access_interface/pa/src/PaTisMessageMappingAccessFactory.h"
#include "core/data_access_interface/pa/src/IPaTisMessageMap.h"
#include <boost/lexical_cast.hpp>

namespace TA_Base_App
{
TrainPaStepEditorParameter::TrainPaStepEditorParameter(TrainPaStepParameter& refParameter) :
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_trainIdHelper( 1, 99 )
{
	static ListItem trainGroups[] =
	{
	    {TG_ALL_TRAINS, "All trains"},
	    {TG_INBOUND_TRAINS, "Inbound trains"},
	    {TG_OUTBOUND_TRAINS, "Outbound trains"},
	    {TG_UNDEFINED, "Single train"}
	};
	for(size_t i = 0; i < sizeof(trainGroups)/sizeof(trainGroups[0]); ++i)
	{
		m_trainGroupHelper.insertGroup(trainGroups[i].ID, trainGroups[i].Item);
	}
}

TrainPaStepEditorParameter::~TrainPaStepEditorParameter()
{
}

bool TrainPaStepEditorParameter::failureIgnorable()
{
    return true;
}

bool TrainPaStepEditorParameter::skippable()
{
    return true;
}

bool TrainPaStepEditorParameter::delayable()
{
    return true;
}

bool TrainPaStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void TrainPaStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamTrainIdValid = false;
	bool ParamTrainGroupValid = false;
	bool ParamMessageValid = false;
	bool ParamDurationValid = false;

	std::string valueTrainGroup = m_trainGroupHelper.getTrainGroupName(m_refParameter.trainGroup);

	std::string valueMessage;
	TA_Base_Core::IPaTrainDvaMessage* trainDvaMsg =
		m_trainDvaMsgHelper.getPaTrainDvsMsgByKey(m_refParameter.message.messageId);
	if(NULL != trainDvaMsg)
	{
		valueMessage = trainDvaMsg->getLabel();
	}

	std::string valueDuration = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmTime);

	if(0 != m_refParameter.trainId || !valueTrainGroup.empty())
	{
		ParamTrainIdValid = true;
		ParamTrainGroupValid = true;
	}
	ParamMessageValid = !valueMessage.empty();
	ParamDurationValid = !valueDuration.empty();

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.trainId, ParamTrainId, ParamTrainId, eList, ParamTrainIdValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTrainGroup, ParamTrainGroup, ParamTrainGroup, eList, ParamTrainGroupValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMessage, ParamMessage, ParamMessage, eList, ParamMessageValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.cyclic, ParamCyclic, ParamCyclic, eBoolean, true);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueDuration, ParamDuration, ParamDuration, eHhMmTime, ParamDurationValid);
}

IBoolEditor* TrainPaStepEditorParameter::getBoolEditor(const std::string& strName)
{
    return this;
}

bool TrainPaStepEditorParameter::updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamCyclic))
    {
        m_refParameter.cyclic = bValue;
        convertToDisplayValue(m_refParameter.cyclic, strDisplayValue);
        bValid = true;
    }
    return false;
}

void TrainPaStepEditorParameter::getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow /*= 0 */)
{
    if(0 == strName.compare(ParamCyclic))
    {
        bValue = m_refParameter.cyclic;
    }
}

IListPicker* TrainPaStepEditorParameter::getListPicker(const std::string& strName)
{
	return this;
}

void TrainPaStepEditorParameter::getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/ )
{
	if(strName == ParamTrainId)
	{
		vecKeyItems = m_trainIdHelper.getTrainIdItems();
		if(m_refParameter.trainId > 0 && m_refParameter.trainId < 100)
		{
			vecSelected.push_back(m_refParameter.trainId);
		}
		nMaxSelection = 1;
	}
	else if(strName == ParamTrainGroup)
	{
		vecKeyItems = m_trainGroupHelper.getTrainGroupItems();
		if(m_trainGroupHelper.isTrainGroupValid(m_refParameter.trainGroup))
		{
			vecSelected.push_back(m_refParameter.trainGroup);
		}
		nMaxSelection = 1;
	}
	else if(strName == ParamMessage)
	{
		vecKeyItems = m_trainDvaMsgHelper.getAllPaTrainDvaMsgItems();
		if(NULL != m_trainDvaMsgHelper.getPaTrainDvsMsgByKey(m_refParameter.message.messageId))
		{
			vecSelected.push_back(m_refParameter.message.messageId);
		}
		nMaxSelection = 1;
	}
}

bool TrainPaStepEditorParameter::pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/ )
{
	if(strName == ParamTrainId)
	{
		if(vecSelectedKeys.empty())
		{
            m_refParameter.trainId = 0;
			return false;
		}
		m_refParameter.trainId = vecSelectedKeys[0];
		convertToDisplayValue(m_refParameter.trainId, strDisplayValue);
	}
	else if(strName == ParamTrainGroup)
	{
		m_refParameter.trainGroup = vecSelectedKeys[0];
		strDisplayValue = m_trainGroupHelper.getTrainGroupName(m_refParameter.trainGroup);
	}
	else if(strName == ParamMessage)
	{
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
		strDisplayValue = m_trainDvaMsgHelper.getPaTrainDvsMsgByKey(m_refParameter.message.messageId)->getLabel();
	}
	return false;
}

ITimeEditor* TrainPaStepEditorParameter::getTimeEditor( const std::string& strName )
{
	return this;
}

bool TrainPaStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
	if(strName == ParamDuration)
	{
		m_refParameter.duration = TimeConvertHelper::convertToParamTime(tmValue, eHhMmTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmTime);
		bValid = true;
	}
	return false;
}

void TrainPaStepEditorParameter::getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/ )
{
	if(strName == ParamDuration)
	{
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.duration, eHhMmTime);
	}
}
}