#if !defined(VideoOutputGroup_F1D08BFD_72A2_4c4a_97BE_4293EE57DCC7__INCLUDED_)
#define VideoOutputGroup_F1D08BFD_72A2_4c4a_97BE_4293EE57DCC7__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/VideoOutputGroup.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * A VideoOutputGroup is an object to group VideoOutputs together,
  * such as a video wall monitor group, a wallboard, all recording devices or all
  * operator workstations.
  */

#include <string>
#include <vector>

#include "bus/cctv/video_switch_agent/IDL/src/VideoOutputGroupCorbaDef.h"

#include "bus/generic_agent/src/IEntity.h"

#include "core/corba/src/ServantBase.h"
#include "core/naming/src/Naming.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/data_access_interface/entity_access/src/VideoOutputGroup.h"

// Forward declarations

namespace TA_Base_Core
{
    class ConfigUpdateDetails;
}

namespace TA_IRS_App
{
    class VideoOutput;
}

namespace TA_IRS_App
{
    class VideoOutputGroup : public virtual POA_TA_Base_Bus::VideoOutputGroupCorbaDef,
                             public virtual TA_Base_Core::ServantBase,
                             public TA_Base_Bus::IEntity
    {
    public:
        /**
          * VideoOutputGroup
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this VideoOutputGroup.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
		  * @exception	InvalidConfigurationData
		  *				Thrown if the entityData is not VideoOutputGroup data.
          */

        VideoOutputGroup( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * ~VideoOutputGroup
          *
          * Standard destructor.
          */

        virtual ~VideoOutputGroup();
        
        ///////////////////////////////////////////////////////////////////////
		//
		// VideoOutputGroupCorbaDef Methods
		//
		///////////////////////////////////////////////////////////////////////

        /**
		  * getKey
		  *
          * Gets the entity key of the VideoOutputGroup.  This should always be used
          * to identify the VideoOutputGroup, because it is unique and fixed, whereas
          * names are unique, but are allowed to change.
          *
		  * @return		unsigned long
		  *				The unique entity key of the VideoOutputGroup.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

		virtual unsigned long getKey();

        /**
          * getName
          *
          * Gets the VideoOutputGroup name.
          *
          * @return     char*
          *             The unique VideoOutputGroup name.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual char* getName();

        /**
          * getGroupType
          *
          * Currently all the VideoOutputs in a group must be of the same type.
          *
          * @return     char*
          *             The type of VideoOutputs in the group.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual char* getGroupType();

        /**
          * getTypeSize
          *
          * Currently all the VideoOutputs in a group must be of the same type and size.
		  * e.g. they must all be small VideoMonitors, or all medium VideoMonitors.
		  * If the type does not have sizes, the default will be small (e.g. for
		  * recording units).  See app/cctv/video_switch_agent/src/EntityTypeConstants.h
		  * for the available sizes.
          *
          * @return     string
          *             The size of the VideoOutputs in the group.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual char* getTypeSize();

        /**
          * getPosition
          *
          * Get the position of the VideoOutputGroup in the VDM overview. 
          * The left most section postion is 1, incrementing by 1 to the right.
          *
          * @return     unsigned short
          *             The position ( 1,2, ... n ).
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual unsigned short getPosition();

        /**
          * getGroupDimensionX
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method gets the width of the matrix.
          *
          * @return     unsigned short
          *             The number of VideoOutputs across the group.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual unsigned short getGroupDimensionX();

        /**
          * getGroupDimensionY
          *
          * The VideoOutputGroup is displayed as a 2D matrix of VideoOutputs.
          * This method gets the height of the matrix.
          *
          * @return     unsigned short
          *             The number of VideoOutputs down the group.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual unsigned short getGroupDimensionY();

        /**
          * getVideoOutputList
          *
          * Gets the configured VideoOutputs contained within this VideoOutputGroup.
          * The VideoOutputs will be in order of display from left to right, top to bottom.
          *
          * @return     TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence*
          *             The VideoOutputs in the VideoOutputGroup.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual TA_Base_Bus::VideoOutputGroupCorbaDef::VideoOutputSequence* getVideoOutputList();
    
        /**
          * isCapableOfLayouts
          *
          * Indicates if this VideoOutputGroup is capable of using layouts to show VideoOutputs,
          * graphics or combinations.  If true the VDM GUI should not include a boarder gap 
          * between monitors in the overview display, because the are a combined output display. 
          *
          * @return     bool
          *             True if the VideoOutputGroup is capable of using layouts.
          *             False otherwise.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          */

        virtual bool isCapableOfLayouts();

        /**
          * getVideoWallboardAgentName
          *
          * Gets the video wallboard agent name to use the layouts
          *
          * @return     string
          *             The unique video wallboard name.
          *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. The agent is in Monitor mode.
          *             5. The VideoOutputGroup is no capable of layouts
          */
        virtual char* getVideoWallboardAgentName();

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
          * @return     bool
          *             True if the entity is valid.
          *             False otherwise.
          */

        virtual bool isValid();

        /**
          * start
          *
          * This is a pure vitual method that must be implemented by 
          * all entity classes. It will start any object specific tasks,
          * such as creating its own thread.
          */

        virtual void start();

        /**
          * stop
          *
          * This is a pure vitual method that must be implemented by all
          * entity classes. It will stop any object specific tasks, such 
          * as stopping its own thread. 
          */

        virtual void stop();

        /**
          * update
          *
          * This method is called by the agent controlling the entity to ask 
          * the entity to update its configuration. Called after an update 
          * configuration event is received.
          *
          * @param      const TA_Base_Core::ConfigUpdateDetails& updateEvent
          *             The received event containing the configuration information.
          */

        virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

        /**
          * remove
          *
          * When a configuration update event is received to delete an entity 
          * from an agent, the agent can call this method to tell the entity 
          * to commence any clean up opperations.
          * 
          * The agent then still has to remove the object from its vector of entities.
          */

        virtual void remove();

        /**
          * setToControlMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to control. This enables the
          * entity to perform tasks such as connecting to any field devices it is
          * responsible for communicating with.
          */

        virtual void activateServantAtInit();

        /**
          * setToMonitorMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to monitor. This enables the
          * entity to perform tasks such as disconnecting from any field devices it was
          * communicating with.
          */

        virtual void deactivateServantAtClose();

		virtual void setToControlMode(){};

		virtual void setToMonitorMode(){};

    private:

        typedef std::vector< VideoOutput* > VideoOutputVector;

        //
        // Disable copy constructor and assignment operator.
        //

        VideoOutputGroup( const VideoOutputGroup& theVideoOutputGroup );
        VideoOutputGroup& operator=( const VideoOutputGroup& theVideoOutputGroup );
               
        /**
          * loadVideoOutputs
          *
          * Loads the VideoOutputs contained in this group from the database.
          *
          * Precondition:   hasInitialised()
          */

        void VideoOutputGroup::loadVideoOutputs();

        //
		// The configuration data object for this VideoOutputGroup.
		//

        TA_Base_Core::VideoOutputGroupPtr m_videoOutputGroupData;

        //
        // The configured VideoOutputs for this group.  
        // The VideoOutputs will be in order of display from left to right, top to bottom.
        // The size of the vector should equal the m_groupDimensionX * m_groupDimensionY.
        // The vector contains pointers to VideoOutput objects that are maintained
        // by the SwitchManager, so they don't have to be cleaned up by this object.
        //

        VideoOutputVector m_videoOutputs;

        //
        // The name under which this object is registered as a CORBA servant.
        //

        std::string m_myCorbaName;

        //
        // Flag indicating if the list of VideoOutputs and their positions has been loaded.
        //

        bool m_isListLoaded;

        //
        // Thread guard for the m_videoOutputs list.
        //

        TA_Base_Core::ReEntrantThreadLockable m_videoOutputsLock;


		//
		// Entity pkey
		//

		unsigned long m_entityKey;

		//chenlei++
		//preload this configuration infomation
		std::map< unsigned short, unsigned long > m_videoOutputsMap;


    }; // VideoOutputGroup

} // TA_IRS_App

#endif // !defined(VideoOutputGroup_F1D08BFD_72A2_4c4a_97BE_4293EE57DCC7__INCLUDED_)
