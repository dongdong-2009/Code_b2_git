/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/ats_page/src/IscsBanner.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// IscsBanner.h : Declaration of the CIscsBanner

#ifndef ISCSBANNER_H
#define ISCSBANNER_H

#include "app/system_control/banner/pages/ats_page/src/stdafx.h"
#include "app/system_control/banner/pages/ats_page/src/BannerInterface.h"

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

#endif // ISCSBANNER_H
