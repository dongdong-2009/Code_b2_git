#if !defined(CameraPresetsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
#define CameraPresetsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/core/data_access_interface/video_switch_agent/src/CameraPresetsData.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This class contains the database retrievals and writes for Camera presets
  * for the Video Switch Agent.
  * It implements the ICameraPresets interface.
  */

#include <map>
#include "core/data_access_interface/video_switch_agent/src/ICameraPresets.h"

namespace TA_IRS_Core
{
    class CameraPresetsData : public ICameraPresets
    {
    public:

        /**
         * CameraPresetsData
         *
         * Standard constructor.
         *
         * @param       unsigned long cameraKey
         *              The entity key of the Camera to retrieve the presets for.
         */

        CameraPresetsData( unsigned long cameraKey );

        /**
         * ~CameraPresetsData
         *
         * Standard destructor.
         */

        virtual ~CameraPresetsData() {};
        
        /**
         * getAllPresets
         *
         * Gets all the presets for this Camera.
         *
         * @return      PresetMap
         *              The map of preset indices to names.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        virtual PresetMap getAllPresets();

        /**
         * getPresetName
         *
         * Gets the name of the preset with the given index.
         *
         * @return      std::string
         *              The name of the preset.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   TA_Base_Core::DataException
         *              Thrown if:
         *              1. The preset index was not found.
         *              2. The data cannot be converted to the correct format.
         */

        virtual std::string getPresetName( unsigned short presetIndex );

        /**
         * setPresetName
         *
         * Sets the name of the preset with the given index.  If the index does
         * not exist in the database already, the index is added.  If it already
         * exists, the existing entry is updated.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset.
         * @param       const std::string& presetName
         *              The name of the preset.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the data cannot be converted to the correct format.
         */

        virtual void setPresetName( unsigned short presetIndex, const std::string& presetName );

        /**
         * addPreset
         *
         * Adds the given preset.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset.
         * @param       const std::string& presetName
         *              The name of the preset.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception   TA_Base_Core::DataException
         *              Thrown if:
         *              1. A preset with the given index already exists.
         *              2. The data cannot be converted to the correct format.
         */

        virtual void addPreset( unsigned short presetIndex, const std::string& presetName );
        
        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate();

    private:
        //
        // The copy constructor and assignment operator aren't needed so they are made
        // private here to hide them and prevent their use.
        //
        CameraPresetsData& operator=( const CameraPresetsData& );
        CameraPresetsData( const CameraPresetsData& );

        /**
         * reload
         *
         * This loads all settings from the database.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         * @exception DatabaseException - This is thrown if there is some error with the database
         *                                connection or an SQL statement failed to execute for
         *                                some reason.
         * @exception DataUserSettingException - This is thrown if duplicate settings appear in
         *                                       the database.
         */

        void reload();

        /**
         * updatePresetInDatabase
         *
         * This updates a preset name in the database.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset to update.
         * @param       const std::string& presetName
         *              The new name of the preset.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         */

        void updatePresetInDatabase( unsigned short presetIndex, const std::string& presetName );

        /**
         * insertPresetIntoDatabase
         *
         * This inserts a new preset into the database.
         *
         * @param       unsigned short presetIndex
         *              The index of the preset to insert.
         * @param       const std::string& presetName
         *              The new preset's name.
         *
         * @exception   DatabaseException
         *              Thrown if:
         *              1. There is some error with the database connection
         *              2. An SQL statement failed to execute for some reason.
         *
         */

        void insertPresetIntoDatabase( unsigned short presetIndex, const std::string& presetName );

        //
        // Holds all the current, configured presets retrieved from the
        // database
        //
        ICameraPresets::PresetMap m_presets;

        //
        // We want to store the Camera entity key so that when we go to
        // save the presets later we use the key.
        //
        unsigned long m_cameraKey;

        //
        // This indicates whether we currently have valid data or not. This is
        // initialised to false and is set to true once we do a read from the
        // database.
        //
        bool m_isValidData;
    };

} //end namespace TA_IRS_Core

#endif // !defined(CameraPresetsData_B2E4FE90_8712_428a_B841_74FEA9709328__INCLUDED_)
