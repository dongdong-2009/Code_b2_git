#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "FailureSummaryStepEditorHelper.h"

namespace TA_Base_App
{
    class FailureSummaryStepParameter;
    class FailureSummaryStepEditorParameter : public InteractiveParameter, public IListPicker
    {
    public:
        FailureSummaryStepEditorParameter(FailureSummaryStepParameter& refParameter);
        ~FailureSummaryStepEditorParameter();

        virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
		virtual IListPicker* getListPicker(const std::string& strName);
		
		// IListPicker
	    virtual void getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 );
	    virtual bool pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 );
    protected:
        virtual bool failureIgnorable();
        virtual bool responseRequired();
        virtual bool skippable();
        virtual bool delayable();
        virtual time_t minimumTimeout();

    private:
        FailureSummaryStepParameter& m_refParameter;
		FailureSummaryStepEditorHelper m_failureSummaryEditorHelper;
    };

}