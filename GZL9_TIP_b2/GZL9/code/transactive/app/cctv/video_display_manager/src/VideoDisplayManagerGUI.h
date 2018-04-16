/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerGUI.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * TODO: <description>
  */

// VideoDisplayManagerGUI.h: interface for the VideoDisplayManagerGUI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(VIDEODISPLAYMANAGERGUI_H__52855386_6960_11D7_B175_0050BAB094A9__INCLUDED_)
#define VIDEODISPLAYMANAGERGUI_H__52855386_6960_11D7_B175_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/cctv/video_display_manager/src/IVideoDisplayManagerDAI.h"
#include "bus/generic_gui/src/AbstractDialogGUI.h"

namespace TA_IRS_App
{
	class VideoDisplayManagerPresenter;
    class VideoDisplayManagerGUI : public TA_Base_Bus::AbstractDialogGUI
    {

    public:
    /**
        * Constructor
        *
        * This must construct an instance of the dialog and store it in m_application. When
        * constructing the dialog this class passes in an instance of itself.
        */
        VideoDisplayManagerGUI( );

        /**
        * Destructor
        *
        * This should delete the object stored at m_application
        */
        ~VideoDisplayManagerGUI();

        /**
        * getApplicationType
        *
        * This returns the application type for this GUI. GenericGUI will then save
        * this in the RunParams and will use it to pass to the Control Station. In the
        * existing system the application type would need to be hard-coded into here.
        *
        * @return unsigned long representing the application type
        *
        */
        unsigned long getApplicationType();

        /**
        * checkEntity
        *
        * This will check that the entity is valid and then store a local pointer to the
        * entity.
        *
        * @param IEntityData* guiEntity - A pointer to the gui entity to check and store.
        *
        * @exceptions GenericGUIException with the type set to ENTITY_CONFIGURATION_INVALID
        *             This should be thrown if the application checks the entity and it is
        *             not configured correctly for the GUI to run.
        *
        */
        void checkEntity(TA_Base_Core::IEntityData* guiEntity);

        /**
        * checkCommandLine
        *
        * This checks that all the appropriate RunParams have been set from the command line.
        * If anything is invalid or missing then this will throw an exception.
        * All settings SHOULD be in the GUI entity and the only additional command line
        * parameter should be --control-entity.
        *
        * @exceptions GenericGUIException with the type set to COMMAND_LINE_INVALID
        *             It must throw this if the command line does not contain the correct
        *             information for the GUI to run.
        */
        void checkCommandLine();

        /**
        * entityChanged
        *
        * This will be called by GenericGUI when it receives a callback indicating that the
        * GUI entity has been modified. GenericGUI will have invalidated the entity database
        * object and will tell the GUI which items were modified. The GUI just has to retrieve
        * the details out of it and update accordingly.
        *
        * @param vector<string>& changes - This is a list of all attributes changed in the
        *                                  object.
        */
        void entityChanged(const std::vector<std::string>& changes);

        /**
        * serverIsDown
        *
        * This is called when the server the workstation is using goes down. The GUI should
        * indicate to the user that the server is down. It should also then limit operations
        * so that the user can only view existing data.
        */
        void serverIsDown();

        /**
        * serverIsUp
        *
        * This is called when the server the workstation is using starts functioning again.
        * The GUI should indicate to the user that the server is functioning. It should
        * then allow the user to perform all valid operations again.
        */
        void serverIsUp();

        /**
        * loadAllUserSettings
        *
        * This will load all the application's user preferences if configured to do so.
        * This should be called by the application itself in OnInit() or equivalent.
        *
        * @exceptions TODO:
        */
        void loadAllUserSettings();

        /**
        * saveAllUserSettings
        *
        * This will save all the application's user preferences if configured to do so.
        * This is provided in case the GUI needs to save user preferences at a predetermined
        * time. The settings are already saved when the application exits.
        *
        * @exceptions TODO:
        */
        void saveAllUserSettings();
        // void setPosition(const unsigned long posFlag,
        // const unsigned long alignFlag,
        // const RECT& objectDim,
        // const RECT& boundaryDim);


    protected:

           /**
        * prepareForClose
        *
        * This method is used to prepare the GUI for closing. It should save the user settings
        * and perform any other tidying up. eg Stop any processing threads, ask the user if they
        * wish to save anything etc.
        */
        void prepareForClose();

        /**
        * createApplicationObject
        *
        * This method must set the m_application member variable. It should create a new
        * instance of the dialog/document/property sheet and assign it to m_application
        */
        virtual void createApplicationObject();

        virtual void onInitGenericGUICompleted();

    private:
        // 
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        // 
        VideoDisplayManagerGUI( const VideoDisplayManagerGUI& theVideoDisplayManagerGUI);
        VideoDisplayManagerGUI& operator=(const VideoDisplayManagerGUI &);

		IVideoDisplayManagerDAI * m_dai;
		VideoDisplayManagerPresenter* m_presenter;
    };

} // TA_IRS_App

#endif // !defined(VIDEODISPLAYMANAGERGUI_H__52855386_6960_11D7_B175_0050BAB094A9__INCLUDED_)
