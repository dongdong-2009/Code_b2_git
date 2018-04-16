// RadioPDSMethodSendData.cpp: implementation of the CRadioPDSMethodSendData class.
//
//////////////////////////////////////////////////////////////////////
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodSendData.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{
	CRadioPDSMethodSendData::~CRadioPDSMethodSendData()
	{
		FUNCTION_ENTRY ("~CRadioPDSMethodSendData");
		FUNCTION_EXIT;
	}

	CRadioPDSMethodSendData::CRadioPDSMethodSendData(std::vector <unsigned char> vctrLibData,
													 std::string strClientName,
													 SRadioBufferType radioControlFrame) 
													 :
													 CRadioPDSMethod (strClientName),
													 m_lEventType (radioControlFrame.ucMsgID)
													
																	  
	{
		FUNCTION_ENTRY ("CRadioPDSMethodSendData");
		frameLibraryData (vctrLibData,radioControlFrame);
		FUNCTION_EXIT;
	}


	void CRadioPDSMethodSendData::setCurrentFrameIndex (const unsigned int & unIndex)
	{
		FUNCTION_ENTRY ("setCurrentFrameIndex");
		clearCurrentFrame();
		FUNCTION_EXIT;
		setFrameMessage(m_frameListToBeSend.at (unIndex).frameData);
	}

	void CRadioPDSMethodSendData::frameLibraryData(std::vector <unsigned char> vctrLibData,
												   SRadioBufferType radioControlFrame)
	{
		
		FUNCTION_ENTRY ("frameLibraryData");
		unsigned int unMessagetype = (EMsgType)radioControlFrame.ucMsgID;

		if ((EMsgType)radioControlFrame.ucMsgID == TA_IRS_App::TtdFrame::DownloadCtrl)
		{
			unMessagetype = TA_IRS_App::TtdFrame::TTISLibData;
		}

		m_frameListBuff = TTDFrameType::frameData(vctrLibData,
												  radioControlFrame.ucTrainID,
												  (EMsgType)unMessagetype,
												  radioControlFrame.ucOrigin);
		
		
	
		m_frameListToBeSend = m_frameListBuff;
	

		m_nframeContainerSize = m_frameListToBeSend.size ();

		FUNCTION_EXIT;
	}

	int CRadioPDSMethodSendData::getFrameContainerSize()
	{
		FUNCTION_ENTRY ("getFrameContainerSize");
		FUNCTION_EXIT;
		return m_nframeContainerSize;
	}

	long CRadioPDSMethodSendData::getEventType ()
	{
		FUNCTION_ENTRY ("getEventType");
		FUNCTION_EXIT;
		return m_lEventType;
	}

	long CRadioPDSMethodSendData::SetupFrameForResend(SRadioBufferType radioControlFrame)
	{
		
		FUNCTION_ENTRY ("SetupFrameForResend");
		
		m_frameListToBeSend.clear();
		m_lEventType = radioControlFrame.ucMsgID;

		m_radioPDBuffer = radioControlFrame;
		
		// message already filtered, already remove the protocol hence data wil be the only entry
		// its starts with 2 since the frame repetition starts as index 2
		// index 0 is Total frame number
		// index 1 is 0x00
		// subrtracted by 2 to omit 2 bytes CRC

		for (int nCounter = 2; nCounter < m_radioPDBuffer.radioPDSMessage.size() - 2; nCounter++)
		{
			unsigned char ucIndexToResend = m_radioPDBuffer.radioPDSMessage.at(nCounter);
			
			if (ucIndexToResend > m_frameListBuff.size() || ucIndexToResend == 0)
			{
				LOG_GENERIC (SourceInfo,
					TA_Base_Core::DebugUtil::DebugInfo,
					"Frame # %d, not valid frame",
					ucIndexToResend
					);
				continue;
			}
			// start at 0 hence will be subtracted by one
			m_frameListToBeSend.push_back(m_frameListBuff.at (ucIndexToResend - 1));
		}
		
		m_nframeContainerSize = m_frameListToBeSend.size ();
		
		FUNCTION_EXIT;
		return m_nframeContainerSize;
	}


}
