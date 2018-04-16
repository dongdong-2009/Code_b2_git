#if !defined ALL_SUPPORTED_FEATURES_H
#define ALL_SUPPORTED_FEATURES_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/WXL1_TIP/3002/transactive/app/cctv/video_switch_agent/src/AllSupportedFeatures.h $
  * @author:   Katherine Thomson
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/07/30 17:42:30 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Provides enumerations of the features provided by the entities
  * monitored by the Video Switch Agent.
  *
  * Design Note: This file was created to remove the dependence between
  * the protocol dependent interfaces and the corba interfaces that
  * occurred when these enums were defined in the non-protocol dependent
  * classes.
  */

#include <string>

namespace TA_IRS_App
{
    namespace TA_Visual
    {
        //
        // String representations of features.
        //

        static const std::string AUTO_FOCUS                 = "AutoFocus";
        static const std::string MANUAL_FOCUS               = "ManualFocus";
        static const std::string FOCUS_NEAR                 = "FocusNear";
        static const std::string FOCUS_FAR                  = "FocusFar";
        static const std::string FOCUS_HALT                 = "FocusHalt";
        static const std::string AUTO_IRIS                  = "AutoIris";
        static const std::string MANUAL_IRIS                = "ManualIris";
        static const std::string IRIS_OPEN                  = "IrisOpen";
        static const std::string IRIS_CLOSE                 = "IrisClose";
        static const std::string IRIS_HALT                  = "IrisHalt";
        static const std::string IRIS_PEAK_WHITE_INVERSION  = "IrisPeakWhiteInversion";
        static const std::string PAN                        = "Pan";
        static const std::string PAN_LEFT                   = "PanLeft";
        static const std::string PAN_RIGHT                  = "PanRight";
        static const std::string PAN_HALT                   = "PanHalt";
        static const std::string TILT                       = "Tilt";
        static const std::string TILT_UP                    = "TiltUp";
        static const std::string TILT_DOWN                  = "TiltDown";
        static const std::string TILT_HALT                  = "TiltHalt";
        static const std::string PAN_AND_TILT               = "PanAndTilt";
        static const std::string ZOOM                       = "Zoom";
        static const std::string ZOOM_IN                    = "ZoomIn";
        static const std::string ZOOM_OUT                   = "ZoomOut";
        static const std::string ZOOM_HALT                  = "ZoomHalt";
        static const std::string PROPORTIONAL_MOVEMENT      = "ProportionalMovement";
        static const std::string PRESETS                    = "Presets";
        static const std::string PRESET_ACTIVATE            = "ActivatePreset";
        static const std::string PRESET_STORE               = "StorePreset";
        static const std::string PRESET_SET_NAME            = "SetPresetName";
        static const std::string WASH                       = "Wash";
        static const std::string WASH_START                 = "StartWashing";
        static const std::string WASH_STOP                  = "StopWashing";
        static const std::string WIPE                       = "Wipe";
        static const std::string WIPE_START                 = "StartWiping";
        static const std::string WIPE_STOP                  = "StopWiping";
        static const std::string LOCK                       = "Lock";
        static const std::string UNLOCK                     = "Unlock";
        static const std::string OVERRIDE_LOCK              = "OverrideLock";
        static const std::string REQUEST_SWITCH_OF_INPUT    = "RequestSwitchOfInput";
        static const std::string SUPERIMPOSE_DATE           = "SuperimposeDate";
        static const std::string SUPERIMPOSE_NAME           = "SuperimposeName";
        static const std::string SUPERIMPOSE_TEXT           = "SuperimposeText";
        static const std::string SET_PUBLIC_VIEWING_ALLOWED = "SetPublicViewingAllowed";
        static const std::string SET_IN_SERVICE             = "SetInService";
        static const std::string UNKNOWN                    = "Unknown";
        static const std::string SEQUENCE_SET_CONFIG        = "SetSequenceConfig";
        static const std::string SEQUENCE_CYCLE_TO_NEXT     = "CycleToNextVideoInput";
        static const std::string SEQUENCE_CYCLE_TO_PREVIOUS = "CycleToPreviousVideoInput";
        static const std::string SEQUENCE_PAUSE             = "Pause";
        static const std::string SEQUENCE_PLAY              = "Play";
        static const std::string SEQUENCE_STOP              = "Stop";

        //
        // Enumeration of the features supported by the FocusIrisControl interface.
        //

        enum EFocusIrisFeature
        {
            AutoFocus               = 0,
            ManualFocus             = 1,
            AutoIris                = 2,
            ManualIris              = 3,
            IrisPeakWhiteInversion  = 4
        };

        //
        // Enumeration of the features supported by the MovementControl interface.
        //

        enum EMovementFeature
        {
            Pan                     = 0,
            Tilt                    = 1,
            PanAndTilt              = 2,
            Zoom                    = 3,
            ProportionalMovement    = 4,
            Presets                 = 5
        };

        //
        // Enumeration of the features supported by the WashWipeControl interface.
        //

        enum EWashWipeFeature
        {
            Wash                    = 0,
            Wipe                    = 1
        };

        //
        // Enumeration of the features supported by the SuperimposedTextControl interface.
        //

        enum ESuperimposedTextFeature
        {
            SuperimposeDate         = 0,
            SuperimposeName         = 1,
            SuperimposeText         = 2
        };
    };

} // TA_IRS_App

#endif // ALL_SUPPORTED_FEATURES_H
