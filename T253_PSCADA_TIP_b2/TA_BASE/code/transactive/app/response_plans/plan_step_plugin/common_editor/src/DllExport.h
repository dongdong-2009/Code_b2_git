#ifndef COMM_EXPORTDEF_INCLUDE
#define COMM_EXPORTDEF_INCLUDE

// [ToDo] Rename _AFXEXT to a unique name
#ifndef PLAN_COMM_DLL
#ifdef _AFXEXT
#define PLAN_COMM_DLL __declspec(dllexport)
#else
#define PLAN_COMM_DLL __declspec(dllimport)
#endif
#endif

#endif