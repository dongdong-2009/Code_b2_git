#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "GuiAppStepEditorHelper.h"

namespace TA_Base_App
{
class LaunchGuiAppStepParameter;
class LaunchGuiAppStepEditorParameter : public InteractiveParameter, public IStringEditor, public IListPicker
{
public:
    LaunchGuiAppStepEditorParameter(LaunchGuiAppStepParameter& refParameter);
    ~LaunchGuiAppStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IStringEditor* getStringEditor(const std::string& strName);
	virtual IListPicker* getListPicker(const std::string& strName);

    // IStringEditor
    virtual bool updateValue(const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();
    virtual time_t minimumTimeout();

private:
    LaunchGuiAppStepParameter& m_refParameter;
	GuiAppStepEditorHelper m_guiAppStepEditorHelper;
};

}