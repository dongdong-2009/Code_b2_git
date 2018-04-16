/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SelectTextDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class presents a list of pre-defined text messages the user can select from.
  * It shows the associated text with each pre-defined message and permits it to be
  * altered.
  *
  */

#if !defined(AFX_SELECTTEXTDIALOG_H__CB5A00F8_4F70_11D7_B14D_0050BAB1D931__INCLUDED_)
#define AFX_SELECTTEXTDIALOG_H__CB5A00F8_4F70_11D7_B14D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include <string>
#include "core/message/src/NameValuePair.h"

namespace TA_Base_Core // Forward declarations
{
	class MessageType;
}

namespace TA_Base_Bus
{
    class RightsLibrary; // Forward declaration
};

/////////////////////////////////////////////////////////////////////////////
// CSelectTextDialog dialog

class CSelectTextDialog : public CDialog
{
// Construction
public:

	CSelectTextDialog(CWnd* pParent, 
                        unsigned long radioEntityKey,
                        CString messageName, 
                        CString messageText, 
                        CString customSDSName); 

	virtual ~CSelectTextDialog();

	static std::string getPredefinedSDSText(CString messageName);
	static void loadPredefinedMessageCombo(CComboBox& combo);

// Dialog Data
	//{{AFX_DATA(CSelectTextDialog)
	enum { IDD = IDD_SELECTTEXTMESSAGE };
	CButton	m_deleteButton;
    CEdit   m_messageTextEdit;
	CButton	m_updateButton;
	CButton	m_newButton;
	CListBox	m_TextList;
	CString	m_messageText;
	//}}AFX_DATA
	CString			m_messageName;
	bool			m_isCustom;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectTextDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectTextDialog)
	afx_msg void OnFreetext();
	virtual BOOL OnInitDialog();
	afx_msg void onSelChangeMessageList();
	afx_msg void onNewText();
	afx_msg void onUpdateText();
	afx_msg void onDeleteText();
	afx_msg void onChangeMessageText();
	afx_msg void onApply();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void LoadPredefinedList();
	bool IsOperationPermitted(TA_Base_Bus::AccessControlledAction action);
	void AuditOperation(const TA_Base_Core::MessageType& messageType,TA_Base_Core::DescriptionParameters parameters);
	void EnableButtons();
	std::string getSelectedPredefinedSDS();

	TA_Base_Bus::RightsLibrary*		m_rightsLibrary;
	unsigned long				m_radioEntityKey;


private:

    /**
     * setIsCustom used to internally modified the text custom (i.e. changed) state
     *  (so can track state changes)
     */
    void    setIsCustom(bool isCustom);

    /// Used for message state change tracking, stores text in text area at last point
    /// setIsCustom was called with false
    CString m_originalMessageText;

    CString m_customSDSMessageName;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTTEXTDIALOG_H__CB5A00F8_4F70_11D7_B14D_0050BAB1D931__INCLUDED_)
