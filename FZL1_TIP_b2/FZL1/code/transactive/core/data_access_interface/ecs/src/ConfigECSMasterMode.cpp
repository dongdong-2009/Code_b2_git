/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/ecs/src/ConfigECSMasterMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * ConfigECSMasterMode is a concrete implementation of IConfigECSMasterMode, which is in turn an implementation
  * of IECSMasterMode. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ECSMasterModes.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ecs/src/ConfigECSMasterMode.h"
#include "core/data_access_interface/ecs/src/ECSMasterModeHelper.h"
#include "core/data_access_interface/ecs/src/IECSStationMode.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{
    long ConfigECSMasterMode::s_nextAvailableIdentifier = 0;

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    static const std::string MASTERMODE  = "Master Mode";
    static const std::string Condition   = "Condition";
    static const std::string ECSZONEID   = "ECS Zone Id";
    static const std::string EVACUATION  = "Evacuation Direction";
    static const std::string AIRFLOW     = "Airflow Direction";
	static const std::string SITUATION   = "Situation";
    static const std::string DISPLAY     = "Display Direction";
    static const std::string STATIONMODE = "Station Mode";
	static const std::string VERIFICATIONMSG = "Verification Message";


    ConfigECSMasterMode::ConfigECSMasterMode()
     : m_ECSMasterModeHelper( new ECSMasterModeHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigECSMasterMode::ConfigECSMasterMode(const unsigned long key)
     : m_ECSMasterModeHelper( new ECSMasterModeHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> modes = m_ECSMasterModeHelper->getAssociatedStationModeKeys();
        m_originalStationModes.insert(modes.begin(),modes.end());
        m_newStationModes.insert(modes.begin(),modes.end());
    }

    ConfigECSMasterMode::ConfigECSMasterMode( const ConfigECSMasterMode& theConfigECSMasterMode)
     : m_ECSMasterModeHelper( new ECSMasterModeHelper( *(theConfigECSMasterMode.m_ECSMasterModeHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> modes = m_ECSMasterModeHelper->getAssociatedStationModeKeys();
        // Don't insert anything in the original modes since all the ones we have are new
        m_newStationModes.insert(modes.begin(),modes.end());
    }

    ConfigECSMasterMode::ConfigECSMasterMode(unsigned long row, TA_Base_Core::IData& data)
     : m_ECSMasterModeHelper( new ECSMasterModeHelper(row, data)),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> modes = m_ECSMasterModeHelper->getAssociatedStationModeKeys();
        m_originalStationModes.insert(modes.begin(),modes.end());
        m_newStationModes.insert(modes.begin(),modes.end());
    }


    ConfigECSMasterMode::~ConfigECSMasterMode()
    {
        try
        {
            delete m_ECSMasterModeHelper;
            m_ECSMasterModeHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigECSMasterMode::isNew()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");

        return m_ECSMasterModeHelper->isNew();
    }


    void ConfigECSMasterMode::setName(const std::string& name)
    {
        TA_ASSERT(false ,"setName() should not be called.");
    }

    
    void ConfigECSMasterMode::setMasterMode(unsigned long mode)
    {
        FUNCTION_ENTRY("setMasterMode");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        
        char buf1[64] = {0}, buf2[64] = {0};
        sprintf(buf1,"%ul", m_ECSMasterModeHelper->getMasterMode());
        sprintf(buf2,"%ul", mode);
        updateECSMasterModeChanges(MASTERMODE, buf1, buf2);

        m_ECSMasterModeHelper->setMasterMode(mode);

        FUNCTION_EXIT;
    }

    
    void ConfigECSMasterMode::setCondition(const std::string& Condition)
    {
        FUNCTION_ENTRY("setCondition");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
    
        updateECSMasterModeChanges(Condition, m_ECSMasterModeHelper->getCondition(), Condition);

        m_ECSMasterModeHelper->setCondition(Condition);

        FUNCTION_EXIT;
    }


    void ConfigECSMasterMode::setECSZoneId(int zoneId)
    {
        FUNCTION_ENTRY("setECSZoneId");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        
        char buf1[64], buf2[64];
        sprintf(buf1,"%d", m_ECSMasterModeHelper->getECSZoneId());
        sprintf(buf2,"%d", zoneId);
        updateECSMasterModeChanges(ECSZONEID, buf1, buf2);

        m_ECSMasterModeHelper->setECSZoneId(zoneId);

        FUNCTION_EXIT;
    }

    void ConfigECSMasterMode::setEvacuationDirection(const std::string& evacDir)
    {
        FUNCTION_ENTRY("setEvacuationDirection");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
    
        updateECSMasterModeChanges(EVACUATION, m_ECSMasterModeHelper->getEvacuationDirection(), evacDir);

        m_ECSMasterModeHelper->setEvacuationDirection(evacDir);

        FUNCTION_EXIT;
    }


    void ConfigECSMasterMode::setAirflowDirection(const std::string& airDir)
    {
        FUNCTION_ENTRY("setAirflowDirection");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
    
        updateECSMasterModeChanges(AIRFLOW, m_ECSMasterModeHelper->getAirflowDirection(), airDir);

        m_ECSMasterModeHelper->setAirflowDirection(airDir);
        FUNCTION_EXIT;
    }
	void ConfigECSMasterMode::setSituation(const std::string& situation)
	{
		FUNCTION_ENTRY("setSituation");
		TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		updateECSMasterModeChanges(SITUATION, m_ECSMasterModeHelper->getSituation(), situation);
		m_ECSMasterModeHelper->setSituation(situation);

        FUNCTION_EXIT;
    }

    void ConfigECSMasterMode::setDisplayDirection(const std::string& dispDir)
    {
        FUNCTION_ENTRY("setDisplayDirection");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
    
        updateECSMasterModeChanges(DISPLAY, m_ECSMasterModeHelper->getDisplayDirection(), dispDir);

        m_ECSMasterModeHelper->setDisplayDirection(dispDir);

        FUNCTION_EXIT;
    }

	void ConfigECSMasterMode::setVerificationMessage(const std::string& msg)
	{
        FUNCTION_ENTRY("setVerificationMessage");
        
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
    
        updateECSMasterModeChanges(VERIFICATIONMSG, m_ECSMasterModeHelper->getVerificationMessage(), msg);

        m_ECSMasterModeHelper->setVerificationMessage(msg);

        FUNCTION_EXIT;
	}

    void ConfigECSMasterMode::setAssociatedStationMode( unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedStationMode");

        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        
        // We just simulate the adding and removing of changes from the list as we need to check the
        // modes manually when the list of changes is retrieved. It is too difficult to try and
        // manage profile changes in strings. Instead we use the m_originalmodes and m_newmodes
        // sets. The havemodesChanged() call below will maintain these sets.

        bool haveChanged = haveStationModesChanged(stationModeKey, isAssociated);

        if (haveChanged)
        {
            updateECSMasterModeChanges(STATIONMODE, "old", "new");
        }
        else
        {
            updateECSMasterModeChanges(STATIONMODE, "old", "old");
        }

        m_ECSMasterModeHelper->setAssociatedStationMode(stationModeKey, isAssociated);

        FUNCTION_EXIT;
    }

    std::string ConfigECSMasterMode::getName()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The m_ECSMasterModeHelper pointer is null.");

        std::ostringstream name;
		name << "ECS Master Mode " << m_ECSMasterModeHelper->getKey();

        return name.str();
    }

    unsigned long ConfigECSMasterMode::getKey()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getKey();
    }

    unsigned long ConfigECSMasterMode::getMasterMode()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getMasterMode();
    }

    std::string ConfigECSMasterMode::getCondition()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getCondition();
    }    

    int ConfigECSMasterMode::getECSZoneId()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getECSZoneId();
    }
    
    std::string ConfigECSMasterMode::getEvacuationDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getEvacuationDirection();
    }  

    std::string ConfigECSMasterMode::getAirflowDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getAirflowDirection();
    }  

	std::string ConfigECSMasterMode::getSituation()
	{
		TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getSituation();
	}
    std::string ConfigECSMasterMode::getDisplayDirection()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getDisplayDirection();
    }

	std::string ConfigECSMasterMode::getVerificationMessage()
	{
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
        return m_ECSMasterModeHelper->getVerificationMessage();
	}

    time_t ConfigECSMasterMode::getDateCreated()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getDateCreated();
    }


    time_t ConfigECSMasterMode::getDateModified()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getDateModified();
    }
 
    std::vector<IECSStationMode*> ConfigECSMasterMode::getAssociatedStationModes()
	{
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getAssociatedStationModes();
	}

    std::vector<unsigned long> ConfigECSMasterMode::getAssociatedStationModeKeys()
    {
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");
		return m_ECSMasterModeHelper->getAssociatedStationModeKeys();
    }

	
    void ConfigECSMasterMode::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSMasterMode::invalidate");
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");

        m_ECSMasterModeHelper->invalidate();
        m_ECSMasterModeChanges.clear();
        m_newStationModes = m_originalStationModes;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSMasterMode::invalidate");
    }


    void ConfigECSMasterMode::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSMasterMode::deleteThisObject");
        TA_ASSERT(m_ECSMasterModeHelper != NULL,"The ECSMasterModeHelper pointer is null.");

        // Delete the ConfigECSMasterMode from the database
        m_ECSMasterModeHelper->deleteECSMasterMode(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete ECSMasterModeHelper pointer m_ECSMasterModeHelper");

        delete m_ECSMasterModeHelper;
        m_ECSMasterModeHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSMasterMode::deleteThisObject");
    }


    void ConfigECSMasterMode::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSMasterMode::applyChanges");
        TA_ASSERT(m_ECSMasterModeHelper != NULL, "applyChanges() has been called after ConfigECSMasterMode was deleted");

        m_ECSMasterModeHelper->writeECSMasterModeData();

        m_newStationModes= m_originalStationModes;
        m_ECSMasterModeChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSMasterMode::applyChanges");
    }


    void ConfigECSMasterMode::updateECSMasterModeChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateECSMasterModeChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_ECSMasterModeChanges.find(name);

        if (matching != m_ECSMasterModeChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_ECSMasterModeChanges.erase(matching);
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
            m_ECSMasterModeChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    bool ConfigECSMasterMode::haveStationModesChanged(unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("haveStationModesChanged");

        if (isAssociated)
        {
            // This means we're adding a new profile so add it to the list of new modes
            m_newStationModes.insert(stationModeKey);
        }
        else
        {
            // This means we're removing a profile so remove it from the list of new modes
            m_newStationModes.erase( stationModeKey );
        }

        // Now figure out if the two sets (original and new) are different.
        std::ostringstream setDifference;
        std::set_symmetric_difference(m_newStationModes.begin(), m_newStationModes.end(),
                                      m_originalStationModes.begin(), m_originalStationModes.end(),
                                      std::ostream_iterator<unsigned long>(setDifference,""));

        // If this is empty then there were no set differences
        if (setDifference.str().empty())
        {
            FUNCTION_EXIT;
            return false;
        }

        // Otherwise there were set differences
        FUNCTION_EXIT;
        return true;

    }
    
} // closes TA_Core

