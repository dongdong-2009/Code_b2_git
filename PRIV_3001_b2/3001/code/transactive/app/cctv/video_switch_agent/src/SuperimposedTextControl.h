#if !defined(SuperimposedTextControl_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_)
#define SuperimposedTextControl_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/src/SuperimposedTextControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Handles all the functions involving superimposing text on a video input.
  * Implements the ISuperimposedTextControl interface. 
  * Uses the IPDSuperimposedTextControl interface to perform the protocol
  * dependent sections of the control functions.
  */

#include <string>
#include <memory>

#include "bus/cctv/video_switch_agent/IDL/src/SuperimposedTextControlCorbaDef.h"
#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "app/cctv/video_switch_agent/src/UserPrivilegeVerifier.h"
#include "app/cctv/video_switch_agent/protocols/src/IPDSuperimposedTextControl.h"
   
#include "core/corba/src/ServantBase.h"

// Forward declarations.

namespace TA_IRS_App
{
    class VideoInput;
}

namespace TA_IRS_App
{
    class SuperimposedTextControl : public virtual POA_TA_Base_Bus::SuperimposedTextControlCorbaDef,
                                    public virtual TA_Base_Core::ServantBase
	{
	public:
        /**
          * SuperimposedTextControl
          *
          * Standard constructor.
          *
          * @param      VideoInput& videoInput
          *             The VideoInput that uses this SuperimposedTextControl.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if the protocol dependent initialisation fails.
          */

	    SuperimposedTextControl( VideoInput& videoInput );

        /**
          * ~SuperimposedTextControl
          *
          * Standard destructor.
          */

        virtual ~SuperimposedTextControl() {};

        /**
          * isFeatureSupported
          *
          * Returns true if the feature is supported by this VideoInput and by 
          * the protocol.
          *
          * @return     bool
          *             True if the feature is supported.
          *             False otherwise, even if the feature is not recognised
          *             or if a database problem occurs.
          * 
          * @param      TA_Visual::ESuperimposedTextFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::ESuperimposedTextFeature feature );

  	    ///////////////////////////////////////////////////////////////////////
	    //
	    // State Update Methods
	    //
	    ///////////////////////////////////////////////////////////////////////

        /** 
          * updateState
          *
          * Updates the superimposed text.
          *
          * @param      const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state
          *             The new state of the VideoInput.
          */

        void updateState( const TA_Base_Bus::VideoInputCorbaDef::VideoInputState& state );

        ///////////////////////////////////////////////////////////////////////
        //
        // SuperimposedTextControlCorbaDef Methods
        //
        ///////////////////////////////////////////////////////////////////////
	    
        /**
          * isDateSuperimposed
          *
          * Identifies if the date is superimposed on the VideoInput.
          * This will return false if SuperimposeDate is not supported.
          *
          * @return     bool
          *             True if the date is superimposed on the input.
          *             False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

        virtual bool isDateSuperimposed();

        /**
          * superimposeDate
          * 
          * Superimposes (or clears) the date on the VideoInput.
          *
          * @param      bool isDateSuperimposed
          *             True if the date should be superimposed on the VideoInput.
          *             False if the date should be cleared from the VideoInput.
          * @param      const char* sessionId
		  *				Used to determine if the user has sufficient 
		  *				privileges for this operation.
          *             
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
          *             3. The VideoInput is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
 		  *				6. There is a communications failure with the hardware.
          */

	    virtual void superimposeDate( bool isDateSuperimposed, const char* sessionId );
	    
        /**
          * isNameSuperimposed
          *
          * Identifies if the name of the VideoInput is superimposed on the VideoInput.
          * This will return false if SuperimposeName is not supported.
          *
          * @return     bool
          *             True if the name is superimposed on the input.
          *             False otherwise.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */

        virtual bool isNameSuperimposed();

        /**
          * superimposeName
          * 
          * Superimposes (or clears) the name of the VideoInput on the VideoInput.
          *
          * @param      bool isNameSuperimposed
          *             True if the name should be superimposed on the VideoInput.
          *             False if the name should be cleared from the VideoInput.
          * @param      const char* sessionId
		  *				Used to determine if the user has sufficient 
		  *				privileges for this operation.
          *             
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
          *             3. The VideoInput is out of service.
		  *				4. The agent is in Monitor mode.
		  *				5. The user does not have sufficient privileges for this operation.
 		  *				6. There is a communications failure with the hardware.
          */

	    virtual void superimposeName( bool isNameSuperimposed, const char* sessionId );
	   
        /**
          * getSuperimposedText
          *
          * This will return false if SuperimposeText is not supported.
          *
          * Gets the free text that is superimposed on the VideoInput.
          *
          * @return     std::string
          *             The text superimposed on the VideoInput or
          *             "" if nothing is displayed.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */
 	   
        virtual char* getSuperimposedText();

        /**
          * superimposeText
          * 
          * Superimposes (or clears) free text on the VideoInput.
          *
          * @param      const char* text
          *             The text to superimpose on the input or
          *             "" to clear the text superimposed on the input.
          * @param      const char* sessionId
		  *				Used to determine if the user has sufficient 
		  *				privileges for this operation.
          * 
          * @exception  VideoSwitchAgentException
          *             Thrown if:
          *             1. The user does not own the lock for this VideoInput.
          *             2. The VideoInput or the protocol does not support this operation.
          *             3. The text is too long.
          *             4. The text contains inappropriate words.
          *             5. The VideoInput is out of service.
		  *				6. The agent is in Monitor mode.
		  *				7. The user does not have sufficient privileges for this operation.
 		  *				8. There is a communications failure with the hardware.
          */

        virtual void superimposeText( const char* text, const char* sessionId );

        /**
          * getMaximumTextLength
          *
          * Gets the maximum number length of the const char* that can be displayed.
          *
          * @return     unsigned short
          *             The maximum number of characters that can be displayed as free text.
          *             Returns 0 if text cannot be displayed.
		  *
		  * @exception	VideoSwitchAgentException
		  *				Thrown if:
		  *				1. The agent is in Monitor mode.
		  *				2. There is a communications failure with the hardware.
          */
        
        virtual unsigned short getMaximumTextLength();

	private:
        //
        // Disable copy constructor and assignment operator.
        //

        SuperimposedTextControl( const SuperimposedTextControl& theSuperimposedTextControl);
		SuperimposedTextControl& operator=( const SuperimposedTextControl& );

        /**
          * hasInitialised
          *
          * Identifies if the object was initialised successfully.
          * Only used to test if the precondition for the following methods are met.
          * 
          * @return     bool
          *             True if the object was initialised successfully.
          */

        bool hasInitialised() const;

        /**
          * verifyFeatureIsSupported
          *
          * Checks that this particular VideoInput and the protocol support the
          * given feature.  If not, an exception is thrown.
          *
          * @param      TA_Visual::ESuperimposedTextFeature feature
          *             The feature to verify.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if the VideoInput or the protocol does not support this feature.
          */
 
        void verifyFeatureIsSupported( TA_Visual::ESuperimposedTextFeature feature );

        //
		// The VideoInput object that this interface supplies the text controls for.
		//

		VideoInput& m_videoInput;

        //
        // Object that handles the protocol dependent (PD/pd) parts of the functions
        // managing superimposed text.  
        // This is a smart pointer so it will automatically be cleaned up.
        //

        std::auto_ptr< IPDSuperimposedTextControl > m_pdSuperimposedTextControl;

        //
        // Determines if users have the rights to perform certain actions.
        //

        UserPrivilegeVerifier m_userPrivilegeVerifier;
    };

} // TA_IRS_App

#endif // !defined(SuperimposedTextControl_F136C5A2_7FC6_401f_98AE_919F73EE9F20__INCLUDED_)