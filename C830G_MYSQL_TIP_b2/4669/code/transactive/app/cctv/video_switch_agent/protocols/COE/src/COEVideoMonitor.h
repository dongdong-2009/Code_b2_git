#if !defined(PelcoVideoMonitor_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
#define PelcoVideoMonitor_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEVideoMonitor.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Handles all the protocol dependent common functions of VideoMonitors.
  * This interface is used by the VideoMonitor class.
  */

#include <string>

#include "app/cctv/video_switch_agent/protocols/src/IPDVideoMonitor.h"

// Forward declarations

namespace TA_Base_Core
{
    class VideoMonitor;
}


namespace TA_IRS_App
{
    class PelcoVideoMonitor : public IPDVideoMonitor
	{
	public:

        // TODO Comments
   		/**
		  * PelcoVideoMonitor
		  *
		  * Standard constructor.
		  */

        PelcoVideoMonitor( TA_Base_Core::VideoMonitor& videoMonitorData ) {};

		/**
		  * ~PelcoVideoMonitor
		  *
		  * Standard destructor.
		  */

        virtual ~PelcoVideoMonitor() {};

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

        virtual bool isValid() { return true; };

        /**
          * start
          *
          * This is a pure vitual method that must be implemented by 
          * all entity classes. It will start any object specific tasks,
          * such as creating its own thread.
          */

        virtual void start() {};

        /**
          * stop
          *
          * This is a pure vitual method that must be implemented by all
          * entity classes. It will stop any object specific tasks, such 
          * as stopping its own thread. 
          */

        virtual void stop() {};

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

        virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent ) {};

        /**
          * remove
          *
          * When a configuration update event is received to delete an entity 
          * from an agent, the agent can call this method to tell the entity 
          * to commence any clean up opperations.
          * 
          * The agent then still has to remove the object from its vector of entities.
          */

        virtual void remove() {};

        /**
          * setToControlMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to control. This enables the
          * entity to perform tasks such as connecting to any field devices it is
          * responsible for communicating with.
          */

        virtual void setToControlMode() {};

        /**
          * setToMonitorMode
          *
          * This method can be called by the agent responsible for the entity in
          * the event of its mode of operation changing to monitor. This enables the
          * entity to perform tasks such as disconnecting from any field devices it was
          * communicating with.
          */

        virtual void setToMonitorMode() {};

	};

} // namespace TA_IRS_App

#endif // !defined(PelcoVideoMonitor_BDE6E42B_B294_4e78_BE77_3ECED963A0C1__INCLUDED_)
