#pragma once;

#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointEditorHelper.h"

namespace TA_Base_App
{
class DataPointSetStepParameter;
class DataPointSetStepEditorParameter : public InteractiveParameter, public IStringEditor, public ITreePicker, public IListPicker
{
public:
    DataPointSetStepEditorParameter( DataPointSetStepParameter& refParameter );
    ~DataPointSetStepEditorParameter();

    // InteractiveParameter
    //virtual void defineAddonParameters();
    virtual void getNameValuePairs( std::vector<ItemPair>& vecNameValues );
    virtual IStringEditor* getStringEditor( const std::string& strName );
    virtual ITreePicker* getTreePicker( const std::string& strName );
    virtual IListPicker* getListPicker( const std::string& strName );

    // IStringEditor
    virtual bool updateValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid, const unsigned int nRow = 0 );
    virtual void getValue( const std::string& strName, std::string& strCurValue, size_t& szMaxLength, const unsigned int nRow = 0 );

    // ITreePicker
    virtual void getInputTreeItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 );
    virtual void getChildItems( const std::string& strName, std::vector<TreeItem>& vecKeyItems, const unsigned int unNodeId );
    virtual bool pickFromTree( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 );

    // IListPicker
    virtual void getInputListItems( const std::string& strName, std::vector<ListItem>& vecKeyItems, std::vector<unsigned int>& vecSelected, unsigned int& nMaxSelection, const unsigned int nRow = 0 );
    virtual bool pickFromList( const std::string& strName, const std::vector<unsigned int>& vecSelectedKeys, std::string& strDisplayValue, const unsigned int nRow = 0 );

protected:
    virtual bool failureIgnorable();
    virtual bool responseRequired();
    virtual bool skippable();
    virtual bool delayable();

private:
    DataPointSetStepParameter& m_refParameter;
    DataPointEditorHelper m_datapointEditorHelper;
};

}