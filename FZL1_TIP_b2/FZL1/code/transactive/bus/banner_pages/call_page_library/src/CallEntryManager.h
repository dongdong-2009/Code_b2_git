/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/banner_pages/call_page_library/src/CallEntryManager.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class is reponsible for managing the memory for the CallEntry
  * objects that are created by the AbstractCallEntryFactory subclasses.
  * It can be interrogated for all entries, all entries of a particular
  * type or a single entry.
  */


#if !defined(CallEntryManager_4F09C4B7_E07A_401f_BC99_30D718B30B73__INCLUDED_)
#define CallEntryManager_4F09C4B7_E07A_401f_BC99_30D718B30B73__INCLUDED_

#include <list>
#include <vector>
#include <set>

#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/naming/src/NamedObject.h"
#include "bus/radio/RadioAgentCorbaDef_4669/idl/src/IRadioSessionCorbaDef.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"

namespace TA_IRS_Bus
{

    // Forward declarations
    class AbstractCallEntryFactory;
    class ICallBanner;
    class ICallEntry;
    class SoundManagerProxy;



    /**
     * @version 1.0
     * @created 10-Feb-2004 10:54:23 AM
     */
    class CallEntryManager
    {

    public:

		//TD 15643
		//zhou yuan++
		/**
		  * closeSound
		  * 
		  * close the ICallEntity related sound
		  * 
		  * @return void 
		  * @param : const ICallEntry& call
		  * 
		  * @exception <exceptions> Optional
		  */
		void closeSound(const ICallEntry& call);
		//++zhou yuan

        /**
         *  Creates a new Call Entry Manager, that manages call entries for
         *  the given call banner at the given location.
         */
        CallEntryManager(ICallBanner& callBanner, unsigned long locationKey);


	    /**
         *  Adds the given call entry to the list maintained by this object.
         *  The CallEntryManager is responsible for the memory of the
         *  ICallEntry object, so it must be created with a call to new.
	     */
	    virtual void addCallEntry(ICallEntry* call);


	    /**
	     *  Deletes the call entry with the given Id from the list maintained
         *  by this object. Tha call is only deleted if it is of the specified type,
         *  This prevents say the radio factory from deleting PEC calls when it shouldnt.
         *
         *  @exception CallEntryDoesNotExistException
         *             thrown if there is no call entry with the given id.
	     */
	    virtual void deleteCallEntry(unsigned long callId, const std::string& type);


        /**
	     *  Refreshes the call entry with the given Id in the call banner,
         *  reloading its fields from the object.
         *
         *  @exception CallEntryDoesNotExistException
         *             thrown if there is no call entry with the given id.
	     */
	    virtual void refreshCallEntry(unsigned long callId);


        /**
	     *  Activates the call entry with the given Id in the call banner,
         *  reloading its fields from the object.
         *
         *  @exception CallEntryDoesNotExistException
         *             thrown if there is no call entry with the given id.
	     */
	    virtual void activateCallEntry(unsigned long callId);


        /**
         *  Gets all call entries that this object knows about.
         *  Note that this class is responsible for cleaning up the memory.
         */
        virtual std::vector<ICallEntry*> getAllCallEntries();


        /**
	     *  Gets the number of call entries of the given type that this object
         *  currently holds.
	     */
        unsigned int getNumberOfCallEntriesOfType(const std::string& type);


        /**
	     *  Gets all call entries of the given types from the list maintained
         *  by this object. The call entries are returned in the reverse order
         *  that they were received by the CallEntryManager.
         *
         *  eg if the calls were added in this order:
         *  "Authorisation", "PEC", "RXH", "Authorisation", "RXH"
         *  they will be returned in this order:
         *  "RXH", "Authorisation", "RXH", "PEC", "Authorisation"
         *
         *  Note that this class is responsible for cleaning up the memory.
	     */
        virtual std::vector<ICallEntry*> getCallEntriesOfType(
            const std::vector<std::string>& callTypes);


        /**
         *  Gets the call entry with the given id from the list maintained by
         *  this object.
         *
         *  @exception CallEntryDoesNotExistException
         *             thrown if there is no call entry with the given id.
         */
	    virtual ICallEntry& getCallEntry(unsigned long callId);


        /**
         *  The destructor.
         */
        virtual ~CallEntryManager();

	    typedef TA_Base_Core::NamedObject<
            TA_Base_Bus::IRadioSessionCorbaDef,
			TA_Base_Bus::IRadioSessionCorbaDef_ptr,
			TA_Base_Bus::IRadioSessionCorbaDef_var>
            IRadioSession;



    protected:

        /**
         *  Creates and sets up all the call factories that communicate with
         *  the call banner.
         *
         *  @param locationKey  the location of the banner
         */
        void initialiseCallFactories(unsigned long locationKey);

        /**
         *  Gets the radio session key, given the location of the banner.
         *  Assumes that the RPARAM_SESSIONID run parameter is set.
         *
         *  @param locationKey  the location of the banner
         */
        long getRadioSessionKey(unsigned long locationKey);
        
        /**
         *  Gets the console key, given the location of the banner.
         *  Assumes that the RPARAM_SESSIONID run parameter is set.
         *
         *  @param locationKey  the location of the banner
         */
        unsigned long getConsoleKey(unsigned long locationKey);


        /**
         *  Retrieves the radio session entity name, given the location of the
         *  banner and the console on which it is being run.
         *
         *  @param locationKey  the location of the banner
         *  @param consoleKey   the console on which the banner is being run
         */
        std::string getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey);

    private:
	    CallEntryManager();
	    CallEntryManager(const CallEntryManager& theCallEntryManager);

        /**
         * registerCallSeverity registers a calls severity details internally, 
         *  in order to update the sound manager
         */
        void registerCallSeverity(const ICallEntry& call);

        /**
         * deregisterCallSeverity deregisters a call previous registered in registerCallSeverity
         */
        void deregisterCallSeverity(const ICallEntry& call);

        /**
         * resetSoundPriorities resets all calls registered with registerCallSeverity
         */
        void resetSoundPriorities();

        /**
         * Used to forward sound priority notifications to Sound Manager interface
         */
        void refreshSoundPriority(const std::string& category, int priority);
        
        // The list of call entries maintained by this object
        typedef std::list<ICallEntry*> CallEntriesList;
        CallEntriesList m_callEntries;

        // The call banner that this object manages call entries for.
        ICallBanner& m_callBanner;

        // The vector of call factories that the CallEntryManager manages
        // call entries for.
        typedef std::vector<AbstractCallEntryFactory*> CallFactoriesVector;
        CallFactoriesVector m_callFactories;

        // This lock is acquired when the call entries are being accessed
        // or modified
        TA_Base_Core::ReEntrantThreadLockable m_callEntriesLock;
		TA_Base_Core::ReEntrantThreadLockable m_callSeverityGroupsLock;

        // Must use a multiset, because mutiple calls with severity will need to be registed
        typedef std::multiset<int> CallSeverities;
        typedef std::map<std::string, CallSeverities> CallSeverityGroups;
        CallSeverityGroups  m_callSeverityGroups;

		std::string m_currentCategory; // TD15687 - current category playing
    };

}// TA_IRS_Bus

#endif // !defined(CallEntryManager_4F09C4B7_E07A_401f_BC99_30D718B30B73__INCLUDED_)

