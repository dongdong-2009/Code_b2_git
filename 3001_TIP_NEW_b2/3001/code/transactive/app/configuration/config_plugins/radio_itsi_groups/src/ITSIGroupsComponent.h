/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsComponent.h $
  * @author:  Derrick Liew
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is the component the main Configuration Editor framework communicates with.
  * It implements the IConfigurableComponent interface. It will handle all toolbar and menu
  * requests and handle them appropriately.
  */

// ITSIGroupsComponent.h: interface for the ITSIGroupsComponent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITSIGROUPSCOMPONENT_H__52456F80_5757_47DF_96FC_030564BD424C__INCLUDED_)
#define AFX_ITSIGROUPSCOMPONENT_H__52456F80_5757_47DF_96FC_030564BD424C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractComponent.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

namespace TA_IRS_App
{

	class ITSIGroupsComponent : public TA_Base_App::AbstractComponent  
	{

	public:

		/** 
         * Constructor
         *
         * @param const string& - This is the name of the component displaying in the selection bar so we
         *                        can use the same name in the DLL for sending audit messages etc.
         */
		ITSIGroupsComponent(const std::string& componentName);

        /**
         * Destructor
         */
		virtual ~ITSIGroupsComponent();

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
         * getRightsActionForAddingThisComponent
         *
         * This should return the rights action the user must have to be able to view and use this
         * component. This action id will be passed straight to the rights library.
         *
         * @return unsigned long - The action id to use when checking whether to add this component
         *                         to the Configuration Editor.
         */
        virtual unsigned long getRightsActionForAddingThisComponent()
        {
            return TA_Base_Bus::aca_CE_CONFIGURE_PROFILES;
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
        ITSIGroupsComponent( const ITSIGroupsComponent&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ITSIGroupsComponent& operator=(const ITSIGroupsComponent&);



    public:
        // This is the name of the DLL and is used to load resources correctly.
        static const CString DLL_NAME;


    private:


        // The icon for this component
        HICON m_icon;
	};
} //end of namespace

#endif // !defined(AFX_ITSIGROUPSCOMPONENT_H__52456F80_5757_47DF_96FC_030564BD424C__INCLUDED_)
