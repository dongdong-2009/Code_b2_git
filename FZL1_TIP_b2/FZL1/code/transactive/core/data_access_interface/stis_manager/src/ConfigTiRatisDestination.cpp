/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/stis_manager/src/ConfigTiRatisDestination.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigTiRatisDestination is a concrete implementation of IConfigTiRatisDestination, which is in turn an implementation
  * of ITiRatisDestination. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TiRatisDestinations.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/stis_manager/src/ConfigTiRatisDestination.h"
#include "core/data_access_interface/stis_manager/src/TiRatisDestinationHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigTiRatisDestination::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.

    static const std::string DESTINATION          = "DESTINATION";


    ConfigTiRatisDestination::ConfigTiRatisDestination()
     : m_tiRatisDestinationHelper( new TiRatisDestinationHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigTiRatisDestination::ConfigTiRatisDestination(unsigned long row, TA_Base_Core::IData& data)
     : m_tiRatisDestinationHelper(new TiRatisDestinationHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigTiRatisDestination::ConfigTiRatisDestination(const unsigned long key)
     : m_tiRatisDestinationHelper( new TiRatisDestinationHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigTiRatisDestination::ConfigTiRatisDestination( const ConfigTiRatisDestination& theConfigTiRatisDestination)
     : m_tiRatisDestinationHelper( new TiRatisDestinationHelper( *(theConfigTiRatisDestination.m_tiRatisDestinationHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }


    ConfigTiRatisDestination::~ConfigTiRatisDestination()
    {
        try
        {
            delete m_tiRatisDestinationHelper;
            m_tiRatisDestinationHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigTiRatisDestination::isNew()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");

        return m_tiRatisDestinationHelper->isNew();
    }

    void ConfigTiRatisDestination::setDestination(const std::string & destination)
    {
        FUNCTION_ENTRY("setDestination");
        
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        
        updateTiRatisDestinationChanges(DESTINATION, 
								isNew() ? "" : m_tiRatisDestinationHelper->getDestination(), 
								destination);

        m_tiRatisDestinationHelper->setDestination(destination);

        FUNCTION_EXIT;
    }
    


	std::string ConfigTiRatisDestination::getDestination()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        return m_tiRatisDestinationHelper->getDestination();
    }



	unsigned long ConfigTiRatisDestination::getKey()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");
        return m_tiRatisDestinationHelper->getKey();
    }


    void ConfigTiRatisDestination::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisDestination::invalidate");
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");

        m_tiRatisDestinationHelper->invalidate();
        m_tiRatisDestinationChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisDestination::invalidate");
    }


    void ConfigTiRatisDestination::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisDestination::deleteThisObject");
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The TiRatisDestinationHelper pointer is null.");

        // Delete the ConfigTiRatisDestination from the database
        m_tiRatisDestinationHelper->deleteTiRatisDestination(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete TiRatisDestinationHelper pointer m_tiRatisDestinationHelper");

        delete m_tiRatisDestinationHelper;
        m_tiRatisDestinationHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisDestination::deleteThisObject");
    }


    void ConfigTiRatisDestination::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisDestination::applyChanges");
        TA_ASSERT(m_tiRatisDestinationHelper != NULL, "applyChanges() has been called after ConfigTiRatisDestination was deleted");

        m_tiRatisDestinationHelper->writeTiRatisDestinationData();

        m_tiRatisDestinationChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisDestination::applyChanges");
    }

    void ConfigTiRatisDestination::updateTiRatisDestinationChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateTiRatisDestinationChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_tiRatisDestinationChanges.find(name);

        if (matching != m_tiRatisDestinationChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_tiRatisDestinationChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }

            FUNCTION_EXIT;
            return;
        }

        // The first time we come in we should get to here and we set the old value to be the original value of the attribute
        if (oldValue != newValue)
        {
            // If the old and new values are different then we can add this item to the map
            TA_Base_Core::Values values;
            values.newValue = newValue;
            values.oldValue = oldValue;
            m_tiRatisDestinationChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigTiRatisDestination::updateTiRatisDestinationChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updateTiRatisDestinationChanges_ul");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updateTiRatisDestinationChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigTiRatisDestination::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    std::string ConfigTiRatisDestination::getName()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The m_tiRatisDestinationHelper pointer is null.");

        std::ostringstream name;
		name << "RATIS Destination " << m_tiRatisDestinationHelper->getKey();

        return name.str();
    }
    
    unsigned long ConfigTiRatisDestination::getModifiedKey()
    {
        TA_ASSERT(m_tiRatisDestinationHelper != NULL,"The m_paTrainDvaMessageHelper pointer is null.");        
        return m_tiRatisDestinationHelper->getModifiedKey();
    }
} // closes TA_IRS_Core
