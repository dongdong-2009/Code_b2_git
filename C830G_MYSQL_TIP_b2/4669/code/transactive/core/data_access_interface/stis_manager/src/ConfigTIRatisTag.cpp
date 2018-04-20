/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/stis_manager/src/ConfigTIRatisTag.cpp $
  * @author chunzhong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigTiRatisTag is a concrete implementation of IConfigTiRatisTag, which is in turn an implementation
  * of ITiRatisTag. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for TiRatisTags.
  */
#ifdef __WIN32__
#pragma warning(disable:4786 4290)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/stis_manager/src/ConfigTiRatisTag.h"
#include "core/data_access_interface/stis_manager/src/TiRatisTagHelper.h"

//#include "core/exceptions/src/DataException.h"
//#include "core/exceptions/src/DatabaseException.h"

//#include "core/utilities/src/TAAssert.h"
//#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigTiRatisTag::s_nextAvailableIdentifier = 0;
    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string TAGGROUP        = "Tag Group";
    static const std::string DETAIL          = "Detail";

	//The combination of "Tag Group" and "Detail", not from DBQuery;
	static const std::string RATISTAG        = "RATIS Tag";

    ConfigTiRatisTag::ConfigTiRatisTag()
     : m_tiRatisTagHelper( new TiRatisTagHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigTiRatisTag::ConfigTiRatisTag(unsigned long row, TA_Base_Core::IData& data)
     : m_tiRatisTagHelper(new TiRatisTagHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigTiRatisTag::ConfigTiRatisTag(const unsigned long key)
     : m_tiRatisTagHelper( new TiRatisTagHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }

    ConfigTiRatisTag::ConfigTiRatisTag( const ConfigTiRatisTag& theConfigTiRatisTag)
     : m_tiRatisTagHelper( new TiRatisTagHelper( *(theConfigTiRatisTag.m_tiRatisTagHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {      
    }


    ConfigTiRatisTag::~ConfigTiRatisTag()
    {
        try
        {
            delete m_tiRatisTagHelper;
            m_tiRatisTagHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigTiRatisTag::isNew()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");

        return m_tiRatisTagHelper->isNew();
    }

    void ConfigTiRatisTag::setTagGroup(const std::string & tagGroup)
    {
        FUNCTION_ENTRY("setTagGroup");
        
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        
        updateTiRatisTagChanges(TAGGROUP, 
								isNew() ?  "" : m_tiRatisTagHelper->getTagGroup(), 
								tagGroup);

		m_tiRatisTagHelper->setTagGroup(tagGroup);

        FUNCTION_EXIT;
    }
    

    void ConfigTiRatisTag::setDetail(const std::string & detail)
    {
        FUNCTION_ENTRY("setDetail");
        
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        
        updateTiRatisTagChanges(DETAIL, 
								isNew() ? "" : m_tiRatisTagHelper->getDetail(), 
								detail);

        m_tiRatisTagHelper->setDetail(detail);

        FUNCTION_EXIT;
    }


	std::string ConfigTiRatisTag::getTagGroup()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getTagGroup();
    }


	//RatisTag is the combination of "TagGroup" and "Detail";
    std::string ConfigTiRatisTag::getRatisTag()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getRatisTag();
    }    


    std::string ConfigTiRatisTag::getDetail()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getDetail();
    }


	unsigned long ConfigTiRatisTag::getKey()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getKey();
    }


    void ConfigTiRatisTag::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisTag::invalidate");
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");

        m_tiRatisTagHelper->invalidate();
        m_tiRatisTagChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisTag::invalidate");
    }


    void ConfigTiRatisTag::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisTag::deleteThisObject");
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The TiRatisTagHelper pointer is null.");

        // Delete the ConfigTiRatisTag from the database
        m_tiRatisTagHelper->deleteTiRatisTag(cascade);
        
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "About to delete TiRatisTagHelper pointer m_tiRatisTagHelper");

        delete m_tiRatisTagHelper;
        m_tiRatisTagHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisTag::deleteThisObject");
    }


    void ConfigTiRatisTag::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigTiRatisTag::applyChanges");
        TA_ASSERT(m_tiRatisTagHelper != NULL, "applyChanges() has been called after ConfigTiRatisTag was deleted");

        m_tiRatisTagHelper->writeTiRatisTagData();

        m_tiRatisTagChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigTiRatisTag::applyChanges");
    }

    void ConfigTiRatisTag::updateTiRatisTagChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateTiRatisTagChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_tiRatisTagChanges.find(name);

        if (matching != m_tiRatisTagChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_tiRatisTagChanges.erase(matching);
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
            m_tiRatisTagChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    void ConfigTiRatisTag::updateTiRatisTagChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
    {
        FUNCTION_ENTRY("updateTiRatisTagChanges_ul");

        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updateTiRatisTagChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
    }
    
    void ConfigTiRatisTag::setName(const std::string& name)
    {
        TA_ASSERT(false, "setName() should not be called.");
    }

    std::string ConfigTiRatisTag::getName()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The m_tiRatisTagHelper pointer is null.");

        std::ostringstream name;
		name << "Ratis Tag " << m_tiRatisTagHelper->getKey();

        return name.str();
    }
    
    unsigned long ConfigTiRatisTag::getModifiedKey()
    {
        TA_ASSERT(m_tiRatisTagHelper != NULL, "The m_tiRatisTagHelper pointer is null.");
        return m_tiRatisTagHelper->getModifiedKey();
    }

} // closes TA_IRS_Core
