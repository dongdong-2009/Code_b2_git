#ifndef IncidentManagerData_H_INCLUDED
#define IncidentManagerData_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/IncidentManagerData.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4250 4786)
#endif // defined (_MSC_VER)

#include "core/data_access_interface/entity_access/src/EntityData.h"


namespace TA_Base_Core
{
	class IncidentManagerData : public EntityData
    {
    public:

        IncidentManagerData() {}

        IncidentManagerData(unsigned long key) 
            : EntityData( key, getStaticType() )
		{
		}

        static IEntityData* clone(unsigned long key)
        {
            return new IncidentManagerData(key);        
        }

        void invalidate();

        std::string getType();
		static std::string getStaticType();

    /**
     * implement methods as required by the interfaces from
     * which this class inherits
     */
    private:

        IncidentManagerData(IncidentManagerData&);

        IncidentManagerData& operator=(IncidentManagerData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
    };
    
    typedef boost::shared_ptr<IncidentManagerData> IncidentManagerDataPtr;
}

#endif // IncidentManagerData_H_INCLUDED
