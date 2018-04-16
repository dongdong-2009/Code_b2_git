/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/ats/src/ConfigAtsFbMapRecord.cpp $
  * @author Nick Jardine
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/06/01 17:56:14 $
  * Last modified by: $Author: peter.wong $
  * 
  * ConfigAtsFbMapRecord is a concrete implementation of IConfigAtsFbMapRecord, which is in turn an implementation
  * of IAtsFbMapRecord. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsFbMapRecords.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ats/src/ConfigAtsFbMapRecord.h"
#include "core/data_access_interface/ats/src/AtsFbMapRecordHelper.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string BRANCHID          = "ATS Branch Id";
    static const std::string TRACK             = "Track";
    
    long ConfigAtsFbMapRecord::s_nextAvailableIdentifier = 0;


    ConfigAtsFbMapRecord::ConfigAtsFbMapRecord()
     : m_atsFbMapRecordHelper( new AtsFbMapRecordHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigAtsFbMapRecord::ConfigAtsFbMapRecord(const unsigned long idKey)
     : m_atsFbMapRecordHelper( new AtsFbMapRecordHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsFbMapRecord::ConfigAtsFbMapRecord( const ConfigAtsFbMapRecord& theConfigAtsFbMapRecord)
     : m_atsFbMapRecordHelper( new AtsFbMapRecordHelper( *(theConfigAtsFbMapRecord.m_atsFbMapRecordHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsFbMapRecord::ConfigAtsFbMapRecord(unsigned long row, TA_Base_Core::IData& data)
     : m_atsFbMapRecordHelper( new AtsFbMapRecordHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsFbMapRecord::~ConfigAtsFbMapRecord()
    {
        try
        {
            delete m_atsFbMapRecordHelper;
            m_atsFbMapRecordHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigAtsFbMapRecord::isNew()
    {
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");

        return m_atsFbMapRecordHelper->isNew();
    }

    void ConfigAtsFbMapRecord::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigAtsFbMapRecord::setBranchIdAsString(const std::string& id)
    {
        FUNCTION_ENTRY("setBranchIdAsString");
        
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        
        updateAtsFbMapRecordChanges(BRANCHID, m_atsFbMapRecordHelper->getBranchIdAsString().c_str(), id.c_str());

        m_atsFbMapRecordHelper->setBranchIdAsString(id);

        FUNCTION_EXIT;
    }

	void ConfigAtsFbMapRecord::setMinAbscissa(unsigned long minAbscissa)
    {
        FUNCTION_ENTRY("setMinAbscissa");
        
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

		std::ostringstream oldAbscissa;
		std::ostringstream newAbscissa;
		oldAbscissa << m_atsFbMapRecordHelper->getMinAbscissa();
		newAbscissa << minAbscissa;
		
        updateAtsFbMapRecordChanges(TRACK, oldAbscissa.str(), newAbscissa.str());

        m_atsFbMapRecordHelper->setMinAbscissa(minAbscissa);

        FUNCTION_EXIT;
    }

	void ConfigAtsFbMapRecord::setMaxAbscissa(unsigned long maxAbscissa)
    {
        FUNCTION_ENTRY("setMaxAbscissa");
        
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
    
        std::ostringstream oldAbscissa;
		std::ostringstream newAbscissa;
		oldAbscissa << m_atsFbMapRecordHelper->getMaxAbscissa();
		newAbscissa << maxAbscissa;
		
        updateAtsFbMapRecordChanges(TRACK, oldAbscissa.str(), newAbscissa.str());

        m_atsFbMapRecordHelper->setMaxAbscissa(maxAbscissa);

        FUNCTION_EXIT;
    }

	void ConfigAtsFbMapRecord::setZoneId(double zoneId)
    {
        FUNCTION_ENTRY("setZoneId");
        
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
    
        std::ostringstream oldZoneId;
		std::ostringstream newZoneId;
		oldZoneId << m_atsFbMapRecordHelper->getZoneId();
		newZoneId << zoneId;
		
        updateAtsFbMapRecordChanges(TRACK, oldZoneId.str(), newZoneId.str());

        m_atsFbMapRecordHelper->setZoneId(zoneId);

        FUNCTION_EXIT;
    }

	void ConfigAtsFbMapRecord::setArmId(double armId)
	{
		FUNCTION_ENTRY("setArmId");

		TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

		std::ostringstream oldArmId;
		std::ostringstream newArmId;
		oldArmId << m_atsFbMapRecordHelper->getArmId();
		newArmId << armId;

		updateAtsFbMapRecordChanges(TRACK, oldArmId.str(), newArmId.str());

		m_atsFbMapRecordHelper->setArmId(armId);

		FUNCTION_EXIT;
	}

	void ConfigAtsFbMapRecord::setZoneTypeName(std::string zoneTypeName)
	{
		FUNCTION_ENTRY("setZoneTypeName");
        
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        
        updateAtsFbMapRecordChanges(BRANCHID, m_atsFbMapRecordHelper->getZoneTypeName().c_str(), zoneTypeName.c_str());

        m_atsFbMapRecordHelper->setZoneTypeName(zoneTypeName);

        FUNCTION_EXIT;
	}
   
	unsigned long ConfigAtsFbMapRecord::getBranchId()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getBranchId();
    }

	std::string ConfigAtsFbMapRecord::getBranchIdAsString()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getBranchIdAsString();
    }

	std::string ConfigAtsFbMapRecord::getTrack()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getTrack();
    }

	unsigned long ConfigAtsFbMapRecord::getMinAbscissa()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getMinAbscissa();
    }

	unsigned long ConfigAtsFbMapRecord::getMaxAbscissa()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getMaxAbscissa();
    }

	double ConfigAtsFbMapRecord::getZoneId()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getZoneId();
    }

	std::string ConfigAtsFbMapRecord::getZoneTypeName()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getZoneTypeName();
    }

	unsigned long ConfigAtsFbMapRecord::getArmId()
	{
		TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
		return m_atsFbMapRecordHelper->getArmId();
	}

	unsigned long ConfigAtsFbMapRecord::getKey()
	{
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");
        return m_atsFbMapRecordHelper->getKey();
    }

	std::string ConfigAtsFbMapRecord::getName()
    {
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");

        std::ostringstream name;
		name << m_atsFbMapRecordHelper->getBranchId();
        name << "/";
		name << m_atsFbMapRecordHelper->getTrack();
		name << " - ";
		name << m_atsFbMapRecordHelper->getZoneId();

        return name.str();
    }
	
    void ConfigAtsFbMapRecord::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsFbMapRecord::invalidate");
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");

        m_atsFbMapRecordHelper->invalidate();
        m_AtsFbMapRecordChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsFbMapRecord::invalidate");
    }


    void ConfigAtsFbMapRecord::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsFbMapRecord::deleteThisObject");
        TA_ASSERT(m_atsFbMapRecordHelper != NULL,"The AtsFbMapRecordHelper pointer is null.");

        // Delete the ConfigAtsFbMapRecord from the database
        m_atsFbMapRecordHelper->deleteAtsFbMapRecord(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsFbMapRecordHelper pointer m_atsFbMapRecordHelper");

        delete m_atsFbMapRecordHelper;
        m_atsFbMapRecordHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsFbMapRecord::deleteThisObject");
    }


    void ConfigAtsFbMapRecord::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsFbMapRecord::applyChanges");
        TA_ASSERT(m_atsFbMapRecordHelper != NULL, "applyChanges() has been called after ConfigAtsFbMapRecord was deleted");

        m_atsFbMapRecordHelper->writeAtsFbMapRecordData();

        m_AtsFbMapRecordChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsFbMapRecord::applyChanges");
    }


    void ConfigAtsFbMapRecord::updateAtsFbMapRecordChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsFbMapRecordChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_AtsFbMapRecordChanges.find(name);

        if (matching != m_AtsFbMapRecordChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_AtsFbMapRecordChanges.erase(matching);
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
            m_AtsFbMapRecordChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core

