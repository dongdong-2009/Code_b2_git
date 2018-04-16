/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/TisLogViewerDlg.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#include "stdafx.h"

#include "resource.h"

#include "app/signs/tis_log_viewer/src/DisplayRequestSender.h"
#include "app/signs/tis_log_viewer/src/RightsManager.h"
#include "app/signs/tis_log_viewer/src/TisLogViewer.h"
#include "app/signs/tis_log_viewer/src/TisLogViewerDlg.h"
#include "app/signs/tis_log_viewer/src/UserMessages.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"  // TES891
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/AppLauncher.h"

#include <sstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78		//zhongjie++
#define MINVALUE				2400	//zhongjie++
#define NUMSCREENS				3		//zhongjie++


using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Bus::TransActiveDialog;

using TA_IRS_Core::IDisplayRequest;


namespace TA_IRS_App
{


        TisLogViewerDlg::TisLogViewerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent /*=NULL*/)
            : TransActiveDialog(genericGUICallback, TisLogViewerDlg::IDD, pParent),
            m_pidDisplayDialog(),
              m_reinstateDisplayRequestDialog(NULL),
              m_canDisplayTTISFreeText(false),
              m_canDisplayTTISPredefined(false),
              m_canDisplaySTISFreeText(false),
              m_canDisplaySTISPredefined(false),
              m_canSubmitRatis(false),
              m_currentSelection(IRequestSelectionListener::None),
              m_progressBarVisible(false),
              m_isReinstatable(false),
              m_locationKey(0),
              m_displayRequestSender( NULL )
        {
            FUNCTION_ENTRY( "TisLogViewerDlg" );

            m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

            FUNCTION_EXIT;
        }


        TisLogViewerDlg::~TisLogViewerDlg()
        {
            FUNCTION_ENTRY( "~TisLogViewerDlg" );

            delete m_reinstateDisplayRequestDialog;
            m_reinstateDisplayRequestDialog = NULL;

            delete m_displayRequestSender;
            m_displayRequestSender = NULL;

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::DoDataExchange(CDataExchange* pDX)
        {
            FUNCTION_ENTRY( "DoDataExchange" );

            TransActiveDialog::DoDataExchange(pDX);
            //{{AFX_DATA_MAP(TisLogViewerDlg)
            DDX_Control(pDX, IDC_PROGRESS, m_progressBar);
            DDX_Control(pDX, IDC_MESSAGE_CONTENT, m_messageContent);
            DDX_Control(pDX, IDC_REQUEST_LIST, m_requestList);
            //}}AFX_DATA_MAP

            FUNCTION_EXIT;
        }


        BEGIN_MESSAGE_MAP(TisLogViewerDlg, TransActiveDialog)
            //{{AFX_MSG_MAP(TisLogViewerDlg)
            ON_COMMAND(ID_FILE_EXIT, OnFileExit)
            ON_WM_KEYDOWN()
            ON_WM_CLOSE()
            ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
            ON_COMMAND(ID_HELP_TISLOGVIEWER, OnHelp)
            ON_BN_CLICKED(IDC_CLOSE, OnCloseButton)
            ON_BN_CLICKED(IDC_VIEW_PID, OnViewPids)
            ON_BN_CLICKED(IDC_REINSTATE, OnReinstate)
            ON_MESSAGE(WM_UPDATE_RIGHTS, onRightsChanged)
            ON_WM_DESTROY()
        ON_NOTIFY(NM_CLICK, IDC_REQUEST_LIST, OnClickRequestList)
        //}}AFX_MSG_MAP
        END_MESSAGE_MAP()

        /////////////////////////////////////////////////////////////////////////////
        // TisLogViewerDlg message handlers

        void TisLogViewerDlg::init()
        {
            FUNCTION_ENTRY( "init" );

            // TES891 <<
            std::stringstream conversionStream;
            conversionStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
            conversionStream >> m_locationKey;

            m_displayRequestSender = new DisplayRequestSender( m_locationKey );


            // register for rights changes
            RightsManager::getInstance().registerForRightsChanges(this);
            m_canDisplayTTISFreeText = RightsManager::getInstance().canDisplayTTISFreeText();
            m_canDisplayTTISPredefined = RightsManager::getInstance().canDisplayTTISPredefined();
            m_canDisplaySTISFreeText = RightsManager::getInstance().canDisplaySTISFreeText();
            m_canDisplaySTISPredefined = RightsManager::getInstance().canDisplaySTISPredefined();
            m_canSubmitRatis = RightsManager::getInstance().canSubmitRatis();

            // hide the progress bar
            showProgressBar(false);

            // set up with the list
            m_requestList.setSelectionListener(this);
            m_requestList.setProgressDisplay(this);

            m_requestList.SetExtendedStyle(LVS_EX_FULLROWSELECT       // Enable selecting a whole row when clicking anywhere in it
                | LVS_EX_HEADERDRAGDROP     // Enable moving the order of columns
                | LVS_EX_INFOTIP            //         // Enable tool tips when column text is too wide
                | LVS_EX_GRIDLINES );       // Show gridlines.
            // initialise buttons
            requestSelectionChanged(m_currentSelection, false);

            // display any queued errors
            UserMessages::getInstance().setMessageSuppression(false);

            FUNCTION_EXIT;
        }


        BOOL TisLogViewerDlg::OnInitDialog()
        {
            FUNCTION_ENTRY( "OnInitDialog" );

            CRect windowSize;
            GetWindowRect(&windowSize);
            TA_Base_Bus::ResizingProperties properties;
            properties.canMaximise = false;
            properties.maxHeight = windowSize.bottom - windowSize.top;
            properties.maxWidth = windowSize.right - windowSize.left;
            properties.minHeight = windowSize.bottom - windowSize.top;
            properties.minWidth = windowSize.right - windowSize.left;
            setResizingProperties(properties);

            TransActiveDialog::OnInitDialog();

            // Set the icon for this dialog.  The framework does this automatically
            //  when the application's main window is not a dialog
            SetIcon(m_hIcon, TRUE);            // Set big icon
            SetIcon(m_hIcon, FALSE);        // Set small icon

            GetDlgItem(IDC_VIEW_PID)->EnableWindow(FALSE); // TD14405


            //TD18095, jianghp, to fix the performance of showing manager application
			//ModifyWindowPos();


            FUNCTION_EXIT;

            return TRUE;  // return TRUE  unless you set the focus to a control
        }


        void TisLogViewerDlg::OnDestroy()
        {
            FUNCTION_ENTRY( "OnDestroy" );

            CDialog::OnDestroy();

            // deregister for rights changes
            RightsManager::getInstance().deregisterForRightsChanges(this);

            m_requestList.setSelectionListener(NULL);

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnViewPids()
        {
            FUNCTION_ENTRY( "OnViewPids" );

            // get selected display request
            IDisplayRequest* request = m_requestList.getLastSelectItem();

            // if its null, nothing was selected. This button shouldnt even be enabled
            TA_ASSERT (request != NULL, "GUI enabled buttons when a message wasnt selected.");

            // display the dialog
            // it needs to know the request type (ie TTIS or STIS)
            // and the destination PID string(s).
            if (request->getDestinationSystem() == "TTIS")
            {
                m_pidDisplayDialog.displayTtisDestinations( request->getTtisDestinationTrainId(),
                                                            request->getTtisDestinationPids() );
            }
            else if (request->getDestinationSystem() == "STIS")
            {
                m_pidDisplayDialog.displayStisDestinations( request->getStisDestinationStation(),
                                                            request->getStisDestinationLevels(),
                                                            request->getStisDestinationPids());
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Unknown destination system: %s", request->getDestinationSystem().c_str() ); //TD14337 yezh++
            }

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnReinstate()
        {
            FUNCTION_ENTRY( "OnReinstate" );

            // get selected display request
            std::vector<IDisplayRequest*> requests = m_requestList.getAllSelectedItems();

            // if its null, nothing was selected. This button shouldnt even be enabled
            TA_ASSERT (requests.size() != 0, "GUI enabled buttons when a message wasnt selected.");
            std::vector<IDisplayRequest*>::iterator iter = requests.begin();


            // Display Reinstate Display Request dialog.
            delete m_reinstateDisplayRequestDialog;

            m_reinstateDisplayRequestDialog = new ReinstateDisplayRequestDialog( requests,
                                                                                 m_requestList.getLastSelectItem(),
                                                                                 *m_displayRequestSender );
            m_reinstateDisplayRequestDialog->reinstateDisplayRequest();

            FUNCTION_EXIT;
        }


        LRESULT TisLogViewerDlg::onRightsChanged(WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "onRightsChanged" );

            // TES891 ~ backup current rights for later comparison against new rights
            const bool canDisplayTTISFreeText     = m_canDisplayTTISFreeText;
            const bool canDisplayTTISPredefined   = m_canDisplayTTISPredefined;
            const bool canDisplaySTISFreeText     = m_canDisplaySTISFreeText;
            const bool canDisplaySTISPredefined   = m_canDisplaySTISPredefined;

            // the rights have changed - re check them
            m_canDisplayTTISFreeText = RightsManager::getInstance().canDisplayTTISFreeText();
            m_canDisplayTTISPredefined = RightsManager::getInstance().canDisplayTTISPredefined();
            m_canDisplaySTISFreeText = RightsManager::getInstance().canDisplaySTISFreeText();
            m_canDisplaySTISPredefined = RightsManager::getInstance().canDisplaySTISPredefined();
            m_canSubmitRatis = RightsManager::getInstance().canSubmitRatis();

            // TES891 ~ added *if* statement and body
            // if the new rights differ from the previous ones then reload list
            // so that the list shows only items relevant to the new profile
            if ((canDisplayTTISFreeText     != m_canDisplayTTISFreeText)    ||
                (canDisplayTTISPredefined   != m_canDisplayTTISPredefined)  ||
                (canDisplaySTISFreeText     != m_canDisplaySTISFreeText)    ||
                (canDisplaySTISPredefined   != m_canDisplaySTISPredefined) )
            {
                requestSelectionChanged(None, false);
                m_requestList.reload(m_canDisplayTTISFreeText, m_canDisplayTTISPredefined,
                                     m_canDisplaySTISFreeText, m_canDisplaySTISPredefined);
            }

            // reenable buttons
            requestSelectionChanged(m_currentSelection, m_isReinstatable);

            FUNCTION_EXIT;
            return 0;
        }


        void TisLogViewerDlg::requestSelectionChanged(RequestType requestType, bool isReinstatable)
        {
            FUNCTION_ENTRY( "requestSelectionChanged" );

            m_currentSelection = requestType;
            m_isReinstatable = isReinstatable;
            CString messageText("N/A");

            IDisplayRequest* request = m_requestList.getLastSelectItem();

            if (isReinstatable)
            {

                bool enableReinstateBtn = false;
                bool isTtisRequest = false;   // TES891

                switch (requestType)
                {
                case IRequestSelectionListener::None:
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(FALSE);
                    enableReinstateBtn = false;
                    break;

                case IRequestSelectionListener::TtisFreeText:
                    isTtisRequest = true; // TES891
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                    enableReinstateBtn = m_canDisplayTTISFreeText;
                    TA_ASSERT(request != NULL,  "null request");
                    messageText = request->getAdhocMessageContent().c_str();
                    break;

                case IRequestSelectionListener::TtisPredefined:
                    isTtisRequest = true; // TES891
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                    enableReinstateBtn = m_canDisplayTTISPredefined;
                    TA_ASSERT(request != NULL,  "null request");
                    messageText = request->getAdhocMessageContent().c_str();
                    break;

                case IRequestSelectionListener::StisFreeText:
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                    enableReinstateBtn = m_canDisplaySTISFreeText;
                    TA_ASSERT(request != NULL,  "null request");
                    messageText = request->getAdhocMessageContent().c_str();
                    break;

                case IRequestSelectionListener::StisPredefined:
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                    enableReinstateBtn = m_canDisplaySTISPredefined;
                    TA_ASSERT(request != NULL,  "null request");
                    messageText = request->getAdhocMessageContent().c_str();
                    break;

                case IRequestSelectionListener::Ratis_In_Clear:
                case IRequestSelectionListener::Ratis_Out_Clear:
                case IRequestSelectionListener::Ratis_In_New:
                case IRequestSelectionListener::Ratis_Out_New:
                case IRequestSelectionListener::Ratis_In_Update:
                case IRequestSelectionListener::Ratis_Out_Update:
                    GetDlgItem(IDC_VIEW_PID)->EnableWindow(FALSE);
                    enableReinstateBtn = m_canSubmitRatis;
                    TA_ASSERT(request != NULL,  "null request");
                    messageText = request->getAdhocMessageContent().c_str();
                    break;
                }

                // check if the train is still at the station (i.e. area of control)
                if (isTtisRequest && enableReinstateBtn)
                {
                    TA_ASSERT(request != NULL,  "null request");
                }

                GetDlgItem(IDC_REINSTATE)->EnableWindow(enableReinstateBtn);
            }
            else
            {
                GetDlgItem(IDC_REINSTATE)->EnableWindow(false);
                //xufeng++ TD17502
                switch (requestType)
                {
                   case IRequestSelectionListener::None:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(FALSE);
                        break;

                    case IRequestSelectionListener::TtisFreeText:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                        TA_ASSERT(request != NULL,  "null request");
                        messageText = request->getAdhocMessageContent().c_str();
                        break;

                    case IRequestSelectionListener::TtisPredefined:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                        TA_ASSERT(request != NULL,  "null request");
                        messageText = request->getAdhocMessageContent().c_str();
                        break;

                    case IRequestSelectionListener::StisFreeText:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                        TA_ASSERT(request != NULL,  "null request");
                        messageText = request->getAdhocMessageContent().c_str();
                        break;

                    case IRequestSelectionListener::StisPredefined:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE);
                        TA_ASSERT(request != NULL,  "null request");
                        messageText = request->getAdhocMessageContent().c_str();
                        break;

                    case IRequestSelectionListener::Ratis_In_Clear:
                    case IRequestSelectionListener::Ratis_Out_Clear:
                    case IRequestSelectionListener::Ratis_In_New:
                    case IRequestSelectionListener::Ratis_Out_New:
                    case IRequestSelectionListener::Ratis_In_Update:
                    case IRequestSelectionListener::Ratis_Out_Update:
                        GetDlgItem(IDC_VIEW_PID)->EnableWindow(FALSE);
                        TA_ASSERT(request != NULL,  "null request");
                        messageText = request->getAdhocMessageContent().c_str();
                        break;
                }
                //++xufeng TD17502
            }
		//like++ CL-20858
            // Set the content of the message content window
            // to allow the operator to copy and paste into the STIS/TTIS Manager
			//TD17904 Liumeng
// 	  		static int strLength=0;
// 	  		strLength=messageText.Find(_T("."),0);
// 	 		CString temStr;
// 	  		while(strLength>0)
// 	  		{
// 	  			if(strLength<=87)
// 	  			   messageText.Insert(++strLength,"\r\n");
// 	  			else
// 	  			{
// 	                 strLength=87;
// 	                 messageText.Insert(strLength,"\r\n");
// 	  
// 	  			}
// 	  			strLength+=2;
// 	  			temStr=messageText.Mid(strLength);
// 	  			if(temStr.FindOneOf(_T(" "))==0)
// 	                     messageText.Delete(strLength);
// 	  			strLength=temStr.Find(_T("."),strLength);
// 	  		     
// 	  		}//TD-End;
			
			messageText.TrimLeft(); 
			int iPos = 0;

			while((iPos = messageText.Find(_T("."), iPos)) != -1)
			{
				messageText.Insert(++iPos, _T("\r\n"));
				iPos += 2;
				while(messageText.Find(_T(" "), iPos) == iPos)
					messageText.Delete(iPos);
				
			}
		//++like CL-20858
            m_messageContent.SetWindowText( messageText.GetLength() ? messageText:"N/A" );

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::showProgressBar( bool show,
                                               int min /* = 0 */,
                                               int max /* = 100 */,
                                               int step /* = 1 */,
                                               int pos /* = 0 */)
        {
            FUNCTION_ENTRY( "showProgressBar" );

            if (show && !m_progressBarVisible)
            {
                // set up the bar
                m_progressBar.SetRange(min, max);
                m_progressBar.SetStep(step);
                m_progressBar.SetPos(pos);

                // show it
                m_progressBar.ShowWindow(SW_SHOW);

                // resize the list above
                RECT progressRect;
                m_progressBar.GetClientRect(&progressRect);
                RECT listRect;
                m_requestList.GetWindowRect(&listRect);
                ScreenToClient(&listRect);
                listRect.bottom -= progressRect.bottom;
                m_requestList.MoveWindow(&listRect);

                m_progressBarVisible = true;
            }
            else if (!show && m_progressBarVisible)
            {
                // hide the bar
                m_progressBar.ShowWindow(SW_HIDE);

                // resize the list above
                // resize the list above
                RECT progressRect;
                m_progressBar.GetClientRect(&progressRect);
                RECT listRect;
                m_requestList.GetWindowRect(&listRect);
                ScreenToClient(&listRect);
                listRect.bottom += progressRect.bottom;
                m_requestList.MoveWindow(&listRect);

                m_progressBarVisible = false;
            }

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::stepProgressBar()
        {
            FUNCTION_ENTRY( "stepProgressBar" );

            // if the bar has been set up, increment it
            if (m_progressBar.IsWindowVisible())
            {
                m_progressBar.StepIt();
            }

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnFileExit()
        {
            FUNCTION_ENTRY( "OnFileExit" );

            //TD18095, jianghp, to fix the performance of showing manager application
            TransActiveDialog::DestroyWindow();

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnClose()
        {
            FUNCTION_ENTRY( "OnClose" );

            //TD18095, jianghp, to fix the performance of showing manager application
            TransActiveDialog::DestroyWindow();

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
        {
            FUNCTION_ENTRY( "OnKeyDown" );

            TransActiveDialog::OnKeyDown(nChar, nRepCnt, nFlags);

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnHelpAbout()
        {
            FUNCTION_ENTRY( "OnHelpAbout" );

            TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnHelp()
        {
            FUNCTION_ENTRY( "OnHelp" );

            TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnCloseButton()
        {
            FUNCTION_ENTRY( "OnCloseButton" );

            TransActiveDialog::OnClose();

            FUNCTION_EXIT;
        }


        void TisLogViewerDlg::OnClickRequestList(NMHDR* pNMHDR, LRESULT* pResult)
        {
            FUNCTION_ENTRY( "OnClickRequestList" );

            // TODO: Add your control notification handler code here
            // TD14405 - if an item is selected on the request list, allow to view
            // the destination PIDs
    //        if(m_requestList.GetSelectedCount() > 0 )
    //        {
    //            GetDlgItem(IDC_VIEW_PID)->EnableWindow(TRUE); // TD14405
    //         }
            //*pResult = 0;

            FUNCTION_EXIT;
        }
	/*

		//resize the window, refer to PAManager
		void TisLogViewerDlg::ModifyWindowPos()
		{
			int left,top,width,height;
			int screenWidth,currentScreen;
			int numScreens = 1;
			int x_pos;
			
			CPoint   pt;   
			GetCursorPos(&pt);   
			
			RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
				TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
				pt.x);
			x_pos = pt.x;
			screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			if(screenWidth >= MINVALUE)
				numScreens = NUMSCREENS;
			currentScreen = x_pos / (screenWidth / numScreens);
			left	= currentScreen * (screenWidth / numScreens)+1;
			width	= screenWidth / numScreens;
			top		= boundary.top;
			height	= boundary.bottom - boundary.top;
			
			this->MoveWindow(left,top,width,height,NULL);
		}
		*/

} // TA_App


