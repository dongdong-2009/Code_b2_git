/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/SequenceEditorSaveAsDlg.h $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * Pops up a list of writable sequences to save a read only sequence to.
  *
  */

#if !defined(AFX_SEQUENCEEDITORSAVEASDLG_H__D2936A1B_EA00_48EF_9E1A_5701FA2B8EAA__INCLUDED_)
#define AFX_SEQUENCEEDITORSAVEASDLG_H__D2936A1B_EA00_48EF_9E1A_5701FA2B8EAA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SequenceEditorSaveAsDlg.h : header file
//
#include "resource.h"
#include "VideoSourcesTreeCtrl.h"

namespace TA_IRS_App
{

    class Sequence;

    class SequenceEditorSaveAsDlg : public CDialog
    {
            // Construction

        public:
            SequenceEditorSaveAsDlg(CWnd* pParent = NULL);   // standard constructor


            /**
              * getSelectedSequence
              *
              * gets the sequence that was selected
              *
              * @return the selected sequence if OK was clicked
              *         NULL if cancel was clicked
              */
            Sequence* getSelectedSequence();


            enum { IDD = IDD_SAVEAS_DIALOG };
            CButton    m_okButton;
            VideoSourcesTreeCtrl    m_sequenceTree;

        protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        protected:

            // Generated message map functions
            // {{AFX_MSG(SequenceEditorSaveAsDlg)
            virtual void OnOK();
            virtual void OnCancel();
            virtual BOOL OnInitDialog();
            afx_msg void OnDestroy();
            afx_msg void onSelchangedSequencetree(NMHDR* pNMHDR, LRESULT* pResult);
            afx_msg void onDblclkSequencetree(NMHDR* pNMHDR, LRESULT* pResult);
            // }}AFX_MSG
            DECLARE_MESSAGE_MAP()

        private:

            Sequence* m_selectedItem;

    };

} // TA_IRS_App

#endif // !defined(AFX_SEQUENCEEDITORSAVEASDLG_H__D2936A1B_EA00_48EF_9E1A_5701FA2B8EAA__INCLUDED_)
