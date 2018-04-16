#ifndef ENTITY_CREATION_FACTORY_H
#define ENTITY_CREATION_FACTORY_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/src/EntityCreationFactory.h $
  * @author:   Katherine Thomson 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Creates and stores the appropriate entity from the given entity data.
  */

#include <string>
#include <map>

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/Camera.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"
#include "core/data_access_interface/entity_access/src/Quad.h"
//#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/RecordingUnit.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "bus/scada/datapoint_library/src/DataPoint.h"

// Forward declarations

namespace TA_Base_Core
{
    class IEntityData;
	class ReEntrantThreadLockable;
}

namespace TA_Base_Bus
{
    class IEntity;
}

namespace TA_IRS_App
{
    class SwitchManager;
	class VideoSwitchAgent;
}

namespace TA_IRS_App
{
    class EntityCreationFactory
    {
    public:
        //
        // Define the format for creator functions that can be registered with this class.
        //

        typedef TA_Base_Bus::IEntity* ( *CreatorFunction )( TA_Base_Core::IEntityDataPtr entityData );


		/**
		 * getInstance()
		 *
		 * Singleton accessor method
		 *
		 * @return Singleton instance of EntityCreationFactory
		 *
		 */
		static EntityCreationFactory& getInstance();


		/**
		 * getInstance()
		 *
		 * Singleton cleanup method
		 *
		 *
		 */
		static void shutdown();


		/**
		 * registerAgent
		 *
		 * Registers the Agent for use with addNewChildEntity() calls.
		 *
		 * @param	agent 	pointer to the Agent
		 *
		 */
		void registerAgent( VideoSwitchAgent& agent );


        /**
          * registerFunction
          *
          * Registers the entity creation function to be used when entity data of
          * the given type is received. This method is public to make the agent
          * extendible.  Other classes can register their own functions to be used
          * to create objects that are not one of the base types.
          *
          * Precondition:   There is no function registered for this type.
          *
          * @param  const std::string& type      
          *         The type of entity data handled by the given function.
          * @param  CreatorFunction function
          *         The function to call when creating an entity with data
          *         of the given type.
          */
        void registerFunction( const std::string& type, CreatorFunction function );

        /**
          * createEntity
          *
          * Creates an entity of the appropriate type from the given entity data
          * and adds the entity to the appropriate list in the VideoSwitch class.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * @return TA_Base_Bus::IEntity*
          *         The new entity or 0 if the entity creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the entity. 
          */
        TA_Base_Bus::IEntity* createEntity( TA_Base_Core::IEntityDataPtr entityData );

    private:

        /**
          * EntityCreationFactory
          *
          * Constructor that registers the functions to be used for each entity data type.
          */
        EntityCreationFactory();
    
        /**
          * ~EntityCreationFactory
          *
          * Standard destructor.
          */
        virtual ~EntityCreationFactory() {};


        //
        // Disable copy constructor and assignment operator
        //

        EntityCreationFactory( const EntityCreationFactory& theEntityCreationFactory );
        EntityCreationFactory& operator=( const EntityCreationFactory& theEntityCreationFactory );

		static TA_Base_Bus::DataPoint* createDataPoint(TA_Base_Core::IEntityDataPtr entityData);
        /**
          * createCamera
          * 
          * Creates a camera object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::Camera::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity* 
          *         The new camera or 0 if the camera creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the camera. 
          */
        static TA_Base_Bus::IEntity* createCamera( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * createVideoMonitor
          * 
          * Creates a VideoMonitor object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::VideoMonitor::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity* 
          *         The new VideoMonitor or 0 if the VideoMonitor creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the VideoMonitor. 
          */
        static TA_Base_Bus::IEntity* createVideoMonitor( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * createVideoOutputGroup
          * 
          * Creates a createVideoOutputGroup object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::VideoOutputGroup::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity* 
          *         The new VideoOutputGroup or 0 if the VideoOutputGroup creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the VideoOutputGroup. 
          */
        static TA_Base_Bus::IEntity* createVideoOutputGroup( TA_Base_Core::IEntityDataPtr entityData );


        /**
          * createQuad
          *
          * Creates a Quad object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::Quad::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity*
          *         The new Quad or 0 if the Quad creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the Quad.
          */
        static TA_Base_Bus::IEntity* createQuad( TA_Base_Core::IEntityDataPtr entityData );


        /**
          * createBVSStage
          *
          * Creates a BVSStage object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::BVSStage::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity*
          *         The new BVSStage or 0 if the BVSStage creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the BVSStage.
          */
    //    static TA_Base_Bus::IEntity* createBVSStage( TA_Base_Core::IEntityDataPtr entityData );


        /**
          * createSequence
          *
          * Creates a Sequence object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::Sequence::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity*
          *         The new Sequence or 0 if the Sequence creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the Sequence.
          */
        static TA_Base_Bus::IEntity* createSequence( TA_Base_Core::IEntityDataPtr entityData );


        /**
          * createRecordingUnit
          *
          * Creates a RecordingUnit object from the entity data.
          *
          * It is the *CALLERS* responsibility to clean up the returned pointer.
          *
          * Precondition: TA_Base_Core::RecordingUnit::getStaticType() == entityData.getType()
          *
          * @return TA_Base_Bus::IEntity*
          *         The new RecordingUnit or 0 if the RecordingUnit creation fails.
          *
          * @param  TA_Base_Core::IEntityData& entityData
          *         The data to use to create the RecordingUnit.
          */
        static TA_Base_Bus::IEntity* createRecordingUnit( TA_Base_Core::IEntityDataPtr entityData );


        //
        // The map of entity data types to the function that handles that type.
        //

        typedef std::map< std::string, CreatorFunction > FunctionMap;
        FunctionMap m_functions;

		static EntityCreationFactory*	s_singleton;
		static TA_Base_Core::ReEntrantThreadLockable	s_singletonLock;


    }; // EntityCreationFactory

} // TA_IRS_App

#endif // ENTITY_CREATION_FACTORY_H
