/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implementation of the Class TelephoneDirectoryEntryData
  */

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.h"
#include "core/utilities/src/DebugUtil.h"

TelephoneDirectoryEntryData::TelephoneDirectoryEntryData()
    : ID(0), number(""), fullName(""), location(""), directoryDataName("")
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
	
}


TelephoneDirectoryEntryData::TelephoneDirectoryEntryData(const TelephoneDirectoryEntryData & directoryEntry)
{

    FUNCTION_ENTRY("TelephoneDirectoryEntryData");
    
    this->ID = directoryEntry.ID;
	this->number = directoryEntry.number;
    this->fullName = directoryEntry.fullName;
    this->location = directoryEntry.location;
    this->directoryDataName = directoryEntry.directoryDataName;

	FUNCTION_EXIT;
}

TelephoneDirectoryEntryData& TelephoneDirectoryEntryData::operator = (const TelephoneDirectoryEntryData &directoryEntry)
{

    FUNCTION_ENTRY("operator = ");
   
    this->ID = directoryEntry.ID;
    this->number = directoryEntry.number;
    this->fullName = directoryEntry.fullName;
    this->location = directoryEntry.location;
    this->directoryDataName = directoryEntry.directoryDataName;

	FUNCTION_EXIT;
    return *this;

}
