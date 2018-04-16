// ParamReader.h: interface for the CParamReader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARAMREADER_H__623497E3_6183_40B3_BF25_9B860F10B0FF__INCLUDED_)
#define AFX_PARAMREADER_H__623497E3_6183_40B3_BF25_9B860F10B0FF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>
#include "IParamReader.h"

namespace TA_IRS_App
{
	class CParamReader  : public IParamReader
	{
	public:
		CParamReader();
		virtual ~CParamReader();
	
		virtual std::string getValueFromSection(std::string strSection);
		virtual void setValueToSection(std::string strSection, std::string strValue);
	};
}
#endif // !defined(AFX_PARAMREADER_H__623497E3_6183_40B3_BF25_9B860F10B0FF__INCLUDED_)
