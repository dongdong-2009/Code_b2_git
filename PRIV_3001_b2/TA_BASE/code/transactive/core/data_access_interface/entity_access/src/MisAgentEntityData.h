/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/entity_access/src/MisAgentEntityData.h $
  * @author C. DeWolfe
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Implement the IEntityData interface.
  */

#ifndef MIS_AGENT_DATA_H_INCLUDED
#define MIS_AGENT_DATA_H_INCLUDED

#include <string>
#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class MisAgentEntityData : public EntityData
    {

    public:


        MisAgentEntityData() {}
        MisAgentEntityData(unsigned long key);

        virtual ~MisAgentEntityData();

    public:
        std::string getAssetName();

        /**
         * getType
         *
         * Returns the type of this entity. This method must be callable when the object
         * is constructed using the default constructor. The entity type string should be
         * stored as a static const variable in the concrete class. The entity type should
         * not be loaded from the database (note that this method defines no exceptions).
         *
         * @return The entity type of this entity as a std::string
         */
        virtual std::string getType();
		static std::string getStaticType();

        /**
         * clone
         *
         * Returns a pointer to a new MisAgentEntityData object that has been initialised with the 
         * specified key.
         *
         * @param key The key with which to construct the MisAgentEntityData object.
         */
        static IEntityData* clone(unsigned long key);
        
        /**
         * invalidate
         *
         * Mark the data contained by this entity as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void invalidate();

	private:

        MisAgentEntityData( const MisAgentEntityData&);            
		MisAgentEntityData& operator=(const MisAgentEntityData&);

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ASSET_NAME;

    };
    
    typedef boost::shared_ptr<MisAgentEntityData> MisAgentEntityDataPtr;
} //end namespace TA_Base_Core

#endif // MIS_AGENT_DATA_H_INCLUDED
