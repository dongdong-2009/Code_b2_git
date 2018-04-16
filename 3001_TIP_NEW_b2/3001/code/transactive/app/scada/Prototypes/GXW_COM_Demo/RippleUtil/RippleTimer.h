/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/scada/Prototypes/GXW_COM_Demo/RippleUtil/RippleTimer.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// RippleTimer.h : Declaration of the CRippleTimer

#ifndef __RIPPLETIMER_H_
#define __RIPPLETIMER_H_

#include "resource.h"       // main symbols
#include "RippleUtilCP.h"

/////////////////////////////////////////////////////////////////////////////
// CRippleTimer
class ATL_NO_VTABLE CRippleTimer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CRippleTimer, &CLSID_RippleTimer>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CRippleTimer>,
	public IDispatchImpl<IRippleTimer, &IID_IRippleTimer, &LIBID_RIPPLEUTILLib>,
	public CProxy_IRippleTimerEvents< CRippleTimer >
{
public:
	CRippleTimer()
	{
	}

	HRESULT FinalConstruct( );
	void FinalRelease();


DECLARE_REGISTRY_RESOURCEID(IDR_RIPPLETIMER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CRippleTimer)
	COM_INTERFACE_ENTRY(IRippleTimer)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()
BEGIN_CONNECTION_POINT_MAP(CRippleTimer)
CONNECTION_POINT_ENTRY(DIID__IRippleTimerEvents)
END_CONNECTION_POINT_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IRippleTimer
public:
	STDMETHOD(Stop)();
	STDMETHOD(Start)();
	STDMETHOD(SetPeriod)(long period);

	bool m_bStop;	
	UINT m_period;

private:
	bool StartTimerThread();
	void StopTimerThread();
	HANDLE m_hThread;

};


	VOID CALLBACK TimerProc(
			  HWND hwnd,         // handle to window
				UINT uMsg,         // WM_TIMER message
				UINT_PTR idEvent,  // timer identifier
				DWORD dwTime);       // current system time);

#endif //__RIPPLETIMER_H_
