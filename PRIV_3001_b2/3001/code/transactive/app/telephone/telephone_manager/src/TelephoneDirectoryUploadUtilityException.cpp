/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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


