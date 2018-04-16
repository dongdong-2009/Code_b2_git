/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingComponent.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

#if !defined(MmsSchedulingComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
#define MmsSchedulingComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_

#include <string>
#include <map>

#include "app/configuration/configuration_editor/src/IConfigurableComponent.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

namespace TA_Base_App
{
    class IDllHandler;
    class IProgressStatusBar;
}

namespace TA_IRS_App
{
    class MmsSchedulingView;
    class MmsSchedulingDatabaseAccessor;

    class MmsSchedulingComponent : public TA_Base_App::IConfigurableComponent
    {

    public:
        /** 
         * Constructor
         *
         * @param const string& - This is the name of the component displaying in the selection bar so we
         *                        can use the same name in the DLL for sending audit messages etc.
         */
        MmsSchedulingComponent(const std::string& componentName);


        /**
         * Destructor
         */
	    virtual ~MmsSchedulingComponent();


    public:
	    
	    /**
         * getIcon
         *
         * This returns the icon to be displayed in the selection bar.
         *
         * @return HICON - A handle to an icon that has been created
         */
	    virtual HICON getIcon();


	    /**
         * getComponentView
         *
         * This returns runtime class for the view to be shown in the bottom-right pane
         * of the application.
         *
         * @return CRuntimeClass* - This will be used to create the view in the correct pane.
         */
	    virtual CRuntimeClass* getComponentView();


        /**
         * getMmsSchedulingActionForAddingThisComponent
         *
         * This should return the rights action the user must have to be able to view and use this
         * component. This action id will be passed straight to the rights library.
         *
         * @return unsigned long - The action id to use when checking whether to add this component
         *                         to the Configuration Editor.
         */
        virtual unsigned long getRightsActionForAddingThisComponent()
        {
            // If we can run the configuration editor, then we're allow to view this component
            return TA_Base_Bus::aca_CE_CONFIGURE_MMS_SCHEDULING;
        }
        

        /** 
         * setUpSingletons
         *
         * This will set up the singletons in the DLL to match those in the main application.
         *
         * @param IDllHandler& - This is a handler from the main application that can be used to
         *                       retrieve the details of it's singletons. This can then be used to
         *                       set up the singletons in the DLL.
         */
        virtual void setUpSingletons(const TA_Base_App::IDllHandler& dllHandler);

        
        /**
         * itemSelected
         *
         * This is called when the user selects a toolbar of menu item that this object is interested
         * in. The menu item or toolbar button name is passed so this method can determine the function to
         * call.
         *
         * @param const CString& - The name of the toolbar or menu item selected. This should be exactly
         *                         the same as one of the strings passed in the MenuItem::menuItem or the
         *                         ToolBarItem::name structure.
         */
        virtual void itemSelected(const CString& menuItem);


        /**
         * getMenuItems
         *
         * This returns the menu items required by this component. We override the abstract
         * versions as we do not want any menu items
         *
         * @return std::vector<TA_Base_App::MenuItem> - All menu items required
         */
    	virtual std::vector<TA_Base_App::MenuItem> getMenuItems();


	    /**
         * getToolBarItems
         *
         * This returns the toolbar items required by this component. We override the abstract
         * versions as we do not want any toolbar items
         *
         * @return std::vector<TA_Base_App::ToolBarItem> - All toolbar items required
         */
    	virtual std::vector<TA_Base_App::ToolBarItem> getToolBarItems();


        /**
         * setCreatedView
         *
         * This is called to pass the configurable component the view that has been created for it.
         * Because this only passes out the CRuntimeClass of the view it doesn't have an actual 
         * pointer to the view itself. This method should cast the view to the type it is
         * expecting so it can use it from now on.
         *
         * @param View* - The view that has been created for us
         */
        virtual void setCreatedView(CView*);


        /**
         * setStatusBar
         *
         * This lets the DLL know about the status bar so that it can use it as a progress bar
         * when loading large amounts of data.
         *
         * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */
        virtual void setStatusBar(TA_Base_App::IProgressStatusBar* statusBar);

        
        /**
         * reload
         *
         * This will be called if the user wishes to reload all data from the database. It should ensure
         * that the user is given an opportunity to save any work they are currently working on and then
         * reloads all data from the database.
         */
        virtual void reload();


        /**
         * areCurrentChangesPending
         *
         * This is called to determine if there are any current changes that have not been applied
         * before reloading or exiting the program. If this returns true then it will populate the
         * vector of strings passed in with all items not applied.
         * 
         * @param vector<string>& - This is an OUT parameter and should be passed in empty. It will
         *                          be populated with names for all data not yet applied
         * 
         * @return bool - This will return true if there are items that have had changes which have  not
         *                been applied.
         */
        virtual bool areCurrentChangesPending(std::vector<std::string>& dataNotApplied);
             
        
        /**
         * getDatabaseAccessor
         *
         * This returns a reference to an MmsSchedulingDatabaseAccessor. This object creates this and it will
         * be used by all other objects requiring database access.
         *
         * @return IDatabaseAccessor* - The object used to access the database.
         */
        MmsSchedulingDatabaseAccessor* getDatabaseAccessor()
        {
            return m_databaseAccessor;
        };


        /**
         * getMessageSender
         *
         * This returns a reference to a MessageSender. This object creates this and it will be used
         * by all other objects needing to send audit and online updates.
         *
         * @return MessageSender& - The object to use to send messages.
         */
       TA_Base_App::MessageSender& getMessageSender()
        {
            return m_messageSender;
        };


        /**
         * getStatusBar
         *
         * This returns a pointer to the status bar. Other objects will need this to display their
         * progress.
         *
         * @return IProgressStatusBar - The status bar object to use to show progress
         */
       TA_Base_App::IProgressStatusBar* getStatusBar() const
        {
            return m_statusBar;
        }


    protected:

        /**
         * getDLLName
         *
         * This returns the name of the DLL.
         */
        virtual std::string getDLLName() const;


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        MmsSchedulingComponent( const MmsSchedulingComponent&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        MmsSchedulingComponent& operator=(const MmsSchedulingComponent&);



    public:
        // This is the name of the DLL and is used to load resources correctly.
        static const CString DLL_NAME;


    private:

        // The icon for this component
        HICON m_icon;

        // The main view for this component. We will need to communicate to it and it needs to talk
        // to us so we store it here.
        MmsSchedulingView* m_frameworkView;

        // This is the status bar to use for showing progress
        TA_Base_App::IProgressStatusBar* m_statusBar;

        // This loads all the rights for this configurable component and stores them for us.
        // This must be stored in this class so it remains persistant and does not get deleted with
        // the views each time the user selects to view a different component.
        MmsSchedulingDatabaseAccessor* m_databaseAccessor;

        // This holds the message senders that can be used to send the audit and online update
        // messages. This must be stored in this class so it remains persistant and does not get
        // deleted with the views each time the user selects to view a different component.
        TA_Base_App::MessageSender m_messageSender;

        // This is the name of this component in the Configuration Editor selection bar. This is
        // needed for some error messages and audit messages.
        std::string m_componentName;
    };
}

#endif // !defined(MmsSchedulingComponent_A4EEC36D_18C5_4d02_927B_EC8EFFC7172B__INCLUDED_)
