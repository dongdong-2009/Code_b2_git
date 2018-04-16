/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/MmsAlarmSubmitterEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Implementataion of  MmsAlarmSubmitterEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MmsAlarmSubmitterEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MmsAlarmSubmitterEntityData::ENTITY_TYPE = "MmsAlarmSubmitter";

     MmsAlarmSubmitterEntityData:: MmsAlarmSubmitterEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {

    }

     MmsAlarmSubmitterEntityData::~MmsAlarmSubmitterEntityData()
    {

    }

    IEntityData*  MmsAlarmSubmitterEntityData::clone(unsigned long key)
    {
        return new  MmsAlarmSubmitterEntityData(key);        
    }


    void  MmsAlarmSubmitterEntityData::invalidate()
    {
		getHelper()->invalidate();
    }

    std::string  MmsAlarmSubmitterEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string  MmsAlarmSubmitterEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
