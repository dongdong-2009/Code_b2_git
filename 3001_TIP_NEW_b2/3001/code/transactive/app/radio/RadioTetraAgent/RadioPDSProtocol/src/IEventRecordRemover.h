// IPDSClientDisconnectionObserver.h: interface for the IPDSClientDisconnectionObserver class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IEVENTRECORDREMOVER_H__BC6E3572_F622_4844_913E_31D508569C60__INCLUDED_)
#define AFX_IEVENTRECORDREMOVER_H__BC6E3572_F622_4844_913E_31D508569C60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class IEventRecordRemover  
{
	public:
		virtual void removeAllRecordForTrain (long lTrainID) = 0;
		virtual void removeResendRecordForTrain (const long & lTrainID) = 0;
};

#endif // !defined(AFX_IPDSCLIENTDISCONNECTIONOBSERVER_H__BC6E3572_F622_4844_913E_31D508569C60__INCLUDED_)





















