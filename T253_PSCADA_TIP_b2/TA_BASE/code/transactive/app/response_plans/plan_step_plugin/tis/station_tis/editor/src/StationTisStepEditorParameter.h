#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/common/StationTisStepParameter.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/TisPidEditorHelper.h"
#include "app/response_plans/plan_step_plugin/tis/station_tis/editor/src/TisStationEditorHelper.h"
#include "app/response_plans/plan_step_plugin/tis/editor_common/TisPredefinedMsgEditorHelper.h"
#include "app/response_plans/plan_step_plugin/tis/editor_common/TisPriorityEditorHelper.h"

namespace TA_Base_App
{

class StationTisStepEditorParameter : public InteractiveParameter, public IBoolEditor, public IListPicker, ITimeEditor
{
public:
    StationTisStepEditorParameter(StationTisStepParameter& refParameter);
    ~StationTisStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs(std::vector<ItemPair>& vecNameValues);
    virtual IBoolEditor* getBoolEditor(const std::string& strName);
    virtual IListPicker* getListPicker(const std::string& strName);
    virtual ITimeEditor* getTimeEditor(const std::string& strName);
	virtual std::string displayValue( const std::string& strName );

    // IBoolEditor
    virtual bool updateBooleanValue(const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0);
    virtual void getBooleanValue(const std::string& strName, bool& bValue, const unsigned int nRow = 0);
    // IListPicker
    virtual void getInputListItems(const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0);
    virtual bool pickFromList(const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0);
    // ITimePicker
	virtual bool updateTimeValue( const std::string& strName, const time_t tmValue, std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getTimeValue(const std::string& strName, time_t& tmValue, const unsigned int nRow = 0);

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    StationTisStepParameter& m_refParameter;
	TisPredefinedMsgEditorHelper m_predefinedMsgHelper;
	TisPidEditorHelper m_pidHelper;
	TisStationEditorHelper m_stationHelper;
    TisPriorityEditorHelper m_priorityHelper;
};

}
