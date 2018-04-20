/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/AtsTwpEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/AtsTwpEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string AtsTwpEntityData::ENTITY_TYPE = "AtsTwp";

    AtsTwpEntityData::AtsTwpEntityData(unsigned long key)
		: EntityData(key, getType()), m_isValidData(false)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AtsTwpEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "AtsTwpEntityData (constructor)");
    }

    
    AtsTwpEntityData::~AtsTwpEntityData()
    {
    }


    std::string AtsTwpEntityData::getType()
    {
        return getStaticType();
    }


    std::string AtsTwpEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
    
    
    IEntityData* AtsTwpEntityData::clone(unsigned long key)
    {
        return new AtsTwpEntityData(key);        
    }

    
    void AtsTwpEntityData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AtsTwpEntityData::invalidate");

        getHelper()->invalidate();
        m_isValidData = false;

        LOG( SourceInfo, DebugUtil::FunctionExit, "AtsTwpEntityData::invalidate");
    }


} //close namespace TA_Base_Core
