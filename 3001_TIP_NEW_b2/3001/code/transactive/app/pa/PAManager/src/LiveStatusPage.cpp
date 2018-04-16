/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/pa/PAManager/src/LiveStatusPage.cpp $
  * @author Jade Lee
  * @version $Revision: #5 $
  * Last modification: $DateTime: 2013/10/01 12:22:53 $
  * Last modified by: $Author: huangjian $
  * 
  * LiveStatusPage controls shared view functionality for live status pages (views)
  *
  */

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/LiveStatusPage.h"
#include "app/pa/PAManager/src/LiveStatusModel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


LiveStatusPage::LiveStatusPage( 
// song toan++
// TD 10316
    bool isOCC, 
// ++song toan
// TD 10316
    UINT nIDTemplate, CWnd* pParentWnd)
:
GenericStatusPage(
// song toan++
// TD 10316
    isOCC, 
// ++song toan
// TD 10316
    nIDTemplate, pParentWnd)
{
}


BEGIN_MESSAGE_MAP(LiveStatusPage, GenericStatusPage)
	//{{AFX_MSG_MAP(LiveStatusPage)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL LiveStatusPage::OnInitDialog() 
{
	GenericStatusPage::OnInitDialog();

    return TRUE;
}


void LiveStatusPage::attemptOperation(const EPAAgentOperation op)
{
    try
    {
        switch (op)
        {
        case RETRY_FAILURES:
            getLiveStatusModel().retryFailedBroadcast();
            break;
        case RETRY_ALL:
            getLiveStatusModel().retryAllBroadcast();
            break;
        default:
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unknown OP" );
			TA_THROW(TA_Base_Core::TransactiveException("Unknown OP"));
        }
    }
    catch ( const TA_Base_Bus::IPAAgentCorbaDef::PaErrorException& expWhat )
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::IPAAgentCorbaDef::PaErrorException", expWhat.strWhat.in() );
        switch ( expWhat.eErrorId )
        {
        case TA_Base_Bus::IPAAgentCorbaDef::eBroadcastIdNotFound:
            // Broadcast ID not found, somehow lost Live ID?
            PaErrorHandler::displayError(PaErrorHandler::BROADCAST_ID_NOT_FOUND);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eAccessDenied:
            PaErrorHandler::displayError(PaErrorHandler::INSUFFICIENT_RIGHTS);
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::eBadParameter:
            PaErrorHandler::displayError(PaErrorHandler::INVALID_REQUEST_PARAMS);
            break;
        default:
            break;
        }
    }
    catch (const CORBA::Exception&)
    {
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);        
    }
    catch (const TA_Base_Core::ObjectResolutionException&)
    {
        // PA Agent is down, resulting in operation being aborted
        PaErrorHandler::displayError(PaErrorHandler::PA_AGENT_FAIL);
    }
    catch (...)
    {
        PaErrorHandler::displayError(PaErrorHandler::UNSPECIFIED_ERROR);        
    }
}


LiveStatusModel& LiveStatusPage::getLiveStatusModel()
{ 
    return castPointerToReference<LiveStatusModel>(getStatusModelPointer()); 
}


void LiveStatusPage::OnDestroy()
{
    try
    {
        // Terminate broadcast before model deleted (in base class GenericStatusPage)
        getLiveStatusModel().terminateAndRemoveBroadcast(BO_LIVE_BROADCAST);
    }
    catch (const CORBA::Exception&)
    {
        // Ignore any errors - if broadcast already terminated
        // we will get exception, but no way of telling if still active
        // (for certain) at the time of calling terminateBroadcast
    }
    catch (const TA_Base_Core::TransactiveException&)
    {
        // Ignore any errors - if broadcast already terminated
        // we will get exception, but no way of telling if still active
        // (for certain) at the time of calling terminateBroadcast
    }

    // Note: If multiple ON_WM_DESTROY() maps exist, this function still only gets called once
    GenericStatusPage::OnDestroy();
}
