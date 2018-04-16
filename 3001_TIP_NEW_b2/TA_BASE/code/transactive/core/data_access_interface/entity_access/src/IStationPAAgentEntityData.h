/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/IStationPAAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * IStationPAAgentEntityData provides read-only access to StationPAAgent entity data.
  */

#ifndef IStationPAAgent_ENTITY_DATA_H
#define IStationPAAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IPAAgentEntityData.h"

namespace TA_Base_Core
{

    class IStationPAAgentEntityData : public virtual IPAAgentEntityData
    {
    public:

        // Nothing specialised so far...
    };

} //close namespace TA_Base_Core

#endif


