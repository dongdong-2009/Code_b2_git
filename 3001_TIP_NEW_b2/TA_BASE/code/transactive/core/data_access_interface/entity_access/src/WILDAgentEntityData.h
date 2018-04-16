/* WILDAgentEntityData
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/entity_access/src/WILDAgentEntityData.h $
 * @author:  <Andy Siow>
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This class contains application specific parameters and implements the IEntityData interface
 */

#ifndef WILD_AGENT_ENTITY_DATA
#define WILD_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/IWILDAgentEntityData.h"

namespace TA_Base_Core
{
        class WILDAgentEntityData : public IWILDAgentEntityData, public EntityData
    {

    public:
        WILDAgentEntityData();
        WILDAgentEntityData( unsigned long key );
        virtual ~WILDAgentEntityData();
        void invalidate();
        std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

        virtual unsigned long getCorrelationThresholdTime();
		virtual unsigned int getATSWILDQueueSize();
		virtual long getCorrelationThresholdTimeoutSecs();

		virtual std::string getWILDDetailsToken();
		virtual std::string getAssetName();

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        WILDAgentEntityData( const WILDAgentEntityData& theWILDAgentEntityData) {};            
		WILDAgentEntityData& operator=(const WILDAgentEntityData &) {return *this;};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;

    public:
		static const std::string CORRELATION_THRESHOLD;
		static const std::string ATS_WILD_QUEUE_SIZE;
		static const std::string WILD_DETAILS_TOKEN;
		static const std::string CORRELATION_THRESHOLD_TIMEOUT;
		static const std::string ASSET_NAME;

    };

        
    typedef boost::shared_ptr<WILDAgentEntityData> WILDAgentEntityDataPtr;
}

#endif // WILD_AGENT_ENTITY_DATA
