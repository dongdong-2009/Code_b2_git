#include "app/response_plans/plan_step_plugin/popup_mft_message/common/PopupMftMessageStepParameter.h"
#include "PopupMftMessageEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TimeConvertHelper.h"

namespace TA_Base_App
{

	static ListItem MESSAGEBOX_STYLE_ARRAY[] =
	{
		{0, "OK"},
		{1, "OK and Cancel"},//MB_OKCANCEL 0x00000001L
		{2, "Abort, Retry, and Ignore"},//MB_ABORTRETRYIGNORE 0x00000002L
		{3, "Yes, No, and Cancel"},//MB_YESNOCANCEL 0x00000003L
		{4, "Yes and No"},//MB_YESNO 0x00000004L
		{5, "Retry and Cancel"}//MB_RETRYCANCEL 0x00000005L
	};

    PopupMftMessageStepEditorParameter::PopupMftMessageStepEditorParameter(PopupMftMessageStepParameter& refParameter, IPlanNode& parent) : 
    InteractiveParameter( refParameter.basic ),
    m_refParameter( refParameter ),
	m_planStepEditorHelper(parent)
    {
    }

    PopupMftMessageStepEditorParameter::~PopupMftMessageStepEditorParameter()
    {
    }

    bool PopupMftMessageStepEditorParameter::failureIgnorable()
    {
        return true;
    }

    bool PopupMftMessageStepEditorParameter::skippable()
    {
        return true;
    }

    bool PopupMftMessageStepEditorParameter::delayable()
    {
        return true;
    }

    // [ToDo] Design general solution for all steps, using only defineAddonParameters
    void PopupMftMessageStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
    {
		std::string valueMessageBoxType;
		for(size_t i = 0; i < sizeof(MESSAGEBOX_STYLE_ARRAY)/sizeof(ListItem); ++i)
		{
			if(m_refParameter.messageBoxType == MESSAGEBOX_STYLE_ARRAY[i].ID)
			{
				valueMessageBoxType = MESSAGEBOX_STYLE_ARRAY[i].Item;
				break;
			}
		}
		std::string valueTimespan = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);
		std::string valueOperatorProfile = m_operatorProfileHelper.getOperatorNameById(m_refParameter.operatorProfileNumber);
		std::string valueAbortStep = m_planStepEditorHelper.getItemById(m_refParameter.abortStep).Item;
		std::string valueRetryStep = m_planStepEditorHelper.getItemById(m_refParameter.retryStep).Item;
		std::string valueIgnoreStep = m_planStepEditorHelper.getItemById(m_refParameter.ignoreStep).Item;
		std::string valueOkStep = m_planStepEditorHelper.getItemById(m_refParameter.okStep).Item;
		std::string valueCancelStep = m_planStepEditorHelper.getItemById(m_refParameter.cancelStep).Item;
		std::string valueYesStep = m_planStepEditorHelper.getItemById(m_refParameter.yesStep).Item;
		std::string valueNoStep = m_planStepEditorHelper.getItemById(m_refParameter.noStep).Item;
		std::string valuePendingStep = m_planStepEditorHelper.getItemById(m_refParameter.pendingStep).Item;

        ItemPair itmNameValue;

		bool ParamMessageBoxTypeValid = !valueMessageBoxType.empty();
		bool ParamOperatorProfileNumberValid = !valueOperatorProfile.empty();
		bool ParamTimeSpanValid = !valueTimespan.empty();
		bool ParamAbortStepValid = !valueAbortStep.empty();
		bool ParamRetryStepValid = !valueRetryStep.empty();
		bool ParamIgnoreStepValid = !valueIgnoreStep.empty();
		bool ParamOkStepValid = !valueOkStep.empty();
		bool ParamCancelStepValid = !valueCancelStep.empty();
		bool ParamYesStepValid = !valueYesStep.empty();
		bool ParamNoStepValid = !valueNoStep.empty();
		bool ParamPendingStepValid = !valuePendingStep.empty();

		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, m_refParameter.decisionMessage, ParamDecisionMessage, ParamDecisionMessage, eString, true);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueMessageBoxType, ParamMessageBoxType, ParamMessageBoxType, eList, ParamMessageBoxTypeValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueOperatorProfile, ParamOperatorProfileNumber, ParamOperatorProfileNumber, eList, ParamOperatorProfileNumberValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueTimespan, ParamTimeSpan, ParamTimeSpan, eHhMmSsTime, ParamTimeSpanValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueAbortStep, ParamAbortStep, ParamAbortStep, eReadOnly, ParamAbortStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueRetryStep, ParamRetryStep, ParamRetryStep, eReadOnly, ParamRetryStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueIgnoreStep, ParamIgnoreStep, ParamIgnoreStep, eReadOnly, ParamIgnoreStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueOkStep, ParamOkStep, ParamOkStep, eReadOnly, ParamOkStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueCancelStep, ParamCancelStep, ParamCancelStep, eReadOnly, ParamCancelStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueYesStep, ParamYesStep, ParamYesStep, eReadOnly, ParamYesStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valueNoStep, ParamNoStep, ParamNoStep, eReadOnly, ParamNoStepValid);
		EDITOR_PUSH_PARAMETER(vecNameValues, itmNameValue, valuePendingStep, ParamPendingStep, ParamPendingStep, eReadOnly, ParamPendingStepValid);
    }

    IStringEditor* PopupMftMessageStepEditorParameter::getStringEditor(const std::string& strName)
    {
        return this;
    }


    // [ToDo] Design general solution for all steps, using only defineAddonParameters
    bool PopupMftMessageStepEditorParameter::updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow)
    {
		if(strName == ParamDecisionMessage)
        {
			convertToActualValue(strDisplayValue, m_refParameter.decisionMessage);
            bValid = true;
        }
        return false;
    }

	bool PopupMftMessageStepEditorParameter::responseRequired()
	{
		return true;
	}

	time_t PopupMftMessageStepEditorParameter::minimumTimeout()
	{
		return 30u;
	}

	IListPicker* PopupMftMessageStepEditorParameter::getListPicker( const std::string& strName )
	{
		return this;
	}

	ITimeEditor* PopupMftMessageStepEditorParameter::getTimeEditor( const std::string& strName )
	{
		return this;
	}

	void PopupMftMessageStepEditorParameter::getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/ )
	{
		if(strName == ParamOperatorProfileNumber)
		{
			vecKeyItems = m_operatorProfileHelper.getOperatorProfileItems();
			vecSelected.push_back(m_refParameter.operatorProfileNumber);
			nMaxSelection = 1;
		}
		else if(strName == ParamMessageBoxType)
		{
			for(size_t i = 0; i < sizeof(MESSAGEBOX_STYLE_ARRAY)/sizeof(ListItem); ++i)
			{
				vecKeyItems.push_back(MESSAGEBOX_STYLE_ARRAY[i]);
			}
			vecSelected.push_back(m_refParameter.messageBoxType);
			nMaxSelection = 1;
		}
		else
		{
	        vecKeyItems = m_planStepEditorHelper.getSteps();
	        ListItem item;
			if(strName == ParamAbortStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.abortStep);
			}
			else if(strName == ParamRetryStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.retryStep);
			}
			else if(strName == ParamIgnoreStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.ignoreStep);
			}
			else if(strName == ParamOkStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.okStep);
			}
			else if(strName == ParamCancelStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.cancelStep);
			}
			else if(strName == ParamYesStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.yesStep);
			}
			else if(strName == ParamNoStep)
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.noStep);
			}
			else //strName == ParamPendingStep
			{
				item = m_planStepEditorHelper.getItemById(m_refParameter.pendingStep);
			}

	        if(m_planStepEditorHelper.INVALID_ID != item.ID)
	        {
	            vecSelected.push_back(item.ID);
	        }
	        nMaxSelection = 1;
		}
	}

	bool PopupMftMessageStepEditorParameter::pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/ )
	{
		if(vecSelectedKeys.empty())
		{
			return false;
		}
		if(strName == ParamOperatorProfileNumber)
		{
			m_refParameter.operatorProfileNumber = vecSelectedKeys[0];
			strDisplayValue = m_operatorProfileHelper.getOperatorNameById(m_refParameter.operatorProfileNumber);
		}
		else if(strName == ParamMessageBoxType)
		{
			m_refParameter.messageBoxType = vecSelectedKeys[0];
			if(m_refParameter.messageBoxType < sizeof(MESSAGEBOX_STYLE_ARRAY)/sizeof(ListItem))
			{
				strDisplayValue = MESSAGEBOX_STYLE_ARRAY[m_refParameter.messageBoxType].Item;
			}
		}
		else
		{
			if(strName == ParamAbortStep)
			{
	            m_refParameter.abortStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.abortStep ).Item;
			}
			else if(strName == ParamRetryStep)
			{
	            m_refParameter.retryStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.retryStep ).Item;
			}
			else if(strName == ParamIgnoreStep)
			{
	            m_refParameter.ignoreStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.ignoreStep ).Item;
			}
			else if(strName == ParamOkStep)
			{
	            m_refParameter.okStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.okStep ).Item;
			}
			else if(strName == ParamCancelStep)
			{
	            m_refParameter.cancelStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.cancelStep ).Item;
			}
			else if(strName == ParamYesStep)
			{
	            m_refParameter.yesStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.yesStep ).Item;
			}
			else if(strName == ParamNoStep)
			{
	            m_refParameter.noStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.noStep ).Item;
			}
			else //strName == ParamPendingStep
			{
	            m_refParameter.pendingStep = vecSelectedKeys.front();
	            strDisplayValue = m_planStepEditorHelper.getItemById( m_refParameter.pendingStep ).Item;
			}
		}
		return false;
	}

	bool PopupMftMessageStepEditorParameter::updateTimeValue(const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow /*= 0*/)
	{
		if(strName == ParamTimeSpan)
		{
			m_refParameter.timeSpan = TimeConvertHelper::convertToParamTime(tmValue, eHhMmSsTime);
			strDisplayValue = TimeConvertHelper::converToDisplayValue(m_refParameter.timeSpan, eHhMmSsTime);
			bValid = true;
		}
		return false;
	}

	void PopupMftMessageStepEditorParameter::getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow /*= 0*/ )
	{
		if(strName == ParamTimeSpan)
		{
			tmValue = TimeConvertHelper::convertToRealTime(m_refParameter.timeSpan, eHhMmSsTime);
		}
	}

	void PopupMftMessageStepEditorParameter::getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow /*= 0 */ )
	{
		if(strName == ParamDecisionMessage)
        {
			strCurValue = m_refParameter.decisionMessage;
			szMaxLength = 800;
        }
	}

}
