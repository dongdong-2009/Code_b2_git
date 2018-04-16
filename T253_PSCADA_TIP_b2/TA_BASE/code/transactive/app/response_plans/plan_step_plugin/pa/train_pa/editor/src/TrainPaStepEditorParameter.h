#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/editor/src/PaTrainDvaMsgEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainGroupEditorHelper.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TrainIDEditorHelper.h"

namespace TA_Base_App
{

class TrainPaStepParameter;
class TrainPaStepEditorParameter : public InteractiveParameter, public IBoolEditor, public IListPicker, public ITimeEditor
{
public:
    TrainPaStepEditorParameter(TrainPaStepParameter& refParameter);
    ~TrainPaStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IBoolEditor* getBoolEditor(const std::string& strName);
    virtual IListPicker* getListPicker(const std::string& strName);
	virtual ITimeEditor* getTimeEditor(const std::string& strName);

    // IBoolEditor
    virtual bool updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow = 0);

    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);

    // ITimeEditor
	virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    TrainPaStepParameter& m_refParameter;
	PaTrainDvaMsgEditorHelper m_trainDvaMsgHelper;
	TrainGroupEditorHelper m_trainGroupHelper;
	TrainIDEditorHelper m_trainIdHelper;
};

}
