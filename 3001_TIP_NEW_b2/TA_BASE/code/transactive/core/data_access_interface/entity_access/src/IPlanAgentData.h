#if !defined IPLAN_AGENT_DATA_HEADER_INCLUDED
#define IPLAN_AGENT_DATA_HEADER_INCLUDED

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/IPlanAgentData.h $
 * @author:  Mahaveer Pareek
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * <description>
 */
#include "core/data_access_interface/entity_access/src/IEntityData.h"
namespace TA_Base_Core
{
    class IPlanAgentData : public IEntityData
    {
    public:
        virtual ~IPlanAgentData() {}
 		virtual std::string getAssetName() = 0;
    };
}

#endif // endif IPLAN_AGENT_DATA_HEADER_INCLUDED
