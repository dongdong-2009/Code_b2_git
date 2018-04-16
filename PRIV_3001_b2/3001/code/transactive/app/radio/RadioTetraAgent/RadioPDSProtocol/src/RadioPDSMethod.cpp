// RadioPDSMethod.cpp: implementation of the CRadioPDSMethod class.
//
//////////////////////////////////////////////////////////////////////
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethod.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{

	CRadioPDSMethod::~CRadioPDSMethod()
	{
		m_vctrFrameMessage.erase(m_vctrFrameMessage.begin(),m_vctrFrameMessage.end());
	}


	CRadioPDSMethod::CRadioPDSMethod (std::string strClientName) : m_strClientName (strClientName)
	{
		m_vctrFrameMessage.erase(m_vctrFrameMessage.begin(),m_vctrFrameMessage.end());
	}

	std::vector <unsigned char> CRadioPDSMethod::getFrameMessage ()
	{
		return m_vctrFrameMessage;
	}

	std::string CRadioPDSMethod::getClientName ()
	{
		return m_strClientName;
	}

	void CRadioPDSMethod::setFrameMessage ( std::vector <unsigned char> vctrCurrentFrame)
	{
		m_vctrFrameMessage = vctrCurrentFrame;
	}

	void CRadioPDSMethod::clearCurrentFrame ()
	{
		m_vctrFrameMessage.clear ();
	}
}