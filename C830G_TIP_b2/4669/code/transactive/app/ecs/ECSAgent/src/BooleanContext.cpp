/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/BooleanContext.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  BooleanContext.cpp
//  Implementation of the Class BooleanContext
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

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/ecs/ECSAgent/src/BooleanContext.h"



BooleanContext::BooleanContext()
: m_isEnabled(false)
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


// ExceptionChecked
BooleanContext::~BooleanContext()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}

void BooleanContext::setControl()
{
    m_isEnabled=true;
}

void BooleanContext::setMonitor()
{
    m_isEnabled=false;
}
