/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/src/GenericGUI.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is the central class for GUI applications. It listens for several callbacks
  * and passes relevant messages onto the GUI application. This class also kicks off
  * ManagedProcess.
  */


#if !defined(GenericGUI_CDAAC093_38A2_4d54_806D_AD954A9FC4D8__INCLUDED_)
#define GenericGUI_CDAAC093_38A2_4d54_806D_AD954A9FC4D8__INCLUDED_

#include <string>
#include "core/process_management/IDL/src/IManagedGUICorbaDef.h"
#include "core/process_management/src/IManagedProcessUser.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/corba/src/ServantBase.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/utilities/src/RunParams.h"


namespace TA_Base_Core
{
	class ManagedProcess;
	class IEntityData;
};

namespace TA_Base_Bus
{
    class RightsLibrary;

	class IGUIApplication;

        class GenericGUI : public virtual POA_TA_Base_Core::IManagedGUICorbaDef,
                           public virtual TA_Base_Core::ServantBase,
                           public TA_Base_Core::IManagedProcessUser,
                           public TA_Base_Core::IOnlineUpdatable,
                           public TA_Base_Core::RunParamUser
    {
    public:

        /**
          * Constructor
          * 
          * This constructs an instance of GenericGUI.
          *
          * @param IGUIApplication& guiApp - This is a reference to a constructed GUI application
          *                                  object for GenericGUI to call.
          * @param string& commandLine     - This is the command line to parse.
          *
          * @exception GenericGUIException - If this exception is thrown the caller should check
          *                                  the type and 'advertise' to the user in an appropriate
          *                                  manner.
          *
          */
        GenericGUI(IGUIApplication& guiApp, const std::string& commandLine);


        /**
          * Destructor
          * 
          */
   	        virtual ~GenericGUI();

		bool init();
        /**
          * run
          * 
          * This starts the application and launches the GUI.
          * Preconditions - m_guiApplication is not null.
          *
          * @return This returns a boolean indicating whether the application should enter
          * the application message pump:
          */
        void run();

        
        /**
        * shutdown
        *
        * This will call notifyTerminating on the ManagedProcess object to
        * alert the Control Station that we are shutting down.
        */
        void shutdown();

        
        /**
          * onTerminate
          * 
          * This is called when the Control Station terminates the application.
          * Preconditions - m_guiApplication is not null.
          *
          */
        void onTerminate();


        /**
        * serverStateChange
        *
        * This lets a GUI know that the server has gone down or come back up. If it is
        * indicating that a server has gone down then this means the GUI application must
        * ensure it is in a reasonable state and stop the user from performing any more
        * actions which require contact with the server. 
        * If it indicates the server is back up the GUI should return to a normal operating
        * state. It should also indicate to the user visually that a server is down or up.
        * Preconditions - m_guiApplication is not null.
        *
        * @param bool isServerUp - This is set to 'true' if the server is functioning.
        *                        - This is set to 'false' if the server is not functioning.
        */
        void serverStateChange(bool isServerUp);


        /**
        * changeFocus
        *
        * This tells the application to change it's focus. It can tell the application
        * to take the focus or it can tell the application to minimise/maximise.
        * Preconditions - m_guiApplication is not null.
        *
        * @param    focusType - This indicates how the application's focus should change
        */
        void changeFocus(TA_Base_Core::EFocusType focusType);


		/**
		* setWindowPosition
		*
		* This tells an application where it should position its main window.
		*
        * @param posFlag - How the application is going to be positioned relative to boundary paramter
        * @param alignFlag - How the application is going to be positioned relative to point parameter
        * @param objectDim - The dimension of the object that this application will be launched relative
        *                    to (if POS_RELATIVE or POS_NOOVERLAP is specified).
        * @param boundaryDim - The dimension of the object that this application will be bounded in
        *                      (if POS_BOUNDED is specified)
		*/
        void setWindowPosition(unsigned long posFlag,
                               unsigned long alignFlag,
                               const TA_Base_Core::IManagedGUICorbaDef::RECT& objectDim,
                               const TA_Base_Core::IManagedGUICorbaDef::RECT& boundaryDim);

        /**
		 * dutyChanged
		 *
		 * This tells an application that the duties for the session in which it is running have been updated.
		 * The appliation is responsible for re-checking all duty-based functionality.
		 */
        void dutyChanged();


		/**
		* getWindowPosition
		*
		* This gets the current window position
        *
        * @return The window position in a RECT.
		*
		*/
		virtual TA_Base_Core::IManagedGUICorbaDef::RECT getWindowPosition();


        /**
        * processUpdate
        *
        * This tells the application that one of the objects it was interested in has
        * been updated and should be processed.
        * Preconditions - m_guiApplication is not null.
        *               - m_guiEntity is not null.
        *
        * @param    updateEvent - This event contains all the information about the update
        */
        void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


        /**
        * OnRunParamChange
        *
        * This tells the application when a RunParam has changed. This should check the
        * name to see that the parameter that has changed is the SESSIONID.
        * Preconditions - m_guiApplication is not null if the change is for the SESSIONID
        *
        * @param string& name -  The name of the changed parameter
        * @param string& value - The new value for the parameter
        */
        void onRunParamChange(const std::string& name, const std::string& value);


        /**
        * fatalErrorOccurred
        *
        * This is called to indicate that an unhandled exception was caught or something
        * terrible happened and the program aborted. This will set the m_terminateCode to
        * FatalError. This should be called by InitInstance() in the main application if an
        * exception is caught.
        * Preconditions - m_guiApplication is not null.
        */
        void fatalErrorOccurred()
        {
                m_terminateCode = TA_Base_Core::FatalError;
        };

        /**
         * getEntityKey
         *
         * This is called to retrieve the entity key from the IEntityData member held by this object.
         * If the IEntityData member is NULL, the entity key returned will be 0.
         *
         * @return The entity key of this applicaitons entity, or 0 if the entity was not succesfully retireved
         * from the database.
         */
        unsigned long getEntityKey();
        /**
        * setUpGUIEntity
        *
        * This is a helper method for the constructor.
        * This will retrieve the GUI Entity, ensure it exists and load it from the database
        */
        void setUpGUIEntity();
    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        GenericGUI( const GenericGUI& theGenericGUI);
        GenericGUI& operator=(const GenericGUI &);

        // Hide the default constructor so that it wouldn't be used by the corba interface
        GenericGUI();


        /**
        * setUpManagedProcess
        *
        * This is a helper method for the constructor. This sets up all the utilities such as
        * CorbaUtil and DebugUtil using the UtilityInitialiser and then creates a new instance
        * of ManagedProcess.
        *
        * @param string& commandLine     - This is the command line to parse.
        *
        */
        void setUpManagedProcess(const std::string& commandLine);


        /**
        * setUpApplicationType
        *
        * This is a helper method for the constructor.
        * Retrieve the application type and set it in RunParams.
        */
        void setUpApplicationType();


        /**
        * checkCommandLine
        *
        * This is a helper method for the constructor.
        * Check the command line passed was correct. We check entity separately so
        * this is just for any extra params.
        */
        void checkCommandLine();




        /**
        * checkPermission
        *
        * Validates the GUI is accessible by the current operator
        * 
        * @exception RightsException if the rights of the current operator is
        *            indeterminate or not applicable
        * @exception GenericGUIException::ACCESS_DENIED if the current operator
        *            does not have the rights to run the application
        */
        //bool checkPermission();


        //
        // This is the GUI application all the calls will be delegated to. This is set
        // in the constructor. This class is NOT responsible for deleting this object.
        //
        IGUIApplication& m_guiApplication;

        //
        // This holds the database information for the GUI entity. This class is responsible
        // for deleting the object when finished.
        //
	    TA_Base_Core::IEntityData* m_guiEntity;

        //
        // This holds the generic application object which is used to interface with
        // ManagedProcess. This class is responsible for deleting the object when
        // finsihed.
        //
        TA_Base_Core::ManagedProcess* m_managedProcess;

        //
        // This holds how the application was terminated. This will be passed to the
        // ManagedProcess object so it can tell the Control Station why we closed
        //
        TA_Base_Core::EManagedProcessTerminateCode m_terminateCode;

        // This is used to check operator's rights to perform certain actions
		// For reducing manager launching time - change the aca_RUN_APPLICATION check into control station
        //TA_Base_Bus::RightsLibrary* m_rightsMgr;

        // Resource id of the GUI entity
        unsigned long m_resourceId;
#if 0	//TD18355 bugfix, transferred to ConfigurationEditor.cpp
		//TD15529
		static const std::string CONFIGURATIONEDITOR_APPNAME;
		static const std::string CONFIGCONNECTIONFILE_NAME;
#endif
    };
}

#endif // !defined(GenericGUI_CDAAC093_38A2_4d54_806D_AD954A9FC4D8__INCLUDED_)
