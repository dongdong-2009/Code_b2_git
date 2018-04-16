/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/core/data_access_interface/radio/src/ConfigRadioSubscriber.cpp $
  * @author:  Anggiono
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */

#include "core/data_access_interface/radio/src/ConfigRadioSubscriber.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
	long ConfigRadioSubscriber::s_nextAvailableIdentifier = 0;

	static const std::string RESOURCE_TYPE = "Resource Type";
	static const std::string RESOURCE_ID = "Resource Id";
	static const std::string SSI = "Ssi";
	static const std::string ALIAS = "Alias";

    ConfigRadioSubscriber::ConfigRadioSubscriber() :
		m_radioSubscriberHelper(new RadioSubscriberHelper()),
		m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigRadioSubscriber::ConfigRadioSubscriber(const unsigned long key):
        m_radioSubscriberHelper(new RadioSubscriberHelper(key)),
		m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigRadioSubscriber::ConfigRadioSubscriber(const unsigned long row, TA_Base_Core::IData& data):
        m_radioSubscriberHelper(new RadioSubscriberHelper(row, data)),
		m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }
        
    ConfigRadioSubscriber::ConfigRadioSubscriber(const RadioResource&  resource):
        m_radioSubscriberHelper(new RadioSubscriberHelper(resource)),
		m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigRadioSubscriber::ConfigRadioSubscriber( const ConfigRadioSubscriber& theConfigRadioSubscriber)
     : m_radioSubscriberHelper(new RadioSubscriberHelper(*(theConfigRadioSubscriber.m_radioSubscriberHelper))),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigRadioSubscriber::~ConfigRadioSubscriber()
    {
	  try
	  {
            delete m_radioSubscriberHelper;
            m_radioSubscriberHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }		
    }

    unsigned long ConfigRadioSubscriber::getKey()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        return m_radioSubscriberHelper->getKey();
    }

    char ConfigRadioSubscriber::getResourceType()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        return m_radioSubscriberHelper->getResourceType();
    }

    unsigned long ConfigRadioSubscriber::getResourceId()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        return m_radioSubscriberHelper->getResourceId();
    }
    
    unsigned long ConfigRadioSubscriber::getSSI()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        return m_radioSubscriberHelper->getSSI();
    }

    std::string ConfigRadioSubscriber::getAlias()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        return m_radioSubscriberHelper->getAlias();
    }

    
    void ConfigRadioSubscriber::invalidate()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        m_radioSubscriberHelper->invalidate();
		m_RadioSubscriberChanges.clear();
    }

    void ConfigRadioSubscriber::save()
    {
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        m_radioSubscriberHelper->writeSubscriberData();
		m_RadioSubscriberChanges.clear();
    }


	void ConfigRadioSubscriber::setResourceId(unsigned long rid)
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        updateRadioSubscriberChanges(RESOURCE_ID, 
                                    isNew() ? 0 : m_radioSubscriberHelper->getResourceId(), 
                                    rid);
		m_radioSubscriberHelper->setResourceId(rid);
	}

	void ConfigRadioSubscriber::setResourceType(RadioResourceType type)
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        updateRadioSubscriberChanges(RESOURCE_TYPE, 
                                    isNew() ? 0 : static_cast<unsigned long>(m_radioSubscriberHelper->getResourceType()), 
                                    static_cast<unsigned long>(type));
		m_radioSubscriberHelper->setResourceType(type);
	}

	void ConfigRadioSubscriber::setSSI(unsigned long ssi)
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        updateRadioSubscriberChanges(SSI, 
                                    isNew() ? 0 : static_cast<unsigned long>(m_radioSubscriberHelper->getSSI()), 
                                    ssi);
		m_radioSubscriberHelper->setSSI(ssi);
	}

	void ConfigRadioSubscriber::setAlias(const std::string& alias)
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        updateRadioSubscriberChanges(ALIAS, 
                                    isNew() ? "" : m_radioSubscriberHelper->getAlias(), 
                                    alias);

		m_radioSubscriberHelper->setAlias(alias);
	}

	time_t ConfigRadioSubscriber::getDateCreated()
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

		return m_radioSubscriberHelper->getDateCreated();
	}

	time_t ConfigRadioSubscriber::getDateModified()
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

		return m_radioSubscriberHelper->getDateModified();
	}

	void ConfigRadioSubscriber::deleteThisObject(bool cascade/* =false */)
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

        m_radioSubscriberHelper->deleteSubscriber();
	}

	void ConfigRadioSubscriber::applyChanges()
	{
		save();
	}

	bool ConfigRadioSubscriber::isNew()
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

		return m_radioSubscriberHelper->isNew();
	}

	std::string ConfigRadioSubscriber::getName()
	{
		TA_ASSERT(m_radioSubscriberHelper != NULL,"The RadioSubscriberHelper pointer is null.");

		return m_radioSubscriberHelper->getAlias();
	}

	void ConfigRadioSubscriber::setName(const std::string& name)
	{
		//do nothing 
	}

	void ConfigRadioSubscriber::updateRadioSubscriberChanges(const std::string& name, const unsigned long oldValue, const unsigned long newValue)
	{
        FUNCTION_ENTRY("updateRadioSubscriberChanges");
        std::ostringstream oldValueStr;
        oldValueStr << oldValue;
        std::ostringstream newValueStr;
        newValueStr << newValue;

        updateRadioSubscriberChanges(name, oldValueStr.str(), newValueStr.str());
        FUNCTION_EXIT;
	}

	void ConfigRadioSubscriber::updateRadioSubscriberChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
	{
        FUNCTION_ENTRY("updateRadioSubscriberChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_RadioSubscriberChanges.find(name);

        if (matching != m_RadioSubscriberChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_RadioSubscriberChanges.erase(matching);
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
            m_RadioSubscriberChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;		
	}
}

