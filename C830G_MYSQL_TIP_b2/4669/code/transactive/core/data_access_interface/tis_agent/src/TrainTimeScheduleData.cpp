/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/tis_agent/src/TrainTimeScheduleData.cpp $
  * @author Robin Ashcroft
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Train Time Schedules
  * for the Traveller Information System (TIS) Agent
  * It implements the ITrainTimeSchedule interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/tis_agent/src/TrainTimeScheduleData.h"
#include "core/data_access_interface/tis_agent/src/ITrainTimeSchedule.h"

#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    //
    // TrainTimeScheduleData
    //
    TrainTimeScheduleData::TrainTimeScheduleData()
        : m_trainTimeScheduleEntries(),
          m_trainTimeScheduleEntriesToRemove(),
          m_entriesAreValid( false )
    {
        // new train time schedule helper
        m_trainTimeScheduleHelper = new TrainTimeScheduleHelper();
    }


    TrainTimeScheduleData::TrainTimeScheduleData( unsigned long pKey )
        : m_trainTimeScheduleEntries(),
          m_trainTimeScheduleEntriesToRemove(),
          m_entriesAreValid( false )
    {
        // assign the key to the helper
        m_trainTimeScheduleHelper = new TrainTimeScheduleHelper( pKey );
    }


    TrainTimeScheduleData::TrainTimeScheduleData( TrainTimeScheduleData& ttsData )
        : m_trainTimeScheduleEntries(),
          m_trainTimeScheduleEntriesToRemove(),
          m_entriesAreValid( true )
    {
        // copy the schedule
        m_trainTimeScheduleHelper = new TrainTimeScheduleHelper( *ttsData.m_trainTimeScheduleHelper );

        //
        // copy the entries
        //

        // force the other schedule to load its entries first (in case they arent loaded
        {
            std::vector<ITrainTimeScheduleEntry*> entries = ttsData.getEntries();
        }

        // now copy each entry
        for (std::vector<TrainTimeScheduleEntryData*>::const_iterator iter = ttsData.m_trainTimeScheduleEntries.begin();
             iter != ttsData.m_trainTimeScheduleEntries.end(); iter++)
        {
            m_trainTimeScheduleEntries.push_back(new TrainTimeScheduleEntryData(**iter));
        }
    }


    TrainTimeScheduleData::~TrainTimeScheduleData()
    {
        // delete each entry
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            if ( (*iter) != NULL)
            {
                delete (*iter);
                (*iter) = NULL;
            }
        }

        m_trainTimeScheduleEntries.clear();

        // delete the entries to remove
        for (std::vector<TrainTimeScheduleEntryData*>::iterator deleteIter = m_trainTimeScheduleEntriesToRemove.begin();
             deleteIter != m_trainTimeScheduleEntriesToRemove.end(); deleteIter++)
        {
            if ( (*deleteIter) != NULL)
            {
                delete (*deleteIter);
                (*deleteIter) = NULL;
            }
        }

        m_trainTimeScheduleEntriesToRemove.clear();

        // delete the schedule
        if( m_trainTimeScheduleHelper != NULL )
		{
			delete m_trainTimeScheduleHelper;
			m_trainTimeScheduleHelper = NULL;
		}
    }


    unsigned long TrainTimeScheduleData::getKey()
    {
        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleHelper->getKey();
    }


	std::string TrainTimeScheduleData::getName()
	{
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleHelper->getName();
	}


    std::string TrainTimeScheduleData::getAdHocMessage(AdhocMessageNumber messageNumber)
    {
        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        return m_trainTimeScheduleHelper->getAdHocMessage(messageNumber);
    }


    std::string TrainTimeScheduleData::getAdHocMessageTitle(AdhocMessageNumber messageNumber)
    {
        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        return m_trainTimeScheduleHelper->getAdHocMessageTitle(messageNumber);
    }


    void TrainTimeScheduleData::invalidate()
	{
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleHelper->invalidate();

        // cause entries to be reloaded as well
        m_entriesAreValid = false;
	}


    void TrainTimeScheduleData::setName(std::string name)
	{
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		m_trainTimeScheduleHelper->setName(name);
	}


    void TrainTimeScheduleData::setAdHocMessage(AdhocMessageNumber messageNumber, std::string message)
    {
        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        m_trainTimeScheduleHelper->setAdHocMessage(messageNumber, message);
    }


    void TrainTimeScheduleData::setAdHocMessageTitle(AdhocMessageNumber messageNumber, std::string messageTitle)
    {
        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        m_trainTimeScheduleHelper->setAdHocMessageTitle(messageNumber, messageTitle);
    }
    

    bool TrainTimeScheduleData::hasChanged()
	{
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        // if the entries havent been loaded yet, load them
        // also if they have, but been invalidated, then they need to be reloaded
        // or they will show that the object has been modified when it should not show that
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }

        // check whether the immediate data has changed
        if ( m_trainTimeScheduleHelper->hasChanged() )
        {
            return true;
        }

        // next check whether we've removed anything
        if ( m_trainTimeScheduleEntriesToRemove.size() > 0 )
        {
            return true;
        }

        // next check if any current entries need saving
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            if ( (*iter)->hasChanged() )
            {
                return true;
            }
        }
        
        // nothing has changed
		return false;
	}


    bool TrainTimeScheduleData::isNew()
	{
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

		return m_trainTimeScheduleHelper->isNew();
	}


    std::vector<ITrainTimeScheduleEntry*> TrainTimeScheduleData::getEntries()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::getEntries" );

        // if the entries havent been loaded yet, load them
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }

        // have to convert our pointers to TrainTimeScheduleEntryData to
        // ITrainTimeScheduleEntry pointers
        std::vector<ITrainTimeScheduleEntry*> tempVector;

        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            tempVector.push_back(*iter);
        }

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::getEntries" );
        // return the vector of pointers to schedule entries
        return tempVector;
    }


    ITrainTimeScheduleEntry* TrainTimeScheduleData::createNewEntry()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::createNewEntry" );
        
        // if the entries havent been loaded yet, load them
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }

        // create a new time schedule entry
        TrainTimeScheduleEntryData* newEntry = new TrainTimeScheduleEntryData();

        // add it to the entries for this schedule
        m_trainTimeScheduleEntries.push_back(newEntry);

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::createNewEntry" );

        // return the entry
        return newEntry;
    }


    void TrainTimeScheduleData::deleteEntry(ITrainTimeScheduleEntry* entryToDelete)
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::deleteEntry" );

        // if the entries havent been loaded yet, load them
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }
        
        // find the matching entry
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            // if this entry matches the one to delete
            if ( (*iter) == entryToDelete)
            {
                LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Entry found. Adding to list to delete." );

                // add it to the ones to delete
                m_trainTimeScheduleEntriesToRemove.push_back(*iter);
                // remove from the list
                m_trainTimeScheduleEntries.erase(iter);

                // found it, iterator is invalid anyway
                break;
            }
        }
        
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::deleteEntry" );
    }


    void TrainTimeScheduleData::applyChanges()
	{
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::applyChanges" );
        
        // if the entries havent been loaded yet, load them
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }
        
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        // because of constraints, the time schedule must be saved first, then each entry
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Saving time schedule." );
		m_trainTimeScheduleHelper->writeTrainTimeSchedule();


        // delete the entries due for deletion
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Deleting time schedule entries queued for deletion." );

        for (std::vector<TrainTimeScheduleEntryData*>::iterator deleteIter = m_trainTimeScheduleEntriesToRemove.begin();
             deleteIter != m_trainTimeScheduleEntriesToRemove.end(); deleteIter++)
        {
            if ( (*deleteIter) != NULL)
            {
                // delete the object in the database (if it isnt new)
                // ie if it exists in the database
                if ( !(*deleteIter)->isNew() )
                {
                    (*deleteIter)->deleteThisObject();
                }

                // delete the object from memory
                delete (*deleteIter);
                (*deleteIter) = NULL;
            }
        }

        m_trainTimeScheduleEntriesToRemove.clear();


        // save the entries that are current
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Saving current time schedule entries." );
        
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            if ( (*iter) != NULL)
            {
                // make sure the entry has it's parent pkey set
                (*iter)->setParentKey(m_trainTimeScheduleHelper->getKey());

                // save it
                (*iter)->applyChanges();
            }
        }


        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::applyChanges" );
	}


    void TrainTimeScheduleData::deleteThisObject()
	{
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::deleteThisObject" );
        
		TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        // if the entries havent been loaded yet, load them
        if ( !m_entriesAreValid )
        {
            reloadEntries();
        }

        // because of constraints, delete the entries first
        // then the schedule itself

		// delete the entries due for deletion
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
              "Deleting time schedule entries queued for deletion." );

        for (std::vector<TrainTimeScheduleEntryData*>::iterator deleteIter = m_trainTimeScheduleEntriesToRemove.begin();
             deleteIter != m_trainTimeScheduleEntriesToRemove.end(); deleteIter++)
        {
            if ( (*deleteIter) != NULL)
            {
                // delete the object in the database (if it isnt new)
                // ie if it exists in the database
                if ( !(*deleteIter)->isNew() )
                {
                    (*deleteIter)->deleteThisObject();
                }

                // delete the object from memory
                delete (*deleteIter);
                (*deleteIter) = NULL;
            }
        }

        m_trainTimeScheduleEntriesToRemove.clear();
        
                
        // delete the entries that are current
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Deleting current time schedule entries." );

        
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            if ( (*iter) != NULL)
            {
                // delete the object in the database (if it isnt new)
                // ie if it exists in the database
                if ( !(*iter)->isNew() )
                {
                    (*iter)->deleteThisObject();
                }

                // delete the object from memory
                delete (*iter);
                (*iter) = NULL;
            }
        }

        m_trainTimeScheduleEntries.clear();

        // delete the schedule itself
        LOG ( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                    "Deleting time schedule." );

        // delete the schedule
        if ( !m_trainTimeScheduleHelper->isNew() )
        {
            m_trainTimeScheduleHelper->deleteTrainTimeSchedule();
        }
        // the helper will be deleted in the destructor
		
        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::deleteThisObject" );
	}


    void TrainTimeScheduleData::reloadEntries()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "TrainTimeScheduleData::reloadEntries" );

        TA_ASSERT(m_trainTimeScheduleHelper != NULL,"The TrainTimeScheduleHelper pointer is null.");

        // delete any existing entries
        for (std::vector<TrainTimeScheduleEntryData*>::iterator iter = m_trainTimeScheduleEntries.begin();
             iter != m_trainTimeScheduleEntries.end(); iter++)
        {
            if ( (*iter) != NULL)
            {
                delete (*iter);
                (*iter) = NULL;
            }
        }

        m_trainTimeScheduleEntries.clear();

        // delete the entries to remove
        for (std::vector<TrainTimeScheduleEntryData*>::iterator deleteIter = m_trainTimeScheduleEntriesToRemove.begin();
             deleteIter != m_trainTimeScheduleEntriesToRemove.end(); deleteIter++)
        {
            if ( (*deleteIter) != NULL)
            {
                delete (*deleteIter);
                (*deleteIter) = NULL;
            }
        }

        m_trainTimeScheduleEntriesToRemove.clear();


        // load entries from the database
        std::vector<unsigned long> entryPkeys = m_trainTimeScheduleHelper->getEntryPkeys();

        // for each key, construct a time schedule entry and add it to our list
        for (std::vector<unsigned long>::iterator pkeyIter = entryPkeys.begin();
            pkeyIter != entryPkeys.end(); pkeyIter++)
        {
            m_trainTimeScheduleEntries.push_back( new TrainTimeScheduleEntryData(*pkeyIter) );
        }

        // entries are now valid
        m_entriesAreValid = true;

        LOG_FUNCTION_EXIT( SourceInfo, "TrainTimeScheduleData::reloadEntries" );
    }


} //end namespace TA_IRS_Core
