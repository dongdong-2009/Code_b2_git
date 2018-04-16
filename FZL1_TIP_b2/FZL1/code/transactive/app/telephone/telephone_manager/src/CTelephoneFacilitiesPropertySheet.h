/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/telephone/telephone_manager/src/CTelephoneFacilitiesPropertySheet.h $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Refer to 4669P62040005-Telephone_Manager_GUI_Design_Note.DND section Telephone
  * Directory Facilities for description.
  */

#if !defined(CTelephoneFacilitiesPropertySheet_80F90849_5111_4d85_8F5F_379694FB12C9__INCLUDED_)
#define CTelephoneFacilitiesPropertySheet_80F90849_5111_4d85_8F5F_379694FB12C9__INCLUDED_

#include <map>

#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisableEx.h"//CL 17641 fjy
#include "app/telephone/telephone_manager/src/CTelephoneDirectLinePage.h"
#include "app/telephone/telephone_manager/src/CTelephoneDirectorySearchPage.h"
#include "app/telephone/telephone_manager/src/CTelephoneConfigureDirectLinesPage.h"
#include "app/telephone/telephone_manager/src/CTelephoneConfigureDirectoryPage.h"
#include "core/utilities/src/RunParams.h"

class ItaTelephoneManagerSummary;

class CDisableTabCtrl : public CTabCtrlWithDisable
{
public:
	virtual BOOL IsTabEnabled(int iTab);
	/*{
		//std::map<unsigned int, unsigned int> temp = m_DisabledPages;
		//std::map<unsigned int, unsigned int>::iterator iter = m_DisabledPages.find(iTab);	
		//return ((iter != m_DisabledPages.end()) ? true : false );
		return iTab != 3;
	}*/
};


class CTelephoneFacilitiesPropertySheet : public CMultiFontPropertySheet, public TA_Base_Core::RunParamUser
{

public:
    CTelephoneFacilitiesPropertySheet(CWnd* pParentWnd = NULL);


    virtual ~CTelephoneFacilitiesPropertySheet();

    void setTelephoneManagerSummary(ItaTelephoneManagerSummary* ptransactiveInterfaces);
// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CTelephoneFacilitiesPropertySheet)
    protected:
    virtual BOOL OnInitDialog();
    virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
    afx_msg void OnDestroy();
    // }}AFX_VIRTUAL

public:

    /**
      * getDirectLinePage
      *
      * Returns the CTelephoneDirectLinePage object
      *
      * @return CTelephoneDirectLinePage*
      *
      * @exception <exceptions> Optional
      */
    CTelephoneDirectLinePage* getDirectLinePage();


    /**
      * getDirectorySearchPage
      *
      * <description>
      *
      * @return CTelephoneDirectorySearchPage*
      *
      * @exception <exceptions> Optional
      */
    CTelephoneDirectorySearchPage* getDirectorySearchPage();


    /**
      * getConfigureDirectLinesPage
      *
      * <description>
      *
      * @return CTelephoneConfigureDirectLinesPage*
      *
      * @exception <exceptions> Optional
      */
    CTelephoneConfigureDirectLinesPage* getConfigureDirectLinesPage();


    /**
      * getConfigureDirectoryPage
      *
      * <description>
      *
      * @return CTelephoneConfigureDirectoryPage*
      *
      * @exception <exceptions> Optional
      */
    CTelephoneConfigureDirectoryPage* getConfigureDirectoryPage();


    /**
      * enablePage
      *
      * Make a property page active if bEnable = true
      * else make the page inactive
      *
      * @return void enablePage
      * @param : int nPage
      * @param : BOOL bEnable
      *
      * @exception <exceptions> Optional
      */
    void enablePage (int nPage, BOOL bEnable);

    /*
     * onRunParamChange (interface implementation)
     *
     * @see RunParamUser::onRunParamChange for more details
     *
     */
    virtual void onRunParamChange(const std::string& name, const std::string& value);

protected:


    /**
      * CTelephoneDirectLinePage
      */
    CTelephoneDirectLinePage m_directLinePage;


    /**
      * CTelephoneDirectorySearchPage
      */
    CTelephoneDirectorySearchPage m_directorySearchPage;

    /**
      * CTelephoneConfigureDirectLinesPage
      */
    CTelephoneConfigureDirectLinesPage m_configureDirectLinesPage;

    /**
      * CTelephoneConfigureDirectoryPage
      */
    CTelephoneConfigureDirectoryPage m_configureDirectoryPage;

// Generated message map functions
protected:

    /**
      * we save the current page in TCN_SELCHANGING
      */
    int m_nLastActive;

    /**
      * list of indexes of disabled pages
      */
    std::map<unsigned int, unsigned int> m_DisabledPages;

    // {{AFX_MSG(CTelephoneFacilitiesPropertySheet)
    // }}AFX_MSG

    DECLARE_MESSAGE_MAP()


public: 
    //static std::map<unsigned int, unsigned int> m_staticDisabledPages;
    static bool m_bInit;


private:
	virtual BOOL PreTranslateMessage(MSG* pMsg); //CL 17641 fjy
	
	 
	CDisableTabCtrl m_tabCtrl; // CL 17641 fjy;
	
	
	
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
};
#endif // !defined(CTelephoneFacilitiesPropertySheet_80F90849_5111_4d85_8F5F_379694FB12C9__INCLUDED_)
