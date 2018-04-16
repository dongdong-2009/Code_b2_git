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
#ifndef DVAMSGDESCEDITDLG_H__INCLUDED_
#define DVAMSGDESCEDITDLG_H__INCLUDED_


class PaRuntimeModel;

class DVAMsgDescEditDlg : public CDialog
{
	DECLARE_DYNAMIC(DVAMsgDescEditDlg)

public:
    DVAMsgDescEditDlg( PaRuntimeModel& refBroadcastModel, int nMsgType, CWnd* pParent = NULL);
	virtual ~DVAMsgDescEditDlg();

    std::string& getBroadcastDes();

private:
    DVAMsgDescEditDlg();
    const DVAMsgDescEditDlg& operator=( const DVAMsgDescEditDlg& );
    DVAMsgDescEditDlg( const DVAMsgDescEditDlg& );

// Dialog Data
	enum { IDD = IDD_DLG_BROADCAST_DES_EDIT };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
    virtual afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()

private:
    int m_nMsgType;
    std::string m_strCurDescription;
    PaRuntimeModel& m_refBroadcastModel;
};

#endif