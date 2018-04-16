/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/core/data_access_interface/ats/src/ConfigAtsDssPlanMap.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * ConfigAtsDssPlanMap is a concrete implementation of IConfigAtsDssPlanMap, which is in turn an implementation
  * of IAtsDssPlanMap. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for AtsDssPlanMaps.
  */

#if defined(_MSC_VER)
	#pragma warning(disable:4786)	// disable the "trucated to 255 character" warning
	#pragma warning(disable:4290)	// C++ Exception Specification ignored
#endif // defined _MSC_VER

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/ats/src/ConfigAtsDssPlanMap.h"
#include "core/data_access_interface/ats/src/AtsDssPlanMapHelper.h"
#include "core/data_access_interface/plans/src/PlanAccessFactory.h"
#include "core/data_access_interface/plans/src/IPlan.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_IRS_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
	static const std::string DSSEVENT          = "DSS Event";
    static const std::string PLANPATH		   = "Plan";
    
    long ConfigAtsDssPlanMap::s_nextAvailableIdentifier = 0;

    ConfigAtsDssPlanMap::ConfigAtsDssPlanMap()
     : m_atsDssPlanMapHelper( new AtsDssPlanMapHelper() ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }


    ConfigAtsDssPlanMap::ConfigAtsDssPlanMap(const unsigned long idKey)
     : m_atsDssPlanMapHelper( new AtsDssPlanMapHelper(idKey) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsDssPlanMap::ConfigAtsDssPlanMap( const ConfigAtsDssPlanMap& theConfigAtsDssPlanMap)
     : m_atsDssPlanMapHelper( new AtsDssPlanMapHelper( *(theConfigAtsDssPlanMap.m_atsDssPlanMapHelper)) ),
       m_uniqueIdentifier( ++s_nextAvailableIdentifier )
    {
    }

    ConfigAtsDssPlanMap::ConfigAtsDssPlanMap(unsigned long row, TA_Base_Core::IData& data)
     : m_atsDssPlanMapHelper( new AtsDssPlanMapHelper(row, data)),
       m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
    }

    ConfigAtsDssPlanMap::~ConfigAtsDssPlanMap()
    {
        try
        {
            delete m_atsDssPlanMapHelper;
            m_atsDssPlanMapHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigAtsDssPlanMap::isNew()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");

        return m_atsDssPlanMapHelper->isNew();
    }

    void ConfigAtsDssPlanMap::setName(const std::string& name)
    {
		TA_ASSERT(false ,"setName() should not be called.");
    }

    void ConfigAtsDssPlanMap::setDssEvent(unsigned long dssEvent)
    {
        FUNCTION_ENTRY("setDssEvent");
        
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        
		std::ostringstream oldStation, newStation;
		oldStation << m_atsDssPlanMapHelper->getDssEvent();
		newStation << dssEvent;

        updateAtsDssPlanMapChanges(DSSEVENT, oldStation.str().c_str(), newStation.str().c_str());

        m_atsDssPlanMapHelper->setDssEvent(dssEvent);

        FUNCTION_EXIT;
    }

	void ConfigAtsDssPlanMap::setPlanPath(const std::string& planPath)
    {
        FUNCTION_ENTRY("setPlanPath");
        
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
    
        updateAtsDssPlanMapChanges(PLANPATH, m_atsDssPlanMapHelper->getPlanPath(), planPath);

        m_atsDssPlanMapHelper->setPlanPath(planPath);

        FUNCTION_EXIT;
    }

	unsigned long ConfigAtsDssPlanMap::getKey()
	{
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getKey();
    }

    unsigned long ConfigAtsDssPlanMap::getDssEvent()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getDssEvent();
    }


	std::string ConfigAtsDssPlanMap::getPlanPath()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getPlanPath();
    }


    std::string ConfigAtsDssPlanMap::getName()
    {
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");
        return m_atsDssPlanMapHelper->getName();
    }
 
	
    void ConfigAtsDssPlanMap::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsDssPlanMap::invalidate");
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");

        m_atsDssPlanMapHelper->invalidate();
        m_atsDssPlanMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsDssPlanMap::invalidate");
    }


    void ConfigAtsDssPlanMap::deleteThisObject(bool cascade)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsDssPlanMap::deleteThisObject");
        TA_ASSERT(m_atsDssPlanMapHelper != NULL,"The AtsDssPlanMapHelper pointer is null.");

        // Delete the AtsDssPlanMap from the database
        m_atsDssPlanMapHelper->deleteAtsDssPlanMap(cascade);
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete AtsDssPlanMapHelper pointer m_atsDssPlanMapHelper");

        delete m_atsDssPlanMapHelper;
        m_atsDssPlanMapHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsDssPlanMap::deleteThisObject");
    }


    void ConfigAtsDssPlanMap::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigAtsDssPlanMap::applyChanges");
        TA_ASSERT(m_atsDssPlanMapHelper != NULL, "applyChanges() has been called after AtsDssPlanMap was deleted");

        m_atsDssPlanMapHelper->writeAtsDssPlanMapData();

        m_atsDssPlanMapChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigAtsDssPlanMap::applyChanges");
    }


    void ConfigAtsDssPlanMap::updateAtsDssPlanMapChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateAtsDssPlanMapChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_atsDssPlanMapChanges.find(name);

        if (matching != m_atsDssPlanMapChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_atsDssPlanMapChanges.erase(matching);
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
            m_atsDssPlanMapChanges.insert( TA_Base_Core::ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
} // closes TA_Core

