// RadioPDSMethodCancel.h: interface for the CRadioPDSMethodCancel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RADIOPDSMETHODCANCEL_H__9C61F110_46B6_4EFA_BD15_9FDEDBE84FE3__INCLUDED_)
#define AFX_RADIOPDSMETHODCANCEL_H__9C61F110_46B6_4EFA_BD15_9FDEDBE84FE3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSMethod.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSEvent.h"

namespace TA_IRS_App
{
	class CRadioPDSMethodCancel  : public CRadioPDSMethod
	{
		typedef TA_IRS_App::CommonTypes::SRadioPDSBuffer   SRadioBufferType;
		typedef TA_IRS_App::TtisTypes::TtisFrameList		FrameList;
		typedef	TA_IRS_App::TtdFrame					    TTDFrameType;
		typedef TA_IRS_App::TtdFrame::EMessageType			EMsgType;

		public:
			CRadioPDSMethodCancel(std::string strClientName,TA_IRS_App::SRadioPDSBuffer radioBuffer);
			virtual ~CRadioPDSMethodCancel();
			int getFrameContainerSize();
			void setCurrentFrameIndex ();
		private:
			
			void frameLibraryData(unsigned int trainID, unsigned int Origin);
			FrameList					m_frameListToBeSend;
	};
}
#endif // !defined(AFX_RADIOPDSMETHODCANCEL_H__9C61F110_46B6_4EFA_BD15_9FDEDBE84FE3__INCLUDED_)
