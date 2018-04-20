/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockVariable.cpp $
  * @author:  Robert Stagg
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4284)
#endif

#include "core/data_access_interface/interlocking_variable/src/InterlockVariable.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    InterlockVariable::InterlockVariable(std::string entityType)
    {
        m_InterlockVariableHelper = new InterlockVariableHelper(entityType);
    }


    InterlockVariable::~InterlockVariable() 
	{
        delete m_InterlockVariableHelper;
        m_InterlockVariableHelper=NULL;
    }



    std::string InterlockVariable::getDllName()
    {
        TA_ASSERT(m_InterlockVariableHelper != NULL,"The InterlockVariableHelper pointer is null.");
        return m_InterlockVariableHelper->getDllName();
    }

    std::string InterlockVariable::getEntityType()
    {
        TA_ASSERT(m_InterlockVariableHelper != NULL,"The InterlockVariableHelper pointer is null.");
        return m_InterlockVariableHelper->getEntityType();
    }

    std::string InterlockVariable::getButtonText()
    {
        TA_ASSERT(m_InterlockVariableHelper != NULL,"The InterlockVariableHelper pointer is null.");
        return m_InterlockVariableHelper->getButtonText();
    }

} // closes TA_Base_Core
