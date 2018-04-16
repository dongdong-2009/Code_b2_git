/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Classes that extend this abstract class are used to create ICallEntry
  * objects. The extending class should listen for events from the system,
  * and inform the CallEntryManager class when a Call Entry is created
  * or deleted.
  *
  * For example when a radio call is initiated, the RadioCallEntryFactory
  * creates a RadioCallEntry using new, and passes it to the CallEntryManager's
  * addCallEntryManager(ICallEntry*) method.
  *
  * The CallEntryManager takes responsibility for deleting the object when
  * informed by the RadioCallEntryFactory with the
  * deleteCallEntry(unsigned long callId) method.
  *
  * This means AbstractCallEntryFactory subclasses must be able to retrieve
  * the id for a given call. The saveCallId and retrieveCallId methods may
  * be used to facilitate this.
  */


#if !defined(AbstractCallEntryFactory_BB9DF2B3_9CA7_40d1_9DAA_AE1926151022__INCLUDED_)
#define AbstractCallEntryFactory_BB9DF2B3_9CA7_40d1_9DAA_AE1926151022__INCLUDED_

#include <map>
#include <string>
#include "bus/banner_pages/call_page_library/src/stdafx.h"

namespace TA_IRS_Bus
{
    class CallEntryManager;

    /**
     * @version 1.0
     * @created 10-Feb-2004 10:54:53 AM
     */
    class AbstractCallEntryFactory
    {

    public:

        /**
         *  The constructor.
         */
	    AbstractCallEntryFactory();

        /**
         *  The destructor.
         */
        virtual ~AbstractCallEntryFactory();


    protected:

        /**
         *  Gets an identifier that is unique for all call entries.
         */
        static unsigned long getUniqueCallEntryId();


        /**
         *  Saves a call entry id, associating it with an identifier that is
         *  unique to the subclass call type. This enables it to be retrieved
         *  later using the retrieveCallId method.
         *  For example
         *  saveCallId("abcd", "1234")
         *  retrieveCallId("abcd")
         *
         *  @param typeUniqueId  an identifier that is unique to this call
         *                       type, eg the radio call's id.
         *  @param callEntryId   the identifier that is unique to all types,
         *                       that was returned from getUniqueCallEntryId
         */
        void saveCallId(
            const std::string& typeUniqueId,
            unsigned long callEntryId);


        /**
         *  Retrieves a call entry id that is associated with an identifier
         *  that is unique to the subclass call type.
         *  @see saveCallId
         *
         *  @param typeUniqueId  an identifier that is unique to this type,
         *                       eg the radio call's id.
         *
         *  @return  the identifier that was stored
         */
        unsigned long retrieveCallId(const std::string& typeUniqueId);


        /**
         *  Deletes a call entry id that is associated with an identifier that
         *  is unique to the subclass call type.
         *  @see saveCallId
         *
         *  @param typeUniqueId  an identifier that is unique to this type,
         *                       eg the radio call's id.
         *
         *  If the callType/typeUniqueId does not exist, the method call does
         *  nothing.
         */
        void deleteCallId(const std::string& typeUniqueId);


        /**
         *  Indicates if there is a call entry id that is associated with the
         *  given type-unique identifier.
         *  @see saveCallId
         *
         *  @param typeUniqueId  an identifier that is unique to this type,
         *                       eg the radio call's id.
         *
         */
        bool doesCallIdExist(const std::string& typeUniqueId);


        /**
         *  Gets the given number as a string.
         */
        static std::string getUnsignedLongAsString(
            unsigned long number);


        /**
         *  Gets the given string as a number.
         */
        static unsigned long getStringAsUnsignedLong(
            const std::string& theString);

    protected:

        void clearAllEntries(const std::string & type, CallEntryManager & callEntryManager);


    private:

        AbstractCallEntryFactory(const AbstractCallEntryFactory&);

        // The identifier that is unique to all call entries.
        // This wraps around when it reaches the maximum value for an unsigned
        // long
        static unsigned long s_uniqueCallEntryId;


        // A map of identifiers that are unique to a call type to identifiers
        // that are unique across all call entries
        typedef std::map<std::string, unsigned long> IdMap;
        IdMap m_callIdMap;
    };

}// TA_IRS_Bus


#endif // !defined(AbstractCallEntryFactory_BB9DF2B3_9CA7_40d1_9DAA_AE1926151022__INCLUDED_)