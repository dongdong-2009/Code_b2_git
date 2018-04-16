#ifndef EXPORTDEF_INCLUDE
#define EXPORTDEF_INCLUDE

#if defined(_MSC_VER)
	#if defined(BUILD_DLL)
		#define Plan_Export __declspec(dllexport)
	#elif defined(BUILD_LIB)
		#define Plan_Export
	#else
		#define Plan_Export __declspec(dllimport)
	#endif
#elif defined(_GCC)
	#if defined(BUILD_DLL)
		#define Plan_Export __attribute__((visibility("default")))
	#elif defined(BUILD_LIB)
		#define Plan_Export
	#else
		#define Plan_Export
	#endif
#else
	#define Plan_Export
	#pragma message ("Warning: Unknown dynamic link import/export semantics.")
#endif

#endif