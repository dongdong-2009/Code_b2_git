/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.cpp $
 * @author:  John Dalin
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * 
 * RadioStateSynchronisationEntityData is an implementation of IEntityData. It proves read-only access to Radio state synchronisation data,
 * and a specific framework for Radio state synchronisation parameters.
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include <iostream>

#include "core/data_access_interface/entity_access/src/RadioStateSynchronisationEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string RadioStateSynchronisationEntityData::ENTITY_TYPE = "RadioStateSynchronisation";
    RadioStateSynchronisationEntityData::RadioStateSynchronisationEntityData(unsigned long key)
		                   : EntityData(key, getType())
    {
    }

    RadioStateSynchronisationEntityData::~RadioStateSynchronisationEntityData()
    {
    }

    std::string RadioStateSynchronisationEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioStateSynchronisationEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    
    IEntityData* RadioStateSynchronisationEntityData::clone(unsigned long key)
    {
        return new RadioStateSynchronisationEntityData(key);        
    }


    void RadioStateSynchronisationEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
