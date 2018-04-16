// src/TextEditDialog.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "FontHelper.h"
#include "TextEditDialog.h"


// TextEditDialog dialog

IMPLEMENT_DYNAMIC(TextEditDialog, CDialog)

TextEditDialog::TextEditDialog( CWnd* pParent ) : CDialog( IDD_TEXT_DIALOG, pParent ),
Title( "" ),
Content( "" ),
LimitTextLength( 800u )
{

}

TextEditDialog::~TextEditDialog()
{
}

void TextEditDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control( pDX, IDC_EDIT_TEXT, m_edtContent );
}


BEGIN_MESSAGE_MAP(TextEditDialog, CDialog)
    ON_BN_CLICKED( IDOK, OnOK )
END_MESSAGE_MAP()

// TextEditDialog message handlers

BOOL TextEditDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText( Title.c_str() );
    m_edtContent.SetWindowText( Content.c_str() );
    m_edtContent.SetLimitText( LimitTextLength );

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void TextEditDialog::OnOK()
{
    CString csText = "";
    m_edtContent.GetWindowText( csText );

    Content = csText.operator LPCSTR();

    CDialog::OnOK();
}
