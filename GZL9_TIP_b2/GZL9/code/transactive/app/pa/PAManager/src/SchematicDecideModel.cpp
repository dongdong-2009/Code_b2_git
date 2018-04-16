/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#include "app/pa/PAManager/src/stdafx.h"

#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/src/SchematicDisplayMapAccessFactory.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/SchematicDecideModel.h"

unsigned long SchematicDecideModel::s_ulSchematicLocationKey 	= UNINITIALIZE_UNSIGNED_LONG;
unsigned long SchematicDecideModel::s_ulSubsystemKey 			= UNINITIALIZE_UNSIGNED_LONG;

SchematicDecideModel::SchematicDecideModel()
{
}

SchematicDecideModel::~SchematicDecideModel()
{
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
        PaErrorHelper::logExceptionCatch( SourceInfo, expWhat.what() );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION, "SchematicDisplayMapAccessFactory::getSchematicDisplay" );
    }

    if ( strFileName.empty() )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_NO_SCHEMATIC_FILE_NAME_IN_DATABASE );
    }

    return strFileName.c_str();
}

void SchematicDecideModel::setSchematicLocationKey( unsigned long ulLocationKey )
{
    s_ulSchematicLocationKey = ulLocationKey;
}

void SchematicDecideModel::setSubsystemKey( unsigned long ulSubsystem )
{
    s_ulSubsystemKey = ulSubsystem;
}