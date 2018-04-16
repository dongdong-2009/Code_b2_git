// RangeNumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "RangeNumberEdit.h"
#include <string>


// CRangeNumberEdit

IMPLEMENT_DYNAMIC(CRangeNumberEdit, CEdit)

CRangeNumberEdit::CRangeNumberEdit(short min,
                                   short max,
                                   short defaultValue)
    : m_minInputNumber(min),
      m_maxInputNumber(max),
      m_defaultValue(defaultValue)
{

}

CRangeNumberEdit::CRangeNumberEdit()
{

}

CRangeNumberEdit::~CRangeNumberEdit()
{
}


BEGIN_MESSAGE_MAP(CRangeNumberEdit, CEdit)
    ON_WM_CHAR()
END_MESSAGE_MAP()

void CRangeNumberEdit::setBunndySpinCtrl(CSpinButtonCtrl* spinCtrl)
{
    m_spinCtrl = spinCtrl;
}

void CRangeNumberEdit::focusKilled()
{
    short inputValue;
    if(!isInputValid()
            || (inputValue = getValue()) < m_minInputNumber
            || inputValue > m_maxInputNumber)
    {
        inputValue = m_defaultValue;
        m_spinCtrl->SetPos(inputValue);
        CString newStrValue;
        newStrValue.Format("%d", inputValue);
        SetWindowText(newStrValue);
    }
}

short CRangeNumberEdit::getValue()
{
    CString wndText;
    GetWindowText(wndText);
    return atoi(wndText);
}

bool CRangeNumberEdit::isInputValid()
{
    CString wndText;
    GetWindowText(wndText);
    return std::string(wndText).find_first_not_of("0123456789")
           == std::string::npos;
}

void CRangeNumberEdit::setDefaultValue(short defaultValue)
{
	m_defaultValue = defaultValue;
}

void CRangeNumberEdit::setValueRange(short min, short max)
{
	m_minInputNumber = min;
	m_maxInputNumber = max;
	m_spinCtrl->SetRange(m_minInputNumber, m_maxInputNumber);
}

void CRangeNumberEdit::setValue( short value )
{
	CString s;
	s.Format("%d", value);
	SetWindowText(s);
}

// CRangeNumberEdit message handlers


