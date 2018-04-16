// RadioPDSMethodSendData.h: interface for the CRadioPDSMethodSendData class.
//
//////////////////////////////////////////////////////////////////////


/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   $RadioPDSMethodSendData$
* @author:  Dennis
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* RadioPDSMethodSendData module wrapped data coming from the client. 
* The wrapping is specific for FrameRepeat when client send begin event
* 
*/




#if !defined(AFX_RADIOPDSMETHODSENDDATA_H__C1B3B2D9_6DB9_4CE1_B6FF_23B65D601CA9__INCLUDED_)
#define AFX_RADIOPDSMETHODSENDDATA_H__C1B3B2D9_6DB9_4CE1_B6FF_23B65D601CA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethod.h"

#include <vector>

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/FrameCheckSequence.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"

namespace TA_IRS_App
{
	class CRadioPDSMethodSendData : public CRadioPDSMethod  
	{

			typedef TA_IRS_App::SRadioPDSBuffer					SRadioBufferType;
			typedef TA_IRS_App::TtisTypes::TtisFrameList		FrameList;
			typedef	TA_IRS_App::TtdFrame					    TTDFrameType;
			typedef TA_IRS_App::TtdFrame::EMessageType			EMsgType;

		public:
			
			int getFrameContainerSize();
			long getEventType ();
			CRadioPDSMethodSendData(std::vector <unsigned char> vctrLibData,
									std::string strClientName,
									SRadioBufferType radioControlFrame);

			virtual ~CRadioPDSMethodSendData();
			void setCurrentFrameIndex (const unsigned int & unIndex);
			long SetupFrameForResend(SRadioBufferType radioControlFrame);

		private:
			
			void frameLibraryData(std::vector <unsigned char> vctrLibData,
								  SRadioBufferType radioControlFrame);
			
			FrameList					m_frameListBuff;
			FrameList					m_frameListToBeSend;
			std::vector <unsigned char> m_vctrFrameCurrent;
			std::vector <unsigned char> m_vctrDataLib;
			SRadioBufferType			m_radioPDBuffer;
			int							m_nframeContainerSize;
			long						m_lEventType;
		};
}
#endif // !defined(AFX_RADIOPDSMETHODSENDDATA_H__C1B3B2D9_6DB9_4CE1_B6FF_23B65D601CA9__INCLUDED_)
