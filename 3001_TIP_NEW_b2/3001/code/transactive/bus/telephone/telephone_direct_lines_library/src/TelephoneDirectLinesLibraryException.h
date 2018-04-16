#if !defined(AFX_TELEPHONEDIRECTLINESLIBRARYEXCEPTION_H__80CB51B0_64CD_4223_B1E9_17C691BE593E__INCLUDED_)
#define AFX_TELEPHONEDIRECTLINESLIBRARYEXCEPTION_H__80CB51B0_64CD_4223_B1E9_17C691BE593E__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.h $
  * @author:  Raymond Pau
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "core/exceptions/src/TransactiveException.h"

class TelephoneDirectLinesLibraryException : public TA_Base_Core::TransactiveException  
{
public:
	TelephoneDirectLinesLibraryException();
    TelephoneDirectLinesLibraryException(const std::string& msg);
	virtual ~TelephoneDirectLinesLibraryException();
};

#endif // !defined(AFX_TELEPHONEDIRECTLINESLIBRARYEXCEPTION_H__80CB51B0_64CD_4223_B1E9_17C691BE593E__INCLUDED_)
