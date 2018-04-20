#if !defined(Quad_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_)
#define Quad_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/src/Quad.h $
  * @author:  Robert Young
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/25 17:58:20 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/QuadCorbaDef.h"
#include "app/cctv/video_switch_agent/src/VideoInput.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"

#include "core/corba/src/ServantBase.h"
#include "core/data_access_interface/entity_access/src/Quad.h"

namespace TA_IRS_App
{
	class IPDQuad;

    class Quad : public VideoInput,
                 public virtual POA_TA_Base_Bus::QuadCorbaDef,
                 public virtual TA_Base_Core::ServantBase
    {

    public:
        /**
          * Quad
          *
          * Standard constructor.
          *
          * @param      TA_Base_Core::IEntityData& entityData
          *             The configuration data object for this Quad.
          *
		  * @exception	VideoSwitchAgentInternalException
		  *				Thrown if:
          *             1. The entityData cannot be converted to Quad data.
          *             2. The protocol dependent initialisation fails.
          */

        Quad( TA_Base_Core::IEntityDataPtr entityData );

	    virtual ~Quad();

		 /**
          * getQuadState
          *
          * Gets the current state of this Quad.
		  * Inherited from QuadCorbaDef.
          *
          * @return     QuadState
          *             The current state of this quad
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The database cannot be accessed.
          *             2. The data cannot be retrieved.
          *             3. The retrieved data is invalid.
          *             4. The agent is in Monitor mode.
          */
        virtual TA_Base_Bus::QuadCorbaDef::QuadState getQuadState();


        /**
          * areInputsSwitchable
          *
          * Indicates whether the inputs to this quad be switched.
		  * Inherited from QuadCorbaDef.
          *
          * @return     boolean
          *             True if the inputs can be switched,
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the agent is in Monitor mode.
          */
        virtual bool areInputsSwitchable();


        /**
          * requestSwitchOfInput
          *
          * Requests switch of input for one of the Quad segments.
		  * Inherited from QuadCorbaDef.
          *
          * @param      enum EQuadSegment
          *             Display segment corresponding to channel that
          *             is to be switched.
          * @param      unsigned long videoInputEntityKey
          *             The entity key of the VideoInput to switch.
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          * @param      boolean isDemand = false
          *             If false => If the VideoInput is locked (hardware locking), the
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
          *             2. A polite request was made and the VideoInput is switch (hardware) locked.
          *             3. The VideoInput key is not recognised.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          *             6. There is a communications failure with the hardware.
          */
        virtual void requestSwitchOfInput( TA_Base_Bus::QuadCorbaDef::EQuadSegment segment, unsigned long videoInputEntityKey,
            							   const char* sessionId, bool isDemand );


        /**
          * requestSwitchOfInputs
		  *
          * Requests switch of inputs for the four segments.
		  * Inherited from QuadCorbaDef.
          *
          * @param      unsigned long topLeftVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for top-left quad segment.
          * @param      unsigned long topRightVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for top-right quad segment.
          * @param      unsigned long bottomLeftVideoInputEntityKey
          *             The entity key of the VideoInput to switch
          *             to channel for bottom-left quad segment.
          * @param      unsigned long bottomRightVideoInputEntityKey
          *             to channel for bottom-right quad segment.
          * @param      string sessionId
          *             Used to determine if the user has sufficient
          *             privileges for this operation.
          * @param      boolean isDemand
          *             If false => If the VideoInput is locked (hardware locking), the
          *                         switch will not occur. Also known as a polite request.
          *             If true  => The switch will occur even if the hardware is locked.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The VideoInput or the protocol does not support this operation.
          *             2. A polite request was made and the VideoInput is switch (hardware) locked.
          *             3. The VideoInput key is not recognised.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          *             6. There is a communications failure with the hardware.
          */
        virtual void requestSwitchOfInputs( unsigned long topLeftVideoInputEntityKey, unsigned long topRightVideoInputEntityKey,
            								unsigned long bottomLeftVideoInputEntityKey, unsigned long bottomRightVideoInputEntityKey,
            								const char* sessionId, bool isDemand );

		/**
		  * setQuadDescription
		  *
		  * Sets the current description of this Quad.
		  *
		  * Precondition: !isReadOnly()
		  *
		  * @param		string newDescription
		  *				The description to be set for this Quad.
          * @param      string sessionId
          *             Used to determine if the user has sufficient 
          *             privileges for this operation.
		  *
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The switching hardware, or the protocol does not support this operation.
          *             2. A VideoInput key is not recognised.
		  *				3. The agent is in Monitor mode.
		  *				4. The user does not have sufficient privileges for this operation.
		  *				5. There is a communications failure with the hardware.
          */
		
		virtual void setQuadDescription( const char* newDescription, const char* sessionId );
		


        /**
          * updateState
          *
          * Updates the state of the Quad.  
          *
          * This is called when a VideoSwitchAgentStateUpdate message is received, 
          * so the agent should be in Monitor mode. Because this method is called 
          * when a message is received, there is not much we can do if the method 
          * fails, except log. 
          *
          * @param      const TA_Base_Bus::QuadCorbaDef::QuadState& state
          *             The new state of the Quad.
          */
        void updateState( const TA_Base_Bus::QuadCorbaDef::QuadState& state );


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
        Quad( const Quad& theQuad );
        Quad& operator=( const Quad& );


		TA_Base_Core::QuadPtr					m_quadEntityData;
		std::auto_ptr<IPDQuad>			m_pdQuad;
//         UserPrivilegeVerifier           m_userPrivilegeVerifier;
        UserPrivilegeVerifier&           m_userPrivilegeVerifier; // HuangQi++
		unsigned long					m_entityKey;

    }; // Quad

} // TA_IRS_App

#endif // !defined(Quad_5B104870_0963_47b2_A195_B1BCFBC9EE36__INCLUDED_)
