/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/trains/train_agent/src/TARethrow.h $
  * @author:  Glen Kidd
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Temporary replacement for TA_THROW which allows exception heirarchy to be propagated.
  *
  */


#if !defined(TA_RETHROW_H)
#define TA_RETHROW_H

#include "core/exceptions/src/TransactiveException.h"

#if !defined(TA_RETHROW)

/** 
  * TA_RETHROW
  *
  * This macro throws exceptions while providing logging. It takes the
  * place of the throw keyword, and is used identically.
  * eg: TA_THROW(CCTVException("A CCTV error occurred"));
  *
  * @note This version of the macro does a re-throw, thus preserving
  * any hierarchy in a catch by reference statement.
  *
  * @param exc  The exception (with parameters) to log
  */
 
#define TA_RETHROW(exc)															\
{																				\
	LOG(SourceInfo,																\
		TA_Base_Core::DebugUtil::ExceptionConstruction,				\
		std::string(#exc).substr(0, (std::string(#exc).find("("))).c_str(),		\
		exc.what());															\
	throw;																	\
}

#endif

#endif // !defined(TA_RETHROW_H)
