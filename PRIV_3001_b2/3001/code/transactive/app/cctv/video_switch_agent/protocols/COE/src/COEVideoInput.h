#if !defined(COEVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)
#define COEVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEVideoInput.h $
  * @author:   Rob Young
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles the protocol dependent sections of the operations for a VideoInput
  * using the COE protocol. 
  * Inherits from IPDVideoInput. Before the methods
  * in this interface are called, the user privileges have been checked and
  * that the protocol supports the required features.
  */

#include <string>

#include "app/cctv/video_switch_agent/protocols/src/IPDVideoInput.h"
#include "core/data_access_interface/entity_access/src/VideoInput.h"


namespace TA_IRS_App
{
    class COEVideoInput : public IPDVideoInput
	{
	public:
		/**
		  * COEVideoInput
		  *
		  * Standard constructor.
          *
          * @param      TA_Base_Core::VideoInput& videoInputData
          *             The database object representing the VideoInput.
          */

        COEVideoInput( TA_Base_Core::VideoInputPtr videoInputData );

		/**
		  * ~COEVideoInput
		  *
		  * Standard destructor.
		  */

		virtual ~COEVideoInput() {};

        ///////////////////////////////////////////////////////////////////////
        //
        // State Update Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * notifyVideoInputStateUpdated
          *
          * Sends a message to the agent's clients notifying them that the
          * state has been updated.   
          */

        void notifyVideoInputStateUpdated();

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

        virtual void updateState( const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state );

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
		  */
		
        virtual void lock( const std::string& sessionId );

		/**
		  * unlock
		  *
		  * Unlocks the IPDVideoInput so that the given user session no longer has 
		  *	exclusive control of the IPDVideoInput.
          *
		  * @param		const std::string& sessionId
		  *				Only used to identify the user. Privileges have already
          *             been checked.
		  */	
		
        virtual void unlock( const std::string& sessionId );

		/**
		  * overrideLock
		  *
		  * Overrides any current ownership of the lock for this IPDVideoInput giving
          * the given user the exclusive camera lock.
          *
		  * @param		const std::string& sessionId
		  *				Only used to identify the user. Privileges have already
          *             been checked.
		  */	
		
        virtual void overrideLock( const std::string& sessionId );

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

        virtual std::string getLockOwner();

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

		virtual void verifyUserHoldsLock( const std::string& sessionId );

		/**
		  * isPublicViewingAllowed
		  *
		  * Identifies if this COEVideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @return		bool
		  *				True if the COEVideoInput is allowed to be viewed publicly.
		  *				False if the COEVideoInput is not allowed to be viewed publicly
		  *				or if public viewing control is not supported.
		  */

        virtual bool isPublicViewingAllowed();

		/**
		  * setPublicViewingAllowed
		  *
		  * Sets if this COEVideoInput is allowed to be viewed publicly 
		  * (e.g. on the internet).
		  *
		  * @param		bool isPublicViewingAllowed
		  *				True if the COEVideoInput is allowed to be viewed publicly.
		  *				False otherwise.
		  */

        virtual void setPublicViewingAllowed( bool isPublicViewingAllowed );

		/**
		  * isInService
		  *
		  * Identifies if this COEVideoInput is in or out of service.
		  *
		  * @return		bool
		  *				True if the COEVideoInput is in service.
		  *				False if the COEVideoInput is not in service
		  *				or if service status control is not supported.
		  */

        virtual bool isInService();
		
		/**
		  * setInService
		  *
		  * Sets this COEVideoInput in or out of service.
		  *
		  * @param		bool isInService
		  *				True to set the COEVideoInput in service.
		  *				False to set the COEVideoInput out of service.
 		  */
		
        virtual void setInService( bool isInService );

    private:

        //
        // Disable copy constructor and assignment operator.
        //

        COEVideoInput( const COEVideoInput& theCOEVideoInput );
        COEVideoInput& operator=( const COEVideoInput& );
        
        //
        // The configuration data object for this VideoInput.
        //

        TA_Base_Core::VideoInputPtr m_videoInputData;

        //
        // The sessionId of the lock owner for this VideoInput.
        //

        std::string m_lockOwner;
	};

} // namespace TA_IRS_App

#endif // !defined(COEVideoInput_B5DC94EC_53A1_432b_99C7_5F3286E2F499__INCLUDED_)