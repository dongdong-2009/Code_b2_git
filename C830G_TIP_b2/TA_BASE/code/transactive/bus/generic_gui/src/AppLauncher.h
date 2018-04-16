/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/AppLauncher.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This singleton contains the methods used to launch applications through the Control
  * Station and load displays into GraphWorX.
  *
  */

#if !defined(AppLauncher_9FCD55E4_FA39_4c9f_85E2_E91D863F9550__INCLUDED_)
#define AppLauncher_9FCD55E4_FA39_4c9f_85E2_E91D863F9550__INCLUDED_

#include "bus/generic_gui/src/stdafx.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include <string>
#include "core/process_management/IDL/src/IControlStationCorbaDef.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

class AppLauncherTest;

namespace TA_Base_Bus
{
    class AppLauncher
    {
        friend class AppLauncherTest;

    public:
        /**
         * Destructor
         */
        virtual ~AppLauncher();


        /**
         * getInstance
         * 
         * This creates an instance of this class on the first call. Following
         * this it will return the created instance on each subsequent call.
         *
         * @return A reference to an AppLauncher object.
         *
         */
        static AppLauncher& getInstance();


        /** 
          * getRect
          *
          * Gets the CRect value of the specified area on the specified screen.
          *
          * @param targetScreen - The screen on the MFT
          * @param targetArea - The target area within the screen
          * @param val - Optional value.  This value should to the x coordinate value 
          *              if targetScreen is SCREEN_CURRENT, SCREEN_PREVIOUS or SCREEN_NEXT.  
          *              It should contain the screen number if target screen is 
          *              SCREEN_SPECIFIC.  By default this is set to -1.
          *
          * @return The CRect value of the targetArea that lies on targetScreen.
          *
          * @exception ApplicationException Indicates it could not get the rect from control station
          *
          */
        RECT getRect(const TA_Base_Bus::TA_GenericGui::EScreen targetScreen, 
                    const TA_Base_Bus::TA_GenericGui::EArea targetArea, 
                    const int val = -1);



        /** 
          * launchApplication
          *
          * This attempts to locate the Control Station and tells it to launch the specified application
          *
          * @param appType - The application to launch
          * @param extraCommandLine - The command line to be passed to the application.
          * @param posType - How the application is going to be positioned relative to boundary paramter
          * @param alignType - How the application is going to be positioned relative to point parameter
          * @param objectDim - The dimension of the object that this application will be launched relative
          *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
          * @param boundaryDim - The dimension of the object that this application will be bounded in
          *                      (if POS_BOUNDED is specified)
          *
          * @exception ApplicationException Indicates the application was not launched
          *
          */
        void launchApplication(const unsigned long appType, 
                               const std::string& extraCommandLine, 
                               const unsigned long posType = 0,
                               const unsigned long alignType = 0,
                               const RECT* objectDim = NULL,
                               const RECT* boundaryDim = NULL);


        /**
         * launchGraphWorxDisplay
         *
         * This attempts to load the specified display in GraphWorX. The GraphWorX instance used
         * will be depedent on the screen passed in.
         *
         * PRECONDITION: The display passed is not an empty string
         *
         * @param string - The name and path of the display to load
         * @param string - The asset in the display to be centered on the screen
         * @param long - The x co-ordinate of the left side of the application that is calling
         *               this method. This is so it can be determined which screen to launch
         *               the display on.
         *
         * @exception ApplicationException Indicates the display was not launched.
         *
         */
        void launchGraphWorxDisplay(const std::string& display, 
                                    const std::string& asset = "", 
                                    const long leftPosition = 0);





        /** 
          * calculateNewCoordinates
          *
          * This method calculates the new coordinates of the application window
          * based on the positioning and alignment flags.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param alignFlag - Contains the alignment flags
          * @param posFlag - Contains the positioning flags
          *
          * @return true if the coordinates have been altered
          *
          */
        static bool calculateNewCoordinates(RECT& appDim,
                                           const RECT& objectDim,
                                           const RECT& boundaryDim,
                                           const unsigned long posFlag,
                                           const unsigned long alignFlag);

    
    private:
        /**
        * These are made private as this is a singleton
        */
        AppLauncher();
        AppLauncher(const AppLauncher& prm1){ };
        AppLauncher& operator =(const AppLauncher& prm1){ };


        /**
         * findControlStation
         *
         * This actually resolves the Control Station and stores it in m_controlStation
         *
         * @exception ApplicationException thrown if the Control Station could not be resolved
         */
        void findControlStation();


        /** 
          * calculateRelativeRect
          *
          * Calculates the rect coordinates of the application window
          * relative to the centre of the specified object.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param vAlign - The vertical alignment of the application relative to the object
          * @param hAlign - The horizontal alignment of the application relative to the object
          *
          */
        static void calculateRelativeRect(RECT& appDim,
                                         const RECT& objectDim,
                                         const unsigned long vAlign,
                                         const unsigned long hAlign);


        /** 
          * calculateNoOverlapRect
          *
          * Calculates the new coordinates of the application window
          * relative to but not overlapping the specified object.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param vAlign - The vertical alignment of the application relative to the object
          * @param hAlign - The horizontal alignment of the application relative to the object
          *
          */
        static void calculateNoOverlapRect(RECT& appDim,
                                             const RECT& objectDim,
                                             const unsigned long vAlign,
                                             const unsigned long hAlign);


        /** 
          * calculateBoundedRect
          *
          * Calculates the rect coordinates of the application window
          * that aligns relative to the boundary.  Application may be resized to fit
          * within the boundary if the application falls outside the boundary and isResizable 
          * is true.
          *
          * @param appDim - The dimension of the application window
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param vAlign - The vertical alignment of the application relative to the boundary
          * @param hAlign - The horizontal alignment of the application relative to the boundary
          * @param isResizable - True if the applicaiton is resizable to fit within the boundary
          *
          */
        static void calculateBoundedRect(RECT& appDim,
                                        const RECT& boundaryDim,
                                        const unsigned long hAlign,
                                        const unsigned long vAlign,
                                        const bool isResizable);

       
        /** 
          * calculateRelativeBoundedRect
          *
          * Calculates the new coordinates of the application window
          * relative to the centre of the specified object and bounded within
          * the boundary.  Application may be resized to fit within the boundary if 
          * the application falls outside the boundary and isResizable is true.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param vAlign - The vertical alignment of the application relative to the object
          * @param hAlign - The horizontal alignment of the application relative to the object
          * @param isResizable - True if the application is resizable to fit within the boundary
          *
          */
        static void calculateRelativeBoundedRect(RECT& appDim,
                                                const RECT& objectDim,
                                                const RECT& boundaryDim,
                                                const unsigned long hAlign, 
                                                const unsigned long vAlign,
                                                const bool isResizable);

    
        /** 
          * calculateNoOverlapBoundedRect
          *
          * This method calculates the rect coordinates of the application window when it is
          * bounded within a boundary and could not overlap a certain object area.
          *
          * It tries to position the application window within the boundary as best as
          * possible.  If it is not resizable and it falls outside the boundary, then it will try
          * to fit into the If the app couldn't fit into the
          * boundary without intersecting the object (i.e. after trying out all four positions)
          * then the application will have to overlap as much of the object as possible to keep
          * the application within the boundary.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param hAlign - horizontal alignment flag
          * @param vAlign - vertical alignment flag
          * @param isResizable - True if the applicaiton is resizable to fit within the boundary
          *
          */
        static void calculateNoOverlapBoundedRect(RECT& appDim,
                                                 const RECT& objectDim,
                                                 const RECT& boundaryDim,
                                                 const unsigned long hAlign,
                                                 const unsigned long vAlign,
                                                 const bool isResizable);


        /** 
          * calculateRelativeCoordinates
          *
          * Helper method for calculateRelativeRect method.  
          *
          * Calculates the horizontal or vertical coordinates of the application window
          * relative to the centre of the specified object.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param align - The alignment of the application relative to the centre
          *                of the object
          *
          */
        static void calculateRelativeCoordinates(RECT& appDim,
                                             const RECT& objectDim,
                                             const unsigned long align);

 
        /** 
          * calculateBoundedCoordinates
          *
          * Helper method for calculateNoOverlapBoundedRect method.  
          *
          * Calculates the horizontal or vertical coordinates of the application window
          * that aligns relative to the boundary.  Application may be resized to fit
          * within the boundary if the application falls outside the boundary and isResizable 
          * is true.
          *
          * @param appDim - The dimension of the application window
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param align The alignment of the application relative to the boundary
          * @param isResizable - True if the applicaiton is resizable to fit within the boundary
          *
          */
        static void calculateBoundedCoordinates(RECT& appDim,
                                                const RECT& boundaryDim,
                                                const unsigned long align,
                                                const bool isResizable);


        /** 
          * calculateNoOverlapBoundedCoordinates
          *
          * Helper method for calculateNoOverlapBoundedRect method.  
          *
          * Calculates the horizontal or vertical coordinates of the application window
          * relative to the edge of the specified object and bounded within
          * the boundary.  Application may be resized to fit within the boundary if 
          * the application falls outside the boundary and isResizable is true.
          *
          * @param appDim - The dimension of the application window
          * @param objectDim - The dimension of the object
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param align - The alignment of the application relative to the centre
          *              or edge of the object
          * @param needOffset - True if the application is to allow for some offset to avoid 
          *                     overlapping the object
          *
          */
        static bool calculateNoOverlapBoundedCoordinates(RECT& appDim,
                                                        const RECT& objectDim,
                                                        const RECT& boundaryDim,
                                                        unsigned long align,
                                                        const bool needOffset);


        /** 
          * isInvalidRect
          *
          * Returns true if the rect is not valid - i.e. if the rect's coordinates does not
          * defines a rect, line or point.  Also returns true if the rect is the default rect
          * i.e. have -1 for all coordinate values.
          *
          * @param rect The rectangle object to examine
          *
          * @return true if the rect is not valid or if it's the default rect
          */
        static bool isInvalidRect(const RECT& rect);


        /** 
          * boundAppRect
          *
          * Confine the application rect within the boundary.  It can either do this by either 
          * truncating the application rect that falls outside the boundary (i.e. shrinking it)
          * or shifting the application rect into the boundary.  If the application is not resizable
          * but a part of it still falls out of the boundary after being shifted, then it will
          * readjust itself such that the top and left parts are confined within the boundary.
          *
          * @param appDim - The dimension of the application window
          * @param boundaryDim - The dimension of the boundary to bind the application within
          * @param isResizable - True if the applicaiton is resizable to fit within the boundary
          *
          */
        static void boundAppRect(RECT& appDim,
                                const RECT& boundaryDim,
                                const bool isResizable);

        // A pointer to the Control Station CORBA object that we need to call to 
        // actually launch an application.
        TA_Base_Core::IControlStationCorbaDef_var m_controlStation;

        static AppLauncher* s_instance;

	    // protect singleton creation
        static TA_Base_Core::ReEntrantThreadLockable m_singletonLock;

        // constants used for calculating alignment
        static const unsigned int HORIZONTAL_ALIGNMENTS;
        static const unsigned int VERTICAL_ALIGNMENTS;
        static const unsigned int DEFAULT_HORIZONTAL_ALIGNMENT;
        static const unsigned int DEFAULT_VERTICAL_ALIGNMENT;
    };
}

#endif // !defined(AppLauncher_9FCD55E4_FA39_4c9f_85E2_E91D863F9550__INCLUDED_)
