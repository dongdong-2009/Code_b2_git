#if !defined(AFX_TELEPHONEDIRECTLINESLIBRARYEXCEPTION_H__80CB51B0_64CD_4223_B1E9_17C691BE593E__INCLUDED_)
#define AFX_TELEPHONEDIRECTLINESLIBRARYEXCEPTION_H__80CB51B0_64CD_4223_B1E9_17C691BE593E__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/telephone/telephone_direct_lines_library/src/TelephoneDirectLinesLibraryException.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
