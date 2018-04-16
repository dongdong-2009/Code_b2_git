/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"
#include "app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::TransactiveException;

TelephoneDirectoryUploadUtilityException::TelephoneDirectoryUploadUtilityException()
    : TransactiveException("TelephoneDirectoryUploadUtilityException")
{
    FUNCTION_ENTRY( "TelephoneDirectoryUploadUtilityException" );
    FUNCTION_EXIT;
}


TelephoneDirectoryUploadUtilityException::TelephoneDirectoryUploadUtilityException(const std::string& msg)
    : TransactiveException(msg)
{
    FUNCTION_ENTRY( "TelephoneDirectoryUploadUtilityException" );
    FUNCTION_EXIT;
}


TelephoneDirectoryUploadUtilityException::~TelephoneDirectoryUploadUtilityException()
{
    FUNCTION_ENTRY( "~TelephoneDirectoryUploadUtilityException" );
    FUNCTION_EXIT;
}


