/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/pa/cachedmaps/src/GenericCachedMapBase.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Base class (non template) for the GenericCachedMap template class
  * Because template classes need to be compiled inline, this reduces
  * the load on the compiler (reducing need to specify /Zm compiler option)
  * by putting non template elements into a base class
  *
  */


#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include "bus/pa/cachedmaps/src/GenericCachedMapBase.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include <algorithm>

namespace TA_IRS_Bus
{

    GenericCachedMapBase::GenericCachedMapBase
    (
     const TA_Base_Core::EObjectType& databaseUpdateListenerType,
     const std::string& tableName
    )
    :
    m_databaseUpdateListenerType(databaseUpdateListenerType),
    m_tableName(tableName)
    {
    }


    GenericCachedMapBase::~GenericCachedMapBase()
    {
        stopListening();

        // All observers should be deregistered on exit
        TA_ASSERT(0 == m_registeredObservers.size(), "Observers remaining");
    }


    void GenericCachedMapBase::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        // Quickly modify the update listener if its a create or delete (before
        // applying local listener lock - since that lock is required by these functions)
        // NOTE: We no longer do this (OnlineUpdateListener does this manually,
        // @see OnlineUpdateListener::setListenOutForNewKeys)
        /*
        if (TA_Base_Core::Create == updateEvent.getModifications())
        {
            startListeningToAdditionalItem(updateEvent.getKey());   
        } 
        else if (TA_Base_Core::Delete == updateEvent.getModifications())
        {
            if (!stopListeningToItem(updateEvent.getKey()))
            {
                // We weren't already listening to the specified key
                // This occurs during shutdown (when deregistering interest)
                // and a few queued updates come through anyway..  just ignore them
                return;
            }
        }
        */

        // This function will block until the complete
        // (so may stall other processUpdate calls being sent to the zone change listener)
        TA_Base_Core::ThreadGuard guard(m_updateListenerLock);

        TA_ASSERT(m_databaseUpdateListenerType== updateEvent.getType(), "Invalid type encountered");
        bool changesMade = false;

        try
        {
            // [Maintains list of changed columns, we don't require] updateEvent.getChangedParams();
            switch (updateEvent.getModifications())
            {
            case TA_Base_Core::Update:
                // FALLTHROUGH
            case TA_Base_Core::Create:
                {                    
                    // set changesMade to false if we get an Update that 
                    // doesn't actually change the data (i.e. a redundant notification)
                    changesMade = processDatabaseUpdateCreateNotification(updateEvent);            
                }
                break;

            case TA_Base_Core::Delete:
                // Remove from database
                changesMade = processDatabaseRemovalNotification(updateEvent.getKey());
                break;
            default:
                TA_ASSERT(false, "Unexpected modification");
            }
        }
        catch (const TA_Base_Core::TransactiveException&)
        {
            onDatabaseUpdateProcessError(updateEvent.getKey());
            return;
        }


        // Send update notifications to all registered observers
        TA_Base_Core::ThreadGuard observerLock(m_updateObserverLock);
    
        for (unsigned int i = 0; i < m_registeredObservers.size(); i ++)
        {
            try
            {
                m_registeredObservers[i]->onCachedMapChanged(updateEvent.getKey(), changesMade);
            }
            catch (const TA_Base_Core::TransactiveException&)
            {
                onDatabaseUpdateProcessError(updateEvent.getKey());
            }
        }
    }

   
    void GenericCachedMapBase::startListening(std::vector<unsigned long> primaryKeys)
    {    
        TA_Base_Core::ThreadGuard guard(m_updateListenerLock);

        // Shouldn't already be registered when calling
        TA_ASSERT(!TA_Base_Core::OnlineUpdateListener::getInstance().isObjectTypeRegistered(
                    m_databaseUpdateListenerType, *this), "Already started listening");

        TA_Base_Core::OnlineUpdateListener::getInstance().registerInterests(m_databaseUpdateListenerType, 
                                                                *this, 
                                                                primaryKeys);
    }       
       

    void GenericCachedMapBase::startListening()
    {
        // Begin listening to changes made to objects with all stored keys
        // NOTE: We may miss some updates in the time taken to subscribe
        // to listener (from when we refreshed from database)
        // Ideally we can listen to all updates using a locationKey parameter
        // (or perhaps just all updates - which would work), in this way we
        // could start the listener first, and ensure up to date)
        startListening(getPrimaryKeyArray());
    }


    void GenericCachedMapBase::stopListening()
    {
        TA_Base_Core::ThreadGuard guard(m_updateListenerLock);

        // only deregister if already registered (so function safe to call if
        // not currently registered)
        if (TA_Base_Core::OnlineUpdateListener::getInstance().isObjectTypeRegistered(m_databaseUpdateListenerType, *this))
        {
            TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(m_databaseUpdateListenerType, 
                                                                    *this);
        }
    }


    void GenericCachedMapBase::startListeningToAdditionalItem(unsigned long pKey)
    {
        if (!TA_Base_Core::OnlineUpdateListener::getInstance().
                isKeyRegistered(m_databaseUpdateListenerType, *this, pKey))
        {
            // Go ahead with registering this single key
            TA_Base_Core::OnlineUpdateListener::getInstance().
                registerInterest(m_databaseUpdateListenerType, *this, pKey);       
        }
        else
        {
            TA_ASSERT(false, "Already listening to specified key");
        }
    }


    bool GenericCachedMapBase::stopListeningToItem(unsigned long pKey)
    {
        TA_Base_Core::ThreadGuard guard(m_updateListenerLock);

        if (TA_Base_Core::OnlineUpdateListener::getInstance().
                isKeyRegistered(m_databaseUpdateListenerType, *this, pKey))
        {
            TA_Base_Core::OnlineUpdateListener::getInstance().
                deregisterInterest(m_databaseUpdateListenerType, *this, pKey);
            return true;
        }
        else
        {
            // We weren't listening to this key - can't deregister (already done..)
            return false;
        }
    }


    void GenericCachedMapBase::registerObserver(ICachedMapObserver* ob)
    {
        TA_ASSERT(ob != 0, "Attempting to register null object");

        TA_Base_Core::ThreadGuard observerLock(m_updateObserverLock);

        if (m_registeredObservers.end() == 
                std::find(m_registeredObservers.begin(), m_registeredObservers.end(), ob))
        {
            m_registeredObservers.push_back(ob);
        }
        else
        {
            TA_ASSERT(false, "Registering same observer multiple times");
        }
    }


    void GenericCachedMapBase::deRegisterObserver(ICachedMapObserver* ob)
    {
        TA_Base_Core::ThreadGuard observerLock(m_updateObserverLock);

        std::vector<ICachedMapObserver*>::iterator it =
                    std::find(m_registeredObservers.begin(), m_registeredObservers.end(), ob);

        if (it == m_registeredObservers.end())
        {
            TA_ASSERT(false, "Attempting to deRegister non registered observers");
        }
        else
        {
            m_registeredObservers.erase(it);
        }
    }


    void GenericCachedMapBase::performDatavalueValidation(bool condition, 
                                                          unsigned long key)
                                        //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        if (condition)
        {
            std::ostringstream msg;

            msg << "An invalid record was found in the "
                << m_tableName 
                << " table (pKey="
                << key 
                << ")";
            
            TA_THROW (TA_Base_Core::InvalidPasConfigurationException(msg.str().c_str()));
        }
    }



    void GenericCachedMapBase::onDatabaseUpdateProcessError(unsigned long pKey)
    {
        /// \todo: Implement an audit message to indicate the database update wasn't received
    }


    std::vector<std::string> GenericCachedMapBase::getAllTokens(const std::string& input, const std::string& delimiter)
    {
        std::string::size_type searchOffset = 0;
        std::string::size_type foundOffset = 0;
        std::vector<std::string> resultingTokens;

        do
        {
            foundOffset = input.find(delimiter, searchOffset);
        
            resultingTokens.push_back(input.substr(searchOffset, 
                                                   foundOffset - searchOffset));

            // Must search from position after delimiter on next iteration
            // (cannot do this on first iteration, hence down here)
            searchOffset = foundOffset + delimiter.length();
        }
        while (foundOffset != std::string::npos);

        return resultingTokens;
    }

} // end namespace TA_IRS_Bus