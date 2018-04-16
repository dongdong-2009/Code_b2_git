/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryEntryData.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
