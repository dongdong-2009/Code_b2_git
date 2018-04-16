/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/AbstractGUIApplication.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This inherits from IGUIApplication and IGUIAccess and implements all methods that are common
  * between dialogs, property sheets and documents. It also introduces some protected methods to 
  * be implemented by child classes to aid in consistency.
  */


#if !defined(AbstractGUIApplication_AAA8CF05_F8FC_451c_B03B_791AB8385F14__INCLUDED_)
#define AbstractGUIApplication_AAA8CF05_F8FC_451c_B03B_791AB8385F14__INCLUDED_

#include "bus/generic_gui/src/IGUIApplication.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include <string>
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

class CWnd;

namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
	class SettingsMgr;

    
    
    class AbstractGUIApplication : public IGUIApplication, public IGUIAccess
    {

    public:
        /**
        * Constructor
        */
        AbstractGUIApplication();


        /**
        * Destructor
        */
        virtual ~AbstractGUIApplication();

		//TD18095, jianghp
		virtual void setGenericGUIStartCompleted(bool bValue)
		{
			m_genericGUIStartCompleted = bValue;
		}

		virtual bool getGenericGUIStartCompleted()
		{
			return m_genericGUIStartCompleted ;
		}
		//TD18095, jianghp

        /**
        * getSessionId
        *
        * Implementing for IGUIAccess. This will return the current sessionId. This should
        * be called each time the sessionId is needed and it will ensure the application is
        * always using the current one.
        *
        * @return string The current sessionId.
        */
        virtual std::string getSessionId(); 


        /**
        * getGUIEntity
        *
        * Implementing for IGUIAccess. This will return a pointer to the current GUI entity.
        * This is the database object representing the GUI entity in the database. A copy of
        * this object should not be stored as only the original object is configured to be
        * automatically updated with configuration changes.
        *
        * @return IEntityData* A pointer to the GUI entity database object.
        */
        virtual TA_Base_Core::IEntityData* getGUIEntity() const 
        { 
            return m_guiEntity; 
        }


        /**
        * changeFocus
        *
        * Implementing for IGUIApplication. The Control Station needs to alert the GUI
        * applications when they should regain the focus on the screen. This is the most
        * common focus request by the Control Station but it may also need to tell the
        * applications to minimise or maximise. This is done through this method. This takes
        * an enumeration indicating how the GUI should change it's focus. Changing the focus
        * of a window is the same for all window types so this can be implemented here.
        *
        * @param    EFocusType focus - This indicates how the window should change it's focus.
        */
        virtual void changeFocus(const TA_Base_Core::EFocusType focus);

        /**
		 * dutyChanged
		 *
		 * This tells an application that the duties for the session in which it is running have been updated.
		 * The appliation is responsible for re-checking all duty-based functionality.
         *
         * NB: The application should override this method if they wish to be notified of duty changes.
		 */
        virtual void dutyChanged();


        /**
        * setPosition
        *
        * This function sets the position of the window. This method does not actually
        * position the window but stores the co-ordinates for later use. (ie the window
        * cannot be moved until it has been created.
		*
        * @param posFlag - How the application is going to be positioned relative to boundary paramter
        * @param alignFlag - How the application is going to be positioned relative to point parameter
        * @param objectDim - The dimension of the object that this application will be launched relative
        *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
        * @param boundaryDim - The dimension of the object that this application will be bounded in
        *                      (if POS_BOUNDED is specified)
        */
        virtual void setPosition(const unsigned long posFlag,
                               const unsigned long alignFlag,
                               const RECT& objectDim,
                               const RECT& boundaryDim);


		/**
		* getWindowCoordinates
		*
		* This gets the current window position
        *
        * @return The window position in a RECT.
		*
		*/
		virtual RECT getWindowCoordinates();

        
        /**
        * terminateFromUser
        *
        * Implementing for IGUIAccess. This is called to actually close the application. This
        * will call the more generic terminateGUI() because whether we are terminating from the
        * Control Station or the application itself the method will be the same.
        */
        virtual void terminateFromUser();


        /**
        * terminateFromControlStation
        *
        * Implementing for IGUIApplication. This is called to actually close the application.
        * This will call the more generic terminateGUI() because whether we are terminating from
        * the Control Station or the application itself the method will be the same.
        */
        virtual void terminateFromControlStation();


    protected:

        /**
        * createApplicationObject
        *
        * This method must set the m_application member variable. It should create a new
        * instance of the dialog/document/property sheet and assign it to m_application
        */
        virtual void createApplicationObject() =0;


        /**
        * terminateGUI
        *
        * This is called to actually close the application. This should call the appropriate
        * MFC calls to shutdown the window properly. Its first task will be to call
        * prepareForClose().
        */
        virtual void terminateGUI();


        /**
        * loadPositionSettings()
        *
        * This loads the size and position settings of the application and should be called from
        * loadAllUserSettings(). The position stuff has been separated out as it is generally
        * the same for all applications.
        * Note: This should only load size and position settings if it is configured in the
        * system that it should do so.
        *
        * @exception UserSettingsException - Thrown for a variety of reasons but it basically means
        *                                    that the window cannot be positioned.
        */
        void loadPositionSettings();


        /**
        * savePositionSettings()
        *
        * This saves the size and position settings of the application and should be called from
        * saveAllUserSettings(). The position stuff has been separated out as it is generally
        * the same for all applications.
        * Note: This should only save size and position settings if it is configured in the
        * system that it should do so.
        *
        * @exception UserSettingsException - Thrown for a variety of reasons but it basically means
        *                                    that the window position cannot be saved.
        */
        void savePositionSettings();


        /**
         * setWindowPosition
         *
         * This will move the position of the window to the position specified in m_leftWindowPosition
         * and m_topWindowPosition. If these values are negative then no setting will be applied.
         * 
         */
        void setWindowPosition();


        /**
        * prepareForClose
        *
        * This method is used to prepare the GUI for closing. It should save the user settings
        * and perform any other tidying up. eg Stop any processing threads, ask the user if they
        * wish to save anything etc.
        * This should be called before any close is done on the GUI whether initiated by the
        * GUI itself or the Control Station. This means it should be the first call in the 
        * implementation of terminateGUI().
        */
        virtual void prepareForClose() =0;


        /** 
          * setGuiEntity
          *
          * Set the Gui entity data
          *
          * @param guiEntity Pointer to the new Gui entity data
          */
        void setGuiEntity(TA_Base_Core::IEntityData& guiEntity)
        {
            m_guiEntity = &guiEntity;
        }


        /** 
          * setApplicationWnd
          *
          * Set the pointer to the actual property sheet/dialog/frame object. It
          * will be used to perform commands such as closing down, displaying the
          * application and changing it's size.
          *
          * @param application Pointer to the actual property sheet/dialog/frame object
          */
        void setApplicationWnd(CWnd& application)
        {
            m_application = &application;
        }

		void setApplicationWnd( CWnd * application )
		{
			m_application = application;
		}
        
        /** 
          * getApplicationWnd
          *
          * Get the pointer to the actual property sheet/dialog/frame object. It
          * will be used to perform commands such as closing down, displaying the
          * application and changing it's size.
          *
          * @return Pointer to the actual property sheet/dialog/frame object
          */
        CWnd* getApplicationWnd() const 
        {
            return m_application;
        }


        /** 
          * setUserSetting
          *
          * Sets the user settings manager.
          *
          * @param userSettings The Pointer to the user settings manager
          */
        void setUserSetting(TA_Base_Bus::SettingsMgr& userSettings);

        
        /** 
          * getUserSetting
          *
          * Gets the user settings manager.
          *
          * @return The Pointer to the user settings manager
          */
        TA_Base_Bus::SettingsMgr* getUserSetting() const 
        {
            return m_userSettings;
        }

	protected:
		//TD18095, jianghp
		bool m_genericGUIStartCompleted;
    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        AbstractGUIApplication(const AbstractGUIApplication& rhs){ };
        AbstractGUIApplication& operator=(const AbstractGUIApplication&){ };


        /**
          * WndEnumHandler
          *
          * This handles callbacks from ::EnumWindows()
          * This is a handler for top level window enumeration, which lets us
          * determine which windows need to be closed before the app can exit.
          *
          * @return TRUE to continue enumerating windows, FALSE to stop.
          *
          */
	    static BOOL CALLBACK WndEnumHandler(HWND hWnd, LPARAM lParam);


        /** 
          * moveWindow
          *
          * Move the window to the calculated position.
          *
          */
        void moveWindow();


        /** 
          * areRectsEqual
          *
          * Returns true if both rects are equal.
          *
          * @param rect1
          * @param rect2
          *
          * @return true if both rects are equal.
          *
          */
        bool areRectsEqual(const RECT& rect1, const RECT& rect2)
        {
            return ((rect1.left == rect2.left) &&
                    (rect1.right == rect2.right) &&
                    (rect1.top == rect2.top) &&
                    (rect1.bottom == rect2.bottom));
        }

        ///////////////////////////////////////////
        // Member Variables
        ///////////////////////////////////////////

        //
        // This holds a pointer to the GUI's entity.
        //
	    TA_Base_Core::IEntityData* m_guiEntity;

        //
        // This will be a pointer to the actual property sheet/dialog/frame object. It
        // will be used to perform commands such as closing down, displaying the
        // application and changing it's size.
        //
        CWnd* m_application;

        //
        // The user settings manager is created and saved here
        //
        TA_Base_Bus::SettingsMgr* m_userSettings;

        struct WindowPositionInformation
        {
            unsigned long posFlag;      // Contains the positioning flags
            unsigned long alignFlag;    // Contains the alignment flags
            RECT boundaryDim;           // The dimension of the boundary to bind the application within
            RECT objectDim;             // The dimension of the object
            RECT appDim;                // The calculated dimension of the object
        };

        // The information required to move the correct application into the correct position.
        WindowPositionInformation m_positionInfo;  
		//WindowPositionInformation m_childpositionInfo;
        // True if it is the first time the window position is calculated
        // This is needed as applications that are calculated not to move
        bool m_isFirst;               
    };
}

#endif // !defined(AbstractGUIApplication_AAA8CF05_F8FC_451c_B03B_791AB8385F14__INCLUDED_)
