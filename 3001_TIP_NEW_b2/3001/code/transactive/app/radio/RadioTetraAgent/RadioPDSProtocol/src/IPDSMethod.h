// IPDSMethod.h: interface for the IPDSMethod class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPDSMETHOD_H__7E414888_B518_4F20_B7E8_F11E7C80ED34__INCLUDED_)
#define AFX_IPDSMETHOD_H__7E414888_B518_4F20_B7E8_F11E7C80ED34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

namespace TA_IRS_App
{
	class IPDSMethod  
	{
	public:
		virtual std::vector <unsigned char> getFrameMessage () = 0;
		virtual std::string getClientName () = 0;
	};
}
#endif // !defined(AFX_IPDSMETHOD_H__7E414888_B518_4F20_B7E8_F11E7C80ED34__INCLUDED_)
