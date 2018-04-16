/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/system_control/banner/src/IscsBanner.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:58:46 $
  * Last modified by:  $Author: builder $
  *
  */
// IscsBanner.cpp : Implementation of CIscsBanner
#include "app/system_control/banner/src/stdafx.h"
#include "app/system_control/banner/src/BannerInterface.h"
#include "app/system_control/banner/src/IscsBanner.h"
#include "app/system_control/banner/src/BannerGUI.h"
#include "bus/banner_framework/src/constants.h"
#include "app/system_control/banner/src/Banner.h"

/////////////////////////////////////////////////////////////////////////////
// CIscsBanner


STDMETHODIMP CIscsBanner::setAtsButtonState(BOOL flash)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())
    PostMessage(HWND_BROADCAST, BANNER_CONST::WM_SET_FLASH_STATE, BANNER_CONST::ATS, flash);
    return S_OK;
}
