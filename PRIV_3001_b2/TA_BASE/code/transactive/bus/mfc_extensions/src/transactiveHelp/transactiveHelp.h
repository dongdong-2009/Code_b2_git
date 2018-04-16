/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/mfc_extensions/src/transactiveHelp/transactiveHelp.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#ifndef IMPORT_EXTENSIONS
#define TRANSACTIVEHELP_API __declspec(dllexport)
#else
#define TRANSACTIVEHELP_API __declspec(dllimport)
#endif

#include "bus/mfc_extensions/src/StdAfx.h"
#include "cots/HTMLHelp/inc/htmlhelp.h"

namespace TA_Base_Bus 
{
	extern "C"
	{
		TRANSACTIVEHELP_API int positionTransactiveHelp(
			const char* szHelp, 
			const char* szWindow,
			const char* szCaption,
			const char* szHome,
			long left, 
			long top, 
			long right, 
			long bottom);
	}
}

