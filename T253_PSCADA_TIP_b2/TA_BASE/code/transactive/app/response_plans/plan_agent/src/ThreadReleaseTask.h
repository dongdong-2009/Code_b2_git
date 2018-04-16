/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ThreadReleaseTask.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#if !defined(AFX_ThreadReleaseTask_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_)
#define AFX_ThreadReleaseTask_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ace/Method_Request.h"

namespace TA_Base_App
{
static const int ReleaseThread = -1;

class ThreadReleaseTask  : public ACE_Method_Request
{
public:
	ThreadReleaseTask();
	virtual int call();

private:
	ThreadReleaseTask& operator=( const ThreadReleaseTask& );
	ThreadReleaseTask( const ThreadReleaseTask& );
};

}
#endif // !defined(AFX_ThreadReleaseTask_H__079D7AFF_3EE2_40AF_B7D7_D0E0C41090A1__INCLUDED_)
