/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/system_control/system_status_agent/src/reporting.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
//reporting.h
//A bit of debugging help
#ifndef CPD_DEBUGGING_CRAP
	#define CPD_DEBUGGING_CRAP
	#include <iostream>


	/////////////////////////////////////
	//#define CPD_REPORTING_ON

	#ifdef CPD_REPORTING_ON
		#define CPD_REPORT(str) {std::cout << str << std::endl;}
	#else
		#define CPD_REPORT(str) {}
	#endif

	/////////////////////////////////////
	//#define CPD_REPORTING_ON_2

	#ifdef CPD_REPORTING_ON_2
		#define CPD_REPORT_2(str) {std::cout << str << std::endl;}
	#else
		#define CPD_REPORT_2(str) {}
	#endif

	/////////////////////////////////////
	//#define CPD_REPORTING_ON_CONTROL_STATION_STATUS

	#ifdef CPD_REPORTING_ON_CONTROL_STATION_STATUS
		#define CPD_REPORT_CS(str) {std::cout << str << std::endl;}
	#else
		#define CPD_REPORT_CS(str) {}
	#endif

	/////////////////////////////////////
	//#define CPD_REPORTING_ON_SYS_CON_STATUS

	#ifdef CPD_REPORTING_ON_SYS_CON_STATUS
		#define CPD_REPORT_SC(str) {std::cout << str << std::endl;}
	#else
		#define CPD_REPORT_SC(str) {}
	#endif

	/////////////////////////////////////
	//#define CPD_REPORTING_ON_DB_SERVER_STATUS

	#ifdef CPD_REPORTING_ON_DB_SERVER_STATUS
		#define CPD_REPORT_DB(str) {std::cout << str << std::endl;}
	#else
		#define CPD_REPORT_DB(str) {}
	#endif

	/////////////////////////////////////
#endif