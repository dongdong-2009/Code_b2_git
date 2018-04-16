#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/EmptyStepParameter.h"


namespace TA_Base_App
{
class EmptyEditorParamter : public InteractiveParameter
{
public:
	EmptyEditorParamter(EmptyStepParameter& refParameter) : InteractiveParameter( refParameter.basic ),
    m_refParameter(refParameter),
    m_bResponseRequired( false ),
    m_bFailureIgnorable( true ),
    m_bSkippable( true ),
    m_bDelayable( true )
	{
	}

    // InteractiveParameter
	virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues) {}

    void responseRequired( const bool bValue ) { m_bResponseRequired = bValue; }
    void failureIgnorable( const bool bValue ) { m_bFailureIgnorable = bValue; }
    void skippable( const bool bValue ) { m_bSkippable = bValue; }
    void delayable( const bool bValue ) { m_bDelayable = bValue; }

protected:
    virtual bool responseRequired() { return m_bResponseRequired; }
    virtual bool failureIgnorable() { return m_bFailureIgnorable; }
    virtual bool skippable() { return m_bSkippable; }
    virtual bool delayable() { return m_bDelayable; }

private:
	EmptyStepParameter& m_refParameter;

    bool m_bResponseRequired;
    bool m_bFailureIgnorable;
    bool m_bSkippable;
    bool m_bDelayable;
};
}