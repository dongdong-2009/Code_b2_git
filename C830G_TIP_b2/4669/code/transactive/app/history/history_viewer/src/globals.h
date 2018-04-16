#if !defined(HISTORY_VIEWER_GLOBALS_H)
#define HISTORY_VIEWER_GLOBALS_H

/**
  * globals.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/globals.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Global variables for Trend Viewer.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/scada/common_library/src/CommonDefs.h"

//
// Entity Parameters
//
static const char* RPARAM_GRAPH_REFRESH_INTERVAL = "GraphRefreshInterval";
static const char* RPARAM_MAX_TRENDS_DISPLAYED = "MaxTrendsDisplayed";
static const char* RPARAM_STATION_NAME = "StationName";
static const char* RPARAM_ENTITY_KEY = "EntityKey";

//
// Entity Parameter Values
//
static const int CFG_GRAPH_REFRESH_RATE_DEFAULT = 60;
static const int CFG_GRAPH_REFRESH_RATE_MIN = 10;
static const int CFG_MAX_TRENDS_DISPLAYED_DEFAULT = 8;
static const int CFG_MAX_TRENDS_DISPLAYED_MAX = 11;    // This is how many colours are available

//
// Data Point Status Values
//
static const int COMMS_ERROR_UNKNOWN = -1;
static unsigned long STATUS_UNKNOWN = 0;
static const int NUM_GOOD_QUALITY_WORDS = 2;
static unsigned long DP_GOOD_QUALITY_WORDS[NUM_GOOD_QUALITY_WORDS] = 
{
    QUALITY_GOOD_NO_SPECIFIC_REASON,
    QUALITY_GOOD_LOCAL_OVERRIDE
};

//
// Limits
//
static const int MAX_DATAPOINT_NAME_LENGTH = 80;
static const int TREND_VIEWER_RANGE_YEAR = 10;

static const char* DP_BOOLEAN_TYPE = "Boolean";

#endif // HISTORY_VIEWER_GLOBALS_H



