#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"

namespace TA_Base_App
{
struct ExecuteScriptStepParameter;
class ExecuteScriptStepEditorParameter : public InteractiveParameter, public IStringEditor
{
public:
    ExecuteScriptStepEditorParameter( ExecuteScriptStepParameter& refParameter );
    ~ExecuteScriptStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs( std::vector<ItemPair>& vecNameValues );
    virtual IStringEditor* getStringEditor( const std::string& strName );

    // IStringEditor
    virtual bool updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    ExecuteScriptStepParameter& m_refParameter;
};

}