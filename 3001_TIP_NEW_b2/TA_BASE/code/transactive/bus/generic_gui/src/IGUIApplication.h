/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/src/IGUIApplication.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This interface provides a contract between GenericGUI and an actual GUI application. GenericGUI
  * calls methods on this interface to alert the GUI to various system changes and requests.
  *
  * TODO:
  * This interface should also inherit from some security interface. The security
  * library should listen for configuration updates and then perform a callback on
  * this interface. This needs to wait until the security library has been designed.
  *
  */


#if !defined(IGUIApplication_B0F30686_4610_4906_90AA_45CF33D4EDA2__INCLUDED_)
#define IGUIApplication_B0F30686_4610_4906_90AA_45CF33D4EDA2__INCLUDED_

#include "core\process_management\IDL\src\ProcessManagementDataDefinitionsCorbaDef.h"
#include <string>
#include <vector>

namespace TA_Base_Core
{
	class IEntityData;
};

namespace TA_Base_Bus
{
    class IGUIApplication
    {

    public:

        /**
        * Destructor
        */
        virtual ~IGUIApplication(){ };

		//TD18095, jianghp
		virtual void onInitGenericGUICompleted() = 0;
		virtual void setGenericGUIStartCompleted(bool bValue) = 0;
		virtual bool getGenericGUIStartCompleted() = 0;
		//TD18095, jianghp

        /**
        * getApplicationType
        * 
        * This returns the application type for this GUI. GenericGUI will then save
        * this in the RunParams and will use it to pass to the Control Station. In the
        * existing system the application type would need to be hard-coded into here.
        * This method must be implemented by the lowest level class.
        *
        * @return unsigned long representing the application type
        *
        */
        virtual unsigned long getApplicationType() =0;


        /**
        * checkEntity
        * 
        * This will check that the entity is valid and then store a local pointer to the
        * entity. This must be implemented in the lowest level class as this will be where
        * the GUI specific configuration details are known.
        *
        * @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
        *
        * @exceptions GenericGUIException with the type set to ENTITY_CONFIGURATION_INVALID
        *             This should be thrown if the application checks the entity and it is
        *             not configured correctly for the GUI to run.
        *
        */
        virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity) =0;


        /**
        * checkCommandLine
        *
        * This checks that all the appropriate RunParams have been set from the command line.
        * If anything is invalid or missing then this will throw an exception. This must be
        * implemented in the lowest level class as this will be where the GUI specific
        * command line details are known. All settings SHOULD be in the GUI entity and the
        * only additional command line parameter should be --control-entity.
        *
        * @exceptions GenericGUIException with the type set to COMMAND_LINE_INVALID
        *             It must throw this if the command line does not contain the correct
        *             information for the GUI to run.
        */
        virtual void checkCommandLine() = 0;


        /**
        * displayApplication
        *
        * This launches and displays the application. For example if the application is a
        * dialog then this will probably call dialog.DoModal().
        *
        * @return bool This indicates if the application should enter the message loop or not.
        *              If this method includes a blocking call such as DoModal() then it should
        *              return false. If it launches an application and returns immediately it
        *              should return true.
        *
        * @exceptions There are no specific exceptions thrown from the displayApplication method.
        *             However this method calls DoModal() or displays a window to the user. This
        *             method should be surrounded by a catch (...) and if an exception is caught
        *             fatalErrorOccurred() should then be called on GenericGUI.
        */
        virtual bool displayApplication() =0;


        /**
        * entityChanged
        *
        * This will be called by GenericGUI when it receives a callback indicating that the
        * GUI entity has been modified. GenericGUI will have invalidated the entity database
        * object and will tell the GUI which items were modified. The GUI just has to retrieve
        * the details out of it and update accordingly. This should be implemented by the
        * lowest level class as it knows exactly what to do with the updates.
        *
        * @param vector<string>& changes - This is a list of all attributes changed in the
        *                                  object.
        *
        */
        virtual void entityChanged(const std::vector<std::string>& changes) =0;


        /**
        * serverIsDown
        *
        * This is called when the server the workstation is using goes down. The GUI should
        * indicate to the user that the server is down. It should also then limit operations
        * so that the user can only view existing data. This must be implemented by the lowest
        * level class as it knows what to do when this occurs.
        */
        virtual void serverIsDown() =0;

        
        /**
        * serverIsUp
        *
        * This is called when the server the workstation is using starts functioning again.
        * The GUI should indicate to the user that the server is functioning. It should 
        * then allow the user to perform all valid operations again. This must be implemented by
        * the lowest level class as it knows what to do when this occurs.
        */
        virtual void serverIsUp() =0;


        /**
        * changeFocus
        *
        * The Control Station needs to alert the GUI applications when they should regain
        * the focus on the screen. This is the most common focus request by the Control
        * Station but it may also need to tell the applications to minimise or maximise.
        * This is done through this method. This takes an enumeration indicating how the
        * GUI should change it's focus.
        *
        * @param    EFocusType focus - This indicates how the window should change it's focus.
        */
        virtual void changeFocus(const TA_Base_Core::EFocusType focus) =0;


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
                               const RECT& boundaryDim) =0;


		/**
		* getWindowCoordinates
		*
		* This gets the current window position
        *
        * @return The window position in a RECT.
		*
		*/
		virtual RECT getWindowCoordinates() =0;

        
        /**
        * terminateFromControlStation
        *
        * This is called to actually close the application.
        */
        virtual void terminateFromControlStation() =0;

        /**
         * dutyChanged
         *
         * The method notifies an application that duty has changed for the session in which it is running.
         * The application is responsible for re-checking all duty based functionality.
         */
        virtual void dutyChanged() =0;
    };
}

#endif // !defined(IGUIApplication_B0F30686_4610_4906_90AA_45CF33D4EDA2__INCLUDED_)
