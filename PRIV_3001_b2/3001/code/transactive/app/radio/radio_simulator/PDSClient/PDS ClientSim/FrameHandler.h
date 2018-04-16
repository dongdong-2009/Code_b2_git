// FrameHandler.h: interface for the CFrameHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEHANDLER_H__A51D9332_582A_49C3_91A5_A0911F5CD269__INCLUDED_)
#define AFX_FRAMEHANDLER_H__A51D9332_582A_49C3_91A5_A0911F5CD269__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/trains/TrainProtocolLibrary/src/FrameCheckSequence.h"
//#include "bus/trains/TrainProtocolLibrary/src/TtdFrame.h"
#include "TtdFrame.h"

#include <vector>



namespace TA_IRS_App 
{
	class CFrameHandler  
	{
	public:
		CFrameHandler();
		virtual ~CFrameHandler();
		
		std::vector <unsigned char> GetControlFrame (std::vector <unsigned char> vctrDataControl,int nTrainID, int nMessageType);

	private:
		std::vector <unsigned char> m_vctrFrameData;
	};

}

#endif // !defined(AFX_FRAMEHANDLER_H__A51D9332_582A_49C3_91A5_A0911F5CD269__INCLUDED_)
