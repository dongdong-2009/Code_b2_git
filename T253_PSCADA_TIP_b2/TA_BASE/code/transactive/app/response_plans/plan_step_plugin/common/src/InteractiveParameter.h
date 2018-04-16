#ifndef INTERACTIVEPARAMETER_INCLUDE
#define INTERACTIVEPARAMETER_INCLUDE

#include <vector>
#include <string>
#include <sstream>
#include "app/response_plans/plan_step_plugin/common/src/StringParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/BoolParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/ListParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/VectorParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/TreeParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/GridParamWrapper.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicCompareRule.h"
#include "app/response_plans/plan_step_plugin/common/src/CompareMethod.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"

namespace TA_Base_App
{

class InteractiveParameter : public IActivityParameter, IBasicEditor
{
public:
    InteractiveParameter( BasicParameter& refBasicParameter );
    virtual ~InteractiveParameter();

public:
    //virtual void defineParameters();
    //virtual void defineAddonParameters() = 0;
    virtual std::string instanceName();
    virtual std::string displayValue( const std::string& strName );
    virtual void getNameValuePairs( std::vector<ItemPair>& vecNameValues );
    virtual IBasicEditor& basicEditor();

    // IBasicEditor interface
    virtual void getBasicNameValuePairs( std::vector<ItemPair>& vecNameValues );
    virtual bool updateBasicBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid );
    virtual void getBasicBooleanValue( const std::string& strName, bool& bValue );
    virtual bool updateBasicValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid );
    virtual void getBasicValue( const std::string& strName, std::string& strValue, size_t& szMaxLength );

protected:
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc,
        const std::string& strTrueLable = "Yes", const std::string& strFalseLable = "No" );

    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc, 
        const CompareMethod eCompare, const bool& refValidTarg, const std::string& strTrueLable = "Yes", const std::string& strFalseLable = "No" );

    template<typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc,
        const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg, const std::string& strTrueLable = "Yes", const std::string& strFalseLable = "No" );

    template<typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc, const CompareMethod eValidComp, const bool& refValidTarg,
        const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg, const std::string& strTrueLable = "Yes", const std::string& strFalseLable = "No" );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc,
        const CompareMethod eCompare, const BasicType& refValidTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc,
        const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
        BasicType& refSrc, const CompareMethod eValidComp, const BasicType& refValidTarg,
        const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc,
        const CompareMethod eCompare, const std::list<BasicType>& refValidTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc,
        const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
        std::list<BasicType>& refSrc, const CompareMethod eValidComp, const std::list<BasicType>& refValidTarg,
        const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc );

    template<typename BasicType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc,
        const CompareMethod eCompare, const std::vector<BasicType>& refValidTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc,
        const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg );

    template<typename BasicType, typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
        std::vector<BasicType>& refSrc, const CompareMethod eValidComp, const std::vector<BasicType>& refValidTarg,
        const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg );

    template<typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, ITreePicker* pPicker,
        const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg );

    template<typename DispType>
    void defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, IGridEditor* pEditor,
        const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg );

	void convertToActualValue( const std::string& strDisplayValue, std::string& refSrc );

    template<typename BasicType>
    void convertToActualValue( const std::string& strDisplayValue, BasicType& refSrc );

    void convertToDisplayValue( const std::string& refSrc, std::string& strDisplayValue );
    void convertToDisplayValue( const bool& refSrc, std::string& strDisplayValue );

    template<typename BasicType>
    void convertToDisplayValue( const BasicType& refSrc, std::string& strDisplayValue );

    //virtual void refreshValueMapping() = 0;

protected:
    virtual bool failureIgnorable() = 0;
    virtual bool responseRequired() = 0;
    virtual bool skippable() = 0;
    virtual bool delayable() = 0;
    virtual time_t minimumTimeout();

private:
    void uninit();

public:
    static std::map<bool, std::string> DefaultBoolMapping;

protected:
    BasicParameter& m_parBasicParameter;

private:
    bool m_bInited;
    unsigned int m_unCurOrder;
    std::vector<IParameterWrapper*> m_vecNameToParameters;
};

template<typename BasicType>
void InteractiveParameter::convertToDisplayValue( const BasicType& refSrc, std::string& strDisplayValue )
{
    std::stringstream ostConvert;

    ostConvert << refSrc;

    ostConvert >> strDisplayValue;
}

template<typename BasicType>
void InteractiveParameter::convertToActualValue( const std::string& strDisplayValue, BasicType& refSrc )
{
    std::istringstream ostConvert( strDisplayValue );

    ostConvert >> refSrc;
}

template<typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc, 
    const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg, const std::string& strTrueLable, const std::string& strFalseLable )
{
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eCompare, refDispTarg ));
    m_vecNameToParameters.push_back( new BoolParamWrapper( strUniqueName, strDisplayName, refSrc, strTrueLable, strFalseLable, ptrDisp ));
}

template<typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, 
    bool& refSrc, const CompareMethod eValidComp, const bool& refValidTarg, const DispType& refDispSrc, const CompareMethod eCompare, 
    const DispType& refDispTarg, const std::string& strTrueLable, const std::string& strFalseLable )
{
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<bool>( refSrc, eValidComp, refValidTarg ));
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eCompare, refDispTarg ));
    m_vecNameToParameters.push_back( new BoolParamWrapper( strUniqueName, strDisplayName, refSrc, strTrueLable, strFalseLable, ptrValid, ptrDisp ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc )
{
    m_vecNameToParameters.push_back( new StringParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc,
    const CompareMethod eCompare, const BasicType& refValidTarg )
{
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<BasicType>( refSrc, eCompare, refValidTarg ));
    m_vecNameToParameters.push_back( new StringParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, NullCompareRule ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, BasicType& refSrc,
    const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg )
{
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<BasicType>( refDispSrc, eCompare, refDispTarg ));
    m_vecNameToParameters.push_back( new StringParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, NullCompareRule, ptrDisp ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
    BasicType& refSrc, const CompareMethod eValidComp, const BasicType& refValidTarg, const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg )
{
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<BasicType>( refSrc, eValidComp, refValidTarg ));
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<BasicType>( refDispSrc, eDispComp, refDispTarg ));
    m_vecNameToParameters.push_back( new StringParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, ptrDisp ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc )
{
    m_vecNameToParameters.push_back( new ListParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc,
    const CompareMethod eCompare, const std::list<BasicType>& refValidTarg )
{
    typedef std::list<BasicType> ListType;
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<ListType>( refSrc, eCompare, refValidTarg ));
    m_vecNameToParameters.push_back( new ListParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, NullCompareRule ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::list<BasicType>& refSrc,
    const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg )
{
    typedef std::list<BasicType> ListType;
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<ListType>( refDispSrc, eCompare, refDispTarg ));
    m_vecNameToParameters.push_back( new ListParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, NullCompareRule, ptrDisp ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
    std::list<BasicType>& refSrc, const CompareMethod eValidComp, const std::list<BasicType>& refValidTarg, const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg )
{
    typedef std::list<BasicType> ListType;
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<ListType>( refSrc, eValidComp, refValidTarg ));
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eDispComp, refDispTarg ));
    m_vecNameToParameters.push_back( new ListParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, ptrDisp ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc )
{
    m_vecNameToParameters.push_back( new VectorParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc ));
}

template<typename BasicType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc,
    const CompareMethod eCompare, const std::vector<BasicType>& refValidTarg )
{
    typedef std::vector<BasicType> VectorType;
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<VectorType>( refSrc, eCompare, refValidTarg ));
    m_vecNameToParameters.push_back( new VectorParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, NullCompareRule ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, std::vector<BasicType>& refSrc,
    const DispType& refDispSrc, const CompareMethod eCompare, const DispType& refDispTarg )
{
    typedef std::vector<BasicType> VectorType;
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<VectorType>( refDispSrc, eCompare, refDispTarg ));
    m_vecNameToParameters.push_back( new VectorParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, NullCompareRule, ptrDisp ));
}

template<typename BasicType, typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName,
    std::vector<BasicType>& refSrc, const CompareMethod eValidComp, const std::vector<BasicType>& refValidTarg, const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg )
{
    typedef std::vector<BasicType> VectorType;
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<VectorType>( refSrc, eValidComp, refValidTarg ));
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eDispComp, refDispTarg ));
    m_vecNameToParameters.push_back( new VectorParamWrapper<BasicType>( nInputType, strUniqueName, strDisplayName, refSrc, ptrValid, ptrDisp ));
}

template<typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, IGridEditor* pEditor, const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg )
{
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eDispComp, refDispTarg ));
    m_vecNameToParameters.push_back( new GridParamWrapper( strUniqueName, strDisplayName, pEditor, ptrDisp ));
}

template<typename DispType>
void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, ITreePicker* pPicker, const DispType& refDispSrc, const CompareMethod eDispComp, const DispType& refDispTarg )
{
    std::auto_ptr<ICompareRule> ptrDisp( new BasicCompareRule<DispType>( refDispSrc, eDispComp, refDispTarg ));
    m_vecNameToParameters.push_back( new TreeParamWrapper( strUniqueName, strDisplayName, pPicker, ptrDisp ));
}

}

#endif
