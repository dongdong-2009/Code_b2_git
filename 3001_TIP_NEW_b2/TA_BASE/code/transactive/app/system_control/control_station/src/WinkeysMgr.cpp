/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/control_station/src/WinkeysMgr.cpp $
  * @author:  Jiang Haipeng
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */


// WinkeysMgr.cpp: implementation of the WinkeysMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "app\system_control\control_station\src\WinkeysMgr.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"

extern "C"
{
	#include "app\system_control\config_keys\src\WinLockDll.h"
}

using namespace TA_Base_Core;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
WinkeysMgr * WinkeysMgr::m_self = NULL;

WinkeysMgr::WinkeysMgr()
:m_bDisableCAD(true)
{

}


WinkeysMgr::~WinkeysMgr()
{
	if (m_self != NULL)
	{
		delete m_self ;
		m_self = NULL;	
	}
}

WinkeysMgr * WinkeysMgr::instance()
{
	if (m_self == NULL)
	{
		m_self = new WinkeysMgr();
	}
	return m_self;
}

void WinkeysMgr::lockCAD()
{
	try
	{
		m_bDisableCAD = SessionManager::getInstance().getControlStationEntity().isDisableCAD();
		if (m_bDisableCAD)
		{
			//CtrlAltDel_Enable_Disable(FALSE); //TD19693
			Prn_Enable_Disable(FALSE);    //TD14928
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "disable the ctrl+alt+detele keys" );
		}
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not get the DisableCAD variable in entity data");
	}
}

void WinkeysMgr::unLockCAD()
{
	try
	{
		if (m_bDisableCAD)
		{
			Prn_Enable_Disable(TRUE);   //TD14928
			//CtrlAltDel_Enable_Disable(TRUE); //TD19693
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "enable the ctrl+alt+detele keys" );
		}
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not get the DisableCAD variable in entity data");
	}
}

void WinkeysMgr::onRuntimeParams()
{
	try
	{
		// AfxMessageBox("it's going to disable the CAD in runtime");
		BOOL bDisable = SessionManager::getInstance().getControlStationEntity().isDisableCAD();
		if (bDisable == m_bDisableCAD) return;
		if (bDisable)
		{
			if (!m_bDisableCAD)
			{
				CtrlAltDel_Enable_Disable(FALSE);
				m_bDisableCAD = bDisable;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "disable the ctrl+alt+detele keys" );
			}
		}else{
			if (m_bDisableCAD)
			{
				CtrlAltDel_Enable_Disable(TRUE);
				m_bDisableCAD = bDisable;
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "enable the ctrl+alt+detele keys" );
			}
		}
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DAI Exception", "Could not get the DisableCAD variable in entity data");
	}
}