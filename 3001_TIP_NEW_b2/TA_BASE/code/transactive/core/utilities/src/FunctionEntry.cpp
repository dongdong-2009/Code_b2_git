/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/utilities/src/FunctionEntry.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/12/03 18:06:55 $
  * Last modified by:  $Author: limin.zhu $
  *
  * FunctionEntry is a class used to log when functions are
  * entered and exited. Use the macro FUNCTION_ENTRY(functname)
  * at the top of a function and FUNCTION_EXIT at the bottom.
  * FunctionEntry's destructor handles exit logging, so even
  * if a function is exited due to, say, an exception, the
  * exit is still logged (but without a line number).
  *
  */

#include "FunctionEntry.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	FunctionEntry::FunctionEntry(const char* functName, char* file, int line)
		: m_line(-1), m_file(file), m_functName(functName)
	{
        static const DebugUtil::EDebugLevel& debugLevel = DebugUtil::getInstance().getLevel();

        if ( DebugUtil::DebugTrace <= debugLevel )
        {
            static DebugUtil& debugUtil = DebugUtil::getInstance();

            debugUtil.log(file, line, DebugUtil::FunctionEntry, m_functName);
        }
	}


    FunctionEntry::~FunctionEntry()
    {
        static const DebugUtil::EDebugLevel& debugLevel = DebugUtil::getInstance().getLevel();

        if ( DebugUtil::DebugTrace <= debugLevel )
        {
            static DebugUtil& debugUtil = DebugUtil::getInstance();

            debugUtil.log(m_file, m_line, DebugUtil::FunctionExit, m_functName);
        }
    }

} // namespace TA_Base_Core
