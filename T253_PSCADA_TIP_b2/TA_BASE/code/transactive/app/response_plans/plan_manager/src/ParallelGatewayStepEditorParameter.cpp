#include "app/response_plans/plan_step_plugin/internal/src/ParallelGatewayParameter.h"
#include "ParallelGatewayStepEditorParameter.h"
#include "app/response_plans/common/src/HelperMacro.h"

namespace TA_Base_App
{

ParallelGatewayStepEditorParameter::ParallelGatewayStepEditorParameter(ParallelGatewayParameter& refParameter, IPlanNode& parent) : 
InteractiveParameter( refParameter.basic ),
m_refParameter( refParameter ),
m_planStepEditorHelper( parent )
{
}

ParallelGatewayStepEditorParameter::~ParallelGatewayStepEditorParameter()
{
}

bool ParallelGatewayStepEditorParameter::failureIgnorable()
{
    return false;
}

bool ParallelGatewayStepEditorParameter::skippable()
{
    return false;
}

bool ParallelGatewayStepEditorParameter::delayable()
{
    return false;
}

// [ToDo] Design general solution for all steps, using only defineAddonParameters
void ParallelGatewayStepEditorParameter::getNameValuePairs(std::vector<ItemPair>& vecNameValues)
{
	bool ParamIncomingStepsValid = !m_refParameter.incomingStepIds.empty();
	bool ParamOutgoingStepsValid = !m_refParameter.outgoingStepIds.empty();
    bool ParamPairForkStepValid = ( 0u != m_refParameter.pairForkStep ) || ( m_refParameter.incomingStepIds.size() <= 1u );
    bool ParamPairJoinStepValid = ( 0u != m_refParameter.pairJoinStep ) || ( m_refParameter.outgoingStepIds.size() <= 1u );

	std::string incomingValueStepIds = "";
	for ( std::vector<unsigned int>::iterator itLoop = m_refParameter.incomingStepIds.begin(); m_refParameter.incomingStepIds.end() != itLoop; ++itLoop )
	{
		std::string valueStepId = m_planStepEditorHelper.getItemById( *itLoop ).Item;
        incomingValueStepIds += (valueStepId + "; ");
	}
	// Remove last "; "
    if (!incomingValueStepIds.empty())
	{
        incomingValueStepIds.resize(incomingValueStepIds.size() - 2u);
	}

	std::string outgoingValueStepIds = "";
	for ( std::vector<unsigned int>::iterator itLoop = m_refParameter.outgoingStepIds.begin(); m_refParameter.outgoingStepIds.end() != itLoop; ++itLoop )
	{
		std::string valueStepId = m_planStepEditorHelper.getItemById( *itLoop ).Item;
        outgoingValueStepIds += (valueStepId + "; ");
	}
	// Remove last "; "
    if (!outgoingValueStepIds.empty())
	{
        outgoingValueStepIds.resize(outgoingValueStepIds.size() - 2u);
	}

    std::string valuePairForkStep = ( m_refParameter.incomingStepIds.size() > 1u ) ? m_planStepEditorHelper.getItemById( m_refParameter.pairForkStep ).Item : "N.A.";
    std::string valuePairJoinStep = ( m_refParameter.outgoingStepIds.size() > 1u ) ? m_planStepEditorHelper.getItemById( m_refParameter.pairJoinStep ).Item : "N.A.";

    ItemPair itmNameValue;

    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, incomingValueStepIds, ParamIncomingSteps, ParamIncomingSteps, eReadOnly, ParamIncomingStepsValid );
    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, outgoingValueStepIds, ParamOutgoingSteps, ParamOutgoingSteps, eReadOnly, ParamOutgoingStepsValid );
    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, valuePairForkStep, ParamPairForkStep, ParamPairForkStep, eReadOnly, ParamPairForkStepValid );
    EDITOR_PUSH_PARAMETER( vecNameValues, itmNameValue, valuePairJoinStep, ParamPairJoinStep, ParamPairJoinStep, eReadOnly, ParamPairJoinStepValid );
}

bool ParallelGatewayStepEditorParameter::responseRequired()
{
    return false;
}

IListPicker* ParallelGatewayStepEditorParameter::getListPicker(const std::string& strName)
{
    return this;
}

void ParallelGatewayStepEditorParameter::getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamIncomingSteps)
    {
		vecKeyItems = m_planStepEditorHelper.getSteps();
		BOOST_FOREACH(unsigned long i, m_refParameter.incomingStepIds)
		{
			ListItem item = m_planStepEditorHelper.getItemById(i);
			if(m_planStepEditorHelper.INVALID_ID != item.ID)
			{
				vecSelected.push_back(item.ID);
			}
		}
		nMaxSelection = vecKeyItems.size();
    }
	else if(strName == ParamOutgoingSteps)
	{
		vecKeyItems = m_planStepEditorHelper.getSteps();
		BOOST_FOREACH(unsigned long i, m_refParameter.outgoingStepIds)
		{
			ListItem item = m_planStepEditorHelper.getItemById(i);
			if(m_planStepEditorHelper.INVALID_ID != item.ID)
			{
				vecSelected.push_back(item.ID);
			}
		}
		nMaxSelection = vecKeyItems.size();
	}
}

bool ParallelGatewayStepEditorParameter::pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow /*= 0*/)
{
    if(strName == ParamIncomingSteps)
    {
		m_refParameter.incomingStepIds.clear();
		strDisplayValue = "";
		for(size_t i = 0; i < vecSelectedKeys.size(); ++i)
		{
			m_refParameter.incomingStepIds.push_back(vecSelectedKeys[i]);
			std::string valueStepId = m_planStepEditorHelper.getItemById( vecSelectedKeys[i] ).Item;
            strDisplayValue += (valueStepId + "; ");
		}
		// Remove last "; "
		if(!strDisplayValue.empty())
		{
			strDisplayValue.resize(strDisplayValue.size() - 2u);
		}
    }
	else if(strName == ParamOutgoingSteps)
	{
		m_refParameter.outgoingStepIds.clear();
		strDisplayValue = "";
		for(size_t i = 0; i < vecSelectedKeys.size(); ++i)
		{
			m_refParameter.outgoingStepIds.push_back(vecSelectedKeys[i]);
			std::string valueStepId = m_planStepEditorHelper.getItemById( vecSelectedKeys[i] ).Item;
            strDisplayValue += (valueStepId + "; ");
		}
		// Remove last "; "
		if(!strDisplayValue.empty())
		{
			strDisplayValue.resize(strDisplayValue.size() - 2u);
		}
	}

    return false;
}

}
