/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/ScadaDatabaseAccessor.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Location information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

// ScadaDatabaseAccessor.cpp: implementation of the ScadaDatabaseAccessor class.
//
//////////////////////////////////////////////////////////////////////

#include "bus/interlocking/DataPointVariableDll/src/stdafx.h"
#include "bus/interlocking/DataPointVariableDll/src/ScadaDatabaseAccessor.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	ScadaDatabaseAccessor::ScadaDatabaseAccessor()
	{

	}

	ScadaDatabaseAccessor::~ScadaDatabaseAccessor()
	{

	}

    std::vector<TA_Base_Core::IEntityData*> ScadaDatabaseAccessor::getChildEntitiesOf(unsigned long entityKey)
	{
        FUNCTION_ENTRY("getChildEntitiesOf");

        std::vector<TA_Base_Core::IEntityData*> entitiesToReturn;

        try
        {
            std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey);

            for (std::vector<TA_Base_Core::IEntityData*>::iterator iter = entitiesRetrieved.begin();
                                                              iter != entitiesRetrieved.end();
                                                              ++iter)
            {
				entitiesToReturn.push_back(*iter);
            }
        }
        catch ( ... )
        {
            // Nothing to do. We just return the empty vector
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve a list of child entities.");
        }
       
        FUNCTION_EXIT;
        return entitiesToReturn;
 	}

    std::vector<TA_Base_Core::IEntityData*> ScadaDatabaseAccessor::getEntitiesOfType(const std::string& entityTypeName)
    {
        FUNCTION_ENTRY("getEntitiesOfType");

        std::vector<TA_Base_Core::IEntityData*> entitiesToReturn;

        try
        {
            entitiesToReturn = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfType(entityTypeName);
        }
        catch ( ... )
        {
            //Nothing to do. We just return the empty vector
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve a list of entities.");
        }

        FUNCTION_EXIT;
        return entitiesToReturn;
    }
    

    TA_Base_Core::IEntityData* ScadaDatabaseAccessor::getEntity(unsigned long entityKey)
    {
        FUNCTION_ENTRY("getEntity");

        FUNCTION_EXIT;
        
		return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityKey);
    }


    TA_Base_Core::IEntityData* ScadaDatabaseAccessor::getEntity(const std::string& entityName)
    {
        FUNCTION_ENTRY("getEntity");

        FUNCTION_EXIT;
        return TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
    }

    /**
     * hasChildren
     *
     * This determines whether the specified entity has children or not.
     * The caller passed in what it has determined so that this method can be lazy if needed
     *
     * @param unsigned long - the key of the entity to check
     * @param bool - This indicates what the entity item object calculated on construction
     */
    bool ScadaDatabaseAccessor::hasChildren(unsigned long entityKey, bool hasChildren)
    {
        // If we already have a record of this entity key stored we can return the cached value
        std::map<unsigned long, bool>::iterator matching = m_childCache.find(entityKey);
        if (matching != m_childCache.end())
        {
            return matching->second;
        }

        // Retrieve all child entities
        std::vector<TA_Base_Core::IEntityData*> entitiesRetrieved = TA_Base_Core::EntityAccessFactory::getInstance().getChildEntitiesOf(entityKey);
        if (entitiesRetrieved.size() > 0)
		{
			m_childCache.insert( std::map<unsigned long, bool>::value_type(entityKey, true) );
			return true;
		}
		else
		{
			return false;
		}
    }

} // end of namespace
