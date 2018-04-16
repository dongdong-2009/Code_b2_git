// src\UnEditableEdit.cpp : implementation file
//

#include "stdafx.h"
#include "app/bas/BasParameterManager/src/UnEditableEdit.h"


namespace TA_IRS_App
{
	// CUnEditableEdit
	IMPLEMENT_DYNAMIC(CUnEditableEdit, CEdit)
	CUnEditableEdit::CUnEditableEdit()
	{

	}

	CUnEditableEdit::~CUnEditableEdit()
	{
	}


	BEGIN_MESSAGE_MAP(CUnEditableEdit, CEdit)
		ON_WM_CTLCOLOR_REFLECT()
	END_MESSAGE_MAP()


	// CUnEditableEdit message handlers
	HBRUSH CUnEditableEdit::CtlColor(CDC* pDC, UINT nCtlColor)
	{
		HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));

		// TODO:  Change any attributes of the DC here
		EnableWindow(FALSE);
		pDC->SetTextColor(RGB(0, 0, 0));
		pDC->SetBkColor(RGB(255, 255, 255));
		// TODO:  Return a different brush if the default is not desired
		return hbr;
	}


}

