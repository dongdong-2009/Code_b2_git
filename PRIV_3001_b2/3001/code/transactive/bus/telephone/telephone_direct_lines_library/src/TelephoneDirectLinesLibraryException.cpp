/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
