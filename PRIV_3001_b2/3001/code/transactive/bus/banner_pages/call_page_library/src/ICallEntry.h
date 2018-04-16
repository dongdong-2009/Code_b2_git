/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/banner_pages/call_page_library/src/ICallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This interface is implemented by classes that represent an item in
  * the Call Banner's list view. The AbstractCallEntry class should be
  * extended rather than extending this class directly.
  * 
  */


#if !defined(ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_)
#define ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_

#include <string>

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
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
	    virtual void onActivate() = 0;
		
		/**
		*	This method is called when user right click on a selected call item
		*	and select a Reset All PEC
		*	This will reset all PEC call that has the same train id
		**/
		virtual void onForceReset () {}
		virtual void onClearCall  () {}
		
		// expose interface to check if the call is invalid or not
		virtual bool isCallInvalid () {return false;}
        /**
         *  The destructor.
         */
        virtual ~ICallEntry() {}

    };

}// TA_IRS_Bus


#endif // !defined(ICallEntry_84DDDEE6_5E67_428a_A79C_DBB4018F7D58__INCLUDED_)