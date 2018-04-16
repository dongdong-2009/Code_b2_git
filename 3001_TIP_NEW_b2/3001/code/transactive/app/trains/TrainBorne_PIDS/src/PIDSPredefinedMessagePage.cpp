// PIDSPredefinedMessagePage.cpp : implementation file
//

#include "stdafx.h"
#include "PIDSPredefinedMessagePage.h"
#include <algorithm>
#include "MessageFilter.h"
#include "core/utilities/src/DebugUtil.h"


// PIDSPredefinedMessagePage dialog
namespace TA_IRS_App
{
    PIDSPredefinedMessagePage::PIDSPredefinedMessagePage( CWnd* pParent /*=NULL*/ )
        : CDialog( PIDSPredefinedMessagePage::IDD, pParent ),
          m_messageSelectionListener( NULL ),
          m_ttisPredefinedMessages( NULL ),
          m_messageListCtrl( 1 ),
		  m_currentTtisLibrary( 0 )
    {
        //get all predefined message
    }

    PIDSPredefinedMessagePage::~PIDSPredefinedMessagePage()
    {
        FUNCTION_ENTRY( "PIDSPredefinedMessagePage" );
        FUNCTION_EXIT
    }

    void PIDSPredefinedMessagePage::Init()
    {
        FUNCTION_ENTRY( "Init" );

        //Init list ctrl
        m_messageListCtrl.SetExtendedStyle( m_messageListCtrl.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        RECT rect;
        m_messageListCtrl.GetClientRect( &rect );
        m_messageListCtrl.InsertColumn( 0, "Message Title" );
        m_messageListCtrl.SetColumnWidth( 0, rect.right );

        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }
        //clearFilter will cause populateMessageList execute
        clearFilter();
        clearMessageContent();

        // register with the pre-defined message singleton to be updated of a change
        m_ttisPredefinedMessages->registerCurrentVersionUser( this );

        FUNCTION_EXIT
    }

    TA_Base_Core::PredefinedMessage PIDSPredefinedMessagePage::getSelectedMessage() const
    {
        FUNCTION_ENTRY( "getSelectedMessage" );
        const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();

        TA_ASSERT ( messagePointer != NULL,
                    "getSelectedMessage() called when there is no valid message selected. Call hasValidSelection() first." );

        FUNCTION_EXIT
        return *messagePointer;
    }

    const TA_Base_Core::PredefinedMessage*  PIDSPredefinedMessagePage::getSelectedMessageData() const
    {
        FUNCTION_ENTRY( "getSelectedMessageData" );
        const TA_Base_Core::PredefinedMessage* message = NULL;

        // get the selected index for the normal priority list
        int selected = -1;
        POSITION currSel = m_messageListCtrl.GetFirstSelectedItemPosition();

        // if something is selected
        if( currSel != NULL )
        {
            // get the selected index
            selected = m_messageListCtrl.GetNextSelectedItem( currSel );
        }



        if( selected > -1 )
        {         
            message = reinterpret_cast<TA_Base_Core::PredefinedMessage*> ( m_messageListCtrl.GetItemData( selected ) );

            // anything selected should point to a valid message
            TA_ASSERT( message != NULL,
                       "The selected message is not a valid message" );
        }

        // otherwise, return NULL

        FUNCTION_EXIT
        return message;
    }


    void PIDSPredefinedMessagePage::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        DDX_Control( pDX, IDC_EDIT_MESSAGE_CONTENT, m_messageContent );
        DDX_Control( pDX, IDC_LIST_PREDFEINED, m_messageListCtrl );
        DDX_Control( pDX, IDC_EDIT_FILTER, m_filter );
    }

    BOOL PIDSPredefinedMessagePage::OnInitDialog()
    {
        CDialog::OnInitDialog();

        return TRUE;
    }

    void PIDSPredefinedMessagePage::clearFilter()
    {
        m_filter.SetWindowText( "" );
    }

    void PIDSPredefinedMessagePage::clearMessageList()
    {
        m_messageListCtrl.DeleteAllItems();
    }

    void PIDSPredefinedMessagePage::clearMessageContent()
    {
        m_messageContent.SetWindowText( "" );
    }

    void PIDSPredefinedMessagePage::populateMessageList()
    {
        FUNCTION_ENTRY( "populateMessageList" );

        clearMessageContent();
        clearMessageList();

        MessageFilter filter( getFilterString() );


        std::vector<TA_Base_Core::PredefinedMessage>::iterator ite = m_allMessages.begin();
        int npos = -1, index = -1;
        while( ite != m_allMessages.end() )
        {
            ite = std::find_if( ite, m_allMessages.end(), filter );
            if( ite != m_allMessages.end() )
            {
                index = m_messageListCtrl.InsertItem( ++npos, ite->description.c_str() );
                if( -1 == index )
                {
                    --npos;
                }
                else
                {
                    m_messageListCtrl.SetItemData( index, reinterpret_cast<DWORD_PTR>( &(*ite) ) );
                }
                ++ite;
            }
        }

        FUNCTION_EXIT
    }


    std::string PIDSPredefinedMessagePage::getFilterString()
    {
        CString filter( "" );
        m_filter.GetWindowText( filter );
        return LPCTSTR( filter );
    }



    /*
    void PIDSPredefinedMessagePage::OnClickPredefinedMsgList(NMHDR* pNMHDR, LRESULT* pResult)
    {

    	m_messageListCtrl.SetItemState(-1, 0, LVIS_SELECTED);

    	POSITION currSel = m_messageListCtrl.GetFirstSelectedItemPosition();

    	// if nothing is selected
    	if(currSel == NULL)
    	{
    		// update the display
    		updatePreviewArea();
    	}

    	if(pResult != NULL)
    	{
    		*pResult = 0;
    	}
    }
    */
    void PIDSPredefinedMessagePage::OnFilterChange()
    {
        //repopulate the ctrllist
        populateMessageList();
        updatePreviewArea();
    }

    void PIDSPredefinedMessagePage::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener )
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    void PIDSPredefinedMessagePage::windowShown()
    {
        updatePreviewArea();
    }

    void PIDSPredefinedMessagePage::updatePreviewArea()
    {

        TA_ASSERT( m_messageSelectionListener != NULL, "Not set m_messageSelectionListener")
        DisplayAttributeManager* displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();
		TA_ASSERT(displayAttributeManager != NULL, "" );
		displayAttributeManager->setPerrunBtnCheck();

		// get the selected message
        const TA_Base_Core::PredefinedMessage* message = getSelectedMessageData();

        // NULL means nothing is selected
        if ( message != NULL )
        {
            // set the message text
            m_messageContent.SetWindowText( message->message.c_str() );

            // notify the owning window
            if ( m_messageSelectionListener != NULL )
            {
                // a message is selected
                m_messageSelectionListener->messageSelectionChanged( true );
            }
        }
        else
        {
            // clear the text in the preview window
            m_messageContent.SetWindowText( "" );

            // a message is not selected
            m_messageSelectionListener->messageSelectionChanged( false );
        }
    }

    void PIDSPredefinedMessagePage::OnDestroy()
    {
        setMessageSelectionListener( NULL );
        if ( NULL != m_ttisPredefinedMessages )
        {
            // deregister for pre-defined message library version update
            m_ttisPredefinedMessages->deregisterCurrentVersionUser( this );
            m_ttisPredefinedMessages = NULL;
            TTISPredefinedMessages::removeInstance();
        }
        CDialog::OnDestroy();
    }

    LRESULT PIDSPredefinedMessagePage::OnUpdateCurrentTTISVersion( WPARAM wParam, LPARAM lParam )
    {
		unsigned long newLibrary = wParam;
		if(newLibrary == m_currentTtisLibrary)
		{
			return 0;
		}
		m_currentTtisLibrary = newLibrary;
        //fill the messages list
        std::vector<TA_Base_Core::PredefinedMessage> normalMessages =
            m_ttisPredefinedMessages->getNormalMessages();
        std::vector<TA_Base_Core::PredefinedMessage> emergencyMessages =
            m_ttisPredefinedMessages->getEmergencyMessages();
        m_allMessages.clear();
        m_allMessages.resize( normalMessages.size() + emergencyMessages.size() );
        std::copy( normalMessages.begin(), normalMessages.end(), m_allMessages.begin() );
        std::copy( emergencyMessages.begin(), emergencyMessages.end(), m_allMessages.begin() + normalMessages.size() );

        // rebuild the lists
        populateMessageList();

        // update all the stuff to do with selected items in lists
        updatePreviewArea();

        return 0;
    }

    bool PIDSPredefinedMessagePage::hasValidSelection() const
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
        // get the selected message
        // NULL means no selection
        const TA_Base_Core::PredefinedMessage* messagePointer = getSelectedMessageData();

        if ( messagePointer != NULL )
        {
            LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
            return true;
        }

        LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectPredefinedPage::hasValidSelection" );
        return false;
    }

    void PIDSPredefinedMessagePage::OnItemchangedPredefined( NMHDR* pNMHDR, LRESULT* pResult )
    {
        NM_LISTVIEW* pNMListView = ( NM_LISTVIEW* )pNMHDR;

        // Only interested in state changes
        if ( pNMListView->uChanged == LVIF_STATE )
        {
            // if it is being selected
            if ( ( ( pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED ) &&
                    ( ( pNMListView->uOldState & LVIS_SELECTED ) == 0 ) )
            {
                // update the display
                updatePreviewArea();
            }
        }

        *pResult = 0;
    }

    BEGIN_MESSAGE_MAP( PIDSPredefinedMessagePage, CDialog )
        ON_EN_CHANGE( IDC_EDIT_FILTER, OnFilterChange )
        //ON_NOTIFY(NM_CLICK, IDC_LIST_PREDFEINED, OnClickPredefinedMsgList)
        ON_MESSAGE( WM_UPDATE_CURRENT_TTIS_VERSION, OnUpdateCurrentTTISVersion )
        ON_NOTIFY( LVN_ITEMCHANGED, IDC_LIST_PREDFEINED, OnItemchangedPredefined )
        ON_WM_DESTROY()
    END_MESSAGE_MAP()

}
// PIDSPredefinedMessagePage message handlers
