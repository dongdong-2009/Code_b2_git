/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/interlocking/SampleVariableDll/src/VariableSelector.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// VariableSelector.cpp : implementation file
//

#include "stdafx.h"
///	\ add additional includes here"
#include "VariableSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // VariableSelector dialog


    VariableSelector::VariableSelector(CWnd* pParent /*=NULL*/)
	    : CDialog(VariableSelector::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(VariableSelector)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void VariableSelector::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(VariableSelector)
	    DDX_Control(pDX, IDC_Selector, m_selector);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(VariableSelector, CDialog)
	    //{{AFX_MSG_MAP(VariableSelector)
	    ON_NOTIFY(NM_DBLCLK, IDC_Selector, OnDblclkSelector)
	ON_BN_CLICKED(IDCLOSE, OnCancel)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VariableSelector message handlers

    BOOL VariableSelector::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    // TODO: Add extra initialization here
        // format and fill the variable list picker
        m_selector.ModifyStyle(LVS_TYPEMASK, LVS_REPORT | LVS_SINGLESEL );
        RECT selectorSize;
        m_selector.GetClientRect( &selectorSize );
        m_selector.InsertColumn(0, "Variable", LVCFMT_LEFT, selectorSize.right / 3);	
        m_selector.InsertColumn(1, "Name", LVCFMT_LEFT, selectorSize.right / 3);	
        m_selector.InsertColumn(2, "Type", LVCFMT_LEFT, selectorSize.right / 3);	

        // add variables to list view
        int i = 0;
        m_selector.InsertItem(i, "Wendy" );
        m_selector.SetItemText(i, 1, "wendy" );
        m_selector.SetItemText(i++, 2, "Character" );
        m_selector.InsertItem(i, "PeterPan" );
        m_selector.SetItemText(i, 1, "peterpan" );
        m_selector.SetItemText(i++, 2, "Character" );
        m_selector.InsertItem(i, "PixyDust" );
        m_selector.SetItemText(i, 1, "pixydust" );
        m_selector.SetItemText(i++, 2, "Prop" );
        m_selector.InsertItem(i, "Wand" );
        m_selector.SetItemText(i, 1, "wand" );
        m_selector.SetItemText(i++, 2, "Prop" );
        m_selector.InsertItem(i, "Flying" );
        m_selector.SetItemText(i, 1, "flying" );
        m_selector.SetItemText(i++, 2, "Action" );
        m_selector.InsertItem(i, "Swimming" );
        m_selector.SetItemText(i, 1, "swimming" );
        m_selector.SetItemText(i++, 2, "Action" );
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void VariableSelector::OnDblclkSelector(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        POSITION pos = m_selector.GetFirstSelectedItemPosition();
        unsigned int i = m_selector.GetNextSelectedItem(pos);

        m_adder->addVariable( std::string( m_selector.GetItemText(i,1) ), 
                              std::string( m_selector.GetItemText(i,0) ), 
                              std::string( m_selector.GetItemText(i,2) )
                            );
	    
	    *pResult = 0;
    }

    void VariableSelector::OnCancel() 
    {
	    // TODO: Add your control notification handler code here
        m_variable->destroySelector();
    }

}//namespace

