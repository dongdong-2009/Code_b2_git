/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ECSAgent/src/CalculatedExpression.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/09/25 14:02:39 $
  * Last modified by:  $Author: huangjian $
  *
  */
///////////////////////////////////////////////////////////
//
//  CalculatedExpression.cpp
//  Implementation of the Class CalculatedExpression
//  Generated by Enterprise Architect
//  Created on:      25-Sep-2003 18:53:34
//  Original author:
//
///////////////////////////////////////////////////////////
//  Modification history:
//
//
///////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include "core/utilities/src/DebugUtil.h"
#include "app/ecs/ECSAgent/src/CalculatedExpression.h"


CalculatedExpression::CalculatedExpression()
: m_isEnabled(false)
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


// ExceptionChecked
CalculatedExpression::~CalculatedExpression()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

void CalculatedExpression::setControl()
{
    m_isEnabled=true;
}

void CalculatedExpression::setMonitor()
{
    m_isEnabled=false;
}
