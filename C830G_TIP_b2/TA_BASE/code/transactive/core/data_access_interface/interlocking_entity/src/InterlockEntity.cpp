/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/interlocking_entity/src/InterlockEntity.cpp $
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

#include "core/data_access_interface/interlocking_entity/src/InterlockEntity.h"
#include "core/data_access_interface/interlocking_entity/src/InterlockEntityHelper.h"

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    InterlockEntity::InterlockEntity(const std::string& entityType)
    {
        m_interlockEntityHelper = new InterlockEntityHelper(entityType);
    }


    InterlockEntity::~InterlockEntity() 
	{
        delete m_interlockEntityHelper;
        m_interlockEntityHelper=NULL;
    }



    std::string InterlockEntity::getDllName()
    {
        TA_ASSERT(m_interlockEntityHelper != NULL,"The InterlockEntityHelper pointer is null.");
        return m_interlockEntityHelper->getDllName();
    }

} // closes TA_Base_Core
