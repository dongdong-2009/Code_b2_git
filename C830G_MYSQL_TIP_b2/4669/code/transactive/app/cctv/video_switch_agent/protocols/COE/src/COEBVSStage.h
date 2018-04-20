#ifndef COEBVSSTAGE_H
#define COEBVSSTAGE_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/COE/src/COEBVSStage.h $
  * @author:   Rob Young
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2008/12/17 14:59:54 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the general functions for a BVSStage
  * using the COE protocol.
  * Inherits from the IPDBVSStage class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the BVSStage and
  * that the BVSStage and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/protocols/src/IPDBVSStage.h"

#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "bus/trains/TrainCorbaProxy/src/TrainCctvCorbaProxy.h"

#include "core/data_access_interface/entity_access/src/BVSStage.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

#include <string>

// Forward declarations.

namespace TA_Base_Core
{
    class CommsMessageSender;
}

namespace TA_IRS_App
{
    class COEBVSStage : public IPDBVSStage,
                public TA_IRS_Bus::ITrainCctvUpdateObserver
    {
    public:

        /**
          * COEBVSStage
          *
          * Standard constructor.
          *
          * @param      BVSStage& BVSStageData
          *             The protocol independent BVSStage object.
          *             This object is owned by GenericAgent, so it should not be deleted.
          */

        COEBVSStage( TA_Base_Core::BVSStagePtr BVSStageData );

        /**
          * ~COEBVSStage
          *
          * Standard destructor.
          */

        virtual ~COEBVSStage();

        virtual unsigned long getKey();

        virtual std::string getAddress();

        ///////////////////////////////////////////////////////////////////////
        //
        // IPDBVSStage Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * getBVSStageState
          *
          * Gets the current state of this BVSStage.
          * Inherited from BVSStageCorbaDef.
          *
          * @return     BVSStageState
          *             The current state of this quad
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */
        virtual TA_Base_Bus::BVSStageCorbaDef::BVSStageState getBVSStageState();


		virtual void updateBVSStageState(TA_Base_Bus::BVSStageCorbaDef::BVSStageState state);
	
		/**
		  * requestSwitchOfInputs
		  * Requests switch of inputs for the four segments.
		  *
		  * @param      CORBA::Octet activeTrainID
		  *				The ID of the actively broadcasting train 
          *             in this stage (0 if no currently active train).
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
		  *				2. The agent is in Monitor mode.
		  */
        virtual void setActiveTrain( CORBA::Octet activeTrainID );

		/**
		  * isInAnAlarmStack
		  *
		  * returns true if this bvs stage is resting in any video outputs alarm stack
		  */
		virtual bool isInAnAlarmStack();


		/**
	     * receiveSpecialisedMessage  This is overriden by the client to receive their specialise message 
	     * @param	T	The narrowed Message
	     * @param    message
	     */

		//other public funtions
		void addObserver();
		void removeObserver();
		unsigned long getObserverCount();
		void parkAllStageMonitors();
		
        virtual void processCctvSwitchState( const TA_IRS_Bus::CctvTypes::CctvSwitchState& event );
    private:

        //
        // Disable copy constructor and assignment operator.
        //

        COEBVSStage( const COEBVSStage& theCOEBVSStage );
        COEBVSStage& operator=( const COEBVSStage& );
		TA_Base_Bus::BVSStageCorbaDef::BVSStageState getBvsDetails();
		void notifyStateChange();
        //
        // The configuration data object for this BVSStage.
        //

        TA_Base_Core::BVSStagePtr m_BVSStageData;
        TA_Base_Bus::BVSStageCorbaDef::BVSStageState m_bvsStageState;  // The current state of this BVSStage.
		TA_Base_Core::ReEntrantThreadLockable m_stateLock;
		TA_Base_Core::CommsMessageSender*     m_messageSender;
        TA_IRS_Bus::TrainCctvCorbaProxyPtr m_trainCctvCorbaProxy;
    };

} // namespace TA_IRS_App

#endif // COEBVSSTAGE_H
