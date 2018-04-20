/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/IWILDAgentEntityData.h $
  * @author AndyS
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * IWILDAgentEntityData provides read-only access to WILDAgent entity data.
  */

#ifndef IWILDAgent_ENTITY_DATA_H
#define IWILDAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IWILDAgentEntityData : public virtual IEntityData
    {
    public:

        virtual unsigned long getCorrelationThresholdTime() = 0;
        virtual unsigned int getATSWILDQueueSize() = 0;
		virtual long getCorrelationThresholdTimeoutSecs() = 0;
		virtual std::string getWILDDetailsToken() = 0;
		virtual std::string getAssetName() = 0;
    };

} //close namespace TA_Base_Core

#endif

