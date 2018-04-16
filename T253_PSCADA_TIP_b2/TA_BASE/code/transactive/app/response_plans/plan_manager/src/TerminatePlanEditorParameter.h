#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanNodeEditorHelper.h"

namespace TA_Base_App
{
	class TerminatePlanParameter;
    class TerminatePlanEditorParameter : public InteractiveParameter, public ITreePicker
    {
    public:
        TerminatePlanEditorParameter(TerminatePlanParameter& refParameter);
        ~TerminatePlanEditorParameter();

        // InteractiveParameter
        virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);

	    // ITreePick
	    virtual ITreePicker* getTreePicker(const std::string& strName);
	    virtual void getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
	    virtual void getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId);
	    virtual bool pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

    protected:
        virtual bool failureIgnorable();
        virtual bool skippable();
        virtual bool delayable();
		virtual bool responseRequired();

    private:
		TerminatePlanParameter& m_refParameter;
		PlanNodeEditorHelper m_planNodeEditorHelper;
    };

}
