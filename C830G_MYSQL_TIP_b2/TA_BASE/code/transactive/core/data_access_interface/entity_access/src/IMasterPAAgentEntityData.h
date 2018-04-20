/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/IMasterPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IMasterPAAgentEntityData provides read-only access to MasterPAAgent entity data.
  */

#ifndef IMasterPAAgent_ENTITY_DATA_H
#define IMasterPAAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IPAAgentEntityData.h"

namespace TA_Base_Core
{

    class IMasterPAAgentEntityData : public virtual IPAAgentEntityData
    {
    public:

        // Nothing specialised so far...
    };

} //close namespace TA_Base_Core

#endif

