/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/ConfigAtsPSDData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include "core/data_access_interface/ats/src/ConfigAtsPSDData.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
	long ConfigAtsPSDData::s_nextAvailableIdentifier = 0;


	ConfigAtsPSDData::ConfigAtsPSDData()
     : m_atsPSDDataHelper( new AtsPSDDataHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPSDData::ConfigAtsPSDData(const unsigned long idKey)
     : m_atsPSDDataHelper( new AtsPSDDataHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPSDData::ConfigAtsPSDData( const ConfigAtsPSDData& theConfigAtsPSDData)
     : m_atsPSDDataHelper( new AtsPSDDataHelper( *(theConfigAtsPSDData.m_atsPSDDataHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsPSDData::ConfigAtsPSDData(unsigned long row, TA_Base_Core::IData& data)
     : m_atsPSDDataHelper( new AtsPSDDataHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsPSDData::~ConfigAtsPSDData() 
	{
        delete m_atsPSDDataHelper;
        m_atsPSDDataHelper=NULL;
    }

	unsigned long ConfigAtsPSDData::getKey()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getKey();
	}

	unsigned long ConfigAtsPSDData::getPSDDataPointEntityKey()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointEntityKey();
	}
		
	std::string ConfigAtsPSDData::getPSDDataPointType()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointType();
	}
		
	std::string ConfigAtsPSDData::getPSDDataPointNameOfType()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDDataPointNameOfType();
	}

	unsigned long ConfigAtsPSDData::getStationId()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getStationId();
	}
		
	unsigned long ConfigAtsPSDData::getPlatformId()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPlatformId();
	}
		
	std::string ConfigAtsPSDData::getPlatformName()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPlatformName();
	}

    unsigned char ConfigAtsPSDData::getTisPlatformId()
    {
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getTisPlatformId();
    }
		
	int ConfigAtsPSDData::getPSDNumber()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getPSDNumber();
	}

	std::string ConfigAtsPSDData::getMSSZone()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->getMSSZone();
	}

	std::string ConfigAtsPSDData::getName()
    {
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");

        std::ostringstream name;
		name << m_atsPSDDataHelper->getPlatformId();
        name << " - ";
		name << m_atsPSDDataHelper->getPSDDataPointNameOfType();
		name << "/";
		name << m_atsPSDDataHelper->getPSDNumber();

        return name.str();
    }

	void ConfigAtsPSDData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPSDData::invalidate");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");

        m_atsPSDDataHelper->invalidate();
        m_atsPSDDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPSDData::invalidate");
    }

	void ConfigAtsPSDData::setPSDDataPointEntityKey(unsigned long key)
	{
        FUNCTION_ENTRY("setPSDDataPointEntityKey");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
    
		std::ostringstream oldKey;
		std::ostringstream newKey;
		oldKey << m_atsPSDDataHelper->getPSDDataPointEntityKey();
		newKey << key;

        updateAtsPSDDataChanges("PSD DataPoint Entity Key", oldKey.str(), newKey.str());

        m_atsPSDDataHelper->setPSDDataPointEntityKey(key);

		FUNCTION_EXIT;
	}

	void ConfigAtsPSDData::setPSDDataPointType(const std::string& type)
	{
		
        FUNCTION_ENTRY("setPSDDataPointEntityKey");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");

        updateAtsPSDDataChanges("PSD DataPoint Type", m_atsPSDDataHelper->getPSDDataPointType(), type);

        m_atsPSDDataHelper->setPSDDataPointType(type);

		FUNCTION_EXIT;
	}

	void ConfigAtsPSDData::setPSDDataPointNameOfType(const std::string& nameOfType)
	{
        FUNCTION_ENTRY("setPSDDataPointEntityKey");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");

        updateAtsPSDDataChanges("PSD DataPoint Name of Type", m_atsPSDDataHelper->getPSDDataPointNameOfType(), 
							    nameOfType);

        m_atsPSDDataHelper->setPSDDataPointNameOfType(nameOfType);

		FUNCTION_EXIT;
	}

	void ConfigAtsPSDData::setPlatformKey(unsigned long key)
	{
		
        FUNCTION_ENTRY("setPlatformKey");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
    
		std::ostringstream oldKey;
		std::ostringstream newKey;
		oldKey << m_atsPSDDataHelper->getPlatformKey();
		newKey << key;

        updateAtsPSDDataChanges("Platform Key", oldKey.str(), newKey.str());

        m_atsPSDDataHelper->setPlatformKey(key);

		FUNCTION_EXIT;
	}
		
	void ConfigAtsPSDData::setPSDNumber(int psdNumber)
	{
        FUNCTION_ENTRY("setPSDDataPointEntityKey");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
    
		std::ostringstream oldNumber;
		std::ostringstream newNumber;
		oldNumber << m_atsPSDDataHelper->getPSDNumber();
		newNumber << psdNumber;

        updateAtsPSDDataChanges("PSD Number", oldNumber.str(), newNumber.str());

        m_atsPSDDataHelper->setPSDNumber(psdNumber);

		FUNCTION_EXIT;
	}

    void ConfigAtsPSDData::deleteThisObject(bool cascade)
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPSDData::deleteThisObject");
        TA_ASSERT(m_atsPSDDataHelper != NULL,"The ConfigAtsPSDDataHelper pointer is null.");

        // Delete the AtsBranchTrack from the database
        m_atsPSDDataHelper->deleteAtsPSDData(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsBranchTrackHelper pointer m_AtsBranchTrackHelper");

        delete m_atsPSDDataHelper;
        m_atsPSDDataHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPSDData::deleteThisObject");
    }

	bool ConfigAtsPSDData::isNew()
	{
		TA_ASSERT(m_atsPSDDataHelper != NULL,"The AtsPSDDataHelper pointer is null.");
        return m_atsPSDDataHelper->isNew();
	}

    void ConfigAtsPSDData::setName(const std::string& name)
	{
		TA_ASSERT(false ,"setName() should not be called.");
	}

	void ConfigAtsPSDData::applyChanges()
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsPSDData::applyChanges");
        TA_ASSERT(m_atsPSDDataHelper != NULL, "applyChanges() has been called after ConfigAtsPSDData was deleted");

        m_atsPSDDataHelper->writeAtsPSDDataData();

        m_atsPSDDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsPSDData::applyChanges");
    }

	void ConfigAtsPSDData::updateAtsPSDDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsPSDDataChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsPSDDataChanges.find(name);

        if (matching != m_atsPSDDataChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsPSDDataChanges.erase(matching);
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
            m_atsPSDDataChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

} // TA_Core
