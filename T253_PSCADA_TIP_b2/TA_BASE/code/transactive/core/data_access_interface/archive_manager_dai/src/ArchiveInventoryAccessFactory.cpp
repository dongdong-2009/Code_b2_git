/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * ArchiveTablesAccessFactory is a singleton that is used to generate and retrieve 
  * IArchiveTables and IArchiveInventory objects.
  */

#pragma warning ( disable : 4786 )

#include <string>

#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryAccessFactory.h"
#include "core/data_access_interface/archive_manager_dai/src/ArchiveInventoryData.h"

namespace TA_Base_Core
{
    //
    // getArchiveInventory
    //
    IArchiveInventory* ArchiveInventoryAccessFactory::getArchiveInventory()
    {
        // Create the ArchiveInventory object to represent this object.
        // Return the inventory pointer. The class that recieves this pointer is responsible
        // for deleting it.

        return new ArchiveInventoryData();
    }

} //end namespace TA_Base_Core


