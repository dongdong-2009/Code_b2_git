// RadioPDSMethod.h: interface for the CRadioPDSMethod class.
//
//////////////////////////////////////////////////////////////////////

/* The source code in this file is the property of 
* ComBuilder Pte. Ltd and is not for redistribution
* in any form.
* Source:   $Source$
* @author:  Dennis
* @version: $Revision$
*
* Last modification: $Date$
* Last modified by:  $Author$
* 
* Generic wrapper of all outgoing data for PDS modules.
*/

#if !defined(AFX_RADIOPDSMETHOD_H__4035A676_E56C_407D_A383_0D7019CE7E7F__INCLUDED_)
#define AFX_RADIOPDSMETHOD_H__4035A676_E56C_407D_A383_0D7019CE7E7F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/FrameCheckSequence.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/TtdFrame.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/IPDSMethod.h"

#include <string>

namespace TA_IRS_App
{

class CRadioPDSMethod  : public IPDSMethod
{
public:
	CRadioPDSMethod () {}
	CRadioPDSMethod (std::string strClientName);

	virtual ~CRadioPDSMethod();
	virtual std::vector <unsigned char> getFrameMessage ();
	virtual std::string getClientName ();	

protected:

	void setFrameMessage (std::vector <unsigned char> vctrCurrentFrame);
	void clearCurrentFrame ();
private:

	std::vector <unsigned char> m_vctrFrameMessage;
	std::string					m_strClientName;

};
}
#endif // !defined(AFX_RADIOPDSMETHOD_H__4035A676_E56C_407D_A383_0D7019CE7E7F__INCLUDED_)
