// IFrameRandGenerator.h: interface for the IFrameRandGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IFRAMERANDGENERATOR_H__901E70DA_03C6_4827_B653_1AAFE2785788__INCLUDED_)
#define AFX_IFRAMERANDGENERATOR_H__901E70DA_03C6_4827_B653_1AAFE2785788__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class IFrameRandGenerator 
{
public:
	virtual void notifyGeneratedFrame (std::vector <std::string> vctrRandFrame) = 0; 

};

#endif // !defined(AFX_IFRAMERANDGENERATOR_H__901E70DA_03C6_4827_B653_1AAFE2785788__INCLUDED_)
