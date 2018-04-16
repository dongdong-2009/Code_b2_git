/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.cpp $
  * @author:  Catherine Loh
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the Class TelephoneDirectoryData
  */

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryData.h"
#include "core/utilities/src/DebugUtil.h"

TelephoneDirectoryData::TelephoneDirectoryData()
    : ID(0), name("")
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
    
}


TelephoneDirectoryData::TelephoneDirectoryData(const TelephoneDirectoryData & directory)
{
    
    FUNCTION_ENTRY("TelephoneDirectoryData");
	
	this->ID = directory.ID;
	this->name = directory.name;

    FUNCTION_EXIT;
	
}

TelephoneDirectoryData& TelephoneDirectoryData::operator = (const TelephoneDirectoryData &directory)
{

    FUNCTION_ENTRY("operator = ");
    
    this->ID = directory.ID;
    this->name = directory.name;  

	FUNCTION_EXIT;
    return *this;
}
