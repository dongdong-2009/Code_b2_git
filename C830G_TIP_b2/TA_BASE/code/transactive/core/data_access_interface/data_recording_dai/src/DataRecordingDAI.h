#ifndef DATA_RECORDING_DAI_H
#define DATA_RECORDING_DAI_H
/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/data_recording_dai/src/DataRecordingDAI.h $
 * @author:  Jade Welton
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 * 
 * Common defines and functions for the Data Recording DAI
 *
 */

#include <string>


namespace TA_Base_Core
{
    namespace DataRecordingDAI
    {
        /**
         * getReadTableName
         *
         * @param   typeName    the name of an entity type
         * @return  name of table/view to read state data from
         *
         */
        static std::string getReadTableName( const std::string& typeName )
        {
            return "DR_" + typeName + "_STATE";
        }


        /**
         * getWriteTableName
         *
         * @param   typeName    the name of an entity type
         * @return  name of table/view to write state data to
         *
         */
        static std::string getWriteTableName( const std::string& typeName )
        {
            return "DR_" + typeName + "_WRITE_V";
        }


        /**
         * Common field names for state data tables
         */
        static const std::string FIELD_ENTITYKEY    = "ENTITYKEY";
        static const std::string FIELD_SOURCETIME   = "SOURCETIME";
        static const std::string FIELD_UPDATETIME   = "UPDATETIME";
        static const std::string FIELD_QUALITY      = "QUALITY";


        /**
         * Value constants
         */
        static const std::string VALUE_TRUE         = "1";
        static const std::string VALUE_FALSE        = "0";
    }
}

#endif // DATA_RECORDING_DAI_H
