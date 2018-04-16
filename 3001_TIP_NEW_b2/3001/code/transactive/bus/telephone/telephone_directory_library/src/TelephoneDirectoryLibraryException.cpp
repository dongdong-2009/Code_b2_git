/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Implements the Exception required by the TelephoneDirectoryLibrary
  */

#include "bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::TransactiveException;

TelephoneDirectoryLibraryException::TelephoneDirectoryLibraryException()
    : TransactiveException("TelephoneDirectoryLibraryException")
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


TelephoneDirectoryLibraryException::TelephoneDirectoryLibraryException(const std::string& msg)
    : TransactiveException(msg)
{
    FUNCTION_ENTRY("TelephoneDirectoryLibraryException");
    FUNCTION_EXIT;
}


TelephoneDirectoryLibraryException::~TelephoneDirectoryLibraryException()
{
	FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT
}
