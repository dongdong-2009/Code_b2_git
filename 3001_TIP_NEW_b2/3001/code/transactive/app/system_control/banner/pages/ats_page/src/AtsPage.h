/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/ats_page/src/AtsPage.h $
  * @author:  C. DeWolfe
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * The class, AtsPage, provides a dialog which contains a list of ATS alarms
  * TODO: describe functionality
  * This class derives from BannerPage which allows to be a page in the Banner Framework.
  *
  */

//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES
#if !defined(AFX_ATSALARMS_H__3FA3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_)
#define AFX_ATSALARMS_H__3FA3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AtsPage.h : header file
//

#include "app\system_control\banner\pages\ats_page\src\AtsBannerPage_rc.h"
#include "bus\banner_framework\src\BannerPage.h"
#include "bus\banner_framework\src\SilenceButton.h"
#include "bus\sound\sound_client\src\ISoundObserver.h"

/////////////////////////////////////////////////////////////////////////////
// AtsPage dialog

class AtsPage : public TA_Base_Bus::BannerPage
{
// Construction
public:
    AtsPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog);

	virtual ~AtsPage();

    /**
     * onButtonDoubleClick
     *
     * Do nothing when our tab is double click
     */
    virtual void onButtonDoubleClick();

    /**
     * addBannerPage
     *
     * Tells the banner page to add itself to the banner.
     */ 
    virtual void addBannerPage();

// Dialog Data
	//{{AFX_DATA(AtsPage)
	enum { IDD = IDD_ATS_ALARMS };
	CWebBrowser2	m_totalsExplorerCtrl;
	CWebBrowser2	m_listExplorerCtrl;
	//}}AFX_DATA
    TA_Base_Bus::SilenceButton m_silenceButton;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(AtsPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void OnPaint();	
	//}}AFX_VIRTUAL
	afx_msg void OnSilence();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(AtsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:

    bool fileExists(const std::string& fileName);

private:

    bool    m_internalRepaint;
	std::string	m_atsAlarmListFilename;
	std::string	m_atsAlarmTotalsFilename;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATSALARMS_H__3FA3A7A2_7A1A_4539_AA8B_9F1D15675D56__INCLUDED_)
