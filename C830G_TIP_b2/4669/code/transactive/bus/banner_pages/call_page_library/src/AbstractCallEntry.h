/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class implements the ICallEntry interface.
  * It is provided as a convenience class so that subclasses of ICallEntry do
  * not have to implement its accessors and mutators.
  * 
  */


#if !defined(AbstractCallEntry_FC2ED4EE_5CEA_469b_9432_09BA702CBAA6__INCLUDED_)
#define AbstractCallEntry_FC2ED4EE_5CEA_469b_9432_09BA702CBAA6__INCLUDED_

#include "bus/banner_pages/call_page_library/src/ICallEntry.h"


namespace TA_IRS_Bus
{
    /**
     * @version 1.0
     * @created 10-Feb-2004 10:55:13 AM
     */
    class AbstractCallEntry : public ICallEntry 
    {

    public:

        /**
         *  The destructor.
         */
	    virtual ~AbstractCallEntry();

        
        /**
         *  Creates a new call entry with the given id, source and information.
         *  Note that there it no set method for the id and the type method is
         *  not implemented.
         *  The type should be constant for a particular class, and the id
         *  should be initialised in the constructor and never changed.
	     */
	    AbstractCallEntry(
            unsigned long id,
            const std::string& source,
            const std::string& information);

        /**
         *  Gets the source of the call.
         */
        virtual std::string getSource() const;

        /**
	     *  Sets the souce of the call.
	     */
	    virtual void setSource(const std::string& source);

        /**
         *  Gets the additional information that the call banner displays, as
         *  a string.
         */
	    virtual std::string getInformation() const;

        /**
	     *  Sets the additional information that the call banner displays
         *  about this call entry.
	     */
	    virtual void setInformation(const std::string& information);


        /**
         *  Gets the id of this call entry. This id must be unique to all
         *  calls.
         *  @see AbstractCallEntryFactory::getUniqueCallEntryId()
         */
	    virtual unsigned long getId() const;

        /**
         * @return true if this is an emergency call
         */
        virtual bool isEmergency() const;

        /**
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate() =0;


        /**
         *  This method is called to retrieve a list of menu items to be displayed in a context menu
		 *	@return	a list of strings, each string representing a menu entry
         */
		virtual std::vector<std::string> getContextMenuItems();


        /**
         *  This method is called when a ContextMenu item was selected.
		 *	@return	a list of strings, each string representing a menu entry
         */
		virtual void onContextMenuCommand(std::string command);
    
        
        /** 
         * Pure virtual implementation
         */
        virtual std::string getTimeStr() const;
        
        /**
         * @return creation time for this call, used by getTimeStr
         */
        virtual time_t getCreateTime() const = 0;

		//TD 15643
		//zhou yuan++
		virtual bool isSoundClosed() {return m_isSoundClosed;}

		virtual void closeSound(){ m_isSoundClosed =true; }
		//++zhou yuan

    private:

        // Disable default constructor.
        AbstractCallEntry();


	    std::string m_source;
	    std::string m_information;
	    unsigned long m_id;
		
		//TD 15643
		//zhou yuan++
		bool m_isSoundClosed;
		//++zhou yuan

    };

}// TA_IRS_Bus


#endif // !defined(AbstractCallEntry_FC2ED4EE_5CEA_469b_9432_09BA702CBAA6__INCLUDED_)