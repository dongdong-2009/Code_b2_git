// FrameHandler.cpp: implementation of the CFrameHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "FrameHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App 
{
	CFrameHandler::CFrameHandler()
	{

	}

	CFrameHandler::~CFrameHandler()
	{

	}


	std::vector <unsigned char> CFrameHandler::GetControlFrame (std::vector <unsigned char> vctrDataControl,int nTrainID, int nMessageType)
	{
		
		m_vctrFrameData.erase(m_vctrFrameData.begin(),m_vctrFrameData.end());
		
		int nType = 0;
		int nLen = 0;
		switch (nMessageType)
		{
			case BEGIN:
				nType = TA_IRS_Bus::TtdFrame::EMessageType::DownloadCtrl;
				nLen = 9;
				break;
			case REPEAT:
				nType = TA_IRS_Bus::TtdFrame::EMessageType::FrameRepition;
				nLen = vctrDataControl.size () + 8;
				break;
		}

		m_vctrFrameData.push_back(7);
		m_vctrFrameData.push_back(nType);
		m_vctrFrameData.push_back(nTrainID);
		m_vctrFrameData.push_back(nLen);
		m_vctrFrameData.push_back(0);
		m_vctrFrameData.push_back(10);

		for (int nCounter = 0; nCounter < vctrDataControl.size (); nCounter++)
		{
			//CString strFormat;

			//strFormat.Format("%d",vctrDataControl.at(nCounter));

			//::MessageBox (NULL,strFormat.GetBuffer(0),"Duplicated",MB_OK);
			m_vctrFrameData.push_back(vctrDataControl.at (nCounter));
		}
		
		unsigned short CRC = TA_IRS_Bus::FrameCheckSequence::calculate( m_vctrFrameData, m_vctrFrameData.size() );
        m_vctrFrameData.push_back( CRC % 0x100 );  // LSB of sum check
        m_vctrFrameData.push_back( CRC / 0x100 );  // MSB of sum check

		return m_vctrFrameData;
	}
}
