/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/RadioEntityData.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * 
  * RadioEntityData is a concrete implementation of IRadioEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to Radio data, and a specific framework
  * for Radio parameters.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/RadioEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string RadioEntityData::ENTITY_TYPE          = "Radio";
    const std::string RadioEntityData::PRIMARY_SERVERS      = "PrimaryServers";
    const std::string RadioEntityData::SECONDARY_SERVERS = "SecondaryServers";
    const std::string RadioEntityData::XON_XOFF_TIMEOUT = "XONXOFFTimeout";
	const std::string RadioEntityData::ASSET_NAME = "AssetName";
	//TD10963 marc_bugfix support for default volume parameter
	const std::string RadioEntityData::DEFAULT_VOLUME = "DefaultVolume";

    RadioEntityData::RadioEntityData(unsigned long key)
		: EntityData(key, getType())
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "RadioEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "RadioEntityData (constructor)");
    }


    RadioEntityData::~RadioEntityData()
    {
    }


	std::string RadioEntityData::getPrimaryTCPServers()
    {
		return getHelper()->getParameter(PRIMARY_SERVERS);
    }


	std::string RadioEntityData::getSecondaryTCPServers()
    {
		return getHelper()->getParameter(SECONDARY_SERVERS);
    }


    unsigned long RadioEntityData::getXONXOFFTimeout()
    {
		std::string value = getHelper()->getParameter(XON_XOFF_TIMEOUT);
		unsigned long XONXOFFTimeout = 0;
		if (!value.empty())
		{
			XONXOFFTimeout = EntityHelper::getUnsignedLongData(value);
		}

        return XONXOFFTimeout;
    }
	//TD10963 marc_bugfix support for default volume 
    unsigned long RadioEntityData::getDefaultVolume()
    {
		std::string value = getHelper()->getParameter(DEFAULT_VOLUME);
		unsigned long DefaultVolume = 8;
		if (!value.empty())
		{
			DefaultVolume = EntityHelper::getUnsignedLongData(value);
		}

        return DefaultVolume;
    }

	std::string RadioEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }


    std::string RadioEntityData::getType()
    {
        return getStaticType();
    }


	std::string RadioEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}


    IEntityData* RadioEntityData::clone(unsigned long key)
    {
        return new RadioEntityData(key);        
    }


    void RadioEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core

