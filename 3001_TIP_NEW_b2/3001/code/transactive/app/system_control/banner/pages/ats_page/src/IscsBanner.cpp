/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/system_control/banner/pages/ats_page/src/IscsBanner.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// IscsBanner.cpp : Implementation of CIscsBanner
#include "app/system_control/banner/pages/ats_page/src/stdafx.h"
#include "app/system_control/banner/pages/ats_page/src/BannerInterface.h"
#include "app/system_control/banner/pages/ats_page/src/BannerInterface_i.c"
#include "app/system_control/banner/pages/ats_page/src/Resource.h"

#include "app/system_control/banner/pages/ats_page/src/IscsBanner.h"
#include "bus/banner_framework/src/constants.h"

/////////////////////////////////////////////////////////////////////////////
// CIscsBanner


STDMETHODIMP CIscsBanner::setAtsButtonState(BOOL flash)
{
	//AFX_MANAGE_STATE(AfxGetStaticModuleState())
    PostMessage(HWND_BROADCAST, BANNER_CONST::WM_SET_FLASH_STATE, reinterpret_cast<WPARAM>(&BANNER_CONST::ATS), flash);
    return S_OK;
}
