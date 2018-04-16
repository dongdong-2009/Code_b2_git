/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDatabaseAccessor.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class uses the Data Access Interface to retrieve ITSI group information and returns it to
  * the caller in a variety of formats. It abstracts some of the database access from the views.
  * 
  */

#pragma warning(disable:4786)

// ITSIGroupsDatabaseAccessor.cpp: implementation of the ITSIGroupsDatabaseAccessor class.
//
//////////////////////////////////////////////////////////////////////

#include "app/configuration/config_plugins/radio_itsi_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDatabaseAccessor.h"

#include "core/data_access_interface/radio/src/IConfigITSIGroup.h"
#include "core/data_access_interface/radio/src/IITSIGroup.h"
#include "core/data_access_interface/radio/src/ITSIGroupAccessFactory.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;
using namespace TA_Base_Core;

namespace TA_IRS_App
{

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    ITSIGroupsDatabaseAccessor::ITSIGroupsDatabaseAccessor()
        : m_isLoaded(false)
    {
    }


    ITSIGroupsDatabaseAccessor::~ITSIGroupsDatabaseAccessor()
    {
        FUNCTION_ENTRY("Destructor");

        try
        {
            // We must clean up all the database objects we loaded
            for (LoadedITSIGroups::iterator iter = m_ITSIGroups.begin(); iter != m_ITSIGroups.end(); ++iter)
            {
                if( (iter->second) != NULL )
                {
                    delete iter->second;
                    iter->second = NULL;
                }
            }
			m_ITSIGroups.clear();
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }

        FUNCTION_EXIT;
    }


    void ITSIGroupsDatabaseAccessor::loadItems()
    {
        FUNCTION_ENTRY("loadItems");

        if (m_isLoaded)
        {
            // This means the data is already loaded so we just ignore it.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "We have already loaded the ITSIs and this is not a refresh so we exit");
            FUNCTION_EXIT;
            return;
        }

       
        // If we get to here this means we are loading the data fresh

        // We must clean up all the database objects we previously loaded in case we are doing a refresh
        for (LoadedITSIGroups::iterator ITSIToDelete = m_ITSIGroups.begin(); ITSIToDelete != m_ITSIGroups.end(); ++ITSIToDelete)
        {
            if( (ITSIToDelete->second) != NULL )
            {
                delete ITSIToDelete->second;
                ITSIToDelete->second = NULL;
            }
        }
        if (m_ITSIGroups.size() > 0) m_ITSIGroups.clear();//if there is something then clear it


        // Now reload all the ITSI groups
        std::vector<TA_Base_Core::IITSIGroup*> ITSIGroups = TA_Base_Core::ITSIGroupAccessFactory::getInstance().getAllITSIGroups(true);

        // And insert the ITSIs into our map. At the same time cast them to IConfigITSI*
        // instead of IITSIData*.
        for (std::vector<TA_Base_Core::IITSIGroup*>::iterator iter = ITSIGroups.begin();
             iter != ITSIGroups.end();
             ++iter
             )
        {
             TA_Base_Core::IConfigITSIGroup* castITSIGp = dynamic_cast<TA_Base_Core::IConfigITSIGroup*>(*iter);
             if( castITSIGp != NULL )
             {
                m_ITSIGroups.insert( LoadedITSIGroups::value_type(castITSIGp->getUniqueIdentifier(), castITSIGp) );
             }
        }

        if (m_ITSIGroups.size() > 0) m_isLoaded = true; // may need to try again as there is no ITSI groups in the database which is impossible
        
        FUNCTION_EXIT;
    }


    std::map<CString,unsigned long> ITSIGroupsDatabaseAccessor::getItemNames(CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("getItemNames");

        std::map<CString,unsigned long> names; 

        // The maximum range is the number of ITSIs times by 2. This is because we want to step for each
        // ITSI while it is loaded and then again while each ITSI is added to the list.
        progress.SetRange( 0, m_ITSIGroups.size() * 2 );

        // Step through all the objects and retrieve the name and key for each. The get methods can
        // throw exceptions and they will be thrown straight out of this method.
        for (LoadedITSIGroups::iterator iter = m_ITSIGroups.begin();
             iter != m_ITSIGroups.end();
             ++iter)
        {
            if (iter->second != NULL)
            {
                names.insert( std::map<CString, unsigned long>::value_type( iter->second->getName().c_str(), iter->second->getUniqueIdentifier() ) );
                progress.StepIt();
            }
        }

        FUNCTION_EXIT;
        return names;
    }


    TA_Base_Core::IConfigItem* ITSIGroupsDatabaseAccessor::getItem(unsigned long key)
    {
        FUNCTION_ENTRY("getItem");

        // Attempt to find the ITSI matching the key passed in.
        LoadedITSIGroups::iterator matching = m_ITSIGroups.find(key);
        if (matching == m_ITSIGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ITSI group with key %lu",key);
            FUNCTION_EXIT;
            return NULL;
        }

        FUNCTION_EXIT;
        return matching->second;
    }


    void ITSIGroupsDatabaseAccessor::deleteItem(unsigned long key)
    {
        FUNCTION_ENTRY("deleteItem");

        // Attempt to find the ITSI matching the key passed in.
        LoadedITSIGroups::iterator matching = m_ITSIGroups.find(key);
        if (matching == m_ITSIGroups.end() )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Could not find the ITSI group with key %lu",key);
            FUNCTION_EXIT;
        }

        TA_ASSERT(matching->second != NULL, "Stored ITSI group was equal to NULL");

        if (!matching->second->isNew())
        {
            matching->second->deleteThisObject();
        }
        delete matching->second;
        matching->second = NULL;
        m_ITSIGroups.erase(matching);

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ITSIGroupsDatabaseAccessor::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_Base_Core::IConfigITSIGroup* newITSIGp = TA_Base_Core::ITSIGroupAccessFactory::getInstance().createITSIGroup();

        m_ITSIGroups.insert( LoadedITSIGroups::value_type( newITSIGp->getUniqueIdentifier(), newITSIGp ) );

        return newITSIGp;

        FUNCTION_EXIT;
    }


    TA_Base_Core::IConfigItem* ITSIGroupsDatabaseAccessor::copyItem(const unsigned long idOfItemToCopy)
    {
        FUNCTION_ENTRY("copyItem");

        TA_Base_Core::IConfigITSIGroup* newITSIGp = NULL;

        // Attempt to find the ITSI matching the key passed in.
        LoadedITSIGroups::iterator matching = m_ITSIGroups.find(idOfItemToCopy);

        TA_ASSERT(matching != m_ITSIGroups.end(), "The key passed could not be found in the map");
        TA_ASSERT(matching->second != NULL, "There was a NULL ConfigITSIGroup stored in the map");

        newITSIGp = TA_Base_Core::ITSIGroupAccessFactory::getInstance().copyITSIGroup(matching->second);

        if (newITSIGp != NULL)
        {
            m_ITSIGroups.insert( LoadedITSIGroups::value_type( newITSIGp->getUniqueIdentifier(), newITSIGp ) );
        }
        
        return newITSIGp;

        FUNCTION_EXIT;
    }


    bool ITSIGroupsDatabaseAccessor::areCurrentChangesPending(std::vector<std::string>& ITSIGroupsNotApplied)
    {
        FUNCTION_ENTRY("areCurrentChangesPending");

        TA_ASSERT(ITSIGroupsNotApplied.empty(), "The vector of strings passed was not empty");

        // If the data has not been loaded yet then there can't be any changes pending
        if (!m_isLoaded)
        {
            return false;
        }

        // Step through all the objects and see if any have changed but not yet been applied.
        for (LoadedITSIGroups::iterator iter = m_ITSIGroups.begin(); iter != m_ITSIGroups.end();  ++iter)
        {
            if( (iter->second != NULL) && iter->second->hasChanged() )
            {
                try
                {
                    ITSIGroupsNotApplied.push_back(iter->second->getName());
                }
                catch ( const TA_Base_Core::DataException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve the name of this ITSI group that has not had it's changes applied yet");
                    ITSIGroupsNotApplied.push_back("Unknown");
                }
                catch ( const TA_Base_Core::DatabaseException& )
                {
                    LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve the name of this ITSI group that has not had it's changes applied yet");
                    ITSIGroupsNotApplied.push_back("Unknown");
                }

            }
        }

        // If there are no strings in the vector then there are no changes currently pending so
        // we can return false.
        FUNCTION_EXIT;

        return !ITSIGroupsNotApplied.empty();
    }

} //end of namespace
