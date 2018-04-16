// RadioPDSMethodCancel.cpp: implementation of the CRadioPDSMethodCancel class.
//
//////////////////////////////////////////////////////////////////////

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethodCancel.h"
#include "core/utilities/src/DebugUtil.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace TA_IRS_App
{

	CRadioPDSMethodCancel::CRadioPDSMethodCancel(std::string strClientName,TA_IRS_App::SRadioPDSBuffer radioBuffer)
	:	CRadioPDSMethod (strClientName)
	{
		FUNCTION_ENTRY ("CRadioPDSMethodCancel");
		frameLibraryData (radioBuffer.ucTrainID, radioBuffer.ucOrigin);
		FUNCTION_EXIT;
	}

	CRadioPDSMethodCancel::~CRadioPDSMethodCancel()
	{
		FUNCTION_ENTRY ("~CRadioPDSMethodCancel");
		FUNCTION_EXIT;
	}

	void CRadioPDSMethodCancel::frameLibraryData(unsigned int trainID, unsigned int Origin)
	{
		FUNCTION_ENTRY ("frameLibraryData");

		std::vector <unsigned char> vctrDataLib;
		vctrDataLib.push_back(3);
		
		m_frameListToBeSend = TTDFrameType::frameData(vctrDataLib,
													  trainID,
													  TA_IRS_App::TtdFrame::DownloadCtrl,
													  Origin);
		FUNCTION_EXIT;

	}

	int CRadioPDSMethodCancel::getFrameContainerSize()
	{
		FUNCTION_ENTRY ("getFrameContainerSize");
		FUNCTION_EXIT;
		return m_frameListToBeSend.size();
	}

	void CRadioPDSMethodCancel::setCurrentFrameIndex ()
	{
		FUNCTION_ENTRY ("setCurrentFrameIndex");
		clearCurrentFrame();
		setFrameMessage(m_frameListToBeSend.at (0).frameData);
		FUNCTION_EXIT;
	}

}