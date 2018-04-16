#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"

namespace TA_Base_App
{
    class PlanSetTimerStepParameter;
    class PlanSetTimerStepEditorParameter : public InteractiveParameter, public ITimeEditor
    {
    public:
        PlanSetTimerStepEditorParameter(PlanSetTimerStepParameter& refParameter);
        ~PlanSetTimerStepEditorParameter();

        // InteractiveParameter
        //virtual void defineAddonParameters();
        virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
		virtual ITimeEditor* getTimeEditor(const std::string& strName);
		// ITimeEditor
	    virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
	    virtual void getTimeValue( const std::string& strName, time_t& tmValue, const unsigned int nRow = 0 );
    protected:
        virtual bool failureIgnorable();
        virtual bool responseRequired();
        virtual bool skippable();
        virtual bool delayable();

    private:
        PlanSetTimerStepParameter& m_refParameter;
    };

}