/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsData.cpp $
  * @author Katherine Thomson
  * @version $Revision: #4 $
  * Last modification: $DateTime: 2012/12/27 17:54:14 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * This class contains the database retrievals and writes for Camera presets
  * for the Video Switch Agent.
  * It implements the ICameraPresets interface.
  */

#pragma warning( disable : 4786 )

#include <string>
#include <sstream>

#include "core/data_access_interface/video_switch_agent/src/CameraPresetsData.h"
#include "core/data_access_interface/video_switch_agent/src/CameraPresetsTable.h"
#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_Core
{

    //
    // CameraPresetsData
    //
    CameraPresetsData::CameraPresetsData( unsigned long m_cameraKey )
        : m_cameraKey( m_cameraKey ),
          m_isValidData( false )
    {
        // Do nothing.
    }   

    
    //
    // getAllPresets
    //
    ICameraPresets::PresetMap CameraPresetsData::getAllPresets()
    {
        if ( !m_isValidData )
        {
            reload();
        }
        return m_presets;
    }


    //
    // getPresetName
    //
    std::string CameraPresetsData::getPresetName( unsigned short presetIndex )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "getPresetName");

        if ( !m_isValidData )
        {
            reload();
        }

        PresetMap::iterator it = m_presets.find( presetIndex );
        if ( it == m_presets.end() )
        {
            std::stringstream error;
            error << "Preset " << presetIndex << "for the Camera with key " << m_cameraKey;
            error << " could not be found.";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), 
                TA_Base_Core::DataException::NO_VALUE, 
                TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN ) );
        }            

        LOG_FUNCTION_EXIT( SourceInfo, "getPresetName");
        return it->second;
    }


    //
    // setPresetName
    //
    void CameraPresetsData::setPresetName( unsigned short presetIndex, const std::string& presetName )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "setPresetName");

        if ( !m_isValidData )
        {
            reload();
        }

        PresetMap::iterator it = m_presets.find( presetIndex );
        if ( it != m_presets.end() )
        {
            updatePresetInDatabase( presetIndex, presetName );
            it->second = presetName;
        }
        else
        {
            std::stringstream error;
            error << "Preset " << presetIndex << "for the Camera with key " << m_cameraKey;
            error << " does not exist. The preset name cannot be set";
            TA_THROW( TA_Base_Core::DataException( error.str().c_str(), 
                        TA_Base_Core::DataException::NO_VALUE, 
                        TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN ) );
        }

        LOG_FUNCTION_EXIT( SourceInfo, "setPresetName");
    }


    //
    // addPreset
    //
    void CameraPresetsData::addPreset( unsigned short presetIndex, const std::string& presetName )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "addPreset");

        if ( !m_isValidData )
        {
            reload();
        }

        PresetMap::iterator it = m_presets.find( presetIndex );
        if ( it != m_presets.end() )
        {
            // The preset already exists. The hardware will store the new settings.
            // Just rename the existing preset in the DB to the specified name.
            setPresetName( presetIndex, presetName );
            LOG_FUNCTION_EXIT( SourceInfo, "addPreset");
            return;
        }
        else
        {
            insertPresetIntoDatabase( presetIndex, presetName );
        }

        m_presets[ presetIndex ] = presetName;

        LOG_FUNCTION_EXIT( SourceInfo, "addPreset");
    }

    
    //
    // invalidate
    //
    void CameraPresetsData::invalidate()
    {
        m_isValidData = false;
        FUNCTION_ENTRY("invalidate");
    }


    //
    // reload
    //
    void CameraPresetsData::reload()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "reload");

       /* std::stringstream sql;
        sql << "select " << TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN;
        sql << "," << TA_VideoSwitchAgentDAI::PRESET_NAME_COLUMN;
        sql << " from " << TA_VideoSwitchAgentDAI::CAMERA_PRESETS_TABLE_NAME;
        sql << " where " << TA_VideoSwitchAgentDAI::CAMERA_KEY_COLUMN;
        sql << "=" << m_cameraKey;	*/	

        // Set up the columnNames vector to be passed to executeQuery().
        
        std::vector< std::string > columnNames;
        columnNames.push_back( TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN );
        columnNames.push_back( TA_VideoSwitchAgentDAI::PRESET_NAME_COLUMN );

        // Execute the query. The method can throw a DatabaseException.
        // This is documented in the comment of this method.
        // We are responsible for deleting the returned TA_Base_Core::IData object when we're 
        // done with it.

        TA_Base_Core::IDatabase *pDB = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Read);
        TA_Assert(pDB != NULL);
//		std::string strSql = defPrepareSQLStatement(pDB->GetDbServerType(), VI_CAMERA_PRESETS_STD_SELECT_61001, m_cameraKey);
//		std::string strSql  = pDB->prepareSQLStatement(VI_CAMERA_PRESETS_SELECT_61001, m_cameraKey);
		TA_Base_Core::SQLStatement strSql;
		pDB->prepareSQLStatement(strSql, VI_CAMERA_PRESETS_SELECT_61001, m_cameraKey);

        TA_Base_Core::IData* data = pDB->executeQuery( strSql, columnNames );

        do // loop for each lot of TA_Base_Core::IData that has been retrieved
        {
            for ( unsigned long row = 0; row < data->getNumRows(); ++row )
            {
                // Retrieve the preset index.

                unsigned short presetIndex = data->getIntegerData( 
                    row, TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN );

                // Check that presetIndex does not already exist in the map. If it does we
                // have an error so throw an exception.

                PresetMap::iterator it = m_presets.find( presetIndex );
                if ( it != m_presets.end() )
                {
                    // This setting is a duplicate. This is an error so throw an exception.

                    std::stringstream error;
                    error << "A duplicate preset index (" << presetIndex;
                    error << ") was found in the database for the Camera with key ";
                    error << m_cameraKey << ".";
                    TA_THROW( TA_Base_Core::DataException( error.str().c_str(), 
                        TA_Base_Core::DataException::NOT_UNIQUE, 
                        TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN ) );
                }

                // Retrieve the preset name.

                std::string presetName = data->getStringData(
                    row, TA_VideoSwitchAgentDAI::PRESET_NAME_COLUMN );
        
                // Add the preset to the map.

                m_presets.insert( PresetMap::value_type( presetIndex, presetName ) );
                LOGMORE( SourceInfo, "index: %d Name: %s", presetIndex, presetName.c_str() );
            }

            // Delete ready to retrieve the next lot of data.

            delete data;
            data = 0;

        } while ( pDB->moreData( data ) );

        m_isValidData = true;

        LOG_FUNCTION_EXIT( SourceInfo, "reload");
    }


    //
    // updatePresetInDatabase
    //
    void CameraPresetsData::updatePresetInDatabase( unsigned short presetIndex, const std::string& presetName )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "updatePresetInDatabase");

        TA_Base_Core::IDatabase* pDB = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);
        TA_Assert(pDB != NULL);

      /*  std::stringstream sql;
        sql << "update " << TA_VideoSwitchAgentDAI::CAMERA_PRESETS_TABLE_NAME;
        sql << " set " << TA_VideoSwitchAgentDAI::PRESET_NAME_COLUMN;
        sql << "='" << pDB->escapeInsertString( presetName );
        sql << "' where " << TA_VideoSwitchAgentDAI::CAMERA_KEY_COLUMN << "=" << m_cameraKey;
        sql << " and " << TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN << "=" << presetIndex;*/
//		std::string strSql = defPrepareSQLStatement(pDB->GetDbServerType(), VI_CAMERA_PRESETS_STD_UPDATE_61151, 
//		std::string strSql  = pDB->prepareSQLStatement(VI_CAMERA_PRESETS_UPDATE_61151, 
		TA_Base_Core::SQLStatement strSql;
		pDB->prepareSQLStatement(strSql, VI_CAMERA_PRESETS_UPDATE_61151,
			pDB->escapeInsertString( presetName ), m_cameraKey, presetIndex);

        // Execute the modification. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        pDB->executeModification( strSql );

        LOG_FUNCTION_EXIT( SourceInfo, "updatePresetInDatabase");
    }


    //
    // insertPresetIntoDatabase
    //
    void CameraPresetsData::insertPresetIntoDatabase( unsigned short presetIndex, const std::string& presetName )
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "insertPresetIntoDatabase");

        TA_Base_Core::IDatabase* pDB = 
            TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Video_Cd, TA_Base_Core::Write);

        TA_Assert(pDB != NULL);

       /* std::stringstream sql;
        sql << "insert into " << TA_VideoSwitchAgentDAI::CAMERA_PRESETS_TABLE_NAME << " (" ;
        sql << TA_VideoSwitchAgentDAI::CAMERA_KEY_COLUMN << ", ";
        sql << TA_VideoSwitchAgentDAI::PRESET_INDEX_COLUMN << ", ";
        sql << TA_VideoSwitchAgentDAI::PRESET_NAME_COLUMN << ") values (";
        sql << m_cameraKey << ", ";
        sql << presetIndex << ", '";
        sql << pDB->escapeInsertString( presetName ) << "')";*/
//		std::string strSql = defPrepareSQLStatement(pDB->GetDbServerType(), VI_CAMERA_PRESETS_STD_INSERT_61301, m_cameraKey,
//		std::string strSql  = pDB->prepareSQLStatement(VI_CAMERA_PRESETS_INSERT_61301, m_cameraKey,
		TA_Base_Core::SQLStatement strSql;
		pDB->prepareSQLStatement(strSql, VI_CAMERA_PRESETS_INSERT_61301, m_cameraKey,
			presetIndex, pDB->escapeInsertString( presetName ));

        // Execute the insertion. The method can throw a DatabaseException.
        // This is documented in the comment of this method.

        pDB->executeModification( strSql );

        LOG_FUNCTION_EXIT( SourceInfo, "insertPresetIntoDatabase");
    }

} //end namespace TA_IRS_Core
