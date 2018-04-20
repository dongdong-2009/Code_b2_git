/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/TransActiveFrame.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/01/02 14:50:26 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from CFrameWnd and implements all common functionality for
  * TransActive document frames.
  */

#if !defined(AFX_TRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
#define AFX_TRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/ResizingProperties.h"
#include "bus/generic_gui/src/ITransActiveFrame.h"

namespace TA_Base_Bus
{
    class IGUIAccess;

    class TransActiveFrame : public CFrameWnd,
                             public ITransActiveFrame
    {
        DECLARE_DYNCREATE(TransActiveFrame)
        
    protected: // create from serialization only

       /**
        * Constructor
        */
        TransActiveFrame();


    public:

        /**
        * Destructor
        */
       virtual ~TransActiveFrame();

       /**
        * setCallbackObject
        *
        * This not only sets the callback IGUIAccess object it also performs
        * the generic initialisation required by all windows. This means this
        * must be called after the Window has been created.
        *
        * @param IGuiAccess& - The IGUIAccess object to perform calls on. 
        */
       virtual void setCallbackObject( IGUIAccess& gui);


    protected:

        /**
         * setResizingProperties
         *
         * This method should be called to set the resizing properties of the
         * dialog. It must be called before setCallbackObject is called. Ideally it
         * will be called from the constructor of the frame.
         */
        void setResizingProperties(const ResizingProperties& resizingProperties);

        /**
         * OnClose
         *
         * This method must be implemented to ensure that when the window
         * is closed the window tells the GUIAccess object to clean up
         */
        virtual afx_msg void OnClose();

        /**
         * OnSysCommand
         *
         * This method must be implemented to handle requests to display
         * the About box.
         */
        virtual afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

        /**
         * OnGetMinMaxInfo
         *
         * This method controls the resizing and maximising of the frame.
         * It ensures the frame is not resized to an inappropriate size.
         * It uses the m_resizingProperties member variable.
         */
        virtual afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);

        /**
         * OnWindowPosChanging
         *
         * This method controls the resizing of the frame.
         * It ensures the frame is not resized to an inappropriate size.
         * It uses the m_resizingProperties member variable.
         */
        virtual afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);

        /**
         * OnSetWindowPosition
         *
         * This method is called whenever GenericGui gets a new window position
         * from control station
         * 
         */
        virtual afx_msg LRESULT OnSetWindowPosition(WPARAM wParam, LPARAM lParam);

        DECLARE_MESSAGE_MAP()

        /**
         * getGenericGUICallbackObject
         *
         * This method is provided so classes that inherit from this class can
         * use the IGUIAccess object.
         *
         * @return IGUIAcces& - A reference to the GUI object so callbacks can
         *                      be performed.
         */
        IGUIAccess& getGenericGUICallbackObject();

		/**
         * getWorkArea
         *
         * This method is create for Move dialog to the work area,
         * do not cover the banner and tool bar (otherwise the work area is the schematic area)
         * same code as OnGetMinMaxInfo mostly
         */
		void getWorkArea();

		/**
         * moveToWorkArea
         *
         * This method is create for Move dialog to the work area,
         * do not cover the banner and tool bar (otherwise the work area is the schamatic area)
         * the return value windowRect is the modified dialog position
         */
		void moveToWorkArea(CRect& windowRect);

    private:

        // Callback object for GenericGUI
        IGUIAccess* m_genericGUICallback;

        // This contains the details on how the window can be resized.
        ResizingProperties m_resizingProperties;

        // This indicates if the resizing properties have been set
        bool m_arePropertiesSet;

        // This variable stores the monitor on which this application is currently located
        int m_currentScreen;

        // This is the available screen rectangle for the current screen. By caching this, 
        // we do not have to query the control station until we change screens.
        CRect m_workingArea;

    };
}

#endif // !defined(AFX_TRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
