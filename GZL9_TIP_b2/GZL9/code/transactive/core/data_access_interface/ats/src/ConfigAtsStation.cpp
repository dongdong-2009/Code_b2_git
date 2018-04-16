/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/transactive/core/data_access_interface/ats/src/Attic/ConfigAtsStation.cpp,v $
  * @author:  Jiang Haipeng
  * @version: $Revision: 1.1.2.4.16.1 $
  *
  * Last modification: $Date: 2007/03/21 06:59:37 $
  * Last modified by:  $Author: changxing $
  *
  * <description>
  *
  */

// ConfigAtsStation.cpp: implementation of the ConfigAtsStation class.
//
//////////////////////////////////////////////////////////////////////

#include "ConfigAtsStation.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Core;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_Core
{
	static const std::string ATS_STATION_ID("AtsStation StationID");
    static const std::string ATS_STATION_LOCATION("AtsStation Location");

	long ConfigAtsStation::s_nextAvailableIdentifier = 0;
	ConfigAtsStation::ConfigAtsStation( unsigned long key )
		:m_uniqueIdentifier( ++s_nextAvailableIdentifier )
	{
		m_helper = new AtsStationHelper(key);
	}

	ConfigAtsStation::ConfigAtsStation()
		:m_uniqueIdentifier( ++s_nextAvailableIdentifier )
	{
		m_helper = new AtsStationHelper(0);	
	}

	ConfigAtsStation::ConfigAtsStation( unsigned long row, TA_Base_Core::IData& data)
		:m_uniqueIdentifier( ++s_nextAvailableIdentifier )
	{
		m_helper = new AtsStationHelper(row, data);
	}

	ConfigAtsStation::~ConfigAtsStation()
	{
		delete m_helper;
	}
	
	void ConfigAtsStation::setStationId(const std::string& id)
	{
		TA_ASSERT(m_helper != NULL,"The AtsStation helper pointer is null.");
		
		std::ostringstream originalStationId, newStationId;
        originalStationId << m_helper->getStationId();
		newStationId << id;
		
        updateAtsStationIdChanges(ATS_STATION_ID, originalStationId.str(), newStationId.str());

		m_helper->setStationId(id);
	}
	
	void ConfigAtsStation::setLocationKey(unsigned long location)
	{
		TA_ASSERT(m_helper != NULL,"The AtsStation helper pointer is null.");
		
		std::ostringstream originalLocation, newLocation;
        originalLocation << m_helper->getLocation();
		newLocation << location;
		
        updateAtsStationIdChanges(ATS_STATION_ID, originalLocation.str(), newLocation.str());

		m_helper->setLocationKey(location);
	}
	
	void ConfigAtsStation::deleteThisObject()
	{
		m_helper->deleteObject();
	}

	void ConfigAtsStation::editThisObject()
	{
		m_helper->save(false);
	}
	
	void ConfigAtsStation::addObject()
	{
		m_helper->save(true);
	}

	unsigned long ConfigAtsStation::getKey()
	{
		return m_helper->getKey();
	}

	std::string ConfigAtsStation::getName()
	{
		return m_helper->getName();
	}

	std::string ConfigAtsStation::getStationId()
	{
		return m_helper->getStationId();
	}

	unsigned long ConfigAtsStation::getLocation()
	{
		return m_helper->getLocation();
	}

	//*******IConfigItem interface************************
	TA_Base_Core::ItemChanges ConfigAtsStation::getAllItemChanges()
	{
		 return m_atsStationIdChanges;
	}
	
	bool ConfigAtsStation::hasChanged()
	{
		return ! m_atsStationIdChanges.empty();
	}
	
	bool ConfigAtsStation::isNew()
	{
		return m_helper->isNew();
	}
	
	void ConfigAtsStation::setName(const std::string& name)
	{
	}
	
	void ConfigAtsStation::applyChanges()
	{
        FUNCTION_ENTRY("ConfigAtsStation::applyChanges");
        TA_ASSERT(m_helper != NULL, "applyChanges() has been called after atsStation was deleted");
		
        m_helper->save(true);
		
        m_atsStationIdChanges.clear();
		
        FUNCTION_EXIT;
	}

	unsigned long ConfigAtsStation::getUniqueIdentifier()
	{
		return m_uniqueIdentifier;
	}

    void ConfigAtsStation::invalidate()
    {
        FUNCTION_ENTRY("ConfigAtsStation::invalidate");
        TA_ASSERT(m_helper != NULL,"The m_helper pointer is null.");
		
        m_helper->invalidate();
        m_atsStationIdChanges.clear();
		
        FUNCTION_EXIT;
    }

    void ConfigAtsStation::updateAtsStationIdChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsStationIdChanges");
		
		std::string oldValueString;
		std::string newValueString;
		
		// Search for this name in the map
		TA_Base_Core::ItemChanges::iterator matching = m_atsStationIdChanges.find(name);
		
        if (matching != m_atsStationIdChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsStationIdChanges.erase(matching);
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
            m_atsStationIdChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }
		
        FUNCTION_EXIT;
    }
}