/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/data_access_interface/entity_access/src/Console.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * 
  * Console is a concrete implementation of IConsole, which is in turn an implementation of
  * IEntityData. It proves read-only access to Console data, and a specific framework
  * for Console parameters.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/Console.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string Console::ENTITY_TYPE              = "Console";
    const std::string Console::ACCESSLEVEL              = "AccessLevel";
    const std::string Console::PAGING_CONSOLE_SOURCE_ID = "PagingConsoleSourceId";
    const std::string Console::PAGING_CONSOLE_ZONE_ID   = "PagingConsoleZoneId";
	const std::string Console::TRAIN_ORIGIN             = "TrainOrigin";


    Console::Console(unsigned long key)
		: EntityData(key, getType())
    {
    }


    Console::~Console()
    {
    }

    
    std::string Console::getType()
    {
        return getStaticType();
    }


	std::string Console::getStaticType()
	{
		return ENTITY_TYPE;
	}


    unsigned long Console::getAccessLevel()
    {
		std::string value = getHelper()->getParameter(ACCESSLEVEL);
		unsigned long accessLevel = 0;
		if (!value.empty())
		{
			accessLevel = EntityHelper::getUnsignedLongData(value);
		}

        return accessLevel;
    }


    unsigned long Console::getPagingConsoleSourceId()
    {
		std::string value = getHelper()->getParameter(PAGING_CONSOLE_SOURCE_ID);
		unsigned long pagingConsoleSourceId = 0;
		if (!value.empty())
		{
			pagingConsoleSourceId = EntityHelper::getUnsignedLongData(value);
		}

        return pagingConsoleSourceId;
    }


    unsigned long Console::getPagingConsoleZoneId()
    {
		std::string value = getHelper()->getParameter(PAGING_CONSOLE_ZONE_ID);
		unsigned long pagingConsoleZoneId = 0;
		if (!value.empty())
		{
			pagingConsoleZoneId = EntityHelper::getUnsignedLongData(value);
		}
		
        return pagingConsoleZoneId;
    }


	unsigned long Console::getTrainOrigin()
	{
		std::string value = getHelper()->getParameter(TRAIN_ORIGIN);
		unsigned long trainOrigin = 0;
		if (!value.empty())
		{
			trainOrigin = EntityHelper::getUnsignedLongData(value);
		}

        return trainOrigin;
	}

    
    IEntityData* Console::clone(unsigned long key)
    {
        return new Console(key);        
    }


    void Console::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

