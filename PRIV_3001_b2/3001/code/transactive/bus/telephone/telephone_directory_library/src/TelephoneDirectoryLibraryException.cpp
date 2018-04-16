/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_directory_library/src/TelephoneDirectoryLibraryException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
