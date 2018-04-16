/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/pa_train_dva_message/src/PaTrainDvaMessageGlobalView.h $
  * @author:  Jade Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains global parameter settings for Train DVA MEssages
  */

#pragma once

#include <afxcview.h>
#include <string>

#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_IRS_App
{
    class PaTrainDvaMessageGlobalView : public CFormView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    PaTrainDvaMessageGlobalView();

	    virtual ~PaTrainDvaMessageGlobalView();

	    DECLARE_DYNCREATE(PaTrainDvaMessageGlobalView)
        
        /**
         * setDetailView
         *
         * Sets the associated train message detail view (which is used to send out
         * configuration updates whenever changes are made).  This coupling is not
         * necessary, just done for convenience
         *
         * @param view defines the PaTrainDvaMessageDetailView object
         */
        void setDetailView(TA_Base_App::IDetailView* view);

    // Operations
    public:
        
        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(PaTrainDvaMessageGlobalView)
        enum { IDD = IDD_PA_TRAIN_DVA_MESSAGE_GLOBAL_VIEW };
	    public:
	    virtual void OnInitialUpdate();
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(PaTrainDvaMessageGlobalView)
    	afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();        	    
        afx_msg void onVersionChanged();
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:
        
    private:

        void refreshApplyResetButtons();
        
        CButton m_applyButton;
        CButton m_resetButton;
        TA_Base_Bus::TextColourEdit m_versionEdit;

        unsigned long m_existingVersion;
        TA_Base_Core::PrimitiveWrapper<unsigned long> m_enteredVersion;

        PaTrainDvaMessageDetailView*    m_detailView;
    };

    /////////////////////////////////////////////////////////////////////////////
    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
