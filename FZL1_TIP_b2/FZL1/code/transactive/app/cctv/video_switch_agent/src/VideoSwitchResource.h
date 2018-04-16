#if !defined(VideoSwitchResource_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)
#define VideoSwitchResource_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/VideoSwitchResource.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles all the common functions of video switch resources.
  *
  * NOTE: When inheriting from this class, if you implement the TA_Base_Bus::IEntity
  * methods, you need to call the TA_Base_Bus::IEntity methods in this class as part
  * of the overriding methods.
  *
  */


#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/VideoSwitchResourceCorbaDef.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "bus/generic_agent/src/IEntity.h"

#include "core/corba/src/ServantBase.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"

// Forward declarations.

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_IRS_App
{
    class VideoSwitchResource : public virtual POA_TA_Base_Bus::VideoSwitchResourceCorbaDef,
        public virtual TA_Base_Core::ServantBase,
        public TA_Base_Bus::IEntity
    {

        public:
            /**
              * VideoSwitchResource
              *
              * Standard constructor.
              *
              * @param      TA_Base_Core::IEntityData& entityData
              *             The configuration data for this VideoSwitchResource.
              *             This object is owned by GenericAgent, so it should not be deleted.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The entityData cannot be converted to VideoSwitchResource data.
              *             2. The protocol dependent initialisation fails.
              */

            VideoSwitchResource(TA_Base_Core::IEntityDataPtr entityData);

            /**
              * ~VideoSwitchResource
              *
              * Standard destructor.
              */

            virtual ~VideoSwitchResource();

            /**
              * getVideoSwitchResourceData
              *
              * Gets the database entity for this VideoSwitchResource.
              *
              * @return     TA_Base_Core::VideoSwitchResource&
              *             The VideoSwitchResource data.
              */

            TA_Base_Core::VideoSwitchResourcePtr& getVideoSwitchResourceData();

            /**
              * getSubsystemKey
              *
              * Gets the key of the subsystem to which this VideoSwitchResource belongs.
              *
              * @return     unsigned long
              *             The unique subsystem key for the VideoSwitchResource.
              *
              * @exception  DatabaseException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              * @exception  DataException
              *             Thrown if:
              *             1. The retrieved data is invalid.
              *             2. The retrieved data cannot be converted to the appropriate type.
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            unsigned long getSubsystemKey();

            /**
              * getLocationKey
              *
              * Gets the key of the location to which this VideoSwitchResource belongs.
              *
              * @return     unsigned long
              *             The unique location key for the VideoSwitchResource.
              *
              * @exception  DatabaseException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              * @exception  DataException
              *             Thrown if:
              *             1. The retrieved data is invalid.
              *             2. The retrieved data cannot be converted to the appropriate type.
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            unsigned long getLocationKey();

            ///////////////////////////////////////////////////////////////////////
            //
            // State Update Methods
            //
            ///////////////////////////////////////////////////////////////////////

            ///////////////////////////////////////////////////////////////////////
            //
            // VideoSwitchResourceCorbaDef Methods
            //
            ///////////////////////////////////////////////////////////////////////

            /**
              * getKey
              *
              * Gets the entity key of the VideoSwitchResource.  This should always be used
              * to identify the VideoSwitchResource, because it is unique and fixed, whereas
              * names are unique, but are allowed to change.
              *
              * @return     unsigned long
              *             The unique entity key of the VideoSwitchResource.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              *             3. The retrieved data is invalid.
              *             4. The agent is in Monitor mode.
              */

            virtual unsigned long getKey();

            /**
              * getName
              *
              * Gets the name of the VideoSwitchResource.
              *
              * @return     char*
              *             The unique name of the VideoSwitchResource.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              *             3. The retrieved data is invalid.
              *             4. The agent is in Monitor mode.
              */

            virtual char* getName();

            /**
              * getType
              *
              * Gets the type of the VideoSwitchResource.
              *
              * @return     char*
              *             The type of the VideoSwitchResource.  e.g. Camera, Sequence, etc.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if the agent is in Monitor mode.
              */

            virtual char*  getType();

            /**
              * getAddress
              *
              * Gets the address of the VideoSwitchResource.
              *
              * @return     char*
              *             The address of the VideoSwitchResource.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              *             3. The retrieved data is invalid.
              *             4. The agent is in Monitor mode.
              */

            virtual char*  getAddress();

            /**
              * getDescription
              *
              * Gets the description of the VideoSwitchResource.
              *
              * @return     char*
              *             The description of the VideoSwitchResource.
              *
              * @exception  VideoSwitchAgentException
              *             Thrown if:
              *             1. The database cannot be accessed.
              *             2. The data cannot be retrieved.
              *             3. The retrieved data is invalid.
              *             4. The agent is in Monitor mode.
              */

            virtual char*  getDescription();


            ///////////////////////////////////////////////////////////////////////
            //
            // Supporting Methods
            //
            ///////////////////////////////////////////////////////////////////////

            ///////////////////////////////////////////////////////////////////////
            //
            // IEntity Methods
            //
            ///////////////////////////////////////////////////////////////////////

            /**
              * isValid
              *
              * This is a pure vitual method that must be implemented by
              * all entity classes.
              *
              * NOTE: This always returns true!
              *
              * @return     bool
              *             True if the entity is valid.
              *             False otherwise.
              */

            virtual bool isValid();

            /**
              * start
              *
              * Creates and activates the SuperimposedTextControl if required.
              *
              * IMPORTANT: When inheriting from this class, if this method
              * is overridden, the overriding method must call this
              * method explicitly.
              */

            virtual void start();

            /**
              * stop
              *
              * Deactivates and deletes the SuperimposedTextControl.
              *
              * IMPORTANT: When inheriting from this class, if this method
              * is overridden, the overriding method must call this
              * method explicitly.
              */

            virtual void stop();

            /**
              * update
              *
              * Called by the agent controlling the entity to ask
              * the entity to update its configuration. Called after
              * an update configuration event is received.
              *
              * IMPORTANT: When inheriting from this class, if this method
              * is overridden, the overriding method must call this
              * method explicitly.
              *
              * @param      const TA_Base_Core::ConfigUpdateDetails& updateEvent
              *             The received event containing the configuration information.
              */

            virtual void update(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

            /**
              * remove
              *
              * When a configuration update event is received to delete an entity
              * from an agent, the agent calls this method to tell the entity
              * to commence any clean up operations. The agent then removes
              * the object from its vector of entities.
              *
              * This method calls stop().
              *
              * IMPORTANT: When inheriting from this class, if this method
              * is overridden, the overriding method must call this
              * method explicitly.
              */

            virtual void remove();

        private:
            //
            // Disable copy constructor and assignment operator.
            //

            VideoSwitchResource(const VideoSwitchResource& theVideoSwitchResource);
            VideoSwitchResource& operator=(const VideoSwitchResource&);


            //
            // The configuration data object for this VideoSwitchResource.
            //

            TA_Base_Core::VideoSwitchResourcePtr m_videoSwitchResourceData;

            //
            // Determines if users have the rights to perform certain actions.
            //

            UserPrivilegeVerifier m_userPrivilegeVerifier;

            //
            // Thread lock used for guarding the lock status of the VideoSwitchResource.
            //

            TA_Base_Core::ReEntrantThreadLockable m_lockOwnerLock;

    };

} // namespace TA_IRS_App

#endif // !defined(VideoSwitchResource_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)