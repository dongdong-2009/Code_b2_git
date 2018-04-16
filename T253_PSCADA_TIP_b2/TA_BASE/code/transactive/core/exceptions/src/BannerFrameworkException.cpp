/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/BannerFrameworkException.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * This file outlines a simple exception class for use with
 * the BannerFramework. It is
 * derived from TransactiveException.
 * This exception class is used when errors are encountered 
 * within the framework that are necessary to pass up to the banner applicaiton.
 */


#if defined(_MSC_VER)
//#pragma warning(disable:4786)
#endif // defined(_MSC_VER)

#include "core\exceptions\src\BannerFrameworkException.h"
#include "core\utilities\src\DebugUtil.h"

namespace TA_Base_Core
{

    BannerFrameworkException::BannerFrameworkException(const char *msg) throw()
        : m_failType(GENERAL_ERROR), TransactiveException(msg)
	{
	}

    BannerFrameworkException::BannerFrameworkException(const EFailType failType, const char *msg) throw()
        : m_failType(failType), TransactiveException(msg)
    {
    }

	BannerFrameworkException::~BannerFrameworkException() throw()
	{

	}

} // Closes TA_Base_Core
