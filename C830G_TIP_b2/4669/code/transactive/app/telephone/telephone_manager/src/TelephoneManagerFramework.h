#if !defined(TelephoneManagerFramework_0E764553_2A21_479f_8886_E426FE487DDF__INCLUDED_)
#define TelephoneManagerFramework_0E764553_2A21_479f_8886_E426FE487DDF__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/TelephoneManagerFramework.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class provides the managed process interface for the application.  This
  * handles the creation of the CtaTelephoneManagerSummary class and  the
  * monitoring of changes passed from the control station.
  */

#include <string>
#include <vector>

#include "bus/generic_gui/src/AbstractDialogGUI.h"
#include "app/telephone/telephone_manager/src/CtaTelephoneManagerSummary.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"

class CTelephoneManagerDlg;
class CtaTelephoneManagerSummary;


class TelephoneManagerFramework : public TA_Base_Bus::AbstractDialogGUI
{

public:
    TelephoneManagerFramework();
    virtual ~TelephoneManagerFramework();

public:
    // TD18095, jianghp
    virtual void onInitGenericGUICompleted();

    /**
      * long getApplicationType
      *
      * This returns the application type for this GUI. GenericGUI will then save this
      * in the RunParams and will use it to pass to the Control Station. In the
      * existing system the application type would need to be hard-coded into here.
      * This method must be implemented by the lowest level class.
      *
      * @return virtual unsigned long representing the application type
      *
      * @exception <exceptions> Optional
      */
    virtual unsigned long getApplicationType();

    /**
      * checkEntity
      *
      * This will check that the entity is valid and then store a local pointer to the
      * entity. This must be implemented in the lowest level class as this will be
      * where the GUI specific configuration details are known.
      * @exceptions GenericGUIException with the type set to
      * ENTITY_CONFIGURATION_INVALID. This should be thrown if the application checks
      * the entity and it is not configured correctly for the GUI to run.
      *
      * @return virtual void
      * @param : TA_Base_Core::IEntityData* guiEntity
      *          A pointer to the gui entity to check and store.
      *
      * @exception <exceptions> Optional
      */
    virtual void checkEntity(TA_Base_Core::IEntityData* guiEntity);

    /**
      * checkCommandLine
      *
      * This checks that all the appropriate RunParams have been set from the command line.
      * If anything is invalid or missing then this will throw an exception. This must be
      * implemented in the lowest level class as this will be where the GUI specific
      * command line details are known. All settings SHOULD be in the GUI entity and the
      * only additional command line parameter should be --control-entity.
      *
      * @return virtual void
      *
      * @exceptions GenericGUIException with the type set to COMMAND_LINE_INVALID
      *             It must throw this if the command line does not contain the correct
      *             information for the GUI to run.
      */

    virtual void checkCommandLine();

   /**
     * This will be called by GenericGUI when it receives a callback indicating that
     * the GUI entity has been modified. GenericGUI will have invalidated the entity
     * database object and will tell the GUI which items were modified. The GUI just
     * has to retrieve the details out of it and update accordingly. This should be
     * implemented by the lowest level class as it knows exactly what to do with the
     * updates.
     * @param changes    This is a list of all attributes changed in the object.
     *
     * @return virtual void
     *
     * @exception <exceptions> Optional
     */
    virtual void entityChanged(const std::vector<std::string>& changes);

    /**
      * serverIsDown
      *
      * This is called when the server the workstation is using goes down. The GUI should
      * indicate to the user that the server is down. It should also then limit operations
      * so that the user can only view existing data. This must be implemented by the lowest
      * level class as it knows what to do when this occurs.
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void serverIsDown();

    /**
      * serverIsUp
      *
      * This is called when the server the workstation is using starts functioning again.
      * The GUI should indicate to the user that the server is functioning. It should
      * then allow the user to perform all valid operations again. This must be implemented by
      * the lowest level class as it knows what to do when this occurs.
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void serverIsUp();

    /**
      * changeFocus
      *
      * The Control Station needs to alert the GUI applications when they should regain
      * the focus on the screen. This is the most common focus request by the Control
      * Station but it may also need to tell the applications to minimise or maximise.
      * This is done through this method. This takes an enumeration indicating how the
      * GUI should change it's focus.
      *
      * @return virtual void
      *
      * @param    EFocusType focus - This indicates how the window should change it's focus.
      *
      *
      * @exception <exceptions> Optional
      */
    // virtual void changeFocus(const TA_Base_Core::EFocusType focus) ; //TES230

    /**
      * This method is used to prepare the GUI for closing. It should save the user
      * settings and perform any other tidying up. eg Stop any processing threads, ask
      * the user if they wish to save anything etc. This should be called before any
      * close is done on the GUI whether initiated by the GUI itself or the Control
      * Station. This means it should be the first call in the implementation of
      * terminateGUI().
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void prepareForClose();

    /**
      * loadAllUserSettings
      *
      * This will load all the application's user preferences if configured to do so.
      * This should be called by the application itself in OnInit() or equivalent.
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void loadAllUserSettings();

    /**
      * saveAllUserSettings
      *
      * This will save all the application's user preferences if configured to do so.
      * This is provided in case the GUI needs to save user preferences at a predetermined
      * time. The settings are already saved when the application exits.
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void saveAllUserSettings();

    /**
      * onRunParamChange
      *
      * called when a runparam we have registered for changes
      *
      * This method checks if the parameter change reflects new launch conditions.
      * If so it updates the message fields and updates the user.
      *
      * If the new parameter is invalid or the resolution of the new fields fails
      * then all fields are cleared and the user is notified accordingly.
      *
      * If the parameter and value do not differ from the current launch conditions then
      * no action is taken
      *
      * @return virtual void
      *
      * @exception <exceptions> Optional
      */
    virtual void onRunParamChange(const std::string& name, const std::string& value);


protected:
    /**
     * This method must set the m_application member variable. It should create a new
     * instance of the dialog/document/property sheet and assign it to m_application
     */
    virtual void createApplicationObject();

private:
    CTelephoneManagerDlg* m_ptelephoneManagerDlg;
    CtaTelephoneManagerSummary *m_pctaTelephoneManagerSummary;
    TelephoneRightsInitializThread m_telephoneRightsInitializThread;
};
#endif // !defined(TelephoneManagerFramework_0E764553_2A21_479f_8886_E426FE487DDF__INCLUDED_)
