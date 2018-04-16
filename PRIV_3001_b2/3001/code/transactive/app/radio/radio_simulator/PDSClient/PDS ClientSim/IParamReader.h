// IParamReader.h: interface for the IParamReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IPARAMREADER_H__E2317404_C49A_4846_834D_0CBD1FF8B097__INCLUDED_)
#define AFX_IPARAMREADER_H__E2317404_C49A_4846_834D_0CBD1FF8B097__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

namespace TA_IRS_App
{
	class IParamReader  
	{
	public:
		virtual std::string getValueFromSection(std::string strSection) = 0;
		virtual void setValueToSection(std::string strSection, std::string strValue) = 0;
	};

}
#endif // !defined(AFX_IPARAMREADER_H__E2317404_C49A_4846_834D_0CBD1FF8B097__INCLUDED_)
