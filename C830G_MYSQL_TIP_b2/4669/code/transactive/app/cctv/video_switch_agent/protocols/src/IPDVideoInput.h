#if !defined(IPDVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)
#define IPDVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/src/IPDVideoInput.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles all the protocol dependent common functions of VideoInputs.
  * This interface is used by the VideoInput class. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <string>

#include "bus/cctv/video_switch_agent/IDL/src/VideoInputCorbaDef.h"

namespace TA_IRS_App
{
    class IPDVideoInput
	{
	public:
		/**
		  * ~IPDVideoInput
		  *
		  * Standard destructor.
		  */

		virtual ~IPDVideoInput() {};

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * updateState
          *
          * Updates the state of the VideoInput.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state
          *             The new state of the VideoInput.
          */

        virtual void updateState( const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state ) = 0;

		///////////////////////////////////////////////////////////////////////
		//
		// IPDVideoInput Methods
		//
		///////////////////////////////////////////////////////////////////////
        
        //*********************************************************************
        //
        // Note regarding lock ownership.
        //
        // Lock ownership is not persistant.  It will be maintained as part of
        // the state updates from Control to Monitor mode agents, but it will
        // not be maintained if all agents are shutdown.  We don't want to have
        // to figure out who last had the lock on an input if the system has
        // been down for maintenance for a while and it is easy enough for the
        // necessary clients to reobtain the lock if required.
        //
        //*********************************************************************
		
		/**
		  * lock
		  *
		  * Locks the IPDVideoInput so that the given user session has exclusive
		  *	control of the IPDVideoInput.
          *
		  * @param		const std::string& sessionId
		  *				Only used to identify the user. Privileges have already
          *             been checked.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
		  */
		
        virtual void lock( const std::string& sessionId ) = 0;

		/**
		  * unlock
		  *
		  * Unlocks the IPDVideoInput so that the given user session no longer has 
		  *	exclusive control of the IPDVideoInput.
          *
		  * @param		const std::string& sessionId
		  *				Only used to identify the user. Privileges have already
          *             been checked.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
		  */	
		
		virtual void unlock( const std::string& sessionId ) = 0;

		/**
		  * overrideLock
		  *
		  * Overrides any current ownership of the lock for this IPDVideoInput giving
          * the given user the exclusive camera lock.
          *
		  * @param		const std::string& sessionId
		  *				Only used to identify the user. Privileges have already
          *             been checked.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
		  */	
		
		virtual void overrideLock( const std::string& sessionId ) = 0;
        
        /** 
          * getLockOwner
          *
          * Gets the sessionId of the current owner of the exclusive lock
          * for this VideoInput.
          *
		  * @param		std::string
		  *				The sessionId of the lock owner or
          *             "" if the lock is not held by anyone or if locking
          *             is not being used by this VideoInput.
          */

        virtual std::string getLockOwner() = 0;

        /**
		  * verifyUserHoldsLock
		  *
		  * Verifies that the user holds the exclusive lock for this IPDVideoInput.
          * If not, an exception is thrown.
		  *
		  * @param		const std::string& sessionId
		  *				Used to identify the user.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if the user does not hold the lock for this
		  *				IPDVideoInput.  The exception will contain the details of the
		  *				user that holds the lock for this VideoInput.
		  */

		virtual void verifyUserHoldsLock( const std::string& sessionId ) = 0;

		/**
		  * isPublicViewingAllowed
		  *
		  * Identifies if this VideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @return		bool
		  *				True if the VideoInput is allowed to be viewed publicly.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. There is a communications failure with the hardware.
		  */

		virtual bool isPublicViewingAllowed() = 0;

		/**
		  * setPublicViewingAllowed
		  *
		  * Sets if this IPDVideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @param		bool isPublicViewingAllowed
		  *				True if the IPDVideoInput is allowed to be viewed publicly.
		  *				False otherwise.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
		  */

		virtual void setPublicViewingAllowed( bool isPublicViewingAllowed ) = 0;
		
		/**
		  * isInService
		  *
		  * Identifies if this VideoInput is in or out of service.
		  *
		  * @return		bool
		  *				True if the VideoInput is in service.
		  *				False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The database cannot be accessed.
		  *				2. The data cannot be retrieved.
		  *				3. The retrieved data is invalid.
		  *				4. There is a communications failure with the hardware.
		  */

		virtual bool isInService() = 0;
		
		/**
		  * setInService
		  *
		  * Sets this IPDVideoInput in or out of service.
		  *
		  * @param		bool isInService
		  *				True to set the IPDVideoInput in service.
		  *				False to set the IPDVideoInput out of service.
          *
          * @exception  VideoSwitchAgentException
		  *				Thrown if there is a communications failure with the hardware.
 		  */
		
		virtual void setInService( bool isInService ) = 0;
	};

} // namespace TA_IRS_App

#endif // !defined(IPDVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)