/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/SequenceEditorDlg.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  */
#if !defined(AFX_SEQUENCEEDITORDLG_H__D1D23CB3_79FD_11D7_B17A_0050BAB094A9__INCLUDED_)
#define AFX_SEQUENCEEDITORDLG_H__D1D23CB3_79FD_11D7_B17A_0050BAB094A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SequenceEditorDlg.h : header file
//

#include "Label.h"
#include "VideoSourcesTreeCtrl.h"
#include "VideoSourcesListBox.h"
#include "TheToolTipCtrl.h"
#include "ISequenceEditorView.h" 
#include "VideoDisplayManagerPresenter.h"
#include <list>

namespace TA_Base_Core
{
	class AuditMessageSender;
}

namespace TA_IRS_App
{
    // forward decalaration
    class Sequence;
    class Camera;

    class SequenceEditorDlg : public CDialog,public ISequenceEditorView
    {
		DECLARE_DYNAMIC(SequenceEditorDlg)
    // Construction
    public:
        SequenceEditorDlg(CWnd* pParent = NULL, VideoSourcesTreeCtrl* pSequenceTreeCtrl= NULL);   // standard constructor

        void enableAddButton(BOOL enable);
        BOOL enableRemoveButton(BOOL enable);
        void enableOrderButtons(BOOL top, BOOL up, BOOL down, BOOL bottom);

        // Dialog Data
        // {{AFX_DATA(SequenceEditorDlg)
        enum { IDD = IDD_SEQUENCEEDITOR_DIALOG };
        CSpinButtonCtrl    m_spinControl;
        VideoSourcesListBox    m_sequencesListBox;
        VideoSourcesTreeCtrl    m_cameraTreeCtrl;
        // TD16691
        VideoSourcesTreeCtrl*    m_sequenceTreeCtrl;
        // TD16691
        CButton    m_cancelButton;
        CButton    m_downButton;
        CButton    m_upButton;
        CButton    m_topButton;
        CButton    m_saveButton;
        CButton    m_removeButton;
        CButton    m_bottomButton;
        CButton    m_addButton;
        CLabel    m_sequenceLabel;
        CEdit    m_sequenceDescription;
        CEdit    m_sequenceDwellTime;
		//CButton  m_closeDlgButton;
        // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(SequenceEditorDlg)
        public:
        virtual int DoModal(Sequence* sequence,unsigned long stationID);
        virtual BOOL PreTranslateMessage(MSG* pMsg);

		void SetPresenter ( VideoDisplayManagerPresenter * presenter ) 
		{
			m_presenter = presenter;
		}

		void Editsequence ( Sequence * sequence,unsigned long stationID );

        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL
      //ISequenceEditorView

		

		
    // Implementation
    protected:
        HICON m_hIcon;                                        // Dialog icon.

        // Generated message map functions
        // {{AFX_MSG(SequenceEditorDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnOK();
        afx_msg void OnUpdateSeDescriptionEditBox();
        afx_msg void OnUpdateSeDwelltimeEditBox();
        afx_msg void OnSeAddButton();
        afx_msg void OnSeRemoveButton();
        afx_msg void OnSeSaveButton();
        afx_msg void OnSeTopButton();
        afx_msg void OnSeUpButton();
        afx_msg void OnSeDownButton();
        afx_msg void OnSeBottomButton();
        afx_msg void OnDestroy();
        afx_msg LRESULT onCameraUpdated(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSequenceUpdated(WPARAM wParam, LPARAM lParam);
        afx_msg void onDblclkCameratree(NMHDR* pNMHDR, LRESULT* pResult);
	//	afx_msg void OnCloseButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

        /**
        * LoadButtonIcons
        *
        * Load and set icons on the dialog buttons.
        */
        void LoadButtonIcons();

        /**
        * SetToolTips
        *
        * Create the tool tips for the buttons.
        */
        void setToolTips();

        void loadExistingVideoSources();

        void loadTree();

        bool isSourcesModified();

        // workaround so disabled buttons still display tooltips (MFC Bug)
        void handleTooltipsActivation(MSG *pMsg, const CButton& button);

    private:
		void submitSequenceEditEvent(Sequence* editSequence);
        // enables/disables the save button
        void enableSaveButton();

        // For all button icons.
        HICON m_addIcon;
        HICON m_removeIcon;
        HICON m_topIcon;
        HICON m_upIcon;
        HICON m_downIcon;
        HICON m_bottomIcon;

        // Tool tip
        CTheToolTipCtrl m_ctlToolTip;        // For the buttons tool tips.

        // this is the sequence (valid only during the modal dialog)
        Sequence* m_sequence;

        // normally the Sequence* is used to retreive existing (saved) values
        // but in the case of a previously unsaved sequence this is needed to
        // store the default dwell time
        CString m_existingDwellTime;

        // this is the modified list of video sources
        std::list<Camera*> m_sequenceVideoSources;

        // flag to indicate the record has been modified
        BOOL m_changed;

        // sequence updates are 2 stage - description and the rest
        // one is a comms message, and the other is an online update
        time_t m_lastUpdatedTime;

		VideoDisplayManagerPresenter* m_presenter;
		unsigned long m_stationID;
		std::auto_ptr<TA_Base_Core::AuditMessageSender> m_auditSender;
    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_SEQUENCEEDITORDLG_H__D1D23CB3_79FD_11D7_B17A_0050BAB094A9__INCLUDED_)
