/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  */

#include "bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::TransactiveException;

TelephoneDirectLinesLibraryException::TelephoneDirectLinesLibraryException()
    : TransactiveException("TelephoneDirectLinesLibraryException")
{
    FUNCTION_ENTRY("Constructor");
    FUNCTION_EXIT;
}


TelephoneDirectLinesLibraryException::TelephoneDirectLinesLibraryException(const std::string& msg)
    : TransactiveException(msg)
{
    FUNCTION_ENTRY("TelephoneDirectLinesLibraryException");
    FUNCTION_EXIT;
}


TelephoneDirectLinesLibraryException::~TelephoneDirectLinesLibraryException()
{
    FUNCTION_ENTRY("Destructor");
    FUNCTION_EXIT;
}
