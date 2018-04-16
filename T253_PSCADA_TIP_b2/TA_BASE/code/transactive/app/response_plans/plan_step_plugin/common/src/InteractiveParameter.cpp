#include "app/response_plans/plan_step_plugin/common/src/InteractiveParameter.h"

namespace TA_Base_App
{

InteractiveParameter::InteractiveParameter( BasicParameter& refBasicParameter ) :
m_parBasicParameter( refBasicParameter ),
m_unCurOrder( 0u ),
m_vecNameToParameters()
{
}

InteractiveParameter::~InteractiveParameter()
{
    uninit();
}

void InteractiveParameter::uninit()
{
    for ( std::vector<IParameterWrapper*>::iterator itLoop = m_vecNameToParameters.begin(); m_vecNameToParameters.end() != itLoop; ++itLoop )
    {
        delete *itLoop;
        *itLoop = NULL;
    }

    m_vecNameToParameters.clear();
}

//void InteractiveParameter::defineParameters()
//{
//    defineParameter( eString, ParamName, ParamName, m_parBasicParameter.Name );
//    defineParameter( eText, ParamDesc, ParamDesc, m_parBasicParameter.Description );
//
//    if ( delayable() )
//    {
//        defineParameter( eMmSsTime, "Delay", "Delay", m_parBasicParameter.Delay );
//    }
//
//    if ( skippable() )
//    {
//        defineParameter( eBoolean, ParamSkip, ParamSkip, m_parBasicParameter.Skip );
//    }
//
//    if ( failureIgnorable() )
//    {
//        defineParameter( eBoolean, "Ignore Failure", "Ignore Failure", m_parBasicParameter.IgnoreFailure );
//    }
//
//    defineAddonParameters();
//}

void InteractiveParameter::convertToDisplayValue( const bool& refSrc, std::string& strDisplayValue )
{
    strDisplayValue = refSrc ? "Yes" : "No";
}

void InteractiveParameter::convertToDisplayValue( const std::string& refSrc, std::string& strDisplayValue )
{
    strDisplayValue = refSrc.c_str();
}

std::string InteractiveParameter::instanceName()
{
    return m_parBasicParameter.name;
}

void InteractiveParameter::getBasicNameValuePairs( std::vector<ItemPair>& vecNameValues )
{
    ItemPair itmNameValue;

    // Insert empty struct object (Step) first
    vecNameValues.push_back( itmNameValue );

    // Update struct copied object (Step) in vector
    vecNameValues.back().DisplayName = ParamStep;
    vecNameValues.back().Name = ParamStep;
    vecNameValues.back().Type = eReadOnly;
    vecNameValues.back().Valid = ( 0 < m_parBasicParameter.position );
    convertToDisplayValue( m_parBasicParameter.position, vecNameValues.back().Item );

    // Insert empty struct object (Name) first
    vecNameValues.push_back( itmNameValue );

    // Update struct copied object (Name) in vector
    vecNameValues.back().DisplayName = ParamName;
    vecNameValues.back().Name = ParamName;
    vecNameValues.back().Type = eString;
    vecNameValues.back().Valid = !m_parBasicParameter.name.empty();
    convertToDisplayValue( m_parBasicParameter.name, vecNameValues.back().Item );

    // Insert empty struct object (Description) first
    vecNameValues.push_back( itmNameValue );

    // Update struct copied object (Description) in vector
    vecNameValues.back().DisplayName = ParamDesc;
    vecNameValues.back().Name = ParamDesc;
    vecNameValues.back().Type = eText;
    vecNameValues.back().Valid = true;
    convertToDisplayValue( m_parBasicParameter.description, vecNameValues.back().Item );

    if ( skippable() )
    {
        // Insert empty struct object (Skip) first
        vecNameValues.push_back( itmNameValue );

        // Update copied struct object (Skip) in vector
        vecNameValues.back().DisplayName = ParamSkip;
        vecNameValues.back().Name = ParamSkip;
        vecNameValues.back().Type = eBoolean;
        vecNameValues.back().Valid = true;
        convertToDisplayValue( m_parBasicParameter.skip, vecNameValues.back().Item );
    }

    if ( failureIgnorable() )
    {
        // Insert empty struct object (IgnoreFailure) first
        vecNameValues.push_back( itmNameValue );

        // Update copied struct object (IgnoreFailure) in vector
        vecNameValues.back().DisplayName = ParamIgnoreFailure;
        vecNameValues.back().Name = ParamIgnoreFailure;
        vecNameValues.back().Type = eBoolean;
        vecNameValues.back().Valid = true;
        convertToDisplayValue( m_parBasicParameter.ignoreFailure, vecNameValues.back().Item );
    }

    if ( delayable() )
    {
        // Insert empty struct object (Skip) first
        vecNameValues.push_back( itmNameValue );

        // Update copied struct object (Skip) in vector
        vecNameValues.back().DisplayName = ParamDelay;
        vecNameValues.back().Name = ParamDelay;
        vecNameValues.back().Type = eInteger;
        vecNameValues.back().Valid = true;
        convertToDisplayValue( m_parBasicParameter.delay, vecNameValues.back().Item );
    }

    if ( responseRequired() )
    {
        // Insert empty struct object (Timeout) first
        vecNameValues.push_back( itmNameValue );

        // Update copied struct object (Timeout) in vector
        vecNameValues.back().DisplayName = ParamTimeout;
        vecNameValues.back().Name = ParamTimeout;
        vecNameValues.back().Type = eInteger;
        vecNameValues.back().Valid = ( minimumTimeout() <= m_parBasicParameter.timeout );
        convertToDisplayValue( m_parBasicParameter.timeout, vecNameValues.back().Item );
    }
}

time_t InteractiveParameter::minimumTimeout()
{
    return 1u;
}

void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc, const std::string& strTrueLable, const std::string& strFalseLable )
{
    m_vecNameToParameters.push_back( new BoolParamWrapper( strUniqueName, strDisplayName, refSrc, strTrueLable, strFalseLable ));
}

void InteractiveParameter::defineParameter( const int nInputType, const std::string& strUniqueName, const std::string& strDisplayName, bool& refSrc, const CompareMethod eCompare, const bool& refValidTarg, const std::string& strTrueLable, const std::string& strFalseLable )
{
    std::auto_ptr<ICompareRule> ptrValid( new BasicCompareRule<bool>( refSrc, eCompare, refValidTarg ));
    m_vecNameToParameters.push_back( new BoolParamWrapper( strUniqueName, strDisplayName, refSrc, strTrueLable, strFalseLable, ptrValid ));
}

void InteractiveParameter::getNameValuePairs( std::vector<ItemPair>& vecNameValues )
{
    for ( std::vector<IParameterWrapper*>::const_iterator itLoop = m_vecNameToParameters.begin(); m_vecNameToParameters.end() != itLoop; ++itLoop )
    {
        IParameterWrapper* pParameter = *itLoop;

        if ( NULL == pParameter )
        {
            continue;
        }

        ItemPair stNewItem;
        vecNameValues.push_back( stNewItem );

        vecNameValues.back().Name = pParameter->uniqueName();
        vecNameValues.back().DisplayName = pParameter->displayName();
        vecNameValues.back().Item = pParameter->displayValue();
        vecNameValues.back().Type = pParameter->type();
        vecNameValues.back().Valid = pParameter->valid();
    }
}

std::string InteractiveParameter::displayValue( const std::string& strName )
{
    return "";
}

IBasicEditor& InteractiveParameter::basicEditor()
{
    return *this;
}

bool InteractiveParameter::updateBasicBooleanValue( const std::string& strName, const bool bValue, std::string& strDisplayValue, bool& bValid )
{
    if ( 0 == strName.compare( ParamSkip ))
    {
        m_parBasicParameter.skip = bValue;
        convertToDisplayValue( bValue, strDisplayValue );
    }
    else if ( 0 == strName.compare( ParamIgnoreFailure ))
    {
        m_parBasicParameter.ignoreFailure = bValue;
        convertToDisplayValue( bValue, strDisplayValue );
    }

    return false;
}

void InteractiveParameter::getBasicBooleanValue( const std::string& strName, bool& bValue )
{
    if ( 0 == strName.compare( ParamSkip ))
    {
        bValue = m_parBasicParameter.skip;
    }
    else if ( 0 == strName.compare( ParamIgnoreFailure ))
    {
        bValue = m_parBasicParameter.ignoreFailure;
    }
}

void InteractiveParameter::getBasicValue( const std::string& strName, std::string& strValue, size_t& szMaxLength )
{
    if ( 0 == strName.compare( ParamDelay ))
    {
        convertToDisplayValue( m_parBasicParameter.delay, strValue );

        szMaxLength = 2;
    }
    else if ( 0 == strName.compare( ParamTimeout ))
    {
        convertToDisplayValue( m_parBasicParameter.timeout, strValue );

        szMaxLength = 4;
    }
    else if ( 0 == strName.compare( ParamName ))
    {
        convertToDisplayValue( m_parBasicParameter.name, strValue );

        szMaxLength = 40;
    }
    else if ( 0 == strName.compare( ParamDesc ))
    {
        convertToDisplayValue( m_parBasicParameter.description, strValue );

        szMaxLength = 800;
    }
}

bool InteractiveParameter::updateBasicValue( const std::string& strName, const std::string& strDisplayValue, bool& bValid )
{
    if ( 0 == strName.compare( ParamDelay ))
    {
        convertToActualValue( strDisplayValue, m_parBasicParameter.delay );

        bValid = true;
    }
    else if ( 0 == strName.compare( ParamTimeout ))
    {
        convertToActualValue( strDisplayValue, m_parBasicParameter.timeout );
        
        bValid = ( minimumTimeout() <= m_parBasicParameter.timeout );
    }
    else if ( 0 == strName.compare( ParamName ))
    {
        convertToActualValue( strDisplayValue, m_parBasicParameter.name );

        bValid = !strDisplayValue.empty();
    }
    else if ( 0 == strName.compare( ParamDesc ))
    {
        convertToActualValue( strDisplayValue, m_parBasicParameter.description );

        bValid = true;
    }

    return false;
}

void InteractiveParameter::convertToActualValue( const std::string& strDisplayValue, std::string& refSrc )
{
	refSrc = strDisplayValue.c_str();
}

}