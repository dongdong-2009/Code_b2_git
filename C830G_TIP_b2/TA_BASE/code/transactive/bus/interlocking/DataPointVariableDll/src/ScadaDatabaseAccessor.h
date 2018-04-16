/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/DataPointVariableDll/src/ScadaDatabaseAccessor.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve Scada information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

// ScadaDatabaseAccessor.h: interface for the ScadaDatabaseAccessor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCADADATABASEACCESSOR_H__12B15704_60C5_4D27_9853_EAF6E33B7600__INCLUDED_)
#define AFX_SCADADATABASEACCESSOR_H__12B15704_60C5_4D27_9853_EAF6E33B7600__INCLUDED_

#include <map>
#include <vector>
#include <string>

#include "bus/scada/scada_tree/src/IEntityFactory.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Forward declarations
namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
	class ScadaDatabaseAccessor  : virtual public TA_Base_Bus::IEntityFactory
	{
	
	public:

        /**
         * Constructor
         */
		ScadaDatabaseAccessor();
		
		/**
         * Destructor
         */
		virtual ~ScadaDatabaseAccessor();

        virtual std::vector<TA_Base_Core::IEntityData*> getChildEntitiesOf(unsigned long entityKey);

        virtual std::vector<TA_Base_Core::IEntityData*> getEntitiesOfType(const std::string& entityTypeName);

        virtual TA_Base_Core::IEntityData* getEntity(unsigned long entityKey);

        virtual TA_Base_Core::IEntityData* getEntity(const std::string& entityName);

        /**
         * hasChildren
         *
         * This determines whether the specified entity has children or not.
         * The caller passed in what it has determined so that this method can be lazy if needed
         *
         * @param unsigned long - the key of the entity to check
         * @param bool - This indicates what the entity item object calculated on construction
         */
        virtual bool hasChildren(unsigned long entityKey, bool hasChildren);

    private:
        // This caches lookups determining each entity key and whether it has children
        std::map<unsigned long, bool> m_childCache;

	};
} // end of namespace

#endif // !defined(AFX_SCADADATABASEACCESSOR_H__12B15704_60C5_4D27_9853_EAF6E33B7600__INCLUDED_)
