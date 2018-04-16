/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/banner_pages/call_page_library/src/CallEntryManager.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallFactory.h"
#include "bus/radio/RadioCallBannerLibrary/src/RadioCallEntry.h"
//#include "bus/telephony/TelephoneCallBannerLibrary/src/TelephoneCallFactory.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallFactory.h"
/*#include "bus/signs/RATISCallBannerLibrary/src/RATISCallFactory.h"
#include "bus/signs/RATISCallBannerLibrary/src/RATISCallEntry.h"*/
#include "bus/sound/sound_client/src/SoundManagerProxy.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
/*#include "core/data_access_interface/entity_access/src/RadioSessionEntityData.h"
#include "core/data_access_interface/entity_access/src/IRadioSessionEntityData.h"
*/
#include "core/exceptions/src/SoundPlayerException.h"

using TA_Base_Core::ThreadGuard;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::IConsole;
using TA_Base_Core::ConsoleAccessFactory;
using TA_Base_Core::IEntityData;
using TA_Base_Core::EntityAccessFactory;
//using TA_Base_Core::RadioSessionEntityData;
//using TA_Base_Core::IRadioSessionEntityData;



namespace TA_IRS_Bus
{
    CallEntryManager::CallEntryManager(
        ICallBanner& callBanner, unsigned long locationKey)
        : m_callBanner(callBanner)
		, m_soundManager(0)
		, m_radioCallFactory(NULL)
		, m_semapore(0)
		, m_bTerminate(false)
    {
        initialiseCallFactories(locationKey);
		start();
    }


    CallEntryManager::~CallEntryManager()
    {
        // Stop any active sound manager items
        resetSoundPriorities();

        //
        // Clean up the memory for the call entry factories
        //
        for(unsigned int i = 0; i < m_callFactories.size(); i ++)
        {
            delete m_callFactories[i];
            m_callFactories[i] = NULL;
        }

		terminateAndWait();
    }


    void CallEntryManager::initialiseCallFactories(unsigned long locationKey)
    {
        long radioSessionKey = 0; //getRadioSessionKey(locationKey);

		long tisSessionKey = 0; // TODO

		/*
		 * Incoming Private calls
		 */

		m_radioCallFactory = 
            new RadioCallFactory(*this, locationKey, radioSessionKey);

		m_callFactories.push_back(m_radioCallFactory);

		/*
		 * Incoming SDS messages
		 * Fixed TD#14311
		 */
        AbstractCallEntryFactory* sdsFactory =
            new AuthorisationCallFactory(*this, locationKey);
		
		//TD18743
		m_callFactories.push_back(sdsFactory);
		//TD18743


		AbstractCallEntryFactory* PECFactory =
				new PECCallFactory(*this);
		m_callFactories.push_back(PECFactory);
    }


    long CallEntryManager::getRadioSessionKey(unsigned long locationKey)
    {
        return getRadioSessionKey(locationKey, getConsoleKey(locationKey));
    }

        
    unsigned long CallEntryManager::getConsoleKey(unsigned long locationKey)
    {
	    unsigned long consoleKey = 0;

        //
        // Get the console key from the session id
        //
        std::string sessionId =
            TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");

	    IConsole* console =
            ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
        try
        {
	        consoleKey = console->getKey();
	        delete console;
            console = NULL;
        }
        catch(...)
        {
	        delete console;
            console = NULL;
            throw;
        }

        return consoleKey;
    }


    long CallEntryManager::getRadioSessionKey(
        unsigned long locationKey, unsigned long consoleKey)
    {
        return 0;
    }
    

    void CallEntryManager::addCallEntry(ICallEntry* call)
    {
        //
        // Save the new call and inform the call banner that it has been
        // added.
        //
        {
            TA_ASSERT(0 != call, "Call Entry is NULL");

            ThreadGuard lock(m_callEntriesLock);
            m_callEntries.push_back(call);
            
            registerCallSeverity(*call);
        }

		//Make sure that it is thread safe
		{
			ThreadGuard lock(m_callBannerLock);
			m_callBanner.addCall( *call );
		}
    }


    void CallEntryManager::refreshCallEntry(unsigned long callId)
    {
        //
        // Simply tell the call banner to refresh the call entry.
        //
        m_callBanner.refreshCall( getCallEntry(callId) );
    }


    void CallEntryManager::deleteCallEntry(unsigned long callId, const std::string& type)
    {
        //
        // Go through the list and find the CallEntry with the given id.
        // Once found, delete the pointer and remove the entry from the list,
        // then return.
        //
        bool found;
        {
            ThreadGuard lock(m_callEntriesLock);

            // Set found after the thread has been locked, so that
            // another thread doesn't reset it while waiting for the lock
            found = false;

			LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
				"A Call Factory of type %s is trying to delete a call  %d.",
				type.c_str(), callId );
            for(CallEntriesList::iterator it = m_callEntries.begin();
                it != m_callEntries.end() ; it++ )
                
            {
                ICallEntry* callEntry = *it;
                TA_ASSERT(callEntry != NULL, "Call entry should never be NULL");

                if( callEntry->getId() == callId ) 
                {
                    if ( callEntry->getType() == type )
                    {
						LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
							"A Call Factory of type %s is trying to deregisterservity a call  %d.",
							type.c_str(), callId );
                        deregisterCallSeverity(*callEntry);
						LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
							"A Call Factory of type %s has deregisterservity a call  %d.",
							type.c_str(), callId );

                        delete (*it);
                         it=m_callEntries.erase( it);
						 found = true ;
                       
                    }
                    else
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
                            "A Call Factory of type %s is trying to delete a call of type %s.",
                            type.c_str(), callEntry->getType().c_str());
						//break ;
                    }
					 break;
                }
				else
				{
					//it++ ;
				}
            }
        
        }// Unlock

		LOG_GENERIC( SourceInfo, DebugUtil::DebugWarn,
			"A Call Factory of type %s has deleted a call  %d.",
			type.c_str(), callId );

        if(found)
        {
			//Make sure that it is thread safe
			 ThreadGuard lock(m_callBannerLock);
            //
            // Inform the call banner that the call entry has been removed
            //
            m_callBanner.removeCall(callId);
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

			//Count the number of objects for RadioCallEntry + CallDriverCallEntry
			if ( ( ! callEntry->getType().compare("Train") || 
				   ! callEntry->getType().compare("Radio"))&&
				 ( ! type.compare("Train") || ! type.compare("Radio") ) )
			{
				count++;
			}
			//For PECCallEntry counter
			else if( ! callEntry->getType().compare("PEC") && 
				     ! type.compare(PECCallEntry::CALL_ENTRY_TYPE) )
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

	void CallEntryManager::removeRadioCallEntry(std::string callId)
	{
		FUNCTION_ENTRY("removeRadioCallEntry");

		if ( NULL != m_radioCallFactory ) {
			unsigned long entryId = m_radioCallFactory->removeRadioCallEntry(callId);

			if ( entryId ) {
				deleteCallEntry(entryId,RadioCallEntry::CALL_ENTRY_TYPE);
			} else {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Call Id %s not exist on radio call entries.", callId.c_str());
			}
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio Call Factory not initialized");
		}

		FUNCTION_EXIT;
	}

    
    void CallEntryManager::registerCallSeverity(const ICallEntry& call)
    {
        std::string category = call.getCategory();
        int newSeverity = call.getSeverity();
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
			"Register Call Severity : %d, Category : %s",newSeverity,category.c_str());
		

        if (m_callSeverityGroups[category].size() > 0)
        {
         	int currentSeverity = *m_callSeverityGroups[category].rbegin();

            if (newSeverity > currentSeverity)
            {
                // Forward notification to sound manager of the new, higher severity
                refreshSoundPriority(category, newSeverity);
            }
        }
        else
        {
            //Otherwise no other severities active in this category (this is first)
            refreshSoundPriority(category, newSeverity);
        }

        // Update internal map
        m_callSeverityGroups[category].insert(newSeverity);
    }


    void CallEntryManager::deregisterCallSeverity(const ICallEntry& call)
    {
        std::string category = call.getCategory();

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
                
                // Revert to a severity of zero, unless other items remain in set
                int highestRemainingSeverity = 0;                
                if (severitySet.size() > 0)
                {
                    highestRemainingSeverity = *(severitySet.rbegin());
                }

                // Refresh sound manager if there has been a change in highest severity
                if (prevHighestSeverity != highestRemainingSeverity)
                {
                    refreshSoundPriority(category, highestRemainingSeverity);
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
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "refreshSoundPriority");
		TA_Base_Core::ThreadGuard guard(m_soundQueuLock);

		T_CategorySev objItem = std::make_pair<std::string, int>(category, priority);
		m_soundPriorityQueue.push(objItem);
		m_semapore.post();
    }
	
	void CallEntryManager::closeSound (ICallEntry * callEntry)
	{
		FUNCTION_ENTRY("closeSound");
		
		if (callEntry)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Closing Sound");
			deregisterCallSeverity(*callEntry);
		}
		
		FUNCTION_EXIT;
	}

    void CallEntryManager::performLogout()
    {

        /*try
        {
            std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
            // Not interested if the sessionid parameter was not set here, as the
            // assert will have happenned MUCH earlier if that was the case
            // TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");

            //
            // Get the radio session key from the Radio Session Corba def
            //
            IRadioSession radioSession;
            radioSession->sessionLogout(sessionId.c_str());
		}
		catch (...)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Performing logout. This is probably because another call banner page logged out first.");
        } */
    }

	void CallEntryManager::run()
	{
		while(!m_bTerminate)
		{
			m_semapore.wait();
			if (m_bTerminate)
				break;

			{
				TA_Base_Core::ThreadGuard guard(m_soundQueuLock);
				if (m_soundPriorityQueue.size() != 0)
				{
					T_CategorySev objItem = m_soundPriorityQueue.front();
					m_soundPriorityQueue.pop();

					setSoundPriority(objItem.first, objItem.second);
				}
			}
		}
	}

	void CallEntryManager::terminate()
	{
		m_bTerminate= true;
		m_semapore.post();
	}

	void CallEntryManager::setSoundPriority(const std::string& category, int priority)
	{
		try
		{
			if (!m_soundManager)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Sound Manager Init");

				m_soundManager = new TA_Base_Bus::SoundManagerProxy(0);

				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Sound Manager Created");
			}

		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "SoundPlayerException while constructing SoundManagerProxy: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}
		catch (...)
		{
			std::string error = "Unspecified exception while constructing SoundManager";
			LOG_EXCEPTION_CATCH(SourceInfo, "Unspecified", "Unspecified exception while constructing SoundManagerProxy");
		}

		// Forward to Sound Manager interface
		try
		{
			if (m_soundManager)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Setting Sound Priority to %d", priority);
				m_soundManager->setPriority(category, priority);

				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
					"Sound Priority Set");
			}
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


}// TA_IRS_Bus

