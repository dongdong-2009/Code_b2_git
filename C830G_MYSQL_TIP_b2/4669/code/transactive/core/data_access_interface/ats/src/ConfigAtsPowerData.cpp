/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsPowerData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/ConfigAtsPowerData.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
	long ConfigAtsPowerData::s_nextAvailableIdentifier = 0;


	ConfigAtsPowerData::ConfigAtsPowerData()
     : m_atsPowerDataHelper( new AtsPowerDataHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPowerData::ConfigAtsPowerData(const unsigned long idKey)
     : m_atsPowerDataHelper( new AtsPowerDataHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPowerData::ConfigAtsPowerData( const ConfigAtsPowerData& theConfigAtsPowerData)
     : m_atsPowerDataHelper( new AtsPowerDataHelper( *(theConfigAtsPowerData.m_atsPowerDataHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPowerData::ConfigAtsPowerData(unsigned long row, TA_Base_Core::IData& data)
     : m_atsPowerDataHelper( new AtsPowerDataHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsPowerData::~ConfigAtsPowerData() 
	{
        delete m_atsPowerDataHelper;
        m_atsPowerDataHelper=NULL;
    }

	unsigned long ConfigAtsPowerData::getKey()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getKey();
	}

	unsigned long ConfigAtsPowerData::getTractionPowerDataPointEntityKey()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getTractionPowerDataPointEntityKey();
	}

	unsigned long ConfigAtsPowerData::getTractionPowerBitPosition()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getBitPosition();
	}

	unsigned long ConfigAtsPowerData::getElectricalSubSectionId()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getElectricalSubSectionId();
	}
	
	unsigned long ConfigAtsPowerData::getLocationId()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getLocationId();
	}
	
	std::string ConfigAtsPowerData::getMSSZone()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->getMSSZone();
	}

	std::string ConfigAtsPowerData::getName()
    {
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        std::ostringstream name;
		name << m_atsPowerDataHelper->getLocationId();
        name << " - ";
		name << m_atsPowerDataHelper->getElectricalSubSectionId();

        return name.str();
    }

	void ConfigAtsPowerData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPowerData::invalidate");
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");

        m_atsPowerDataHelper->invalidate();
        m_atsPowerDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPowerData::invalidate");
    }

	void ConfigAtsPowerData::setElectricalSubSectionId(unsigned long id)
	{
		FUNCTION_ENTRY("setElectricalSubSectionId");
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
    
		std::ostringstream oldId;
		std::ostringstream newId;
		oldId << m_atsPowerDataHelper->getElectricalSubSectionId();
		newId << id;

        updateAtsPowerDataChanges("Power DataPoint Entity Key", oldId.str(), newId.str());

        m_atsPowerDataHelper->setElectricalSubSectionId(id);

		FUNCTION_EXIT;
	}


	void ConfigAtsPowerData::setLocationId(unsigned long id)
	{
		FUNCTION_ENTRY("setLocationId");
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
    
		std::ostringstream oldId;
		std::ostringstream newId;
		oldId << m_atsPowerDataHelper->getLocationId();
		newId << id;

        updateAtsPowerDataChanges("Power Record Location Id", oldId.str(), newId.str());

        m_atsPowerDataHelper->setLocationId(id);

		FUNCTION_EXIT;
	}

	void ConfigAtsPowerData::setTractionPowerDataPointEntityKey(unsigned long key)
	{
		FUNCTION_ENTRY("setTractionPowerDataPointEntityKey");
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The AtsPowerDataHelper pointer is null.");
    
		std::ostringstream oldKey;
		std::ostringstream newKey;
		oldKey << m_atsPowerDataHelper->getTractionPowerDataPointEntityKey();
		newKey << key;

        updateAtsPowerDataChanges("Power DataPoint Entity Key", oldKey.str(), newKey.str());

        m_atsPowerDataHelper->setTractionPowerDataPointEntityKey(key);

		FUNCTION_EXIT;
	}

	void ConfigAtsPowerData::deleteThisObject(bool cascade)
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPowerData::deleteThisObject");
        TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");

        // Delete the AtsBranchTrack from the database
        m_atsPowerDataHelper->deleteAtsPowerData(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsBranchTrackHelper pointer m_AtsBranchTrackHelper");

        delete m_atsPowerDataHelper;
        m_atsPowerDataHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPowerData::deleteThisObject");
    }

	bool ConfigAtsPowerData::isNew()
	{
		TA_ASSERT(m_atsPowerDataHelper != NULL,"The ConfigAtsPowerDataHelper pointer is null.");
        return m_atsPowerDataHelper->isNew();
	}

    void ConfigAtsPowerData::setName(const std::string& name)
	{
		TA_ASSERT(false ,"setName() should not be called.");
	}

	void ConfigAtsPowerData::applyChanges()
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPowerData::applyChanges");
        TA_ASSERT(m_atsPowerDataHelper != NULL, "applyChanges() has been called after ConfigAtsPowerData was deleted");

        m_atsPowerDataHelper->writeAtsPowerDataData();

        m_atsPowerDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPowerData::applyChanges");
    }

	void ConfigAtsPowerData::updateAtsPowerDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsPowerDataChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsPowerDataChanges.find(name);

        if (matching != m_atsPowerDataChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsPowerDataChanges.erase(matching);
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
            m_atsPowerDataChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


} // TA_Core
