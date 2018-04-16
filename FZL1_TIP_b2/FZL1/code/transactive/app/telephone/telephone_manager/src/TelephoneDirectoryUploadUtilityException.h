#if !defined(AFX_TELEPHONEDIRECTORYUPLOADUTILITYEXCEPTION_H__9D90141C_0D9A_4768_94B8_051580E13140__INCLUDED_)
#define AFX_TELEPHONEDIRECTORYUPLOADUTILITYEXCEPTION_H__9D90141C_0D9A_4768_94B8_051580E13140__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/TelephoneDirectoryUploadUtilityException.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/exceptions/src/TransactiveException.h"

class TelephoneDirectoryUploadUtilityException : public TA_Base_Core::TransactiveException
{
public:
	TelephoneDirectoryUploadUtilityException();
    TelephoneDirectoryUploadUtilityException(const std::string& msg);
	virtual ~TelephoneDirectoryUploadUtilityException();
};

#endif // !defined(AFX_TELEPHONEDIRECTORYUPLOADUTILITYEXCEPTION_H__9D90141C_0D9A_4768_94B8_051580E13140__INCLUDED_)
