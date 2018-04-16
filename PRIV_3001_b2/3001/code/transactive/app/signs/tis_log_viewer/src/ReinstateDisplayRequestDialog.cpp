/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/ReinstateDisplayRequestDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// ReinstateDisplayRequestDialog.cpp : implementation file
//

#include "app/signs/tis_log_viewer/src/stdafx.h"
#include "app/signs/tis_log_viewer/src/ReinstateDisplayRequestDialog.h"
#include "app/signs/tis_log_viewer/src/TisLogViewer.h"
#include "app/signs/tis_log_viewer/src/UserMessages.h"

#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <iomanip>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ReinstateDisplayRequestDialog dialog

namespace TA_IRS_App
{

    ReinstateDisplayRequestDialog::ReinstateDisplayRequestDialog( std::vector<TA_IRS_Core::IDisplayRequest*> requests,
                                                                  TA_IRS_Core::IDisplayRequest* lastSelectedItem,
                                                                  DisplayRequestSender& displayRequestSender,
                                                                  CWnd* pParent /*=NULL*/)
	    : CDialog(ReinstateDisplayRequestDialog::IDD, pParent),
          m_requests( requests ),
          m_lastSelectedItem( lastSelectedItem ),
          m_displayRequestSender( displayRequestSender )
    {
	    //{{AFX_DATA_INIT(ReinstateDisplayRequestDialog)
	    //}}AFX_DATA_INIT
    }


    void ReinstateDisplayRequestDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ReinstateDisplayRequestDialog)
	    DDX_Control(pDX, IDC_CHECK1, m_adhocToPredefinedCheck);
	    DDX_Control(pDX, IDC_MESSAGE_EDIT, m_textEntry);
	    //}}AFX_DATA_MAP
    }

    void ReinstateDisplayRequestDialog::reinstateDisplayRequest()
    {
        LOG_FUNCTION_ENTRY( SourceInfo, "ReinstateDisplayRequestDialog::reinstateDisplayRequest");

        this->DoModal();
    
        LOG_FUNCTION_EXIT( SourceInfo,  "ReinstateDisplayRequestDialog::reinstateDisplayRequest");
    }


    BEGIN_MESSAGE_MAP(ReinstateDisplayRequestDialog, CDialog)
	    //{{AFX_MSG_MAP(ReinstateDisplayRequestDialog)
	    ON_BN_CLICKED(IDC_CHECK1, OnConvertToAdHoc)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ReinstateDisplayRequestDialog message handlers

    /**
     * When this is invoked, the operator wants to reinstate the message.
     * The only things this method will do before reinstating the request is:
     * 1. Change the message type to ADHOC if it is a PREDEFINED message.
     * 2. Reinsert the text just in case it has been altered.
     * Finally it will send the request.
     */
    void ReinstateDisplayRequestDialog::OnOK() 
    {
        LOG_FUNCTION_ENTRY( SourceInfo,  "ReinstateDisplayRequestDialog::OnOK");

        int ret = UserMessages::getInstance().askQuestion("Are you sure you wish to Reinstate the request...?");

        if (ret == IDYES)
	    {
	    
	        if (m_requests.size() == 0)
	        {
		        return;
	        }
	        else if(m_requests.size() == 1)
	        {
		        std::vector<TA_IRS_Core::IDisplayRequest*>::iterator iter = m_requests.begin();
		        CString message;
		        this->m_textEntry.GetWindowText( message );

		        if( TA_IRS_Core::IDisplayRequest::PREDEFINED == (*iter)->getMessageType() 
			        &&1 == CWnd::IsDlgButtonChecked(IDC_CHECK1))
		        {

			        TA_IRS_Core::IDisplayRequest* modifiedRequest =
				        TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();

			        // Set the appropriate fields
			        //xufeng++ 2006/11/08 TD17567
			        std::stringstream timeStamp;
			        CTime time = CTime::GetCurrentTime();
			        timeStamp << time.GetYear();
			        timeStamp << std::setw(2) << std::setfill('0') << time.GetMonth();
			        timeStamp << std::setw(2) << std::setfill('0') << time.GetDay();
			        timeStamp << std::setw(2) << std::setfill('0') << time.GetHour();
			        timeStamp << std::setw(2) << std::setfill('0') << time.GetMinute();
			        timeStamp << std::setw(2) << std::setfill('0') << time.GetSecond();
			        std::string newTimeStamp = timeStamp.str();
			        modifiedRequest->setTimestamp(TA_IRS_Core::DisplayRequestData::timestampToString(newTimeStamp).c_str());
			        //xufeng++ 2006/11/08 TD17567
			        modifiedRequest->setMessageType( TA_IRS_Core::IDisplayRequest::ADHOC );
			        modifiedRequest->setDestinationSystem( (*iter)->getDestinationSystem() );
			        modifiedRequest->setAdhocMessageContent( std::string( (LPCTSTR)message ));
			        modifiedRequest->setDisplayAttributes( (*iter)->getDisplayAttributes() );
			        modifiedRequest->setPlasmaAttributes( (*iter)->getPlasmaAttributes() );
			        modifiedRequest->setLEDAttributes( (*iter)->getLEDAttributes() );
			        modifiedRequest->setPriority( (*iter)->getPriority() );
			        modifiedRequest->setStartTime( (*iter)->getStartTime() );
			        modifiedRequest->setEndTime( (*iter)->getEndTime() );
			        modifiedRequest->setSubmittedBy( getOperatorNameFromSession( TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ) ) );

			        if ((*iter)->getDestinationSystem() == "TTIS")
                    {
				        modifiedRequest->setTtisDestinationTrainId( (*iter)->getTtisDestinationTrainId() );
				        modifiedRequest->setTtisDestinationPids( (*iter)->getTtisDestinationPids() );
			        }
			        else if ((*iter)->getDestinationSystem() == "STIS")
                    {
				        modifiedRequest->setStisDestinationLevels((*iter)->getStisDestinationLevels());
				        modifiedRequest->setStisDestinationPids((*iter)->getStisDestinationPids());
				        modifiedRequest->setStisDestinationStation((*iter)->getStisDestinationStation());
			        }

			        modifiedRequest->applyChanges();
    
			        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						         "Submitting request: %s", modifiedRequest->getAdhocMessageContent().c_str() ); //TD14337 yezh++

	                // Send the request
			        m_displayRequestSender.sendRequest(modifiedRequest);
			        delete modifiedRequest;
		        }
		        else if(TA_IRS_Core::IDisplayRequest::ADHOC == (*iter)->getMessageType())
		        {
			        (*iter)->setAdhocMessageContent( std::string( (LPCTSTR)message ));
			        m_displayRequestSender.sendRequest( (*iter) );
		        }
		        else
		        {
			        // Send the request
			        m_displayRequestSender.sendRequest( (*iter) );
		        }
	        }
	        else
	        {
		        std::vector<TA_IRS_Core::IDisplayRequest*>::iterator iter = m_requests.begin();
		        for (; iter != m_requests.end(); ++iter)
		        {
			        // Send the request
			        m_displayRequestSender.sendRequest( (*iter) );
		        }
	        }

            CDialog::OnOK();
        }
        LOG_FUNCTION_EXIT( SourceInfo,  "ReinstateDisplayRequestDialog::OnOK");
    }


    BOOL ReinstateDisplayRequestDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

	    //If there is no record selected, then return, do nothing
	    if (m_requests.size() <= 0)
	    {
		    return FALSE;
	    }
	    // if one record selected,  then check if we should disable the text box and check box.
	    else if(m_requests.size() == 1)
	    {
		    CString message;
		    if (m_lastSelectedItem != NULL) 
		    {
			    message = m_lastSelectedItem->getAdhocMessageContent().c_str();
			    this->m_textEntry.SetWindowText( message );
		    }
		    else
		    {
			    return FALSE;
		    }
		    // Disable the checkbox if it is NOT a predefined message
		    if( TA_IRS_Core::IDisplayRequest::PREDEFINED == m_lastSelectedItem->getMessageType() )
		    {
			    this->m_textEntry.EnableWindow( false );
			    this->m_adhocToPredefinedCheck.EnableWindow( true );
		    }
		    // Otherwise disable the checkbox
		    else if(TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR == m_lastSelectedItem->getMessageType())
		    {
			    this->m_textEntry.EnableWindow( false );
			    this->m_adhocToPredefinedCheck.EnableWindow( false );
		    }
		    else
		    {
			    this->m_textEntry.EnableWindow( true );
			    this->m_adhocToPredefinedCheck.EnableWindow( false );
		    }
	    }
	    // If more than one record selected ,then disable text box and check box.
	    else
	    {
		    CString message;
		    if (m_lastSelectedItem != NULL) 
		    {
			    message = m_lastSelectedItem->getAdhocMessageContent().c_str();
			    this->m_textEntry.SetWindowText( message );
		    }
		    else
		    {
			    return FALSE;
		    }

		    this->m_textEntry.EnableWindow( false );
		    this->m_adhocToPredefinedCheck.EnableWindow( false );

	    }
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    // When the user checks the check box, we want to enable the text field 
    // and disable the textfield and reinstate the message if the checkbox
    // is unchecked.
    //
    // This checkbox is only enabled if the message is a predefined message
    void ReinstateDisplayRequestDialog::OnConvertToAdHoc() 
    {
	    if( 1 == CWnd::IsDlgButtonChecked(IDC_CHECK1) )
        {
            this->m_textEntry.EnableWindow( true );
        }
        else
        {
	        CString message( m_lastSelectedItem->getAdhocMessageContent().c_str() );
            this->m_textEntry.SetWindowText( message );
            this->m_textEntry.EnableWindow( false );
        }
    }

    std::string ReinstateDisplayRequestDialog::getOperatorNameFromSession( const std::string& currentSessionID )
    {
	    FUNCTION_ENTRY("getOperatorNameFromSession");

	    std::string returnName("Not available");

	    TA_Base_Core::ISession* operatorSession = NULL;

	    try
	    {
		    operatorSession = TA_Base_Core::SessionAccessFactory::getInstance().getSession( currentSessionID );

		    if( NULL != operatorSession )
		    {
			    returnName = operatorSession->getOperatorName();

			    delete operatorSession;
		    }
	    }
	    catch( const TA_Base_Core::DatabaseException& dbe )
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());
	    }
	    catch( const TA_Base_Core::DataException& de )
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());
	    }

	    return returnName;

	    FUNCTION_EXIT;
    }

} // namespace TA_IRS_App

