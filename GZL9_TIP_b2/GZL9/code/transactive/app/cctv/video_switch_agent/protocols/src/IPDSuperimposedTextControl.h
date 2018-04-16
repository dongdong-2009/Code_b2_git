#if !defined(IPDSuperimposedTextControl_BB59CA3D_CE10_4f96_9F54_812C0C59A337__INCLUDED_)
#define IPDSuperimposedTextControl_BB59CA3D_CE10_4f96_9F54_812C0C59A337__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_switch_agent/protocols/src/IPDSuperimposedTextControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of superimposed text control for a VideoInput.
  * This interface is used by the SuperimposedTextControl class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the VideoInput and
  * that the VideoInput and the protocol support the required features.
  */

#include <string>

#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"
#include "bus/cctv_gzl9/video_switch_agent/IDL/src/VideoInputCorbaDef.h"

namespace TA_IRS_App
{
    class IPDSuperimposedTextControl
    {
    public:
        /**
          * ~IPDSuperimposedTextControl
          *
          * Standard destructor.
          */

        virtual ~IPDSuperimposedTextControl() {};

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
          * @param      const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state
          *             The new state of the VideoInput.
          */

        virtual void updateState( const TA_Base_Bus_GZL9::VideoInputCorbaDef::VideoInputState& state ) = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Protocol Dependent ISuperimposedTextControl Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * isDateSuperimposed
          *
          * Identifies if the date is superimposed on the VideoInput.
          *
          * @return     bool
          *             True if the date is superimposed on the input.
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual bool isDateSuperimposed() = 0;

        /**
          * superimposeDate
          * 
          * Superimposes (or clears) the date on the VideoInput.
          *
          * @param      bool isDateSuperimposed
          *             True if the date should be superimposed on the VideoInput.
          *             False if the date should be cleared from the VideoInput.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void superimposeDate( bool isDateDisplayed ) = 0;
        
        /**
          * isNameSuperimposed
          *
          * Identifies if the name of the VideoInput is superimposed on the VideoInput.
          *
          * @return     bool
          *             True if the name is superimposed on the input.
          *             False otherwise.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual bool isNameSuperimposed() = 0;

        /**
          * superimposeName
          * 
          * Superimposes (or clears) the name of the VideoInput on the VideoInput.
          *
          * @param      bool isNameSuperimposed
          *             True if the name should be superimposed on the VideoInput.
          *             False if the name should be cleared from the VideoInput.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void superimposeName( bool isNameSuperimposed ) = 0;
       
        /**
          * getSuperimposedText
          *
          * Gets the free text that is superimposed on the VideoInput.
          *
          * @return     std::string
          *             The text superimposed on the VideoInput or
          *             "" if nothing is displayed.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */
        
        virtual std::string getSuperimposedText() = 0;

        /**
          * superimposeText
          * 
          * Superimposes (or clears) free text on the VideoInput.
          *
          * @param      const std::string& text
          *             The text to superimpose on the input or
          *             "" to clear the text superimposed on the input.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */

        virtual void superimposeText( const std::string& text ) = 0;

        /**
          * getMaximumTextLength
          *
          * Gets the maximum number length of the string that can be displayed.
          *
          * @return     unsigned short
          *             The maximum number of characters that can be displayed as free text.
          *             Returns 0 if text cannot be displayed.
          *
          * @exception  VideoSwitchAgentException
          *             Thrown if there is a communications failure with the hardware.
          */
        
        virtual unsigned short getMaximumTextLength() = 0;

        ///////////////////////////////////////////////////////////////////////
        //
        // Supporting Methods
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * isFeatureSupported
          *
          * Returns true if the feature is supported by the protocol.
          * This DOES NOT test if this VideoInput supports the feature only if the
          * protocol this VideoInput uses supports it.
          *
          * @return     bool
          *             True if the feature is supported.
          *             False otherwise, even if the feature is not recognised.
          * 
          * @param      TA_Visual::ESuperimposedTextFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::ESuperimposedTextFeature feature ) = 0;
    };

} // TA_IRS_App

#endif // !defined(IPDSuperimposedTextControl_BB59CA3D_CE10_4f96_9F54_812C0C59A337__INCLUDED_)
