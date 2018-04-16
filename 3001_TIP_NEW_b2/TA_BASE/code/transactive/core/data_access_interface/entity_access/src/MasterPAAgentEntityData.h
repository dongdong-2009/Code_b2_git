/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/MasterPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * MasterPAAgentEntityData implements the interface IMasterPAAgentEntityData to provide read-only access
  * to MasterPAAgent entity data.
  */

#ifndef MasterPAAgent_ENTITY_DATA_H
#define MasterPAAgent_ENTITY_DATA_H

#include "core/data_access_interface/entity_access/src/IMasterPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/AbstractPAAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MasterPAAgentEntityData: public virtual AbstractPAAgentEntityData, public virtual IMasterPAAgentEntityData
    {
	public:

		MasterPAAgentEntityData();
        MasterPAAgentEntityData ( unsigned long key );

        virtual ~MasterPAAgentEntityData();

        //
        // operations required by AbstractPAAgentEntityData
        //

        static IEntityData* clone ( unsigned long key )
        {
            return new MasterPAAgentEntityData ( key );
        }

        /**
         * getType
         *
         * Returns the entity type of this entity (i.e. "Camera").
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();

        static std::string getStaticType();

    private:

        MasterPAAgentEntityData ( const MasterPAAgentEntityData & obj ){};
		MasterPAAgentEntityData & operator= ( const MasterPAAgentEntityData &){};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    };
    
    typedef boost::shared_ptr<MasterPAAgentEntityData> MasterPAAgentEntityDataPtr;
} //close namespace TA_Base_Core

#endif
