/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/GenericGuiConstants.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This file contains the constants required to launch an application
  * 
  */

#if !defined(__GENERIC_GUI_CONSTANTS_H__)
#define __GENERIC_GUI_CONSTANTS_H__

// Since this is a constant file that may be used by applications and control station we use
// two levels of namespace to ensure clashes do not happen.
namespace TA_Base_Bus
{
	namespace TA_GenericGui
	{
        ////////////////////////////////////////////////////////////////////////
        // 
        // Enumerated types and constants required for application positioning
        //
        ////////////////////////////////////////////////////////////////////////

        enum EScreen                // Used to specify a screen on the MFT
        {
            SCREEN_CURRENT,         // Area within the current monitor that currentX lies on 
            SCREEN_PREVIOUS,        // Area within the monitor on the left of the one that currentX lies on.  If there is no screen on the left of this, it will just use the current monitor.
            SCREEN_NEXT,            // Area within the monitor on the right of the one that currentX lies on.  If there is no screen on the right of this, it will just use the current monitor.
            SCREEN_SPECIFIC,        // Area within the specified monitor
            SCREEN_FIRST,           // Area within the first monitor (ignores val)
            SCREEN_LAST,            // Area within the last monitor (ignores val)
            SCREEN_ALL              // Area within all screens
        };

        enum EArea                  // Used to specify an area on the screen
        {
            AREA_SCREEN,            // The entire screen area
            AREA_BANNER,            // The banner area
            AREA_LAUNCHPAD,         // The launch pad area 
            AREA_MAINNAVIGATOR,     // The main navigator area
            AREA_SCHEMATIC,         // The display area within the main navigator
        };

        enum EPosition              // Postioning flags - Used to indicate how the application will be positioned, can use a combination
        {
            POS_NONE = 0,           // Do not move the window
            POS_RELATIVE = 0x1,     // Relative to the 'objectDim'.  
            POS_BOUNDED = 0x2,      // Bounded within the rectangle area of 'boundaryDim'.
            POS_NOOVERLAP = 0x4,    // Relative to the edges of 'objectDim'
            POS_RESIZABLE = 0x8     // If POS_BOUNDED is defined, it will resize to fit within the boundary
        };

        enum EAlignment             // Alignment flags - Used to indicate how the application will be aligned relative to a point, can use a combination
        {
            ALIGN_FIT = 0x1,        // Fit into the specified area
            ALIGN_LEFT = 0x2,       // Leftmost position at x coordinate of 'objectDim' or 'boundaryDim'
            ALIGN_VERTICAL = 0x4,   // Vertically centred (i.e. x coordinate = [left+right]/2)'objectDim' or 'boundaryDim'
            ALIGN_RIGHT = 0x8,      // Rightmost position at x coordinate of 'objectDim' or 'boundaryDim'
            ALIGN_TOP = 0x10,       // Topmost position at y coordinate of 'objectDim' or 'boundaryDim'
            ALIGN_HORIZONTAL = 0x20,// Horizontally centred (i.e. y coordinate = [top+bottom]/2) 'objectDim' or 'boundaryDim'
            ALIGN_BOTTOM = 0x40,    // Bottommost position at y coordinate of 'objectDim' or 'boundaryDim'
        };

        enum EWindowPosition        // The new position of the window
        {
            MINIMISE,               // Minimise the app
            MAXIMISE,               // Maximise the app
            FOCUS,                  // Give focus to the app
            REPOSITION,             // Reposition the app
            NO_REPOSITION,          // This is the same as FOCUS but this is used in launchApplication call rather than getFocus call
            CENTRE,                 // Centre the app
            REPOSITION_NO_RESTORE
        };

        // This is used to indicate caller has not specified a proper rectangle area
        static const RECT DEFAULT_RECT = {-1, -1, -1, -1};

        // Get a unique user defined message id for set window position message.
        // Send WM_SET_WINDOW_POSITION message with RECT* as first argument (NULL if not POSITION)
        // and the  EWindowPosition as second argument.
        static UINT WM_SET_WINDOW_POSITION = RegisterWindowMessage("SetWindowPosition");
    };
};

#endif

