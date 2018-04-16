/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Huang Qi
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */
#ifndef GROUPNAMEEDITDLG_H__INCLUDED_
#define GROUPNAMEEDITDLG_H__INCLUDED_

class PaRuntimeModel;

class GroupNameEditDlg : public CDialog
{
	DECLARE_DYNAMIC(GroupNameEditDlg)

public:
    GroupNameEditDlg( PaRuntimeModel& refBroadcastModel, const std::string& strGroupName, CWnd* pParent = NULL);
	virtual ~GroupNameEditDlg();

    std::string& getGroupName();
    unsigned long getNewGroupKey();

private:
    GroupNameEditDlg();
    const GroupNameEditDlg& operator=( const GroupNameEditDlg& );
    GroupNameEditDlg( const GroupNameEditDlg& );

// Dialog Data
	enum { IDD = IDD_DLG_GROUP_EDIT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
    virtual afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
    bool UpdateGroupName();

private:
    std::string m_strGroupName;
    unsigned long m_ulNewGroupKey;
    PaRuntimeModel& m_refBroadcastModel;
};

#endif
