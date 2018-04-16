/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ecs/src/ConfigECSMasterMode.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
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
#include "core/exceptions/src/TransactiveException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/DatabaseKey.h"

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
     : m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		m_ECSMasterModeHelper = new ECSMasterModeHelper();
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
    }


    ConfigECSMasterMode::ConfigECSMasterMode(const unsigned long key)
     : m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		m_ECSMasterModeHelper = new ECSMasterModeHelper(key);
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
        std::vector<unsigned long> modes = m_ECSMasterModeHelper->getAssociatedStationModeKeys();
        m_originalStationModes.insert(modes.begin(),modes.end());
        m_newStationModes.insert(modes.begin(),modes.end());
    }

    ConfigECSMasterMode::ConfigECSMasterMode( const ConfigECSMasterMode& theConfigECSMasterMode)
     : m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		m_ECSMasterModeHelper = new ECSMasterModeHelper( *(theConfigECSMasterMode.m_ECSMasterModeHelper));
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
        std::vector<unsigned long> modes = m_ECSMasterModeHelper->getAssociatedStationModeKeys();
        // Don't insert anything in the original modes since all the ones we have are new
        m_newStationModes.insert(modes.begin(),modes.end());
    }

    ConfigECSMasterMode::ConfigECSMasterMode(unsigned long row, TA_Base_Core::IData& data)
     : m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		m_ECSMasterModeHelper = new ECSMasterModeHelper(row, data);
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			TA_THROW( TA_Base_Core::TransactiveException("The ECSMasterModeHelper pointer is null." ));
		}
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
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return false;
		}

        return m_ECSMasterModeHelper->isNew();
    }


    void ConfigECSMasterMode::setName(const std::string& name)
    {
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "setName() should not be called." );
    }

    
    void ConfigECSMasterMode::setMasterMode(unsigned long mode)
    {
        FUNCTION_ENTRY("setMasterMode");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
        
        char buf1[64], buf2[64];
        sprintf(buf1,"%ul", m_ECSMasterModeHelper->getMasterMode());
        sprintf(buf2,"%ul", mode);
        updateECSMasterModeChanges(MASTERMODE, buf1, buf2);

        m_ECSMasterModeHelper->setMasterMode(mode);

        FUNCTION_EXIT;
    }

    
    void ConfigECSMasterMode::setCondition(const std::string& Condition)
    {
        FUNCTION_ENTRY("setCondition");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
    
        updateECSMasterModeChanges(Condition, m_ECSMasterModeHelper->getCondition(), Condition);

        m_ECSMasterModeHelper->setCondition(Condition);

        FUNCTION_EXIT;
    }


    void ConfigECSMasterMode::setECSZoneId(int zoneId)
    {
        FUNCTION_ENTRY("setECSZoneId");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
        
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
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
    
        updateECSMasterModeChanges(EVACUATION, m_ECSMasterModeHelper->getEvacuationDirection(), evacDir);

        m_ECSMasterModeHelper->setEvacuationDirection(evacDir);

        FUNCTION_EXIT;
    }

    void ConfigECSMasterMode::setAirflowDirection(const std::string& airDir)
    {
        FUNCTION_ENTRY("setAirflowDirection");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
    
        updateECSMasterModeChanges(AIRFLOW, m_ECSMasterModeHelper->getAirflowDirection(), airDir);

        m_ECSMasterModeHelper->setAirflowDirection(airDir);

        FUNCTION_EXIT;
    }

	void ConfigECSMasterMode::setSituation(const std::string& situation)
	{
		FUNCTION_ENTRY("setSituation");

		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}

		updateECSMasterModeChanges(SITUATION, m_ECSMasterModeHelper->getSituation(), situation);

		m_ECSMasterModeHelper->setSituation(situation);

		FUNCTION_EXIT;
	}

    void ConfigECSMasterMode::setDisplayDirection(const std::string& dispDir)
    {
        FUNCTION_ENTRY("setDisplayDirection");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
    
        updateECSMasterModeChanges(DISPLAY, m_ECSMasterModeHelper->getDisplayDirection(), dispDir);

        m_ECSMasterModeHelper->setDisplayDirection(dispDir);

        FUNCTION_EXIT;
    }

	void ConfigECSMasterMode::setVerificationMessage(const std::string& msg)
	{
        FUNCTION_ENTRY("setVerificationMessage");
        
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
    
        updateECSMasterModeChanges(VERIFICATIONMSG, m_ECSMasterModeHelper->getVerificationMessage(), msg);

        m_ECSMasterModeHelper->setVerificationMessage(msg);

        FUNCTION_EXIT;
	}

    void ConfigECSMasterMode::setAssociatedStationMode( unsigned long stationModeKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedStationMode");

		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return;
		}
        
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
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}

        std::ostringstream name;
		name << "ECS Master Mode " << m_ECSMasterModeHelper->getKey();

        return name.str();
    }

    unsigned long ConfigECSMasterMode::getKey()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getKey();
    }

    unsigned long ConfigECSMasterMode::getMasterMode()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getMasterMode();
    }

    std::string ConfigECSMasterMode::getCondition()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getCondition();
    }    

    int ConfigECSMasterMode::getECSZoneId()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
        return m_ECSMasterModeHelper->getECSZoneId();
    }
    
    std::string ConfigECSMasterMode::getEvacuationDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getEvacuationDirection();
    }  

    std::string ConfigECSMasterMode::getAirflowDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getAirflowDirection();
    } 

	std::string ConfigECSMasterMode::getSituation()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
		return m_ECSMasterModeHelper->getSituation();
	}

    std::string ConfigECSMasterMode::getDisplayDirection()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getDisplayDirection();
    }

	std::string ConfigECSMasterMode::getVerificationMessage()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return "";
		}
        return m_ECSMasterModeHelper->getVerificationMessage();
	}

    time_t ConfigECSMasterMode::getDateCreated()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
		return m_ECSMasterModeHelper->getDateCreated();
    }


    time_t ConfigECSMasterMode::getDateModified()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return TA_Base_Core::DatabaseKey::getInvalidKey();
		}
		return m_ECSMasterModeHelper->getDateModified();
    }
 
    std::vector<IECSStationMode*> ConfigECSMasterMode::getAssociatedStationModes()
	{
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			
			std::vector<IECSStationMode*> vectStationMode;
			vectStationMode.clear();
			return vectStationMode;
		}
		return m_ECSMasterModeHelper->getAssociatedStationModes();
	}

    std::vector<unsigned long> ConfigECSMasterMode::getAssociatedStationModeKeys()
    {
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );

			std::vector<unsigned long> vectStationModeKey;
			vectStationModeKey.clear();
			return vectStationModeKey;
		}
		return m_ECSMasterModeHelper->getAssociatedStationModeKeys();
    }

	
    void ConfigECSMasterMode::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSMasterMode::invalidate");
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return ;
		}

        m_ECSMasterModeHelper->invalidate();
        m_ECSMasterModeChanges.clear();
        m_newStationModes = m_originalStationModes;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigECSMasterMode::invalidate");
    }


    void ConfigECSMasterMode::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigECSMasterMode::deleteThisObject");
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The ECSMasterModeHelper pointer is null." );
			return ;
		}

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
		if (NULL == m_ECSMasterModeHelper)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "applyChanges() has been called after ConfigECSMasterMode was deleted" );
			return ;
		}

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

