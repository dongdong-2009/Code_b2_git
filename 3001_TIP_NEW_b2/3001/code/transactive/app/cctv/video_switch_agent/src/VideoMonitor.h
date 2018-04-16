#if !defined(VideoMonitor_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
#define VideoMonitor_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/VideoMonitor.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/11/05 19:13:17 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Handles the specific functions for a video monitor.
  * Implements the IVideoMonitor interface. 
  * Inherits from VideoOutput.
  *
  * Note: There are no protocol specific actions that can be performed on 
  * a VideoMonitor - all protocol specific actions are performed on the 
  * VideoOutput class that this inherits from.
  */

#include <string>

#include "bus/cctv/video_switch_agent/IDL/src/VideoMonitorCorbaDef.h"
#include "app/cctv/video_switch_agent/src/VideoOutput.h"

#include "core/corba/src/ServantBase.h"
#include "core/naming/src/Naming.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"

namespace TA_IRS_App
{
    class VideoMonitor : public VideoOutput,
                         public virtual POA_TA_Base_Bus::VideoMonitorCorbaDef,
                         public virtual TA_Base_Core::ServantBase
    {
    public:
        /**
          * VideoMonitor
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data for this VideoMonitor.
          *             This object is owned by GenericAgent, so it should not be deleted.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The entityData cannot be converted to 
          *             VideoMonitor or VideoInput data.
          *             2. The protocol dependent initialisation fails.
          */

        VideoMonitor( TA_Base_Core::IEntityDataPtr entityData );

        /**
          * ~VideoMonitor
          *
          * Standard destructor.
          */

        virtual ~VideoMonitor();

        ///////////////////////////////////////////////////////////////////////
        //
        // VideoMonitorCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////
  
        /**
          * getSize
          * 
          * Gets the size of the VideoMonitor.
          *
          * @return     TA_Base_Bus::VideoMonitorCorbaDef::EVideoMonitorSize
          *             The size of the VideoMonitor.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */
        
        virtual TA_Base_Bus::VideoMonitorCorbaDef::EVideoMonitorSize getSize();
       
        /**
          * getConsoleKey
          *
          * Gets the key of the Console to which this VideoMonitor is attached.
          * (0 if this is not a console monitor)
          * 
          * @return     unsigned long
          *             The unique key of the console to which this VideoMonitor
          *             is attached or 0 if it is not a Console VideoMonitor.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */

        unsigned long getConsoleKey();

		unsigned long getDisplayOrder();

		bool isOverallMonitor();
        ///////////////////////////////////////////////////////////////////////
        //
        // IEntity Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * start
          *
          * Activates the VideoMonitor Corba servant.
          *
          * IMPORTANT: Also calls VideoOutput::start().
          */

        virtual void start();

        /**
          * stop
          *
          * Deregisters and deactivates the VideoMonitor Corba servant.
          *
          * IMPORTANT: Also calls VideoOutput::stop().
          */

        virtual void stop();

        /**
          * update
          *
          * Called by the agent controlling the entity to ask 
          * the entity to update its configuration. Called after 
          * an update configuration event is received.
          *
          * IMPORTANT: Also calls VideoOutput::update().
          *
          * @param      const TA_Base_Core::ConfigUpdateDetails& updateEvent
          *             The received event containing the configuration information.
          */

        virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

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
          * IMPORTANT: Also calls VideoOutput::remove().
          */

        virtual void remove();

        /**
          * setToControlMode
          *
          * Called when the agent's mode of operation changes to Control. 
          * Registers the VideoMonitor so it can be resolved
          * by the agent's clients.
          */


        /**
          * setToMonitorMode
          *
          * Called when the agent's mode of operation changes to Monitor. 
          * Deregisters the VideoMonitor so it cannot be resolved
          * by the agent's clients.
          */


		virtual void setToControlMode();

		virtual void setToMonitorMode();

    private:
		void activateTheServant();
        void deactivateTheServant();
        //
        // Strings representing VideoMonitor sizes.
        //
        static const std::string SMALL;
        static const std::string MEDIUM;
        static const std::string LARGE;

        //
        // Disable copy constructor and assignment operator.
        //

        VideoMonitor( const VideoMonitor& theVideoMonitor );
        VideoMonitor& operator=( const VideoMonitor& theVideoMonitor );

        //
        // The configuration data object for this VideoMonitor.
        // The object that this points to is cleaned up by GenericAgent.
        //

        TA_Base_Core::VideoMonitorPtr m_videoMonitorData;

        //
        // The name under which this object is registered as a CORBA servant.
        //

        std::string m_myCorbaName;

        //
        // Entity pkey for this Video Monitor
        //

        unsigned long m_entityKey;

    }; // VideoMonitor

} // TA_IRS_App
    
#endif // !defined(VideoMonitor_6D856C3B_635C_4e91_8C91_072E38E337FD__INCLUDED_)
