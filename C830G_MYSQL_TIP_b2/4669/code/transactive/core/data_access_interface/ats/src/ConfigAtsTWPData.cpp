/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsTWPData.cpp $
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

#include "core/data_access_interface/ats/src/ConfigAtsTWPData.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
	long ConfigAtsTWPData::s_nextAvailableIdentifier = 0;


	ConfigAtsTWPData::ConfigAtsTWPData()
     : m_atsTWPDataHelper( new AtsTWPDataHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsTWPData::ConfigAtsTWPData(const unsigned long idKey)
     : m_atsTWPDataHelper( new AtsTWPDataHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsTWPData::ConfigAtsTWPData( const ConfigAtsTWPData& theConfigAtsTWPData)
     : m_atsTWPDataHelper( new AtsTWPDataHelper( *(theConfigAtsTWPData.m_atsTWPDataHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsTWPData::ConfigAtsTWPData(unsigned long row, TA_Base_Core::IData& data)
     : m_atsTWPDataHelper( new AtsTWPDataHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsTWPData::~ConfigAtsTWPData() 
	{
        delete m_atsTWPDataHelper;
        m_atsTWPDataHelper=NULL;
    }

	unsigned long ConfigAtsTWPData::getTWPDataPointEntityKey()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPDataPointEntityKey();
	}

	std::string ConfigAtsTWPData::getTWPDataPointType()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPDataPointType();
	}

	std::string ConfigAtsTWPData::getTWPCommand()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getTWPCommand();
	}
	
	unsigned long ConfigAtsTWPData::getKey()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
		return m_atsTWPDataHelper->getKey();
	}

	std::string ConfigAtsTWPData::getName()
    {
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
        std::ostringstream name;
		name << m_atsTWPDataHelper->getTWPCommand();
        name << " - ";
		name << m_atsTWPDataHelper->getTWPDataPointType();

        return name.str();
    }

	void ConfigAtsTWPData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsTWPData::invalidate");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");

        m_atsTWPDataHelper->invalidate();
        m_atsTWPDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsTWPData::invalidate");
    }

	void ConfigAtsTWPData::setTWPCommand(std::string command)
	{
		FUNCTION_ENTRY("setTWPCommand");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
    
		updateAtsTWPDataChanges("TWP Command", m_atsTWPDataHelper->getTWPCommand(), command);

        m_atsTWPDataHelper->setTWPCommand(command);

		FUNCTION_EXIT;
	}

	void ConfigAtsTWPData::setTWPDataPointType(std::string type)
	{
		FUNCTION_ENTRY("setTWPDataPointType");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
    
		updateAtsTWPDataChanges("TWP Command Type", m_atsTWPDataHelper->getTWPDataPointType(), type);

        m_atsTWPDataHelper->setTWPDataPointType(type);

		FUNCTION_EXIT;
	}

	void ConfigAtsTWPData::setTWPDataPointEntityKey(unsigned long key)
	{
		FUNCTION_ENTRY("setTWPDataPointEntityKey");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The AtsTWPDataHelper pointer is null.");
    
		std::ostringstream oldKey;
		std::ostringstream newKey;
		oldKey << m_atsTWPDataHelper->getTWPDataPointEntityKey();
		newKey << key;

        updateAtsTWPDataChanges("TWP DataPoint Entity Key", oldKey.str(), newKey.str());

        m_atsTWPDataHelper->setTWPDataPointEntityKey(key);

		FUNCTION_EXIT;
	}

	void ConfigAtsTWPData::deleteThisObject(bool cascade)
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsTWPData::deleteThisObject");
        TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");

        // Delete the AtsBranchTrack from the database
        m_atsTWPDataHelper->deleteAtsTWPData(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsBranchTrackHelper pointer m_AtsBranchTrackHelper");

        delete m_atsTWPDataHelper;
        m_atsTWPDataHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsTWPData::deleteThisObject");
    }

	bool ConfigAtsTWPData::isNew()
	{
		TA_ASSERT(m_atsTWPDataHelper != NULL,"The ConfigAtsTWPDataHelper pointer is null.");
        return m_atsTWPDataHelper->isNew();
	}

    void ConfigAtsTWPData::setName(const std::string& name)
	{
		TA_ASSERT(false ,"setName() should not be called.");
	}

	void ConfigAtsTWPData::applyChanges()
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsTWPData::applyChanges");
        TA_ASSERT(m_atsTWPDataHelper != NULL, "applyChanges() has been called after ConfigAtsTWPData was deleted");

        m_atsTWPDataHelper->writeAtsTWPDataData();

        m_atsTWPDataChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsTWPData::applyChanges");
    }

	void ConfigAtsTWPData::updateAtsTWPDataChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsTWPDataChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsTWPDataChanges.find(name);

        if (matching != m_atsTWPDataChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsTWPDataChanges.erase(matching);
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
            m_atsTWPDataChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


} // TA_Core
