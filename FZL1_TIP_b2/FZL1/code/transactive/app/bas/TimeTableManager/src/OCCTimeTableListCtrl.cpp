// src\OCCTimeTableListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/OCCTimeTableListCtrl.h"

namespace TA_IRS_App
{
	// OCCTimeTableListCtrl
	IMPLEMENT_DYNAMIC(OCCTimeTableListCtrl, TimeTableListCtrl)
	OCCTimeTableListCtrl::OCCTimeTableListCtrl() : TimeTableListCtrl()
	{

	}

	OCCTimeTableListCtrl::~OCCTimeTableListCtrl()
	{
	}

	BEGIN_MESSAGE_MAP(OCCTimeTableListCtrl, TimeTableListCtrl)
	END_MESSAGE_MAP()
}
