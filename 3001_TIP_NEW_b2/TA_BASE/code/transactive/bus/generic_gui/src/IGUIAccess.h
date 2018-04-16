/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/generic_gui/src/IGUIAccess.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This interface provides a contract between a GUI application and GenericGUI. The application
  * calls methods on this interface to retrieve the updates done by GenericGUI and to access
  * functionality common to all GUI's.
  */

#include <string>

#if !defined(IGUIAccess_A8A39D5A_19B4_4f34_A9A4_D8EB8A947ADF__INCLUDED_)
#define IGUIAccess_A8A39D5A_19B4_4f34_A9A4_D8EB8A947ADF__INCLUDED_

namespace TA_Base_Core
{
	class IEntityData;
};

namespace TA_Base_Bus
{
    class IGUIAccess
    {

    public:

        /**
         * Destructor
         */
        virtual ~IGUIAccess() { };


       /**
        * loadAllUserSettings
        *
        * This will load all the application's user preferences if configured to do so.
        * This should be called by the application itself in OnInit() or equivalent. 
        *
        */
        virtual void loadAllUserSettings() =0;


        /**
        * saveAllUserSettings
        *
        * This will save all the application's user preferences if configured to do so.
        * This is provided in case the GUI needs to save user preferences at a predetermined
        * time. The settings are already saved when the application exits.
        *
        */
        virtual void saveAllUserSettings() =0;


        /**
        * getSessionId
        *
        * This will return the current sessionId. This should be called each time the sessionId
        * is needed and it will ensure the application is always using the current one.
        *
        * @return string The current sessionId.
        */
        virtual std::string getSessionId() =0;


        /**
        * getGUIEntity
        *
        * This will return a pointer to the current GUI entity. This is the database object
        * representing the GUI entity in the database. A copy of this object should not be
        * stored as only the original object is configured to be automatically updated with
        * configuration changes.
        *
        * @return IEntityData* A pointer to the GUI entity database object.
        */
	        virtual TA_Base_Core::IEntityData* getGUIEntity() const =0;


        /**
        * terminateFromUser
        *
        * This is called to actually close the application. All closing will be done from in
        * here and the not the application itself.
        */
        virtual void terminateFromUser() =0;
    };
}

#endif // !defined(IGUIAccess_A8A39D5A_19B4_4f34_A9A4_D8EB8A947ADF__INCLUDED_)
