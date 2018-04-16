/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/ManagedProcessHelper.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#include "ManagedProcessHelper.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_Core::IDatabase;
using TA_Base_Core::DataException;

namespace TA_Base_Core
{

    ManagedProcessHelper::ManagedProcessHelper(unsigned long systemControllerKey,
                             unsigned long managedProcessKey)
        : m_isValidData(false), m_snsconId(systemControllerKey), 
          m_managedProcessPkey(managedProcessKey), 
          m_defaultOperatingMode(TA_Base_Core::NotApplicable)
    {
    }

    ManagedProcessHelper::~ManagedProcessHelper()
    {
    }

    unsigned long ManagedProcessHelper::getSystemControllerKey()
    {
        return m_snsconId;
    }

    unsigned long ManagedProcessHelper::getManagedProcessKey()
    {
        return m_managedProcessPkey;
    }

    EOperationMode ManagedProcessHelper::getDefaultOperatingMode()
    {        
        if (!m_isValidData) 
        {
            reload();
        }
        return m_defaultOperatingMode;
    }

    void ManagedProcessHelper::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        // Set the flag to false
        m_isValidData = false;

        FUNCTION_EXIT;
    }

    void ManagedProcessHelper::reload()
    {
        FUNCTION_ENTRY("reload");

        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(SysController_Cd, Read);
        
        // Create the SQL string to retrieve the data of the system controller
        // based upon the key
        char sql[256] = {0};
        sprintf(sql, "SELECT default_operating_mode FROM sn_managed_process WHERE snscon_id = %lu AND managed_process_pkey = %lu", 
                        m_snsconId, m_managedProcessPkey);

        std::string defaultOperatingModeColumn = "DEFAULT_OPERATING_MODE";
        std::vector<std::string> columnNames;
        columnNames.push_back(defaultOperatingModeColumn);

        // Execute the query. The method can throw a DatabaseException.
        // We are responsible for deleting the returned IData object when we're 
        // done with it.
        IData* data = databaseConnection->executeQuery(sql,columnNames);

        // Need to bring in the DataException
        using TA_Base_Core::DataException;

        if (0 == data->getNumRows()) // No entry found with the specified pkey
        {
            char reason[256] = {0};
			sprintf(reason, "No managed process found for snscon_id = %lu and managed_process_pkey = %lu", 
                    m_snsconId, m_managedProcessPkey);			
            TA_THROW(DataException(reason, DataException::NO_VALUE, ""));
        }
        else if (1 < data->getNumRows()) // More than one entry found for the pkey
        {
            char reason[256] = {0};
			sprintf(reason, "More than one System Controller found for snscon_id = %lu and managed_process_pkey = %lu", 
                    m_snsconId, m_managedProcessPkey);			
            TA_THROW(DataException(reason, DataException::NOT_UNIQUE, ""));
        }

        // Assign the operating mode to a member variable.
        // This can throw a DataException.
        unsigned long mode = data->getUnsignedLongData(0, defaultOperatingModeColumn);
        switch (mode)
        {
        case 0:
            m_defaultOperatingMode = TA_Base_Core::Control;
            break;
        case 1:
            m_defaultOperatingMode = TA_Base_Core::Monitor;
            break;
        default:
            TA_THROW(DataException("Default operating mode set to neither Control or Monitor",
                DataException::WRONG_TYPE, ""));
            break;
        }

        // Now that we're done with the IData object, it's our responsibility to 
        // delete it.
        delete data;
        data = NULL;
        
        // Need to record that we now have valid data.
        m_isValidData = true; 

        FUNCTION_EXIT;
    }
}
