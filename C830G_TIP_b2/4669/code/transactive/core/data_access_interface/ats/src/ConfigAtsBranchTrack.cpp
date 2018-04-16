/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/ats/src/ConfigAtsBranchTrack.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * ConfigAtsBranchTrack is a concrete implementation of IConfigAtsBranchTrack, which is in turn an implementation
  * of IAtsBranchTrack. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsBranchTracks.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ats/src/ConfigAtsBranchTrack.h"
#include "core/data_access_interface/ats/src/AtsBranchTrackHelper.h"

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
    
    long ConfigAtsBranchTrack::s_nextAvailableIdentifier = 0;


    ConfigAtsBranchTrack::ConfigAtsBranchTrack()
     : m_AtsBranchTrackHelper( new AtsBranchTrackHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigAtsBranchTrack::ConfigAtsBranchTrack(const unsigned long idKey)
     : m_AtsBranchTrackHelper( new AtsBranchTrackHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsBranchTrack::ConfigAtsBranchTrack( const ConfigAtsBranchTrack& theConfigAtsBranchTrack)
     : m_AtsBranchTrackHelper( new AtsBranchTrackHelper( *(theConfigAtsBranchTrack.m_AtsBranchTrackHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsBranchTrack::ConfigAtsBranchTrack(unsigned long row, TA_Base_Core::IData& data)
     : m_AtsBranchTrackHelper( new AtsBranchTrackHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsBranchTrack::~ConfigAtsBranchTrack()
    {
        try
        {
            delete m_AtsBranchTrackHelper;
            m_AtsBranchTrackHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigAtsBranchTrack::isNew()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

        return m_AtsBranchTrackHelper->isNew();
    }

    void ConfigAtsBranchTrack::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigAtsBranchTrack::setAtsBranchId(const std::string& id)
    {
        FUNCTION_ENTRY("setAtsBranchTrackId");
        
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        
        updateAtsBranchTrackChanges(BRANCHID, m_AtsBranchTrackHelper->getAtsBranchId().c_str(), id.c_str());

        m_AtsBranchTrackHelper->setAtsBranchId(id);

        FUNCTION_EXIT;
    }
    
    void ConfigAtsBranchTrack::setTrack(const std::string& track)
    {
        FUNCTION_ENTRY("setTrack");
        
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
    
        updateAtsBranchTrackChanges(TRACK, m_AtsBranchTrackHelper->getTrack(), track);

        m_AtsBranchTrackHelper->setTrack(track);

        FUNCTION_EXIT;
    }

	unsigned long ConfigAtsBranchTrack::getKey()
	{
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getKey();
    }

    std::string ConfigAtsBranchTrack::getAtsBranchId()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getAtsBranchId();
    }


    std::string ConfigAtsBranchTrack::getTrack()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");
        return m_AtsBranchTrackHelper->getTrack();
    }

    std::string ConfigAtsBranchTrack::getName()
    {
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

        std::ostringstream name;
		name << m_AtsBranchTrackHelper->getAtsBranchId();
        name << " - ";
		name << m_AtsBranchTrackHelper->getTrack();

        return name.str();
    }
 
	
    void ConfigAtsBranchTrack::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsBranchTrack::invalidate");
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

        m_AtsBranchTrackHelper->invalidate();
        m_AtsBranchTrackChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsBranchTrack::invalidate");
    }


    void ConfigAtsBranchTrack::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsBranchTrack::deleteThisObject");
        TA_ASSERT(m_AtsBranchTrackHelper != NULL,"The AtsBranchTrackHelper pointer is null.");

        // Delete the AtsBranchTrack from the database
        m_AtsBranchTrackHelper->deleteAtsBranchTrack(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsBranchTrackHelper pointer m_AtsBranchTrackHelper");

        delete m_AtsBranchTrackHelper;
        m_AtsBranchTrackHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsBranchTrack::deleteThisObject");
    }


    void ConfigAtsBranchTrack::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsBranchTrack::applyChanges");
        TA_ASSERT(m_AtsBranchTrackHelper != NULL, "applyChanges() has been called after AtsBranchTrack was deleted");

        m_AtsBranchTrackHelper->writeAtsBranchTrackData();

        m_AtsBranchTrackChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsBranchTrack::applyChanges");
    }


    void ConfigAtsBranchTrack::updateAtsBranchTrackChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsBranchTrackChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_AtsBranchTrackChanges.find(name);

        if (matching != m_AtsBranchTrackChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_AtsBranchTrackChanges.erase(matching);
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
            m_AtsBranchTrackChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core

