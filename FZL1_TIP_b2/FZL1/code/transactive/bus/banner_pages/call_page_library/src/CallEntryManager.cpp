/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/banner_pages/call_page_library/src/CallEntryManager.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/26 13:52:38 $
  * Last modified by:  $Author: builder $
  * 
  * This class is reponsible for managing the memory for the CallEntry
  * objects that are created by the AbstractCallEntryFactory subclasses.
  * It can be interrogated for all entries, all entries of a particular
  * type or a single entry.
  */

// Disable "identifier truncated" warning
#if defined(_MSC_VER)
#pragma warning(disable:4786 4250 4284)
#endif // _MSC_VER


#include "bus/banner_pages/call_page_library/src/AbstractCallEntry.h"
#include "bus/banner_pages/call_page_library/src/AbstractCallEntryFactory.h"
#include "bus/banner_pages/call_page_library/src/CallEntryManager.h"
#include "bus/banner_pages/call_page_library/src/ICallBanner.h"
#include "bus/banner_pages/call_page_library/src/CallEntryDoesNotExistException.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallFactory.h"
#include "bus/trains/PECCallBannerLibrary/src/PECCallEntry.h"
//#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
//#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
//#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"
//#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
#include "bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.h"
#include "bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
//#include "bus/radio/RadioEntityAccess/src/RadioEntityAccess.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/SoundPlayerException.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_IRS_Bus
{
    CallEntryManager::CallEntryManager(
        ICallBanner& callBanner, unsigned long locationKey)
        : m_callBanner(callBanner)
    {
       	initialiseCallFactories(locationKey); //td19480
    }


    CallEntryManager::~CallEntryManager()
    {
        // Stop any active sound manager items
        resetSoundPriorities();

		//TD18614
        //
        // Clean up the memory for the call entry factories
        //
        for(unsigned int i = 0; i < m_callFactories.size(); i ++)
        {
            delete m_callFactories[i];
            m_callFactories[i] = NULL;
        }
    }


    void CallEntryManager::initialiseCallFactories(unsigned long locationKey)
    {
		long tisSessionKey = 0; // TODO

		try
		{
			AbstractCallEntryFactory* PECFactory =
				new PECCallFactory(*this);

			//AbstractCallEntryFactory* authorisationFactory =
			//	new AuthorisationCallFactory(*this, locationKey);

   //         // remove session key/reference awareness in gui
   //         AbstractCallEntryFactory* radioFactory =
   //             new RadioCallFactory(*this, locationKey);

			// TODO - check that this is the OCC - RATIS calls are not processed at stations
			AbstractCallEntryFactory* ratisFactory =
				new RATISCallFactory(*this, locationKey, tisSessionKey);

			m_callFactories.push_back(PECFactory);
			//m_callFactories.push_back(authorisationFactory);
			//m_callFactories.push_back(radioFactory);
 			m_callFactories.push_back(ratisFactory);
		}
		catch (...) 
		{
		}
		
    }

        
    unsigned long CallEntryManager::getConsoleKey(unsigned long locationKey)
    {
        FUNCTION_ENTRY("getConsoleKey()");
		FUNCTION_EXIT;
		//return RadioEntityAccess::getConsoleKey();
		return 0;
	}

    std::string CallEntryManager::getRadioSessionEntityName(unsigned long locationKey,unsigned long consoleKey)
    {
        FUNCTION_ENTRY("getRadioSessionEntityName()");
		FUNCTION_EXIT;
		//return RadioEntityAccess::getRadioSessionEntityName(locationKey, consoleKey);
		return 0;
    }

    void CallEntryManager::addCallEntry(ICallEntry* call)
    {
        //
        // Save the new call and inform the call banner that it has been
        // added.
        //
		LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
			"Entry: addCallEntry()");
		
        {
            TA_ASSERT(0 != call, "Call Entry is NULL");

            ThreadGuard lock(m_callEntriesLock);
            m_callEntries.push_back(call);
            
			try
			{
				registerCallSeverity(*call);
			}
			catch ( ... )
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "caught exption when registering call severity, callid= %d, category = %s", call->getId(),call->getCategory().c_str());
			}
        }

        m_callBanner.addCall( *call );
    }


    void CallEntryManager::refreshCallEntry(unsigned long callId)
    {
        //
        // Simply tell the call banner to refresh the call entry.
        //
        m_callBanner.refreshCall( getCallEntry(callId) );
    }


    void CallEntryManager::activateCallEntry(unsigned long callId)
    {
        //
        // Simply tell the call banner to refresh the call entry.
        //
        m_callBanner.activateCall( getCallEntry(callId) );
    }


    void CallEntryManager::deleteCallEntry(unsigned long callId, const std::string& type)
    {
        //
        // Go through the list and find the CallEntry with the given id.
        // Once found, delete the pointer and remove the entry from the list,
        // then return.
        //
        ICallEntry* callEntry = NULL;

        {
            ThreadGuard lock(m_callEntriesLock);

            for( CallEntriesList::iterator it = m_callEntries.begin();
                 m_callEntries.end() != it; ++it )
            {
                TA_ASSERT( NULL != *it, "Call entry should never be NULL");

                // check the id and type matches
                if ( ( (*it)->getId() == callId ) &&
                     ( (*it)->getType() == type ) )
                {
                    // take a copy of the pointer
                    callEntry = *it;
                    
                    // stop the sound.
                    if( false == callEntry->isSoundClosed() )
					{
                        try
					    {
                            deregisterCallSeverity( *callEntry );
    				    }
					    catch ( ... )
					    {
						    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                         "caught exceptions when deregister call severity,callid=%d, type is %s ",
                                         callId, type.c_str() );
					    }
					}

                    // remove the item from the list
                    m_callEntries.erase( it );

                    // exit the loop, found
                    break;
                }
            }
        
        }// Unlock

        // if it was found
        if( NULL != callEntry )
        {
            //
            // Inform the call banner that the call entry has been removed
            //
            m_callBanner.removeCall( callId );

            // delete the entry
            delete callEntry;
            callEntry = NULL;
        }
        else
        {
            //
            // If the callId was not found in the list, throw an exception.
            //
            TA_THROW( CallEntryDoesNotExistException() );
        }
    }


    std::vector<ICallEntry*> CallEntryManager::getAllCallEntries()
    {
        ThreadGuard lock(m_callEntriesLock);

        //
        // Convert the call entries to a vector and return it.
        //
        std::vector<ICallEntry*> entryVector;
        entryVector.reserve( m_callEntries.size() );

        for(CallEntriesList::const_iterator it = m_callEntries.begin();
            it != m_callEntries.end();
            it ++)
        {
            ICallEntry* callEntry = *it;
            TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

            entryVector.push_back(callEntry);
        }
        
        return entryVector;
    }


    unsigned int CallEntryManager::getNumberOfCallEntriesOfType(
        const std::string& type)
    {
        ThreadGuard lock(m_callEntriesLock);

        //
        // Go through the list and find call entries with the given type,
        // incrementing the counter.
        //
        unsigned int count = 0;

        for(CallEntriesList::const_iterator it = m_callEntries.begin();
            it != m_callEntries.end();
            it ++)
        {
            ICallEntry* callEntry = *it;
            TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

            if( callEntry->getType() == type )
            {
                count ++;
            }
        }

        return count;
    }

        
    std::vector<ICallEntry*> CallEntryManager::getCallEntriesOfType(
        const std::vector<std::string>& callTypes)
    {
        ThreadGuard lock(m_callEntriesLock);

        //
        // Go through the list and find call entries that match one
        // of the the given types, adding them to the vector.
        // Note that the order must be maintained so no sorting is
        // done to speed up the process. There shouldn't be huge numbers
        // of calls anyway.
        //
        std::vector<ICallEntry*> entryVector;

        for(CallEntriesList::const_iterator it = m_callEntries.begin();
            it != m_callEntries.end();
            it ++)
        {
            ICallEntry* callEntry = *it;
            TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

            for(unsigned int i = 0; i < callTypes.size(); i ++)
            {
                std::string type = callTypes[i];
                if( callEntry->getType() == type )
                {
                    entryVector.push_back(callEntry);
                }
            }
        }

        return entryVector;
    }


    ICallEntry& CallEntryManager::getCallEntry(unsigned long callId)
    {
        ThreadGuard lock(m_callEntriesLock);

        //
        // Go through the list and find the CallEntry with the given id, and
        // return a reference to it.
        //
        for(CallEntriesList::iterator it = m_callEntries.begin();
            it != m_callEntries.end();
            it ++)
        {
            ICallEntry* callEntry = *it;
            TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

            if( callEntry->getId() == callId )
            {
                return *callEntry;
            }
        }


        //
        // If the callId was found in the list the method would have returned,
        // so throw an exception.
        //
        TA_THROW( CallEntryDoesNotExistException() );
    }

    
    void CallEntryManager::registerCallSeverity(const ICallEntry& call)
    {
        std::string category = call.getCategory();
        int newSeverity = call.getSeverity();

		ThreadGuard guard(m_callSeverityGroupsLock);

		if (m_callEntries.size() == 1 || m_currentCategory.empty()) // no other call entires (this is the first)
		{
			m_currentCategory = category;
            refreshSoundPriority(category, newSeverity);
		}
		else
		{
			int currentSeverity = *m_callSeverityGroups[m_currentCategory].rbegin();

			std::ostringstream st;
			st << "[SMEE] Current Severity = " << currentSeverity << ", New Severity = " << newSeverity
				<< " Current m_currentCategory = " << m_currentCategory << ", Category = " << category;
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, st.str().c_str());

            if (newSeverity < currentSeverity)
            {
				// stop the current category
				refreshSoundPriority(m_currentCategory, 0);
                // Forward notification to sound manager of the new, higher severity
                refreshSoundPriority(category, newSeverity);

				m_currentCategory = category;
            }

		}

        // Update internal map
        m_callSeverityGroups[category].insert(newSeverity);
    }


    void CallEntryManager::deregisterCallSeverity(const ICallEntry& call)
    {
        std::string category = call.getCategory();

		ThreadGuard guard(m_callSeverityGroupsLock);

        CallSeverityGroups::iterator itrGroup = m_callSeverityGroups.find(category);

        bool removedOK = false;

        // A call with the specified category has not been registered
        if (m_callSeverityGroups.end() != itrGroup)
        {
            CallSeverities& severitySet = (itrGroup->second);

            CallSeverities::iterator itr = severitySet.find(call.getSeverity());

            // This severity hasn't already been registered - if severity changes
            // during the lifetime of an object, then this will happen
            if (severitySet.end() != itr)
            {               
                int prevHighestSeverity = *(severitySet.rbegin());

                severitySet.erase(itr);
                removedOK = true;

                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Remove severity in the category = %s, \
					size of severitySet = %d",category.c_str(), severitySet.size());

				if (m_currentCategory == category)
				{
					// Revert to a severity of zero, unless other items remain in set
					int highestRemainingSeverity = 0;                
					if (severitySet.size() > 0)
					{
						highestRemainingSeverity = *(severitySet.rbegin());
					}

					// Refresh sound manager if there has been a change in highest severity
					if (prevHighestSeverity != highestRemainingSeverity)
					{
						LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Refresh Sound Priority = %d, \
							category = %s",highestRemainingSeverity, category.c_str());

						//TD15687
						//stop the current category 
						refreshSoundPriority(category, highestRemainingSeverity);

						//if the category has no more call Entry. search the next catagory by priority order.
						if (highestRemainingSeverity == 0) // play the next highest category in the CallEntries
						{
							if (m_callSeverityGroups[PECCallEntry::CALL_ENTRY_TYPE].size() > 0)
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Refresh Sound Priority = %d, \
									category = PECCallEntry::CALL_ENTRY_TYPE",highestRemainingSeverity);
								refreshSoundPriority(PECCallEntry::CALL_ENTRY_TYPE, 1);
								m_currentCategory = PECCallEntry::CALL_ENTRY_TYPE;
							}
							//else if (m_callSeverityGroups[RadioCallEntry::CALL_ENTRY_TYPE].size() > 0)
							//{
							//	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Refresh Sound Priority = %d, \
							//		category = RadioCallEntry::CALL_ENTRY_TYPE",highestRemainingSeverity);
							//	refreshSoundPriority(RadioCallEntry::CALL_ENTRY_TYPE, 2);
							//	m_currentCategory = RadioCallEntry::CALL_ENTRY_TYPE;
							//}
							else if (m_callSeverityGroups[RATISCallEntry::CALL_ENTRY_TYPE].size() > 0)
							{
								LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Refresh Sound Priority = %d, \
									category = RATISCallEntry::CALL_ENTRY_TYPE",highestRemainingSeverity);
								refreshSoundPriority(RATISCallEntry::CALL_ENTRY_TYPE, 3);
								m_currentCategory = RATISCallEntry::CALL_ENTRY_TYPE;
							}
							//else if (m_callSeverityGroups[AuthorisationCallEntry::CALL_ENTRY_TYPE].size() > 0)
							//{

							//	LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "[SMEE] Refresh Sound Priority = %d, \
							//		category = PECCallEntry::CALL_ENTRY_TYPE",highestRemainingSeverity);
							//	refreshSoundPriority(AuthorisationCallEntry::CALL_ENTRY_TYPE, 4);
							//	m_currentCategory = AuthorisationCallEntry::CALL_ENTRY_TYPE;
							//}
							else	//nothing left.
							{
								m_currentCategory = "";
							}
						}
						//TD15687
					}
				}
            }
        }
        

        if (!removedOK)
        {
            LOG(SourceInfo, 
                TA_Base_Core::DebugUtil::GenericLog, 
                TA_Base_Core::DebugUtil::DebugError, 
				"Couldn't find severity to remove in deregisterCallSeverity!");

            // Reset the call severity register from scratch, some kind of internal error
            // (should be able to recover OK, but the severity shouldn't have changed!)
            resetSoundPriorities();
            
            for(CallEntriesList::iterator it = m_callEntries.begin();
                    it != m_callEntries.end();
                    ++it )
            {
                ICallEntry* callEntry = *it;

                if (callEntry == &call)
                {
                    // Don't add call we're removing
                    continue;
                }
                TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

                registerCallSeverity(*callEntry);
            }
        }
    }


    void CallEntryManager::resetSoundPriorities()
    {
		ThreadGuard guard(m_callSeverityGroupsLock);
        for (CallSeverityGroups::iterator itr = m_callSeverityGroups.begin(); 
                itr != m_callSeverityGroups.end(); 
                ++itr )
        {
            // Reset all severities to silence alarms
            refreshSoundPriority(itr->first, 0);
        }
        
        m_callSeverityGroups.clear();
    }


    void CallEntryManager::refreshSoundPriority(const std::string& category, int priority)
    {
        // Forward to Sound Manager interface
        try
        {
			TA_Base_Bus::SoundManagerProxy soundManager(0);
            soundManager.setPriority(category, priority);
        }
    	catch(TA_Base_Core::SoundPlayerException& ex)
    	{
    		std::string error = "SoundPlayerException while attempting to set priority for category \"";
			error += category;
			error += "\" : ";
    		error += ex.what();
    		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
    	}
        catch (...)
        {
            std::ostringstream ostr;
            ostr << "Sound Manager exception setting priority for category '" << category << "'";
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", ostr.str().c_str());
        }

    }
            
	//TD 15643
	//zhou yuan++
	void CallEntryManager::closeSound(const ICallEntry& call)
	{
        FUNCTION_ENTRY("closeSound()");
		deregisterCallSeverity(call);
        FUNCTION_EXIT;
	}
	//++zhou yuan

}// TA_IRS_Bus

