// src\TimeTableManagerBaseDialog.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/TimeTableManager/src/TimeTableManager.h"
#include "app/bas/TimeTableManager/src/TimeTableManagerBaseDialog.h"


namespace TA_IRS_App
{

	///////////////////////////////////////////////////////////////////////////////////////
	// TimeTableManagerBaseDialog dialog
	IMPLEMENT_DYNAMIC(TimeTableManagerBaseDialog, CDialog)
	TimeTableManagerBaseDialog::TimeTableManagerBaseDialog(UINT nIDTemplate, CWnd* pParent /*=NULL*/)
		: CDialog(nIDTemplate, pParent)
	{
	}

	TimeTableManagerBaseDialog::~TimeTableManagerBaseDialog()
	{
	}

	void TimeTableManagerBaseDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
	}


	BEGIN_MESSAGE_MAP(TimeTableManagerBaseDialog, CDialog)
	END_MESSAGE_MAP()

	BOOL TimeTableManagerBaseDialog::OnInitDialog()
	{
		return CDialog::OnInitDialog();
	}

	void TimeTableManagerBaseDialog::OnCancel()
	{
	}
	// TimeTableManagerBaseDialog message handlers
	// the interface
	TA_Base_Bus::ResizingProperties TimeTableManagerBaseDialog::getResizingProperties()
	{
		// fix build warning
		TA_Base_Bus::ResizingProperties rp;
		return rp;
	}

	DlgInitializeSize TimeTableManagerBaseDialog::getDlgInitSize()
	{
		// fix build warning
		DlgInitializeSize dlg(0,0);
		return dlg;
	}

	void TimeTableManagerBaseDialog::init()
	{
	}

	CString TimeTableManagerBaseDialog::getApplicationName()
	{
		CString applicationName("");
		return applicationName;
	}
}
