#if !defined(BVSStage_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_)
#define BVSStage_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/src/BVSStage.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/BVSStageCorbaDef.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/BVSStage.h"

namespace TA_IRS_App
{
	class IPDBVSStage;

    class BVSStage : public VideoInput,
                 public virtual POA_TA_Base_Bus::BVSStageCorbaDef,
                 public virtual TA_Base_Core::ServantBase
    {

    public:

        /**
          * BVSStage
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this BVSStage.
          *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
          *             1. The entityData cannot be converted to BVSStage data.
          *             2. The protocol dependent initialisation fails.
          */

        BVSStage( TA_Base_Core::IEntityDataPtr entityData );

	    virtual ~BVSStage();

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


		void updateBVSStageState(TA_Base_Bus::BVSStageCorbaDef::BVSStageState state);
		
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
        void setActiveTrain( CORBA::Octet activeTrainID );

		/**
		  * isInAnAlarmStack
		  *
		  * returns true if this bvs stage is resting in any video outputs alarm stack
		  */
		bool isInAnAlarmStack();



		//
		// IEntity functions
		//
		virtual void start();

		virtual void stop();

		virtual void update( const TA_Base_Core::ConfigUpdateDetails& updateEvent );

		virtual void remove();

		virtual void activateServantAtInit();

		virtual void deactivateServantAtClose();

		virtual void setToControlMode(){};

		virtual void setToMonitorMode(){};

    private:

        //
        // Disable copy constructor and assignment operator.
        //
        BVSStage( const BVSStage& theBVSStage );
        BVSStage& operator=( const BVSStage& );


		TA_Base_Core::BVSStagePtr					m_BVSStageEntityData;
		std::auto_ptr<IPDBVSStage>			m_pdBVSStage;
		unsigned long					    m_entityKey;


    }; // BVSStage

} // TA_IRS_App

#endif // !defined(BVSStage_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_)
