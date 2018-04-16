// ActivePlansDisplayCommonDefs.h : header file
//

#if !defined(AFX_ACTIVEPLANSDISPLAYCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
#define AFX_ACTIVEPLANSDISPLAYCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/response_plans/active_plans_display/src/ActivePlansDisplayCommonDefs.h $
  * @author:  Rob Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Common definitions for ActivePlansDisplayLibrary
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_Bus
{
    // Custom messages
    #define WM_ACTIVE_PLAN_DETAIL_UPDATE           (WM_USER + 901)
    #define WM_ACTIVE_PLAN_LIST_POST_INITIALISE    (WM_USER + 902)

	// Types
	typedef unsigned long TreeNodeId;
	typedef unsigned long PlanInstanceNumber;
    typedef unsigned long LocationKey;

} // namespace TA_Base_Bus


#endif // !defined(AFX_ACTIVEPLANSDISPLAYCOMMONDEFS_H__9A958DFD_A9E3_46FC_8EF8_61E132D3C1D0__INCLUDED_)
