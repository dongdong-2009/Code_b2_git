/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/AbstractDialogGUI.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to dialogs.
  */


#if !defined(AbstractDialogGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
#define AbstractDialogGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_

#include "bus/generic_gui/src/AbstractGUIApplication.h"

namespace TA_Base_Bus
{
    class AbstractDialogGUI : public AbstractGUIApplication
    {
    public:
        /**
        * Constructor
        */
        AbstractDialogGUI(UINT nIDTemplate);


        /**
        * Destructor
        */
        virtual ~AbstractDialogGUI();


        /**
        * displayApplication
        *
        * This launches and displays the application. In this situation we have a dialog so
        * we call DoModal() on the dialog.
        *
        * @return bool This indicates if the application should enter the message loop or not.
        *              Because we are blocking on DoModal() then we will return false.
        */
        bool displayApplication();

		//TD18413
		//xinsong ++
		//for ecs manager, we should set DlgResourceId dynamically, it means that we should set the resource ID after called winApp::initInstance 
		void setDlgResourceID( UINT dlgResourceID )
		{
			m_dlgResourceID = dlgResourceID;
		}

    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        AbstractDialogGUI( const AbstractDialogGUI& theAbstractDialogGUI);
        AbstractDialogGUI& operator=(const AbstractDialogGUI &);

	private:
		UINT m_dlgResourceID;
    };
}

#endif // !defined(AbstractDialogGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
