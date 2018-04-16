/*******************************************************************************
Copyright (c) 2011-2012, Zhejiang Uniview Technologies Co., Ltd. All rights reserved.
--------------------------------------------------------------------------------
                              imos_public.h

  Project Code: IMOS PLAT
   Module Name: BP
  Date Created: 2008-08-28
        Author: wuxianmin/04253
   Description: ����ƽ̨����ͷ�ļ�
   
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

/* ����ѡ�� */
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

/* ������׼����Լ�� */
#ifdef WIN32
    #define STDCALL         __stdcall
#else
    #ifdef i386
        #define STDCALL         __attribute__((stdcall))
	#else
	    #define STDCALL
    #endif
#endif

/* ���������Ͷ��� */

/** @brief 8λ�з������� */
#ifndef CHAR
typedef char CHAR;
#endif

/** @brief 8λ�޷������� */
#ifndef UCHAR
typedef unsigned char UCHAR;
#endif

/** @brief 16λ�з������� */
#ifndef SHORT
typedef short SHORT;
#endif

/** @brief 16λ�޷������� */
#ifndef USHORT
typedef unsigned short USHORT;
#endif

/** @brief 32λ�з������� */
#ifndef LONG
typedef long LONG;
#endif

/** @brief 32λ�޷������� */
#ifndef ULONG
typedef unsigned long ULONG;
#endif

/** @brief 64λ�з������� */
#ifndef DLONG
#if defined (_MSC_VER)
typedef __int64 DLONG;
#else
typedef long long DLONG;
#endif
#endif

/** @brief 64λ�޷������� */
#ifndef DULONG
#if defined (_MSC_VER)
typedef unsigned __int64 DULONG;
#else
typedef unsigned long long DULONG;
#endif
#endif

/** @brief ˫������������ */
#ifndef DOUBLE
typedef double DOUBLE;
#endif

/** @brief ������������ */
#ifndef FLOAT
typedef float FLOAT;
#endif

/** @brief ������ */
#ifndef BOOL_T
typedef unsigned long BOOL_T;
#endif

/** @brief ��ʾ������������ */
#ifndef SIZE_T
typedef unsigned long SIZE_T;
#endif

/** @brief ��ʾʱ���������� */
#ifndef TIME_T
typedef long TIME_T;
#endif

/** @brief VOID�� */
#define VOID                            void

/** @brief ��ʾͨ�þ������ */
#ifndef HANDLE
typedef VOID* HANDLE;
#endif

/** @brief IMOS ID �������� */
#ifndef IMOS_ID
typedef DULONG IMOS_ID;
#endif

/** @brief ͨ�þ������ IMOS_HANDLE */
#ifndef IMOS_HANDLE
#if defined (WIN32)
typedef HANDLE IMOS_HANDLE;
#else
typedef int IMOS_HANDLE;
#endif
#endif

/** @brief �����������Ͷ��� */
#define IN
#define OUT
#define INOUT

/** @brief ͨ��״̬�� */
#define OK                              0UL
#define ERR                             1UL

/** @brief IMOS��̹淶����ֵ */
#define BOOL_TRUE                       ((BOOL_T) 1)
#define BOOL_FALSE                      ((BOOL_T) 0)

#ifdef  __cplusplus
}
#endif /* end of __cplusplus */

#endif /* _IMOS_PUBLIC_H_ */
