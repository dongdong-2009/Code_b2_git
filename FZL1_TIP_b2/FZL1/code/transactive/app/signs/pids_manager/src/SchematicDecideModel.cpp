#include "stdafx.h"

//#include "core/data_access_interface/pa_4669/src/PaStationAccessFactory.h"

#include "PidManagerConsts.h"
//#include "PaDataManager.h"
//#include "app/pa/PAManager/src/CachedConfig.h"
#include "SchematicDecideModel.h"
#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

unsigned long SchematicDecideModel::s_ulSchematicLocationKey = UNINITIALIZE_UNSIGNED_LONG;
unsigned long SchematicDecideModel::s_ulSubsystemKey = UNINITIALIZE_UNSIGNED_LONG;

SchematicDecideModel::SchematicDecideModel()
{
}

SchematicDecideModel::~SchematicDecideModel()
{
    m_mapStationToSchematicFile.clear();
}

std::string SchematicDecideModel::getSchematicFileName()
{
    if (( UNINITIALIZE_UNSIGNED_LONG == s_ulSchematicLocationKey ) || ( UNINITIALIZE_UNSIGNED_LONG == s_ulSubsystemKey ))
    {
        return EMPTY_STRING;
    }

    std::string strFileName = EMPTY_STRING;
    try
    {
        strFileName = TA_Base_Core::SchematicDisplayMapAccessFactory::getInstance().getSchematicDisplay( s_ulSchematicLocationKey, s_ulSubsystemKey );
    }
    catch ( TA_Base_Core::DataException& expWhat )
    {
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", expWhat.what());
    }
    catch (TA_Base_Core::DatabaseException& expWhat)
    {
		LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataBaseException", expWhat.what());
    }
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Schematic path:%s", strFileName.c_str());
    return strFileName;
}

void SchematicDecideModel::setSchematicLocationKey( unsigned long ulLocationKey )
{
    s_ulSchematicLocationKey = ulLocationKey;
}


void SchematicDecideModel::setSubsystemKey( unsigned long ulSubsystem )
{
    s_ulSubsystemKey = ulSubsystem;
}