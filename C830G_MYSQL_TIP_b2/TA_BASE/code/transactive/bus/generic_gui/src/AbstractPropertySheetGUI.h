/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/AbstractPropertySheetGUI.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to 
  * property sheets.
  */


#if !defined(AbstractPropertySheetGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
#define AbstractPropertySheetGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_

#include "bus/generic_gui/src/AbstractGUIApplication.h"

namespace TA_Base_Bus
{
    class AbstractPropertySheetGUI : public AbstractGUIApplication
    {
    public:
        /**
        * Constructor
        */
        AbstractPropertySheetGUI();


        /**
        * Destructor
        */
        virtual ~AbstractPropertySheetGUI();


        /**
        * displayApplication
        *
        * This launches and displays the application. In this situation we have a
        * Property Sheet so we call DoModal() on the Property Sheet.
        *
        * @return bool This indicates if the application should enter the message loop or not.
        *              Because we are blocking on DoModal() then we will return false.
        */
        bool displayApplication();


    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        AbstractPropertySheetGUI( const AbstractPropertySheetGUI& theAbstractPropertySheetGUI){ };
        AbstractPropertySheetGUI& operator=(const AbstractPropertySheetGUI &) { }

    };
}

#endif // !defined(AbstractPropertySheetGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
