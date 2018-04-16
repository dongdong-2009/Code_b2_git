#include <sstream>
#include "app/response_plans/plan_step_plugin/tis/train_tis/editor/src/TrainTisStepEditorParameter.h"
#include "app/response_plans/plan_step_plugin/tis/train_tis/common/TrainTisStepParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

TrainTisStepEditorParameter::TrainTisStepEditorParameter(TrainTisStepParameter& refParameter) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_predefinedMsgHelper( TisPredefinedMsgEditorHelper::TRAIN ),
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

TrainTisStepEditorParameter::~TrainTisStepEditorParameter()
{
}

bool TrainTisStepEditorParameter::failureIgnorable()
{
    return true;
}

bool TrainTisStepEditorParameter::skippable()
{
    return true;
}

bool TrainTisStepEditorParameter::delayable()
{
    return true;
}

bool TrainTisStepEditorParameter::responseRequired()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void TrainTisStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamTrainIdValid = false;
	bool ParamTrainGroupValid = false;
	bool ParamMessageValid = false;
	bool ParamPriorityValid = false;
	bool ParamDurationValid = false;

	std::string valueTrainGroup = m_trainGroupHelper.getTrainGroupName(m_refParameter.trainGroup);

	std::string valueMessage =
		m_predefinedMsgHelper.getPredefinedMsgBySectionAndMessageId(m_refParameter.message.librarySection, m_refParameter.message.messageId).description;

	std::string valueDuration = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmTime);

	std::string valuePriority = m_priorityHelper.getPriorityDescription(m_refParameter.priority);

    ItemPair itmNameValue;

	if(0 != m_refParameter.trainId || !valueTrainGroup.empty())
	{
		ParamTrainIdValid = true;
		ParamTrainGroupValid = true;
	}
	ParamMessageValid = !valueMessage.empty();
	ParamPriorityValid = !valuePriority.empty();
	ParamDurationValid = !valueDuration.empty();

    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.trainId, ParamTrainId, ParamTrainId, eList, ParamTrainIdValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTrainGroup, ParamTrainGroup, ParamTrainGroup, eList, ParamTrainGroupValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMessage, ParamMessage, ParamMessage, eList, ParamMessageValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePriority, ParamPriority, ParamPriority, eList, ParamPriorityValid);
    EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueDuration, ParamDuration, ParamDuration, eHhMmTime, ParamDurationValid);
}

std::string TrainTisStepEditorParameter::displayValue( const std::string& strName )
{
	if(strName == ParamPriority)
	{
		std::ostringstream strPriority;
		strPriority << m_refParameter.priority;
		return strPriority.str();
	}

	return "";
}

IListPicker* TrainTisStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

ITimeEditor* TrainTisStepEditorParameter::getTimeEditor(const std::string& strName)
{
    return this;
}

void TrainTisStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamTrainId)
    {
        vecKeyItems = m_trainIdHelper.getTrainIdItems();
        if(m_trainIdHelper.isTrainIdValid(m_refParameter.trainId))
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

bool TrainTisStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
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
		if(vecSelectedKeys.empty())
		{
			return false;
		}
        m_refParameter.trainGroup = vecSelectedKeys[0];
		strDisplayValue = m_trainGroupHelper.getTrainGroupName(vecSelectedKeys[0]);
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
		strDisplayValue = m_predefinedMsgHelper.getPredefinedMsgById(vecSelectedKeys[0]).description;
    }
    else if(strName == ParamPriority)
    {
		if(vecSelectedKeys.empty())
		{
			m_refParameter.priority = 0;
			return false;
		}
        m_refParameter.priority = vecSelectedKeys[0];
		strDisplayValue = m_priorityHelper.getPriorityDescription(vecSelectedKeys[0]);
    }
	return false;
}

bool TrainTisStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDuration)
    {
		m_refParameter.duration = TimeConvertHelper::convertToParamTime(tmValue, eHhMmTime);
		strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.duration, eHhMmTime);
        bValid = true;
    }
	return false;
}

void TrainTisStepEditorParameter::getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamDuration)
    {
		tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.duration, eHhMmTime);
    }
}

}