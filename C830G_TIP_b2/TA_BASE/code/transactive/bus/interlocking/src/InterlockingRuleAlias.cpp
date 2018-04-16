/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/src/InterlockingRuleAlias.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

///////////////////////////////////////////////////////////
//  InterlockingRuleAlias.cpp
//  Implementation of the Class InterlockingAlias
//  Created on:      28-Jan-2005 10:33:30 AM
///////////////////////////////////////////////////////////

#include "core/data_access_interface/interlocking/src/IInterlockAlias.h"
#include "InterlockingRuleAlias.h"

namespace TA_Base_Bus
{

    InterlockingRuleAlias::InterlockingRuleAlias()
        : m_variableName(""),
          m_variableType(""),
          m_aliasName("")
    {
    }

    InterlockingRuleAlias::InterlockingRuleAlias( TA_Base_Core::IInterlockAlias& ialias )
        : m_variableName( ialias.getVariableName() ),
          m_variableType( ialias.getVariableType() ),
          m_aliasName( ialias.getAlias() )
    {

    }



    InterlockingRuleAlias::~InterlockingRuleAlias()
    {

    }



    std::string InterlockingRuleAlias::getVariableName()
    {
        return m_variableName;
    }


    void InterlockingRuleAlias::setVariableName( const std::string& variableName )
    {
        m_variableName = variableName;
    }


    std::string InterlockingRuleAlias::getVariableType()
    {
        return m_variableType;
    }


    void InterlockingRuleAlias::setVariableType( const std::string& variableType )
    {
        m_variableType = variableType;
    }


    std::string InterlockingRuleAlias::getAliasName()
    {
        return m_aliasName;
    }


    void InterlockingRuleAlias::setAliasName( const std::string& aliasName )
    {
        m_aliasName = aliasName;
    }

} // namespace
