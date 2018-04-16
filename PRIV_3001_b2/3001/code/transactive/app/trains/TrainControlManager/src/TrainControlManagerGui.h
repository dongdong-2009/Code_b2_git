/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  oliverk
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#if !defined(TrainControlManagerGui_H)
#define TrainControlManagerGui_H


#include "bus/generic_gui/src/AbstractDialogGUI.h"


// forward declarations
namespace TA_Base_Bus
{
    class CtaRights;
}


namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 21-Dec-2007 3:57:37 PM
     */
    //take train control GUI window

    class TrainControlManagerGui : public TA_Base_Bus::AbstractDialogGUI
    {

    public:


        /**
         * Constructor
         */
        TrainControlManagerGui();


        /**
         * Destructor
         */
        virtual ~TrainControlManagerGui();


        /**
         * checkCommandLine  This checks that all the appropriate RunParams have been set
         * from the command line. If anything is invalid or missing then this will throw
         * an exception. This must be implemented in the lowest level class as this will
         * be where the GUI specific command line details are known. All settings SHOULD
         * be in the GUI entity and the only additional command line parameter should be --
         * control-entity.
         */
        virtual void checkCommandLine();


        /**
         * checkEntity  This will check that the entity is valid and then store a local
         * pointer to the entity. This must be implemented in the lowest level class as
         * this will be where the GUI specific configuration details are known.
         * @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
         *
         *
         * @param guiEntity
         */
        virtual void checkEntity( TA_Base_Core::IEntityData* guiEntity );


        /**
         * entityChanged  This will be called by GenericGUI when it receives a callback
         * indicating that the GUI entity has been modified. GenericGUI will have
         * invalidated the entity database object and will tell the GUI which items were
         * modified. The GUI just has to retrieve the details out of it and update
         * accordingly. This should be implemented by the lowest level class as it knows
         * exactly what to do with the updates.
         * @param vector<string>& changes - This is a list of all attributes changed in
         * the object.
         *
         * @param changes
         */
        virtual void entityChanged( const std::vector<std::string>& changes );


        /**
         * getApplicationType  This returns the application type for this GUI. GenericGUI
         * will then save this in the RunParams and will use it to pass to the Control
         * Station. In the existing system the application type would need to be hard-
         * coded into here. This method must be implemented by the lowest level class.
         * @return TRAIN_MANAGER_GUI_APPTYPE
         */
        virtual unsigned long getApplicationType();


        /**
         * loadAllUserSettings  This will load all the application's user preferences if
         * configured to do so. This should be called by the application itself in
         * OnInit() or equivalent.
         */
        virtual void loadAllUserSettings();


        /**
         * saveAllUserSettings  This will save all the application's user preferences if
         * configured to do so. This is provided in case the GUI needs to save user
         * preferences at a predetermined time. The settings are already saved when the
         * application exits.
         */
        virtual void saveAllUserSettings();


        /**
         * serverIsDown  This is called when the server the workstation is using goes down.
         * The GUI should indicate to the user that the server is down. It should also
         * then limit operations so that the user can only view existing data. This must
         * be implemented by the lowest level class as it knows what to do when this
         * occurs.
         */
        virtual void serverIsDown();


        /**
         * serverIsUp  This is called when the server the workstation is using starts
         * functioning again. The GUI should indicate to the user that the server is
         * functioning. It should  then allow the user to perform all valid operations
         * again. This must be implemented by the lowest level class as it knows what to
         * do when this occurs.
         */
        virtual void serverIsUp();


        virtual void onInitGenericGUICompleted();


    protected:

        /**
         * createApplicationObject  This method must set the m_application member variable.
         * It should create a new instance of the dialog/document/property sheet and
         * assign it to m_application
         */
        virtual void createApplicationObject();


        /**
         * prepareForClose  This method is used to prepare the GUI for closing. It should
         * save the user settings and perform any other tidying up. eg Stop any processing
         * threads, ask the user if they wish to save anything etc. This should be called
         * before any close is done on the GUI whether initiated by the GUI itself or the
         * Control Station. This means it should be the first call in the  implementation
         * of terminateGUI().
         */
        virtual void prepareForClose();


    private:

        /**
         * to determine if a particular session is allowed to perform an action on a resource
         */
        TA_Base_Bus::CtaRights* m_taRights;

    };

}

#endif // !defined(TrainControlManagerGui_H)
