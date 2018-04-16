// IConfigServerObserver.h: interface for the IConfigServerObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONFIGSERVEROBSERVER_H__88974B1B_6216_4623_98A1_7C9B035E860D__INCLUDED_)
#define AFX_ICONFIGSERVEROBSERVER_H__88974B1B_6216_4623_98A1_7C9B035E860D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class IConfigServerObserver  
{
public:
	virtual void notifyServerSettingChange (std::string strIpAddress, std::string strServicePort) = 0;
	virtual void notifyCheckSumOverRide (const BOOL & bStatus) = 0;
};

#endif // !defined(AFX_ICONFIGSERVEROBSERVER_H__88974B1-B_6216_4623_98A1_7C9B035E860D__INCLUDED_)
