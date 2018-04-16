#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditorHelper.h"

namespace TA_Base_App
{
class DataPointTimeCheckStepParameter;
class IPlanNode;

class DataPointTimeCheckStepEditorParameter : public InteractiveParameter, public IListPicker, public ITreePicker, public ITimeEditor
{
public:
    DataPointTimeCheckStepEditorParameter(DataPointTimeCheckStepParameter& refParameter, IPlanNode& parent);
    ~DataPointTimeCheckStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IListPicker* getListPicker(const std::string& strName);
    virtual ITreePicker* getTreePicker(const std::string& strName);
    virtual ITimeEditor* getTimeEditor(const std::string& strName);
	virtual std::string displayValue( const std::string& strName );

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);
    // ITreePicker
    virtual void getInputTreeItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual void getChildItems(const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId);
    virtual bool pickFromTree(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);
    // ITimeEditor
	virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow = 0);
protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    DataPointTimeCheckStepParameter& m_refParameter;
	DataPointEditorHelper m_datapointEditorHelper;
	PlanStepEditorHelper m_planStepEditorHelper;
};

}