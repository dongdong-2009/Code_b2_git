/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/TA_BASE/transactive/core/data_access_interface/entity_access/src/OpcServerAgentEntityData.h $
  * @author Dhanshri Mokashi
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:17:48 $
  * Last modified by: $Author: Dhanshri Mokashi $
  * 
  * Implement the IEntityData interface.
  */

#ifndef OPCSERVER_AGENT_ENTITY_DATA
#define OPCSERVER_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class OpcServerAgentEntityData : public EntityData
    {

    public:
        OpcServerAgentEntityData();
        OpcServerAgentEntityData( unsigned long key );
        virtual ~OpcServerAgentEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);        

    protected:

    private:
        static const std::string ENTITY_TYPE;
    };

    typedef boost::shared_ptr<OpcServerAgentEntityData> OpcServerAgentEntityDataPtr;
}

#endif // DATALOG_AGENT_ENTITY_DATA
