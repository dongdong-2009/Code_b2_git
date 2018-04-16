//////////////////////////////////////////////////////////////////////
///		@file		CommonDef.h
///		@author		zhilin,ouyang
///		@date		2017-12-25 10:44:41
///
///		@brief	    to define some common use macro or constant static 
///                 variables used in the library.
///                 
//////////////////////////////////////////////////////////////////////
#ifndef  COMMONDEF_H__
#define  COMMONDEF_H__

#include <vector>
#include "core/types/src/ta_types.h"

// easy usage of the namespace identifier
#define  NS_BEGIN(name) namespace name {
#define  NS_END(name)   };

NS_BEGIN(TA_Base_App)
NS_BEGIN(TA_MLL_Provider)

//////////////////////////////////////////////////////
enum  Ret_Code
{
	Ret_Success = 0,
	Ret_Failed,
	Ret_Exit
};

// define common type which will be used by this module
typedef std::vector<ta_float64>         RealVec;
typedef std::vector<ta_float64>         RealRow;
typedef std::vector<RealRow>            RealTable;

typedef std::vector<size_t>             uVec;
typedef std::vector<size_t>             uRow;
typedef std::vector<uRow>               uTable;

#define  DEF_EXIT_CHECK(condition)   if((condition))   return Ret_Exit  
#define  DEF_ERROR_CHECK(condition)  if(!(condition))  return  Ret_Failed
#define  DEF_ERROR_EXIT(condition)   if(!(condition))  goto Exit0
#define  DEF_ASSERT_EXIT(condition)  if(!(condition))  goto Exit0

#ifndef  countof
#define  countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

#define DEF_DELETE_ARRAY(pArray)    if (pArray) { delete [](pArray); (pArray) = NULL; }
#define DEF_DELETE(p)   if (p) { delete (p); (p) = NULL; }

#define DEF_DISABLE_COPY_AND_ASSIGNMENT(className)   private:   \
	className(const className&); \
	className& operator=(const className&);


#if defined(_WIN32)
	// mysqlcppconn_EXPORTS is added by cmake and defined for dynamic lib build only
	#ifdef DLL_EXPORTS
		#define PUBLIC_DLL_EXPORTS __declspec(dllexport)
	#else		
		#define PUBLIC_DLL_EXPORTS		
	#endif
#else
	#define PUBLIC_DLL_EXPORTS
#endif

NS_END(TA_MLL_Provider)
NS_END(TA_Base_App)

#endif ///COMMONDEF_H__