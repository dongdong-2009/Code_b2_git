#if !defined(IPDWashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_)
#define IPDWashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_switch_agent/protocols/src/IPDWashWipeControl.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Handles the protocol dependent sections of the washing and wiping
  * operations for a Camera.  
  * This interface is used by the WashWipeControl class. Before the methods
  * in this interface are called, the user privileges have been checked,
  * it has been verified that the user holds the lock on the Camera and
  * that the Camera and the protocol support the required features.
  */

#include "app/cctv/video_switch_agent/src/AllSupportedFeatures.h"

namespace TA_IRS_App
{
    class IPDWashWipeControl
    {
    public:
        /**
          * ~IPDWashWipeControl
          *
          * Standard destructor.
          */

        virtual ~IPDWashWipeControl() {};

        ///////////////////////////////////////////////////////////////////////
        //
        // Wash Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * startWashing
          *
          * Starts washing the Camera.
          *
          * @exception  VideoSwitchAgentException
          *                Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void startWashing() = 0;

        /**
          * stopWashing
          *
          * Stops washing the Camera.
          *
          * @exception  VideoSwitchAgentException
          *                Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void stopWashing() = 0;
        
        ///////////////////////////////////////////////////////////////////////
        //
        // Wipe Controls
        //
        ///////////////////////////////////////////////////////////////////////

        /**
          * startWiping
          *
          * Starts wiping the Camera.
          *
          * @exception  VideoSwitchAgentException
          *                Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void startWiping() = 0;

        /**
          * stopWiping
          *
          * Stops wiping the Camera.
          *
          * @exception  VideoSwitchAgentException
          *                Thrown if:
          *             1. The Camera address cannot be retrieved from the database.
          *             2. There is a communications failure with the hardware.
          */

        virtual void stopWiping() = 0;


        /**
          * isWiperOn
          *
          * Returns the current state of the wiper.
          *
          * @return     bool
          *             The wiper status.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          */

        virtual bool isWiperOn() = 0;


        /**
          * isWasherOn
          *
          * Returns the current state of the washer.
          *
          * @return     bool
          *             The washer status.
          *
          * @exception  VideoSwitchAgentInternalException
          *             Thrown if:
          *             1. The user does not own the lock for this Camera.
          *             2. The Camera or the protocol does not support this operation.
          *             3. The Camera is out of service.
          *             4. The agent is in Monitor mode.
          *             5. The user does not have sufficient privileges for this operation.
          */

        virtual bool isWasherOn() = 0;

    
        ///////////////////////////////////////////////////////////////////////
        //
        // Supporting Methods
        //
        ///////////////////////////////////////////////////////////////////////
        
        /**
          * isFeatureSupported
          *
          * Returns true if the feature is supported by the protocol.
          * This DOES NOT test if this Camera supports the feature only if the
          * protocol this Camera uses supports it.
          *
          * @return     bool
          *             True if the feature is supported.
          *             False otherwise, even if the feature is not recognised.
          * 
          * @param      TA_Visual::EWashWipeFeature feature
          *             The feature to check.
          */

        virtual bool isFeatureSupported( TA_Visual::EWashWipeFeature feature ) = 0;

    }; // IPDWashWipeControl

} // TA_IRS_App
    
#endif // !defined(IPDWashWipeControl_6FADFBB2_10B7_4ee8_AAFB_4BE601F07D48__INCLUDED_)
