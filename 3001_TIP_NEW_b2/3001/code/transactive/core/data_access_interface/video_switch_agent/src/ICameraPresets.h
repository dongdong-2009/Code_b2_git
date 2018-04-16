#if !defined(ICameraPresets_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
#define ICameraPresets_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/core/data_access_interface/video_switch_agent/src/ICameraPresets.h $
  * @author Katherine Thomson
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/02/10 15:24:16 $
  * Last modified by:  $Author: weikun.lin $
  * 
  * This interface contains the database retrieval and write methods
  * for Camera presets for the Video Switch Agent.
  */

#include <string>
#include <map>

namespace TA_IRS_Core
{
    class ICameraPresets
    {
    public:

        //
        // Map of preset index to name.
        //
         
        typedef std::map< unsigned short, std::string > PresetMap;

        /**
         * ~ICameraPresets
         *
         * Standard destructor.
         */

        virtual ~ICameraPresets() {};

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

        virtual PresetMap getAllPresets() = 0;

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

        virtual std::string getPresetName( unsigned short presetIndex ) = 0;

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

        virtual void setPresetName( unsigned short presetIndex, const std::string& presetName ) = 0;
        
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

        virtual void addPreset( unsigned short presetIndex, const std::string& presetName ) = 0;
                    
        /**
          * invalidate
          *
          * Marks the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */

        virtual void invalidate() = 0;

    };

} //end namespace TA_IRS_Core

#endif // !defined(ICameraPresets_23951DDF_A220_48c9_9F59_DF77937A06A7__INCLUDED_)
