/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main view for configuring MMS Scheduling
  * This is the view that will display all scheduling information
  * and allow it to be configured.
  */

#if !defined(AFX_MMSSCHEDULINGVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_)
#define AFX_MMSSCHEDULINGVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/config_plugin_helper/src/IMainView.h"
#include "app/configuration/config_plugins/mms_scheduling/src/resource.h"

#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingGridControl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_App
{
    class MmsSchedulingComponent;
    class MMSSchedulingUtilityDlg;

    class MmsSchedulingView : public CFormView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    MmsSchedulingView();


        /**
         * Destructor
         */
	    virtual ~MmsSchedulingView();


	    DECLARE_DYNCREATE(MmsSchedulingView)

    public:

	    //{{AFX_DATA(MmsSchedulingView)
	    enum { IDD = IDD_MMS_SCHEDULE_VIEW };
	    CListCtrl	m_schedulingList;
	    //}}AFX_DATA

    public:

        /**
         * setComponentInterface
         *
         * This is called by the component interface to register itself with this view. This view
         * is the main view so it provides all communication between the various
         * bits and pieces. This method will also perform any final initialisation required on its
         * child views now that it has all the information it requires.
         */
        virtual void setComponentInterface(MmsSchedulingComponent* component);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         *
         * @param bool - This indicates if the buttons should be available or not.
         */
        virtual void refreshButtonAvailability(bool isAvailable);
        // Second implementation performs its own determination of whether
        // the buttons should be available or not
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(MmsSchedulingView)
	public:
	    virtual void OnInitialUpdate();
        virtual void OnDestroy();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL



    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
	    //{{AFX_MSG(MmsSchedulingView)
	    afx_msg void onButtonApply();
	    afx_msg void onButtonReset();
        afx_msg void onButtonSchedulingUtility();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    private:

        /**
         * loadItems
         *
         * This will load all the items and populate the grids.
         */
        void loadItems();
        
        // This is the maximum number of items we should show in a list in an error message
        static const int MAX_NUMBER_OF_ITEMS_TO_LIST;

        MmsSchedulingComponent* m_component;

        MmsSchedulingGridControl m_schedulingGrid;    

        MMSSchedulingUtilityDlg* m_scheduleUtility;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_MMSSCHEDULINGVIEW_H__E99415DD_E331_4C0D_A2FF_554CD680D7A1__INCLUDED_)
