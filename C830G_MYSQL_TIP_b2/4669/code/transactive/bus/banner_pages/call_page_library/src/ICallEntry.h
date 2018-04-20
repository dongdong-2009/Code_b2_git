/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/banner_pages/call_page_library/src/ICallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This interface is implemented by classes that represent an item in
  * the Call Banner's list view. The AbstractCallEntry class should be
  * extended rather than extending this class directly.
  * 
  */


#if !defined(ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_)
#define ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_

#include <string>
#include <vector>

#include "bus/banner_pages/call_page_library/src/stdafx.h"


namespace TA_IRS_Bus
{

    /**
     * @version 1.0
     * @created 10-Feb-2004 10:53:09 AM
     */
    class ICallEntry
    {

    public:

		//TD 15643
		//zhou yuan++
		virtual bool isSoundClosed() = 0;

		virtual void closeSound() = 0;
		//++zhou yuan

        /**
         * Gets a string representation of the time of this call.
         */
        virtual std::string getTimeStr() const = 0;

        /**
         * Gets a string representation of the location of this call.
         */
        virtual std::string getLocationStr() const = 0;

        /**
         * Gets a string representation of the status of this call.
         */
        virtual std::string getStatusStr() const = 0;

        /**
         *  Gets the type of call entry that this object represents, as a
         *  string.
         */
        virtual std::string getType() const = 0;

        /**
         *  Gets the source of the call.
         */
        virtual std::string getSource() const = 0;

        /**
         *  Gets the additional information that the call banner displays, as
         *  a string.
         */
        virtual std::string getInformation() const = 0;
    
        /**
         * @return the severity of this call, 1 indicates lower severity (0 should not be used)
         *      It is assumed the severity does not change for the lifetime of a call entry
         */
        virtual int getSeverity() const = 0;
        
        /**
         * @return a string description of the category of this call, ie "Radio" or "PEC"
         */    
        virtual std::string getCategory() const = 0;

        /**
         *  Gets the id of this call entry. This id must be unique to all
         *  calls.
         *  @see AbstractCallEntryFactory::getUniqueCallEntryId()
         */
	    virtual unsigned long getId() const = 0;

        /**
         * @return true if this is an emergency call
         */
        virtual bool isEmergency() const = 0;

        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
	    virtual void onActivate() = 0;


        /**
         *  This method is called to retrieve a list of menu items to be displayed in a context menu
		 *	@return	a list of strings, each string representing a menu entry
         */
		virtual std::vector<std::string> getContextMenuItems() = 0;


        /**
         *  This method is called when a ContextMenu item was selected.
		 *	@return	a list of strings, each string representing a menu entry
         */
		virtual void onContextMenuCommand(std::string command) = 0;


        /**
         *  The destructor.
         */
        virtual ~ICallEntry() {}

    };

}// TA_IRS_Bus


#endif // !defined(ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_)