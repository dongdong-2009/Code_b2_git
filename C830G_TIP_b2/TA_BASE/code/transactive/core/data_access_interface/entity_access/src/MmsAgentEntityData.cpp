/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/entity_access/src/MmsAgentEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Implementataion of MmsAgentEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string MmsAgentEntityData::ENTITY_TYPE       = "MmsAgent";
	const std::string MmsAgentEntityData::SERVER_ADDRESS = "ServerAddress";
	const std::string MmsAgentEntityData::SERVER_GROUP_ID = "ServerQueueGroupId";
	const std::string MmsAgentEntityData::SERVER_QUEUE_ID = "ServerQueueId";
	const std::string MmsAgentEntityData::CLIENT_GROUP_ID = "ClientQueueGroupId";
	const std::string MmsAgentEntityData::CLIENT_QUEUE_ID = "ClientQueueId";
	const std::string MmsAgentEntityData::ASSET_NAME = "AssetName";

    MmsAgentEntityData::MmsAgentEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }


    MmsAgentEntityData::~MmsAgentEntityData()
    { // Nothing to clean up
    }


    IEntityData* MmsAgentEntityData::clone(unsigned long key)
    {
        return new MmsAgentEntityData(key);        
    }


    void MmsAgentEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string MmsAgentEntityData::getType()
    {
        return getStaticType();
    }


    std::string MmsAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    std::string MmsAgentEntityData::getServerAddress()
    {
		std::string serverAddress = "175.124.10.50";
		std::string value = getHelper()->getParameter(SERVER_ADDRESS);
		if (!value.empty())
		{
			serverAddress = value;
		}

        return serverAddress;
    }


    std::string MmsAgentEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }


    unsigned int MmsAgentEntityData::getServerQueueId()
    {
        unsigned int serverQueueId = 120;
		std::string value = getHelper()->getParameter(SERVER_QUEUE_ID);
		if (!value.empty())
		{
			serverQueueId = EntityHelper::getIntegerData(value);
		}

        return serverQueueId;
    }


	unsigned int MmsAgentEntityData::getServerGroupId()
    {
        unsigned int serverGroupId = 1;

		std::string value = getHelper()->getParameter(SERVER_GROUP_ID);
		if (!value.empty())
		{
			serverGroupId = EntityHelper::getIntegerData(value);
		}

        return serverGroupId;
    }


	unsigned int MmsAgentEntityData::getClientQueueId()
    {
        unsigned int clientQueueId = 121;
		std::string value = getHelper()->getParameter(CLIENT_QUEUE_ID);
		if (!value.empty())
		{
			clientQueueId = EntityHelper::getIntegerData(value);
		}
        return clientQueueId;
    }


	unsigned int MmsAgentEntityData::getClientGroupId()
    {
        unsigned int clientGroupId = 1;

		std::string value = getHelper()->getParameter(CLIENT_GROUP_ID);
		if (!value.empty())
		{
			clientGroupId = EntityHelper::getIntegerData(value);
		}

        return clientGroupId;
    }

}
