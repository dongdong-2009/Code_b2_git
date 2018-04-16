/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_public.h

  Project Code: IMOS PLAT
   Module Name: BP
  Date Created: 2008-08-28
        Author: wuxianmin/04253
   Description: 基础平台公共头文件
   
--------------------------------------------------------------------------------
  Modification History
  DATE        NAME             DESCRIPTION
--------------------------------------------------------------------------------
  YYYY-MM-DD  

*******************************************************************************/
#ifndef _IMOS_PUBLIC_H_
#define _IMOS_PUBLIC_H_

#ifdef  __cplusplus
extern "C"{
#endif

/* 编译选项 */
#if defined(WIN32)
    #if defined(IMOS_EXPORTS_DLL)
        #define IMOS_EXPORT  __declspec( dllexport )
    #else
        #if defined(IMOS_AS_STATIC_LIB)
            #define IMOS_EXPORT
        #else
            #define IMOS_EXPORT  __declspec( dllimport )    
        #endif
    #endif
#else
    #define IMOS_EXPORT
#endif

/* 函数标准调用约定 */
#ifdef WIN32
    #define STDCALL         __stdcall
#else
    #ifdef i386
        #define STDCALL         __attribute__((stdcall))
	#else
	    #define STDCALL
    #endif
#endif

/* 简单数据类型定义 */

/** @brief 8位有符号整型 */
#ifndef CHAR
typedef char CHAR;
#endif

/** @brief 8位无符号整型 */
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

/** @brief 16位有符号整型 */
#ifndef SHORT
typedef short SHORT;
#endif

/** @brief 16位无符号整型 */
#ifndef USHORT
typedef unsigned short USHORT;
#endif

/** @brief 32位有符号整型 */
#ifndef LONG
typedef long LONG;
#endif

/** @brief 32位无符号整型 */
#ifndef ULONG
typedef unsigned long ULONG;
#endif

/** @brief 64位有符号整型 */
#ifndef DLONG
#if defined (_MSC_VER)
typedef __int64 DLONG;
#else
typedef long long DLONG;
#endif
#endif

/** @brief 64位无符号整型 */
#ifndef DULONG
#if defined (_MSC_VER)
typedef unsigned __int64 DULONG;
#else
typedef unsigned long long DULONG;
#endif
#endif

/** @brief 双精度数据类型 */
#ifndef DOUBLE
typedef double DOUBLE;
#endif

/** @brief 浮点数据类型 */
#ifndef FLOAT
typedef float FLOAT;
#endif

/** @brief 布尔型 */
#ifndef BOOL_T
typedef unsigned long BOOL_T;
#endif

/** @brief 表示长度数据类型 */
#ifndef SIZE_T
typedef unsigned long SIZE_T;
#endif

/** @brief 表示时间数据类型 */
#ifndef TIME_T
typedef long TIME_T;
#endif

/** @brief VOID型 */
#define VOID                            void

/** @brief 表示通用句柄类型 */
#ifndef HANDLE
typedef VOID* HANDLE;
#endif

/** @brief IMOS ID 数据类型 */
#ifndef IMOS_ID
typedef DULONG IMOS_ID;
#endif

/** @brief 通用句柄类型 IMOS_HANDLE */
#ifndef IMOS_HANDLE
#if defined (WIN32)
typedef HANDLE IMOS_HANDLE;
#else
typedef int IMOS_HANDLE;
#endif
#endif

/** @brief 函数参数类型定义 */
#define IN
#define OUT
#define INOUT

/** @brief 通用状态码 */
#define OK                              0UL
#define ERR                             1UL

/** @brief IMOS编程规范布尔值 */
#define BOOL_TRUE                       ((BOOL_T) 1)
#define BOOL_FALSE                      ((BOOL_T) 0)

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_PUBLIC_H_ */
