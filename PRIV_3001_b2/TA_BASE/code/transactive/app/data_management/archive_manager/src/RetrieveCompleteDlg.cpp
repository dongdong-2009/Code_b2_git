/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/data_management/archive_manager/src/RetrieveCompleteDlg.cpp $
  * @author:  Katherine Thomson
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Displayed if there are errors during the retrieval process.
  */

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/RetrieveCompleteDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // RetrieveCompleteDlg dialog


    RetrieveCompleteDlg::RetrieveCompleteDlg(const TA_ArchiveManager::FileStatusMap& fileStatusMap, CWnd* pParent /*=NULL*/)
	    : CDialog(RetrieveCompleteDlg::IDD, pParent),
          m_fileStatusMap( fileStatusMap )
    {
	    //{{AFX_DATA_INIT(RetrieveCompleteDlg)
	    //}}AFX_DATA_INIT
    }


    void RetrieveCompleteDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(RetrieveCompleteDlg)
	    DDX_Control(pDX, IDC_ERROR_TREE, m_errorTree);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(RetrieveCompleteDlg, CDialog)
	    //{{AFX_MSG_MAP(RetrieveCompleteDlg)
	    ON_BN_CLICKED(IDNO, OnNo)
	    ON_BN_CLICKED(IDYES, OnYes)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // RetrieveCompleteDlg message handlers

    BOOL RetrieveCompleteDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
        
        //  Give this dialog its parent's icons.

        SetIcon( GetParent()->GetIcon( true ), TRUE );			// Set big icon
        SetIcon( GetParent()->GetIcon( false ), FALSE );		// Set small icon

        //  Display an hour-glass cursor.

        HCURSOR hOldCursor = SetCursor( LoadCursor( NULL, IDC_APPSTARTING ) );

        // If there are no errors to report, return yes, because we want to continue.

        if ( m_fileStatusMap.empty() )
        {
            CDialog::EndDialog( IDYES );
        }

        insertItems( TA_ArchiveManager::EX_FAIL, std::string( "File(s) Containing Errors" ) );
        insertItems( TA_ArchiveManager::EX_OKWARN, std::string( "File(s) Containing Warnings" ) );
        insertItems( TA_ArchiveManager::UNKNOWN, std::string( "File(s) Containing An Unknown Status" ) );
         
        // If there are no errors to report, return yes, because we want to continue.
       
        if ( 0 == m_errorTree.GetCount() )
        {
            CDialog::EndDialog( IDYES );
        }

        return TRUE;  // return TRUE unless you set the focus to a control
	          // EXCEPTION: OCX Property Pages should return FALSE
    }

 
    void RetrieveCompleteDlg::OnNo() 
    {
        CDialog::EndDialog( IDCANCEL );
    }


    void RetrieveCompleteDlg::OnYes() 
    {
        CDialog::EndDialog( IDOK );
    }


    void RetrieveCompleteDlg::insertItems( TA_ArchiveManager::EErrorStatus status, const std::string& rootMsg )
    {
        std::pair< TA_ArchiveManager::FileStatusMap::const_iterator, 
            TA_ArchiveManager::FileStatusMap::const_iterator > range = 
            m_fileStatusMap.equal_range( status );           

        if( ( range.first != range.second ) && ( m_fileStatusMap.end() != range.first ) )
        {
            // Insert a root item using the structure. We must
            // initialize a TVINSERTSTRUCT structure and pass its
            // address to the call. 

            TVINSERTSTRUCT tvInsert;
            tvInsert.hParent = NULL;
            tvInsert.hInsertAfter = NULL;
            tvInsert.item.mask = TVIF_TEXT;
            tvInsert.item.pszText = const_cast< char* >( rootMsg.c_str() );
            HTREEITEM hRoot = m_errorTree.InsertItem(&tvInsert);

            for ( ; range.first != range.second; ++range.first )
            {
                // Insert the files as subitems.

                HTREEITEM hItem = m_errorTree.InsertItem(TVIF_TEXT,
                   range.first->second.c_str(), 0, 0, 0, 0, 0, hRoot, NULL);
            }
        }
    }

    
} // TA_Base_App
