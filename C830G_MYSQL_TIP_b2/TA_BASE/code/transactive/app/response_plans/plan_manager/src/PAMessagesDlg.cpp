/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PAMessagesDlg.cpp $
  * @author:  Ismar Duderija
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of PA message parameter.
  *
  */

#include "stdafx.h"
#include "planmanager.h"
#include "planmanagercommondefs.h"
#include "PaMessagesDlg.h"
#include "core/data_access_interface/pa_4669/src/PaDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaDvaMessage.h"
#include "core/data_access_interface/pa_4669/src/PaTrainDvaMessageAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/IPaTrainDvaMessage.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;

//column names for messages list control
//note that for train messages type column
//is not applicable
const CString MESSAGE_TYPE_CLMN  = "Type";
const CString MESSAGE_ID_CLMN        = "ID";
const CString MESSAGE_LABEL_CLMN = "Message";
const unsigned short NUM_CLMN        = 3;

const CString NOT_APPLICABLE = "NA";

/////////////////////////////////////////////////////////////////////////////
// CPAStationZones dialog
CPaMessagesDlg::CPaMessagesDlg(std::string messageType, PaMessageStepParameter& parameter, bool canEdit) :
    CPlanStepParameterEditDlg(parameter, CPaMessagesDlg::IDD, canEdit),
    m_PAMessage (parameter),
    m_messageType    (messageType)
{
    // {{AFX_DATA_INIT(CPaMessagesDlg)
    // }}AFX_DATA_INIT
}


CPaMessagesDlg::~CPaMessagesDlg ()
{
}

void CPaMessagesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPaMessagesDlg)
    DDX_Control (pDX, IDC_LIB_VERSION_TXT, m_libraryVerTxt);
    DDX_Control (pDX, IDC_LIB_SECTION_TXT, m_librarySecTxt);
    DDX_Control (pDX, IDC_MESSAGES_LIST, m_messagesList);
    DDX_Control (pDX, IDC_PARAMETER_NAME_TXT, m_parameterNameTxt);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaMessagesDlg, CPlanStepParameterEditDlg)
    // {{AFX_MSG_MAP(CPaMessagesDlg)
    ON_NOTIFY (LVN_ITEMCHANGED, IDC_MESSAGES_LIST, OnMessageListItemChanged)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CPaMessagesDlg::OnInitDialog()
{
    CPlanStepParameterEditDlg::OnInitDialog();

    m_parameterNameTxt.SetWindowText (m_messageType.c_str ());

    // train messages do not support library data on a message level
    // rather every message in the message table has same library version
    if (m_messageType.compare (PA_TRAIN_MESSAGE_TYPE) == 0)
    {
        CString libVersion;
        libVersion.Format ("%u", TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getLibraryVersion ());
        m_libraryVerTxt.SetWindowText (libVersion);
        m_librarySecTxt.SetWindowText (NOT_APPLICABLE);
        m_libraryVerTxt.EnableWindow (FALSE);
        m_librarySecTxt.EnableWindow (FALSE);
    }

    PopulateMessagesList ();
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CPAStationZones message handlers

void CPaMessagesDlg::OnCancel()
{
    // TODO: Add extra cleanup here

    CDialog::OnCancel();
}

void CPaMessagesDlg::OnOK()
{
    // remember the selected message info
    POSITION pos = m_messagesList.GetFirstSelectedItemPosition();
    if (pos == NULL)
        return;

    int nItem = m_messagesList.GetNextSelectedItem(pos);
    m_selectedMessage = m_messagesList.GetItemText (nItem, 2);

    DWORD messageKey;
    messageKey = m_messagesList.GetItemData (nItem);
    m_PAMessage.setPaMessageId (messageKey);


    CString tmp;
    m_libraryVerTxt.GetWindowText (tmp);
    unsigned short msgLibVer = atoi (tmp.GetBuffer (1));
    m_PAMessage.setPAMessageLibVersion (msgLibVer);

    if (m_messageType.compare (PA_STATION_MESSAGE_TYPE) == 0)
    {
        m_librarySecTxt.GetWindowText (tmp);
        unsigned short msgLibSection = atoi (tmp.GetBuffer (1));
        m_PAMessage.setPAMessageLibVersion (msgLibVer);
    }

    CDialog::OnOK();
}


void CPaMessagesDlg::PopulateMessagesList ()
{
    CWaitCursor wait;

    // determine what message types we are to display
    bool bTrainMessages = (m_messageType.compare (PA_TRAIN_MESSAGE_TYPE) == 0);

    DWORD newStyle = LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_INFOTIP;
    DWORD itemData;
    m_messagesList.SetExtendedStyle (m_messagesList.GetExtendedStyle () | newStyle);

    m_messagesList.InsertColumn (0, MESSAGE_ID_CLMN);
    m_messagesList.InsertColumn (1, MESSAGE_TYPE_CLMN);
    m_messagesList.InsertColumn (2, MESSAGE_LABEL_CLMN);

    int itemIdx, index = 0;
    CString messageType, messageID, messageLabel;
    try
    {
        if (bTrainMessages)
        {
            TA_Base_Core::IPaTrainDvaMessages  messages = TA_Base_Core::PaTrainDvaMessageAccessFactory::getInstance().getAllPaTrainDvaMessages ();

            for (TA_Base_Core::IPaTrainDvaMessagesIt it = messages.begin(); it != messages.end(); ++it)
            {
                TA_Base_Core::IPaTrainDvaMessage* message = *it;
                messageID.Format ("%u", message->getKey ());
                itemIdx = m_messagesList.InsertItem (index, messageID);
                itemData = message->getKey ();
                m_messagesList.SetItemData (itemIdx, itemData);
                m_messagesList.SetItemText (itemIdx, 1, NOT_APPLICABLE);
                messageLabel = message->getLabel().c_str();
                m_messagesList.SetItemText (itemIdx, 2, messageLabel);

                index++;
            }
        }
        else
        {
            // get only occ messages
            const unsigned long messageLocationKey(TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey());
            TA_Base_Core::IPaDvaMessages  messages = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessagesByLocationKey (messageLocationKey);

            for (TA_Base_Core::IPaDvaMessagesIt it = messages.begin(); it != messages.end(); ++it)
            {
                TA_Base_Core::IPaDvaMessage* message = *it;

                messageID.Format ("%u", message->getId ());
                itemIdx = m_messagesList.InsertItem (index, messageID);
                itemData = message->getKey ();
                m_messagesList.SetItemData (itemIdx, itemData);
                messageType  = message->getType().c_str();
                m_messagesList.SetItemText (itemIdx, 1, messageType);
                messageLabel = message->getLabel().c_str();
                m_messagesList.SetItemText (itemIdx, 2, messageLabel);

                index++;
            }
        }
    }
    catch (TA_Base_Core::DatabaseException&  dbe)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbe.what() );
        TA_Base_Bus::TransActiveMessage userMsg;
        // UINT selectedButton = userMsg.showMsgBox (IDS_UE_210103);
        // m_availableMessagesLst.EnableWindow (FALSE);
    }
    catch (TA_Base_Core::DataException&  de)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );
        TA_Base_Bus::TransActiveMessage userMsg;
        // UINT selectedButton = userMsg.showMsgBox (IDS_UE_210103);//"could not load messages from db"
        // m_availableMessagesLst.EnableWindow (FALSE);
    }

    // resize the columns
    CHeaderCtrl* pHeader =  static_cast <CHeaderCtrl*>(m_messagesList.GetDlgItem(0));
    ASSERT(pHeader);
    if (pHeader)
    {
        // turn off redraw until the columns have all been resized
        m_messagesList.SetRedraw(FALSE);

        // get the nbr of columns
        int iNbrOfCols = pHeader->GetItemCount();
        for (int iCurrCol = 0; iCurrCol < iNbrOfCols; iCurrCol++)
        {
            m_messagesList.SetColumnWidth(iCurrCol, LVSCW_AUTOSIZE_USEHEADER);
            int nColHdrWidth = m_messagesList.GetColumnWidth(iCurrCol);
            m_messagesList.SetColumnWidth(iCurrCol,LVSCW_AUTOSIZE);
            int nCurrWidth = m_messagesList.GetColumnWidth(iCurrCol);
            m_messagesList.SetColumnWidth(iCurrCol, max(nCurrWidth, nColHdrWidth) + 10);
        }

        // now that col sizing is finished,
        // invalidate so that the control is repainted
        m_messagesList.SetRedraw(TRUE);
        m_messagesList.Invalidate();
    }


}

void CPaMessagesDlg::OnMessageListItemChanged (NMHDR* pNMHDR, LRESULT* pResult)
{
    // only applies to station messages
    if (m_messageType.compare (PA_STATION_MESSAGE_TYPE) == 0)
    {
        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

        // Enable the OK button when there is a single message selected in the list
        if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
            (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
        {
            // update lib info for selected message
            POSITION pos = m_messagesList.GetFirstSelectedItemPosition();
            if (pos == NULL)
            {
                return;
            }

            int nItem = m_messagesList.GetNextSelectedItem(pos);
            unsigned long messageKey = m_messagesList.GetItemData (nItem);
            CString messageLibSection;
            CString messageLibType;

            TA_Base_Core::IPaDvaMessage* message = TA_Base_Core::PaDvaMessageAccessFactory::getInstance().getPaDvaMessage (messageKey, false);

            messageLibSection.Format ("%i", message->getTisLibrarySection ());
            m_librarySecTxt.SetWindowText (messageLibSection);
            messageLibType.Format ("%i", message->getTisLibraryVersion ());
            m_libraryVerTxt.SetWindowText (messageLibType);

        }
    }

    *pResult = 0;
}
