/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/ManagedProcessHelper.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class contains the data associated with one
  * entry in the sn_managed_process table.
  *
  */

#ifndef MANAGEDPROCESSHELPER_H
#define MANAGEDPROCESSHELPER_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{

    class ManagedProcessHelper
    {

    public:
        ManagedProcessHelper(unsigned long systemControllerKey,
                             unsigned long managedProcessKey);

        virtual ~ManagedProcessHelper();

        /** 
          * getSystemControllerKey
          *
          * Returns the key of the System Controller for this entry
          * in the sn_managed_process table (ie the sncon_id column).
          *
          * @return The System Controller's key.
          *
          */
        unsigned long getSystemControllerKey();

        /** 
          * getManagedProcessKey
          *
          * Returns the entity key of the managed process for this entry
          * in the sn_managed_process table.
          *
          * @return The managed process's entity key.
          *
          */
        unsigned long getManagedProcessKey();

        /** 
          * getDefaultOperatingMode
          *
          * Returns the default operating mode for the managed process
          * running on the System Controller in this entry in the
          * sn_managed_process table.
          *
          * @return The operation mode.
          *
          */
        EOperationMode getDefaultOperatingMode();

        /**
          * invalidate
          *
          * Mark the data contained by this object as invalid. The next call to get...() 
          * following a call to invalidate() will cause all the data to be reloaded from
          * the database.
          */
        void invalidate();
        
    private:
        ManagedProcessHelper( const ManagedProcessHelper& );
		ManagedProcessHelper& operator=( const ManagedProcessHelper& );

        /**
          * reload()
          *
          * This method reloads the data from the database. It is called when a get
          * method is called and the data state is not valid.
          */
        void reload();

        bool m_isValidData;

        unsigned long m_snsconId;
        unsigned long m_managedProcessPkey;
        EOperationMode m_defaultOperatingMode;
    };

} // namespace TA_Base_Core

#endif // MANAGEDPROCESSHELPER_H
