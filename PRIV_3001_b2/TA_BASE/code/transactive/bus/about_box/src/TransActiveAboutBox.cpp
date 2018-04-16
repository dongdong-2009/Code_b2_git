/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/about_box/src/TransActiveAboutBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// TransActiveAboutBox.cpp: implementation of the CTransActiveAboutBox class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TransActiveAboutBox.h"
#include "TransActiveAboutDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void CTransActiveAboutBox::DisplayAboutBox( const char buildVersion[],
										    const char buildDate[],
											const char componentName[],
											const char componentVersion[],
											const int headerArraySize,
											char* headerArray[])
{
	//TD18095, jianghp, performance issue, so need set the parent window handle.
	CTransActiveAboutDlg TAAboutDlg( buildVersion, buildDate,
									 componentName, componentVersion,
									 headerArraySize, headerArray, AfxGetMainWnd());

	TAAboutDlg.DoModal();
}

