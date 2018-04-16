/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/AbstractDocumentGUI.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to documents.
  */


#if !defined(AbstractDocumentGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
#define AbstractDocumentGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_

#include "bus/generic_gui/src/AbstractGUIApplication.h"

namespace TA_Base_Bus
{
   
    class AbstractDocumentGUI : public AbstractGUIApplication
    {

    public:
        /**
        * Constructor
        */
        AbstractDocumentGUI();


        /**
        * Destructor
        */
        virtual ~AbstractDocumentGUI();


        /**
        * displayApplication
        *
        * This launches and displays the application. 
        *
        * @return bool This indicates if the application should enter the message loop or not.
        *              Because we are not blocking then we will return true.
        */
        bool displayApplication();

    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        AbstractDocumentGUI( const AbstractDocumentGUI& theAbstractDocumentGUI){ };
        AbstractDocumentGUI& operator=(const AbstractDocumentGUI &) { }

    };
}

#endif // !defined(AbstractDocumentGUI_7242C8D4_73D4_471a_9871_4BE5DA5B6156__INCLUDED_)
