/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntry.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
         *  This method is called when the call entry is activated, for
         *  example when the user double-clicks on it.
         */
        virtual void onActivate() =0;
    
        
        /** 
         * Pure virtual implementation
         */
        virtual std::string getTimeStr() const;
        
        /**
         * @return creation time for this call, used by getTimeStr
         */
        virtual time_t getCreateTime() const = 0;

    private:

        // Disable default constructor.
        AbstractCallEntry();


	    std::string m_source;
	    std::string m_information;
	    unsigned long m_id;

    };

}// TA_IRS_Bus


#endif // !defined(AbstractCallEntry_FC2ED4EE_5CEA_469b_9432_09BA702CBAA6__INCLUDED_)