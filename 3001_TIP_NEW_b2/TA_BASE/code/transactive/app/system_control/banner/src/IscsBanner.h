/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/banner/src/IscsBanner.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// IscsBanner.h : Declaration of the CIscsBanner

#ifndef __ISCSBANNER_H_
#define __ISCSBANNER_H_

#include "app/system_control/banner/src/resource.h"       // main symbols
#include "app/system_control/banner/src/stdafx.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CIscsBanner
class ATL_NO_VTABLE CIscsBanner : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CIscsBanner, &CLSID_IscsBanner>,
	public IDispatchImpl<IIscsBanner, &IID_IIscsBanner, &LIBID_RippleBannerLib>
{
public:
	CIscsBanner()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_ISCSBANNER)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CIscsBanner)
	COM_INTERFACE_ENTRY(IIscsBanner)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()

// IIscsBanner
public:
	STDMETHOD(setAtsButtonState)(BOOL flash);
};

#endif //__ISCSBANNER_H_
