/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioEntityAccess/src/RadioEntityAccess.cpp $
 * @author:  Robert van Hugten
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2013/01/24 17:57:59 $
 * Last modified by:  $Author: CM $
 * 
 */

#ifdef __WIN32__
#pragma warning(disable:4786)  // identifier was truncated to 'number' characters in the debug information
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "RadioEntityAccess.h"

#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\TAAssert.h"

#include "core\data_access_interface\entity_access\src\IConsole.h"
#include "core\data_access_interface\entity_access\src\ConsoleAccessFactory.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\RadioEntityData.h"
#include "core\data_access_interface\entity_access\src\RadioSessionEntityData.h"

using namespace TA_Base_Core;
using TA_Base_Core::DebugUtil;

RadioEntityAccess::RadioEntityAccess()
{

}

RadioEntityAccess::~RadioEntityAccess()
{

}


unsigned long RadioEntityAccess::getConsoleKey()
{
 	FUNCTION_ENTRY("getConsoleKey");
	unsigned long consoleKey;

    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
	consoleKey = console->getKey();
	delete console;

	FUNCTION_EXIT;
	return consoleKey;
}

unsigned long RadioEntityAccess::getLocationKey()
{
 	FUNCTION_ENTRY("getLocationKey");
	unsigned long locationKey;

	unsigned long consoleKey = getConsoleKey();
	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(consoleKey);
	locationKey = console->getLocation();
	delete console;

	FUNCTION_EXIT;
	return locationKey;
}

unsigned long RadioEntityAccess::getLocationKey(unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getLocationKey");
	unsigned long locationKey;

	TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsole(consoleKey);
	locationKey = console->getLocation();
	delete console;

	FUNCTION_EXIT;
	return locationKey;
}

std::string RadioEntityAccess::getRadioEntityName(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getRadioEntityName");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData* entityData;
	std::string   radioEntityName;

	entityData = getRadioEntity(locationKey);
	radioEntityName = entityData->getName();

	if(entityData != NULL)
		delete entityData;

	FUNCTION_EXIT;
	return radioEntityName;
}


unsigned long RadioEntityAccess::getRadioEntityKey(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getRadioEntityKey");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IEntityData*  entityData;
	unsigned long entitykey;

	entityData = getRadioEntity(locationKey);
	entitykey = entityData->getKey();

	if(entityData != NULL)
		delete entityData;
	FUNCTION_EXIT;
	return entitykey;	
}

IEntityData* RadioEntityAccess::getRadioEntity(unsigned long locationKey)
{
 	FUNCTION_ENTRY("getRadioEntity");
	std::vector<IEntityData*> entityList;

	try
	{
		// Should be just one radio entity at each location
		entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioEntityData::getStaticType(),locationKey);

		TA_ASSERT(entityList.size()>0,"Radio entity not found for this location.");
		TA_ASSERT(entityList.size()<2,"More than one radio entity defined for this location.");
		if(entityList.size() > 0)
			return entityList[0];
		if(entityList.size() >= 2)
		{
			std::ostringstream info;
			info << " Error: More than one entities are founded for location " << locationKey
				 << " size = " << entityList.size();
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, info.str().c_str());

			for( int i=1; i<entityList.size(); i++ )
				delete entityList[i];
		}
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error in geting radio Entity!");
	}
	FUNCTION_EXIT;
	return NULL;
}

std::string RadioEntityAccess::getRadioSessionEntityName(unsigned long locationKey, unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getRadioSessionEntityName");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IRadioSessionEntityData*  radioSessionEntityData;
	std::string   radioSessionEntityName;
	bool entityFound = false;

	radioSessionEntityData = getRadioSessionEntity(locationKey, consoleKey);

	radioSessionEntityName = radioSessionEntityData->getName();

	std::ostringstream info;
	info << " Radio Session Entity found for locationkey( " << locationKey << " ) consolekey( "<<consoleKey <<" )"
		 << " Entity name: " << radioSessionEntityData->getName()
		 << " Entity key: " << radioSessionEntityData->getKey();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, info.str().c_str());

	if(radioSessionEntityData != NULL)
		delete radioSessionEntityData;

	FUNCTION_EXIT;
	return radioSessionEntityName;
}

unsigned long RadioEntityAccess::getRadioSessionEntityKey(unsigned long locationKey, unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getRadioSessionEntityName");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	TA_Base_Core::IRadioSessionEntityData*  radioSessionEntityData;
	unsigned long radioSessionEntityKey;
	bool entityFound = false;

	radioSessionEntityData = getRadioSessionEntity(locationKey, consoleKey);

	radioSessionEntityKey = radioSessionEntityData->getKey();

	std::ostringstream info;
	info << " Radio Session Entity found for locationkey( " << locationKey << " ) consolekey( "<<consoleKey <<" )"
		 << " Entity name: " << radioSessionEntityData->getName()
		 << " Entity key: " << radioSessionEntityData->getKey();
	LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, info.str().c_str());

	if(radioSessionEntityData != NULL)
		delete radioSessionEntityData;

	FUNCTION_EXIT;
	return radioSessionEntityKey;
}

IRadioSessionEntityData* RadioEntityAccess::getRadioSessionEntity(unsigned long locationKey,unsigned long consoleKey)
{
 	FUNCTION_ENTRY("getRadioSessionEntity");
	std::vector<TA_Base_Core::IEntityData*> entityList;
	IRadioSessionEntityData*  radioSessionEntityData;
	std::string   radioSessionEntityName;
	bool entityFound = false;

	// Should be just one radio entity at each location
	try
	{
		entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioSessionEntityData::getStaticType(),locationKey);
		TA_ASSERT(entityList.size()>0,"No radio session entities found for this location.");

		while (!entityList.empty())
		{
			IRadioSessionEntityData* radioSessionEntity = dynamic_cast<TA_Base_Core::IRadioSessionEntityData*>(entityList[0]);
			entityList.erase(entityList.begin());

			TA_ASSERT(radioSessionEntity,"Invalid entity type returned from RasioSession search.");
			if(!radioSessionEntity)
				continue;

			if (radioSessionEntity->getAssociatedConsoleKey() == consoleKey)
			{
				if (entityFound)
				{
					TA_ASSERT(false,"Multiple radio sessions associated with this console.");
					std::ostringstream info;
					info << " Multiple radio sessions associated with this console key: "
						 << consoleKey ;
					LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, info.str().c_str());
				}
				else
				{
					radioSessionEntityData = radioSessionEntity;
					entityFound = true;
					continue;
				}
			}

			delete radioSessionEntity;
		}
	}
	catch(...)
	{
	}
	TA_ASSERT(entityFound,"Radio session not found for this console.");

	FUNCTION_EXIT;
	return radioSessionEntityData;
}