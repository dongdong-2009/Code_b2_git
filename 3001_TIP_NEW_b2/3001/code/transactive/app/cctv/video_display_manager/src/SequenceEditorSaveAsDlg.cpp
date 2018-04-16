/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/SequenceEditorSaveAsDlg.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Pops up a list of writable sequences to save a read only sequence to.
  *
  */


#include "stdafx.h"
#include "videodisplaymanager.h"
#include "SequenceEditorSaveAsDlg.h"
#include "DisplayItemManager.h"
#include "Sequence.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{


    SequenceEditorSaveAsDlg::SequenceEditorSaveAsDlg(CWnd* pParent /*=NULL*/)
        : CDialog(SequenceEditorSaveAsDlg::IDD, pParent),
          m_selectedItem(NULL)
    {
        FUNCTION_ENTRY( "SequenceEditorSaveAsDlg" );

        // {{AFX_DATA_INIT(SequenceEditorSaveAsDlg)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT

        FUNCTION_EXIT;
    }


    void SequenceEditorSaveAsDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        CDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(SequenceEditorSaveAsDlg)
        DDX_Control(pDX, IDOK, m_okButton);
        DDX_Control(pDX, IDC_SAVEAS_SEQUENCETREE, m_sequenceTree);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(SequenceEditorSaveAsDlg, CDialog)
        // {{AFX_MSG_MAP(SequenceEditorSaveAsDlg)
        ON_WM_DESTROY()
        ON_NOTIFY(TVN_SELCHANGED, IDC_SAVEAS_SEQUENCETREE, onSelchangedSequencetree)
    ON_NOTIFY(NM_DBLCLK, IDC_SAVEAS_SEQUENCETREE, onDblclkSequencetree)
    // }}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL SequenceEditorSaveAsDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CDialog::OnInitDialog();

        // load the sequences
        std::map<std::string, Sequence*> sequences =
            DisplayItemManager::getInstance().getAllSequences();

        // build a vector
        std::vector<VideoInput*> inputs;
        for ( std::map<std::string, Sequence*>::iterator it = sequences.begin(); it != sequences.end(); it++ )
        {
            // insert only writeable sequences
            if (!it->second->getReadOnly())
            {
                inputs.push_back(it->second);
            }
        }

        m_sequenceTree.insertVideoSources(inputs);

        m_okButton.EnableWindow( FALSE );

        FUNCTION_EXIT;
        return TRUE;  // return TRUE unless you set the focus to a control
                      // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SequenceEditorSaveAsDlg::OnDestroy()
    {
        FUNCTION_ENTRY( "OnDestroy" );

        CDialog::OnDestroy();

        FUNCTION_EXIT;
    }


    void SequenceEditorSaveAsDlg::onSelchangedSequencetree(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onSelchangedSequencetree" );

        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        // enable the ok button
        Sequence* selectedItem = dynamic_cast<Sequence*>( m_sequenceTree.getSelectedInput() );
        m_okButton.EnableWindow( selectedItem != NULL );

        *pResult = 0;

        FUNCTION_EXIT;
    }


    Sequence* SequenceEditorSaveAsDlg::getSelectedSequence()
    {
        FUNCTION_ENTRY( "getSelectedSequence" );
        FUNCTION_EXIT;
        return m_selectedItem;
    }


    void SequenceEditorSaveAsDlg::onDblclkSequencetree(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onDblclkSequencetree" );

        Sequence* selectedSeq = dynamic_cast<Sequence*>(m_sequenceTree.getSelectedInput());

        // if a sequence is selected
        if (selectedSeq != NULL)
        {
            OnOK();
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void SequenceEditorSaveAsDlg::OnOK()
    {
        FUNCTION_ENTRY( "OnOK" );

        // populate with the selected item
        m_selectedItem = dynamic_cast<Sequence*>( m_sequenceTree.getSelectedInput() );

        CDialog::OnOK();

        FUNCTION_EXIT;
    }


    void SequenceEditorSaveAsDlg::OnCancel()
    {
        FUNCTION_ENTRY( "OnCancel" );

        // nothing was selected
        m_selectedItem = NULL;

        CDialog::OnCancel();

        FUNCTION_EXIT;
    }


} // TA_IRS_App
