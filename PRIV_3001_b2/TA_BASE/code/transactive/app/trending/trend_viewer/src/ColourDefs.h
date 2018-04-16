#if !defined(HISTORY_VIEWER_COLOUR_DEFS_H)
#define HISTORY_VIEWER_COLOUR_DEFS_H

/**
  * ColourDefs.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/ColourDefs.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Colour Definitions for Trend Viewer.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


static const COLORREF DEFAULT_PEN_COLOUR = RGB(0, 0, 0);
static const COLORREF CURSOR_COLOUR = RGB(255,0,0);
static const COLORREF TITLE_COLOUR = RGB(0,0,0);
static const COLORREF BAD_STATUS_COLOUR = RGB(255,0,0);

static const COLORREF DEFAULT_CELL_BACKGROUND_COLOUR = RGB(255, 255, 255);

//
// Default Colours
//
static const int NUM_COLOURS = 8;
static const COLORREF DEFAULT_COLOURS[] =
{
    RGB(0, 0, 255),      // Blue
    RGB(255, 0, 255),    // Fuchsia
    RGB(0, 255, 0),      // Lime
    RGB(255, 165, 0),    // Orange
    RGB(128, 0, 0),      // Maroon
    RGB(148, 0, 211),    // Dk Violet
    RGB(255, 255, 0),    // Yellow
    RGB(0, 255, 255),    // Cyan
};

#endif
