#ifndef IPDBVSSTAGE_H
#define IPDBVSSTAGE_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/src/IPDBVSStage.h $
 * @author:  Rob Young
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * Handles the protocol dependent sections of the operations for a BVSStage.
 *
 */

#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"

namespace TA_IRS_App
{
	class IPDBVSStage
	{

	public:

		virtual ~IPDBVSStage() {};


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
        virtual TA_Base_Bus::BVSStageCorbaDef::BVSStageState getBVSStageState() = 0;


		virtual void updateBVSStageState(TA_Base_Bus::BVSStageCorbaDef::BVSStageState state) = 0;
	
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
        virtual void setActiveTrain( CORBA::Octet activeTrainID ) = 0;

		/**
		  * isInAnAlarmStack
		  *
		  * returns true if this bvs stage is resting in any video outputs alarm stack
		  */
		virtual bool isInAnAlarmStack() = 0;


	};
}

#endif // IPDBVSSTAGE_H
