#include "stdafx.h"
#include "CallBackComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    CallBackComboBox::CallBackComboBox()
        : m_listener(NULL)
    {
    }

    CallBackComboBox::~CallBackComboBox()
    {
    }



	BOOL CallBackComboBox::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult)
	{

		if( WM_COMMAND == message && 
		    CBN_SELCHANGE == HIWORD(wParam) &&
		    NULL != m_listener )
		{
            m_listener->selectedChanged(this);
			return TRUE;
		}
		return CComboBox::OnChildNotify(message, wParam, lParam, pLResult);
	}

    void CallBackComboBox::setCallback(IComboBoxListener* listener)
    {
        m_listener = listener;
    }

}
