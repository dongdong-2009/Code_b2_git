
#include "VideoDisplayManagerDAIImpl.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/entity_access/src/Sequence.h"
#include "core/data_access_interface/entity_access/src/VideoMonitor.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::IEntityDataList;
using TA_Base_Core::EntityAccessFactory;

namespace TA_IRS_App
{
    VideoDisplayManagerDAIImpl ::VideoDisplayManagerDAIImpl(void)
    {
    }

    VideoDisplayManagerDAIImpl::~VideoDisplayManagerDAIImpl()
    {
    }

    TA_Base_Core::ILocation*  VideoDisplayManagerDAIImpl::getLocation(const std::string& sessionID)
    {
		TA_Base_Core::ILocation* location = NULL;
        try
        {
            TA_Base_Core::IConsole* console =
                TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
            location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(console->getLocation());
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
		return location;
    }

    TA_Base_Core::ILocation::ELocationType VideoDisplayManagerDAIImpl::getLocationType(const std::string& sessionID)
    {
        using TA_Base_Core::DataException;
        using TA_Base_Core::DatabaseException;
		TA_Base_Core::ILocation* location = getLocation(sessionID);
		return location != NULL ? location->getLocationType() : TA_Base_Core::ILocation::OCC;
    }

    IEntityDataList VideoDisplayManagerDAIImpl::getSequences(unsigned long locationKey)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading sequences");
        IEntityDataList sequenceEntities;
        try
        {
            sequenceEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::Sequence::getStaticType(), locationKey);
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        return sequenceEntities;
    }

    std::vector<TA_Base_Core::ILocation*> VideoDisplayManagerDAIImpl::getAllLocations()
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loading locations");
        std::vector<TA_Base_Core::ILocation*> locationEntities;
        try
        {
            locationEntities = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        return locationEntities;
    }

    IEntityDataList VideoDisplayManagerDAIImpl::getMonitors(unsigned long locationKey)
    {
        IEntityDataList monitorEntities;
        try
        {
            monitorEntities = EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::VideoMonitor::getStaticType(), locationKey);
        }
        catch(const TA_Base_Core::DatabaseException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", e.what());
        }
        catch(const TA_Base_Core::DataException& e)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", e.what());
        }
        return monitorEntities;
    }
}
