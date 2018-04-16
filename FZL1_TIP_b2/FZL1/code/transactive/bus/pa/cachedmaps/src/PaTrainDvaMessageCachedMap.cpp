/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * Maintains a cache of Train Dva Message mappings, also listens
  * out for changes to database regarding the PA_TRAIN_DVA_MESSAGE table
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <algorithm>

#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "core/exceptions/src/PasExceptions.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"

namespace TA_IRS_Bus
{
    static void toUpper(std::string& convertMe)
    {
        std::transform(convertMe.begin(), convertMe.end(), convertMe.begin(), toupper);
    }

    PaTrainDvaMessageCachedMap::PaTrainDvaMessageCachedMap()
    :
    GenericCachedMap<TrainDvaMessageRecord, TA_Base_Core::IPaTrainDvaMessage>
        (TA_Base_Core::PA_TRAIN_DVA_MESSAGE, "TrainDvaMessage")
    {

    }

    PaTrainDvaMessageCachedMap::~PaTrainDvaMessageCachedMap()
    {
        // All observers should be deregistered on exit
		if (0 != m_registeredGlobalVersionObservers.size())
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Observers remaining" );
		}
    }


    TA_Base_Core::IPaTrainDvaMessage* PaTrainDvaMessageCachedMap::getRecordFromDatabaseUsingKey(unsigned long pKey, bool readWrite)
        //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException)
    {
        return TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getPaTrainDvaMessage(pKey, readWrite);
    }


    void PaTrainDvaMessageCachedMap::validateRecordData(const TrainDvaMessageRecord& dvaMessage)
    {
        // Validate various parameters (failure on a true condition met)
        performDatavalueValidation(0 == dvaMessage.m_label.length(), dvaMessage.m_pKeyOrId);
    }
         

    TrainDvaMessageRecord PaTrainDvaMessageCachedMap::createRecordFromInterface(TA_Base_Core::IPaTrainDvaMessage& item)
         //throw (TA_Base_Core::InvalidPasConfigurationException)
    {
        TrainDvaMessageRecord result;

        result.m_pKeyOrId = item.getKey();
        result.m_label = item.getLabel();

        return result;
    }


    void PaTrainDvaMessageCachedMap::refreshKeyToTrainDvaMessageRecordMap() 
            //throw(TA_Base_Core::DataException, TA_Base_Core::DatabaseException, TA_Base_Core::InvalidPasConfigurationException)
    {
        FUNCTION_ENTRY("refreshKeyToTrainDvaMessageRecordMap");

        // Obtain read only array of *all* train messages
        TA_Base_Core::IPaTrainDvaMessages trainDvaMessages =
                TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getAllPaTrainDvaMessages(false);

        try
        {
            refreshKeyToRecordMapUsing(trainDvaMessages);
        }
        catch (...)
        {
            // Clean up (this is defined in GenericCachedMap.inl)
            deleteAllItemsInArray(trainDvaMessages);
            throw;
        }

        // Clean up (this is defined in GenericCachedMap.inl)
        deleteAllItemsInArray(trainDvaMessages);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "PaTrainDvaMessageCachedMap refreshed and loaded %d items", m_keyToRecordMap.size() );
        FUNCTION_EXIT;
    }

    TrainDvaMessageRecords PaTrainDvaMessageCachedMap::getTrainDvaMessageRecords(const std::string& filterKeyword)
        //throw(TA_Base_Core::CachedMappingNotFoundException)
    {
        FUNCTION_ENTRY("getTrainDvaMessageRecords");

        std::string ucFilterKeyword(filterKeyword);
        toUpper(ucFilterKeyword);

        // Begin with all messages (erasing those we don't want)
        TrainDvaMessageRecords filteredMessages = getAllRecords();
    
        TrainDvaMessageRecordsIt it = filteredMessages.begin();

        while (it != filteredMessages.end())
        {        
            if (!ucFilterKeyword.empty())
            {
                std::string ucSource ((*it).m_label);
		        toUpper(ucSource);
		        
		        if (std::string::npos == ucSource.find(ucFilterKeyword))
		        {
                    // Keyword not found, we may erase this item from result
                    TrainDvaMessageRecordsIt eraseIt = it;
                    // Progress to next before erasing
                    it ++;

                    // Now progressed beyond, may safely erase this item
                    filteredMessages.erase(eraseIt);

                    // Continue (because don't want to increment iterator twice)
                    continue;
		        }
            }      
        
            it ++;
        }

        FUNCTION_EXIT;
        return filteredMessages;
    }


    void PaTrainDvaMessageCachedMap::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        if (TA_Base_Core::DatabaseKey::getInvalidKey() == updateEvent.getKey())
        {
            TA_Base_Core::ThreadGuard observerLock(m_updateGlobalVersionObserverLock);

            // The global train version has changed, don't pass this message down for normal processing
            for (unsigned int i = 0; i < m_registeredGlobalVersionObservers.size(); i ++)
            {
                m_registeredGlobalVersionObservers[i]->onGlobalTrainVersionChanged();
            }
        }
        else
        {
            GenericCachedMap<TrainDvaMessageRecord, TA_Base_Core::IPaTrainDvaMessage>::processUpdate(updateEvent);
        }
    }


    void PaTrainDvaMessageCachedMap::registerGlobalVersionObserver(IGlobalTrainVersionObserver* ob)
    {
		if (NULL == ob)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempting to register null object" );
			return;
		}

        TA_Base_Core::ThreadGuard observerLock(m_updateGlobalVersionObserverLock);

        if (m_registeredGlobalVersionObservers.end() == 
                std::find(m_registeredGlobalVersionObservers.begin(), m_registeredGlobalVersionObservers.end(), ob))
        {
            m_registeredGlobalVersionObservers.push_back(ob);
        }
        else
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Registering same observer multiple times" );
        }
    }


    void PaTrainDvaMessageCachedMap::deRegisterGlobalVersionObserver(IGlobalTrainVersionObserver* ob)
    {
        TA_Base_Core::ThreadGuard observerLock(m_updateGlobalVersionObserverLock);

        std::vector<IGlobalTrainVersionObserver*>::iterator it =
                    std::find(m_registeredGlobalVersionObservers.begin(), m_registeredGlobalVersionObservers.end(), ob);

        if (it == m_registeredGlobalVersionObservers.end())
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempting to deRegister non registered observers" );
        }
        else
        {
            m_registeredGlobalVersionObservers.erase(it);
        }
    }


    void PaTrainDvaMessageCachedMap::startListening()
    {
        GenericCachedMap<TrainDvaMessageRecord, TA_Base_Core::IPaTrainDvaMessage>::startListening();

        // Special case: This is the global version parameter, linked to all messages
        startListeningToAdditionalItem(TA_Base_Core::DatabaseKey::getInvalidKey());
    }

} // end namespace TA_IRS_Bus