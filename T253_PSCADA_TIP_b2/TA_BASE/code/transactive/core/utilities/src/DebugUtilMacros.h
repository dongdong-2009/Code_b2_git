#ifndef DEBUGUTILMACROS_H_INCLUDED
#define DEBUGUTILMACROS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/utilities/src/DebugUtilMacros.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2018/02/23 15:35:09 $
  * Last modified by:  $Author: limin.zhu $
  *
  * <description>
  *
  */


/** 
  * LOG. 
  * 
  * This macro is used to log a debug message.
  *
  * *** NOTE: *** 
  *     Any strings passed in MUST be a const char* NOT a std::string.
  *
  * Usage: 
  *      LOG(SourceInfo, DebugUtil::GenericLog, <Debug level>, <format string>, ...)
  *      LOG(SourceInfo, <Preformatted log type>, ...)
  */
#define LOG          TA_Base_Core::DebugUtil::getInstance().log

/**
  * LOG_GENERIC
  *
  * This macro is used to make generic log messages (ie a message not fitting
  * one of the preformatted message types).
  *
  * Usage:
  *      LOG_GENERIC(SourceInfo, <Debug level>, <format string>, ...)
  */
#define LOG_GENERIC  TA_Base_Core::DebugUtil::getInstance().logGeneric


// Macros that are equivalent to LOG_GENERIC, but avoid evaluating arguments if
// the current debug level is less than the message's log level.
#define LOG0(sourceInfo, debugLevel, formatStr) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr); \
    } 

#define LOG1(sourceInfo, debugLevel, formatStr, arg1) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1); \
    } 

#define LOG2(sourceInfo, debugLevel, formatStr, arg1, arg2) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2); \
    } 

#define LOG3(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3); \
    } 

#define LOG4(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4); \
    } 

#define LOG5(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5); \
    } 

#define LOG6(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5, arg6) \
    if (debugLevel <= TA_Base_Core::DebugUtil::getInstance().getLevel()) \
    { \
        TA_Base_Core::DebugUtil::getInstance().logGeneric(sourceInfo, debugLevel, formatStr, arg1, arg2, arg3, arg4, arg5, arg6); \
    } 

#define LOGBEFORECOTS(cotsname)  // \
//	TA_Base_Core::DebugUtil::getInstance().logGeneric(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "COTS %s is going to be used.", cotsname);

#define LOGAFTERCOTS(cotsname)  // \
//	TA_Base_Core::DebugUtil::getInstance().logGeneric(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "COTS %s is used.", cotsname);
/** 
  * LOGMORE. 
  * 
  * This macro is used to log an indented debug message without the date, time,
  * debug level, thread and location information.
  * This must be called after a LOG as its level is related to the last LOG.
  *
  * *** NOTE: *** 
  *     Any strings passed in MUST be a const char* NOT a std::string.
  *
  * Usage: 
  *      LOGMORE(SourceInfo, DebugUtil::GenericLog, <Debug level>, <format string>, ...)
  */
#define LOGMORE     TA_Base_Core::DebugUtil::getInstance().logMore

#define LOGLARGESTRING TA_Base_Core::DebugUtil::getInstance().logLargeString

/**
  * Source Info.
  *
  * This macro needs to be the first argument to
  * every LOG call so that the file name and line number
  * can be logged.
  */
#define SourceInfo  __FILE__,__LINE__
#define DEBUG_INFO   SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo
#define DEBUG_ERROR  SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError
#define DEBUG_WARN   SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn
#define DEBUG_DEBUG  SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug

#define LOG_FATAL(...)      TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugFatal, __VA_ARGS__)
#define LOG_ERROR(...)      TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, __VA_ARGS__)
#define LOG_WARNING(...)    TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, __VA_ARGS__)
#define LOG_NORMAL(...)     TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugNorm, __VA_ARGS__)
#define LOG_INFO(...)       TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, __VA_ARGS__)
#define LOG_DEBUG(...)      TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, __VA_ARGS__)
#define LOG_SQL(...)        TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugSQL, __VA_ARGS__)
#define LOG_CORBA(...)      TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugCORBA, __VA_ARGS__)
#define LOG_MSG(...)        TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugMsg, __VA_ARGS__)
#define LOG_MSGPUB(...)     TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugMsgPub, __VA_ARGS__)
#define LOG_PERIODIC(...)   TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugPeriodic, __VA_ARGS__)
#define LOG_DATABASE(...)   TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDatabase, __VA_ARGS__)
#define LOG_EXTERNAL(...)   TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugExternal, __VA_ARGS__)
#define LOG_TRACE(...)      TA_Base_Core::DebugUtil::getInstance().log(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugTrace, __VA_ARGS__)
#define LOG_EXCEPTION(name, description) TA_Base_Core::gLogExceptionCatch(SourceInfo, name, description)
#define LOG_EXCEPTION_Details(name, details) TA_Base_Core::gLogExceptionCatch(SourceInfo, name, details)

#define LOG_EXCEPTION_CATCH      TA_Base_Core::gLogExceptionCatch
#define LOG_EXCEPTION_DETAILS    TA_Base_Core::gLogExceptionDetails

namespace TA_Base_Core
{
    // Necessary to support LOG_XXX_XXX macros above.
    inline void gLogExceptionDetails(const std::string& file, int line, const std::string& name, const std::string& details)
    {
        TA_Base_Core::DebugUtil::getInstance().log(file, line, DebugUtil::ExceptionDetails, name.c_str(), details.c_str());
    }

    inline void gLogExceptionCatch(const std::string& file, int line, const std::string& name, const std::string& description)
    {
        TA_Base_Core::DebugUtil::getInstance().log(file, line, DebugUtil::ExceptionCatch, name.c_str(), description.c_str());
    }
}   // namespace TA_Base_Core

// DEPRECATED.  Use FUNCTION_ENTRY and FUNCTION_EXIT macros instead.
#define LOG_FUNCTION_ENTRY(sourceInfo, name)    FUNCTION_ENTRY(name)
#define LOG_FUNCTION_EXIT(sourceInfo, name)     FUNCTION_EXIT

#endif // DEBUGUTILMACROS_H_INCLUDED

