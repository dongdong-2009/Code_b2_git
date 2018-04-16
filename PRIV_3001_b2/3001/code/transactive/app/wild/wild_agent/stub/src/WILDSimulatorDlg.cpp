/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/wild/wild_agent/stub/src/WILDSimulatorDlg.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This is a test App which is used to simulate ATS and PMS message updates to the WILD
  * Agent. All that is required is the WILD and PMS Agent to be running to test.
  */

#include "stdafx.h"
#include "WILDSimulator.h"
#include "WILDSimulatorDlg.h"
#include <time.h>
#include <math.h>
#include "core/data_access_interface/entity_access/src/DataPointAccessFactory.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "bus\scada\DataPointCorbaDef_Impl\src\DataPoint.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"
#include "bus\scada\DataPointCorbaDef_Impl\src\DataPointFactory.h"
// Nous commented
//#include "bus/ats/AtsAgentCorbaDef/src/IAtsWildCorbaDef.h"
// Nous commented ends

// Nous changes
#include "bus/ats/ats_agent/IDL/src/IAtsWildCorbaDef.h"
// Nous changes ends


#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::TransActiveDialog;

/////////////////////////////////////////////////////////////////////////////
// CWILDSimulatorDlg dialog

CWILDSimulatorDlg::CWILDSimulatorDlg(TA_Base_Bus::IGUIAccess* genericGUICallback)
	: TransActiveDialog(*genericGUICallback, CWILDSimulatorDlg::IDD, NULL),
	m_commsMessageSender(NULL)

{

	// If necessary this code can go in OnInitDialog before the call to TransActiveDialog::OnInitDialog()
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minWidth = -1;
    properties.minHeight = -1;
    setResizingProperties(properties);

	m_commsMessageSender =
		TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

}

void CWILDSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWILDSimulatorDlg)
	DDX_Control(pDX, IDC_DATEPICKER, m_datePicker);
	DDX_Control(pDX, IDC_TIMEPICKER, m_timePicker);
	DDX_Control(pDX, IDC_ATS_DATEPICKER, m_atsDatePicker);
	DDX_Control(pDX, IDC_ATS_TIMEPICKER, m_atsTimePicker);
	DDX_Control(pDX, IDC_CHECK_A1, m_checkA1);
	DDX_Control(pDX, IDC_CHECK_B1, m_checkB1);
	DDX_Control(pDX, IDC_CHECK_A2, m_checkA2);
	DDX_Control(pDX, IDC_CHECK_B2, m_checkB2);
	DDX_Control(pDX, IDC_CHECK_A3, m_checkA3);
	DDX_Control(pDX, IDC_CHECK_B3, m_checkB3);
	DDX_Control(pDX, IDC_CHECK_A4, m_checkA4);
	DDX_Control(pDX, IDC_CHECK_B4, m_checkB4);
	DDX_Control(pDX, IDC_CHECK_A5, m_checkA5);
	DDX_Control(pDX, IDC_CHECK_B5, m_checkB5);
	DDX_Control(pDX, IDC_CHECK_A6, m_checkA6);
	DDX_Control(pDX, IDC_CHECK_B6, m_checkB6);

	DDX_Control(pDX, IDC_CHECK_A7, m_checkA7);
	DDX_Control(pDX, IDC_CHECK_B7, m_checkB7);
	DDX_Control(pDX, IDC_CHECK_A8, m_checkA8);
	DDX_Control(pDX, IDC_CHECK_B8, m_checkB8);
	DDX_Control(pDX, IDC_CHECK_A9, m_checkA9);
	DDX_Control(pDX, IDC_CHECK_B9, m_checkB9);
	DDX_Control(pDX, IDC_CHECK_A10, m_checkA10);
	DDX_Control(pDX, IDC_CHECK_B10, m_checkB10);
	DDX_Control(pDX, IDC_CHECK_A11, m_checkA11);
	DDX_Control(pDX, IDC_CHECK_B11, m_checkB11);
	DDX_Control(pDX, IDC_CHECK_A12, m_checkA12);
	DDX_Control(pDX, IDC_CHECK_B12, m_checkB12);
	DDX_Control(pDX, IDC_ATC_CAR_NUMBER, m_ATCCarNumber);
	DDX_Control(pDX, IDC_PV_NUMBER, m_PVNumber);

	DDX_Control(pDX, IDC_CHECK_CRITICAL_ALARM, m_critCheck);
	DDX_Control(pDX, IDC_SEND_WILD_UPDATE, m_sendWILDUpdateButton);
	DDX_Control(pDX, IDC_ATS_BUTTON, m_atsButton);
	DDX_Control(pDX, IDC_RESET_WILD, m_resetButton);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWILDSimulatorDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(CWILDSimulatorDlg)
	ON_BN_CLICKED(IDC_SEND_WILD_UPDATE, OnSendWILDUpdateButton)
	ON_BN_CLICKED(IDC_ATS_BUTTON, OnATSUpdateButton)
	ON_BN_CLICKED(IDC_RESET_WILD, OnResetWILDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWILDSimulatorDlg message handlers
using namespace TA_Base_Core;
using namespace TA_Base_Bus;

BOOL CWILDSimulatorDlg::OnInitDialog()
{
	TransActiveDialog::OnInitDialog();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

int CWILDSimulatorDlg::binToDec(const std::string& binary) 
{ 
	LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "binToDec");

	int i = 0; 
	int j = 0;
	int k = 0;
	int z = 0;
	int x = strlen(binary.c_str()); j = atoi(binary.c_str()); 
	for (i = 0; i < x; i++) 
	{ 
		k = j % 10; z += static_cast<int>(k * (pow (2, i))); j = j/10; 
	} 

	LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "binToDec");
	return z; 
} 

unsigned int CWILDSimulatorDlg::decToBin(unsigned char decimal)
{
	LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "decToBin");
	
	// Special case when decimal is 1 or 0..
	if(decimal == 1 || decimal == 0)
	{
		return decimal ? 1 : 0;
	}

	char str[100]; 
	char result[100]; 
	unsigned int i = 0; 
	unsigned int k = 0; 
	do 
	{ 
		str[i] = static_cast<char>((decimal % 2) + 48); decimal /= 2; i++; 
	} 
	while (decimal != 1); 

	str[i] = '1'; str[i+1]='\0'; i = strlen(str); 
	for (k = 0; i != 0; k++, i--) 
	{ 
		result[k] = str[i - 1]; 
	} 
	result[k]='\0'; 
	k = atoi (result); 

	LOG( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "decToBin");
	return k; 
}

void CWILDSimulatorDlg::OnSendWILDUpdateButton()
{
	try
	{
		// Initial set up.
		DataPoint* theDataPoint = 0;
		TA_Base_Core::DataPointEntityData* dataPointEntityData = 0;
		std::vector<DataPointEntityData*> matchingDataPoints;
			
		// cast the EntityData to DataPointEntityData
		DataPointAccessFactory::getDummyInstance()->getAllDataPointEntitiesWithNameLikeToken("%WILD.TRS_SEND.rtiiWILD-WILDDetails",matchingDataPoints);
		std::auto_ptr<TA_Base_Bus::DataPointFactory> m_dataPointFactory(new TA_Base_Bus::DataPointFactory(NULL));
		int sizeOf =  matchingDataPoints.size();
		dataPointEntityData = dynamic_cast< TA_Base_Core::DataPointEntityData * > ( matchingDataPoints[0] );
		theDataPoint = m_dataPointFactory->createDataPoint( dataPointEntityData );

		theDataPoint->setToControlMode();

		std::string buffer("");
		m_critCheck.GetCheck();
		buffer += static_cast<unsigned char>(0);
		std::ostringstream binaryStr;

		if(m_checkB3.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_checkA3.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_checkB2.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_checkA2.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_checkB1.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_checkA1.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		if(m_critCheck.GetCheck())
		{
			binaryStr << 1;
		}
		else
		{
			binaryStr << 0;
		}

		binaryStr << 00;

		int firstWORD = binToDec(binaryStr.str());
		buffer += static_cast<unsigned char>(firstWORD);


		std::ostringstream firstBinaryStr;

		if(m_checkB7.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkA7.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}		
		if(m_checkB6.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkA6.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkB5.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkA5.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkB4.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}
		if(m_checkA4.GetCheck())
		{
			firstBinaryStr << 1;
		}
		else
		{
			firstBinaryStr << 0;
		}

		int secondFirstWord = binToDec(firstBinaryStr.str());
		buffer += static_cast<unsigned char>(secondFirstWord);

				std::ostringstream secondBinaryStr;

		if(m_checkB11.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkA11.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}		
		if(m_checkB10.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkA10.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkB9.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkA9.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkB8.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}
		if(m_checkA8.GetCheck())
		{
			secondBinaryStr << 1;
		}
		else
		{
			secondBinaryStr << 0;
		}

		int secondSecondWord = binToDec(secondBinaryStr.str());
		buffer += static_cast<unsigned char>(secondSecondWord);

		std::ostringstream secondWordBinaryStr;

		secondWordBinaryStr << 000000;

		if(m_checkB12.GetCheck())
		{
			secondWordBinaryStr << 1;
		}
		else
		{
			secondWordBinaryStr << 0;
		}
		if(m_checkA12.GetCheck())
		{
			secondWordBinaryStr << 1;
		}
		else
		{
			secondWordBinaryStr << 0;
		}

		int thirdSecondWord = binToDec(secondWordBinaryStr.str());
		buffer += static_cast<unsigned char>(thirdSecondWord);
		buffer += static_cast<unsigned char>(0); // Stuff byte

		CTime date;
		m_datePicker.GetTime(date);

		CTime timePick;
		m_timePicker.GetTime(timePick);
		
		int year = date.GetYear();
		int month = date.GetMonth();
		int day = date.GetDay();
		int hour = timePick.GetHour();
		int min = timePick.GetMinute();
		int secs = timePick.GetSecond();

		std::ostringstream yearStr;
		yearStr << year;

		std::string LSB_YEAR = (yearStr.str()).substr(2);
		std::string MSB_YEAR = (yearStr.str()).erase(2);

		buffer += static_cast<unsigned char>(hexToDec(atoi(LSB_YEAR.c_str()))); // 6
		buffer += static_cast<unsigned char>(hexToDec(atoi(MSB_YEAR.c_str()))); // 7

		buffer += static_cast<unsigned char>(0); // stuff byte  // 8
		buffer += static_cast<unsigned char>(hexToDec(month));  // 9
		
		buffer += static_cast<unsigned char>(0); // stuff byte. //10 
		buffer += static_cast<unsigned char>(hexToDec(day));    //11

		buffer += static_cast<unsigned char>(0); // stuff byte  //12
		buffer += static_cast<unsigned char>(hexToDec(hour));   //13
		
		buffer += static_cast<unsigned char>(0); // stuff byte  //14
		buffer += static_cast<unsigned char>(hexToDec(min));    //15
		
		buffer += static_cast<unsigned char>(0); // stuff byte  //16 
		buffer += static_cast<unsigned char>(hexToDec(secs));   //17

		DataPointValue dpValue;
		dpValue.setString(buffer);

		DataPointState dpState( dpValue, time(NULL), QUALITY_GOOD_NO_SPECIFIC_REASON );
		theDataPoint->updateFieldState(dpState);
	}
	catch(...)
	{
		// Do nothing.
		AfxMessageBox("Unknown exception caught. Operation unsucessful.");
	}


}


void CWILDSimulatorDlg::OnATSUpdateButton()
{
	CString pvNumber;
	m_PVNumber.GetWindowText(pvNumber);

	CString atcCarNumber;
	m_ATCCarNumber.GetWindowText(atcCarNumber);

	CTime date;
	m_atsDatePicker.GetTime(date);
	CTime time;
	m_atsTimePicker.GetTime(time);

	struct tm when;

	when.tm_year= date.GetYear() - 1900;
	when.tm_mon	= date.GetMonth() - 1;
	when.tm_mday= date.GetDay();
	when.tm_hour= time.GetHour();
	when.tm_min	= time.GetMinute();
	when.tm_sec	= time.GetSecond();

	time_t timetTimeStamp = mktime(&when);

	TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef m_WILDMessage;

	m_WILDMessage.timeStamp = timetTimeStamp;
	m_WILDMessage.physicalTrainNumber = atol(pvNumber);
	m_WILDMessage.activeCabNumber = atol(atcCarNumber);
	
	TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* wildMessage 
			= new TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef(m_WILDMessage);

	CORBA::Any data;
	data <<= wildMessage;

	std::auto_ptr<IEntityData> atsAgent(TA_Base_Core::EntityAccessFactory::getInstance().getEntity("OccAtsAgent"));
	m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::WILDTrainUpdate,
												atsAgent->getKey(),
												data,
												0,
												atsAgent->getLocation());

}


void CWILDSimulatorDlg::OnResetWILDButton()
{
	m_checkA1.SetCheck(0);
	m_checkB1.SetCheck(0);
	m_checkA2.SetCheck(0);
	m_checkB2.SetCheck(0);
	m_checkA3.SetCheck(0);
	m_checkB3.SetCheck(0);
	m_checkA4.SetCheck(0);
	m_checkB4.SetCheck(0);
	m_checkA5.SetCheck(0);
	m_checkB5.SetCheck(0);
	m_checkA6.SetCheck(0);
	m_checkB6.SetCheck(0);
	m_checkA7.SetCheck(0);
	m_checkB7.SetCheck(0);
	m_checkA8.SetCheck(0);
	m_checkB8.SetCheck(0);
	m_checkA9.SetCheck(0);
	m_checkB9.SetCheck(0);
	m_checkA10.SetCheck(0);
	m_checkB10.SetCheck(0);
	m_checkA11.SetCheck(0);
	m_checkB11.SetCheck(0);
	m_checkA12.SetCheck(0);
	m_checkB12.SetCheck(0);
	m_critCheck.SetCheck(0);

	CTime time;
	m_datePicker.SetTime(&time.GetCurrentTime());
	m_timePicker.SetTime(&time.GetCurrentTime());
	m_atsDatePicker.SetTime(&time.GetCurrentTime());
	m_atsTimePicker.SetTime(&time.GetCurrentTime());
}

unsigned long CWILDSimulatorDlg::hexToDec(unsigned char hexValue)
{

	std::ostringstream tmp;
	
	if(hexValue < 10)
	{
		unsigned int LSB_Binary = decToBin(hexValue);
		tmp << LSB_Binary;


	}
	else
	{
		int LSB = hexValue%10;
		int MSB = hexValue/10;
		unsigned int LSB_Binary = decToBin(LSB);

		std::stringstream ss;
		ss << LSB_Binary;
		std::string binaryTime(ss.str());
		unsigned int numberOfBitsToAdd = 4 - binaryTime.length();

		std::string prefixBits("");
		if (numberOfBitsToAdd > 0)
		{
			for(unsigned int i=0; i<numberOfBitsToAdd; i++)
			{
				prefixBits += "0";
			}
		}
		
		prefixBits+=binaryTime;

		unsigned int MSB_Binary = decToBin(MSB);

		tmp << MSB_Binary;
		tmp << prefixBits;
	}

	return binToDec(tmp.str());
}