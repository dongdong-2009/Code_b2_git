/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/system_controller/src/IManagedProcessData.h $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This interface contains methods to access an entry
  * in the sn_managed_process table.
  *
  */

#ifndef IMANAGEDPROCESSDATA_H
#define IMANAGEDPROCESSDATA_H

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
    class IManagedProcessData
    {

    public:

        /** 
          * getSystemControllerKey
          *
          * Returns the key of the System Controller for this entry
          * in the sn_managed_process table (ie the sncon_id column).
          *
          * @return The System Controller's key.
          *
          */
        virtual unsigned long getSystemControllerKey() = 0;

        /** 
          * getManagedProcessKey
          *
          * Returns the entity key of the managed process for this entry
          * in the sn_managed_process table.
          *
          * @return The managed process's entity key.
          *
          */
        virtual unsigned long getManagedProcessKey() = 0;

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
        virtual EOperationMode getDefaultOperatingMode() = 0;

    };
    
} // namespace TA_Base_Core

#endif // IMANAGEDPROCESSDATA_H
