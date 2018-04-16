// MessageCreator.h: interface for the CMessageCreator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MESSAGECREATOR_H__67E3737E_DFA7_45D5_935F_A5174424A287__INCLUDED_)
#define AFX_MESSAGECREATOR_H__67E3737E_DFA7_45D5_935F_A5174424A287__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"

class CPDSMessageCreator  
{
	public:
		CPDSMessageCreator() {};
		virtual ~CPDSMessageCreator() {};
		static std::string GetNotificationMessage (long lCommand,std::string strModule,const long & lTrainID);

};

#endif // !defined(AFX_MESSAGECREATOR_H__67E3737E_DFA7_45D5_935F_A5174424A287__INCLUDED_)
