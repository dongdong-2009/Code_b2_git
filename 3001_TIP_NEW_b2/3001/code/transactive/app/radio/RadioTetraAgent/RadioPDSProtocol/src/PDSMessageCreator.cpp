// MessageCreator.cpp: implementation of the CMessageCreator class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/PDSMessageCreator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

std::string CPDSMessageCreator::GetNotificationMessage (long lCommand,std::string strModule,const long & lTrainID)
{
	std::string strRet;

	std::string strCommand;

	switch (lCommand)
	{
		case TA_IRS_App::TtisTypes::PDSLibDownloadMakeDataCall:
			strCommand = "MakeDataCall";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadBegin:
			strCommand = "Begin Transfer";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadCancelled:
			strCommand = "User Cancelled";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadResend:
			strCommand = "Resend Packets";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadCallDisconnection:
			strCommand = "Transfer DisConnect";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireFailed:
			strCommand = "Timer expire - No event recieve";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadTimerExpireSuccess:
			strCommand = "Timer expire - Download success";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadTimerEventWait:
			strCommand = "Timer start - Data recieved, waiting for begin event (40)";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadTimerSuccessWait:
			strCommand = "Timer start - Sending done, Waiting for resend event (41)";
			break;
		case TA_IRS_App::TtisTypes::PDSLibDownloadTimerStopEventRecv:
			strCommand = "Timer stop - Event recieved";
			break;
		default:;
	}
	
	char * pszData = new char [1023];
	memset(pszData, 0, 1023);
	sprintf (pszData,"%d",lTrainID);
	std::string strTrainID = pszData;

	delete [] pszData;
	pszData = NULL;

	strRet = strModule + " - " + strCommand + " TrainID - " + strTrainID;
	
	return strRet;

}